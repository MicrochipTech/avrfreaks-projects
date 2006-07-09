;***************************************************************************
;*
;* "BCDsub" - 6-digit packed BCD subtraction
;*
;* This subroutine subtracts the two unsigned 2-digit BCD numbers
;* "BCD0" and "BCD0a" (BCD0 - BCD0a). The result is returned in "BCD0", and
;* the underflow carry in "BCD0a".
;*
;***************************************************************************

BCDsub:
	sub		BCD0,BCD0a				;subtract the numbers binary
	clr		BCDt
	brcc	BCDsub_1				;if carry not clear
	ldi		BCDt,1					;store carry in BCD0t, bit 0
BCDsub_1:
	brhc	BCDsub_2				;if half carry not clear
	subi	BCD0,0x06				;LSD = LSD - 6
BCDsub_2:
	sbrs	BCDt,0					;if previous carry not set
	rjmp	BCDsub_3
	subi	BCD0,0x60				;subtract 6 from MSD
	ldi		BCDt,1					;set underflow carry
	brcc	BCDsub_3				;if carry not clear
	ldi		BCDt,1					;clear underflow carry	
BCDsub_3:	
	rjmp	BCDsub
	ret			


