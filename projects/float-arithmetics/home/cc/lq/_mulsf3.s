
; .include "c:/home/cc/include/longbr.inc"
.include "../../../include/longbr.inc"

; Author : HJHerbert (2007-02-07)
;
; float fa = -17.444;
; float fb = 306.2;
; ...
; hr1 = fa * fb ;
;
; needs 150 cycles = 9.375 usec at 16 MHz
; The original library routine needed 2059 cycles, 128.6875 usec
;
; Arithmetic Errors: less than 0.2 ppm of result
;
; Hint: because the mantissa is between 0.5 and ca. 0.999xxx
; the multiplication result is betweeen 0.25 and 0.999xxx
; Therefore after a multiplication no full narmalisation is neccesary
; Just to check if a single shift is neccesary or not.
;
; It is just a interpretation if the mantissa is to be considered 
; as  to be in the range 1.0 to 1.999xxx (hence the exponent offset is 127)
; or see the mantissa as 0.5 to 0.999xxx (hence the exponent offset is 126)
; the code and the numbers are the same!
;
	.data

	.text

; hr = ha * hb
; (See also /home/html/hjh/atmel/lib/float.htm)
; Input : 
; r21r20r19r18 = The first argument
; r25r24r23r22 = b the second (IEEE)
; Output :
; r25r24r25r24 = x The result, (IEEE)
; used registers
; r9r8r7r6 : The new mantissa (32 bit, the least 8 bits will be ignored on return
; r5 = 0 (because r0 is used by CPU)
 	.global __mulsf3
__mulsf3:
 ;	push	r0	; avr-gcc convention r0 = temporary
	push	r5
 	push	r6
 	push	r7
 	push	r8
 	push	r9
 	push	r18
 	push	r19
 	push	r20
 	push	r21
	push	r15
	push	r16
 	push	r17

; load the first argument (a) into r21r20r19r18

; Have first Argument      (ha) in r21r20r19r18
; Have the second argument (hb) in r25r24rmb1rmb0
; unpack the exponent of a to r16 and mantissa of a to r20r19r18
	add	r20,r20	; Bit 7 = lowest bit of exponend a
	mov	r16,r21 	; Bit 0..6 = Bit 1..7 of exponent a
	adc	r16,r16 	; r16 = exp a
	j_eq	underflow	; If the exponent was 0, then the result is 0

	sec			; Set Carry = M23
	ror	r20		; r20 M22..M17

; unpack the exponent of b to r24 and mantissa of b to r24rmb1rmb0
	add	r24,r24	; exp 0 b -> Carry
	mov	r15,r25 	; Bit 0..6 = Bit 1..7 of exponent b
	adc	r15,r15	; r15 = exp b
	j_eq	underflow	; If the exponent was 0, then the result is 0

	sec			; Set Carry = M23
	ror	r24		; r24 Mb22..Mb17

	eor	r5,r5
	eor	r17,r17	; Result exponent extension
	add	r16,r15	; Add exponents
	adc	r17,r5	; extension r17:r16 = 9 bit exponent
	subi	r16,0x7E	; subtract defined offset
	sbc	r17,r5
;	brmi	underflow	; result = 0 


; Perform unsigned mutiplication and add to
;     r20         r19         r18
;r24  A B         B C         C D
;r23  B C         C D         D E
;r22  C D         D E         E F
; each mul result will be in r01r00
; need to keep A B C for result, D for rounding, no need to keep and calculate E F
; A B C D = r09r08r07r06
	eor	r5,r5

	mul	r24,r20	; A B = r24*r20
	movw	r8,r0

	mul	r23,r19	; C D = r23*r19
	movw	r6,r0

	mul	r24,r19	; B C += r24*r19
	add	r7,r0
	adc	r8,r1
	adc	r9,r5

	mul	r23,r20	; B C += r23*r20
	add	r7,r0
	adc	r8,r1
	adc	r9,r5

	mul	r22,r20	; C D += r22*r20
	add	r6,r0
	adc	r7,r1
	adc	r8,r5
	adc	r9,r5

	mul	r24,r18	; C D += r22*r20
	add	r6,r0
	adc	r7,r1
	adc	r8,r5
	adc	r9,r5

	mul	r22,r19	; D E += r22*r19
	adc	r6,r1
	adc	r7,r5
	adc	r8,r5
	adc	r9,r5

	mul	r23,r18	; D E += r23*r18
	adc	r6,r1
	adc	r7,r5
	adc	r8,r5
	adc	r9,r5
; The mantissa is in normal form, if the highest bit is set (between 0.5 and 0.999999940395355224609375)
	brmi	mulresult
	add	r6,r6
	adc	r7,r7
	adc	r8,r8
	adc	r9,r9
	subi	r16,1	; 
	sbc	r17,r5
	brmi	underflow
mulresult:
	
;	The result mantissa is in r9r8r7
; The result exponent is in r16, r17 must be 0 , else we have an overflow
	or	r17,r17	; bit 9 of exponent is set - too big number result
	brne	overflow

	eor	r21,r25	; sign of result = sign of b ^ sign of a (bit 7)
	mov	r22,r7
	mov	r23,r8
	mov	r24,r9
	add	r24,r24	; unused highest bit M23 is always 1, no information
	mov	r25,r16	; full exponent, exp7..exp0
	add	r21,r21	; sign of result to carry
	ror	r25		; sign, exp7...exp1
	ror	r24		; exp0, M22..M16

mulreturn:
; r25 = s, exp7..exp1
; r24 = exp0, M22..M16
; r25 = M15..M8
; r24 = M7..M0
	pop	r17
	pop	r16
	pop	r15
	pop	r21
	pop	r20
	pop	r19
	pop	r18
	pop	r9
	pop	r8
	pop	r7
	pop	r6
	pop	r5
	eor	r1,r1	; avr-gcc convention r1=0
;	pop	r0	; avr-gcc convention r0 = temporary
	ret

underflow:
	ldi	r24,0
	ldi	r25,0
	movw	r24,r24
	rjmp	mulreturn

overflow:
	eor	r25,r21	; sign = sign ^ sign a
	add	r25,r25	; sign of result to carry

	ldi	r24,0
	ldi	r25,0
	ldi	r24,0x80
	ldi	r25,0x7f<<1	; exp7..exp1
	ror	r25		; sign of result from carry to bit 7
	rjmp	mulreturn
