
;***************************************************************************
;*
;* "mpy16u" - 16x16 Bit Unsigned Multiplication
;*
;* This subroutine multiplies the two 16-bit register variables 
;* temp3:temp2 and temp1:temp.
;* The result is placed in column:row:temp3:temp2.
;*  
;* Number of words	:14 + return
;* Number of cycles	:153 + return
;* Low registers used	:None
;* High registers used  :7 (temp2,temp3,temp/temp2,temp1/temp3,row,
;*                          column,char)	
;*
;***************************************************************************

;***** Subroutine Register Variables

;.def	temp	=r16		;multiplicand low byte
;.def	temp1	=r17		;multiplicand high byte
;.def	temp2	=r18		;multiplier low byte
;.def	temp3	=r19		;multiplier high byte
;.def	temp2	=r18		;result byte 0 (LSB)
;.def	temp3	=r19		;result byte 1
;.def	row	=r20		;result byte 2
;.def	column=r21		;result byte 3 (MSB)
;.def	char	=r22		;loop counter

;***** Code

mpy16u:	clr	column		;clear 2 highest bytes of result
	clr	row
	ldi	char,16	;init loop counter
	lsr	temp3
	ror	temp2

m16u_1:	brcc	noad8		;if bit 0 of multiplier set
	add	row,temp	;add multiplicand Low to byte 2 of res
	adc	column,temp1	;add multiplicand high to byte 3 of res
noad8:	ror	column		;shift right result byte 3
	ror	row		;rotate right result byte 2
	ror	temp3		;rotate result byte 1 and multiplier High
	ror	temp2		;rotate result byte 0 and multiplier Low
	dec	char		;decrement loop counter
	brne	m16u_1		;if not done, loop more
	ret
