;***************************************************************************
;*
;* "mpy8s" - 8x8 Bit Signed Multiplication
;*
;* This subroutine multiplies signed the two register variables mp8s and 
;* mc8s. The result is placed in registers m8sH, m8sL
;* The routine is an implementation of Booth's algorithm. If all 16 bits
;* in the result are needed, avoid calling the routine with
;* -128 ($80) as multiplicand
;*  
;* Number of words	:10 + return
;* Number of cycles	:73 + return
;* Low registers used	:None
;* High registers used  :4 (mc8s,mp8s/m8sL,m8sH,mcnt8s)	
;*
;***************************************************************************

;***** Subroutine Register Variables

.def	mc8s	=r16		;multiplicand
.def	mp8s	=r17		;multiplier
.def	m8sL	=r17		;result Low byte
.def	m8sH	=r18		;result High byte
.def	mcnt8s	=r19		;loop counter

;***** Code

mpy8s:	sub	m8sH,m8sH	;clear result High byte and carry
	ldi	mcnt8s,8	;init loop counter
m8s_1:	brcc	m8s_2		;if carry (previous bit) set
	add	m8sH,mc8s	;    add multiplicand to result High byte
m8s_2:	sbrc	mp8s,0		;if current bit set
	sub	m8sH,mc8s	;    subtract multiplicand from result High
	asr	m8sH		;shift right result High byte
	ror	m8sL		;shift right result L byte and multiplier
	dec	mcnt8s		;decrement loop counter
	brne	m8s_1		;if not done, loop more
	ret


;---------------------------

;***************************************************************************
;*
;* "div16s" - 16/16 Bit Signed Division
;*
;* This subroutine divides signed the two 16 bit numbers 
;* "dd16sH:dd16sL" (dividend) and "dv16sH:dv16sL" (divisor). 
;* The result is placed in "dres16sH:dres16sL" and the remainder in
;* "drem16sH:drem16sL".
;*  
;* Number of words	:39
;* Number of cycles	:247/263 (Min/Max)
;* Low registers used	:3 (d16s,drem16sL,drem16sH)
;* High registers used  :7 (dres16sL/dd16sL,dres16sH/dd16sH,dv16sL,dv16sH,
;*			    dcnt16sH)
;*
;***************************************************************************

;***** Subroutine Register Variables

.def	d16s	=r13		;sign register
.def	drem16sL=r14		;remainder low byte		
.def	drem16sH=r15		;remainder high byte
.def	dres16sL=r16		;result low byte
.def	dres16sH=r17		;result high byte
.def	dd16sL	=r16		;dividend low byte
.def	dd16sH	=r17		;dividend high byte
.def	dv16sL	=r18		;divisor low byte
.def	dv16sH	=r19		;divisor high byte
.def	dcnt16s	=r20		;loop counter

;***** Code

div16s:	mov	d16s,dd16sH	;move dividend High to sign register
	eor	d16s,dv16sH	;xor divisor High with sign register
	sbrs	dd16sH,7	;if MSB in dividend set
	rjmp	d16s_1
	com	dd16sH		;    change sign of dividend
	com	dd16sL		
	subi	dd16sL,low(-1)
	sbci	dd16sL,high(-1)
d16s_1:	sbrs	dv16sH,7	;if MSB in divisor set
	rjmp	d16s_2
	com	dv16sH		;    change sign of divisor
	com	dv16sL		
	subi	dv16sL,low(-1)
	sbci	dv16sL,high(-1)
d16s_2:	clr	drem16sL	;clear remainder Low byte
	sub	drem16sH,drem16sH;clear remainder High byte and carry
	ldi	dcnt16s,17	;init loop counter

d16s_3:	rol	dd16sL		;shift left dividend
	rol	dd16sH
	dec	dcnt16s		;decrement counter
	brne	d16s_5		;if done
	sbrs	d16s,7		;    if MSB in sign register set
	rjmp	d16s_4
	com	dres16sH	;        change sign of result
	com	dres16sL
	subi	dres16sL,low(-1)
	sbci	dres16sH,high(-1)
d16s_4:	ret			;    return
d16s_5:	rol	drem16sL	;shift dividend into remainder
	rol	drem16sH
	sub	drem16sL,dv16sL	;remainder = remainder - divisor
	sbc	drem16sH,dv16sH	;
	brcc	d16s_6		;if result negative
	add	drem16sL,dv16sL	;    restore remainder
	adc	drem16sH,dv16sH
	clc			;    clear carry to be shifted into result
	rjmp	d16s_3		;else
d16s_6:	sec			;    set carry to be shifted into result
	rjmp	d16s_3


;========================
