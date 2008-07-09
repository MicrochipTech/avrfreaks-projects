; EE.asm  parallel EEPROM reader and writer for Atmel AVR Mega161 (easy to adapt to Mega162)
; ver 1.00   June 23, 2008  alan_probandt@yahoo.com     Portland Oregon USA
; Use unkerned font like Terminal or Lucida Console to view source formatted correctly.  Tab size = 8
;
; assemble with:  c:\AVR\avrasm2  EE.asm -fI -l EE.lst -o EE.hex
;
; system clock crystal = 3684000 Hz   CLSEL low fuses 3-0: 0100  (page 27 Mega161 PDF)
; schematic GIF file: \AVR\src\mega161\EEPROM_M161_schematic.gif
;
; **************************************************************************
;  Read and Write data to a 128Kx8 Parallel EEPROM such as Atmel AT29EE010 or Winbond W29EE011.
;  This programmer connects to the PC serial port 9600 baud 8/N/1.
;  Data is sent to be written into the EEPROM in blocks of 128 bytes.
; **************************************************************************
;
;
;            Mega161 16K/1K/512 EEPROM    external memory 0x0460 ~ 0xffff  (page 84 Mega161 PDF)
;                     _______
; EEPROM Addr15   PB0 |1  40| Vcc                       _______
; EEPROM Addr16   PB1 |2  39| PA0/AD0        (Vpp) n.c  |1   32| VCC
;  CTS  to  PC    PB2 |3  38| PA1/AD1              A16  |2   31| WE\
;  RTS from PC    PB3 |4  37| PA2/AD2              A15  |3   30| n.c. (RP\)
;                 PB4 |5  36| PA3/AD3              A12  |4   29| A14
;            MOSI/PB5 |6  35| PA4/AD4               A7  |5   28| A13
;            MISO/PB6 |7  34| PA5/AD5               A6  |6   27| A8
;             SCK/PB7 |8  33| PA6/AD6               A5  |7   26| A9
;              \RESET |9  32| PA7/AD7               A4  |8   25| A10
;  RS232 from PC RXD0 |10 31| PE0/ICP/INT2          A3  |9   24| OE\
;  RS232  to  PC TXD0 |11 30| PE1/ALE               A2  |10  23| A11
;           INT0 /PD2 |12 29| PE2/OC1B              A1  |11  22| CE\
;           INT1/ PD3 |13 28| PC7/A15               A0  |12  21| D7
;           TOSC1/PD4 |14 27| PC6/A14               D0  |13  20| D6
;      OC1A/TOSC2/PD5 |15 26| PC5/A13               D1  |14  19| D5
;             \WR PD6 |16 25| PC4/A12               D2  |15  18| D4
;             \RD PD7 |17 24| PC3/A11               gnd |16  17| D3
;               XTAL2 |18 23| PC2/A10                   --------
;               XTAL1 |19 22| PC1/A9                  128Kx8 EEPROM
;                 GND |20 21| PC0/A8
;     	               ------

.nolist
.include "m161def.inc"
.list

.equ	clock	 	= 3684000
.equ	BaudRate 	= 9600
.equ	PCbaud   	= ( (clock/BaudRate) / 16) - 1; error 0.0%
.equ	QUEUESIZE	= 128
.equ	QUEUEWRAPMASK	= QUEUESIZE-1
.equ	AddressLinePort	= PortB
.equ	AddressLinePin	= PinB
.equ	AddressLineDDR	= DDRB
.equ	HandshakePort	= PortB
.equ	HandshakeDDR	= DDRB


.equ	Addr15	 	= 0 ; pin 1  selects upper or lower block of 64K of 128K EEPROM
.equ	Addr16	 	= 1 ; pin 2  selects upper or lower 32K section of Addr16 block  256 pages per Addr15 block
.equ	CTSpin		= 2 ; pin 3
.equ	RTSpin		= 3 ; pin 4


;****************************************************************************
; register usage
.def	SREGtemp	= r1
.def	ByteCounter	= r2
.def	InQueueHead	= r3
.def	InQueueTail	= r4
.def	SectorCounter	= r5
.def	TestValue	= r6

.def	zeroreg		= r15

; upper registers
.def	temp		= r16
.def	irqtemp		= r17
.def	temp2		= r18
.def	irqtemp2	= r19
.def	Flags		= r20
.equ	UART_TimeOut_Occurred	= 4 ; 35 mS passed since the last byte received.  ISR sets this flag for the main program.


; ZH,ZL=r31,30   YH,YL=r29,28   XH,XL=r27,r26


;***** Static RAM data *****************
.dseg  ; 1K SRAM in Mega161
InQueue:	.byte	QUEUESIZE
SectorBuffer:	.byte	128
;***********************************


.cseg
; interrupt and power-on vectors
.org 0x0000
	jmp reset       ;0x0000 power-on
; interrupt vector table for Mega161
	reti nop	;0x0002 External Interrupt 0
	reti nop	;0x0004 External Interrupt 1
	reti nop	;0x0006 External Interrupt 2
	reti nop	;0x0008 Timer/Counter2 Compare Match
	reti nop	;0x000a Timer/Counter2 Overflow
	reti nop	;0x000c Timer/Counter1 Capture Event
	reti nop	;0x000e Timer/Counter1 Compare Match A
	reti nop	;0x0010 Timer/Counter1 Compare Match B
	reti nop	;0x0012 Timer/Counter1 Overflow
	reti nop	;0x0014 Timer/Counter0 Compare Match
	jmp T0_Overflow ;0x0016 Timer/Counter0 Overflow
	reti nop	;0x0018 Serial Transfer Complete
	jmp RxC_IRQ	;0x001a UART0, Rx Complete
	reti nop 	;0x001c UART1, Rx Complete
	reti nop	;0x001e UART0 Data Register Empty
	reti nop	;0x0020 UART1 Data Register Empty
	reti nop	;0x0022 UART0, Tx Complete
	reti nop	;0x0024 UART1, Tx Complete
	reti nop	;0x0026 EEPROM Ready
	reti nop	;0x0028 Analog Comparator


reset:
; initialize stack
	ldi	temp, high (RAMEND)
	out	SPH, temp
	ldi	temp, low (RAMEND)
	out	SPL, temp

; initialize USART0
	ldi	temp, high (PCbaud)
	out 	UBRRHI, temp
	ldi	temp, low (PCbaud)
	out 	UBRR0, temp
	ldi	temp, (1<<TXEN0) | (1<<RXEN0) | (1<<RXCIE0)
	out 	UCSR0B, temp

; initialize Input/Output Ports
	sbi	AddressLineDDR, Addr16
	sbi	AddressLineDDR, Addr15
	sbi	HandshakeDDR, CTSpin
	cbi	HandshakeDDR, RTSpin

        
; enable global interrupts
	sei

; initialize application
	clr	zeroreg
	clr	InQueueHead	; CLR OK for low registers
	clr	InQueueTail
	clr	Flags
	in	temp, MCUCR
	sbr	temp, (1<<SRE)	; enable external static RAM access
	out	MCUCR, temp	; external memory 0x0460 ~ 0xffff  (page 84 Mega161 PDF)
; deassert CTS by setting CTSpin [pin 3].  This stops the PC from sending any more bytes out to the serial port

; enable the PC to start sending data in order to receive letter commands
 	cbi	HandshakePort, CTSpin

;***********************************************************
MainLoop:
;	rcall	InitMsgDisplay

ML_1:	cpse	InQueueHead, InQueueTail  ; CPSE - OK for both registers to be lower
	rjmp	GetCommand
	rjmp	ML_1

GetCommand:
	ldi	ZH, high (InQueue)
	ldi	ZL, low  (InQueue)
	add	ZL, InQueueHead  ; ADD - OK for both registers to be lower
	ld	temp, Z
	inc	InQueueHead	; INC - OK for register to be lower
	mov	temp2, InQueueHead
	andi	temp2, QUEUEWRAPMASK ; ; ANDI - register must be higher
	mov	InQueueHead, temp2
	andi	temp, 0b01011111 ; convert to command upper case ASCII

	cpi	temp, 'E'
	brne	GC_a
	rcall	EraseEEPROM
	rjmp	GC_exit

GC_a:	cpi	temp, 'R'
	brne	GC_b
	rcall	ReadTest
	rjmp	GC_exit

GC_b:	cpi	temp, 'W'
	brne	GC_c
	rcall	WriteTest
	rjmp	GC_exit

GC_c:	cpi	temp, 'F'  
	brne	GC_d       
	rcall	WriteFile
	rjmp	GC_exit    

GC_d:	cpi	temp, 'A'  
	brne	GC_e       
	rcall	ReadAllEEPROM
	rjmp	GC_exit 
	
GC_e:
GC_exit:
	rjmp	MainLoop


;~~~~~~~~~~~~~~~~~~~~ subroutines ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
;****************************************************************************************
; WriteTest  - load first four sectors with values 0x00 to 0x7f sequencially
;
;******************************************************************************************
WriteTest:
	clr	SectorCounter
	clr	XL ; use 32K upper block of Mega161 Static RAM address space 0x8000-0xffff
	ldi	XH, 0x80 ; point to the beginning of the EEPROM memory
	cbi	AddressLinePort, Addr15
	cbi	AddressLinePort, Addr16

WT_WriteSectorToEE:
; defeat_write_prevention with three byte sequence
	ldi	ZL, 0x55	; byte 1 addr = 0 0101 0101 0101 0101
	ldi	ZH, 0x55
	ldi	temp, 0xaa
	st	Z, temp
	ldi	ZL, 0xaa	; defeat_write_prevention_sequence byte: 2 of 3
	ldi	ZH, 0x2a	; address #2 = 0 0010 1010 1010 1010
	ldi	temp, 0x55
	st	Z, temp
	ldi	ZL, 0x55	; defeat_write_prevention_sequence byte: 3 of 3
	ldi	ZH, 0x55        ;  addr #3 = 0 0101 0101 0101 0101
	ldi	temp, 0xa0
	st	Z, temp
; send 128 bytes of the sector
	clr	TestValue
WT_WriteBytetoEE:
	st	X, TestValue
	adiw	XH:XL,1
	inc	TestValue
	mov	temp, TestValue
	cpi	temp, 128
	breq	WT_WriteSector
	rjmp	WT_WriteBytetoEE

WT_WriteSector:
; wait 10mS+ for sector write
	ldi	temp, (1<<TOV0)
	out	TIFR, temp ; clear overflow flag for timer0, if set
	ldi	temp, 90
	out	TCNT0, temp ; prescale x256  144 ticks=10mS
	ldi	temp, (1<<CS02) | (0<<CS01) | (1<<CS00) ; each tick 69.4uS
	out	TCCR0, temp  ; time out after 10mS
WT_delay10mS:
	in	temp, TIFR
	andi	temp, (1<<TOV0)
	breq	WT_delay10mS

	inc	SectorCounter
	mov	temp, SectorCounter
	cpi	temp, 8
	brlo	WT_WriteSectortoEE

WT_exit:
	ret


;*******************************************************************************************
; ReadTest - Start with address 0x00000 and read until 0x001ff of the Parallel EEPROM.
; After each byte is read, send it out through the UART.
;
;*******************************************************************************************
ReadTest:  ; read first four sectors (512 bytes total)
	clr	SectorCounter
	ldi	XL, 0	; 0x8000 = offset of the EEPROM 32K window in the AVR's external sRAM address map.
	ldi	XH, 0x80
	cbi	AddressLinePort, Addr15 ; Addr 15,16 = 0 > select the first 32K block of the EEPROM
	cbi	AddressLinePort, Addr16
RT_0:	clr	ByteCounter
RT_a:	ld	temp, X+  ; read byte from EEPROM
RT_b:	sbis	UCSR0A, UDRE0
	rjmp	RT_b
	out	UDR0, temp   ; send byte out through UART
	inc	ByteCounter
	mov	temp2, ByteCounter  ; CPI instruction needs high register
	cpi	temp2, 128
	brlo	RT_a
	inc	SectorCounter
	mov	temp2, SectorCounter
	cpi	temp2, 8
	brlo	RT_0
	ret


;-----------------------------------------------
; WEstatusMsg  - select a message string from a set of multiple output strings using an address table.
; send:  temp with the number of the message string to be output
; return: nothing
; uses: ZH:ZL, temp, UART peripheral
WEstatusMsg:
	ldi	ZL, low  (WriteTextAdrTable*2)
	ldi	ZH, high (WriteTextAdrTable*2)
	lsl	temp ; multiply by 2
	add	ZL,temp
	adc	ZH, zeroreg
	lpm	temp, Z+
	lpm	temp2, Z
	mov	ZH, temp
	mov	ZL, temp2
WEsm_0:	lpm	temp, Z+
	tst	temp
	breq	WEsm_exit
	out	UDR0, temp
WEsm_1:	sbis	UCSR0A, UDRE0
	rjmp	WEsm_1
	rjmp	WEsm_0
WEsm_exit:
	ret

WriteTextAdrTable:
	.db	high (WriteText0*2),  low  (WriteText0*2)
	.db	high (WriteText1*2),  low  (WriteText1*2)

WriteText0: .db 0x0d,0x0a, "UART Receive Timeout", 0x0d,0x0a,0,0
WriteText1: .db 0x0d,0x0a, "Filled 128Kx8 space of the EEPROM", 0x0d,0x0a,0



;******************************************************
;  EraseEEPROM
;******************************************************
EraseEEPROM:
	cbi	AddressLinePort, Addr15
	cbi	AddressLinePort, Addr16
	ldi	ZL, 0x55	; erase sequence byte: 1 of 6  5555 AA
	ldi	ZH, 0x55
	ldi	temp, 0xaa
	st	Z, temp

	ldi	ZL, 0xaa	; erase sequence byte: 2 of 6  2AAA 55
	ldi	ZH, 0x2a
	ldi	temp, 0x55
	st	Z, temp

	ldi	ZL, 0x55	; erase sequence byte: 3 of 6  5555 80
	ldi	ZH, 0x55
	ldi	temp, 0x80
	st	Z, temp

	ldi	temp, 0xaa      ; erase sequence byte: 4 of 6  5555 aa
	st	Z, temp

	ldi	ZL, 0xaa	; erase sequence byte: 5 of 6  2AAA 55
	ldi	ZH, 0x2a
	ldi	temp, 0x55
	st	Z, temp

	ldi	ZL, 0x55	; erase sequence byte: 6 of 6  5555 10
	ldi	ZH, 0x55
	ldi	temp, 0x10
	st	Z, temp

	ser	temp		; clear timer overflow flags
	out	TIFR, temp
	ldi	temp,  (1<<CS22) | (1<<CS21) | (1<<CS20)
	out	TCCR2, temp
	ldi	temp, 200
	out	TCNT2, temp
eloop1:	in	temp, TIFR
	andi	temp, (1 << TOV2)
	breq	eloop1
	ret


;;*******************************************************************************************
;; ReadAllEEPROM - Start with address 0x00000 and read until 0x1ffff of the Parallel EEPROM.
;; Because the AVR reads its internal SRAM and registers in the same address space as its 
;; external static RAM, it is necessary to remap the EEPROM into banks of 32K each. 
;; The 32K of address space that the AVR uses to work with the selected bank is at 0x8000-0xFFFF
;; in the AVR external static RAM address range.  Swapping between banks on the EEPROM itself
;; is done by controlling the EEPROM's A15 and A16 pins with two AVR I/O lines not on the ALE bus.
;;*******************************************************************************************
ReadAllEEPROM:  ; read four blocks of 32K each
	cbi	AddressLinePort, Addr15
	cbi	AddressLinePort, Addr16
	rcall	SendBlock
	sbi	AddressLinePort, Addr15
	rcall	SendBlock
	cbi	AddressLinePort, Addr15
	sbi	AddressLinePort, Addr16
	rcall	SendBlock
	sbi	AddressLinePort, Addr15
	rcall	SendBlock
	ret


;;************************************************************************************
;; WriteFile
;; The AT29C010 parallel EEPROM is written in 128-byte sectors. Pulling the \WE line low
;; latches the data and address into a buffer.  Further writes to the same sector must be done
;; within 150uS.  After one write, a pause of 150uS starts the programming cycle. The cycle
;; takes 10mS max. During the cycle, data read from the last address written wil have bit 7 complemented.
;; When bit 7 of the last address written is the same as was written, then the cycle is finished.
;;
;;; To write sector, first receive a block of 128 (or less) bytes from the PC.
;; Then signal the PC to stop sending data until the page is written to EEPROM.
;; Determine the state of the Addr16 and Addr15 lines in order to place the page into the EEPROM.
;;
;; This routine ends when there is a 35mS time out of timer0.
;; send:
;; return:
;; uses: XH:XL (pointer into the AVR ext SRAM block 0x8000:0xFFFF), 
;      ZH:ZL (SectorBuffer)
;;***********************************************************************************
WriteFile:
	cbi	AddressLinePort, Addr15
	cbi	AddressLinePort, Addr16 
WF_WriteBlock:	
	clr	XL ; use 32K upper block of Mega161 Static RAM address space 0x8000-0xffff
	ldi	XH, 0x80 ; point to the beginning of the EEPROM memory
  	clr	SectorCounter	; increment after each sector prg cycle. Adjust Addr16 and Addr15 pins this value rolls over (0xff>>0x00).
; The EEPROM start address is offset by 0x8000 because A15 and A16 are not controlled by the AVR's external sRAM access.
; When a counter (Sector_WriteCount) overflows (0xff>>0x00), the A16:A15 pin pair should be incremented.

WF_WriteSectortoEE:   
; Erase sector buffer in case it is only partially filled by data sent by the PC
	clr	temp
	ser	temp2
	ldi	ZL, low  (SectorBuffer) ; point to where the data from the PC will be stored in sRAM
	ldi	ZH, high (SectorBuffer)
WF_clrSecBuff:
	st	Z+,temp2
	inc	temp
	cpi	temp, 128
	brlo	WF_clrSecBuff
		
	cbi	HandshakePort, CTSpin ; signal the PC to send data by asserting ClearToSend
; First load 128 bytes from PC. Put each byte into the SectorBuffer as each byte arrives into the UART input buffer.
; If 35mS passes without a byte arriving into the UART, set UART_TimeOut flag and return to Main.
	ldi	ZL, low  (SectorBuffer) ; point to where the data from the PC will be stored in sRAM
	ldi	ZH, high (SectorBuffer)
	clr	ByteCounter
; wait until byte arrives from PC into UART, then activate timer0 with new count
WF_PollQ:
	cpse	InQueueHead, InQueueTail ; bytes arrive from PC about one per millisecond
	rjmp	WF_NewByteFromPC ; new byte arrived from PC
; No new byte has arrived in this cycle of the polling loop. So check for a timer0 overflow
; before doing the next cycle of the polling loop.  If the UART_TimeOut flag is set, then
; an overflow on Timer0 happened and the interrupt routine set the flag. 
; Clear the flag and exit this subroutine back to Main code.
	sbrc	Flags, UART_TimeOut_Occurred  ; poll in loop until a new byte arrives
	rjmp	WF_UARTtimeOut
	rjmp	WF_PollQ	; TimeOut flag is clear so keep checking for bytes arriving from the PC

WF_NewByteFromPC:
	ldi	temp, (1<<TOIE0)  ; enable interrupt on Timer 0 overflow
	out	TIMSK, temp
	clr	temp	; Timer0 acts a re-triggerable one-shot monostable multivibrator
	out	TCNT0, temp
	ldi	temp, (0<<CS02) | (1<<CS01) | (1<<CS00) ; prescale 64 overflow period with 3.68MHz clock = 35.5mS
	out	TCCR0, temp  ; time out after period of inactivity  ; 1mS per byte transfer rate from PC
	cbr	Flags, (1<<UART_TimeOut_Occurred) ; previous timeout indicator
	
	ldi	YH, high (InQueue) ; determine where new byte is located in the Input queue
	ldi	YL, low  (InQueue)
	add	YL, InQueueHead  ; ADD - OK for both registers to be lower
	inc	InQueueHead	; INC - OK for register to be lower
	mov	temp2, InQueueHead ; adjust input queue pointer
	andi	temp2, QUEUEWRAPMASK ; ; ANDI instruction - register must be in higher reg bank
	mov	InQueueHead, temp2

	ld	temp, Y  ; get data byte from queue
	st	Z+, temp  ; put data byte into page buffer
	inc	ByteCounter ; check if page buffer if full
	mov	temp2, ByteCounter
	cpi	temp2, 128 ; keep getting bytes from PC if page buffer is not full
	brne	WF_PollQ

; received 128 bytes from the PC.  Now write them to the EEPROM.
WF_writeSector:
	sbi	HandshakePort, CTSpin ; signal the PC to stop sending data by deactivating ClearToSend
; send the three Software Data Protection bytes
	ldi	ZL, 0x55	; erase sequence byte: 1 of 6  5555 AA
	ldi	ZH, 0x55
	ldi	temp, 0xaa
	st	Z, temp
	ldi	ZL, 0xaa	; erase sequence byte: 2 of 6  2AAA 55
	ldi	ZH, 0x2a
	ldi	temp, 0x55
	st	Z, temp
	ldi	ZL, 0x55	; erase sequence byte: 3 of 6  5555 80
	ldi	ZH, 0x55
	ldi	temp, 0xa0
	st	Z, temp

; send 128 bytes from the SectorBuffer to the EEPROM
	ldi	ZL, low  (SectorBuffer) ; point to where the data from the PC is stored in sRAM
	ldi	ZH, high (SectorBuffer)
	ldi	temp2, 0x80
	mov	ByteCounter, temp2 
WF_MoveByteToEE:
	ld	temp, Z+ ; ZH:ZL=pointer into the page buffer of the current byte being written.
	st	X+, temp ; XH:XL=EEPROM
	dec	ByteCounter
	breq	WF_BeginPrgCycle
	rjmp	WF_MoveByteToEE

; finished sending 128 bytes to EEPROM, now wait until it has finished writing this data.
WF_BeginPrgCycle:
; wait 10mS+ for sector write
	in	temp, TIMSK
	andi	temp, ~(1<<TOIE0)  ; turn off timer0 interrupt
	out	TIMSK, temp
	ldi	temp, (1<<TOV0)
	out	TIFR, temp ; clear overflow flag for timer0, if set
	ldi	temp, 90
	out	TCNT0, temp ; prescale/256  144 ticks=10mS
	ldi	temp, (1<<CS02) | (0<<CS01) | (0<<CS00) ; prescale/256:each tick 69.4uS
	out	TCCR0, temp  ; time out after 10mS
WF_delay10mS:
	in	temp, TIFR
	andi	temp, (1<<TOV0)
	breq	WF_delay10mS
	ldi	temp, (0<<CS02) | (0<<CS01) | (0<<CS00) ; turn off timer
	out	TCCR0, temp  ; time out after 10mS
; Finished burning this sector into EEPROM.  Check if AVR needs to adjust to the next 32K bank of the EEPROM.
; If not, then get the next sector from PC and write it to EEPROM.  CTS is deasserted now.
; XH:XL is pointed at the beginning of the next sector in the EEPROM to be written
; Check if partial sector was just written. If yes (UART_TimeOut flag set), then exit.
	sbrc	Flags, UART_TimeOut_Occurred
	rjmp	WF_PartialSecExit
	inc	SectorCounter
	breq	WF_NewEEbank ; sector counter rolled over (finished writing 32K to EEPROM)
	rjmp	WF_WriteSectortoEE ; get the next sector from PC and write it to EEPROM.

WF_NewEEbank:    ; Adjust Addr16 & Addr15 to next higher 32K block of the EEPROM.
	in	temp, AddressLinePort   ; read the Port B output latches, not the data on the pins
	mov	temp2, temp
	andi	temp, (1<<Addr16) | (1<<Addr15) ; 0b00000011=mask
	inc	temp
	cpi	temp, 4
	breq	WF_FilledAll
	andi	temp2, ~ (   (1<<Addr16) | (1<<Addr15)  )
	or	temp2, temp
	out	AddressLinePort, temp2
	rjmp	WF_WriteBlock

; 35mS has passed since the last received byte from the PC.
WF_UARTtimeOut:
	ldi	temp, 0
	rcall	WEstatusMsg
; instead of exiting here, write the partially filled sector buffer to EEPROM
	rjmp	WF_writeSector
	
WF_FilledAll:
	ldi	temp, 1
	rcall	WEstatusMsg
	rjmp	WF_exit
	
WF_PartialSecExit:
	cbr	Flags, (1<<UART_TimeOut_Occurred)
	ldi	temp, (0<<CS02) | (0<<CS01) | (0<<CS00) ; turn off timer0
	out	TCCR0, temp  ; time out after 10mS
	
WF_exit:
	ret


;***********************************************************************************************************
; SendBlock  ; send a block of 32K bytes from the parallel EEPROM to RS232 out.
; Fill the output buffer, then wait until all bytes have been transmitted. Repeat until 32K block finished
;
; send:
; return:
; uses: YH, YL, temp
;************************************************************************************************************
SendBlock:
	ldi	YL, 0
	ldi	YH, 0x80
SBa:	ld	temp, Y+
SBb:	sbis	UCSR0A, UDRE0
	rjmp	SBb
	out	UDR0, temp
  	tst	YH ; rolled over yet
	brne	SBa
	ret

;>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
;   Interrupt Routines
;>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

;;*************************************************************************************
;; Timer0 Overflow  - very quick, does nothing but set flag for main code.
;;  "Egg Timer" function for 35 mS.  Main code turns on timer0
;; with counter reg value 00 and reloads 00 value to prevent overflow.
;; Main code sets a flag to let this ISR know what timed out.
;; If overflow occurs, the flag that was set by the main code allows this ISR
;; to set correct flag for the main program indicating that an 35mS timeout happened.
;;************************************************************************************
T0_Overflow:
	in	SREGtemp, SREG
	sbr	Flags, (1<<UART_TimeOut_Occurred)	
	out	SREG, SREGtemp
	reti

;*************************************************************************************************
;   RxC_IRQ:  reads bytes from RS232 input. Puts RxD bytes into InQueueTail pointer location
; and then increments InQueueTail pointer.
;
; send: InQueueTail, InQueue
; return:
; used: XH,XL (nondestructive)
;*************************************************************************************************
RxC_IRQ:
	in	SREGtemp, SREG
	push	XH
	push	XL
	ldi	XH, high (InQueue)
	ldi	XL, low (InQueue)
	add	XL, InQueueTail
	brcc	RxCa
	inc	XH
RxCa:	in	irqtemp, UDR0
	st	X+, irqtemp
	inc	InQueueTail
	mov	irqtemp, InQueueTail
	andi	irqtemp, QUEUEWRAPMASK
	mov	InQueueTail, irqtemp

	pop	XL
	pop	XH
	out	SREG, SREGtemp
	reti


;.db	"****  EE.asm  v1.00 "
.db	"****  EEa.ms  1v0. 0"


; scrap and unused code

;;*************************************************************************
;; InitMsgDisplay  -  First display of text sent from AVR to PC terminal
;;
;;*************************************************************************
;InitMsgDisplay:
;	ldi	ZL, low  (IntroText*2)
;	ldi	ZH, high (IntroText*2)
;IMD_0:	lpm	temp, Z+
;	tst	temp
;	breq	IMD_exit
;	out	UDR0, temp
;IMD_1:	sbis	UCSR0A, UDRE0
;	rjmp	IMD_1
;	rjmp	IMD_0
;IMD_exit:
;	ret
;
;IntroText: .db  0x0d, 0x0a,"Alan's 29EE010 Parallel", 0x0d, 0x0a
;	.db "EEPROM programmer v0.90", 0x0d, 0x0a
;	.db "E = erase EEPROM", 0x0d, 0x0a
;	.db "F = write PC file to EEPROM", 0x0d, 0x0a 
;	.db "A = read all EEPROM sectors", 0x0d, 0x0a
;	.db "R = read test (8 sectors)", 0x0d, 0x0a
;	.db "W = write test (8 sectors)", 0x0d, 0x0a
;	.db " >>> ", 0x00

