; PWMtimer.asm
; A 2.5ms interrupt timer

; Temp pulse 0.5ms @ 2.5ms interval(s)

PWM_TEMP:
	sbi PORTC, 5
	rcall DELY_HALFMS
	cbi PORTC, 5
reti


; Fetch duration
; Starts with 0.5ms on
; Count down duration
; Increment channel #
reti



DELY_HALFMS:
	push LoopC0
	push LoopC1

	ldi LoopC1, $06
	ldi LoopC0, $80
DELY_HALFMSa:
	DELY_HALFMSb:
		dec LoopC0
		brne DELY_HALFMSb

	dec LoopC1
	brne DELY_HALFMSa

	pop LoopC1
	pop LoopC0
ret
