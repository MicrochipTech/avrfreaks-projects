
; .include "c:/home/cc/include/longbr.inc"
.include "../../../include/longbr.inc"

; Author : HJHerbert (2007-02-07)
;
; long s ;
; unsigned long u ;
; float hs ;
; float hu ;
; ...
; hs = s ;	// 118 cycles (including load from static, save to static)
; hu = u ;	// 78 cycles (including load from static, save to static)

	round = 0 			; disable (0) or enable (1) rounding

	.data

	.text

; Input : 
; r1 = 0 ;	The C Compilers convention
; r25:r24:r23:r22 = a
; Output :
; r16:24:r23:r22 = r The result, (IEEE)
; register usage :
; r1 : 0
;
	.global __floatsisf	; Floating point addition
__floatsisf:
; Have first Argument      (u) in r25r24r23r22
	or	r25,r25
	brpl	__floatunssisf
; need to negate
	com	r25
	com	r24
	com	r23
	neg	r22
	sbci	r23,-1
	sbci	r24,-1
	sbci	r25,-1
	rcall	__floatunssisf
; No need to check if zero, a signed ulong 0 will not come to this branch of program
	subi	r25,0x80	; negate the result
	ret


	.global	__floatunssisf
__floatunssisf:
; Have first Argument      (u) in r25r24r23r22
	mov	r0,r16		; save original r16 : push r16

	mov	r16,r25
	or	r16,r24
	breq	only_a_word

more_than_16:	; r25 or r24 contains bits=1
	ldi	r16,0x7e+32
	or	r25,r25
	brne	bitshift
; have only 3 bytes
	mov	r25,r24
	mov	r24,r23
	mov	r23,r22
	ldi	r22,0
	subi	r16,8		; We have shifted 8 bits

	rjmp	bitshift

only_a_word:
; only a word

	ldi	r16,0x7e+16
	mov	r25,r23
	mov	r24,r22
	ldi	r23,0
	ldi	r22,0
; have only 3 bytes
	or	r25,r25	; Was at entry r23
	brne	bitshift
; have only 1 bytes
	mov	r25,r24	; was at entry in r22
	ldi	r24,0
	subi	r16,8		; We have shifted 8 bits

	or	r25,r25	; Was at entry r22
	brne	bitshift
; have no byte (All 4 bytes have been 0
	ldi	r25,0
	ldi	r24,0
	ldi	r23,0
	ldi	r22,0
	rjmp	result

bitshift:
	or	r25,r25	; Has mantissa already bit 7 set
	brmi	moveexp	; Mantissa has normal form
moreshift:
	dec	r16
	add	r22,r22
	adc	r23,r23
	adc	r24,r24
	adc	r25,r25
	brpl	moreshift
moveexp:
.if round
	add	r22,r22	; shift msb to carry
	adc	r23,r1	; and if carry is set, then increment r23
	adc	r24,r1
	adc	r25,r1
	brmi	roundend
; Because of rounding,the mantissa overflow
	inc	r16
	sec
	ror	r25
	ror	r24
	ror	r23
	adc	r23,r1
roundend:
.endif
	mov	r22,r23
	mov	r23,r24
	mov	r24,r25
	add	r24,r24	; throw bit 7 of mantissa
	lsr	r16		; shift exponent right, exp0 to carry
	ror	r24		; Exp0, M22...M16
; The floating point number is now in r16:r24:r23:r22
	mov	r25,r16	; sign=0, exp7...exp1
; The floating point number is now in r25:r24:r23:r22
; r25 = 0 exp7...exp1
; r24 = exp0, M22...M16
; r23 = M15 .. M8
; r22 = M7 .. M0
result:
	mov	r16,r0	; restore original r16 : pop r16
	ret
