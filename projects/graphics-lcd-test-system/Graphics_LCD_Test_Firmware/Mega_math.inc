;********* ATmega Series Math Subroutines **************************************
;Mega_math.inc
;3-18-2007 CFS


;for ATmega series with hardware multiplier

;includes BCD/binary and binary/BCD conversions
;and improved 8 bit interpolation

;changed interp2D and interp3D to use Y pointer
;added interp2DM

;updated sort3X16 to prevent corrupted data during interrupt

;*******************************************************************************
;* FUNCTION
;*	mpy8u
;* DESCRIPTION
;*	Unsigned multiplication of two 8 bit numbers with 16 bit result.
;* USAGE
;*	r18:r17 = r17 * r16
;* STATISTICS
;*	Cycles:		3 + ret
;*	Words:		3 + ret
;*	Register usage: r0 to r1 and r16 to r18
;* NOTE
;*	For compatibility with some older routines that may call the original
;*	8 bit multiplication routine.
;*
;*	Result low byte and the multiplier share the same register.
;*	This causes the multiplier to be overwritten by the result.
;*
;*******************************************************************************

mpy8u:		mul	r17,r16
		mov	r18,r1
		mov	r17,r0
		ret





;*******************************************************************************
;*
;* FUNCTION
;*	mpy16u
;* DESCRIPTION
;*	Unsigned multiplication of two 16 bit numbers with 32 bit result.
;* USAGE
;*	r21:r20:r19:r18 = r19:r18 * r17:r16
;* STATISTICS
;*	Cycles:		18 + ret
;*	Words:		14 + ret
;*	Register usage: r0 to r2 and r16 to r23 (11 registers)
;* NOTE
;*	Modified for compatibility with older routines
;*	Code is based on mul16x16_32
;*	Same register usage as original mpy16u
;*
;*******************************************************************************

mpy16u:
		clr	r2
		movw	r23:r22,r19:r18	;move ah:al out of the way
		mul	r23,r17		;ah * bh
		movw	r21:r20,r1:r0
		mul	r22,r16		;al * bl
		movw	r19:r18,r1:r0
		mul	r23,r16		;ah * bl
		add	r19,r0
		adc	r20,r1
		adc	r21,r2
		mul	r17,r22		;bh * al
		add	r19,r0
		adc	r20,r1
		adc	r21,r2
		ret




;*******************************************************************************
;*
;* FUNCTION
;*	div8u
;* DESCRIPTION
;*	Unsigned division of two 8 bit numbers with 8 bit result.
;* USAGE
;*	r16 = r16/r17  with remainder in r15
;* STATISTICS
;*	Cycles:		97 + ret (12 usec)
;*	Words:		14 + ret
;*	Register usage: r15 to r18
;*
;*
;******************************************************************************

;***** Subroutine Register Variables

.def	drem8u		=r15		;remainder
.def	dres8u		=r16		;result
.def	dd8u		=r16		;dividend
.def	dv8u		=r17		;divisor
.def	dcnt8u		=r18		;loop counter

;***** Code

div8u:		sub	drem8u,drem8u	;clear remainder and carry
		ldi	dcnt8u,9	;init loop counter
d8u_1:		rol	dd8u		;shift left dividend
		dec	dcnt8u		;decrement counter
		brne	d8u_2		;if done
		ret			;return
d8u_2:		rol	drem8u		;shift dividend into remainder
		sub	drem8u,dv8u	;remainder = remainder - divisor
		brcc	d8u_3		;if result negative
		add	drem8u,dv8u	;restore remainder
		clc			;clear carry to be shifted into result
		rjmp	d8u_1		;else
d8u_3:		sec			;set carry to be shifted into result
		rjmp	d8u_1




;*******************************************************************************
;*
;* FUNCTION
;*	div16u
;* DESCRIPTION
;*	Unsigned division of two 16 bit numbers with 16 bit result.
;* USAGE
;*	r17:r16 = r17:r16/r19:r18  with remainder in r15:r14
;* STATISTICS
;*	Cycles:		235-251 (31.4 usec)
;*	Words :		19 + ret
;*	Register usage: r14 to r20
;*
;*
;******************************************************************************


;***** Subroutine Register Variables

.def	drem16uL	=r14
.def	drem16uH	=r15
.def	dres16uL	=r16
.def	dres16uH	=r17
.def	dd16uL		=r16
.def	dd16uH		=r17
.def	dv16uL		=r18
.def	dv16uH		=r19
.def	dcnt16u		=r20

;***** Code

div16u:		clr	drem16uL	;clear remainder Low byte
		sub	drem16uH,drem16uH	;clear remainder High byte and carry
		ldi	dcnt16u,17	;init loop counter
d16u_1:		rol	dd16uL		;shift left dividend
		rol	dd16uH
		dec	dcnt16u		;decrement counter
		brne	d16u_2		;if done
		ret			;return
d16u_2:		rol	drem16uL	;shift dividend into remainder
		rol	drem16uH
		sub	drem16uL,dv16uL	;remainder = remainder - divisor
		sbc	drem16uH,dv16uH	
		brcc	d16u_3		;if result negative
		add	drem16uL,dv16uL	;restore remainder
		adc	drem16uH,dv16uH
		clc			;clear carry to be shifted into result
		rjmp	d16u_1		;else
d16u_3:		sec			;set carry to be shifted into result
		rjmp	d16u_1



;*******************************************************************************
;*
;* FUNCTION
;*	div24u
;* DESCRIPTION
;*	Unsigned division of two 24 bit numbers with 24 bit result.
;* USAGE
;*	r18:r17:r16 = r18:r17:r16/r21:r20:r19  with remainder in r15:r14:r13
;* STATISTICS
;*	Cycles:		460 + ret (58 usec)
;*	Words:		24 + ret
;*	Register usage: r13 to r22
;*
;*
;******************************************************************************

;***** Subroutine Register Variables


.def	drem24uL	=r13
.def	drem24uH	=r14
.def	drem24u3	=r15
.def	dres24uL	=r16
.def	dres24uH	=r17
.def	dres24u3	=r18
.def	dd24uL		=r16
.def	dd24uH		=r17
.def	dd24u3		=r18
.def	dv24uL		=r19
.def	dv24uH		=r20
.def	dv24u3		=r21 
.def	dcnt24u		=r22

;***** Code

div24u:		clr	drem24uL	;clear remainder low byte
		clr	drem24uH	;clear remainder high byte
		sub	drem24u3,drem24u3	;clear remainder 3rd byte and carry
		ldi	dcnt24u,25	;init loop counter
d24u_1:		rol	dd24uL		;shift left dividend
		rol	dd24uH
		rol	dd24u3
		dec	dcnt24u		;decrement counter
		brne	d24u_2		;if done
		ret			;return
d24u_2:		rol	drem24uL	;shift dividend carry into remainder
		rol	drem24uH
		rol	drem24u3
		sub	drem24uL,dv24uL	;remainder = remainder - divisor
		sbc	drem24uH,dv24uH
		sbc	drem24u3,dv24u3
		brcc	d24u_3		;if result negative
		add	drem24uL,dv24uL	;restore remainder
		adc	drem24uH,dv24uH
		adc	drem24u3,dv24u3
		clc			;clear carry to be shifted into result
		rjmp	d24u_1		;else
d24u_3:		sec			;set carry to be shifted into result
		rjmp	d24u_1



;*******************************************************************************
;*
;* FUNCTION
;*	bin2BCD16
;* DESCRIPTION
;*	Converts 16 bit number to a 5 digit packed BCD number represented by
;*	3 bytes. MSD of 5 digit BCD is in lower nibble of 3rd byte.
;* USAGE
;*	r15:r14:r13 (packed BCD) <-  r17:r16 (binary)
;* STATISTICS
;*	Cycles:		751-768 + ret
;*	Words:		25 + ret
;*	Register usage: r13 to r19 and Z
;*
;*
;******************************************************************************;


;***** Subroutine Register Variables

.equ	AtBCD0		=13			;address of tBCD0
.equ	AtBCD2		=15			;address of tBCD2

.def	tBCD0		=r13			;BCD value digits 1 and 0
.def	tBCD1		=r14			;BCD value digits 3 and 2
.def	tBCD2		=r15			;BCD value digit 4
.def	fbinL		=r16			;binary value Low byte
.def	fbinH		=r17			;binary value High byte
.def	cnt16a		=r18			;loop counter
.def	tmp16a		=r19			;temporary value

;***** Code

bin2BCD16:
		ldi	cnt16a,16	;Init loop counter
		clr	tBCD2		;clear result (3 bytes)
		clr	tBCD1
		clr	tBCD0
		clr	ZH		;clear ZH
bBCDx_1:	lsl	fbinL		;shift input value
		rol	fbinH		;through all bytes
		rol	tBCD0
		rol	tBCD1
		rol	tBCD2
		dec	cnt16a		;decrement loop counter
		brne	bBCDx_2		;if counter not zero
		ret			;return

bBCDx_2:	ldi	r30,AtBCD2+1	;Z points to result MSB + 1
bBCDx_3:
		ld	tmp16a,-Z	;get (Z) with pre-decrement
		subi	tmp16a,-$03	;add 0x03
		sbrc	tmp16a,3	;if bit 3 not clear
		st	Z,tmp16a	;store back
		ld	tmp16a,Z	;get (Z)
		subi	tmp16a,-$30	;add 0x30
		sbrc	tmp16a,7	;if bit 7 not clear
		st	Z,tmp16a	;store back
		cpi	ZL,AtBCD0	;done all three?
		brne	bBCDx_3		;loop again if not
		rjmp	bBCDx_1




;*******************************************************************************
;*
;* FUNCTION
;*	bin2BCD8
;* DESCRIPTION
;*	Converts 8 bit number to a 2 digit packed BCD number.
;* USAGE
;*	r16 (packed BCD) <-  r16 (binary)
;* STATISTICS
;*	Cycles:		5-50 + ret
;*	Words:		7 + ret
;*	Register usage: r16 to r17
;*
;*
;******************************************************************************;


;***** Subroutine Register Variables

.def	fbin		=r16		;8-bit binary value
.def	tBCDL		=r16		;packed BCD result 
.def	tBCDH		=r17		;temporary BCD high digit

;***** Code

bin2bcd8:
		clr	tBCDH		;clear result MSD
bBCD8_1:	subi	fbin,10		;input = input - 10
		brcs	bBCD8_2		;abort if carry set
		subi	tBCDH,-$10	;tBCDH = tBCDH + 10
		rjmp	bBCD8_1		;loop again
bBCD8_2:	subi	fbin,-10	;compensate extra subtraction
		add	fbin,tBCDH
		ret



;*******************************************************************************
;*
;* FUNCTION
;*	BCD2bin16
;* DESCRIPTION
;*	Converts 5 digit BCD number represented by 3 bytes to a 16 bit
;*	binary number. MSD of 5 digit BCD must be in lower nibble of 3rd byte.
;* USAGE
;*	r16:r15 (binary <- r19:r18:r17 (packed BCD)
;* STATISTICS
;*	Cycles:		108 + ret
;*	Words:		30 + ret
;*	Register usage: r13 to r20
;* NOTE
;*	Let "abcde" denote the 5-digit number. The conversion is done by
;*	computing the formula: 10(10(10(10a+b)+c)+d)+e.
;*	The subroutine "mul10a"/"mul10b" does the multiply-and-add operation
;*	which is repeated four times during the computation
;*
;******************************************************************************;


;***** "mul10a"/"mul10b" Subroutine Register Variables

.def	copyL		=r13		;temporary register
.def	copyH		=r14		;temporary register
.def	mp10L		=r15		;Low byte of number to be multiplied by 10
.def	mp10H		=r16		;High byte of number to be multiplied by 10
.def	adder		=r20		;value to add after multiplication

;***** Code

mul10a:					;multiplies "mp10H:mp10L" with
					;10 and adds "adder" high nibble
		swap	adder
mul10b:					;multiplies "mp10H:mp10L" with
					;10 and adds "adder" low nibble
		mov	copyL,mp10L	;make copy
		mov	copyH,mp10H
		lsl	mp10L		;multiply original by 2
		rol	mp10H
		lsl	copyL		;multiply copy by 2
		rol	copyH
		lsl	copyL		;multiply copy by 2 (4)
		rol	copyH
		lsl	copyL		;multiply copy by 2 (8)
		rol	copyH
		add	mp10L,copyL	;add copy to original
		adc	mp10H,copyH
		andi	adder,0x0f	;mask away upper nibble of adder
		add	mp10L,adder	;add lower nibble of adder
		brcc	m10_1		;if carry not cleared
		inc	mp10H		;inc high byte
m10_1:		ret

;***** Main Routine Register Variables

.def	tbinL		=r15		;Low byte of binary result (same as mp10L)
.def	tbinH		=r16		;High byte of binary result (same as mp10H)
.def	fBCD0		=r17		;BCD value digits 1 and 0
.def	fBCD1		=r18		;BCD value digits 2 and 3
.def	fBCD2		=r19		;BCD value digit 5

;***** Code

BCD2bin16:
		andi	fBCD2,0x0f	;mask away upper nibble of fBCD2
		clr	mp10H
		mov	mp10L,fBCD2	;mp10H:mp10L = a
		mov	adder,fBCD1
		rcall	mul10a		;mp10H:mp10L = 10a+b
		mov	adder,fBCD1
		rcall	mul10b		;mp10H:mp10L = 10(10a+b)+c
		mov	adder,fBCD0
		rcall	mul10a		;mp10H:mp10L = 10(10(10a+b)+c)+d
		mov	adder,fBCD0
		rcall	mul10b		;mp10H:mp10L = 10(10(10(10a+b)+c)+d)+e
		ret



;*******************************************************************************
;*
;* FUNCTION
;*	BCD2bin8
;* DESCRIPTION
;*	Converts 2 digit packed BCD number to an 8 bit binary number
;* USAGE
;*	r16 (binary <- r17 (packed BCD)
;* STATISTICS
;*	Cycles:		3 to 48 + ret
;*	Words:		6 + ret
;*	Register usage: r16 and r17
;*
;******************************************************************************;

;***** Subroutine Register Variables

.def	tbin	=r16			;binary result
.def	fBCDH	=r17			;packed BCD input

;***** Code

BCD2bin8:
		mov	tbin,fBCDH	;copy input to result
		andi	tbin,$0f	;clear higher nibble of result
BCDb8_0:	subi	fBCDH,$10	;MSD = MSD - 1
		brcs	BCDb8_1		;if not borrow (negative result)
		subi	tbin,-10	;result = result + 10
		rjmp	BCDb8_0		;loop again
BCDb8_1:	ret			;else return



;*******************************************************************************
;*
;* FUNCTION
;*	interp8
;* DESCRIPTION
;*	Interpolates two 8-bit binary numbers (x1 and x2) based on a
;*	7-bit interpolation factor (i1) where 0<=i1<=128. x12=f(x1,x2,i1).
;*	Higher interpolation factor i1 moves result towards value x2.
;* USAGE
;*	r17 =  f(r16,r17,r18) where
;*	r17 = r16 if r18=0 and
;*	r17 = r17 if r18=128 
;* STATISTICS
;*	Cycles:		14 + ret
;*	Words:		12 + ret
;*	Register usage:	r0 and r1 and r16 to r19
;*
;******************************************************************************;



;***** Subroutine Register Variables

.def	x1		=r16
.def	x2		=r17
.def	i1		=r18
.def	x12		=r17


;***** Code

interp8:	ldi	r19,$80		;x12=((128-i1)*x1 + i1*x2)*2/256
		sub	r19,i1
		mul	r19,x1
		mov	r19,x2
		movw	r17:r16,r1:r0
		mul	i1,r19
		add	r16,r0
		adc	r17,r1
		lsl	r16		;multiply by 2
		rol	r17
		sbrc	r16,7		;truncate to 8 bits and round up
		inc	r17		;to perform 1/256 division
		ret



;*******************************************************************************
;*
;* FUNCTION
;*	interp2D
;* DESCRIPTION
;*	Specialized 2D interpolation function for RPM and advance slope
;*	Interpolates four 8-bit binary numbers (x1 to x4) based on two
;*	interpolation factors (i1 and i2) where 0<=i1<=128 and 0<=i2<=9.
;*	x14=f(x1..x4,i1,i2). Higher RPM interpolation factor i1 moves result
;*	towards value x2 and x4. Higher advance slope interpolation factor i2
;	moves result towards value x3 and x4. Reads x1...i2 from SRAM.
;* USAGE
;*	r17 =  f(x1..x4,i1,i2) where
;*	(Y)   is x1
;*	(Y+1) is x2
;*	(Y+2) is x3
;*	(Y+3) is x4
;*	(Y+4) is i1
;*	(Y+5) is i2
;* STATISTICS
;*	Cycles:		93 + ret
;*	Words:		22 + ret
;*	Register usage:	r0 and r1, r16 to r19, and Y
;*
;******************************************************************************;



interp2D:				;interpolate for x12
		ld	r16,Y
		ldd	r17,Y+1
		ldd	r18,Y+4
		rcall   interp8
		st	Y,r17		;x12 in (Y)


					;interpolate for x34
		ldd	r16,Y+2
		ldd	r17,Y+3
		ldd	r18,Y+4
		rcall   interp8
		std	Y+1,r17		;x34 in (Y+1)


					;scale i2 to 0-128 range
					;required by interp8 function
		ldd	r18,Y+5		;if i2<9 then
		cpi	r18,9		;i2=14*i2, else
		brcc	interp2D_1	;i2=128

		ldi	r19,14
		mul	r18,r19
		mov	r18,r0
		rjmp	interp2D_2

interp2D_1:	ldi	r18,128


interp2D_2:				;interpolate for x14
		ld	r16,Y		;x12
		ldd	r17,Y+1		;x34
		rcall	interp8		;scaled i2 already in r18
					;x14 result returned in r17

		ret




;*******************************************************************************
;*
;* FUNCTION
;*	interp2DM
;* DESCRIPTION
;*	Specialized 2D interpolation function for RPM and MAP
;*	Interpolates four 8-bit binary numbers (x1 to x4) based on two
;*	interpolation factors (i1 and i2) where 0<=i1<=128 and 0<=i2<=15.
;*	x14=f(x1..x4,i1,i2). Higher RPM interpolation factor i1 moves result
;*	towards value x2 and x4. Higher MAP interpolation factor i2
;	moves result towards value x3 and x4. Reads x1...i2 from SRAM.
;* USAGE
;*	r17 =  f(x1..x4,i1,i2) where
;*	(Y)   is x1
;*	(Y+1) is x2
;*	(Y+2) is x3
;*	(Y+3) is x4
;*	(Y+4) is i1
;*	(Y+5) is i2
;* STATISTICS
;*	Cycles:		93 + ret
;*	Words:		22 + ret
;*	Register usage:	r0 and r1, r16 to r19, and Y
;*
;******************************************************************************;



interp2DM:				;interpolate for x12
		ld	r16,Y
		ldd	r17,Y+1
		ldd	r18,Y+4
		rcall   interp8
		st	Y,r17		;x12 in (Y)


					;interpolate for x34
		ldd	r16,Y+2
		ldd	r17,Y+3
		ldd	r18,Y+4
		rcall   interp8
		std	Y+1,r17		;x34 in (Y+1)


					;scale i2 to 0-128 range
					;required by interp8 function
		ldd	r18,Y+5		;i2=8*i2
		lsl	r18
		lsl	r18
		lsl	r18

					;interpolate for x14
		ld	r16,Y		;x12
		ldd	r17,Y+1		;x34
		rcall	interp8		;scaled i2 already in r18
					;x14 result returned in r17

		ret






;*******************************************************************************
;*
;* FUNCTION
;*	interp3D
;* DESCRIPTION
;*	Specialized 3D interpolation function for RPM, MAP and advance slope
;*	Interpolates eight 8-bit binary numbers (x1 to x8) based on three
;*	interpolation factors (i1 to i3) where 0<=i1<=128, 0<=i2<=16 and 0<=i3<=9.
;*	x18=f(x1..x8,i1..i3). Higher RPM interpolation factor i1 moves result
;*	towards value x2, x4, x6 and x8. Higher MAP interpolation factor i2
;*	moves result towards value x3, x4, x7 and x8. Higher advance slope 
;*	interpolation factor moves result towards x5..x8. Reads x1...i3 from SRAM.
;* USAGE
;*	r17 =  f(x1..x8,i1..i3) where
;*	(Y)   is x1
;*	(Y+1) is x2
;*	(Y+2) is x3
;*	(Y+3) is x4
;*	(Y+4) is x5
;*	(Y+5) is x6
;*	(Y+6) is x7
;*	(Y+7) is x8
;*	(Y+8) is i1
;*	(Y+9) is i2
;*	(Y+10) is i3
;* STATISTICS
;*	Cycles:		217 + ret
;*	Words:		45 + ret
;*	Register usage:	r0 and r1, r16 to r19, and Y
;*
;******************************************************************************;



;***** Code

interp3D:				;interpolate for x12
		ld	r16,Y
		ldd	r17,Y+1
		ldd	r18,Y+8
		rcall	interp8
		st	Y,r17		;x12 in (Y)


					;interpolate for x34
		ldd	r16,Y+2
		ldd	r17,Y+3
		ldd	r18,Y+8
		rcall	interp8
		std	Y+1,r17		;x34 in (Y+1)


					;interpolate for x56
		ldd	r16,Y+4
		ldd	r17,Y+5
		ldd	r18,Y+8
		rcall	interp8
		std	Y+2,r17		;x56 in (Y+2)


					;interpolate for x78
		ldd	r16,Y+6
		ldd	r17,Y+7
		ldd	r18,Y+8
		rcall	interp8
		std	Y+3,r17		;x78 in (Y+3)


					;scale i2 to 0-128 range
					;required by interp8 function
		ldd	r18,Y+9		;i2=8*i2
		ldi	r19,8
		mul	r18,r19
		std	Y+9,r0


					;interpolate for x14
		ld	r16,Y
		ldd	r17,Y+1
		ldd	r18,Y+9
		rcall	interp8
		st	Y,r17		;x14 in (Y)


					;interpolate for x58
		ldd	r16,Y+2
		ldd	r17,Y+3
		ldd	r18,Y+9
		rcall	interp8
		std	Y+1,r17		;x58 in (Y+1)


					;scale i3 to 0-128 range
					;required by interp8 function
		ldd	r18,Y+10	;if i2<9 then
		cpi	r18,9		;i2=14*i2, else
		brcc	interp3D_1	;i2=128

		ldi	r19,14
		mul	r18,r19
		mov	r18,r0
		rjmp	interp3D_2

interp3D_1:	ldi	r18,128


interp3D_2:				;interpolate for x184
		ld	r16,Y		;x14
		ldd	r17,Y+1		;x58
		rcall	interp8		;scaled i3 already in r18
					;x18 result returned in r17

		ret


				


;*******************************************************************************
;*
;* ROUTINE
;*	sort3x16
;* DESCRIPTION
;*	Threesort algorithm taken from Algorithms by R. Sedgewick
;*	Sorts three 16-bit binary numbers (a1H:a1L, a2H:a2L, and a3H:a3L
;*	Returns median value in r19:r18 (low in r17:r16 and high in r21:r20)
;*	Original values unchanged
;*	Disables and then re-enables interrupts
;* USAGE
;*	(Y)   is a1L
;*	(Y+1) is a1H
;*	(Y+2) is a2L
;*	(Y+3) is a2H
;*	(Y+4) is a3L
;*	(Y+5) is a3H

;* STATISTICS
;*	Cycles:
;*	Words:
;*	Register usage:	r16 to r23, and Y
;*
;******************************************************************************;



;***** Code
					;refer to Sedgewick page 93 for algorithm details 
					;r17:r16 is a1
					;r19:r18 is a2
					;r21:r20 is a3
					;r23:r22 is t
				
					;disable interrupts
					;while loading data

sort3x16:	cli

		ld	r16,Y		;load values
		ldd	r17,Y+1
		ldd	r18,Y+2
		ldd	r19,Y+3
		ldd	r20,Y+4
		ldd	r21,Y+5

		sei
		
					;if a1 >= a2 then
					;t=a1
					;a1=a2
					;a2=t
		cp	r16,r18	
		cpc	r17,r19
		brlo	sort3x16_1

		mov	r22,r16
		mov	r23,r17

		mov	r16,r18
		mov	r17,r19

		mov	r18,r22
		mov	r19,r23


					;if a1 >= a3 then
					;t=a1
					;a1=a3
					;a3=t
sort3x16_1:	cp	r16,r20	
		cpc	r17,r21
		brlo	sort3x16_2

		mov	r22,r16
		mov	r23,r17

		mov	r16,r20
		mov	r17,r21

		mov	r20,r22
		mov	r21,r23


					;if a2 >= a3 then
					;t=a2
					;a2=a3
					;a3=t
sort3x16_2:	cp	r18,r20	
		cpc	r19,r21
		brlo	sort3x16_3

		mov	r22,r18
		mov	r23,r19

		mov	r18,r20
		mov	r19,r21

		mov	r20,r22
		mov	r21,r23

sort3x16_3:	ret
		
						



