;-------------------------------------------------------------------------------
;B A T H R O O M   E L E C T R I C   F A N   T I M E R
;-------------------------------------------------------------------------------
;
;File Name	: Ventilator
;
;Version	: 1.1
;
;Date		: 07-07-2006
;
;Author		: Johan de Boer
;
;Hardware Configuration:
;	System Clock = Internal RC Oscillator at 1.2 MHz
;
;	Port B	B0 = push-button
;			B3 = bathroom fan (active low)
;			B4 = led (active low)
;
;History:
;0.1	24-05-06	Initial Version
;0.2	16-06-06	Push button added; timer implemented
;0.3	17-06-06	Pin change interrupt handler added
;1.0	04-07-06	Released with idicator led blinking once plus once for every
;					ten minutes the fan will remain switched on.
;1.1	07-07-06	corrected: led is turned on during last ten minutes
;					(should be turned off)
;-------------------------------------------------------------------------------
.include "tn12def.inc"					;specific ATtiny12 device definitions



;-------------------------------------------------------------------------------
;Register Definitions
;-------------------------------------------------------------------------------
.def temp		= R16
.def temp2		= R17
.def tick		= R18					;1 second = 250 * 4ms timer ticks
.def seconds	= R19
.def minutes	= R20
.def button		= R21					;push button debounce register
.def state		= R22					;current status
.def blink_cnt	= R23



;-------------------------------------------------------------------------------
;Definitions
;-------------------------------------------------------------------------------
.equ IO_BUTTON			= 0
.equ IO_FAN				= 3
.equ IO_LED				= 4
.equ TMR0_OFFSET		= 256 - 75		;counter offset for timer 0
.equ BTN_PRESSED		= 0b10000000	;code shift register -> button pressed
.equ BTN_IDLE			= 0b11111111	;button has been idle for a while
.equ BTN_INIT			= 0b11111110	;initial value for button register
.equ TMR_INIT_MIN		= 59			;duration the timer output is turn on
.equ TMR_INIT_SEC		= 59
.equ TMR_INIT_TICK		= 250
.equ ST_IDLE			= 0
.equ ST_ACTIVE			= 1



;-------------------------------------------------------------------------------
;Interrupt Table
;-------------------------------------------------------------------------------
.org 0x0000
	rjmp	init
.org PCI0addr
	rjmp	irq_pin_change
.org OVF0addr
	rjmp	irq_timer0



;-------------------------------------------------------------------------------
;Initialisation
;-------------------------------------------------------------------------------
init:
;registers
	clr		tick
	clr		seconds
	clr		minutes
	clr		blink_cnt
	ldi		button, BTN_INIT
	ldi		state, ST_IDLE

;sleep mode
	ldi		temp, 1<<SE					;enable sleep mode; sleep mode = idle
	out		MCUCR, temp

;port B
	ldi		temp, (1<<IO_FAN)|(1<<IO_LED)
	out		DDRB, temp					;pin PB0 = input; pin PB3, PB4 = output
	ldi		temp, 0b00011110			;turn off bathroom fan and led
	out		PORTB, temp					;enable pull-ups for pins PB1 and PB2

;enable pin change interrupt
	ldi		temp, (1<<SE)|(1<<SM)		;set sleep mode to power down
	out		MCUCR, temp					;
	ldi		temp, 1<<PCIF				;clear interrupt flag
	out		GIFR, temp					;
	ldi		temp, 1<<PCIE				;enable pin change interrupt
	out		GIMSK, temp					;

;enable interrupts
	sei									;enable global interrupts
	rjmp	main						;jump to main program



;-------------------------------------------------------------------------------
;Interrupt Handler for Pin Change Interrupt
;
;	Pin Change interrupt occurs when the logical value of an I/O pin changes.
;
;	Tasks:	- return to active mode
;-------------------------------------------------------------------------------
irq_pin_change:
	ldi		temp, 0<<PCIE				;turn off pin change interrupts
	out		GIMSK, temp					;
	ldi		temp, 1<<SE					;set sleep mode to idle
	out		MCUCR, temp					;
	ldi		temp, TMR0_OFFSET			;timer 0 offset
	out		TCNT0, temp					;
	ldi		temp, (1<<CS01)|(1<<CS00)	;prescale = clk(cpu) / 64
	out		TCCR0, temp					;
	ldi		temp, 1<<TOV0				;clear interrupt flag
	out		TIFR, temp					;
	ldi		temp, 1<<TOIE0				;enable timer 0 overflow interrupts
	out		TIMSK, temp
	ldi		button, BTN_INIT			;set initial value for button register
	reti



;-------------------------------------------------------------------------------
;Interrupt Handler for Timer 0 Overflow
;
;	Timer 0 overflow occurs every 4 milliseconds.
;
;	Tasks:	- retrieve push button value and put it in shift register
;-------------------------------------------------------------------------------
irq_timer0:
	ldi		temp, TMR0_OFFSET			;set timer 0 offset
	out		TCNT0, temp

	lsl		button						;update button shift register
	sbic	PINB, IO_BUTTON
	ori		button, 1

	reti



;-------------------------------------------------------------------------------
;Main Program
;-------------------------------------------------------------------------------
main:
	sleep
	cpi		state, ST_IDLE				;if state = idle
	breq	main_st_idle				;then go to main_st_idle

main_st_active:
	cpi		button, BTN_PRESSED			;if push button is pressed
	breq	turn_fan_off				;then go to turn_fan_off

	dec		tick						;decrease counter registers
	brne	main_st_active2
	ldi		tick, TMR_INIT_TICK
	dec		seconds
	brne	main_st_active2
	ldi		seconds, 60
	tst		minutes						;if all timer registers reaches zero
	breq	turn_fan_off				;then go to turn_fan_off
	dec		minutes

main_st_active2:
	cpi		tick, TMR_INIT_TICK - 1		;if tick = 249 and (seconds mod 5) = 4
	brne	test_5sec_false				;then go to test_5sec_true
	ldi		temp,4						;else go to test_5sec_false
	ldi		temp2, 5
test_5sec:
	cp		seconds, temp
	breq	test_5sec_true
	brlo	test_5sec_false
	add		temp, temp2
	rjmp	test_5sec

test_5sec_true:
	clr		blink_cnt					;determine blink count
	ldi		temp, 10					;blink_cnt = (minutes div 10) + 1
	ldi		temp2, 10
set_blink_cnt:
	cp		minutes, temp
	brlo	main						;return to main loop
	cbi		PORTB, IO_LED				;if blink count > 0 then turn on the led
	inc		blink_cnt
	add		temp, temp2
	rjmp	set_blink_cnt

test_5sec_false:
	tst		blink_cnt					;if blink count = 0
	breq	main						;then go to main loop
	ldi		temp, 49					;if tick in (49,99,149,199,249)
	ldi		temp2, 50					;then go to test_200ms_true
test_200ms:								;else go to test_200ms_false
	cp		tick, temp
	breq	test_200ms_true
	brlo	test_200ms_false
	add		temp, temp2
	rjmp	test_200ms

test_200ms_true:
	sbis	PINB, IO_LED				;switch led on or off
	rjmp	turn_led_off

turn_led_on:
	cbi		PORTB, IO_LED
	rjmp	main						;return to main loop

turn_led_off:
	sbi		PORTB, IO_LED				;when led is turned off
	dec		blink_cnt					;decrement blink counter
	rjmp	main						;return to main loop

test_200ms_false:
	rjmp	main						;return to main loop

turn_fan_on:
	ldi		state, ST_ACTIVE			;set state to active
	cbi		PORTB, IO_FAN				;turn on bathroom fan
	cbi		PORTB, IO_LED				;also turn on the led 
	ldi		minutes, TMR_INIT_MIN		;set the timer registers
	ldi		seconds, TMR_INIT_SEC		;return to main loop
	ldi		tick, TMR_INIT_TICK
	rjmp	main

turn_fan_off:
	ldi		state, ST_IDLE				;set state to idle
	sbi		PORTB, IO_FAN				;turn off bathroom fan
	sbi		PORTB, IO_LED				;turn off led
	rjmp 	main

main_st_idle:
	cpi		button, BTN_PRESSED			;if push button is pressed
	breq	turn_fan_on					;then jump to turn_fan_on
	cpi		button, BTN_IDLE
	brne	main

go_to_sleep:
	cli									;disable all interrupts
	ldi		temp, 0<<TOIE0				;turn off timer 0 overflow interrupts
	out		TIMSK, temp					;turn off timer 0 operation
	ldi		temp, (0<<CS02)|(0<<CS01)|(0<<CS00)
	out		TCCR0, temp					;
	ldi		temp, (1<<SE)|(1<<SM)		;set sleep mode to power down
	out		MCUCR, temp					;
	ldi		temp, 1<<PCIF				;clear interrupt flag
	out		GIFR, temp					;
	ldi		temp, 1<<PCIE				;enable pin change interrupt
	out		GIMSK, temp					;
	sei									;enable global interrupts
	rjmp	main						;return to main loop
