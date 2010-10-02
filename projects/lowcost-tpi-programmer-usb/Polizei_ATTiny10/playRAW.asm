.include "tn10def.inc"

/*
attainable frequencies with 8MHz clocksource

CS	OCR1C	f [Hz]
----------------------
/256	249		125
/128	249		250
/64	249		500
/64	199		625
/32	249		1000
/32	199		1250
/16	249		2000
/16	199		2500
/16	159		3125
/8	249		4000
/8	199		5000
/8	159		6250
/4	249		8000
/4	199		10000
/4	159		12500
/2	249		16000
/2	199		20000
/2	159		25000
/1	255		31250
/1	249		32000
/1	199		40000
/1	159		50000
/1	127		62500
/1	125		64000
 */

;*****************************************************************
;
;	MACRO adpcm_soundout @0
;
; output sound to PWM
;*****************************************************************
.MACRO adpcm_soundout
;	out	OCR1A, @0
;	out	OCR0AL, @0
	out	OCR0BL, @0
.ENDMACRO
.MACRO adpcm_soundin
;	out	OCR1A, @0
	in	@0, OCR1B
.ENDMACRO



;*****************************************************************
;
;	adpcm data
;
;*****************************************************************
.DSEG
;adpcm variables
adpcm_fcounter:	.BYTE 1	; current oversampling counter
adpcm_fcountmax:	.BYTE 1	; oversampling factor (update pwm every N interrupts)
adpcm_soundsrt_lo:	.BYTE 1	; lo pointer to sound data start
adpcm_soundsrt_hi:	.BYTE 1	; hi pointer to sound data start
adpcm_soundcur_lo:	.BYTE 1	; lo pointer to sound data current
adpcm_soundcur_hi:	.BYTE 1	; hi pointer to sound data current
adpcm_soundend_lo:	.BYTE 1	; lo pointer to sound data end
adpcm_soundend_hi:	.BYTE 1	; hi pointer to sound data end
adpcm_soundflags:	.BYTE 1

adpcm_vibrato:		.BYTE 2

.CSEG
;*****************************************************************
;
;	adpcm_init
;
; init PLL, DDR, and TIMER1
; PWM frequency = 32000Hz, resolution 0..249 (not quite 8bits)
;*****************************************************************
adpcm_init:
	push	r16
	push	r17
;
; DDRB (/OC1B only works in non-PLL mode)
	sbi	DDRB, PB1			; output: PB1=OC0B

; TIMER0
	; CS0  = 001  : 1:1 = 8MHz
	; WGM0 = 1110 : Fast PWM, TOP = ICR0
	; COM0B= 10   : non-inverting mode
	; 
	ldi	r16, (1<<COM0B1) | (1<<WGM01)
	out	TCCR0A, r16
	ldi	r16, (1<<WGM02) | (1<<WGM03) | (1<<CS00)
	out	TCCR0B, r16
	ldi	r16, (1<<PSR)
	out	GTCCR, r16
	; 8MHz/200 = 40000Hz "PWM carrier signal"
;	clr	r17
;	ldi	r16, 199
	; 8MHz/250 = 32000Hz (*)
	clr	r17
	ldi	r16, 249
	; 8MHz/256 = 31250Hz
;	clr	r17
;	ldi	r16, 255
	; 8MHz/125 = 64000Hz
;	clr	r17
;	ldi	r16, 124
	out	ICR0H, r17
	out	ICR0L, r16

	out	OCR0AH, r17
	out	OCR0AL, r17
	out	OCR0BH, r17
	out	OCR0BL, r17
;
	clr	r16
	sts	adpcm_vibrato, r16
	sts	adpcm_vibrato+1, r16
;
	pop	r17
	pop	r16
	ret

;*****************************************************************
;
;	adpcm_play_looped
;
; play sound
; Z points to sound-structure
; r16/r17 points to sound-end
; Y points to playback
;*****************************************************************
adpcm_play_looped:
	rcall adpcm_play
;
	push	r20
;
	ldi	r20, 1
	sts	adpcm_soundflags, r20
;
	pop	r20
	ret

;*****************************************************************
;
;	adpcm_play
;
; play sound
; Z points to sound-structure
; r16/r17 points to sound-end
;*****************************************************************
adpcm_play:
	push	r20
	push	ZL
	push	ZH
	cli
;
; oversampling
	; 32000Hz / 16 = 2000Hz sample rate
;	ldi	r20, 16
	; 32000Hz / 8 = 4000Hz sample rate
;	ldi	r20, 8
	; 32000Hz / 5 = 6400Hz sample rate
;	ldi	r20, 5
	; 32000Hz / 4 = 8000Hz sample rate
;	ldi	r20, 4
	; 32000Hz / 2 = 16000Hz sample rate
	ldi	r20, 2
	sts	adpcm_fcountmax, r20
;
	ldi	r20, 1
	sts	adpcm_fcounter, r20
;
	sts	adpcm_soundsrt_lo, ZL
	sts	adpcm_soundsrt_hi, ZH
	sts	adpcm_soundcur_lo, ZL
	sts	adpcm_soundcur_hi, ZH
	sts	adpcm_soundend_lo, r16
	sts	adpcm_soundend_hi, r17
;
	clr	r20
	sts	adpcm_soundflags, r20
;
	pop	ZH
	pop	ZL
	pop	r20
	;fall through...

;*****************************************************************
;
;	adpcm_continue
;
; continue to play sound = enable TIMER1 OVF interrupt
;*****************************************************************
adpcm_continue:
	push	r16
;
	in	r16, TIMSK0
	ori	r16, (1<<OCIE0A)
	out	TIMSK0, r16
;	sei
;
	pop	r16
	reti

;*****************************************************************
;
;	adpcm_play_blocking
;
; play sound
; Z points to sound-structure
; r16/r17 points to sound-end
; Y points to playback
;*****************************************************************
adpcm_play_blocking:
	rcall adpcm_play
;
	push	r20
;
adpcm_play_blocking_lop_hi:
	lds	r20, adpcm_soundcur_hi
	cp	r20, r17
	brne	adpcm_play_blocking_lop_hi
adpcm_play_blocking_lop_lo:
	lds	r20, adpcm_soundcur_lo
	cp	r20, r16
	brne	adpcm_play_blocking_lop_lo
;
	pop	r20
	ret

;*****************************************************************
;
;	adpcm_stop
;
; stop to play sound = disable TIMER1 OVF interrupt
;*****************************************************************
adpcm_stop:
	cli
	push	r16
;
	in	r16, TIMSK0
	andi	r16, 255-(1<<OCIE0A)
	out	TIMSK0, r16
;
;	sei
	pop	r16
	reti

;*****************************************************************
;
;	adpcm_int
;
; service TIMER1 OVF interrupt
;*****************************************************************
adpcm_int:
; allow nested interrupts
;	sei
;
	push r16
	in	r16, SREG
	push	r16
;
; oversampling
	lds	r16, adpcm_fcounter
	dec	r16
	breq	adpcm_int_service
	sts	adpcm_fcounter, r16
;
	pop	r16
	out	SREG, r16
	pop	r16
	reti
;
; ok, it's time to do something
adpcm_int_service:
;
	push	r17
	clr	r17
;
	lds	r16, adpcm_fcountmax
	sts	adpcm_fcounter, r16
;
	lds	r16, adpcm_vibrato
	inc	r16
	sts	adpcm_vibrato, r16
	cpi	r16, 16000/20/(249-245)
	brne	adpcm_vibrato_skip
	clr	r16
	sts	adpcm_vibrato, r16
	lds	r16, adpcm_vibrato+1
	cpi	r16, 0
	breq	adpcm_vibrato_down
adpcm_vibrato_up:
	in	r16, ICR0L
	inc	r16
	out	ICR0H, r17
	out	ICR0L, r16
	cpi	r16, 249
	brne	adpcm_vibrato_skip
	ldi	r16, 0
	sts	adpcm_vibrato+1, r16
	rjmp	adpcm_vibrato_skip
adpcm_vibrato_down:
	in	r16, ICR0L
	dec	r16
	out	ICR0H, r17
	out	ICR0L, r16
	cpi	r16, 245
	brne	adpcm_vibrato_skip
	ldi	r16, 1
	sts	adpcm_vibrato+1, r16
adpcm_vibrato_skip:
;
	push	ZH
	push	ZL
;
	lds	ZL, adpcm_soundcur_lo
	lds	ZH, adpcm_soundcur_hi
	ld	r16, Z+
	adpcm_soundout	r16
	lds	r16, adpcm_soundend_lo
	cp	r16, ZL
	brne	adpcm_dontstop
	lds	r16, adpcm_soundend_hi
	cp	r16, ZH
	brne	adpcm_dontstop
	lds	r16, adpcm_soundflags
	cpi	r16, 1
	breq	adpcm_loop
	rcall adpcm_stop
	rjmp	adpcm_dontstop
adpcm_loop:
	lds	ZL, adpcm_soundsrt_lo
	lds	ZH, adpcm_soundsrt_hi
adpcm_dontstop:
	sts	adpcm_soundcur_lo, ZL
	sts	adpcm_soundcur_hi, ZH
	pop	ZL
	pop	ZH
;
	pop	r17
;
	pop	r16
	out	SREG, r16
	pop	r16
	reti
