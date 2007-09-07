;* Routine for finding the sqrt(xl:xh:yl:xh)
;* 32 bit numbers

;***************************************************************************
; Input :32 bit number
;        in xl - low byte number
;           xh
;           yl
;           yh - high byte number
;
; Result :16 bit result
;         in t1l - low byte number
;            t1h - high byte number
;
; Remainder :in xl - low byte number
;               xh
;               yl
;               yh - high byte number
;***************************************************************************

.def	one	=r16
.def	zero	=r17
.def	t1l	=r18
.def	t1h	=r19

sqrt32:

	ldi	one,1
	clr	zero

	clr	zh
	ldi	zl,1

	clr	t1l
	clr	t1h

	tst	xl
	brne	sqrt1

	tst	xh
	brne	sqrt1

	tst	yl
	brne	sqrt1

	tst	yh
	brne	sqrt1

sqrt_exit:

	ret

sqrt1:

	cp	xl,zl
	cpc	xh,zh
	cpc	yl,zero
	cpc	yh,zero

	brlo	sqrt_exit

	sub	xl,zl
	sbc	xh,zh
	sbc	yl,zero
	sbc	yh,zero

	add	t1l,one
	adc	t1h,zero

	adiw	zl,2

	rjmp	sqrt1

