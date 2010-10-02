.include "tn10def.inc"

.org 0
;Interrupt vector table
rjmp reset_vector	;RESET

//.org OC0Aaddr		; Interrupt vector for timer1 output compare match A
rjmp dummy_vector	;INT0
rjmp dummy_vector	;PCINT0
rjmp dummy_vector	;TIM0_CAPT
rjmp adpcm_int		;TIM0_OVF
rjmp dummy_vector	;TIM0_COMPA
rjmp dummy_vector	;TIM0_COMPB
rjmp dummy_vector	;ANA_COMP
rjmp dummy_vector	;WDT
rjmp dummy_vector	;VLM
rjmp dummy_vector	;ADC

dummy_vector:
	reti

;Main code
reset_vector:
; SP
	ldi r16, high(RAMEND)				; SP
	out SPH, r16
	ldi r16, low(RAMEND)				; SP
	out SPL, r16

; set clock to 8MHz
	ldi	r16, 0xd8
	out	CCP, r16
	ldi	r16, 0
	out	CLKPSR, r16

	sbi	DDRB, DDB2		; PB2: LED 1
	sbi	DDRB, DDB3		; PB3: LED 2

	rcall adpcm_init		; PB1: Sound out (OC0B)

	ldi	r19, 4
playlop:
	cbi	PORTB, PORTB2
	sbi	PORTB, PORTB3

	ldi	ZL, low(adpcm_sound1*2)			; CSEG address
	ldi	ZH, 0x40 + high(adpcm_sound1*2)
	ldi	r16, low(adpcm_sound1_end*2)		; CSEG address
	ldi	r17, 0x40 + high(adpcm_sound1_end*2)
	rcall adpcm_stop
	rcall adpcm_play_looped

	ldi	r16,19
	rcall delay

	sbi	PORTB, PORTB2
	cbi	PORTB, PORTB3

	ldi	ZL, low(adpcm_sound2*2)			; CSEG address
	ldi	ZH, 0x40 + high(adpcm_sound2*2)
	ldi	r16, low(adpcm_sound2_end*2)		; CSEG address
	ldi	r17, 0x40 + high(adpcm_sound2_end*2)
	rcall adpcm_stop
	rcall adpcm_play_looped
	
	ldi	r16,19
	rcall delay

	dec	r19
	brne	playlop

	rcall adpcm_stop

	cbi	PORTB, PORTB2
	cbi	PORTB, PORTB3

idle:
	ldi r16, (1<<SE) | (1<<SM1)	; Enable sleep: powerdown
	out SMCR, r16
	sleep
	rjmp idle

delay:
	push	r24
	push	r25
	push	r26
delaylop1:

	clr	r24
	clr	r25
	clr	r26
delaylop2:
	subi	r24, 1
	sbc	r25, r26
	brne	delaylop2

	dec	r16
	brne	delaylop1

	pop	r26
	pop	r25
	pop	r24
	ret

.include "playRAW.asm"	

.CSEG
adpcm_sound1:
	.include "pol_low.mix.16000.inc"
adpcm_sound1_end:

adpcm_sound2:
	.include "pol_high.mix.16000.inc"
adpcm_sound2_end:
	nop



