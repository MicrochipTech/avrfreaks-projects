
; .include "c:/home/cc/include/longbr.inc"
.include "../../../include/longbr.inc"

; Author : HJHerbert (2007-02-11)
;
; Input : 
; r25r24r23r22 = a  the first argument
;
; coding :
; float a, b ;
;
; if ( isnan(a) ) ... exponent = 0xFF
;
; Output r25:r24 = 0 if the exponent = 0xFF
;        r25:r24 = 1 if the exponent is 0..FE
;
;

 	.global isnan   ;
isnan:
	add	r24,r24	; exp0 to carry
	adc	r25,r25	; r25 = exp7...exp0
	cpi	r25,0xff	; is 0xff
	brne	return_false
; inf is not nan
	or	r24,r23
	or	r24,r22
	breq	return_false	; any bit is set,, so it is inf, but not nan
	ldi	r25,0
	ldi	r24,1		; return true
	ret

return_false:
	ldi	r25,0
	ldi	r24,0		; return false
	ret

 	.global isinf   ;
isinf:
	add	r24,r24	; exp0 to carry
	adc	r25,r25	; r25 = exp7...exp0
	sbc	r24,r24	; sign to all bits of r24
	cpi	r25,0xff	; is 0xff
	brne	return_false
; inf is not nan
	or	r24,r23
	or	r24,r22
	breq	return_false	; any bit is set
	add	r24,r24	; pos : r24 = 0 , neg r24 = -2
	dec	r24		; pos : r24 = 1 , neg r24 = -1
	mov	r25,r24
	asr	r25		; 00 or FF
	ret
