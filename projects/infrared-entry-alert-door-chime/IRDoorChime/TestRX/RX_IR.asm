;***************************************************************************
;Infrared Entry Alert Door Chime
;8 bit IR Receiver
;Monday, February 07, 2011, 10:44:32 AM
;works with transmitter that sends continuous 0x55
;beeps a buzzer OR turns on a relay if IR stream interrupted
;Crystal frequency is 4MHz
;
;program fuses for attiny2313:
;FuseLow: 0xFD
;FuseHigh: 0xDF
;FuseExt: 0xFF
;
.nolist
.include "tn2313def.inc"
.list
;
.equ	buzz	= 0	;PD0 Buzzer
.equ	relay = 1	;PD1 relay (low = on)
.equ	INPUT	= 2	;PD2

.equ	cmd	= 0x55	;
;
.def	ref1	  =R2		;
.def	ref2	  =R3		;
.def	S 	  =R4		;
;
;for long delays
.def	dcount  =r5		;
.def	dcount2 =r6		;
.def	dcount3 =r7		;
.def	dcount4 =r8		;
;
.def	temp	  =R16	;
.def	timerL  =R17	;
.def	timerH  =R18	;
.def	command =R19	;
.def	bitcnt  =R20	;
;--------------------------------------------------------------------
.cseg
.org 0
	rjmp	reset
;********************************************************************
;* "TIM0_OVF" - Timer/counter overflow interrupt handler
;* The overflow interrupt increments the "timerL" and "timerH"
;* every 64us and 16,384us.
;********************************************************************
.org OVF0addr
TIM0_OVF:
	in	S,sreg
	inc	timerL		;Updated every 64us @ 4MHz
	brne	TIM0_OVF_exit
	inc	timerH
TIM0_OVF_exit:
	out	sreg,S
	reti
;********************************************************************
;* Initialize timer, ports and interrupts.
;********************************************************************
reset:	
	ldi  temp,low(RAMEND)	;Initialize stackpointer
	out	SPL,temp			

	sbi	PORTD,buzz		;buzzer off
	cbi	PORTD,Relay		;relay off
	ldi	temp,0b00000011	
	out	DDRD,temp

	ldi	temp,1		;Timer/Counter 0 clocked at CK
	out	TCCR0,temp

	ldi	temp,1<<TOIE0	;Enable Timer0 overflow interrupt
	out	TIMSK,temp

	ser	temp			;PORTB as output
	out	DDRB,temp
	out	portb,temp		;turn off LED & buzzer
	sei				;Enable global interrupt
;********************************************************************
;* Calls "detect" in an endless loop and beeps buzzer for 500mS
;* if IR stream interrupted
;********************************************************************
main:
	rcall	detect			;Call RC5 detect routine

	cpi	command,cmd	   		;error if  cmd <> 0x55
	breq	main	

;	sbi	portd,relay			;turn on relay
	cbi	portd,buzz			;turn on buzzer

	rcall	HalfSecDelay		;
;
;	cbi	portd,relay			;turn off relay
	sbi	portd,buzz			;turn off buzzer
;
	rcall	FiveSecDelay
	rjmp	main
;********************************************************************
;* "detect" - RC5 decode routine
;*
;* This subroutine decodes the RC5 bit stream applied on PORTD
;* pin "INPUT". 
;*
;* If success: 	The command is returned in "command".
;* If failed: 	$FF is returned in "command"
;*
;* Crystal frequency is 4MHz
;********************************************************************
detect:
		clr	timerH

detect1:	clr	timerL

detect2:	cpi	timerH,8		;If line not idle within 131ms
		brlo	Del1
		rjmp	fault			;then exit

Del1:		cpi	timerL,55		;If line low for 3.5ms
		brge	start1		;then wait for start bit

		sbis	PIND,INPUT		;If line is
		rjmp	detect1		;low  - jump to detect1
		rjmp	detect2		;high - jump to detect2

start1:
		cpi	timerH,8		;If no start bit detected
		brge	fault			;within 130ms then exit

		sbic	PIND,INPUT		;Wait for start bit
		rjmp	start1


		clr	timerL		;Measure length of start bit
		
start2:
		cpi	timerL,17		;If startbit longer than 1.1ms,
		brge	fault			;exit

		sbis	PIND,INPUT
		rjmp	start2
						;Positive edge of 1st start bit

		mov	temp,timerL		;timer is 1/2 bit time
		clr	timerL

		mov	ref1,temp
		lsr	ref1
		mov	ref2,ref1
		add	ref1,temp		;ref1 = 3/4 bit time
		lsl	temp
		add	ref2,temp		;ref2 = 5/4 bit time
start3:
		cp	timerL,ref1		;If high periode St2 > 3/4 bit time
		brge	fault			;exit

		sbic	PIND,INPUT		;Wait for falling edge start bit 2
		rjmp	start3

		clr	timerL
		ldi	bitcnt,8		;Receive 8 bits
		clr	command
;		clr	system
sample:
		cp	timerL,ref1		;Sample INPUT at 3/4 bit time
		brlo	sample

		sbic	PIND,INPUT
		rjmp	bit_is_a_1		;Jump if line high
bit_is_a_0:
		clc				;Store a '0'
		rol	command
						;Synchronize timing
bit_is_a_0a:
		cp	timerL,ref2		;If no edge within 5/4 bit time
		brge	fault			;exit
		sbis	PIND,INPUT		;Wait for rising edge
		rjmp	bit_is_a_0a		;in the middle of the bit

		clr	timerL
		rjmp	nextbit
bit_is_a_1:
		sec				;Store a '1'
		rol	command
						;Synchronize timing
bit_is_a_1a:
		cp	timerL,ref2		;If no edge within 5/4 bit time
		brge	fault			;exit
		sbic	PIND,INPUT		;Wait for falling edge
		rjmp	bit_is_a_1a		;in the middle of the bit

		clr	timerL

nextbit:	dec	bitcnt		;If bitcnt > 0
		brne	sample		;get next bit
		ret

fault:
		ser	command		;Both "command" and "system"
		ret
;*************************************************************
.include "delay.inc"
;*************************************************************
.exit