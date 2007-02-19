
; .include "c:/home/cc/include/longbr.inc"
.include "../../../include/longbr.inc"

; Author : HJHerbert (2007-02-11)
;
; needs 168 cycles = 10.5 usec at 16 MHz
;
; The original library routine needed 949 cycles, 59.3125 usec
;
; Arithmetic Errors: less than 0.2 ppm of ( max(abs(a),abs(b),abs(r) )
;
; Example of usage from CPP :
; float r, a, b ;
; r = a + b
;

	round = 1 ; enable rounding
	.data

	.text

; r = a * 2 ^ i

; (See also /home/html/hjh/atmel/lib/float.htm)
; Input : 
; r1 = 0 ;	The C Compilers convention
; r25r24r23r22 = a
; r21r20 = i
; Output :
; r2524r23r22 = r The result, (IEEE)
; register usage :
;
	.global _Z8fmulpow2fi 	; r = a * 2 ^ i
_Z8fmulpow2fi:
 	push	r21
 	push	r17

; Have first Argument      (a) in r25:r24:r23:r22
; Have the second argument (i) in r21:r20

; unpack the exponent of a to r17 and mantissa of a to r24r23r22
	add	r24,r24	; -> Carry
	mov	r17,r25 	; Bit 6...0 = Bit 7...1 of exponent a
	adc	r17,r17	; r17 = exp a
	cpi	r17,0xff	; was NaN or Inf
	breq	return_NaN_or_Inf
	add	r17,r20
	adc	r21,r1	; r1 = 0
	brne	overflow_or_underflow	; Has added too much

	or	r17,r17
	breq	underflow
; r17 8 bits exp a - now exp sum
; r25 bit 7 = sign
; r24 mantissa bit 7..1 = M22..M16, bit 0 = 0
; r23r22 mantissa M15...M0
return:
	add	r25,r25	; sign of result to carry
	mov	r25,r17	; New exponent e7..e0
	ror	r25		; sign shift into exponent , r21 = s,e7...e1
	ror	r24		; Shift exp0 together with M22..M16 to bit 7, 6..0
; r25r2r25r24 = a The result, (IEEE)
	pop	r17
	pop	r21
	ret

return_NaN_or_Inf:
	ldi	r17,0xFF
	rjmp	return

overflow_or_underflow:
	or	r21,r21
	ldi	r17,0
	brmi	underflow	; r21 has bit 7 set
	ldi	r17,0xFF	; Overflow
underflow:
	ldi	r22,0
	ldi	r23,0
	ldi	r24,0
	rjmp	return

