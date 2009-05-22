;	Servo control system, 1-2ms pulse max 20ms repeat
;	1.5ms Neutral, 1.25ms - 1.75ms stroke
;	1.1ms - 1.8ms esc throttle
;	pulse every 20ms

;	Uses AVR Tiny 26 @ 1MHz internal osc


.include "tn26def.inc"

.def		tmp = R16
.def		delay = R17
.def		analog = R18

.equ		pot = 3				; ADC3 / PA4
.equ		servo = PA5			; servo out pin
.equ		servo_d = DDRA
.equ		servo_p = PORTA

.cseg
.org		0x0000
	rjmp	reset

reset:
	ldi		tmp, LOW(RAMEND)	; setup stack
	out		SP, tmp
	ldi		tmp, 1<<servo		; enable servo pin output
	out		servo_d, tmp

main:
	ldi		tmp, (1<<ADLAR)|pot	; set ADC channel and left adjust
	out		ADMUX, tmp
	ldi 	tmp, 1<<ADEN		; set ADC params
	out		ADCSR, tmp

sense:
	sbi		ADCSRA,ADSC			; start a single conversion
wadc:
	sbis	ADCSRA, ADIF		; wait until conversion is complete
	rjmp	wadc
	in		analog, ADCL		; read value from adc
	in		analog, ADCH

	ldi		delay, 50			; 20ms delay required between pulses
	rcall	delay_ms

	ldi		tmp, 1<<servo		; set servo pin high
	out		servo_p, tmp

	ldi		delay, 250			; delay first 1ms
	rcall	delay_servo


	mov		delay, analog		; delay 0-1ms making it total 1-2ms
	rcall	delay_servo
	clr		tmp
	out		servo_p, tmp		; set servo pin low

	rjmp	main


delay_ms:						; delay 20ms
	ldi		tmp, 134
delay_ms_:
	dec		tmp
	brne		delay_ms_
	dec		delay
	brne		delay_ms
	ret

delay_servo:					; delay 0-1ms
	tst		delay
	breq	delay_done
delay_loop:
	nop
	dec		delay
	brne	delay_loop
delay_done:
	ret
