;simple pump controller  two inputs one output and a auto stop timer
;INPUTS
;PB0 - high level - turn on the pump, start the timer
;PB1 - nominal level - turns pump off and stops timer

;OUTPUT
;PB3 - pump relay
;PB4 - error indicator  means cut out timer expired

;TIMER
;stops the pump on timeout. prevents the pump from running dry

;use 4.8mhz rc internal oscillator, pb5 is reset
.include		"tn13def.inc"

;definitions
.def	temp	=r16
.def	stat	=r17
.def	dly1	=r20		;load with 150dec
.def	dly2	=r21		;load with 150dec


;equates





.cseg

.org	0x0000 rjmp RESET ; Reset Handler
;0x0001 rjmp EXT_INT0 ; IRQ0 Handler
;0x0002 rjmp PCINT0 ; PCINT0 Handler
.org	0x0003 rjmp TIM0_OVF ; Timer0 Overflow Handler
;0x0004 rjmp EE_RDY ; EEPROM Ready Handler
;0x0005 rjmp ANA_COMP ; Analog Comparator Handler
;0x0006 rjmp TIM0_COMPA ; Timer0 CompareA Handler
;0x0007 rjmp TIM0_COMPB ; Timer0 CompareB Handler
;0x0008 rjmp WATCHDOG ; Watchdog Interrupt Handler
;0x0009 rjmp ADC ; ADC Conversion Handler
;


 RESET:
 		ldi		temp, low(RAMEND); Main program start
		out		SPL,temp
		ldi		temp,0xfc		;pb1..0 - inputs  all others outputs
		out		ddrb,temp
		ldi		temp,0x0f
		out		portb,temp
		ldi		temp,0x80
		out		clkpr,temp
		ldi		temp,0x04
		out		clkpr,temp		;prescale the main oscillator
			;

main:
		
		in		stat,pinb
		sbrs	stat,0
		rjmp	pumpon			;high level - start pump
		rjmp	main			;nope keep waiting

pumpon:
		sbi		portb,4			;turn on pump
		ldi		temp,0x00
		out		tcnt0,temp		;clear timer0
		ldi		dly1,0x96
		ldi		dly2,0x96
		ldi		temp,0x02
		out		timsk0,temp		;enable timer0 interrupt
		sei						;enable interrupts
		ldi		temp,0x05
		out		tccr0b,temp		;start timer0 with /1024 prescaler

watch:	in		stat,pinb
		sbrc	stat,1			;look to see if tank is empty
		rjmp	empty			;yes - end 
		rjmp	watch			;no - keep looking

empty:
		cbi		portb,4			;stop the pump
		ldi		temp,0x00
		out		tccr0b,temp		;stop the timer
		cli						;stop interrrupts
		rjmp	main			;all done start over



		
	





tim0_ovf:
		tst		dly1		;check to see if dly1 is zero
		brne	dec1		;no - 
		rjmp	dec2		;yes - 

dec1:	dec		dly1
		reti				;decrement dly1 and return

dec2:	tst		dly2		;check dly2 for zero
		brne	dec3		;no - 
		rjmp	error		;yes - 

dec3:	dec		dly2
		reti				;decrement dly2 and return

error:	cbi		portb,4		;shut off pump
		ldi		temp,0x00
		out		tccr0b,temp	;turn off timer
		cbi		portb,3		;turn on error led
ever:	rjmp	ever		;loop
