
; .include "c:/home/cc/include/longbr.inc"
.include "../../../include/longbr.inc"

; Author : HJHerbert (2007-02-07)
;
; Input : 
; r25r24r23r22 = a  the first argument
; r21r20r19r18 = b  the second (IEEE)
;
; coding :
; float a, b ;
;
; if ( a == b ) ...
; if ( a != b ) ...
;
; Output r24 = 0 if the two numbers are the same
;        r24 = 1 if a > b
;        r24 = -1 if a < b
;
; time: 30 .. 42 Cycles (including load from static float a, b ;
; it makes no sense to optimize eq / ne , because these are not useful for float
;

 	.global __eqsf2   ;
__eqsf2:

 	.global __nesf2   ;
__nesf2:

 	.global __ltsf2   ;
__ltsf2:

 	.global __gtsf2   ;
__gtsf2:

 	.global __lesf2   ;
__lesf2:

 	.global __gesf2   ;
__gesf2:
	mov	r0,r21
	eor	r0,r25
	brpl	ssb		; same sign bits
	; different signs
	; if a < b then a was negative, b was positive
	add	r25,r25	; sign of a to carry
	sbc	r24,r24	; carry to r24 (all bits) and keep in carry
	add	r24,r24	; 0 if a was positive, -2 if a was negative
	inc	r24
	ret

ssb:
	or	r25,r25
	brmi	cmprev	; compare reverse (both negative)

	cp	r22,r18	; compare a with b
	cpc	r23,r19
	cpc	r24,r20
	cpc	r25,r21
	breq	ret0
; if a < b then carry is set

ret_1_minus_double_carry:
	sbc	r24,r24	; carry to r24 (all bits) and keep in carry
				; r24 - r24 - carry
	add	r24,r24	; 0 if a > b, -2 if a < b
				; double carry
	inc	r24		; plus 1
	ret


cmprev:
	andi	r21,0x7f
	andi	r25,0x7f
	cp	r18,r22
	cpc	r19,r23
	cpc	r20,r24
	cpc	r21,r25
; if a < b then carry is set
; if -b < -a then carry is set
	brne	ret_1_minus_double_carry
ret0:
	sub	r24,r24
	ret

