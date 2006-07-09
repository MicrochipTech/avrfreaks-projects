;***************************************************************************
;*
;* "BCDadd" - 2-digit packed BCD addition
;*
;* This subroutine adds the two unsigned 2-digit BCD numbers
;* "BCD0" and "BCD0a". The result is returned in "BCD0", and the overflow
;* carry in "BCD0a".
;*
;* Number of words	:21
;* Number of cycles	:23/25 (Min/Max)
;* Low registers used	:None
;* High registers used  :3 (BCD0,BCD0a,tmpadd)	
;*
;***************************************************************************

;***** Subroutine Register Variables

.def	BCD0	=r18		;BCD input value #1
.def	BCD0a	=r22		;BCD input value #2
.def	tmpadd	=r17		;temporary register

;***** Code
MAIN:
;	rcall	BCDadd
;	rjmp	main

BCDadd:
	add		BCD0,BCD0a	;add the numbers binary
	clr		BCD0a		;clear BCD carry
	brcc	BCDadd_0		;if carry not clear
	ldi		BCD0a,1		;    set BCD carry
BCDadd_0:
	brhs	BCDadd_1		;if half carry not set
	subi	BCD0,-6				;add 6 to LSD ("inverted add")
	brhc	BCDadd_2			;if half carry set (LSD <= 9)
	subi	BCD0,6		;        restore value
	rjmp	BCDadd_2				;else
BCDadd_1:
	subi	BCD0,-6					;add 6 to LSD ("inverted add")
BCDadd_2:
	brcc	BCDadd_3
	ldi		BCD0a,0	
BCDadd_3:
	subi	BCD0,-(0x60)			;add 6 to MSD ("inverted add")
	brcc	BCDadd_4				;if carry not set (MSD <= 9)
	sbrs	BCD0a,0					;    if previous carry not set
	subi	BCD0,$60				;	restore value
;	ret								;else
	rjmp	BCDadd
BCDadd_4:
	ldi		BCD0a,1		;    set BCD carry
	rjmp	BCDadd
;	ret

