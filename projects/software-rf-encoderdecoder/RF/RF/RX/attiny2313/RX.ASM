;********************************************************************
;* Example program
;* One chip RF decoder with attiny2313 & RX433 receiver
;* Monday, March 15, 2010, 11:18:37 AM
;*
;* this receiver code measures bit timing to distinguish
;* valid data from noise (timing is tight)
;* In order to use this code, you must have an external 3.6864MHz clock
;* it WILL NOT work with any other clock unless timing is changed
;* code works with ATTiny2313
;
;* when using attiny2313, program fuses as follows:
;* fuse Low: 0xFD, fuse High: 0xDF
;
;* must have good antenna! I used an 8" piece of copper wire
;* stripped from an RG6 coax cable (center conductor)
;
;some timerL values
;timerL = 13	;900us @3.6864 clock
;timerL = 10	;700us @3.6864 clock
;timerL = 4		;281.85us @3.6864 clock
;timerL = 5		;351.02us @3.6864 clock
;timerL = 6		;420.74us @3.6864 clock
;timerL = 7		;489.64us @3.6864 clock
;timerL = 8		;559.62us @3.6864 clock
;timerL = 9		;628.80us @3.6864 clock
;timerL = 10	;698.51us @3.6864 clock
;timerL = 11	;767.14us @3.6864 clock
;timerL = 12	;836.85us @3.6864 clock
;timerL = 13	;906.56us @3.6864 clock
;timerL = 14	;975.75us @3.6864 clock
;timerL = 15	;1044.92us @3.6864 clock
;
.nolist
.include "tn2313def.inc"
.list
; 	
; 	
.equ	INPUT		=2		;PD2
;.equ	DIAG		=3		;PD3 used for debugging with logic analyzer

.equ	LED		=5		;PD5 LED is on when valid signal received
.equ	Buzz		=6		;PD6 buzzer is on when valid signal received
;
;addr and cmd can be anything - must be the same on the transmitter!	
.equ	addr		=0x0A		;
.equ	cmd		=0x69		;
;
.def	S   = R4
;
;for long delays (use lower registers to free up high reg's)
.def	dcount  =r5
.def	dcount2 =r6
.def	dcount3 =r7

.def	temp	  =R16		;
.def	timerL  =R17
.def	address =R18
.def	command =R19
.def	bitcnt  =R20
;
.def	flags	= r21
.equ	f_low	= 0			;signal that line is low from ISR
;--------------------------------------------------------------------
.cseg
.org 0
	rjmp	Start

.ORG	INT0addr
	rjmp	EXT_INT0			;IRQ0 Handler

.org OVF0addr
	rjmp	TIM0_OVF
;
;********************************************************************
;* Initialize timer, ports and interrupts. 
;********************************************************************
.org 0x13
Start:	
	ldi  temp,low(RAMEND)	;Initialize stackpointer
	out	SPL,temp		;	
	
	ldi	temp,1		;Timer/Counter 0 clocked at CK
	out	TCCR0,temp

	ldi	temp,1<<TOIE0	;Enable Timer0 overflow interrupt
	out	TIMSK,temp

	ldi	temp,0b00111010 	;port D setup
	out	PORTD,temp
	ldi	temp,0b11111010	;PD0,PD2 = input
	out	DDRD,temp

	ldi	temp,0b00000010	;Setup Interrupt Sense Control for trigging on falling edge
	out	MCUCR,temp
;********************************************************************
main:
	ldi	temp,(1<<INT0)	;Enable "External Interrupt0 Request" 
	out	GIMSK,temp	
	ldi	flags,0

	sbr	temp,(1<<INTF0)	;clear any pending interrupts
	out	GIFR,temp

	sei				;Enable global interrupt

main1:	
	sbrs flags,f_low		;data line is low, check low time
	rjmp	main1

	ldi	flags,0
;
;********************************************************************
;bit always begins with a "0"
;     __
;____|  | = 1 (001)
;   ____
;__|    | = 0 (011)
;
;ISR begins here
;                 __
;start bit = ____|  |
;low for two bit times, then high for one bit time
;one bit time = 300uS, two bit times = 600 uS
;
Start0:
	ldi	temp,~(1<<INT0)	;Disable "External Interrupt0 Request" 
	out	GIMSK,temp	
	
	sbr	temp,(1<<INTF0)	;clear any pending interrupts
	out	GIFR,temp

	clr	timerL
;---------------------------------
;we are here because line is low from ISR, (flag was set)
Wait_High:
	cpi	timerL,15		;If line NOT high within 1mS then exit
	brlo	Wait_High1		;ELSE, measure high time
	rjmp	main

Wait_High1:
	sbis	PIND,INPUT		;wait HERE for pin to go high
	rjmp	Wait_High

	cpi	timerL,8		;If line low for at least 559.62us (two bit times)
	brge	start1		;then low time of start bit is good
	rjmp	main			;Else, ignore bit & restart

start1:				
	clr	timerL		;now line is high so measure high time of 2nd start bit

start2:
	sbis	PIND,INPUT		;wait for line to go low again
	rjmp	Start3		;line is low, measure high time of start bit

	cpi	timerL,15		;If line NOT low within 1mS then exit
	brlo	start2		;ELSE, continue to check PD2
	rjmp	main

Start3:				;we are here because line is low again
	cpi	timerL,8		;If line low for at least 559.62us (two bit times)
	brge	GetAddr		;then high time of start bit is good
	rjmp	main			;Else, ignore bit & restart
;-------------------------------------------------------------------
;we are here because start bit is good (both low & high times)
;we are synchronized to falling edge of second bit in start sequence
;now line should be low again (every bit starts with a low)
;so we got this:
;         ________
;        |        |
;________|        |we are here
;2 bits    2 bits
;
GetAddr:
	cli	
	sbr	temp,(1<<INTF0)	;clear any pending interrupts
	out	GIFR,temp

	ldi	bitcnt,8		;Receive 8 address bits
;
;check line state after 450uS (1 1/2 bit times)
GetAddr1:
	rcall BitDelay
	rcall	Half_Bit_delay

	sbis	PIND,INPUT		;skip if line is high
	rjmp	check1		;if high, must be a 0, ELSE it's a 1
	rjmp	check0

Check0:
	clc				;2nd bit = 1, clr carry bit (bit will be a "0")
	rjmp	CheckNext		;check last bit (should be a 1)

Check1:
	sec				;2nd bit = 0, set carry bit (bit will be a "1")
	rjmp	CheckNext		;this is here to keep timing correct

CheckNext:	
	rol	address		;Get next bit
	rcall BitDelay

CheckNext1:
	sbic	PIND,INPUT		;wait for line to go low
	rjmp	CheckNext1

CheckNext2:
	dec	bitcnt		;If not all bit rec'd
	brne	GetAddr1		;get next
;----------------------------------------------------------------
;now do the command
	ldi	bitcnt,8		;Receive 8 command bits
;
;check line state after 450uS (1 1/2 bit times)
GetCmd1:
	rcall BitDelay
	rcall	Half_Bit_delay

	sbis	PIND,INPUT		;skip if line is high
	rjmp	check1a		;if high, must be a 0, ELSE it's a 1
	rjmp	check0a

Check0a:
	clc				;2nd bit = 1, clr carry bit (bit will be a "0")
	rjmp	CheckNexta		;check last bit (should be a 1)

Check1a:
	sec				;2nd bit = 0, set carry bit (bit will be a "1")
	rjmp	CheckNexta		;this is here to keep timing correct

CheckNexta:	
	rol	command		;Get next bit
	rcall BitDelay

CheckNext1a:
	sbic	PIND,INPUT		;wait for line to go low
	rjmp	CheckNext1a

CheckNext2a:
	dec	bitcnt		;If not all bit rec'd
	brne	GetCmd1		;get next

	cpi	address,addr
	brne	restart

;	cpi	command,cmd		;who cares?, if we get this far it's good.
;	brne	restart

	sbi	PORTD,buzz		;Buzzer ON
	cbi	PORTD,LED		;LED ON
	ldi	temp,0xFF
	mov	dcount,temp
	rcall	Delay			;
	ldi	temp,0xFF
	mov	dcount,temp
	rcall	Delay			
	sbi	PORTD,LED		;LED OFF
	cbi	PORTD,buzz		;Buzzer OFF
	ldi	temp,0xFF
	mov	dcount,temp
	rcall	Delay			;
	ldi	temp,0xFF
	mov	dcount,temp
	rcall	Delay	
	sbi	PORTD,buzz		;Buzzer ON
	cbi	PORTD,LED		;LED ON
	ldi	temp,0xFF
	mov	dcount,temp
	rcall	Delay			;
	ldi	temp,0xFF
	mov	dcount,temp
	rcall	Delay			
	cbi	PORTD,buzz		;Buzzer OFF
	sbi	PORTD,LED		;LED OFF
restart:
	rjmp	main
;----------------------------------------------------------
;****** External Interrupt Routine 0 *********
EXT_INT0:
	in	S,sreg
	sbr flags,(1<<f_low)	;Data Line low,(signal main program)
	out	sreg,S
	reti
;********************************************************************
;* "TIM0_OVF" - Timer/counter overflow interrupt handler
;* The overflow interrupt increments the "timerL" every 72.16us.
;********************************************************************
TIM0_OVF:
	in	S,sreg
	inc	timerL		;Updated every 72.16us
	out	sreg,S
	reti
;********************************************************************
.include "delay.inc"
;********************************************************************
.exit