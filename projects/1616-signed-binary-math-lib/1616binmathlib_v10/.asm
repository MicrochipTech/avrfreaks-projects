
; 16.16 signed binary math lib for small RAM-less AVR's V1.0
; ALL code by Rolf R Bakke,  September 2008

 

;	-*- 16.16 signed binary addition -*-


;	Register usage:

;	   sum          augend        addend  
;	r0:r1.r2:r3 = r0:r1.r2:r3 + r4:r5.r6:r7

b16_add:add r3,r7
	adc r2,r6
	adc r1,r5
	adc r0,r4

	ret





;	-*- 16.16 signed binary subtraction -*-


;	Register usage:

;	difference      minuend     subtrahend
;	r0:r1.r2:r3 = r0:r1.r2:r3 - r4:r5.r6:r7

b16_sub:sub r3,r7
	sbc r2,r6
	sbc r1,r5
	sbc r0,r4

	ret





;	-*- 16.16 signed binary multiply -*-


;	Register usage:

;	  product     multiplicand   multiplier
;	r0:r1.r2:r3 = r4:r5.r6:r7 * r8:r9.r10:r11


;	Internal register usage, These is free to use in your code, but they are destroyed by this routine.

;	r12:     result sign
;	r13:r14: product extender (adds 16 LSB to the product)
;	r15:r16: multiplicand extender (adds 16 MSB to the multiplicand)
;	r17:     general use



b16_mul:rcall b16_pre	;calculate result sign and convert operands to their absolute values

	clr r0		;clear the product
	clr r1
	clr r2
	clr r3
	clr r13		;and the extended part of the product
	clr r14

	clr r15		;clear the extended part of the multiplicand
	clr r16	

	clt		;clear the multiplicand overflow flag 

	ldi r17,32	;there is 32 bits

b16_mu2:lsr r8		;get the LSB of multiplier
	ror r9
	ror r10
	ror r11

	brcc b16_mu3	;one?

	brts b16_mu4	;yes: if we get to here, and the multiplicand overflow flag is set, we have product overflow

	add r14,r7	;product = product + multiplicand
	adc r13,r6
	adc r3,r5
	adc r2,r4
	adc r1,r16
	adc r0,r15

	brmi b16_mu4	;product overflow?
	brcs b16_mu4

b16_mu3:lsl r7		;shift multiplicand to the left
	rol r6
	rol r5
	rol r4
	rol r16
	rol r15

	brcc b16_mu5	;multiplicand overflow?

	set		;yes, set the multiplicand overflow flag

b16_mu5:dec r17		;more bits?
	brne b16_mu2	;yes


	tst r13		;round off the product
	brpl b16_mu6

	ldi r17,1
	add r3,r17
	clr r17
	adc r2,r17
	adc r1,r17
	adc r0,r17

	brmi b16_mu4	;product overflow?

b16_mu6:tst r12		;is result sign negative? (see b16_pre)
	brpl b16_mu1	;no
	
	ldi r17,1	;yes: negate product
	sub r14,r17
	clr r17
	sbc r13,r17
	sbc r3,r17
	sbc r2,r17
	sbc r1,r17
	sbc r0,r17
	
	ldi r17,0xff
	eor r0,r17
	eor r1,r17
	eor r2,r17
	eor r3,r17
	eor r13,r17
	eor r14,r17

b16_mu1:clc		;return with no error
	ret

b16_mu4:sec		;yes: set error flag and return
	ret




;	-*- 16.16 signed binary divide -*-


;	Register usage:

;	 quotient      dividend       divisor
;	r0:r1.r2:r3 = r4:r5.r6:r7 / r8:r9.r10:r11

	
;	Internal register usage, These is free to use in your code, but they are destroyed by this routine.

;	r12:     result sign
;	r13,r16: general use


b16_div:rcall b16_pre	;calculate result sign and convert operands to their absolute values

	mov r0,r4	;count # of used bits in dividend
	mov r1,r5
	mov r2,r6
	mov r3,r7

	rcall b16_bc

	mov r13,r16	;r13 = # of used bits in dividend

	
	mov r0,r8	;count # of used bits in divisor
	mov r1,r9
	mov r2,r10
	mov r3,r11

	rcall b16_bc

	clr r0		;clear quotient
	clr r1
	clr r2
	clr r3


	tst r16		;check if divisor is zero
	brne b16_di4

	sec		;yes: return with error and quotient = 0
	ret	


b16_di4:tst r13		;check if dividend is zero
	brne b16_di5

	clc		;yes: return with no error and quotient = 0
	ret


b16_di5:sub r13,r16	;do we need to align the divisor to the dividend?
	ldi r16,17	;there is 17 bits
	brmi b16_di9	;no
	breq b16_di9	;no

	mov r16,r13	;yes: shift divisor left r13 times
b16_di7:lsl r11
	rol r10
	rol r9
	rol r8
	dec r16
	brne b16_di7

	ldi r16,17	;there is 17 bits
	add r16,r13	;if we scaled the divisor, we need to scale the quotient as well

	
			;(From now on the dividend renames to the remainder)

b16_di9:sub r7,r11	;remainder = remainder - divisor (does divisor fit inside remainder?)
	sbc r6,r10
	sbc r5,r9
	sbc r4,r8

	sec

	brpl b16_di10	;yes	
	
	add r7,r11	;no: remainder = remainder + divisor (restore remainder)
	adc r6,r10
	adc r5,r9
	adc r4,r8

	clc

b16_di10:rol r3		;shift result into quotient
	rol r2
	rol r1
	rol r0

	brmi b16_di17	;qoutient overflow?

	lsl r7		;shift remainder left
	rol r6
	rol r5
	rol r4

	dec r16		;more bits?
	brne b16_di9	;yes
	
	
	sub r7,r11	;round off the quotient
	sbc r6,r10
	sbc r5,r9
	sbc r4,r8

	brmi b16_di15	
	
	ldi r16,1
	add r3,r16
	clr r16
	adc r2,r16
	adc r1,r16
	adc r0,r16
	
	brmi b16_di17	;qoutient overflow?
	
b16_di15:tst r12	;is result sign negative? (see b16_pre)
	brpl b16_di13	;no
	
	ldi r16,1	;yes: negate quotient
	sub r3,r16
	clr r16
	sbc r2,r16
	sbc r1,r16
	sbc r0,r16
	
	ldi r16,0xff
	eor r0,r16
	eor r1,r16
	eor r2,r16
	eor r3,r16

b16_di13:clc		;return with no error
	ret

b16_di17:sec		;yes: set error flag and return
	ret



;---- subroutines

b16_bc:	ldi r16,33	;bit counter
	sec
b16_bc1:dec r16
	rol r3
	rol r2
	rol r1
	rol r0
	brcc b16_bc1
	ret



b16_pre:mov r12,r4	;r12 bit 7 = sign of OP1 EOR sign of OP2
	eor r12,r8


	tst r4		;OP1 = ABS(OP1)
	brpl b16_pr1

	ldi r16,0xff
	eor r4,r16
	eor r5,r16
	eor r6,r16
	eor r7,r16

	ldi r16,1
	add r7,r16
	clr r16
	adc r6,r16
	adc r5,r16
	adc r4,r16


b16_pr1:tst r8		;OP2 = ABS(OP2)
	brpl b16_pr2

	ldi r16,0xff
	eor r8,r16
	eor r9,r16
	eor r10,r16
	eor r11,r16

	ldi r16,1
	add r11,r16
	clr r16
	adc r10,r16
	adc r9,r16
	adc r8,r16

b16_pr2:ret

