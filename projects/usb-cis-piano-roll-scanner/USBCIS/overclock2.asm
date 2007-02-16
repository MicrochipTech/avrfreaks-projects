;.include "4433def.inc"
.include "m8def.inc"
; System Process equates
;----------------------------------------------------------------------
.equ NOOFTASKS 		= 1  	; tasks are numbered 0 - 2  
.equ STACKSIZE 		= 16 	; hope there is room
							; 

.equ ProcMainID		= 0		; Task ID of the main process


.ifdef sp
.equ	SPL					=SP
.endif

.if 0
;comment for AT90S2313
.equ	UCR					=UCSRB
;.equ	UBRR				=UBRRL
;.equ	EEAR				=EEARL
.equ	USR					=UCSRA
;.equ	E2END				=127
.equ	RAMEND128			=96+127
.endif

.def mtr_delayL	= r11
.def mtr_delayH	= r12
.def chkcmnd	= r13	; checksum of command
.def chksum		= r14	; simple checksum of recieved bytes
.def ssreg    	= r15

.def shiftbuf	= R16
.def temp3		= R17	;temporary register
.def TASKSTATE	= R18	;temporary register
.def temp1		= R19	;temporary register
.def idx		= R20	;temporary register
.def c_tmp     	= r21 	; passed arguments
.def ARGL     	= r22 	; passed arguments
.def ARGH     	= r23 	; to OS, usually a pointer
.def ACC      	= r24 	; Old fashioned double wideaccumulator
.def BCC      	= r25 	;  

; TASKSTATE semephors and task no 76543210
;                                 KSSSIITT
.equ keywaiting		= 7	; a keypress occured		
.equ task2Semaphore	= 6 ; to be used by
.equ task1Semaphore	= 5 ; file load and
.equ task0Semaphore	= 4 ; playback
; bits 3 and 2 are task ID when timer was interrupted
; bits 0 and 1 are task ID of current process

;==============================================================================
;               
;                     D A T A   S E G M E N T 
;
;==============================================================================
.equ MSGQ_SIZE = 64

.DSEG                		; Start data segment 
.ORG sram_start            	; Start data segment 

.ORG RAMEND-(NOOFTASKS * STACKSIZE); - STACKSIZE
STKRESERVE: .BYTE (NOOFTASKS * STACKSIZE); - STACKSIZE
.ORG STKRESERVE
MSGQEND:
.ORG MSGQEND-MSGQ_SIZE
MSGQ:   .BYTE MSGQ_SIZE             	;Buffer for writing LCD
.ORG MSGQ - ((NOOFTASKS * 4)+4)	; OS globals
world_save:	.byte 4
TTSL:   .BYTE NOOFTASKS *2		; 0x60 task timers
SPTS:   .BYTE NOOFTASKS *2		; 0x68 task saved stack pointers

; interrupt table
.cseg

.org 0
	rjmp reset		; reset device

.org INT0addr	; External Interrupt 0
	rjmp USBDown
	
.org INT1addr	; External Interrupt 1
	rjmp USBRead
	
.org ICP1addr	; Timer/Counter Capture Event
	reti
	
.ifdef OC1addr
	.org OC1addr		; Timer/Counter1 Compare Match
	reti
.endif

.org OVF1addr	; Timer/Counter1 Overflow
	reti
	
.org OVF0addr	; Timer/Counter0 Overflow
	reti
	
.org SPIaddr		; Serial Transfer Complete
	reti
	
.org URXCaddr	; UART, Rx Complete
	reti
	
.org UDREaddr	; UART Data Register Empty
	reti
	
.org UTXCaddr	; UART, Tx Complete
	reti
	
.org ADCCaddr	; ADC Conversion Complete
	reti
	
.org ERDYaddr	; EEPROM Ready
	reti
	
.org ACIaddr		; Analog Comparator
	reti		; last  interrupt handler

.org INT_VECTORS_SIZE
;**********************************************************************
;
;		Y E   O L D E   I N T E R U P T E   H A N D L E R S
;
;**********************************************************************
_saveWorld:
	; save all upper registers in case of deferment
	; a trick to save a 16 bit value in a register
	; for a HW function we do no use.
	; make sure output compare A is off :-) if this
;	BRID PC+2
;	nop
	sts world_save,r4
	sts world_save+1,r0 
;	out OCR1AH,r4			; store the last 2 values in an unused	 
;	out OCR1AL,r0			; timer compare
	pop r4					; get the return address for placing at
	pop r0					; the bottom of the swich queue
	sts world_save+2,r4
	sts world_save+3,r0 
;
;	out OCR1BH,r4			; save the return address for later	 
;	out OCR1BL,r0			; (note, one could use this as a debug
	lds r4,world_save		
	lds r0,world_save+1		
;	in r4,OCR1AH			; restore user's register
;	in r0,OCR1AL			; 

	; save all but one register pair onto the stack

sz_save_all_st:				; special marker to keep from leaking memory
	push ZH
	push ZL
	push YH
	push YL
	push XH			 
	push XL			 	
	push BCC
	push ACC
	push ARGH		
	push ARGL	 
    push c_tmp		; fflags are used for inter process communication
	push idx		; R20				
	push temp1		; R19	
;	push temp2		; R18
	push temp3		; R17
	push shiftbuf	; R16

	; only a few low registers are not timers or belong only
	; to the MIDI task.
	push ssreg				; sreg at time of interrupt
	push r5					; usually result from divison
	push r4

	push r0					; we could defer inside an LPM
sz_save_all_end:
.equ sz_save_all = (sz_save_all_end -sz_save_all_st)

	; unfortunatally without using a second 16 bit temp it is
	; diffucault to restore a register pair and use push/pop
	; this work around take some explainig. At this point all
	; registers are on the stack.
	
	lds r4,world_save+2		
	lds r0,world_save+3		

;	in r4,OCR1BH			; push the return addres of the interrupt 
;	in r0,OCR1BL			; routine onto the stack
	push r0					; caller's return is on stack and registers are
	push r4					; saved for context switch at the end of the
							; satallite routine

	lds r4,world_save		
	lds r0,world_save+1		
;	in r4,OCR1AH			; the user arguments are still in the timer 
;	in r0,OCR1AL			; 
	push r0					; push them onto the stack so the satallite
	push r4					; routine can restore them
	
	rcall saveMoon			; return through save moon to restore temp register
	; fatal error if the code gets here
	; ldi RESULT,ERR_OS
	; rjmp OSERR

saveMoon:
;	BRID PC+2
;	nop
	pop r4					; put address of _save world into dev/null
	pop r0	
	pop r4					; restore user's args
	pop r0		
	ret						; back to interupt handler from save world with
							; all registers intact
; Interrupt 0 -- USB active

USBDown:
	; the PWREN# line went from low to high

	sbr TASKSTATE,1 << task0Semaphore
	reti

; Interrupt 1 -- Read Request
USBRead:
;	in ssreg,SREG
;	rcall _saveWorld

	; a new command is in the buffer
	ldi XL,low(MSGQ)			; t11
	ldi XH,high(MSGQ)			; t12
	clr chksum					; t13

	cbi PORTC,PINC4				; t14 read in checksum byte
	out PORTD,temp3				; t15 tick 01 set clock bit on port
								; keep clock high for read and command
								; selection? 

	in ACC,PINC					; t16 latch the two nybbles
	in BCC,PINB					;t1
	sbi PORTC,PINC4				;t2

	andi ACC,0x0F				;t3 convert the two nybbles to a single
	swap BCC					;t4 byte. -- this should catch the read
	andi BCC,0xF0				;t5 busy pre charge time T6
	or ACC,BCC
	mov chkcmnd,ACC				;t6 compare this to the calculated checksum
								; to involk a command.
	nop
	nop 
	nop	
USBRD001:
	cbi PORTC,PINC4				;t15 RD# strobe
	nop							;t16
	in ACC,PINC					;t1 latch the two nybbles
	in BCC,PINB					;t2
	sbi PORTC,PINC4				;t3

	andi ACC,0x0F				;t4 convert the two nybbles to a single
	swap BCC					;t5 byte. -- this should catch the read
	andi BCC,0xF0				;t6 busy pre charge time T6
	or ACC,BCC					;t7

	ldi c_tmp,high(MSGQEND-1)	;t1
	cpi XL,low(MSGQEND-1)		;t2
	cpc XH,c_tmp				;t3
	breq USBRD010				;t4
	st X+,ACC					;t5
	ldi c_tmp,high(MSGQ+7)		;t6 commands are 6 bytes
	cpi XL,low(MSGQ+7)			;t7
	cpc XH,c_tmp				;t8
	brge USBRD010				;t9 keep scanning for set parameters
	add chksum,ACC				;t10
USBRD010:
	sbis PIND,PIND3				;t12
	rjmp USBRD001				;t13 loop until FIFO empty

	clr ACC						;t14 0 byte terminator.
	st X+,ACC					;t15

	sbr TASKSTATE,1 << task1Semaphore	;t16
	ldi	ACC, 1 << INT1			;t1
	out GIFR,ACC				;t2 clear Read Busy cycle

	; test echo the string back
;	rcall USBWriteMSG

;	rjmp KEXIT				; restore SREG, other registers and reti
	ret

;	reti

;----------------------------------------------------------------------
USBWriteMSG:
	ldi XL,low(MSGQ)
	ldi XH,high(MSGQ)

	sbic PIND,PIND1
	nop						; TXE# busy


	ldi c_tmp,0b00011111	; activate bus for write
	out DDRB,c_tmp
	ldi c_tmp,0b00111111
	out DDRC,c_tmp


USBWR001:					; write loop
	ld ACC,X+
	cpi ACC,0
	breq EXIT_WR
	mov BCC,ACC
	swap BCC
	andi BCC,0x0F
	in c_tmp,PORTB			; save the scanner state
	andi c_tmp,0x30
	or BCC,c_tmp
	out PORTB,BCC			; drive the bus hard

	andi ACC,0x0F
	in c_tmp,PORTC			; save the RW state
	andi c_tmp,0x30
	or ACC,c_tmp
	ori ACC,0b00100000		; enable write pulse
	out PORTC,ACC			; drive the bus hard
	nop						; settle time
	cbi PORTC,PINC5			; latch the data
	nop						; data stable
	sbic PIND,PIND1			; buffer full?
	nop						
	rjmp USBWR001

EXIT_WR:
	in c_tmp,PORTB			; clear the last byte from the
	andi c_tmp,0b00110000	; bus
	out PORTB,c_tmp
	in c_tmp,PORTC
	andi c_tmp,0b00110000
	out PORTC,c_tmp

	sbi PORTC,PINC5			
	nop
	cbi PORTC,PINC5	
	nop

	ldi c_tmp,0b00010000	; restore tristate
	out DDRB,c_tmp
	ldi c_tmp,0b00110000	
	out DDRC,c_tmp

	cbi PORTD,PIND0			; send a SI message?
	nop
	nop
	sbi PORTD,PIND0
	ret

USB_WriteZ:					; write string from program memory

	sbic PIND,PIND1
	nop						; TXE# busy

	ldi c_tmp,0b00011111	; activate bus for write
	out DDRB,c_tmp
	ldi c_tmp,0b00111111
	out DDRC,c_tmp

WRITEZ00:
	lpm
	mov ACC,r0
	cpi ACC,0
	breq EXIT_WRZ
	mov BCC,ACC
	adiw ZL,1

	swap BCC
	andi BCC,0x0F
	in c_tmp,PORTB			; save the scanner state
	andi c_tmp,0x30
	or BCC,c_tmp
	out PORTB,BCC			; drive the bus hard

	andi ACC,0x0F
	in c_tmp,PORTC			; save the RW state
	andi c_tmp,0x30
	or ACC,c_tmp
	ori ACC,0b00100000		; enable write pulse
	out PORTC,ACC			; drive the bus hard
	nop						; settle time
	cbi PORTC,PINC5			; latch the data
	nop						; data stable
	sbic PIND,PIND1			; buffer full?
	nop						
	rjmp WRITEZ00

EXIT_WRZ:
	ret

;----------------------------------------------------------------------
;********************************************************************
;* Init program
;********************************************************************
;------------------------------------------------------------------------------------------
reset:			;initialization of processor and variables to right values
	; port pin usage
	; PD0 OUT	SI/WU		; send immediate wake up
	; PD1 IN	TXE#		; FIFO out busy
	; PD2 IN	PWR0		; USB active		 (int0)
	; PD3 IN	RXF#		; FIFO message avail (int1)
	; PD4 OUT   CIS_XFER	; cis strobe
	; PD5 OUT	CIS_CLK		; cis clock
	; PD6 PD7 Analog capture

	ldi temp3,0b00100001	; set clock state bit and SI
	ldi ACC,0b00110001
	out DDRD,ACC       		; 
	out PORTD,temp3  

	CLI
	ldi	ACC,low(RAMEND)		; initialization of stack
	out	SPL,ACC

.ifdef sph
	ldi	ACC,high(RAMEND)	; initialization of stack
	out	SPH,ACC
.endif
	clr ACC					; slam all interrupts off
	out GIMSK,ACC
	WDR

	; disable watchdog for testing
	ldi ACC,1 << WDTOE | 1 << WDE | 1 << WDP2 | 1 << WDP1 | 1 << WDP0
	out WDTCR,ACC
	ldi ACC,0 << WDE | 1 << WDP2 | 1 << WDP1 | 1 << WDP0
	out WDTCR,ACC

	clr	XH					;
	clr	YH					;
	clr	ZH					; ROM pointer

			
		
   	; PC0,1,2,3 IO			; low nybble
	; PC4 OUT	RD# 		; read low strobe, clocks data in when low to
	;						; high
	; PC5 OUT	WR			; write strobe on high to low
	ldi ACC,0b00110000
	out DDRC,ACC       	
	ldi ACC,0b00010000
	out PORTC,ACC      

	; PB0,1,2,3 IO			; high nybble
	; PB4 OUT				; step pulse
	; PB5 IN				; Scan abt

	ldi ACC,0b00010000
	out DDRB,ACC       	

	ldi ACC,0b00100000
	out PORTB,ACC      

	;set the motor timout
	ser ACC
	mov mtr_delayL,ACC
	mov mtr_delayH,ACC


;Set up the MCU Control register (3-29)
;
	; this sets the interrupts for a one bit scanner
	; the two bit scanner does not have interrupts
	
	ldi	ACC, 1 << ISC11 | 0 << ISC10 | 1 << ISC01 | 1 << ISC00 	
	out	MCUCR,ACC 
	
	ldi	ACC, 1 << INT1 | 1 << INT0 
	out	GIMSK,ACC
	
	ldi idx,0b00000000  	; blank for state reset
	ldi temp1,0b00000001	; default port D state

	; main polling routine, scanner will output clocks wile idle
	; this is to keep the array from saturating and pulling too
	; much power.

PWR0WAIT_SYNC:
	; wait for USB online PWREN# == 0
	nop ; tick 11		; 
	nop ; tick 12	from saturating

PWR0WAIT:
	ldi c_tmp,0xFF			; t13 -1 for motor decrement
	cp  mtr_delayL,c_tmp	; tick 14
	cpc mtr_delayH,c_tmp	; tick 15
	brne MOTOR_STEP			; t16 branch if motor active 					
	; clock on the array
						; tick 00 (tick 16)
	out PORTD,temp3		; tick 01 set clock bit on port
	WDR					; tick 02 reset the dog 
	nop 				; tick 03 
	nop 				; tick 04
	out PORTD,temp1		; tick 05 clear clock pulse
	nop ; tick 06		; 
	nop ; tick 07		; 

	sbic PIND,PIND2		; tick 8
	rjmp PWR0WAIT_SYNC	; tick 9 when skip

	; wait for a read request to issue us a command

	; commands are
	; 0 scanner on -> scan state READY | SENSOR_ABT
	; 1 scan single -> returns one line of CIS data bit packed
	; 2 scan step	-> same as 1 but issue a step pulse
	; 3 single pulse -> READY | SENSOR_ABT
	; 5 set step speed ASCII (pulse width) -> READY | SENSOR_ABT
	; 6 read step speed (pulse width) -> width ascii
	; 7 scan RLE 	-> return RLE data
	; 8 scan RLE with step

	; check for pending data in FIFO
	sbic PIND,PIND3		;tick 10
	rjmp PWR0WAIT 		;tick 11 wait for a command

	rcall USBRead 		; get it

	; got it
	cp chkcmnd,chksum
	brne BADCMD

	; good.
ck10:
	ldi c_tmp,0x86		; single scan
	cp chkcmnd,c_tmp
	brne ck11
	rjmp SCAN10
ck11:
	ldi c_tmp,0x87
	cp chkcmnd,c_tmp
	brne ck01
	rjmp SCAN11
ck01:
	ldi c_tmp,0x9D
	cp chkcmnd,c_tmp
	brne ck48	
	rjmp STEP01	; - Step motor 1 pulse width.

ck48:
	ldi c_tmp,0x9C
	cp chkcmnd,c_tmp
	brne ck20	
	rjmp STAT00	; - Step motor 1 pulse width.

ck20:

BADCMD:
	ldi ZL,low(ERRORSTR *2)     ; indicate card detected
	ldi ZH,high(ERRORSTR *2)

	rcall USB_WriteZ
	rcall USBWriteMSG

	
forever:
	WDR
	rjmp PWR0WAIT

MOTOR_STEP:
	out PORTD,temp3			; tick 01 set clock bit on port
	sub mtr_delayL,c_tmp	; tick 02 this will add one to the 
	sbc mtr_delayH,c_tmp 	; tick 03 motor pulse count
	nop 					; tick 04
	out PORTD,temp1			; tick 05 clear clock pulse
	cp  mtr_delayL,c_tmp	; tick 06 compare it to timeout
	cpc mtr_delayH,c_tmp	; tick 07 blocking value
	breq MS050				; tick 08
	nop 					; t9	; should we check for pipe in message
	rjmp PWR0WAIT 			; t10   ; while busy?
MS050:	
	sbis PINB,PINB4
	rjmp PWR0WAIT			; done with pulse

	cbi PORTB,PINB4			;t9 or t10? clear the pulse bit
	ldi ARGL,low(0xFFFF-1900)
	ldi ARGH,high(0xFFFF-1900)
	mov mtr_delayL,ARGL
	mov mtr_delayH,ARGH		; set duty cycle

;	ldi ZL,low(READYSTR *2)     ; indicate card detected
;	ldi ZH,high(READYSTR *2)

;	rcall USB_WriteZ
;	rcall EXIT_WR

	rjmp PWR0WAIT 			; 
	
	
	
	
 		
;------------------------------------------------------------------------------------------
;********************************************************************
;* Main program
;********************************************************************
;		SEI					;enable interrupts globally
SCAN10:
; Enable Comparitor
	cbi ACSR,ACD


;		simple test to see if we can clock the system for scan rate of 1 MHz

				
StartPulse: 
						; tick 00 (tick 16)
	out PORTD,temp3		; tick 01 set clock bit on port
	ldi temp3,0b00010001; tick 02 set start pulse after clock
	nop 				; tick 03
	nop 				; tick 04
	out PORTD,temp1		; tick 05 clear clock pulse
	out PORTD,temp3		; tick 06 start pulse at end of clock
	ldi temp3,0b00110001; tick 07 hold start pulse on during first clock
	ldi shiftbuf,7		; tick 08 set first three bits on line start
	ldi XL, 0x60        ; tick 09 ring tail 
	ldi XH, 0			; tick 10 
	ldi ZH,high(3648)	; tick 11 number of pixels in a tight scan line
	ldi ZL,low(3648)	; tick 12
	ldi YL, 0x60		; tick 13 ring head
	ldi YH, 0x00		; tick 14 
	clr c_tmp           ; tick 15 simple semaphore for blocking FIFO
						; 		  and PWREN# lines 
	nop					; tick 16 
		
	out PORTD,temp3		; tick 01 set clock bit on port
	ldi temp3,0b00010001; tick 02 clear start pulse on
	clr ARGL   	 		; tick 03 used for ring semaphore 
	clr ARGH     		; tick 04 used for ring count
	out PORTD,temp3     ; tick 05 clear clock pulse
	out PORTD,temp1     ; tick 06 clear start pulse
	ldi temp3,0b00100001; tick 07 arm for fast write to start pulse
	ldi c_tmp,0b00011111; tick 08 activate bus for write
	out DDRB,c_tmp		; tick 09
	ldi c_tmp,0b00111111; tick 10
	out DDRC,c_tmp		; tick 11 bus is now in write mode
	nop					; tick 12 
	nop					; tick 13 
	nop					; tick 14 
	nop					; tick 15 
	nop					; tick 16 

		out PORTD,temp3		; tick 01 set clock bit on port
		nop					; tick 02 clear start pulse on
		nop     	 		; tick 03 
		nop     	 		; tick 04 
		out PORTD,temp1     ; tick 05 clear clock pulse
		nop				    ; tick 06 
		nop 				; tick 07 
		nop					; tick 08
		nop					; tick 09 
		nop					; tick 10 
		nop					; tick 11 
		nop					; tick 12 
		nop					; tick 13 
		nop					; tick 14 
		rjmp bit4			; tick 15 the first three bit times
							; tick 16 used to write the FIFO 
								

							
TightScan:
bit7:		
		out PORTD,temp3		; tick 01 set clock bit on port
		mov ACC,shiftbuf	; tick 02 save new byte
		mov BCC,shiftbuf	; tick 03 
		swap BCC			; tick 04 prepare for transmit 
		out PORTD,temp1		; tick 05 clear clock pulse
		clr shiftbuf 		; tick 06 prep byte for next scan
		andi ACC,0x0F		; tick 07 prep the nybbles for a bus write
		andi BCC,0x0F		; tick 08
		ori ACC,0b00110000	; tick 09 set the RD# and WR for a bus write
		sbic ACSR,ACO		; tick 10 
		sec					; tick    11   bit to shift
		sbis ACSR,ACO		; tick 11 12  
		clc                 ; tick 12
		rol shiftbuf  		; tick 13
		in c_tmp,PIND		; tick 14 get the FIFO state
		or c_tmp,ARGL 		; tick 15 Blocking semaphore in bit 2

;gets complicate here -- If we can write we do one branch
; if the Fifo is busy we need to do a different branch the tick count
; and sample point must remain constant.

; We  implement a classic ring here; saving the
; two output bytes into the ring tail. This would be pointed to by X
; Y or Z would then point to the ring head, which if the tranmitter is ready
; and the ring not empty would supply the FIFO

bit6:
		out PORTD,temp3		; tick 01 set clock bit on port
		st X+,ACC			; tick 02 save the high nybble in the ring
							; tick 03  
		inc ARGH			; tick 04 bump the ring counter -- We do this
							;		  here since there are not enough ticks
							;		  to do the next save
		out PORTD,temp1		; tick 05 clear clock pulse


		st X+,BCC			; tick 06 Store the low Nybble into the ring
							; tick 07 we have already counted it

		ld ACC,Y+			; tick 08 get next byte from the ring
							; tick 09							 
		sbic ACSR,ACO		; tick 10 Sample point
		sec					; tick    11   bit to shift
		sbis ACSR,ACO		; tick 11 12  
		clc                 ; tick 12
		rol shiftbuf  		; tick 13
		ld BCC,Y+			; tick 14 The other part of the byte pair to transmit 
							; tick 15

bit5:
		out PORTD,temp3		; tick 01 set clock bit on port
		andi c_tmp,0b0000110; tick 02 -- Fifo clear, we can write
		brne FIFO_BLOCKED	; tick 03 if bit 2 (TXE#) or bit 3 (PWREN#)
							; is set then no write is allowed
							;         bit 3 PWREN# is overlayed with the
							;		  blocking semaphore which means
							;		  that the rest of the scan line is
							;		  buffered, and will be sent (If possible)
							;		  when the transmitter is ready

		out PortB,BCC		; tick 04 fifo is clear start the write 
		out PORTD,temp1		; tick 05 clear clock pulse
		out PortC,ACC		; tick 06 -- WR active
		cbi PORTC,PINC5		; tick 07 write latch the data
		dec ARGH			; tick 08 remove entry from ring counter
		andi YL,0x7F		; tick 09 mask the ring to 128 for ICE testing
		sbic ACSR,ACO		; tick 10 
		sec					; tick    11   bit to shift
		sbis ACSR,ACO		; tick 11 12  
		clc                 ; tick 12
		rol shiftbuf  		; tick 13
		rjmp bit4			; tick 14 
							; tick 15


FIFO_BLOCKED:	
							; tick 04 
		out PORTD,temp1		; tick 05 clear clock pulse
		sbiw YL,2 			; tick 06 
		nop					; tick 07 
		nop					; tick 08		
		nop					; tick 09
		sbic ACSR,ACO		; tick 10 
		sec					; tick    11   bit to shift
		sbis ACSR,ACO		; tick 11 12  
		clc                 ; tick 12
		rol shiftbuf  		; tick 13
		nop					; tick 14 
		nop					; tick 15

bit4:
		out PORTD,temp3		; tick 01 set clock bit on port
		andi XL,0x7F		; tick 02 mask the ring to 128 for ICE testing
		ori  XL,0x60		; tick 03 cant write below 0x60 this gives 32b
		ori  YL,0x60		; tick 04 for testing, ring is from 0x60 to 0x7F 
		out PORTD,temp1		; tick 05 clear clock pulse
		nop					; tick 06 
		nop					; tick 07 
		nop					; tick 08		
		nop					; tick 09
		sbic ACSR,ACO		; tick 10 sample starts here
		sec					; tick    11   bit to shift
		sbis ACSR,ACO		; tick 11 12  
		clc                 ; tick 12
		rol shiftbuf  		; tick 13
		nop					; tick 14 
		nop					; tick 15
bit3:
		out PORTD,temp3		; tick 01 set clock bit on port
		nop					; tick 02 
		nop					; tick 03 
		nop					; tick 04 
		out PORTD,temp1		; tick 05 clear clock pulse
		nop					; tick 06 
		nop					; tick 07 
		nop					; tick 08		
		nop					; tick 09
		sbic ACSR,ACO		; tick 10 
		sec					; tick    11   bit to shift
		sbis ACSR,ACO		; tick 11 12  
		clc                 ; tick 12
		rol shiftbuf  		; tick 13
		nop					; tick 14 
		nop					; tick 15
bit2:
		out PORTD,temp3		; tick 01 set clock bit on port
		nop					; tick 02 
		nop					; tick 03 
		nop					; tick 04 
		out PORTD,temp1		; tick 05 clear clock pulse
		nop					; tick 06 
		nop					; tick 07 
		nop					; tick 08		
		nop					; tick 09
		sbic ACSR,ACO		; tick 10 
		sec					; tick    11   bit to shift
		sbis ACSR,ACO		; tick 11 12  
		clc                 ; tick 12
		rol shiftbuf  		; tick 13
		nop					; tick 14 
		nop					; tick 15
bit1:
		out PORTD,temp3		; tick 01 set clock bit on port
		nop					; tick 02 
		nop					; tick 03 
		nop					; tick 04 
		out PORTD,temp1		; tick 05 clear clock pulse
		nop					; tick 06 
		nop					; tick 07 
		nop					; tick 08		
		nop					; tick 09
		sbic ACSR,ACO		; tick 10 
		sec					; tick    11   bit to shift
		sbis ACSR,ACO		; tick 11 12  
		clc                 ; tick 12
		rol shiftbuf  		; tick 13
		nop					; tick 14 
		nop					; tick 15
bit0:
		out PORTD,temp3		; tick 01 set clock bit on port
		subi ZL,8		 	; tick 02 
		sbci ZH,0			; tick 03 
		nop					; tick 04 
		out PORTD,temp1		; tick 05 clear clock pulse
		breq TightExit		; tick 06
		nop					; tick 07
		nop					; tick 08 
		nop					; tick 09 
		sbic ACSR,ACO		; tick 10
		sec					; tick 11   
		sbis ACSR,ACO		; tick 12 13  
		clc                 ; tick    14
		rol shiftbuf		; tick 14		;
		rjmp TightScan		; tick 15/16 	; 
							

TightExit:
		nop					; tick 08 
		nop					; tick 09 
		sbic ACSR,ACO		; tick 10
		sec					; tick 11   
		sbis ACSR,ACO		; tick 12 13  
		clc                 ; tick    14
		rol shiftbuf		; tick 14		;

		ldi ZH,high(3648)   ; tick 15 number of pixels in a tight scan line
		ldi ZL,low(3648) 	; tick 16

		; buffer save the last byte of data
		out PORTD,temp3		; tick 01 set clock bit on port
		mov ACC,shiftbuf	; tick 02 save new byte
		mov BCC,shiftbuf	; tick 03 
		swap BCC			; tick 04 prepare for transmit 
		out PORTD,temp1		; tick 05 clear clock pulse
		andi ACC,0x0F		; tick 06 prep byte for next scan
		andi BCC,0x0F		; tick 07 prep the nybbles for a bus write
		ori ACC,0b00110000	; tick 08
		in c_tmp,PIND		; tick 09 set the RD# and WR for a bus write
		or c_tmp,ARGL		; tick 10
		out PORTD,temp3		; tick 11
		st X+,ACC			; tick 12
		inc ARGH			; tick 13
		out PORTD,temp1		; tick 14 get the FIFO state
		st X+,BCC			; tick 15 Blocking semaphore in bit 2
		ld ACC,Y+			; tick 16
				
		out PORTD,temp3		; tick 01 set clock bit on port
		ld BCC,Y+			; tick 02 save the high nybble in the ring
		out PORTD,temp3		; tick 03  
		andi BCC,0x0F		; tick 04 no pulse bits for debug 	
		out PORTD,temp1		; tick 05 clear clock pulse
		andi c_tmp,0b0000110; tick 06 
		brne FIFO_BLOCKED000; tick 07 

		out PortB,BCC		; tick 08 fifo is clear start the write 
		out PortC,ACC		; tick 09-- WR active
		cbi PORTC,PINC5		; tick 10 write latch the data
		dec ARGH			; tick 11 remove entry from ring counter
		andi YL,0x7F		; tick 12 mask the ring to 128 for ICE testing
		nop			  		; tick 13
		nop					; tick 14
		rjmp HASRINGDATA	; tick 15 
		nop					; tick 16


FIFO_BLOCKED000:	
							; tick 08 
		sbiw YL,2 			; tick 09 
		nop					; tick 07 
		nop					; tick 08		
		nop					; tick 09
		nop					; tick 10 
		nop					; tick 11
		nop					; tick 12 
		nop					; tick 13
		nop					; tick 14 
		nop					; tick 15
		nop					; tick 16 

		; ARGH will be non zero if ring still has data to transmit

HASRINGDATA:
	out PORTD,temp3
RINGDATABSY:
	cpi ARGH,0			;t2
	breq RINGDEPL		;t3

	; is USB online? -> abort if PWREN# is not 0

	sbic PIND,PIND1		;t4		; is FIFO ready?
	rjmp RINGDATABSY	;t5		; TXE# busy
	
;	out PORTD,temp1		; tick 05 clear clock pulse
		
	ld ACC,Y+			; t6 buffered data is preped and
	ld BCC,Y+			; t7 stored in the ring all we have to do
	andi BCC,0x0F		; for fifo ICE debug
	andi ACC,0x0F		; t8 fix garbage data in ice test SRAM too
	ori ACC,0b00110000	; t9 small for a full ring
;	ldi ACC,0b00110101
;	ldi BCC,0b00001010
	out PORTB,BCC		; t10 is write it onto the already configured
	out PORTC,ACC		; t11 bus.
	cbi PORTC,PINC5		; t12
	dec ARGH			; t13	
	rjmp RINGDATABSY	; t14 t15

RINGDEPL:
	; send a sync EOL packet

	; restore bus to input mode
;	out PORTD,temp1			; tick 05 clear clock pulse
	in c_tmp,PORTB			; t6 clear the last byte from the
	andi c_tmp,0b00110000	; t7 bus
	out PORTB,c_tmp			; t8
	in c_tmp,PORTC			; t9
	andi c_tmp,0b00110000	; t10	
	out PORTC,c_tmp			; t11

	ldi c_tmp,0b00010000	; t12 restore tristate
	out DDRB,c_tmp			; t13
	ldi c_tmp,0b00110000	; t14
	out DDRC,c_tmp			; t15

	cbi PORTD,PIND0			; t16 send a SI message?
	out PORTD,temp3			; t1
	nop						; t2
	sbi PORTD,PIND0			; t3
	nop						; t4
;	out PORTD,temp1			; tick 05 clear clock pulse
	nop						; t6
	nop						; t7
	nop						; t8

; check for a motor tick timer

	ldi c_tmp,0xFF	
	cp mtr_delayL,c_tmp
	cpc mtr_delayH,c_tmp
	breq SKIP_SYNC000

	sbi PORTB,PINB4			; turn the motor on

SKIP_SYNC000:
	rjmp PWR0WAIT_SYNC 		; t9 (starts at t11) StartPulse     
							; tick blah

SCAN11: ;- scan and return one line of single bit data bit packed -step motor one pulse width
	; set motor state
	ldi ARGL,low(0xFFFF-1900)
	ldi ARGH,high(0xFFFF-1900)
	mov mtr_delayL,ARGL
	mov mtr_delayH,ARGH

	rjmp SCAN10	; t9 (starts at t11) StartPulse     

SCAN20: ;- scan and return one line of two bit data interleaved (twin bit array)
SCAN21: ;- scan and return one line of two bit data interleaved  - step motor one pulse width
SCAN18: ;- scan and return one line from array one
SCAN19: ;- scan and return one line from array one, step motor one pulse width
SCAN28: ;- scan and return one line from array two
SCAN29: ;- scan and return one line from array two, step motor one pulse width
	; disable comparator
	rjmp PWR0WAIT_SYNC 		; t9 (starts at t11) StartPulse     

STEP01:	; - Step motor 1 pulse width.
	; start motor tick pulse counter	
	; read mtr_delayL & H from EEPROM

	ldi ARGL,low(0xFFFF-3800)
	ldi ARGH,high(0xFFFF-3800)
	mov mtr_delayL,ARGL
	mov mtr_delayH,ARGH

	sbi PORTB,PINB4			; turn the motor on

	rjmp PWR0WAIT_SYNC 		; t9 (starts at t11) StartPulse     

STEP33:	; 000 - set the step pulse width in CIS clock event ticks.
STEP22:	; - return the STEP pulse width stored in the EEPROM

STAT00:	; - return the state of the scanner as ASCII
	; compare port b to assert mask
	; return abort if line asserted

	; compare to step counter
	; return busy if in step pulse	

	ldi ZL,low(READYSTR *2)     ; indicate card detected
	ldi ZH,high(READYSTR *2)

	rcall USB_WriteZ
	rcall EXIT_WR
	rjmp PWR0WAIT_SYNC

STAT99:	; - return the state of the microcontoller IO pins


SETA01:	; - set abort bit to assert high
SETA00:	; - set abort bit to assert low
SETC01:	; - turn CIS clock on
SETC02:	; - turn CIS clock off

SETC20:	; - set CIS clock line low
SETC21:	; - set CIS clock line high
SETC40:	; - set SP clock line low
SETC41:	; - set SP clock line high

;Optional commands that work only with the existing single bit interface

RLE010: ;- return a scan line as Run length encoded data into the FIFO
	ldi BCC,0b00000000  	; blank for state reset		
	ldi ACC,1 << CS12 | 1 << CS11	
	CLI 					; busy till line is scanned
	out TCCR1A,BCC			; No comapre match or PWM
	out TCCR1B,ACC			; set timer to trip on write 0 to T1

	rjmp PWR0WAIT_SYNC		;StartPulse      ; tick blah

RLE011: ;- return a scan line as Run length encoded data into the FIFO, with step
COMP22: ;- read the ADC voltage on the neg input of the comparator.

;				E		R			R		O			R
;ERRORSTR: .db 0x04,0x05,0x05,0x02,0x05,0x02,0x04,0x0F,0x05,0x02 ,0, 0
ERRORSTR:	.db "Error: Bad command ",0x00 
READYSTR:	.db "READY",0x00
BUSY_STR:	.db "BUSY",0x00,0x00
ABORTSTR:	.db "ABORT",0x00
