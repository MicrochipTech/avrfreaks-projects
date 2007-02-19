
; .include "c:/home/cc/include/longbr.inc"
.include "../../../include/longbr.inc"

; Author : HJHerbert (2007-02-07)
;
; float fa = -17.444;
; float fb = 306.2;
; ...
; hr1 = fa + fb ;
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

	round = 0 			; disable (0) or enable (1) rounding
	.data

	.text

; r = a + b
; (See also /home/html/hjh/atmel/lib/float.htm)
; Input : 
; r1 = 0 ;	The C Compilers convention
; r25r24r23r22 = a
; r21r20r19r18 = b 
; Output :
; r2524r23r22 = r The result, (IEEE)
; register usage :
; r0 : separated exponent of b
; r17 : separated exponent of a
; r16 : The exponent difference
; r1 : 0
;
	.global __subsf3	; Floating point subtraction
__subsf3:
 	subi	r21,0x80	; toggle sign bit of b
	.global __addsf3	; Floating point addition
__addsf3:
 	push	r21
 	push	r20
 	push	r19
 	push	r18
 	push	r17
 	push	r16

; Have first Argument      (ha) in r21r20r19r18
; Have the second argument (hb) in r25r24r23r22
; Check if both signs are equal, then add else subtract
	mov	r16,r21
	eor	r16,r25
	j_mi	subtract
; unpack the exponent of b to r0 and mantissa of b to r20r19r18
	add	r20,r20	; Bit 7 = lowest bit of exponend b to carry
	mov	r16,r21 	; Bit 6...0 = Bit 7...1 of exponent b
	adc	r16,r16 	; r16 = exp b
	j_eq	result_is_a	; result is r25r24r23r22
				; special case to correct 0.0 + 0.0
	mov	r0,r16	; r0 = exp b
	
	sec			; Set Carry = M23
	ror	r20		; r20 M22..M17

; unpack the exponent of a to r17 and mantissa of a to r24r23r22
	add	r24,r24	; -> Carry
	mov	r17,r25 	; Bit 6...0 = Bit 7...1 of exponent a
	adc	r17,r17	; r17 = exp a

	sec			; Set Carry = M23
	ror	r24		; r24 Mb22..Mb17

; Check if both exponents are same
	sub	r16,r17 	; exp b - exp a
	brcc	shift_b	; exp b > exp a : need to shift  mantissa of b, i.e. r0r21r20
; need to shift mantissa of a, i.e. r2r25r24
; case 3 + 5
	neg	r16		; The exponent difference was negative, now r16 = exp a - exp b
	cpi	r16,24	; Will shift more than mantissa precision ?
	brcs	add_shifta16
; r25 sign of result
; r17 8 bits exp a of result
; r24r23r22 mantissa of result (with bit 7 of r24=1)
; restore a, b is too small to add
	rjmp	nor24	; ready

add_shifta16:
	cpi	r16,16		; exp a - exp b, compare to 16
	brcs	add_shifta8
	subi	r16,16
.if round
	add	r18,r18		; from thrown least significant byte, highest bit to C
.endif
	mov	r18,r20		; perform 16 bit shift
	clr	r19
	clr	r20
.if round
	adc	r18,r1		; round up
	adc	r19,r1		
.endif
; no carry from r19 to r20 possible because r19 = 0
	rjmp	add_shifta_bits

add_shifta8:
	cpi	r16,8
	brcs	add_shifta_bits
	subi	r16,8
.if round
	add	r18,r18		; from thrown least significant byte, highest bit to C
.endif
	mov	r18,r19		; perform 8 bit shift
	mov	r19,r20
	clr	r20
.if round
	adc	r18,r1		; round up
	adc	r19,r1
	adc	r20,r1
.endif
add_shifta_bits:
	or	r16,r16
	breq	adda_m
	dec	r16
	lsr	r20			; shift mantissa right (3bytes, by 1 bit)
	ror	r19
	ror	r18
.if round
	adc	r18,r1			; r1 = 0 , round up if shifted 1 out
	adc	r19,r1
	adc	r20,r1
.endif
	rjmp	add_shifta_bits


adda_m:
	add	r22,r18		; a = a + b (b has been shifted right)
	adc	r23,r19
	adc	r24,r20
;	j_eq	addresult0
	brcc	nor24
	; mantissa overflow - in b
	; need to shift right mantissa b, inc exp b
	; shift mantissa, increment exponent
	; example 7.3 + 0.8
	dec	r16			; exponent difference
	lsr	r24			; 0 M22..M16
	ror	r23			
	ror	r22	
.if round
	adc	r22,r1			; r1 = 0 , round up if shifted 1 out
	adc	r23,r1
	adc	r24,r1
.endif
	inc	r17	; exp a
	breq	overflow

nor24:
; r25 sign of result
; r17 8 bits exp a of result
; r24r23r22 mantissa of result (with bit 7 of r24=1)
	add	r24,r24	; Mantissa M22..M16 to bit 7..1, (bit 0=0), Ignore M23
	add	r25,r25	; sign of a to carry
	mov	r25,r17	; New exponent
	ror	r25		; sign shift into exponent , r25 = s,e7...e1
	ror	r24		; Shift exp0 together with M22..M16 to bit 7, 6..0
; r25r24r23r22 = the result, packed (IEEE)
	rjmp	addresult	; The result is in r25r2r25r24




; need to shift mantissa of b, i.e. r0r21r20
; r16 = exponent difference , exp a - exp b
shift_b:
; case 5 + 3
	cpi	r16,24	; Will shift more than mantissa precision ?
	brcs	add_shiftb16
; restore b, a is too small to add
; r21 sign of result
; r0 8 bits exp a of result
; r20r19r18 mantissa of result (with bit 7 of r24=1)
	rjmp	nor20

add_shiftb16:
	cpi	r16,16		; exp b - exp a, compare to 16
	brcs	add_shiftb8
	subi	r16,16
.if round
	adc	r22,r22		; shift highest bit os least significant byte to carry
.endif
	mov	r22,r24		; perform 16 bit shift
	clr	r23
	clr	r24
.if round
	adc	r22,r1		; round up
	adc	r23,r1
.endif
; adc r24,r1 not needed, becaue r23 = 0

	rjmp	add_shiftb_bits

add_shiftb8:
	cpi	r16,8
	brcs	add_shiftb_bits
	subi	r16,8
.if round
	adc	r22,r22		; shift highest bit os least significant byte to carry
.endif
	mov	r22,r23		; perform 8 bit shift
	mov	r23,r24
	clr	r24
.if round
	adc	r22,r1		; round up
	adc	r23,r1
	adc	r24,r1
.endif
add_shiftb_bits:
	or	r16,r16
	breq	addb_m

	dec	r16			; exponent difference
	lsr	r24			; shift mantissa right (3bytes, by 1 bit)
	ror	r23
	ror	r22
.if round
	adc	r22,r1		; round up
	adc	r23,r1
	adc	r24,r1
.endif
	rjmp	add_shiftb_bits


addb_m:
	add	r18,r22		; Add a to b, (b has been shifted right)
	adc	r19,r23
	adc	r20,r24

	brcc	nor20
	; mantissa overflow - in a
	; need to shift right mantissa a, inc exp a
	; shift mantissa, increment exponent
	; example 7.3 + 0.8
	dec	r16			; exponent difference
	lsr	r20
	ror	r19
	ror	r18
.if round
	adc	r22,r1			; r1 = 0 , round up if shifted 1 out
	adc	r23,r1
	adc	r24,r1
.endif
	inc	r0	; exp a
	breq	overflow

nor20:
; r21 sign of result
; r0 8 bits exp a of result
; r20r19r18 mantissa of result (with bit 7 of r24=1)
	add	r20,r20	; Mantissa M22..M16 to bit 7..1, (bit 0=0)
	add	r21,r21	; sign of b to carry
	mov	r21,r0	; New exponent
	ror	r21		; sign shift into exponent , r25 = s,e7...e1
	ror	r20		; Shift exp0 together with M22..M16 to bit 7, 6..0
; r21r20r19r18 = the result, packed
	movw	r24,r20
	movw	r22,r18
; r25r2r25r24 = a The result, (IEEE)
	rjmp	addresult	; The result is in r25r2r25r24




overflow:
	ldi	r25,0x7F	; sign = 0 , Exponent = 0xFF
	ldi	r24,0x01	; e0, M22..M16
	ldi	r23,0	; M15..M8
	ldi	r22,0	; M7..M0
	rjmp	addresult

addresult0:
	ldi	r25,0	; sign = 0 , Exponent = 0x00
	ldi	r24,0	; e0, M22..M16
;	ldi	r23,0	; M15..M8
;	ldi	r22,0	; M7..M0
	movw	r22,r24

result_is_a:	; result is r25r24r23r22
addresult:

addreturn:

	pop	r16
	pop	r17
	pop	r18
	pop	r19
	pop	r20
	pop	r21
	ret
; result is r25r24r23r22




; The signs are different, need to subtract
subtract:
; unpack the exponent of b to r0 and mantissa of b to r20r19r18
	add	r20,r20	; Bit 7 = lowest bit of exponend b
	mov	r16,r21 ; Bit 0..6 = Bit 1..7 of exponent b
	adc	r16,r16 ; r16 = exp b
	mov	r0,r16	; r0 = exp b

	sec			; Set Carry = M23
	ror	r20		; r20 M22..M17

; unpack the exponent of a to r17 and mantissa of a to r24r23r22
	add	r24,r24	; -> Carry
	mov	r17,r25 ; Bit 0..6 = Bit 1..7 of exponent a
	adc	r17,r17	; r17 = exp a

	sec			; Set Carry = M23
	ror	r24		; r24 Mb22..Mb17

; Calculate the difference of the exponents
	sub	r16,r17 ; exp b - exp a

; decide which value is greater, consider 0.9001 > 0.90009
	cp	r18,r22	; compare mantissa b against mantissa a
	cpc	r19,r23
	cpc	r20,r24
	cpc	r0,r17	; compare exp b against exp a
	; Carry = Clear, if b >= a,  r0r20r19r18 >= r2r24r23r22
	; Then we need to shift a r24r23r22
	j_cc	sub_shift_a	; need to shift  mantissa of b, i.e. r0r21r20
; case 3 - 5
	neg r16
; need to shift mantissa of a, i.e. r2r25r24
	cpi	r16,24	; Will shift more than mantissa precision ?
	j_cc	nor24	; r25 sign of result
				; r17 8 bits exp a of result
				; r24r23r22 mantissa of result (with bit 7 of r24=1)
				; ready

sub_shifta16:
	cpi	r16,16		; exp a - exp b, compare to 16
	brcs	sub_shifta8
	subi	r16,16
.if round
	add	r18,r18		; from thrown least significant byte, highest bit to C
.endif
	mov	r18,r20		; perform 16 bit shift
	clr	r19
	clr	r20
.if round
	adc	r18,r1		; round up
	adc	r19,r1		
.endif
; no carry from r19 to r20 possible because r19 = 0
	rjmp	sub_shifta_bits

sub_shifta8:
	cpi	r16,8
	brcs	sub_shifta_bits
	subi	r16,8
.if round
	add	r18,r18		; from thrown least significant byte, highest bit to C
.endif
	mov	r18,r19		; perform 8 bit shift
	mov	r19,r20
	clr	r20
.if round
	adc	r18,r1		; round up
	adc	r19,r1
	adc	r20,r1
.endif
sub_shifta_bits:
	or	r16,r16
	breq	suba_m
	dec	r16
	lsr	r20			; shift mantissa right (3bytes, by 1 bit)
	ror	r19
	ror	r18
.if round
	adc	r18,r1			; r1 = 0 , round up if shifted 1 out
	adc	r19,r1
	adc	r20,r1
.endif
	rjmp	sub_shifta_bits


suba_m:
	sub	r22,r18		; a = a - b (b has been shifted right)
	sbc	r23,r19
	sbc	r24,r20
; Check if result is 0
	mov	r16,r22
	or	r16,r23
	or	r16,r24
	j_eq	addresult0
; shift result left, until highest bit is set
sub_normb1:
	or	r24,r24
	brne	sub_normb2
	mov	r24,r23
	mov	r23,r22
	ldi	r22,0
	subi	r17,8		; exp b (b was greater)
	brcc	sub_normb1
	rjmp	addresult0	; before subi, r17 < 7 --> mantissa cannot be normed - underflow
		;
sub_normb2:
	or	r17,r17			; exp b
	j_eq	addresult0	; underflow
	or	r24,r24
	brmi	sub_normb4	; done, mantissa is in norm form
sub_normb3:
	dec	r17
	j_eq	addresult0	; underflow
	add	r22,r22
	adc	r23,r23
	adc	r24,r24
	brpl	sub_normb3
sub_normb4:
; r0 8 bits exp a - now exp sum
; r24r23r22 mantissa 
	add	r24,r24	; Mantissa of result M22..M16 to bit 7..1, (bit 0=0)
	add	r25,r25	; sign of b to carry
	mov	r25,r17	; New exponent e7..e0
	ror	r25		; sign shift into exponent , r25 = s,e7...e1
	ror	r24		; Shift exp0 together with M22..M16 to bit 7, 6..0
; r25r24r23r22 = the result, packed (IEEE)
	rjmp	addresult	; The result is in r25r2r25r24 - has sign of a, was : |a| > |b|

; need to shift mantissa of b, i.e. r0r21r20
; r16 = exponent difference , exp a - exp b
sub_shift_a:
; case 5 - 3 ( or -5 + 3) ( or 5 + (-3))
	cpi	r16,24	; Will shift more than mantissa precision ?
				; r16 >= 24 : then Carry = 0
	j_cc	nor20	; r21 sign of result
				; r0 8 bits exp a of result
				; r20r19r18 mantissa of result (with bit 7 of r24=1)

; sub_shiftb16:
	cpi	r16,16		; exp b - exp a, compare to 16
	brcs	sub_shiftb8
	subi	r16,16
.if round
	adc	r22,r22		; shift highest bit os least significant byte to carry
.endif
	mov	r22,r24		; perform 16 bit shift
	clr	r23
	clr	r24
.if round
	adc	r22,r1		; round up
	adc	r23,r1
.endif
; adc r24,r1 not needed, because r23 = 0

	rjmp	sub_shiftb_bits

sub_shiftb8:
	cpi	r16,8
	brcs	sub_shiftb_bits
	subi	r16,8
.if round
	adc	r22,r22		; shift highest bit of least significant byte to carry
.endif
	mov	r22,r23		; perform 8 bit shift
	mov	r23,r24
	clr	r24
.if round
	adc	r22,r1		; round up
	adc	r23,r1
	adc	r24,r1
.endif
sub_shiftb_bits:
	or	r16,r16
	breq	subb_m

	dec	r16			; exponent difference
.if round
	lsr	r24			; shift mantissa right (3bytes, by 1 bit)
.endif
	ror	r23
	ror	r22
.if round
	adc	r22,r1		; round up
	adc	r23,r1
	adc	r24,r1
.endif
	rjmp	sub_shiftb_bits


subb_m:
	sub	r18,r22		; a = a - b, (b has been shifted right)
	sbc	r19,r23
	sbc	r20,r24
; Check if result is 0
	mov	r16,r18
	or	r16,r19
	or	r16,r20
	j_eq	addresult0
; shift result left, until highest bit is set
	ldi	r16,8
sub_nor19:
	or	r20,r20
	brne	sub_nor20
	mov	r20,r19
	mov	r19,r18
	clr	r18
	sub	r0,r16
	brcc	sub_nor19
	rjmp	addresult0	; before subi, r17 < 7 --> mantissa cannot be normed - underflow
		;
sub_nor20:
	or	r0,r0
	j_eq	addresult0	; underflow
	or	r20,r20
	brmi	sub_norma4	; done, mantissa is in norm form
sub_norma3:
	dec	r0
	j_eq	addresult0
	add	r18,r18
	adc	r19,r19
	adc	r20,r20
	brpl	sub_norma3
sub_norma4:
; r17 8 bits exp a - now exp sum
; r24r23r22 mantissa 
	add	r20,r20	; Mantissa of result M22..M16 to bit 7..1, (bit 0=0)
	add	r21,r21	; sign of greater value to carry
	mov	r21,r0	; New exponent e7..e0
	ror	r21		; sign shift into exponent , r21 = s,e7...e1
	ror	r20		; Shift exp0 together with M22..M16 to bit 7, 6..0
; r21r20r19r18 = the result, packed
	movw	r24,r20
	movw	r22,r18
; r25r2r25r24 = a The result, (IEEE)
	rjmp	addresult	; The result is in r25r2r25r24

