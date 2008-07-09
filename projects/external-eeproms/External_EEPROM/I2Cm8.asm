; I2Cm8.asm  == Programmer for I2C serial EEPROM; using the TWI function on AVR Mega family.
; version 1.0  July 9, 2008	alan_probandt@yahoo.com       Portland, Oregon USA
;
; Target CPU >> Atmel AVR Mega8
; Fuses: high:	11010111  EEPROM preserved through chip erase  [SP12 -wH10111]
;	 low:	11100100  clk/8=off, no clk out, 8MHz Int RC clock, full-swing, slow-rise power  [SP12 -wF11101111]
;
; Assemble with:  avrasm2  I2C_block.asm -fI -l I2C_block.lst -o I2C_block.hex
; RealTerm cmd line: C:\Realterm\realterm.exe baud=38400 port=1 flow=2 capfile=g:\AVR\src\m48\I2C\capture.txt RTS=1 Display=0 rows=34
;
; Use unkerned font like Terminal or Lucida Console to view source formatted correctly.  Tab size = 8
;
; Connect to PC RS-232 serial port.
; Program starts by opening AVR Mega8 USART for 38.4K baud 8/N/1.
; Then program prompts for input 'R', 'W', 'D', 'E', or 'P' on PC terminal.
;
;  >> 'R' entered (Read Page):
; 	< Prompt appears on PC terminal for a 24AA515 EEPROM page [range= 0x0000-0x03ff ]
;	> Enter four hex characters on PC terminal.
;	< Eight rows of eight chars appear below the address.  These are the bytes (in hex) found at that page.
;	< Program returns to original prompt for 'R', 'W', 'D', 'E', or 'P'.
;  >> 'W' entered (Write Page):
; 	< Prompt appears on PC terminal for a 24AA515 EEPROM page [range= 0x0000-0x03ff ]
;	> Enter four hex characters on PC terminal.
;	< The selected page is filled with 64 bytes. The first four bytes are the ASCII of the first address of the page.
;	  Next is the colon char.  Then the values 0x05 to 0x3f are placed in the remaining chars of the page.
;	< program returns to original prompt for 'R', 'W', 'D', 'E', or 'P'.
;  >> 'D' entered (Dump Multiple Pages):
; 	< Prompt appears on PC terminal for page number. [range= 0x0000-0x03ff ]
;	> Enter four hex characters on PC terminal. This is the starting page of the data sent out.
; 	< Prompt appears on PC terminal for hex number. [range= 0x0000-0x0400 ]
;	> Enter four hex characters on PC terminal.  This is the number of pages to send.
;       < All bytes in the range of pages in the EEPROM dumped to RS232 as quickly as possible.
;  >> 'E' entered (Erase Multiple Pages):
; 	< Prompt appears on PC terminal for page number. [range= 0x0000-0x03ff ]
;	> Enter four hex characters on PC terminal. This is the starting page of the data sent out.
; 	< Prompt appears on PC terminal for page number. [range= 0x0000-0x0400 ]
;	> Enter four hex characters on PC terminal.  This is the number of pages to send.
;       < All bytes in the selected pages are written to 0xFF as quickly as possible.
;  >> 'P' entered (Program Multiple Pages)
;	< Prompt appears on PC terminal for page number. [range= 0x0000-0x03ff ]
;	> Enter four hex characters on PC terminal. This is the starting page of where the data will be written.
;	>> On the RealTerm.exe terminal program, select 'SendFile' and the file of data to be written to the EEPROM.
;       >> After completing writing period (@20 seconds for entire EEPROM, maxed progress bar on RT), press <RESET> for prompt
;       >> Use 'R' to verify that the data was written correctly to selected pages.
;
; >>>> Serial EEPROM types
;   This program as written works specifically for the MicroChip Corporation's 24AA515 serial EEPROM.  It also works
; unchanged (as yet untested) with 24LC515 and 24FC515.  These are a 64K byte 8-pin devices.  The address space is in
; two blocks of 32K each.  A Bank-Select bit in the I2C control byte determines the upper or lower block.  The lines
; in this program that deal with this bit are commented with the string "%%%~~~".  With the 24xx515, pin 3 of the IC
; must be tied high to Vcc. There are only four I2C addresses, determined by A0 and A1.  Vcc can be between +2.5v to +5v.
;
;   The more common high-density serial EEPROM 24LC512 can use this program after adjustment of the Bank-Select code
; lines.  24LC512 EEPROMs use the full 64K 16-bit address range in the I2C address bytes sent after the control byte.
; There is no Bank-Select bit in the control byte.  These ICs can also use pin 3 as a standard A2 address selector,
; giving eight I2C chip addresses.  Lower density EEPROMs such as 24LC32, 24LC64, 24LC128, and 24LC256 also use this format.
;
; Data Protocol - Standard I2C device protocol for writing data is: Start Condition, Control byte -ACK-, high address -ACK-,
; low address -ACK-, write data byte(s) -ACK-, and ending with a Stop Condition.  For reading data, the write protocol
; is used for latching the data address into the device, then a RepeatedStart followed with a Control byte with the
; read/write bit set.  One or more data bytes are then read with the AVR generating the Acknowledge bit. The master
; generates a NACK after the last read byte accepted. The lower density EEPROMs like 24LC32, 24LC64, 24LC128, and 24LC256
; also use this protocol.   24LC32 and 24LC64 use 32-byte page sizes; 24LC128, 24LC512, and 24LC515 use 64-byte pages.

;	This application's pinout - Atmel AVR Mega48                         MAX232
;   			  _______			 ;                  _______
;    reset/deBugWire  PC6 |1  28| PC5 SCL		 ;           + Cap1 |1  16| Vcc  +5V
;		 RxD  PD0 |2  27| PC4 SDA                ;           + Cap2 |2  15| Gnd
;		 TxD  PD1 |3  26| PC3 (ADC3/PCINT11)	 ;           - Cap1 |3  14| RS232 out
;		 RTS  PD2 |4  25| PC2 (ADC2/PCINT10)	 ;           + Cap3 |4  13| RS232 in
;		 CTS  PD3 |5  24| PC1 (ADC1/PCINT9)      ;           - Cap3 |5  12| TTL out
;                     PD4 |6  23| PC0 (ADC0/PCINT8)      ;           - Cap4 |6  11| TTL in
;		 +5V  VCC |7  22| GND                    ;    RS232 TxD out |7  10| TTL TxD out (from AVR)
;		      GND |8  21| AREF  +5V              ;    RS232 RxD  in |8   9| TTL RxD in  (from AVR)
;(pcint6/XTAL1/TOSC1) PB6 |9  20| AVCC  +5V              ;                  -------
;   (pcint21/OC0B/T1) PD5 |11 18| PB4 (MISO/PCINT4)      ;           A0     |1  16| Vcc  +5V
; (pcint22/OC0A/AIN0) PD6 |12 17| PB3 (MOSI/OC2A/PCINT3) ;           A1     |2  15| WP (gnd)
;      (pcint23/AIN1) PD7 |13 16| PB2 (SS/OC1B/PCINT2)   ;           A2 (+5)|3  14| SCL
;  (pcint0/CLKO/ICP1) PB0 |14 15| PB1 (OC1A/PCINT1)      ;           gnd    |4  13| SDA
;			  -------                                           24AA515

.nolist
.include "m8def.inc"
.list

.equ	SRAMstart	= 0x060  ; 0x060 on Mega8 and 0x100 on Mega48
.equ	clock		= 8000000; AVR system clock speed in Hertz. Internal RC generated.
.equ	baud38K		= (clock/(38400*16))-1 ;14  AVR USART baud rate register value
.equ	baud9600	= (clock/(9600*16))-1  ;59
.equ	baud19K		= (clock/(19200*16))-1  ;59   

; AVR Device and I2C Target equates
.equ	MostSigBit	= 7
.equ	TWI_Port	= PortC ; Mega8, 48,88,168
.equ	PAGE_SIZE	= 64 ; 128K - 512K bit EEPROM devices.  32K and 64K use 32-byte page sizes.
.equ	PAGESHIFTS	= 6  ; log2 of PAGE_SIZE
.equ	HandshakePort	= PortD
.equ	HandshakeDDR	= DDRD
.equ	RTS_pin		= 2 ; pulled high by PC when data is ready to be sent to the AVR and be programmed
.equ	CTS_pin		= 3 ; PortD3 goes high to signal to the PC that the AVR is ready to receive the next page
.equ	QUEUESIZE	= 128 ; must be 2^n and <256
.equ	QUEUEWRAPMASK	= QUEUESIZE-1
.equ	PgWrTimeOut	= ~ (40) ;0xd5  5.2mS [8MHz at prescale/1024==128uSec per timer increment]
.equ	Delay8uSec	= 45 ; 135 cycles at 0.125uSec/cycle     
.equ	MaxPgsHi 	=  4 ; high byte of maximum number of pages in the EEPROM device 0x0400 


; Lower Registers
.def	SREGtemp	= r1
.def	UpperDigit	= r2
.def	HighAddress	= r3
.def	LowAddress	= r4
.def	ControlByte	= r5
.def	PageCounter	= r6
.def	Counter		= r7	; doRead, doDump, doProgram
.def	LineCounter	= r8
.def	High16bitValue	= r9
.def	Low16bitValue	= r10
.def	InQueueHead	= r11
.def	InQueueTail	= r12
.def	zeroreg		= r15

; Upper Registers
.def	temp		= r16
.def	irqtemp		= r17
.def	temp2		= r18
.def	ReadCharStore	= r19	; doRead, Read_I2C_Page
.def	DataByte	= r20
.def	ErrorValue	= r21
.def	FlagsReg	= r22
.equ	BOF		= 7
.equ	ZeroPages	= 6	; doDump:set when number of pages to dump is zero.
.equ	UART_TimeOut	= 5	; PC has stopped sending data, causing Timer0 to overflow


; ZH:ZL=R31:R30    YH:YL=R29:R28   XH:XL=R27:R26


;~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
.dseg ; 1024 bytes of static RAM in mega8
.org	SRAMstart
Page_Buffer:		.byte 	PAGE_SIZE ; 0x060-0x09f
InQueue:		.byte	QUEUESIZE ; 0x0a0-0x11f    0x0df for 64 byte page
;-------------------------------------------
HighPageAddress: 	.byte	1 ; the start address of the page being worked. 0-0xffff
LowPageAddress:		.byte	1
HighPageNumber:		.byte	1 ; the number of the page being worked. 0x000-0x3ff
LowPageNumber:		.byte	1
FirstPgmPgOffset: 	.byte	1 ; offset of the first byte to be programmed in a page

;~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
.cseg
.org 000
	rjmp 	reset	; Reset Handler
; interrupt vectors for Mega8
	reti	;	INT0addr= 0x0001
	reti	;	INT1addr= 0x0002
	reti	;	OC2addr	= 0x0003
	reti	;	OVF2addr= 0x0004
	reti	;	ICP1addr= 0x0005
	reti	;	OC1Aaddr= 0x0006
	reti	;	OC1Baddr= 0x0007
	reti	;	OVF1addr= 0x0008
	reti	;	OVF0addr= 0x0009
	reti	;	SPIaddr	= 0x000a
	rjmp	RxC_IRQ ;URXCadd= 0x000b
	reti	;	UDREaddr= 0x000c
	reti	;	UTXCaddr= 0x000d
	reti	;	ADCCaddr= 0x000e
	reti	;	ERDYaddr= 0x000f
	reti	;	ACIaddr	= 0x0010
	reti	;	TWIaddr	= 0x0011
	reti	;	SPMRaddr= 0x0012

reset:
; initialize stack
	ldi	temp, high (RAMEND) ; Main program start
	out	SPH, temp	; Set Stack Pointer to top of RAM
	ldi	temp, low (RAMEND)
	out	SPL, temp

; initialize I/O ports
	sbi	HandshakeDDR, CTS_pin ; CTS [clear to send] is output from the AVR to the PC
	cbi	HandshakePort, CTS_pin ; Bring CTS [PD3] low to signal to PC that the AVR is ready to receive serial data

; initialize USART   ; USCR0C default values OK for 8/N/1
	ldi	temp, high (baud38K) ;Baud rate H
	out	UBRRH, temp
	ldi	temp, low (baud38K) ;Baud rate L
	out	UBRRL, temp
	ldi	temp, (1<<RXEN) | (1<<TXEN) | (1<<RXCIE)
	out	UCSRB, temp  ; turn on receive and transmit

; initialize TwoWire Interface
	ldi 	temp, 0x10 ; cpu clock / (16 + (2*twbr)* prescale
	out 	twbr, temp ; 8000000/48=166,666 Hz =TWI clock rate
	ldi 	temp, 0x00
	out 	twsr, temp ; TW status Reg.
	clr 	temp
	out 	twar, temp	; TW address Reg.
	ldi 	temp, (0<<TWINT)|(0<<TWEA)|(0<<TWSTA)|(0<<TWSTO)|(0<<TWWC)|(1<<TWEN)|(0<<TWIE)
;                    bit  7         6          5         4          3        2         0
	out 	twcr, temp ; TW control Reg

; initialize application
	ldi	temp, 0b10100000
	mov	ControlByte, temp
	clr	FlagsReg
	clr	zeroreg
	clr	InQueueHead
	clr	InQueueTail

; enable all selected interrupts
	sei

;************************************************
;
;  <<<<<<< MAIN CODE Loop >>>>>>>>>
;
;************************************************
main:
	ldi	temp, 3 ; FirstPrompt - display main menu options on terminal
	rcall 	DisplayMessage

	rcall 	USART_Rx     ; get main menu option ~ char returned in DataByte
	out	UDR, DataByte

; one-instruction for converting an ASCII letter to uppercase. 0x41~0x5a is unchanged; 0x61~0x7a becomes 0x41~0x5a.
	andi	DataByte, 0b11011111 ; DataByte must be a letter

Main_R:	cpi 	DataByte,'R'  ; Read a single page and send to RS232 terminal
	brne	main_W
	rcall 	doRead
	rjmp	MainLoop

Main_W:	cpi 	DataByte,'W'  ; Write a single page with predetermined pattern
	brne	Main_D
	rcall	doWrite
	rjmp	MainLoop

Main_D:	cpi 	DataByte,'D'  ; Dump the entire contents of the EEPROM
	brne	Main_E
	rcall	doDump
	rjmp	MainLoop

Main_E:	cpi 	DataByte,'E'  ; load every EEPROM location with value 0xff
	brne	Main_Q
	rcall	doErase
	rjmp	MainLoop

Main_Q:	cpi 	DataByte,'P' ; receive data from PC and write to EEPROM continously
	brne	MainLoop
	rcall	doProgram
	rjmp	MainLoop


;  add more options to the main loop here

MainLoop:
	rjmp 	main




;@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
; doRead
;@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
doRead:
	ldi	temp, 0		; prompt for four hex characters of EEPROM page 0x0000-0x03ff
	rcall 	DisplayMessage
	rcall 	Get_16bitValue	;get EEPROM page Value (four hex chars)
	sts	HighPageNumber, High16bitValue
	sts	LowPageNumber, Low16bitValue
	rcall	PageNumToAddress
	rcall	Format_BankSelect ; set or clear the Bank-Select bit in the Control byte according to HighAddress
	rcall 	read_I2C_Page	;read 64 bytes from i2c

	ldi	DataByte, 0x0d  ; carriage return
dR_2c:	in	temp2, UCSRA ; Wait for empty transmit buffer
	sbrs	temp2, UDRE
	rjmp 	dR_2c
	out	UDR, DataByte
  	ldi	DataByte, 0x0a  ; line feed
dR_2d:	in	temp2, UCSRA ; Wait for empty transmit buffer
	sbrs	temp2, UDRE
	rjmp 	dR_2d
	out	UDR, DataByte

	ldi	temp, PAGE_SIZE ; number of bytes in page buffer
	mov	Counter, temp   ; can't write immediate value to lower register
	ldi	temp, PAGE_SIZE / 8 ; number of lines displayed on terminal with 8 bytes in a line
	mov	lineCounter, temp   ; can't write immediate value to lower register
	ldi	ZH, high (Page_Buffer)
	ldi	ZL, low  (Page_Buffer)

dR_3a:	ld	ReadCharStore, z+
	mov	DataByte, ReadCharStore
	andi	DataByte, 0xf0
	swap	DataByte
	rcall	Hex2ASCII ;  send single hex digit in bits 3-0 of DataByte, return ASCII in temp
	mov	UpperDigit, temp
	mov	DataByte, temp

dR_3b:	in	temp2, UCSRA ; Wait for empty transmit buffer
	sbrs	temp2, UDRE
	rjmp 	dR_3b
	out	UDR, DataByte

	mov	DataByte, ReadCharStore
	andi	DataByte, 0x0f
	rcall	Hex2ASCII ;  send single hex digit in bits 3-0 of DataByte, return ASCII in temp
	mov	DataByte, temp
dR_3c:	in	temp2, UCSRA ; Wait for empty transmit buffer
	sbrs	temp2, UDRE
	rjmp 	dR_3c
	out	UDR, DataByte

	ldi	DataByte, 0x20  ; space

dR_3d:	in	temp2, UCSRA ; Wait for empty transmit buffer
	sbrs	temp2, UDRE
	rjmp 	dR_3d
	out	UDR, DataByte

	dec	Counter
	breq	dr_exit

	dec	LineCounter
	brne	dR_3a
	ldi	DataByte, 0x0d  ; carriage return

dR_3e:	in	temp2, UCSRA ; Wait for empty transmit buffer
	sbrs	temp2, UDRE
	rjmp 	dR_3e
	out	UDR, DataByte
	ldi	DataByte, 0x0a  ; line feed

dR_3f:	in	temp2, UCSRA ; Wait for empty transmit buffer
	sbrs	temp2, UDRE
	rjmp 	dR_3f
	out	UDR, DataByte
	ldi	temp, 8
	mov	lineCounter, temp
	rjmp	dR_3a  ; print next line

dR_exit: ret ; back to main




;@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
; doWrite  - write a test pattern into a single 64-byte page
;@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
doWrite:
	ldi	temp, 0
	rcall 	DisplayMessage ; prompt for four hex characters of EEPROM page 0x0000-0x03ff
	rcall 	Get_16bitValue	;get EEPROM page Value (four hex chars)
	sts	HighPageNumber, High16bitValue
	sts	LowPageNumber, Low16bitValue
	rcall	PageNumToAddress; return HighAddress/LowAddress
	rcall	Format_BankSelect ; return ControlByte
	mov	temp, ControlByte
	andi	temp,~ (0x01) ; clear r/_w bit for write
	mov	ControlByte, temp
; put the ASCII value of the page's real start address into the first four bytes of the page.  Follow with the colon character.
	ldi	ZH, high (Page_Buffer)
	ldi	ZL, low  (Page_Buffer)
	mov	DataByte, HighAddress
	andi	DataByte, 0b11110000
	swap	DataByte        ;DB 00001111 high nib of high addr
	rcall	Hex2ASCII	; return: temp with ASCII value
	st	z+, temp
	mov	DataByte, HighAddress
	andi	DataByte, 0b00001111
	rcall	Hex2ASCII
	st	z+, temp
	mov	DataByte, LowAddress
	andi	DataByte, 0b11110000
	swap	DataByte              ;DB 0000hhhh  high nib of low addr
	rcall	Hex2ASCII             ; return: temp with ASCII value
	st	z+, temp
	mov	DataByte, LowAddress
	andi	DataByte, 0b00001111    ;DB 0000llll  low nib of low addr
	rcall	Hex2ASCII
	st	z+, temp
	ldi	temp, ':'
	st	z+, temp

	ldi	temp, 4  ; fill the page buffer with the binary value of each byte's page_offset_value
dW_3a:	inc	temp
	st	z+, temp
	cpi	temp, PAGE_SIZE
	brlo	dW_3a

	mov	temp, HighAddress	; %%%~~~     clear bit 7 of high address to limit range 0-0x7fff
	cbr	temp, (1<<MostSigBit)	; %%%~~~
	mov	HighAddress, temp	; %%%~~~

	rcall	Write_I2C_Page ; send HighAddress, LowAddress, ControlByte, Page_Buffer (in SRAM)

dW_exit: ret	; back to main




;@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
; doDump:  get start Page and number of pages 0x0000-0x0400 to send to USART. Send data.
;@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
doDump:
	rcall	GetPageInfo
; returns with HPgNum,LPgNum=1stPage_YH:YL=#pages_HAdr,LAdr=1st page full adr_zeropg flag
	sbrc	FlagsReg, ZeroPages
	rjmp	dD_ZeroPgs
dD_0:	rcall	Format_BankSelect ; send HighAddress :: return ControlByte with BankSelect bit adjusted to real address
	rcall 	read_I2C_Page	;read 64 bytes from i2c to Page_Buffer
; send each byte in the Page_Buffer to the RS232 transmitter
	ldi	ZH, high (Page_Buffer)
	ldi	ZL, low  (Page_Buffer)
	ldi	temp, PAGE_SIZE
	mov	counter, temp
dD_2:	ld	DataByte, z+
dD_2a:	in	temp2, UCSRA ; Wait for empty transmit buffer
	sbrs	temp2, UDRE
	rjmp 	dD_2a
	out	UDR, DataByte
	dec	counter ; position in the page currently being transmitted
	brne	dD_2

	sbiw	YH:YL, 1 ; number of pages left to do
	breq	dD_exit

dD_3:	lds	temp, LowPageNumber ; increment Page Address for next Page_read
	inc	temp ; H-L PgAdr = 0x000 ~ 0x3ff
	sts	LowPageNumber, temp
	tst	temp
	brne	dD_4
	lds	temp, HighPageNumber
	inc	temp
	sts	HighPageNumber, temp
dD_4:	rcall	PgI_0
	rjmp	dD_0

dD_ZeroPgs:
	cbr	FlagsReg, (1<<ZeroPages)
dD_exit:	ret




;@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
; doErase:  get start Page and number of pages to send to USART.
;       Send 0xFF to each byte in each page. 
;	Takes about 9 seconds to erase the entire 24AA515 64Kx8 EEPROM.
;@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
doErase:
	rcall	FillPage0xFF
	rcall	GetPageInfo
; returns with HPgAdr,LPgAdr=1stPage_YH:YL=#pages_HAdr,LAdr=1st page full adr_zeropg flag
	sbrc	FlagsReg, ZeroPages ; set if zero pages to erase
	rjmp	dE_ZeroPgs
;send 64 bytes of 0xFF from Page_Buffer to EEPROM
dE_0:	rcall	Format_BankSelect ; send HighAddress :: return ControlByte with BankSelect bit adjusted to real address
	rcall 	Write_I2C_Page ; send HighAddress, LowAddress, ControlByte, Page_Buffer (in SRAM)

; **********  time out the 5mS page write  *****************************************
	ldi	temp, (1<<Tov0)
	out	TIFR, temp	; clear Timer 0 overflow flag (defaults to clear)
	ldi	temp, PgWrTimeOut; ~40	; time-out period is 5mSec==page write to be completed inside the EEPROM  
	out	TCNT0, temp
	ldi	temp, (1<<CS02) | (0<<CS01) | (1<<CS00) ; prescale /1024  [128uS per timer increment at 8MHz]
	out	TCCR0, temp
dE_loop:
	in	temp, TIFR	; wait until timer0 overflow
	sbrs	temp, TOV0
	rjmp	dE_loop        ; Overflow happened
	ldi	temp, (1<<TOV0) ; clear Timer 0 overflow flag (defaults to clear)
	out	TIFR, temp
; ---------5mS timeout --  page written in EEPROM, ready for new data
;************************************************************************************

; determine the number of pages left to do
	sbiw	YH:YL, 1
	breq	dE_exit
	lds	temp, LowPageNumber ; increment Page Address for next Page_read
	inc	temp ; H-L PgAdr = 0x000 ~ 0x3ff
	sts	LowPageNumber, temp
	tst	temp
	brne	dE_1
	lds	temp, HighPageNumber
	inc	temp
	sts	HighPageNumber, temp
dE_1:	rcall	PageNumToAddress ; return with High+LowAddress
	rjmp	dE_0

dE_ZeroPgs:
	cbr	FlagsReg, (1<<ZeroPages)
dE_exit: ret




;;@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
;; doProgram:  Get EEPROM Start Address. Receive data from PC.  If PC doesn't send a
;;	byte to the USART for 35 milliseconds, then exit this procedure.
;;	PC sends four hex chars for address of the page where writing begins.
;;
;; uses:	HighAddress, LowAddress, High16bitValue, Low16bitValue are low registers
;;	HighPageNumber, LowPageNumber are SRAM variables.
;;
;;  Get address from terminal (4 hex chars). Shift right six places to get page number. ANDI with 0xffc0 for boundary.
;;    :repeat until reset pressed:
;;      format control byte
;;      clear CLSpin to enable data transmission from the PC.
;;      get 64 data bytes from terminal (or timeout). 65mS to receive 64 bytes at 9600Kbaud.
;;      write control word and address to EEPROM.
;;      write 64-byte page to EEPROM.
;;      Send stop command to commence page write (5mS). Set CLSpin to stop data transmission from PC.
;;      adjust address to next page
;;
;;@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
doProgram:
	ldi	temp, 2   ;"Enter EEPROM address: 0x0000-0xffff  "
	rcall 	DisplayMessage ; prompt for four hex characters of EEPROM address 0x0000-0ffff
	rcall 	Get_16bitValue ; get four hex chars of EEPROM address where programming starts 0x0000-0ffff
	mov	HighAddress, High16bitValue
	mov	LowAddress, Low16bitValue
	rcall	AddressToPageNum
;--------------------------------------------------------
; read 64 bytes from the USART.
;----------------------------------------------------------
dP_ProgramPage:
	rcall	Format_BankSelect; send: HighAddress, return: adjusted ControlByte
	cbi	HandshakePort, CTS_pin ; enable the PC to send data by activating ClearToSend signal
	nop
; First load 64 bytes from PC. Put each byte into the Page_Buffer as each byte arrives into the UART input buffer.
; If 35mS passes without a byte arriving into the UART, set UART_TimeOut flag and return to Main.
	ldi	ZL, low  (Page_Buffer) ; point to where the data from the PC will be stored in sRAM
	ldi	ZH, high (Page_Buffer)
	clr	Counter
;; wait until first byte arrives from PC into UART, then activate timer0 with new count
dP_PollQ:
	mov	temp2, Counter
	cpi	temp2, PAGE_SIZE ; keep getting bytes from PC if page buffer is not full
	brsh	dP_WrPg
	rcall	USART_Rx ; wait until data byte arrives into UART
	st	Z+, DataByte  ; put data byte into page buffer
	inc	Counter ; check if page buffer if full
	rjmp	dP_PollQ
;;----------------------------------------------------------------
;; received 64 bytes from the PC.  Now write them to the EEPROM.
;;----------------------------------------------------------------
dP_WrPg:
	sbi	HandshakePort, CTS_pin ; signal the PC to stop sending data by deactivating ClearToSend
	rcall	Write_I2C_Page  ; send HighAddress, LowAddress, ControlByte, Page_Buffer (in SRAM) 
;; @3.5mS to write page {@585 clocks at 6uSec per clock} ~~  @8.5mS between CTS unasserted and reasserted
	ldi	temp, (1<<Tov0)
	out	TIFR, temp	; clear Timer 0 overflow flag (defaults to clear)
	ldi	temp, PgWrTimeOut; ~40	; time-out period is 5mSec==page write to be completed inside the EEPROM  
	out	TCNT0, temp
	ldi	temp, (1<<CS02) | (0<<CS01) | (1<<CS00) ; prescale /1024  [128uS per timer increment at 8MHz]
	out	TCCR0, temp
dP_loop:
	in	temp, TIFR	; wait until timer0 overflow
	sbrs	temp, TOV0
	rjmp	dP_loop        ; Overflow happened
	ldi	temp, (1<<TOV0) ; clear Timer 0 overflow flag (defaults to clear)
	out	TIFR, temp
; ---------5mS timeout --  page written in EEPROM, ready for new data
	lds	ZL, LowPageNumber
	lds	ZH, HighPageNumber
	adiw	ZH:ZL, 1
	sts	LowPageNumber, ZL
	sts	HighPageNumber, ZH
	rcall	PageNumToAddress   ; send: HighPageNumber, LowPageNumber return: HighAddress, LowAddress uses: temp2
	cpi	ZH, MaxPgsHi ; 4 for 24AA515
	breq	dP_FilledAll
  	rjmp	dP_ProgramPage
dP_FilledAll:
	ldi	temp, 7
	rcall	DisplayMessage
	rjmp	dP_exit

dP_exit:
	ret




;@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
;                 Subroutines
;@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@




;;**********************************************************************
;; Timer0Delay - pause until Timer0 overflows
;; send: TimerCount value in temp, prescaler in temp2
;; return:  ---
;; uses: temp
;;***********************************************************************
;Timer0Delay:
;	out	TCNT0, temp
;	in	temp, TIFR   ; clear Timer0 overflow flag
;	ori	temp, (1<<TOV0)
;	out	TIFR, temp ; clear flag by writing logic one to the flag
;	out	TCCR0, temp2 ; turn on timer 0
;T0D_loop:
;	in	temp, TIFR	; poll until timer 0 overflow flag gets set
;	andi	temp, (1<<TOV0)
;	breq	T0D_loop
;	clr	temp
;	out	TCCR0, temp	; turn off timer 0
;	in	temp, TIFR   ; clear Timer0 overflow flag
;	ori	temp, (1<<TOV0)
;	out	TIFR, temp ; clear flag by writing logic one to the flag
;	ret



;**********************************************************************************************
; Format_BankSelect - adjust the Bank-Select bit in the Control byte
; send: HighAddress
; return: ControlByte
; uses: temp
;**********************************************************************************************
Format_BankSelect:
	mov	temp, HighAddress	; adjust Bank-Select bit in the 24AA515 Control-Byte
	sbrc	temp, MostSigBit        ; skip next instruction if addr<0x8000, clear control byte bank-select bit
	rjmp	FCB_1               	; SBRC can use R0-31, but LDI must use R16-31
	ldi	temp, 0b10100001    	; address < 0x8000
	rjmp	FCB_2               	;
FCB_1:	ldi	temp, 0b10101001    	; address => 0x8000
FCB_2:	mov	ControlByte, temp  	;
	mov	temp, HighAddress	; clear bit 7 of high address to limit range 0-0x7fff
	cbr	temp, (1<<MostSigBit)	; for use with 24AA515 EEPROM
	mov	HighAddress, temp	;
	ret



;**********************************************************************************************
; PageNumToAddress
;
; send: HighPageNumber, LowPageNumber
; return: HighAddress, LowAddress
; uses: temp2
;**********************************************************************************************
PageNumToAddress:
	lds	HighAddress, HighPageNumber
	lds	LowAddress,  LowPageNumber
	ldi	temp2, PAGESHIFTS  ; expand Page Value into starting real address for that page
PNtA_1:	lsl	LowAddress
	rol	HighAddress
	dec	temp2
	brne	PNtA_1
	ret



;**********************************************************************************************
; AddressToPageNum
;
; send: HighAddress, LowAddress
; return: HighPageNumber, LowPageNumber
; uses: temp2
;**********************************************************************************************
AddressToPageNum:
	ldi	temp2, PAGESHIFTS  ; expand Page Value into starting real address for that page
AtPN_1: lsr	HighAddress
	ror	LowAddress
	dec	temp2
	brne	AtPN_1
	sts	HighPageNumber, HighAddress
	sts	LowPageNumber,  LowAddress
	ret



;**********************************************************************************************
; GetPageInfo  Sub with two entry points. Alt Enter at PgI_0
;
; called by: doErase, doDump
; send:
; return: HighPageAddress,LowPageAddress with FirstPageNumber 0-0x3ff
;	  YL, YH with the number of pages to be processed
;	  HighAddress, LowAddress with the full address of FirstPageNumber
;	  FlagsReg with ZeroPages flag set if num of pages is 0, cleared otherwise
; uses:  HighAddress, LowAddress, High16bitValue, Low16bitValue are low registers
;**********************************************************************************************
GetPageInfo:
	cbr	FlagsReg, (1<<ZeroPages)
;get four hex chars of initial page
	ldi	temp, 0	; "Enter EEPROM page: 0x0000-0x03ff  "
	rcall	DisplayMessage
	rcall 	Get_16bitValue
	sts	HighPageNumber, High16bitValue; H-L PgAddr = 0x000 - 0x3ff
	sts	LowPageNumber, Low16bitValue
;get four hex chars of number of pages to erase
	ldi	temp, 1 ; "Enter Number of Pages: 0x0000-0x0400  "
	rcall	DisplayMessage
	rcall 	Get_16bitValue
	mov	YH, High16bitValue
	mov	YL, Low16bitValue
	tst	YH
	brne	PgI_0
	tst	YL
	breq	PgInfo_noPgs; check for count of 0 pages initially entered

; expand PageAddress into starting real address for that page
PgI_0:	rcall	PageNumToAddress
	rjmp	PgInfo_exit
PgInfo_noPgs:
	sbr	FlagsReg, (1<<ZeroPages)
PgInfo_exit:
	ret



;*************************************************************************************************
;  FillPage0xFF  - used to erase page and for programming pages with less than 64 bytes of data
;
; send, receive:  nothing
; uses: ZH, ZL, temp, counter
;*************************************************************************************************
FillPage0xFF:       ; 327 cycles total  35uS
	ldi	ZH, high (Page_Buffer) ; fill Page_Buffer with value 0xFF
	ldi	ZL, low  (Page_Buffer)
	ldi	temp, PAGE_SIZE
	mov	counter, temp
	ser	temp
FPff_1:	st	z+, temp
	dec	counter
	brne    FPff_1
	ret



;******************************************************************************************************
; Get_16bitValue  -  Wait until user enters four hex characters on PC terminal.  No error checking.
;   	Convert chars to a binary value 0x0000~0xffff.
;
; send:	-
; return: valid values in High16bitValue, Low16bitValue
; uses registers: UpperDigit
; calls: ASCII2Hex
;******************************************************************************************************
Get_16bitValue:
	rcall	get_char ; return with temp
	mov	High16bitValue, temp
	rcall	get_char
	mov	Low16bitValue, temp
	ret



;********************************************************************************************
; GetChar  -  Wait until user enters two hex characters on PC terminal.  No error checking.
;   	Convert chars to a binary value 0x00~0xff.
;
; send:	-
; return: temp with binary value 00~0xff
; uses registers: UpperDigit
; calls: ASCII2Hex
;********************************************************************************************
get_char:
	rcall	USART_Rx ; wait until single keypress on PC terminal arrives into USART
GC_0:	in	temp2, UCSRA ; Wait for empty transmit buffer
	sbrs	temp2, UDRE
	rjmp 	GC_0
	out 	UDR, DataByte ; echo character pressed

	rcall	ASCII2Hex
	mov	UpperDigit, temp
	rcall	USART_Rx ; wait until single keypress on PC terminal arrives into USART

GC_1:	in	temp2, UCSRA ; Wait for empty transmit buffer
	sbrs	temp2, UDRE
	rjmp 	GC_1
	out	UDR, DataByte   ; echo character pressed
	rcall	ASCII2hex  ;send DataBye/return temp
	swap	UpperDigit

	or	temp, UpperDigit
	ret




;************************************************************************************************************
; DisplayMessage  - select a message string from a set of multiple output strings using an address table.
; send:  temp with the number of the message string to be output
; return: nothing
; uses: ZH:ZL, temp, UART peripheral
;************************************************************************************************************
DisplayMessage:
	ldi	ZL, low  (WriteTextAdrTable*2)
	ldi	ZH, high (WriteTextAdrTable*2)
	lsl	temp ; multiply by 2
	add	ZL,temp
	adc	ZH, zeroreg
	lpm	temp, Z+ ; move to low addr byte after load
	lpm	temp2, Z
	mov	ZH, temp
	mov	ZL, temp2
DM_0:	lpm	temp, Z+
	tst	temp
	breq	DM_exit
	out	UDR, temp
DM_1:	in	temp2, UCSRA ; Wait for empty transmit buffer
	sbrs	temp2, UDRE
	rjmp 	DM_1
	rjmp	DM_0
DM_exit:
	ret

WriteTextAdrTable:
	.db	high (PageText*2),  low  (PageText*2)   	 ;0
	.db	high (NmPgText*2),  low  (NmPgText*2)		 ;1
	.db	high (adrText*2),   low  (adrText*2)		 ;2
	.db	high (FirstPromptTxt*2), low  (FirstPromptTxt*2) ;3
	.db	high (ErrorText*2), low  (ErrorText*2)		 ;4
	.db	high (DataText*2),  low  (DataText*2)   	 ;5
	.db	high (UARTtext*2), low  (UARTtext*2)		 ;6
	.db	high (FilledAllText*2),  low  (FilledAllText*2)  ;7

PageText: .db 0x0a,0x0d,"Enter EEPROM page: 0x0000-0x03ff  ",0,0   	;0
NmPgText: .db 0x0a,0x0d,"Enter Number of Pages: 0x0000-0x0400  ",0,0  	;1
adrText:  .db 0x0a,0x0d,"Enter EEPROM address: 0x0000-0xffff  ",0	;2
FirstPromptTxt: .db 0x0d, 0x0a, 0x0d, 0x0a,  "24AA515 Page Test", 0x0d, 0x0a, "Press R, W, D, E, or P: ", 0x0d, 0x0a, 0 ;3
ErrorText: .db 0x0d,0x0a,0x0d,0x0a,"Error: ",0				;4
DataText: .db 0x0a,0x0d,"Enter Serial EEPROM Data: 00-FF  ",0		;5
UARTtext: .db 0x0d,0x0a, "UART Receive Timeout", 0x0d,0x0a,0,0		;6
FilledAllText: .db 0x0d,0x0a, "Filled 64Kx8 space of the EEPROM", 0x0d,0x0a,0,0 ;7



;********************************************************************************************
; ASCII2Hex  Convert an ASCII character to a binary value in bits 0-3.
;
; ASCII hex chars are 0x30~0x39 for numbers and 0x41~0x46 (or 0x61~0x66) for letters.
; if bit 6 is set, it's a letter.  Convert letters by stripping upper nibble and adding 0x09.
; if bit 6 is clear, then it's a number.  Convert by stripping the upper nibble.
;
; send:	DataByte with ASCII char
; return: temp with binary in bits 0-3.  DataByte returns unchanged.
;********************************************************************************************
ASCII2Hex:
	mov	temp, DataByte
	sbrc	temp, 6
	rjmp	A2HLtr
	andi	temp, 0b00001111
	ret
A2HLtr:	andi	temp, 0b00000111
	subi	temp, -9
	ret



;********************************************************************************************
; Hex2ASCII  Convert a binary value 0x00~0x0f to an ASCII character.
;
; Point ZH:ZL to an array in program memory and use binary value as an offset into the array.
; Return the value at the offset. Some AVR devices don't have LPM z instruction.
;
; send:	DataByte (binary in bits 0-3)
; return: temp with ASCII value, letters are uppercase.  DataByte returns unchanged.
; uses registers: ZH, ZL
;********************************************************************************************
Hex2ASCII: ; send single hex digit in bits 3-0 of DataByte, return ASCII in temp
	push	ZH
	push	ZL
	ldi	ZH, high (hexchars*2)
	ldi	ZL, low  (hexchars*2)
	add	ZL, DataByte
H2A0:	lpm	temp, z
	pop	ZL
	pop	ZH
 	ret
hexchars: .db "0123456789ABCDEF"


;*****************************************************************************
; Write_I2C_Page -  Write a 64-byte page to an address in 64Kx8 serial
;    EEPROM using AVR TWI interface. Writing starts on a page boundary.
;    Either 0x**00, 0x**40, 0x**80, or 0x**c0.
;    The 64 bytes that are written are in the Page_Buffer in SRAM.
;
; send:	HighAddress, LowAddress, ControlByte, Page_Buffer (in SRAM)
; return: ~
; uses registers: temp
; calls: tw_wait
;****************************************************************************
Write_I2C_Page:
	ldi	temp, (1<<TWINT)|(1<<TWSTA)|(1<<TWEN)	;Transmit start cond
	out	TWCR, temp
	rcall 	tw_wait

	in	ErrorValue, TWSR
	andi	ErrorValue, 0xF8
	ldi	temp2, 0x08
	cpse	ErrorValue, temp2	;Read SReg. 0x08 received (START sent)
	rjmp	ERROR
;----------------------------------------
; Send control byte [device code, address, and R/W bit]
; 24AA515 code with SLA and Bank-Select previously adjusted
	mov	temp, ControlByte
	andi	temp, 0b11111110 ; clear R~/w bit for write
	out 	TWDR, temp
	ldi 	temp, (1<<TWINT) | (1<<TWEN)
	out 	TWCR, temp
	rcall 	tw_wait

	in	ErrorValue, TWSR ;Read Status Register for TWI
	andi	ErrorValue, 0xF8
	ldi	temp2, 0x18	;0x18 = Write SLA+w - received ACK
	cpse	ErrorValue, temp2
	rjmp	ERROR ;0x40=SLA+R sent_ACK received
;--------------------------
	out 	TWDR, HighAddress
	ldi 	temp, (1<<TWINT) | (1<<TWEN)
	out 	TWCR, temp
	rcall 	tw_wait

	in 	ErrorValue, TWSR
	andi 	ErrorValue, 0xF8
	ldi	temp2, 0x28	;0x28 = Write Data - received ACK
	cpse	ErrorValue, temp2
	rjmp	ERROR
;-----------------------------------
	out 	TWDR, LowAddress
	ldi 	temp, (1<<TWINT) | (1<<TWEN)
	out 	TWCR, temp
	rcall 	tw_wait

	in	ErrorValue,TWSR
	andi	ErrorValue, 0xF8
	ldi	temp2, 0x28	;0x28 = Write Data - received ACK
	cpse	ErrorValue, temp2
	rjmp	ERROR
;-----------------------------------
	ldi	ZH, high (Page_Buffer)
	ldi	ZL, low  (Page_Buffer)
	ldi	temp, PAGE_SIZE
	mov	counter, temp
WIP_0:	ld	DataByte, z+
	out 	TWDR, DataByte
	ldi 	temp, (1<<TWINT) | (1<<TWEN)
	out 	TWCR, temp
	rcall 	tw_wait

	in	ErrorValue, TWSR ;Read SReg. 0x28 received (Acknowledge)
	andi	ErrorValue, 0xF8
	ldi	temp2, 0x28
	cpse	ErrorValue, temp2 ;Read SReg. 0x28 received (Acknowledge)
	rjmp	ERROR
	dec	counter
	brne	WIP_0 ; loop to write each byte to EEPROM page
;;-----------------------------
	ldi 	temp, (1<<TWINT) | (1<<TWSTO) | (1<<TWEN)
	out 	TWCR, temp ;Transmit stop cond to start writing page to EEPROM
	ret     ; one of two exit points for write_I2C_page subroutine, the other is in ERROR routine.



;***************************************************************************
; READ_I2C_Page
;
; Set up 64Kx8 serial EEPROM for a control byte write.  After sending START
; condition, WRITE command word, high address, and low address, do a REPEAT_START.
; Send control byte for READ. Then read 64 bytes of data, and send a STOP condition.
; Data goes into Page_Buffer Full Address range 0x0000~0xffff.
;
; send:	HighAddress, LowAddress (shifted to a page boundary), ControlByte
; return:  Page_Buffer (64 bytes in SRAM)
; uses registers: temp, counter
; calls: tw_wait
;*******************************************************************************
Read_I2C_Page:
	ldi 	temp, (1<<TWINT)|(1<<TWSTA)|(1<<TWEN)	;Transmit start cond
	out 	TWCR, temp
	rcall 	tw_wait

	in	ErrorValue, TWSR
	andi	ErrorValue, 0xF8
	ldi	temp2, 0x08
	cpse	ErrorValue, temp2	;Read SReg. 0x08 received (START sent)
	rjmp	ERROR

;--------------------------------------------
; Send control byte [device code, address, and R/W bit]
; 24AA515 code with SLA =00 and Write bit=0
	mov	temp, ControlByte
	andi	temp, 0b11111110 ; write is always first control byte
	out 	TWDR, temp
	ldi 	temp, (1<<TWINT) | (1<<TWEN)
	out 	TWCR, temp
	rcall 	tw_wait

	in	ErrorValue, TWSR
	andi	ErrorValue, 0xF8
	ldi	temp2, 0x18      ; NACK received after SLA+R
	cpse	ErrorValue, temp2
	rjmp	ERROR

;--------------------------------------------
; TWDR should only be written when TWINT is high
	out 	TWDR, HighAddress
	ldi 	temp, (1<<TWINT) | (1<<TWEN)
	out 	TWCR, temp
	rcall 	tw_wait

	in	ErrorValue,TWSR
	andi	ErrorValue, 0xF8
	ldi	temp2, 0x28      ;  0x28 data sent ACK received
	cpse	ErrorValue, temp2
	rjmp	ERROR
;-----------------------------------
	out 	TWDR, LowAddress ; Low address byte
	ldi 	temp, (1<<TWINT) | (1<<TWEN)
	out 	TWCR, temp
	rcall 	tw_wait

	in	ErrorValue,TWSR
	andi	ErrorValue, 0xF8
	ldi	temp2, 0x28      ;  0x28 data sent ACK received
	cpse	ErrorValue, temp2
	rjmp	ERROR
;-----------------------------------
;  This is a REPEATED START and it tells the slave that the
;  write operation is aborted while keeping the address that was sent.
	ldi 	temp, (1<<TWINT) | (1<<TWSTA) | (1<<TWEN)
	out 	TWCR, temp
	rcall 	tw_wait

	in	ErrorValue,TWSR
	andi	ErrorValue, 0xF8
	ldi	temp2, 0x10     ;RepeatedSTART sent
	cpse	ErrorValue, temp2
	rjmp	ERROR
;----------------------------------------------
; Send control byte [device code, address, and R/W bit]
; 24AA515 code with SLA=00 , read bit = 1
	mov	temp, ControlByte
	ori	temp, 0b00000001 ; r~/w (bit 0) set (read data)
	out 	TWDR, temp
	ldi 	temp, (1<<TWINT) | (1<<TWEN)
	out 	TWCR, temp
	rcall 	tw_wait

	in	ErrorValue, TWSR
	andi	ErrorValue, 0xF8
	ldi	temp2, 0x40	; ACK received after SLA+R
	cpse	ErrorValue, temp2
	rjmp	ERROR
;----------------------------------------------------------------
;  read 64 bytes from EEPROM
	ldi	ZH, high (Page_Buffer)
	ldi	ZL, low  (Page_Buffer)
	ldi	temp, PAGE_SIZE
	mov	counter, temp

RIP_1:	ldi 	temp, (1<<TWINT) | (1<<TWEN) | (1<<TWEA) ;Read Data with Enable ACK
	out 	TWCR, temp
	rcall 	tw_wait

	in	ReadCharStore, TWDR ; store the data byte read from the EEPROM

	in	ErrorValue, TWSR
	andi 	ErrorValue, 0xF8
	ldi	temp2, 0x50     ; 0x50 Master Read data received w/ Acknowledge
	cpse	ErrorValue, temp2
	rjmp	ERROR

	st	z+, ReadCharStore
	dec	counter
	brne	RIP_1  ; loop to read each byte in the EEPROM page

; This code doesn't use NACK to signal the last data read byte accepted by the master.
; Sending a STOP command to the TWI does that.  This may not work on non-24AA515 devices.
; When the master signals last-data-requested with NACK, the slave releases the SDA line.
; This allows the master to pull SDA and SCL low, then releasing SCL.  After a half period
; delay, the master creates a STOP condition by releasing SDA.  The master can't create
; a STOP if the slave is controlling SDA because the Most-Significant Bit of the next
; data byte that the slave is expecting to send might be logic 0.  In this case, the
; slave holds SDA low while the master controls SCL to make clock pulses.  The master
; needs SDA to be high in order to create a STOP.
;
; A STOP is a low-to-high transistion on SDA when SCL is high.   An ACK is a pulled-down
; SDA through a SCL rise-and-fall.  A NACK is a released SDA (pulled high by resistor)
; through a SCL rise-and-fall.
;-----------------------------------------------------------------------------------------
	ldi 	temp, (1<<TWINT) | (1<<TWSTO) | (1<<TWEN) ; do STOP condition
	out 	TWCR, temp
	ldi	temp, Delay8uSec  ; @8 uSec delay
RIP_2:	dec	temp
	brne	RIP_2 ; wait TWI period after STOP before turning off TWI
	ldi 	temp, (0<<TWEN)
	out 	TWCR, temp
	ret     ; one of two exit points for read_I2C_page subroutine, the other is in ERROR routine.


;*************************************************************************************************************
; ERROR  not a subroutine itself, but an extension of the 'read_I2C_page' or 'write_I2C_page' subroutine
;  display text and TWI error-condition number.  Then close TWI and return to Main.
;*************************************************************************************************************
ERROR:
 	ldi	temp, 4	; error message
	rcall 	DisplayMessage

dEr1:	mov	DataByte, ErrorValue
	andi	DataByte, 0xf0
	swap	DataByte
	rcall	Hex2ASCII ;  send single hex digit in bits 3-0 of DataByte, return ASCII in temp
	mov	DataByte, temp

ER_0:	in	temp2, UCSRA ; Wait for empty transmit buffer
	sbrs	temp2, UDRE
	rjmp 	ER_0
	out	UDR, DataByte

	mov	DataByte, ErrorValue
	andi	DataByte, 0x0f
	rcall	Hex2ASCII ;  send single hex digit in bits 3-0 of DataByte, return ASCII in temp
	mov	DataByte, temp

ER_1:	in	temp2, UCSRA ; Wait for empty transmit buffer
	sbrs	temp2, UDRE
	rjmp 	ER_1
	out	UDR, DataByte
	ldi 	temp, (1<<TWINT) | (1<<TWSTO) | (1<<TWEN) ; create STOP condition
	out 	TWCR, temp
	ldi	temp, Delay8uSec  ;45  ; @8 uSec delay Delay8uSec
dEr2:	dec	temp
	brne	dEr2 ; wait TWI period after STOP before turning off TWI
	ldi 	temp, (0<<TWEN)
	out 	TWCR, temp
	ret ; return to code that called either read_I2C_page or write_I2C_page subroutine



;*********************************************
; TWI_WAIT   wait for TWINT flag to set
;
; send: -
; return: -
; uses and changes: temp
;*********************************************
tw_wait:
	in 	temp, TWCR
	sbrs 	temp, TWINT ;TWINT set means TWI is ready for new command
	rjmp 	tw_wait
twaitx:	ret ;  TWINT is high now. TWDR should only be written when TWINT is high.



;*********************************************
; USART_Rx  -USART_RECEIVE
;
; send:	-
; returns: DataByte
; uses and changes: temp
;********************************************
USART_Rx:
	cpse	InQueueHead, InQueueTail
	rjmp	Rx_0
	rjmp	USART_Rx

Rx_0:	ldi	XH, high (InQueue)
	ldi	XL, low (InQueue)
	add	XL, InQueueHead
	brcc	Rx_1
	inc	XH
Rx_1:	ld	DataByte, X
	inc	InQueueHead
	mov	temp, InQueueHead
	andi	temp, QUEUEWRAPMASK
	mov	InQueueHead, temp
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
;T0_Overflow:
;	in	SREGtemp, SREG
;	sbr	FlagsReg, (1<<UART_TimeOut)
;	out	SREG, SREGtemp
;	reti



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
RxCa:	in	irqtemp, UDR
	st	X, irqtemp
	inc	InQueueTail
	mov	irqtemp, InQueueTail
	andi	irqtemp, QUEUEWRAPMASK
	mov	InQueueTail, irqtemp

	pop	XL
	pop	XH
	out	SREG, SREGtemp
	reti




;*******************************************************************************************************************
; Program identification is ASCII characters inserted after the code.
; Download the embedded code from the programmed AVR with C:>SP12 -rpfh ThisCode.txt
; If using AVR Dragon to download the code from the AVR device, then use HEX2BIN.EXE on the resulting .hex file.
; Use a text editor to view assembler file name at the end of the code binary listing.


program_ID:
;	"****  I2Cm8.asm  v1.0  alan_probandt@yahoo.com"
.db	"****  2ImC.8sa mv .1 0a al_nrpbonatdy@haooc.mo" ; character pairs reversed for SP12 -rpfh results
;*******************************************************************************************************************