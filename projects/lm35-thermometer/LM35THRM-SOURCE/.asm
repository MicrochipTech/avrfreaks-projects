
;***********************************************************************
;*BINARY TO PACKED BCD CONVERTION ROUTINE
;*********************************************************************** 
;*FIRST WE SUBTRACT OUR 16BIT NUMBER BY 1000 IN A LOOP
;*THE NUMBER OF LOOPS INDICATES THE NUMBER OF 1000
;*SECOND SUBTRACT THE NUMBER BY 100 AND THEN SUBTRACT THE 
;*NUMBER BY 10  AND THE REMAINDER WILL BE THE FIRST DIGIT!


BIN2PBCD:
	CLR		PBCD_BYTEH
	LDI		TEMP,5
	MUL 	HEX_DIGITL,TEMP
	MOV 	HEX_DIGITL,R0
	MOV		R2,R1
	MUL		HEX_DIGITH,TEMP
	ADD		R0,R2
	MOV		HEX_DIGITH,R0
	MOV		DD16UL,HEX_DIGITL
	MOV		DD16UH,HEX_DIGITH
	LDI		DV16UL,$E8
	LDI		DV16UH,$3
	RCALL	DIV16U
	MOV		PBCD_BYTEH,DRES16UL
	MOV		DD16UL,DREM16UL
	MOV		DD16UH,DREM16UH
	LDI		DV16UH,0
	LDI		DV16UL,$64
	RCALL	DIV16U
	SWAP	PBCD_BYTEH
	OR		PBCD_BYTEH,DRES16UL
	MOV		DD16UL,DREM16UL
	MOV		DD16UH,DREM16UH
	LDI		DV16UH,0
	LDI		DV16UL,10
	RCALL	DIV16U
	MOV		PBCD_BYTEL,DRES16UL
	SWAP	PBCD_BYTEL
	OR		PBCD_BYTEL,DREM16UL
	RCALL	RIGHT_ADJUST
	RET
;***********************************************************************
;* RIGHT ADJUST ROUTINE
;* REMOVES ZERO LEADING FROM THE LEFT SIDE OF AN INTEGER
;***********************************************************************
RIGHT_ADJUST:
	MOV		TEMP,PBCD_BYTEH
	ANDI	TEMP,0XF0
	TST		TEMP
	BRNE	END_OF_RIGHT_ADJUST
	ORI		PBCD_BYTEH,0XA0
	MOV		TEMP,PBCD_BYTEH
	ANDI	TEMP,0X0F
	TST		TEMP
	BRNE	END_OF_RIGHT_ADJUST
	ORI		PBCD_BYTEH,0X0A
END_OF_RIGHT_ADJUST:
	RET	    

;***************************************************************************
;*
;* "div16u" - 16/16 Bit Unsigned Division
;*
;* This subroutine divides the two 16-bit numbers 
;* "dd8uH:dd8uL" (dividend) and "dv16uH:dv16uL" (divisor). 
;* The result is placed in "dres16uH:dres16uL" and the remainder in
;* "drem16uH:drem16uL".
;*  
;* Number of words	:19
;* Number of cycles	:235/251 (Min/Max)
;* Low registers used	:2 (drem16uL,drem16uH)
;* High registers used  :5 (dres16uL/dd16uL,dres16uH/dd16uH,dv16uL,dv16uH,
;*			    dcnt16u)
;*
;***************************************************************************

;***** Subroutine Register Variables

.def	drem16uL=r14
.def	drem16uH=r15
.def	dres16uL=r16
.def	dres16uH=r17
.def	dd16uL	=r16
.def	dd16uH	=r17
.def	dv16uL	=r18
.def	dv16uH	=r19
.def	dcnt16u	=r20

;***** Code

div16u:	clr	drem16uL	;clear remainder Low byte
	sub	drem16uH,drem16uH;clear remainder High byte and carry
	ldi	dcnt16u,17	;init loop counter
d16u_1:	rol	dd16uL		;shift left dividend
	rol	dd16uH
	dec	dcnt16u		;decrement counter
	brne	d16u_2		;if done
	ret			;    return
d16u_2:	rol	drem16uL	;shift dividend into remainder
	rol	drem16uH
	sub	drem16uL,dv16uL	;remainder = remainder - divisor
	sbc	drem16uH,dv16uH	;
	brcc	d16u_3		;if result negative
	add	drem16uL,dv16uL	;    restore remainder
	adc	drem16uH,dv16uH
	clc			;    clear carry to be shifted into result
	rjmp	d16u_1		;else
d16u_3:	sec			;    set carry to be shifted into result
	rjmp	d16u_1
				