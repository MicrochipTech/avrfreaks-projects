
; .include "c:/home/cc/include/longbr.inc"
.include "../../../include/longbr.inc"

; Author : HJHerbert (2007-02-07)
;
; float fa = -17.444;
; float fb = 306.2;
; ...
; hr1 = fa / fb ;
;
; requires 515 cycles, = 32.1875 usec at 6 MHz ;
; The original library routine needed 1453 cycles, 90.75 usec
;
; Arithmetic Errors: less than 0.3 ppm of result
;
; used rs
; r22 - a mantissa 7..0
; r23 - a mantissa 15..8
; r24 - a exp 0 mantissa 22..16
; r25 - a sign, exp 7..1
; r18 - b mantissa 7..0
; r19 - b mantissa 15..8
; r20 - b exp 0 mantissa 22..16
; r21 - b sign, exp 7..1
; r16 - exponent of b
; r14 - exponent of a
; r15 - extension of exponent of a (only 1 bit will be used)
; r6 - mantissa 7..0 of result (during division)
; r7 - mantissa 15..8 of result (during division)
; r8 - mantissa 23..16 of result (during division)

; r1 = 0 
	.data

	.text

; hr = ha / hb
; a r25 r24 r23 r22
; b r21 r20 r25 r18
; r = a / b
; Output :
; r25 r24 r23 r22 = a The result, (IEEE)
 	.global __divsf3   ; ffl ffl::operator*( ffl other )
 __divsf3:

	push	r6
	push	r7
	push	r8
	push	r16
	push	r14
	push	r15
	push	r17
	push	r18
	push	r19
	push	r20
	push	r21

; Have first Argument      (a) in r25 r24 r23 r22
; Have the second argument (b) in r21 r20 r19 r18
; unpack the exponent of a to r14 and mantissa of a to r24 r23 r22
	add	r24,r24	; Bit 7 = lowest bit of exponend a
	mov	r14,r25 	; Bit 0..6 = Bit 1..7 of exponent a
	adc	r14,r14 	; r14 = exp a
	breq	underflow

	sec			; Set Carry = M23 = 1
	ror	r24		; r24 M22..M17

; unpack the exponent of b to r16 and mantissa of b to r20 r19 r18
	add	r20,r20	; exp 0 b -> Carry
	mov	r16,r21 ; Bit 0..6 = Bit 1..7 of exponent b
	adc	r16,r16	; r16 = exp b
	breq	overflow

	sec			; Set Carry = M23
	ror	r20		; r20 Mb22..Mb17

	eor	r15,r15	; Result exponent extension
	; r15r14 = exp a (r15=0)
	; r1r16 = exp b (r1=0)
	sub	r14,r16	; subtract exponents exp r = exp a - exp b
	sbc	r15,r1	; extension (r1 = 0)
	; add to r15r14 the defined exponent offset 0x7F
	ldi	r16,0x7F	; add 0x007F = Exponent offset
	add	r14,r16	;
	adc	r15,r1	; r1 = 0
	brmi	underflow	; result = 0 
; Perform unsigned division
	cp	r14,r1
	cpc	r15,r1
	breq	underflow

	eor	r6,r6			; clear result
	eor	r7,r7
	eor	r8,r8
	ldi	r17,24			; Bit count - loop over 24 bits
compare:
;; perform this for r17 = 24 ... r17 = 1
	cp	r22,r18			; Compare a with b
	cpc	r23,r19
	cpc	r24,r20
	brcs	nosub		; Do not perform subtraction, because a < b
subtract :
	sub	r22,r18	; a - b
	sbc	r23,r19
	sbc	r24,r20
	sec			
	adc	r6,r6		; shift result left
	adc	r7,r7
	adc	r8,r8
	rjmp	shifta
nosub:
	add	r6,r6		; shift result left
	adc	r7,r7
	adc	r8,r8
shifta:
	dec	r17		; The shift and binary digit counter
	breq	mantissaok

	add	r22,r22
	adc	r23,r23
	adc	r24,r24
	brcc	compare	; check before subtract
	; Carry is set :Has shifted 1 therefore next subtract will be valid
	rjmp	subtract
mantissaok:
; new mantissa is in r8 r7 r6 (24 bits)
; new exponent is still in r14 r15	
	eor	r21,r25		; The sign of the result is now in r25, bit7
	movw	r22,r6		; Mantissa , M15...M0 in r19:r18
	mov	r24,r8		; Mantissa M23..M16 in r20
	and	r24,r24
	brmi	nonorm		; highest bit is set
; normalize the result
	add	r22,r22
	adc	r23,r23
	adc	r24,r24
	dec	r14
	breq	underflow
nonorm:
	or	r15,r15		; check the result exponent (is in r15r14)
	brne	overflow		; result > 0xff
	mov	r25,r14		; Has now result in r25 r24 r23 r22
	add	r24,r24		; bit 7..1 = M22..M16, bit 0=0
	add	r21,r21		; shift sign of result to carry
	ror	r25			; r14 = s, exp 7 .. exp 1, carry= exp0
	ror	r24			; exp0, M22..M16
	rjmp	divreturn

underflow:
	ldi	r22,0
	ldi	r23,0
	movw	r24,r22
	rjmp	divreturn
overflow:
	eor	r21,r25	; sign = sign ^ sign a
	add	r21,r21	; sign of result to carry

	ldi	r22,0
	ldi	r23,0
	ldi	r24,0x80
	ldi	r25,0x7f<<1	; exp7..exp1
	ror	r25		; sign of result from carry to bit 7
; The result after + overflow is r21 r16 r21 r20 = 7F800000
; The result after - overflow is r21 r16 r21 r20 = FF800000

; r21 = s, exp7..exp1
; r16 = exp0, M22..M16
; r21 = M15..M8
; r20 = M7..M0
divreturn:
; r21 = s, exp7..exp1
; r16 = exp0, M22..M16
; r21 = M15..M8
; r20 = M7..M0

	pop	r21
	pop	r20
	pop	r19
	pop	r18
	pop	r17
	pop	r15
	pop	r14
	pop	r16
	pop	r8
	pop	r7
	pop	r6
	ret

