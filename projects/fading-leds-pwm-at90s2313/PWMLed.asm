;PWM Led
;
; Written by Evert

.include "2313def.inc"

.def temp=r16

.cseg

ldi	temp,RAMEND
out	spl,temp
ser	temp
out	ddrb,temp

;clear All Registers

clr	temp
out	tccr1a,temp
out	tccr1b,temp
out	tcnt1h,temp
out	tcnt1l,temp
out	ocr1ah,temp
out	ocr1al,temp

; Set PWM Output

ldi	temp,0x81	; Non-Inverting PWM, 8-bit
out	tccr1a,temp
ldi	temp,0x02	; CK/8
out	tccr1b,temp

clr	temp

up:
	out	ocr1al,temp
	inc	temp
	inc	temp
	inc	temp
	inc	temp
	inc	temp
	rcall long_dly
	cpi temp,250
	breq down
	rjmp up

down:
	out ocr1al,temp
	dec temp
	dec temp
	dec temp
	dec temp
	dec temp
	rcall long_dly
	cpi temp,0
	breq up
	rjmp down


long_dly:
	ldi	r19,1

long_dly1:
	rcall dly
	dec	r19
	brne long_dly1
	ret

dly:
	ser	r17

dly1:
	ser	r18

dly2:
	dec	r18
	brne dly2
	dec r17
	brne dly1
	ret