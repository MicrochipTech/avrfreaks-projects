
; DIVISION ROUTINES with scaled reciprocals for constants
; (all functions optimized for speed, ~ <36 cycles w/o push/pop)

; Target: AVR MCUs with hardware multiplier ("mul" instruction)
; Author: Andreas Lenze (andreas.lenze@t-online.de)
; Feb. 2003, 2005

;div. by n:	n: scaled reciprocal:     shift count:

;Div16_3	3  1010101010101011 AAAB  17
;Div16_5	5  1100110011001101 CCCD  18
;Div16_6	6  1010101010101011 AAAB  18
;Div16_7	7  10010010010010011      19 -> 17 bits req'd,(MSB=1,rest 2493h)
;Div16_7a	7  1001001001001001 9249  18 -> example for correction needed for accurate result
;Div16_9	9  1110001110001111 E38F  19
;Div16_10	10 1100110011001101 CCCD  19
;Div16_11	11 1011101000101111 BA2F  19
;Div16_12	12 1010101010101011 AAAB  19
;Div16_13	13 1001110110001010 9D8A  19
;Div16_14	14 10010010010010011      20 -> 17 bits req'd,(MSB=1,rest 2493h)
;Div16_15	15 1000100010001001 8889  19
;Div16_17	17 1111000011110001 F0F1  20
;Div16_18	18 1110001110001111 E38F  20
;Div16_19	19 0110101111001011 6BCB  19
;Div16_20	20 1100110011001101 CCCD  20
;Div16_21	21 1100001100001011 C30B  20 -> needs correction for accurate result
;Div16_22	22 1011101000101111 BA2F  20
;Div16_23	23 1011001000010101 B215  20 -> needs correction for accurate result

;D16_nn 	2-23 -> "C-style" function with macro 'Div16_by' to perform a
;			constants division with all divisors from 2 to 23. Price
;			tag for the comfort is ~50 cycles / ~50 words overhead

; NOTE: Other divisor constants like /24 etc. can easily be created by
;	modifying the shift count in "Q = Q >> x": add 1 shift right for
;	'divisor x 2' (e.g. for "/24" we need a total of 20 instead of
;	the 19 shifts needed for "/12")

;	If the remainder of the division is not needed, the multiply/subtract
;	operation after the comment
;
;		r19:r18 now "Q" (= result >> xx)

;		R = A - xx*Q	(start removal
;		ldi	r17,xx
;		....
;		sbc	XH,YH	(end removal)
;
;	may be omitted to save another 9 cycles / 7 words
;	(* not applicable for 'Div16_7a' - remainder is always needed *)
;
; "Div16_7a" demonstrates and uses the 'approximate and correct'
; technique (as compared to the 'straightforward' "Div16_7") which
; may be necessary for some divisors (here: /21, /23)

;---------------------------------------------------------------------------

;***************************************************************************
;*
;* Function "Div16_3"
;* Divides an unsigned 16 bit word (XH:XL) by 3
;* Returns quotient in YH:YL and remainder in XL
;*
;* Author: Andreas Lenze (andreas.lenze@t-online.de)
;* Equations by D. W. Jones:
;*
;*	Reciprocal mul w. extra precision:
;*	unsigned int A;
;*	unsigned int scaled_reciprocal = 0xAAAB;
;*	unsigned int Q; /* the quotient */
;*
;*	Q = ((A * 0xAAAB) >> 17)
;*
;* Uses: high regs: 7 (r17, r18, r19, X, Y)
;*	 low regs:  3 (r0, r1, r2)
;*
;*	 words:     36 (w. push/pop = 10 words)
;*	 cycles:    48 (w. push/pop = 16 cycles)
;*
;* Note: Hardware multiplier required ("mul" instruction)
;*
;***************************************************************************

Div16_3:
	push	r2
	push	r19
	push	r18
	push	r17

	ldi	YH,0xAA		; scaled reciprocal for /3
	ldi	YL,0xAB

	; Q = A * 0xAAAB
	; (r19:r18:r17[:rXX] = XH:XL * YH:YL)
        clr     r2
        mul     XH, YH		; ah * bh
        movw    r19:r18, r1:r0
        mul     XL, YL		; al * bl
	mov	r17,r1		; r0 to [rXX] is superfluous
        mul     XH, YL		; ah * bl
        add     r17, r0
        adc     r18, r1
        adc     r19, r2
        mul     YH, XL		; bh * al
        add     r17, r0
        adc     r18, r1
        adc     r19, r2

	; Q = Q >> 16: use r19:r18 as word
	; Q = Q >> 1
	lsr	r19		; do the last shift
	ror	r18

	; r19:r18 now "Q" (= result >> 17)
	; R = A - 3*Q;
	ldi	r17,3		; multiply r19:r18 by 3
	mul     r18, r17	; al * bl
	sub	XL,r0
	clr	XH
	movw	YL,r18		; make copy of Q

	; XL holds "R"
	; YH:YL holds "Q"
	pop	r17
	pop	r18
	pop	r19
	pop	r2

	ret

;**** End of function Div16_3 ------------------------------------------****


;***************************************************************************
;*
;* Function "Div16_5"
;* Divides an unsigned 16 bit word (XH:XL) by 5
;* Returns quotient in YH:YL and remainder in XL
;*
;* Author: Andreas Lenze (andreas.lenze@t-online.de)
;* Equations by D. W. Jones:
;*
;*	Reciprocal mul w. extra precision:
;*	unsigned int A;
;*	unsigned int scaled_reciprocal = 0xCCCD;
;*	unsigned int Q; /* the quotient */
;*
;*	Q = ((A * 0xCCCD) >> 18)
;*
;* Uses: high regs: 7 (r17, r18, r19, X, Y)
;*	 low regs:  3 (r0, r1, r2)
;*
;*	 words:     38 (w. push/pop = 10 words)
;*	 cycles:    54 (w. push/pop = 20 cycles)
;*
;* Note: Hardware multiplier required ("mul" instruction)
;*
;***************************************************************************

Div16_5:
	push	r2
	push	r19
	push	r18
	push	r17

	ldi	YH,0xCC		; scaled reciprocal for /5
	ldi	YL,0xCD

	; Q = A * 0xCCCD
	; (r19:r18:r17[:rXX] = XH:XL * YH:YL)
        clr     r2
        mul     XH, YH		; ah * bh
        movw    r19:r18, r1:r0
        mul     XL, YL		; al * bl
	mov	r17,r1		; r0 to [rXX] is superfluous
        mul     XH, YL		; ah * bl
        add     r17, r0
        adc     r18, r1
        adc     r19, r2
        mul     YH, XL		; bh * al
        add     r17, r0
        adc     r18, r1
        adc     r19, r2

	; Q = Q >> 16: use r19:r18 as word
	; Q = Q >> 2
	lsr	r19		; do the last 2 shifts
	ror	r18
	lsr	r19
	ror	r18

	; r19:r18 now "Q" (= result >> 18)
	; R = A - 5*Q;
	ldi	r17,5		; multiply r19:r18 by 5
	mul     r18, r17	; al * bl
	sub	XL,r0
	clr	XH
	movw	YL,r18		; make copy of Q

	; XL holds "R"
	; YH:YL holds "Q"
	pop	r17
	pop	r18
	pop	r19
	pop	r2

	ret

;**** End of function Div16_5 ------------------------------------------****


;***************************************************************************
;*
;* Function "Div16_6"
;* Divides an unsigned 16 bit word (XH:XL) by 6
;* Returns quotient in YH:YL and remainder in XL
;*
;* Author: Andreas Lenze (andreas.lenze@t-online.de)
;* Equations by D. W. Jones:
;*
;*	Reciprocal mul w. extra precision:
;*	unsigned int A;
;*	unsigned int scaled_reciprocal = 0xAAAB;
;*	unsigned int Q; /* the quotient */
;*
;*	Q = ((A * 0xAAAB) >> 18)
;*
;* Uses: high regs: 7 (r17, r18, r19, X, Y)
;*	 low regs:  3 (r0, r1, r2)
;*
;*	 words:     38 (w. push/pop = 10 words)
;*	 cycles:    54 (w. push/pop = 20 cycles)
;*
;* Note: Hardware multiplier required ("mul" instruction)
;*
;***************************************************************************

Div16_6:
	push	r2
	push	r19
	push	r18
	push	r17

	ldi	YH,0xAA		; scaled reciprocal for /6
	ldi	YL,0xAB

	; Q = A * 0xAAAB
	; (r19:r18:r17[:rXX] = XH:XL * YH:YL)
        clr     r2
        mul     XH, YH		; ah * bh
        movw    r19:r18, r1:r0
        mul     XL, YL		; al * bl
	mov	r17,r1		; r0 to [rXX] is superfluous
        mul     XH, YL		; ah * bl
        add     r17, r0
        adc     r18, r1
        adc     r19, r2
        mul     YH, XL		; bh * al
        add     r17, r0
        adc     r18, r1
        adc     r19, r2

	; Q = Q >> 16: use r19:r18 as word
	; Q = Q >> 2
	lsr	r19		; do the last 2 shifts
	ror	r18
	lsr	r19
	ror	r18

	; r19:r18 now "Q" (= result >> 18)
	; R = A - 6*Q;
	ldi	r17,6		; multiply r19:r18 by 6
	mul     r18, r17	; al * bl
	sub	XL,r0
	clr	XH
	movw	YL,r18		; make copy of Q

	; XL holds "R"
	; YH:YL holds "Q"
	pop	r17
	pop	r18
	pop	r19
	pop	r2

	ret

;**** End of function Div16_6 ------------------------------------------****


;***************************************************************************
;*
;* Function "Div16_7"
;* Divides an unsigned 16 bit word (XH:XL) by 7
;* Returns quotient in YH:YL and remainder in XL
;*
;* Author: Andreas Lenze (andreas.lenze@t-online.de)
;* Equations by D. W. Jones:
;*
;*	Reciprocal mul w. extra precision:
;*	unsigned int A;
;*	unsigned int scaled_reciprocal = 0x2493;
;*	unsigned int Q; /* the quotient */
;*
;*	Q = (((A * 0x2493) >> 16) + A) >> 3 -> 17 bits reciprocal!
;*
;* Uses: high regs: 7 (r17, r18, r19, X, Y)
;*	 low regs:  3 (r0, r1, r2)
;*
;*	 words:     38 (w. push/pop = 8 words)
;*	 cycles:    46 (w. push/pop = 16 cycles)
;*
;* Note: Hardware multiplier required ("mul" instruction)
;*
;***************************************************************************

Div16_7:
	push	r2
	push	r19
	push	r18
	push	r17

	ldi	YH,0x24		; scaled reciprocal for /7
	ldi	YL,0x93

	; Q = A * 0x2493
	; (r19:r18:r17[:rXX] = XH:XL * YH:YL)
        clr     r2
        mul     XH, YH		; ah * bh
        movw    r19:r18, r1:r0
        mul     XL, YL		; al * bl
	mov	r17,r1		; r0 to [rXX] is superfluous
        mul     XH, YL		; ah * bl
        add     r17, r0
        adc     r18, r1
        adc     r19, r2
        mul     YH, XL		; bh * al
        add     r17, r0
        adc     r18, r1
        adc     r19, r2

	; Q = Q >> 16: use r19:r18 as word
	; Q = Q + A
	add	r18,XL
	adc	r19,XH

	; Q = Q >> 3
	ror	r19		; do the last 3 shifts, including
	ror	r18		; carry (!) from previous addition
	lsr	r19
	ror	r18
	lsr	r19
	ror	r18

	; r19:r18 now "Q" (= result >> 19)
	; R = A - 7*Q;
	ldi	r17,7		; multiply r19:r18 by 7
	mul     r18, r17	; al * bl
	sub	XL,r0
	clr	XH
	movw	YL,r18		; make copy of Q

	; XL holds "R"
	; YH:YL holds "Q"
	pop	r17
	pop	r18
	pop	r19
	pop	r2

	ret

;**** End of function Div16_7 ------------------------------------------****


;***************************************************************************
;*
;* Function "Div16_7a"
;* Divides an unsigned 16 bit word (XH:XL) by 7
;* Call with 16 bit number in XH:XL
;* Returns quotient in YH:YL and remainder in XL
;*
;* Author: Andreas Lenze (andreas.lenze@t-online.de)
;* (Equations partly by D. W. Jones)
;*
;*      Reciprocal multiplication w. extra precision:
;*	(This version uses correction to achieve the required precision)
;*      unsigned int R; /* remainder */
;*      unsigned int long A; /* dividend */
;*      unsigned int long Q; /* quotient */
;*
;*      Q = ((A * 0x9249) >> 18)
;*
;*      /* Q = A/7 or Q+1 = A/7 for all A <= 65535 */
;*      /* correct Q and calculate remainder */
;*      R = A - 7*Q
;*      if (R >= 7) {
;*              R = R - 7;
;*              Q = Q + 1;
;*      }
;* Uses: high regs: 7 (r17, r18, r19, X, Y)
;*       low regs:  3 (r0, r1, r2)
;*
;*       words:     36 (w. push/pop = 8 words)
;*       cycles:    59 (w. push/pop = 20 cycles)
;*
;* Note: Hardware multiplier required ("mul" instruction)
;*
;***************************************************************************

Div16_7a:
        push    r2
        push    r19
        push    r18             ; Tmp3
        push    r17             ; Tmp2

        ldi     YH,0x92         ; scaled reciprocal for /7
        ldi     YL,0x49		; (16 bit only, 0/-1 error possible)

        ; Q = A * 0x9249
	; (r19:r18:r17[:rXX] = XH:XL * YH:YL)
        clr     r2
        mul     XH, YH		; ah * bh
        movw    r19:r18, r1:r0
        mul     XL, YL		; al * bl
	mov	r17,r1		; r0 to [rXX] is superfluous
        mul     XH, YL		; ah * bl
        add     r17, r0
        adc     r18, r1
        adc     r19, r2
        mul     YH, XL		; bh * al
        add     r17, r0
        adc     r18, r1
        adc     r19, r2

        ; Q = Q >> 16: use r19:r18 as word
        ; Q = Q >> 2
        lsr     r19             ; 2 shifts remaining
        ror     r18
        lsr     r19
        ror     r18

        ; r19:r18 now "Q" (= result >> 18)
        ; R = A - 7*Q;
	ldi	r17,7		; multiply r19:r18 by 7
	mul     r18, r17	; al * bl
	sub	XL,r0
	clr	XH
	movw	YL,r18		; make copy of current Q

        ; XH:XL now "R":
        ; if (R >= 7)		; apply correction
        ;       R = R - 7;
        ;       Q = Q + 1;
        cpi     XL,0x07
        brlo    PC+3
        subi    XL,7
        adiw    YL,1

	; XL holds "R"
        ; YH:YL holds "Q"
        pop     r17
        pop     r18
        pop     r19
        pop     r2

        ret

;**** End of function Div16_7a -----------------------------------------****


;***************************************************************************
;*
;* Function "Div16_9"
;* Divides an unsigned 16 bit word (XH:XL) by 9
;* Returns quotient in YH:YL and remainder in XL
;*
;* Author: Andreas Lenze (andreas.lenze@t-online.de)
;* Equations by D. W. Jones:
;*
;*	Reciprocal mul w. extra precision:
;*	unsigned int A;
;*	unsigned int scaled_reciprocal = 0xE38F;
;*	unsigned int Q; /* the quotient */
;*
;*	Q = ((A * 0xE38F) >> 19)
;*
;* Uses: high regs: 7 (r17, r18, r19, X, Y)
;*	 low regs:  3 (r0, r1, r2)
;*
;*	 words:     36 (w. push/pop = 8 words)
;*	 cycles:    46 (w. push/pop = 16 cycles)
;*
;* Note: Hardware multiplier required ("mul" instruction)
;*
;***************************************************************************

Div16_9:
	push	r2
	push	r19
	push	r18
	push	r17

	ldi	YH,0xE3		; scaled reciprocal for /9
	ldi	YL,0x8F

	; Q = A * 0xE38F
	; (r19:r18:r17[:rXX] = XH:XL * YH:YL)
        clr     r2
        mul     XH, YH		; ah * bh
        movw    r19:r18, r1:r0
        mul     XL, YL		; al * bl
	mov	r17,r1		; r0 to [rXX] is superfluous
        mul     XH, YL		; ah * bl
        add     r17, r0
        adc     r18, r1
        adc     r19, r2
        mul     YH, XL		; bh * al
        add     r17, r0
        adc     r18, r1
        adc     r19, r2

	; Q = Q >> 16: use r19:r18 as word
	; Q = Q >> 3
	lsr	r19		; do the last 3 shifts
	ror	r18
	lsr	r19
	ror	r18
	lsr	r19
	ror	r18

	; r19:r18 now "Q" (= result >> 19)
	; R = A - 9*Q;
	ldi	r17,9		; multiply r19:r18 by 9
	mul     r18, r17	; al * bl
	sub	XL,r0
	clr	XH
	movw	YL,r18		; make copy of Q

	; XL holds "R"
	; YH:YL holds "Q"
	pop	r17
	pop	r18
	pop	r19
	pop	r2

	ret

;**** End of function Div16_9 ------------------------------------------****


;***************************************************************************
;*
;* Function "Div16_10"
;* Divides an unsigned 16 bit word (XH:XL) by 10
;* Returns quotient in YH:YL and remainder in XL
;*
;* Author: Andreas Lenze (andreas.lenze@t-online.de)
;* Equations by D. W. Jones:
;*
;*	Reciprocal mul w. extra precision:
;*	unsigned int A;
;*	unsigned int scaled_reciprocal = 0xCCCD;
;*	unsigned int Q; /* the quotient */
;*
;*	Q = ((A * 0xCCCD) >> 19)
;*
;* Uses: high regs: 7 (r17, r18, r19, X, Y)
;*	 low regs:  3 (r0, r1, r2)
;*
;*	 words:     36 (w. push/pop = 8 words)
;*	 cycles:    46 (w. push/pop = 16 cycles)
;*
;* Note: Hardware multiplier required ("mul" instruction)
;*
;***************************************************************************

Div16_10:
	push	r2
	push	r19
	push	r18
	push	r17

	ldi	YH,0xCC		; scaled reciprocal for /10
	ldi	YL,0xCD

	; Q = A * 0xCCCD
	; (r19:r18:r17[:rXX] = XH:XL * YH:YL)
        clr     r2
        mul     XH, YH		; ah * bh
        movw    r19:r18, r1:r0
        mul     XL, YL		; al * bl
	mov	r17,r1		; r0 to [rXX] is superfluous
        mul     XH, YL		; ah * bl
        add     r17, r0
        adc     r18, r1
        adc     r19, r2
        mul     YH, XL		; bh * al
        add     r17, r0
        adc     r18, r1
        adc     r19, r2

	; Q = Q >> 16: use r19:r18 as word
	; Q = Q >> 3
	lsr	r19		; do the last 3 shifts
	ror	r18
	lsr	r19
	ror	r18
	lsr	r19
	ror	r18

	; r19:r18 now "Q" (= result >> 19)
	; R = A - 10*Q;
	ldi	r17,10		; multiply r19:r18 by 10
	mul     r18, r17	; al * bl
	sub	XL,r0
	clr	XH
	movw	YL,r18		; make copy of Q

	; XL holds "R"
	; YH:YL holds "Q"
	pop	r17
	pop	r18
	pop	r19
	pop	r2

	ret

;**** End of function Div16_10 -----------------------------------------****


;***************************************************************************
;*
;* Function "Div16_11"
;* Divides an unsigned 16 bit word (XH:XL) by 11
;* Returns quotient in YH:YL and remainder in XL
;*
;* Author: Andreas Lenze (andreas.lenze@t-online.de)
;* Equations by D. W. Jones:
;*
;*	Reciprocal mul w. extra precision:
;*	unsigned int A;
;*	unsigned int scaled_reciprocal = 0xBA2F;
;*	unsigned int Q; /* the quotient */
;*
;*	Q = ((A * 0xBA2F) >> 19)
;*
;* Uses: high regs: 7 (r17, r18, r19, X, Y)
;*	 low regs:  3 (r0, r1, r2)
;*
;*	 words:     36 (w. push/pop = 8 words)
;*	 cycles:    46 (w. push/pop = 16 cycles)
;*
;* Note: Hardware multiplier required ("mul" instruction)
;*
;***************************************************************************

Div16_11:
	push	r2
	push	r19
	push	r18
	push	r17

	ldi	YH,0xBA		; scaled reciprocal for /11
	ldi	YL,0x2F

	; Q = A * 0xBA2F
	; (r19:r18:r17[:rXX] = XH:XL * YH:YL)
        clr     r2
        mul     XH, YH		; ah * bh
        movw    r19:r18, r1:r0
        mul     XL, YL		; al * bl
	mov	r17,r1		; r0 to [rXX] is superfluous
        mul     XH, YL		; ah * bl
        add     r17, r0
        adc     r18, r1
        adc     r19, r2
        mul     YH, XL		; bh * al
        add     r17, r0
        adc     r18, r1
        adc     r19, r2

	; Q = Q >> 16: use r19:r18 as word
	; Q = Q >> 3
	lsr	r19		; do the last 3 shifts
	ror	r18
	lsr	r19
	ror	r18
	lsr	r19
	ror	r18

	; r19:r18 now "Q" (= result >> 19)
	; R = A - 11*Q;
	ldi	r17,11		; multiply r19:r18 by 11
	mul     r18, r17	; al * bl
	sub	XL,r0
	clr	XH
	movw	YL,r18		; make copy of Q

	; XL holds "R"
	; YH:YL holds "Q"
	pop	r17
	pop	r18
	pop	r19
	pop	r2

	ret

;**** End of function Div16_11 -----------------------------------------****


;***************************************************************************
;*
;* Function "Div16_12"
;* Divides an unsigned 16 bit word (XH:XL) by 12
;* Returns quotient in YH:YL and remainder in XL
;*
;* Author: Andreas Lenze (andreas.lenze@t-online.de)
;* Equations by D. W. Jones:
;*
;*	Reciprocal mul w. extra precision:
;*	unsigned int A;
;*	unsigned int scaled_reciprocal = 0xAAAB;
;*	unsigned int Q; /* the quotient */
;*
;*	Q = ((A * 0xAAAB) >> 19)
;*
;* Uses: high regs: 7 (r17, r18, r19, X, Y)
;*	 low regs:  3 (r0, r1, r2)
;*
;*	 words:     36 (w. push/pop = 8 words)
;*	 cycles:    46 (w. push/pop = 16 cycles)
;*
;* Note: Hardware multiplier required ("mul" instruction)
;*
;***************************************************************************

Div16_12:
	push	r2
	push	r19
	push	r18
	push	r17

	ldi	YH,0xAA		; scaled reciprocal for /12
	ldi	YL,0xAB

	; Q = A * 0xAAAB
	; (r19:r18:r17[:rXX] = XH:XL * YH:YL)
        clr     r2
        mul     XH, YH		; ah * bh
        movw    r19:r18, r1:r0
        mul     XL, YL		; al * bl
	mov	r17,r1		; r0 to [rXX] is superfluous
        mul     XH, YL		; ah * bl
        add     r17, r0
        adc     r18, r1
        adc     r19, r2
        mul     YH, XL		; bh * al
        add     r17, r0
        adc     r18, r1
        adc     r19, r2

	; Q = Q >> 16: use r19:r18 as word
	; Q = Q >> 3
	lsr	r19		; do the last 3 shifts
	ror	r18
	lsr	r19
	ror	r18
	lsr	r19
	ror	r18

	; r19:r18 now "Q" (= result >> 19)
	; R = A - 12*Q;
	ldi	r17,12		; multiply r19:r18 by 12
	mul     r18, r17	; al * bl
	sub	XL,r0
	clr	XH
	movw	YL,r18		; make copy of Q

	; XL holds "R"
	; YH:YL holds "Q"
	pop	r17
	pop	r18
	pop	r19
	pop	r2

	ret

;**** End of function Div16_12 -----------------------------------------****


;***************************************************************************
;*
;* Function "Div16_13"
;* Divides an unsigned 16 bit word (XH:XL) by 13
;* Returns quotient in YH:YL and remainder in XL
;*
;* Author: Andreas Lenze (andreas.lenze@t-online.de)
;* Equations by D. W. Jones:
;*
;*	Reciprocal mul w. extra precision:
;*	unsigned int A;
;*	unsigned int scaled_reciprocal = 0x9D8A;
;*	unsigned int Q; /* the quotient */
;*
;*	Q = ((A * 0x9D8A) >> 19)
;*
;* Uses: high regs: 7 (r17, r18, r19, X, Y)
;*	 low regs:  3 (r0, r1, r2)
;*
;*	 words:     36 (w. push/pop = 8 words)
;*	 cycles:    46 (w. push/pop = 16 cycles)
;*
;* Note: Hardware multiplier required ("mul" instruction)
;*
;***************************************************************************

Div16_13:
	push	r2
	push	r19
	push	r18
	push	r17

	ldi	YH,0x9D		; scaled reciprocal for /13
	ldi	YL,0x8A

	; Q = A * 0x9D8A
	; (r19:r18:r17[:rXX] = XH:XL * YH:YL)
        clr     r2
        mul     XH, YH		; ah * bh
        movw    r19:r18, r1:r0
        mul     XL, YL		; al * bl
	mov	r17,r1		; r0 to [rXX] is superfluous
        mul     XH, YL		; ah * bl
        add     r17, r0
        adc     r18, r1
        adc     r19, r2
        mul     YH, XL		; bh * al
        add     r17, r0
        adc     r18, r1
        adc     r19, r2

	; Q = Q >> 16: use r19:r18 as word
	; Q = Q >> 3
	lsr	r19		; do the last 3 shifts
	ror	r18
	lsr	r19
	ror	r18
	lsr	r19
	ror	r18

	; r19:r18 now "Q" (= result >> 19)
	; R = A - 13*Q;
	ldi	r17,13		; multiply r19:r18 by 13
	mul     r18, r17	; al * bl
	sub	XL,r0
	clr	XH
	movw	YL,r18		; make copy of Q

	; XL holds "R"
	; YH:YL holds "Q"
	pop	r17
	pop	r18
	pop	r19
	pop	r2

	ret

;**** End of function Div16_13 -----------------------------------------****


;***************************************************************************
;*
;* Function "Div16_14"
;* Divides an unsigned 16 bit word (XH:XL) by 14
;* Returns quotient in YH:YL and remainder in XL
;*
;* Author: Andreas Lenze (andreas.lenze@t-online.de)
;* Equations by D. W. Jones:
;*
;*	Reciprocal mul w. extra precision:
;*	unsigned int A;
;*	unsigned int scaled_reciprocal = 0x2493;
;*	unsigned int Q; /* the quotient */
;*
;*	Q = (((A * 0x2493) >> 16) + A) >> 4 -> 17 bits reciprocal!
;*
;* Uses: high regs: 7 (r17, r18, r19, X, Y)
;*	 low regs:  3 (r0, r1, r2)
;*
;*	 words:     40 (w. push/pop = 8 words)
;*	 cycles:    44 (w. push/pop = 16 cycles)
;*
;* Note: Hardware multiplier required ("mul" instruction)
;*
;***************************************************************************

Div16_14:
	push	r2
	push	r19
	push	r18
	push	r17

	ldi	YH,0x24		; scaled reciprocal for /7, /14
	ldi	YL,0x93

	; Q = A * 0x2493
	; (r19:r18:r17[:rXX] = XH:XL * YH:YL)
        clr     r2
        mul     XH, YH		; ah * bh
        movw    r19:r18, r1:r0
        mul     XL, YL		; al * bl
	mov	r17,r1		; r0 to [rXX] is superfluous
        mul     XH, YL		; ah * bl
        add     r17, r0
        adc     r18, r1
        adc     r19, r2
        mul     YH, XL		; bh * al
        add     r17, r0
        adc     r18, r1
        adc     r19, r2

	; Q = Q >> 16: use r19:r18 as word
	; Q = Q + A
	add	r18,XL
	adc	r19,XH

	; Q = Q >> 4
	ror	r19		; do the last 4 shifts, including
	ror	r18		; carry (!) from previous addition
	lsr	r19
	ror	r18
	lsr	r19
	ror	r18
	lsr	r19
	ror	r18

	; r19:r18 now "Q" (= result >> 20)
	; R = A - 14*Q;
	ldi	r17,14		; multiply r19:r18 by 14
	mul     r18, r17	; al * bl
	sub	XL,r0
	clr	XH
	movw	YL,r18		; make copy of Q

	; XL holds "R"
	; YH:YL holds "Q"
	pop	r17
	pop	r18
	pop	r19
	pop	r2

	ret

;**** End of function Div16_14 -----------------------------------------****


;***************************************************************************
;*
;* Function "Div16_15"
;* Divides an unsigned 16 bit word (XH:XL) by 15
;* Returns quotient in YH:YL and remainder in XL
;*
;* Author: Andreas Lenze (andreas.lenze@t-online.de)
;* Equations by D. W. Jones:
;*
;*	Reciprocal mul w. extra precision:
;*	unsigned int A;
;*	unsigned int scaled_reciprocal = 0x8889;
;*	unsigned int Q; /* the quotient */
;*
;*	Q = ((A * 0x8889) >> 19)
;*
;* Uses: high regs: 7 (r17, r18, r19, X, Y)
;*	 low regs:  3 (r0, r1, r2)
;*
;*	 words:     36 (w. push/pop = 8 words)
;*	 cycles:    46 (w. push/pop = 16 cycles)
;*
;* Note: Hardware multiplier required ("mul" instruction)
;*
;***************************************************************************

Div16_15:
	push	r2
	push	r19
	push	r18
	push	r17

	ldi	YH,0x88		; scaled reciprocal for /15
	ldi	YL,0x89

	; Q = A * 0x8889
	; (r19:r18:r17[:rXX] = XH:XL * YH:YL)
        clr     r2
        mul     XH, YH		; ah * bh
        movw    r19:r18, r1:r0
        mul     XL, YL		; al * bl
	mov	r17,r1		; r0 to [rXX] is superfluous
        mul     XH, YL		; ah * bl
        add     r17, r0
        adc     r18, r1
        adc     r19, r2
        mul     YH, XL		; bh * al
        add     r17, r0
        adc     r18, r1
        adc     r19, r2

	; Q = Q >> 16: use r19:r18 as word
	; Q = Q >> 3
	lsr	r19		; do the last 3 shifts
	ror	r18
	lsr	r19
	ror	r18
	lsr	r19
	ror	r18

	; r19:r18 now "Q" (= result >> 19)
	; R = A - 15*Q;
	ldi	r17,15		; multiply r19:r18 by 15
	mul     r18, r17	; al * bl
	sub	XL,r0
	clr	XH
	movw	YL,r18		; make copy of Q

	; XL holds "R"
	; YH:YL holds "Q"
	pop	r17
	pop	r18
	pop	r19
	pop	r2

	ret

;**** End of function Div16_15 -----------------------------------------****


;***************************************************************************
;*
;* Function "Div16_17"
;* Divides an unsigned 16 bit word (XH:XL) by 17
;* Returns quotient in YH:YL and remainder in XL
;*
;* Author: Andreas Lenze (andreas.lenze@t-online.de)
;* Equations by D. W. Jones:
;*
;*	Reciprocal mul w. extra precision:
;*	unsigned int A;
;*	unsigned int scaled_reciprocal = 0xF0F1;
;*	unsigned int Q; /* the quotient */
;*
;*	Q = ((A * 0xF0F1) >> 20)
;*
;* Uses: high regs: 7 (r17, r18, r19, X, Y)
;*	 low regs:  3 (r0, r1, r2)
;*
;*	 words:     39 (w. push/pop = 10 words)
;*	 cycles:    45 (w. push/pop = 16 cycles)
;*
;* Note: Hardware multiplier required ("mul" instruction)
;*
;***************************************************************************

Div16_17:
	push	r2
	push	r19
	push	r18
	push	r17

	ldi	YH,0xF0		; scaled reciprocal for /17
	ldi	YL,0xF1

	; Q = A * 0xF0F1
	; (r19:r18:r17[:rXX] = XH:XL * YH:YL)
        clr     r2
        mul     XH, YH		; ah * bh
        movw    r19:r18, r1:r0
        mul     XL, YL		; al * bl
	mov	r17,r1		; r0 to [rXX] is superfluous
        mul     XH, YL		; ah * bl
        add     r17, r0
        adc     r18, r1
        adc     r19, r2
        mul     YH, XL		; bh * al
        add     r17, r0
        adc     r18, r1
        adc     r19, r2

	; Q = Q >> 16: use r19:r18 as word
	; Q = Q >> 4
	swap	r19		; do the last 4 shifts
	mov	XH,r19
	andi	XH,0xF0
	andi	r19,0x0F
	swap	r18
	andi	r18,0x0F
	add	r18,XH

	; r19:r18 now "Q" (= result >> 20)
	; R = A - 17*Q;
	ldi	r17,17		; multiply r19:r18 by 17
	mul     r18, r17	; al * bl
	sub	XL,r0
	clr	XH
	movw	YL,r18		; make copy of Q

	; XL holds "R"
	; YH:YL holds "Q"
	pop	r17
	pop	r18
	pop	r19
	pop	r2

	ret

;**** End of function Div16_17 -----------------------------------------****


;***************************************************************************
;*
;* Function "Div16_18"
;* Divides an unsigned 16 bit word (XH:XL) by 18
;* Returns quotient in YH:YL and remainder in XL
;*
;* Author: Andreas Lenze (andreas.lenze@t-online.de)
;* Equations by D. W. Jones:
;*
;*	Reciprocal mul w. extra precision:
;*	unsigned int A;
;*	unsigned int scaled_reciprocal = 0xE38F;
;*	unsigned int Q; /* the quotient */
;*
;*	Q = ((A * 0xE38F) >> 20)
;*
;* Uses: high regs: 7 (r17, r18, r19, X, Y)
;*	 low regs:  3 (r0, r1, r2)
;*
;*	 words:     39 (w. push/pop = 10 words)
;*	 cycles:    45 (w. push/pop = 16 cycles)
;*
;* Note: Hardware multiplier required ("mul" instruction)
;*
;***************************************************************************

Div16_18:
	push	r2
	push	r19
	push	r18
	push	r17

	ldi	YH,0xE3		; scaled reciprocal for /9 /18
	ldi	YL,0x8F

	; Q = A * 0xE38F
	; (r19:r18:r17[:rXX] = XH:XL * YH:YL)
        clr     r2
        mul     XH, YH		; ah * bh
        movw    r19:r18, r1:r0
        mul     XL, YL		; al * bl
	mov	r17,r1		; r0 to [rXX] is superfluous
        mul     XH, YL		; ah * bl
        add     r17, r0
        adc     r18, r1
        adc     r19, r2
        mul     YH, XL		; bh * al
        add     r17, r0
        adc     r18, r1
        adc     r19, r2

	; Q = Q >> 16: use r19:r18 as word
	; Q = Q >> 4
	swap	r19		; do the last 4 shifts
	mov	XH,r19
	andi	XH,0xF0
	andi	r19,0x0F
	swap	r18
	andi	r18,0x0F
	add	r18,XH

	; r19:r18 now "Q" (= result >> 20)
	; R = A - 18*Q;
	ldi	r17,18		; multiply r19:r18 by 18
	mul     r18, r17	; al * bl
	sub	XL,r0
	clr	XH
	movw	YL,r18		; make copy of Q

	; XL holds "R"
	; YH:YL holds "Q"
	pop	r17
	pop	r18
	pop	r19
	pop	r2

	ret

;**** End of function Div16_18 -----------------------------------------****


;***************************************************************************
;*
;* Function "Div16_19"
;* Divides an unsigned 16 bit word (XH:XL) by 19
;* Returns quotient in YH:YL and remainder in XL
;*
;* Author: Andreas Lenze (andreas.lenze@t-online.de)
;* Equations by D. W. Jones:
;*
;*	Reciprocal mul w. extra precision:
;*	unsigned int A;
;*	unsigned int scaled_reciprocal = 0x6BCB;
;*	unsigned int Q; /* the quotient */
;*
;*	Q = ((A * 0x6BCB) >> 19)
;*
;* Uses: high regs: 7 (r17, r18, r19, X, Y)
;*	 low regs:  3 (r0, r1, r2)
;*
;*	 words:     36 (w. push/pop = 8 words)
;*	 cycles:    46 (w. push/pop = 16 cycles)
;*
;* Note: Hardware multiplier required ("mul" instruction)
;*
;***************************************************************************

Div16_19:
	push	r2
	push	r19
	push	r18
	push	r17

	ldi	YH,0x6B		; scaled reciprocal for /19
	ldi	YL,0xCB

	; Q = A * 0x6BCB
	; (r19:r18:r17[:rXX] = XH:XL * YH:YL)
        clr     r2
        mul     XH, YH		; ah * bh
        movw    r19:r18, r1:r0
        mul     XL, YL		; al * bl
	mov	r17,r1		; r0 to [rXX] is superfluous
        mul     XH, YL		; ah * bl
        add     r17, r0
        adc     r18, r1
        adc     r19, r2
        mul     YH, XL		; bh * al
        add     r17, r0
        adc     r18, r1
        adc     r19, r2

	; Q = Q >> 16: use r19:r18 as word
	; Q = Q >> 3
	lsr	r19		; do the last 3 shifts
	ror	r18
	lsr	r19
	ror	r18
	lsr	r19
	ror	r18

	; r19:r18 now "Q" (= result >> 19)
	; R = A - 19*Q;
	ldi	r17,18		; multiply r19:r18 by 18
	mul     r18, r17	; al * bl
	sub	XL,r0
	clr	XH
	movw	YL,r18		; make copy of Q

	; XL holds "R"
	; YH:YL holds "Q"
	pop	r17
	pop	r18
	pop	r19
	pop	r2

	ret

;**** End of function Div16_19 -----------------------------------------****


;***************************************************************************
;*
;* Function "Div16_20"
;* Divides an unsigned 16 bit word (XH:XL) by 20
;* Returns quotient in YH:YL and remainder in XL
;*
;* Author: Andreas Lenze (andreas.lenze@t-online.de)
;* Equations by D. W. Jones:
;*
;*	Reciprocal mul w. extra precision:
;*	unsigned int A;
;*	unsigned int scaled_reciprocal = 0xCCCD;
;*	unsigned int Q; /* the quotient */
;*
;*	Q = ((A * 0xCCCD) >> 20)
;*
;* Uses: high regs: 7 (r17, r18, r19, X, Y)
;*	 low regs:  3 (r0, r1, r2)
;*
;*	 words:     37 (w. push/pop = 8 words)
;*	 cycles:    47 (w. push/pop = 16 cycles)
;*
;* Note: Hardware multiplier required ("mul" instruction)
;*
;***************************************************************************

Div16_20:
	push	r2
	push	r19
	push	r18
	push	r17

	ldi	YH,0xCC		; scaled reciprocal for /10, /20
	ldi	YL,0xCD

	; Q = A * 0xCCCD
	; (r19:r18:r17[:rXX] = XH:XL * YH:YL)
        clr     r2
        mul     XH, YH		; ah * bh
        movw    r19:r18, r1:r0
        mul     XL, YL		; al * bl
	mov	r17,r1		; r0 to [rXX] is superfluous
        mul     XH, YL		; ah * bl
        add     r17, r0
        adc     r18, r1
        adc     r19, r2
        mul     YH, XL		; bh * al
        add     r17, r0
        adc     r18, r1
        adc     r19, r2

	; Q = Q >> 16: use r19:r18 as word
	; Q = Q >> 4
	swap	r19		; do the last 4 shifts
	mov	XH,r19
	andi	XH,0xF0
	andi	r19,0x0F
	swap	r18
	andi	r18,0x0F
	add	r18,XH

	; r19:r18 now "Q" (= result >> 20)
	; R = A - 20*Q;
	ldi	r17,20		; multiply r19:r18 by 20
	mul     r18, r17	; al * bl
	sub	XL,r0
	clr	XH
	movw	YL,r18		; make copy of Q

	; XL holds "R"
	; YH:YL holds "Q"
	pop	r17
	pop	r18
	pop	r19
	pop	r2

	ret

;**** End of function Div16_20 -----------------------------------------****


;***************************************************************************
;*
;* Function "Div16_21"
;* Divides an unsigned 16 bit word (XH:XL) by 21
;* Call with 16 bit number in XH:XL
;* Returns quotient in YH:YL and remainder in XL
;*
;* Author: Andreas Lenze (andreas.lenze@t-online.de)
;* (Equations partly by D. W. Jones)
;*
;*      Reciprocal multiplication w. extra precision:
;*	(uses correction to achieve the required precision)
;*      unsigned int R; /* remainder */
;*      unsigned int long A; /* dividend */
;*      unsigned int long Q; /* quotient */
;*
;*      Q = ((A * 0xC30B) >> 20)
;*
;*      /* Q = A/21 or Q+1 = A/21 for all A <= 65535 */
;*      /* correct Q and calculate remainder */
;*      R = A - 21*Q
;*      if (R >= 21) {
;*              R = R - 21;
;*              Q = Q + 1;
;*      }
;* Uses: high regs: 7 (r17, r18, r19, X, Y)
;*       low regs:  3 (r0, r1, r2)
;*
;*       words:     41 (w. push/pop = 8 words)
;*       cycles:    53 (w. push/pop = 16 cycles)
;*
;* Note: Hardware multiplier required ("mul" instruction)
;*
;***************************************************************************

Div16_21:
        push    r2
        push    r19
        push    r18
        push    r17

        ldi     YH,0xC3         ; scaled reciprocal for /21
        ldi     YL,0x0B		; (16 bit only, 0/-1 error possible)

        ; Q = A * 0xC30B
	; (r19:r18:r17[:rXX] = XH:XL * YH:YL)
        clr     r2
        mul     XH, YH		; ah * bh
        movw    r19:r18, r1:r0
        mul     XL, YL		; al * bl
	mov	r17,r1		; r0 to [rXX] is superfluous
        mul     XH, YL		; ah * bl
        add     r17, r0
        adc     r18, r1
        adc     r19, r2
        mul     YH, XL		; bh * al
        add     r17, r0
        adc     r18, r1
        adc     r19, r2

        ; Q = Q >> 16: use r19:r18 as word
        ; Q = Q >> 4
	swap	r19		; do the last 4 shifts
	mov	XH,r19
	andi	XH,0xF0
	andi	r19,0x0F
	swap	r18
	andi	r18,0x0F
	add	r18,XH

        ; r19:r18 now "Q" (= result >> 20)
        ; R = A - 21*Q;
	ldi	r17,21		; multiply r19:r18 by 21
	mul     r18, r17	; al * bl
	sub	XL,r0
	clr	XH
	movw	YL,r18		; make copy of current Q

        ; XH:XL now "R":
        ; if (R >= 21)		; apply correction
        ;       R = R - 21;
        ;       Q = Q + 1;
        cpi     XL,0x15
        brlo    PC+3
        subi    XL,21
        adiw    YL,1

	; XL holds "R"
        ; YH:YL holds "Q"
        pop     r17
        pop     r18
        pop     r19
        pop     r2

        ret

;**** End of function Div16_21 -----------------------------------------****


;***************************************************************************
;*
;* Function "Div16_22"
;* Divides an unsigned 16 bit word (XH:XL) by 22
;* Returns quotient in YH:YL and remainder in XL
;*
;* Author: Andreas Lenze (andreas.lenze@t-online.de)
;* Equations by D. W. Jones:
;*
;*	Reciprocal mul w. extra precision:
;*	unsigned int A;
;*	unsigned int scaled_reciprocal = 0xBA2F;
;*	unsigned int Q; /* the quotient */
;*
;*	Q = ((A * 0xBA2F) >> 20)
;*
;* Uses: high regs: 7 (r17, r18, r19, X, Y)
;*	 low regs:  3 (r0, r1, r2)
;*
;*	 words:     37 (w. push/pop = 8 words)
;*	 cycles:    47 (w. push/pop = 16 cycles)
;*
;* Note: Hardware multiplier required ("mul" instruction)
;*
;***************************************************************************

Div16_22:
	push	r2
	push	r19
	push	r18
	push	r17

	ldi	YH,0xBA		; scaled reciprocal for /11, /22
	ldi	YL,0x2F

	; Q = A * 0xBA2F
	; (r19:r18:r17[:rXX] = XH:XL * YH:YL)
        clr     r2
        mul     XH, YH		; ah * bh
        movw    r19:r18, r1:r0
        mul     XL, YL		; al * bl
	mov	r17,r1		; r0 to [rXX] is superfluous
        mul     XH, YL		; ah * bl
        add     r17, r0
        adc     r18, r1
        adc     r19, r2
        mul     YH, XL		; bh * al
        add     r17, r0
        adc     r18, r1
        adc     r19, r2

	; Q = Q >> 16: use r19:r18 as word
        ; Q = Q >> 4
	swap	r19		; do the last 4 shifts
	mov	XH,r19
	andi	XH,0xF0
	andi	r19,0x0F
	swap	r18
	andi	r18,0x0F
	add	r18,XH

	; r19:r18 now "Q" (= result >> 20)
	; R = A - 22*Q;
	ldi	r17,22		; multiply r19:r18 by 22
	mul     r18, r17	; al * bl
	sub	XL,r0
	clr	XH
	movw	YL,r18		; make copy of Q

	; XL holds "R"
	; YH:YL holds "Q"
	pop	r17
	pop	r18
	pop	r19
	pop	r2

	ret

;**** End of function Div16_22 -----------------------------------------****


;***************************************************************************
;*
;* Function "Div16_23"
;* Divides an unsigned 16 bit word (XH:XL) by 23
;* Call with 16 bit number in XH:XL
;* Returns quotient in YH:YL and remainder in XL
;*
;* Author: Andreas Lenze (andreas.lenze@t-online.de)
;* (Equations partly by D. W. Jones)
;*
;*      Reciprocal multiplication w. extra precision:
;*	(uses correction to achieve the required precision)
;*      unsigned int R; /* remainder */
;*      unsigned int long A; /* dividend */
;*      unsigned int long Q; /* quotient */
;*
;*      Q = ((A * 0xB215) >> 20)
;*
;*      /* Q = A/23 or Q+1 = A/23 for all A <= 65535 */
;*      /* correct Q and calculate remainder */
;*      R = A - 1*Q
;*      if (R >= 23) {
;*              R = R - 23;
;*              Q = Q + 1;
;*      }
;* Uses: high regs: 7 (r17, r18, r19, X, Y)
;*       low regs:  3 (r0, r1, r2)
;*
;*       words:     37 (w. push/pop = 8 words)
;*       cycles:    60 (w. push/pop = 16 cycles)
;*
;* Note: Hardware multiplier required ("mul" instruction)
;*
;***************************************************************************

Div16_23:
        push    r2
        push    r19
        push    r18
        push    r17

        ldi     YH,0xB2         ; scaled reciprocal for /23
        ldi     YL,0x15		; (16 bit only, 0/-1 error possible)

        ; Q = A * 0xB215
	; (r19:r18:r17[:rXX] = XH:XL * YH:YL)
        clr     r2
        mul     XH, YH		; ah * bh
        movw    r19:r18, r1:r0
        mul     XL, YL		; al * bl
	mov	r17,r1		; r0 to [rXX] is superfluous
        mul     XH, YL		; ah * bl
        add     r17, r0
        adc     r18, r1
        adc     r19, r2
        mul     YH, XL		; bh * al
        add     r17, r0
        adc     r18, r1
        adc     r19, r2

        ; Q = Q >> 16: use r19:r18 as word
        ; Q = Q >> 4
	swap	r19		; do the last 4 shifts
	mov	XH,r19
	andi	XH,0xF0
	andi	r19,0x0F
	swap	r18
	andi	r18,0x0F
	add	r18,XH

        ; r19:r18 now "Q" (= result >> 20)
        ; R = A - 23*Q;
	ldi	r17,23		; multiply r19:r18 by 23
	mul     r18, r17	; al * bl
	sub	XL,r0
	clr	XH
	movw	YL,r18		; make copy of currentQ

        ; XH:XL now "R":
        ; if (R >= 23)		; apply correction
        ;       R = R - 23;
        ;       Q = Q + 1;
        cpi     XL,0x17
        brlo    PC+3
        subi    XL,23
        adiw    YL,1

	; XL holds "R"
        ; YH:YL holds "Q"
        pop     r17
        pop     r18
        pop     r19
        pop     r2

        ret

;**** End of function Div16_23 -----------------------------------------****


;***************************************************************************
;*
;* Function "Div16_60"
;* Divides an unsigned 16 bit word (XH:XL) by 60
;* Returns quotient in YH:YL and remainder in XL
;*
;* Author: Andreas Lenze (andreas.lenze@t-online.de)
;* Equations by D. W. Jones:
;*
;*	Reciprocal mul w. extra precision:
;*	unsigned int A;
;*	unsigned int scaled_reciprocal = 0xCCCD;
;*	unsigned int Q; /* the quotient */
;*
;*	Q = ((A * 0x8889) >> 21)
;*
;* Uses: high regs: 7 (r17, r18, r19, X, Y)
;*	 low regs:  3 (r0, r1, r2)
;*
;*	 words:     37 (w. push/pop = 8 words)
;*	 cycles:    47 (w. push/pop = 16 cycles)
;*
;* Note: Hardware multiplier required ("mul" instruction)
;*
;***************************************************************************

Div16_60:
	push	r2
	push	r19
	push	r18
	push	r17

	ldi	YH,0x88		; scaled reciprocal for /60
	ldi	YL,0x89

	; Q = A * 0x8889
	; (r19:r18:r17[:rXX] = XH:XL * YH:YL)
        clr     r2
        mul     XH, YH		; ah * bh
        movw    r19:r18, r1:r0
        mul     XL, YL		; al * bl
	mov	r17,r1		; r0 to [rXX] is superfluous
        mul     XH, YL		; ah * bl
        add     r17, r0
        adc     r18, r1
        adc     r19, r2
        mul     YH, XL		; bh * al
        add     r17, r0
        adc     r18, r1
        adc     r19, r2

	; Q = Q >> 16: use r19:r18 as word
	; Q = Q >> 5
	lsr	r19		; do 1 shift
	ror	r18
	swap	r19		; do the last 4 shifts
	mov	XH,r19
	andi	XH,0xF0
	andi	r19,0x0F
	swap	r18
	andi	r18,0x0F
	add	r18,XH

	; r19:r18 now "Q" (= result >> 21)
	; R = A - 60*Q;
	ldi	r17,60		; multiply r19:r18 by 60
	mul     r18, r17	; al * bl
	sub	XL,r0
	clr	XH
	movw	YL,r18		; make copy of Q

	; XL holds "R"
	; YH:YL holds "Q"
	pop	r17
	pop	r18
	pop	r19
	pop	r2

	ret

;**** End of function Div16_60 -----------------------------------------****



;******************************************************************************************
; macro definition to call/use "division by xx" - module (D16_nn)
macro Div16_by
	push	r20
	ldi	r20,@0
	call	D16_nn
	pop	r20
.endm
;******************************************************************************************


;******************************************************************************************
;*
;* Function "D16_nn"
;* Divides an unsigned 16 bit word by [2] -> [23]
;* Note: divisor 2, 4, 8, 16 options are provided for remainder calculation
;*	 and (for ease-of-use) to cover the complete divisors range (2-23)
;*
;* Call with dividend loaded to XH:XL (high/low bytes)
;* Returns quotient in YH:YL and remainder in XL
;*
;* Usage: define the macro "Div16_by" prior to using the function,
;*	  use macro with divisor as parameter, e.g. "Div16_by 17" to
;*	  divide XH:XL by 17 decimal
;*
;*		.macro Div16_by
;*			push	r20
;*			ldi	r20,@0
;*			call	D16_nn
;*			pop	r20
;*		.endm
;*
;* Author: Andreas Lenze (andreas.lenze@t-online.de)
;* Feb. 2003, 2005
;* Equations mostly by D. W. Jones
;*
;*	Reciprocal mul w. extra precision:
;*	unsigned int A;
;*	unsigned int scaled_reciprocal = xxxx
;*	unsigned int Q; /* the quotient */
;*
;*	Q = ((A * scaled_reciprocal) >> 16) >> nn
;* 	or
;*	Q = (((A * scaled_reciprocal) >> 16) + A) >> nn -> for /7 and /14
;*
;*	/* special case: use correction for Q (e.g. for /21, /23) */
;*          if (R >= divisor)
;*                R = R - divisor;
;*                Q = Q + 1;
;*
;*	div. by	n: scaled reciprocal:     shift count:				       cycles:
;*
;*		2  -			  1						78
;*		3  1010101010101011 AAAB  17						93
;*		4  -			  2						80
;*		5  1100110011001101 CCCD  18						95
;*		6  1010101010101011 AAAB  18						95
;*		7  10010010010010011      19 -> 17 bits req'd,(MSB=1,rest 2493h)       105
;*		8  -			  3						78
;*		9  1110001110001111 E38F  19						93
;*		10 1100110011001101 CCCD  19						93
;*		11 1011101000101111 BA2F  19						93
;*		12 1010101010101011 AAAB  19						93
;*		13 1001110110001010 9D8A  19						93
;*		14 10010010010010011      20 -> 17 bits req'd,(MSB=1,rest 2493h)       106
;*		15 1000100010001001 8889  19						93
;*		16 -			  4						80
;*		17 1111000011110001 F0F1  20						98
;*		18 1110001110001111 E38F  20						95
;*		19 0110101111001011 6BCB  19						95
;*		20 1100110011001101 CCCD  20						95
;*		21 1100001100001011 C30B  20 -> needs correction for accurate result	95
;*		22 1011101000101111 BA2F  20						95
;*		23 1011001000010101 B215  20 -> needs correction for accurate result	95
;*
;* Uses: high regs: 11 (r16, r17, r18, r19, r20, X, Y, Z)
;*	 low regs:  3 (r0, r1, r2)
;*	 regs 16-20 saved, low regs and X, Y, Z destroyed
;*
;*	 words:       91 (incl. 5 words for macro)
;*	 table bytes: 66
;*	 cycles:      78-107 (w. call/ret, reg. saves & macro), typically
;*		      94 or 95, push/pops, saves & macro 27 cycles overhead
;*
;* Target: AVR MCUs with hardware multiplier ("mul" instruction and
;*	   "lpm rd,Z/Z+" functionality required)
;*
;******************************************************************************************


D16_nT:
; look-up table for D16_nn: 3 bytes per entry, range for divisor "2" to "23"
; data format: 2 bytes scaled reciprocal (word, high/low), 3rd byte "flags"
.cseg
.db 0x00, 0xFF, 0x01, 0xAA, 0xAB, 0x01, 0x00, 0xFF, 0x02, 0xCC, 0xCD, 0x02, 0xAA, 0xAB
;by /2                /3                /4                /5                /6

.db 0x02, 0x24, 0x93, 0x13, 0x00, 0xFF, 0x03, 0xE3, 0x8F, 0x03, 0xCC, 0xCD, 0x03, 0xBA
;by       /7                /8                /9                /10               /11

.db 0x2F, 0x03, 0xAA, 0xAB, 0x03, 0x9D, 0x8A, 0x03, 0x24, 0x93, 0x14, 0x88, 0x89, 0x03
;by              /12               /13               /14               /15

.db 0x00, 0xFF, 0x04, 0xF0, 0xF1, 0x04, 0xE3, 0x8F, 0x04, 0x6B, 0xCB, 0x03, 0xCC, 0xCD
;by /16               /17               /18               /19               /20

.db 0x04, 0xC3, 0x0B, 0x04, 0xBA, 0x2F, 0x04, 0xB2, 0x15, 0x04
;by       /21		    /22		      /23

D16_nn:
        push	r19		; save scrap regs
	push	r18
        push	r17
        push	r16

	push	r20		; save divisor
	cpi	r20,24		; exit if divisor > 23
	brsh	D16_0
	subi	r20,2		; 'normalize' divisor for table
	brmi	D16_0		; exit if divisor was <= 1

	ldi	ZH,high(D16_nT*2)
	ldi	ZL,low(D16_nT*2)
	clr	r2
	mov	r1,r20
	lsl	r20		; x2 (3 bytes per entry)
	add	r20,r1		; + org value = x3
	add	ZL,r20		; point Z to divisor's data table position
	adc	ZH,r2

	lpm	YH,Z+		; scaled reciprocal for /xx
	lpm	YL,Z+		; Z now points to flags
	cpi	YL,0xFF		; low byte "FFh" in the scaled_reciprocal data
				; indicates divisor 2, 4, 8, 16 -> no 'mul' req.
	brne	D16_1		; != FFh -> mul required
				; FFh: no div., shifts only: move input
	movw	r18,XL		; to "result registers" and go to shifts
	rjmp	D16_2		; directly (saves ~15 cycles)

D16_0:	pop	r20		; clean up stack before exit
	rjmp	D16_Err		; intermediate label (avoid "out of reach" for brxx)

	; Q = A * scaled_reciprocal
	; (r19:r18:r17:r16 = XH:XL * YH:YL)
D16_1:	clr     r2
        mul     XH, YH		; ah * bh
        movw    r19:r18, r1:r0
        mul     XL, YL		; al * bl
	mov	r17,r1		; r0 to r16 is superfluous
        mul     XH, YL		; ah * bl
        add     r17, r0
        adc     r18, r1
        adc     r19, r2
        mul     YH, XL		; bh * al
        add     r17, r0
        adc     r18, r1
        adc     r19, r2

	; Q = Q >> 16: use r19:r18 as word
	; do the remaining shifts
D16_2:	lpm	r20,Z		; fetch "flag"

	cpi	r20,3		; flag = 3 -> 3 normal shifts required
	breq	D16_5		; (div. 8, 9, 10, 11, 12, 13, 15, 19)

	cpi	r20,4		; flag = 4 -> 4 normal shifts required
	breq	D16_3		; (div. 16, 17, 18, 20, 21, 22, 23)

	cpi	r20,2		; flag = 2 -> 2 normal shifts required
	breq	D16_6		; (div. 4, 5, 6)

	cpi	r20,1		; flag = 1 -> 1 normal shift required
	breq	D16_7		; (div. 2, 3)

	sbrs	r20,4		; bit 4 set if flag = 4spec or 3spec
				; 4spec (0x14) -> 4 special shifts required
				; (div. 14, extra addition, 1st shift w. carry)
				; 3spec (0x13) -> 3 special shifts required
				; (div. 7, extra addition, 1st shift w. carry)
	rjmp	D16_Err		; no valid flags, exit
	rjmp	D16_4		; continue

	; Q = Q >> 4
D16_3:	swap	r19		; 4 normal shifts
	mov	r16,r19
	andi	r16,0xF0
	andi	r19,0x0F
	swap	r18
	andi	r18,0x0F
	add	r18,r16
	rjmp	D16_8

	; Q = (Q + A) >> 3-4
D16_4:	add	r18,XL		; (Q + A)
	adc	r19,XH
	ror	r19		; 3-4 "special" shifts, include
	ror	r18		; carry from previous addition into 1st shift
D16_5:	lsr	r19
	ror	r18
D16_6:	lsr	r19
	ror	r18
	cpi	r20,0x13	; flag = 3spec -> 3 special shifts required
	breq	D16_8		; if flag = 3spec, skip fourth shift
D16_7:	lsr	r19
	ror	r18

	; r19:r18 now "Q" (= result >> yy)
	; R = A - xx*Q;
D16_8:	pop	r16		; reclaim original divisor,
				; multiply r19:r18 by divisor
	mul     r18, r16	; al * bl
	sub	XL,r0
	clr	XH
	movw	YL,r18		; make copy of Q

	; XL = "R" (remainder)
	; /* use correction - can be omitted if /21, /23 are not used */
        ; if (R >= divisor)
        ;       R = R - divisor;
        ;       Q = Q + 1;
        cp      XL,r16
        brlo    PC+3
        sub     XL,r16
        adiw    YL,1
	; YH:YL = "Q" (quotient, result)

D16_Err:
	pop     r16		; restore regs
        pop     r17
	pop     r18
        pop     r19
	ret

;**** End of function D16_nn ----------------------------------------------------------****

