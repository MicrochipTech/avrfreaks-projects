Fast (but NOT speed optimized) sqrt routines for 16/8 bit integer values.

Author: Lukasz Ochman
e-mail: luke@fapr.net



.def	PosCnt	=r19
.def	RetVal	=r16
.def	TmpH	=r1
.def	TmpL	=r0
.def	InputH	=r31
.def	InputL	=r30

Sqrt16:
	clr	RetVal
	clr	TmpL
	ldi	PosCnt,(1 << 7)
	clt

Sqrt16Loop:
	mov	TmpH,PosCnt
	lsr	TmpH
	ror	TmpL
	or	TmpH,RetVal

	brts	Sqrt16Sub

	cp	InputL,TmpL
	cpc	InputH,TmpH
	brcs	Sqrt16NoSub

Sqrt16Sub:
	sub	InputL,TmpL
	sbc	InputH,TmpH
	or	RetVal,PosCnt

Sqrt16NoSub:
	bst	InputH,7
	lsl	InputL
	rol	InputH

	lsr	PosCnt
	brcc	Sqrt16Loop

	ret

; RETURN:
; return value: RetVal ( = sqrt(InputH:InputL))
; remainder:	SREG(t):InputH ( = InputH:InputL - (sqrt(InputH:InputL))^2)
;               (SREG(t) - MSB (8th) of remainder)






.def	PosCnt	=r19
.def	RetVal	=r16
.def	Input	=r30
.def	Remainder=r31
.def	TmpB	=r18

Sqrt8:
	ldi	PosCnt,4
	clr	RetVal
	clr	Remainder

Sqrt8Loop:
	lsl	Input
	rol	Remainder
	lsl	Input
	rol	Remainder

	lsl	RetVal
	mov	TmpB,RetVal
	sec
	rol	TmpB

	cp	Remainder,TmpB
	brcs	Sqrt8NoSub

	sub	Remainder,TmpB
	ori	RetVal,1

Sqrt8NoSub:
	dec	PosCnt
	brne	Sqrt8Loop

	ret


; RETURN:
; return value: RetVal ( = sqrt(Input))
; remainder:	Remainder ( = Input - (sqrt(Input))^2)
