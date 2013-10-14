;********************************************************
; This file holds the operator commands.
;********************************************************
MULTIPLYcommand:
	rcall	GetTwoParm			;get two parameters from dstack
	mul		U,temp
	tst		r1					;check for 8-bit overflow
	SKBC	GPIOR1,AOVErr,temp	;skip next if AOVErr disabled
	brne	multiplycommand9	;branch if overflow
	mov		U,r0
	GoSub	pushU
	ret

multiplycommand9:
	STOREI	ERRCode,$0F			;error code "F"
	Goto	error
;
;*************************************	
;*	drem8u		=H			;remainder
;*	dres8u		=T			;result
;*	dd8u		=T			;dividend
;*	dv8u		=temp		;divisor
;*	dcnt8u		=U			;loop counter
;*************************************	
DIVIDEcommand:
MODULUScommand:
	push	U					;save the operator we used to get here
	rcall	Get1stParm			;get the last parameter
	tst		U					;test for divide by zero
	SKBC	GPIOR1,AOVErr,r0	;skip next if AOVErr disabled
	breq	dividecommand9		;branch if divide by zero
	mov		temp,U		
	GoSub	popU				;divisor 2nd off stack
	mov		T,U

	clr		H					;clear remainder and carry
	ldi		U,9					;init loop counter
dividecommand1:	
	rol		T					;shift left dividend
	dec		U					;decrement counter
	breq	dividecommand4		;if done
dividecommand2:	
	rol		H					;shift dividend into remainder
	sub		H,temp				;remainder = remainder - divisor
	brcc	dividecommand3		;if result negative
	add		H,temp				;restore remainder
	clc							;clear carry to be shifted into result
	rjmp	dividecommand1		;else
dividecommand3:	
	sec							;set carry to be shifted into result
	rjmp	dividecommand1

dividecommand4:	
	pop		U					;get out operator back
	cpi		U,('/' & $5F)		;was it a "/" (divide)?
	brne	dividecommand4a		;no
	mov		U,T					;get result into U (divide)
	rjmp	dividecommand4b

dividecommand4a:	
	mov		U,H					;get remainder into U (modulus)

dividecommand4b:				;save result to data stack
	GoSub	pushU
	ret

dividecommand9:
	STOREI	ERRCode,$11			;error code "11"
	Goto	error
;
;*************************************	
ADDcommand:
	rcall	GetTwoParm			;get two parameters from dstack
	add		U,temp
	SKBC	GPIOR1,AOVErr,temp	;skip next if AOVErr disabled
	brcs	addcommand9			;branch if overflow
	GoSub	pushU
	ret

addcommand9:
	STOREI	ERRCode,$0F			;error code "F"
	Goto	error
;
;*************************************	
SUBTRACTcommand:
	rcall	GetTwoParm			;get two parameters from dstack
	sub		U,temp
	SKBC	GPIOR1,AOVErr,temp	;skip next if AOVErr disabled
;	brvs	subtractcommand9	;branch if underflow
	brcs	subtractcommand9	;branch if underflow/carry
	GoSub	pushU
	ret

subtractcommand9:
	STOREI	ERRCode,$10			;error code "10"
	Goto	error
;
;******************************************************
EQUALcommand:		;Returns "1" if true, else "0"
	rcall	GetTwoParm			;get two parameters from dstack
	cp		U,temp
	SKBS	SREG,SREG_Z,temp	;skip next if Z set
	rjmp	makeU0				;set U to "0"
	rjmp	makeU1				;set U to "1"
;
;******************************************************
NOTEQUALcommand:	;Returns "1" if true, else "0"
	rcall	GetTwoParm			;get two parameters from dstack
	cp		U,temp
	SKBS	SREG,SREG_Z,temp	;skip next if Z set
	rjmp	makeU1				;set U to "1"
	rjmp	makeU0				;set U to "0"
;
;*************************************	
DIFFcommand:
	push	U					;save operator on stack
	rcall	GetTwoParm			;get two parameters from dstack

	pop		T					;restore operator
	cpi		T,('<' & $5F)		;is it greater-than?
	brne	diffcommand1	;no, branch 
	cp		U,temp				;test for less-than
	rjmp	diffcommand2

DIFFcommand1:
	cp		temp,U				;test for greater-than

DIFFcommand2:
	brcs	makeU1
;	
;Supports the DIFFcommand ******************************
makeU0:	;sets U to return a "0"
	clr		U
	sbrc	U,0					;skip next if U = 0
;
makeU1:	;sets U to return a "1"
	ldi		U,1
;
makeUx:
	GoSub	pushU
	ret
;
;*************************************	
ANDcommand:
	rcall	GetTwoParm			;get two parameters from dstack
	and		U,temp
	GoSub	pushU
	ret
;
;*************************************	
LSRcommand:
	GoSub	LogShftSetup		;setup the registers
	tst		T					;did user want "0" for shift?
	breq	LSRcommand2			;yes, just pass original value back

LSRcommand1:
	lsr		U					;shift right
	dec		T					;decrement counter
	brne	LSRcommand1			;loop till zero

LSRcommand2:
	GoSub	pushU
	ret
;
;*************************************	
LSLcommand:
	GoSub	LogShftSetup		;setup the registers
	tst		T					;did user want "0" for shift?
	breq	LSLcommand2			;yes, just pass original value back

LSLcommand1:
	lsl		U					;shift left
	dec		T					;decrement counter
	brne	LSLcommand1			;loop till zero

LSLcommand2:
	GoSub	pushU
	ret
;
;************************************************************************
;LogShftSetup: Supports the logical shift operators by pulling the data
;	and shift count off the datastack.  If the shift count is invalid
;	then an error is issued.
;	- Returns with value to shift in U, shift count less one in T
;************************************************************************
LogShftSetup:
	rcall	Get1stParm			;get the last parameter
	ldi		T,1					;T = 1 to start
	tst		DStkPtr				;zero bytes on data stack?
	brne	LogShftSetup1 		;branch if there is data on dstack
	ret	

LogShftSetup1:
	ldi		temp,7				;only values of [0..7] allowed
	cp		temp,U
	brmi	LSSerror			;branch if allowable exceeded 

;	dec		U					;# of bit positions less one
	mov		T,U					;move count to register
	GoSub	popU				;fetch U
	ret							;return to caller
;
LSSerror:	;error, too many bit positions specified
	STOREI	ERRCode,$0A		;error code "A"
	Goto	error
;
;*************************************	
ORcommand:
	rcall	GetTwoParm			;get two parameters from dstack
	or		U,temp
	GoSub	pushU
	ret
;
;*************************************	
XORcommand:
	rcall	GetTwoParm			;get two parameters from dstack
	eor		U,temp
	GoSub	pushU
	ret
;
;*************************************	
COMcommand:
	rcall	Get1stParm			;get the last parameter
	com		U
	GoSub	pushU
	ret
;
;*************************************	
NEGcommand:
	rcall	Get1stParm			;get the last parameter
	neg		U
	GoSub	pushU
	ret
;
;*****************************************************************	
; SWAPcommand:	SWAP the high and low nibbles
SWAPcommand:
	rcall	Get1stParm			;get the last parameter
	swap	U					;swap nibbles
	GoSub	pushU				;save data
	ret
;
;*****************************************************************	
;NBHcommand:	Return the high nibble
NBHcommand:
	rcall	Get1stParm			;get the last parameter
	swap	U					;swap nibbles
	andi	U,0x0F				;strip off high nibble
	GoSub	pushU				;save data
	ret
;
;*****************************************************************	
;NBLcommand:	Return the low nibble
NBLcommand:
	rcall	Get1stParm			;get the last parameter
	andi	U,0x0F				;strip off high nibble
	GoSub	pushU				;save data
	ret
;
;****************************************************************************
;CRBcommand: calculates the CRC-8 of a series of data passed on the dstack.
;CRWcommand: calculates the CRC-16 of a series of data passed on the dstack.
;	If there are no arguments passed, then the data in the data statement
;	  buffer is used.
;	Returns with:
;	 CRC8 on dstack
;	 CRC16 on dstack as CRC16L, CRC16H
;****************************************************************************
CRCcommand:
.if	CRC16R
	clt							;clear T-Flag for CRC8
	rjmp	CRCcommand0			;calculate

CRWcommand:
	set							;set T-Flag for CRC16
.endif

CRCcommand0:
	PUSHZ						;save Z-Ptr
	clr		r1	
	GoSub	ChkCmdParms			;interpret line, tst for zero parms
	brne	CRCcommand3			;parameters on dstack, continue
	
CRCcommand1:	;no command line parms, use DATA buffer
	LOAD	r0,DATAptr+1		;fetch number of bytes in DATA buffer
	tst		r0					;r0 = 0?
	brne	CRCcommand2

	STOREI	ERRCode,$07			;value not available
	Goto	error

CRCcommand2:
	SetZPtr	READSTOR+READSTKSZ	;Z = DATA buffer
	rjmp	CRCcommand5			;calculate CRC-8/CRC-16

CRCcommand3:
	ldi		temp,DATASTKSZ		;max DATASTKSZ parms in direct mode
	cp		cmdparms,temp		;too many parms?
	brmi	CRCcommand4			;good

	STOREI	ERRCode,$06			;too many arguments
	Goto	error

CRCcommand4:
	mov		r0,cmdparms			;copy of # of parms into r0
	SetZPtr	DATASTK+DATASTKSZ	;Z = dstack

CRCcommand5:
.if	CRC16R
	brts	CRCcommand6			;CRC16 if T-flag set
.endif
	rcall	CRC8				;calculate CRC-8
.if	CRC16R
	rjmp	CRCcommand9			;save CRC8 and exit

CRCcommand6:
	rcall	CRC16				;calculate CRC-16
	mov		U,H					;save CRC16H
	GoSub	pushU				;save value on dstack
	mov		U,T					;save CRC16L
.endif

CRCcommand9:
	sub		DStkPtr,cmdparms	;adjust DStkPtr
	GoSub	pushU				;save value on dstack
	POPZ						;restore Z-Ptr
	ret
;
;****************************************************************************
;CRC8: calculates the CRC-8 of a series of data ( x^8 + x^5 + x^4 + 1 ).
;	- Call with Y pointing to start of data in RAM.
;	- Call with byte count in r1:r0
;	- Returns with CRC-8 in U
;	Destroys temp, U, H, T, inchar, and Z pointers.
;****************************************************************************
CRC8:
	clr		U						;zero crc accumulator 
	ldi		H,CRC8POLY				; CRC8POLY = 0x18

CRC8a:								;Y points to data in RAM
	ld		temp,-Z					;load data and decrement pointer
	ldi		T,8						;load bit counter

CRC8b:
	mov		inchar,temp				;feedback_bit = (crc ^ data) & 0x01
	eor		inchar,U
	sbrc	inchar,bit0    			;skip next if (feedback_bit != 1)
	eor		U,H						;crc = crc ^ CRC8POLY;

CRC8c:
  	lsr		U						;crc = (crc >> 1) & 0x7F;
	sbrc	inchar,bit0    			;skip next if (feedback_bit != 1)
	ori		U,0x80					;crc = crc | 0x80;

CRC8d:
	dec		T						;decrement bit counter
	breq	CRC8e      				;loop till (bit_counter > 0)

	lsr		temp					;data = data >> 1
	rjmp	CRC8b

CRC8e:
	dec		r0						;decrement byte counter
	brne	CRC8a					;loop till byte count reached

	ret								;return w/ crc in U
;
.if CRC16R
;****************************************************************************
;CRC16: calculates the CRC-16 of a series of data.  Set the polynomial and
; initial value in Include/Defs_Constants.inc
;	- Call with Y pointing to start of data in RAM.
;	- Call with byte count in r1:r0
;	- Returns with CRC-16 in H:T
;	Destroys temp, H, T, inbyteh, inbytel and Z pointers.
;****************************************************************************
; Author: John C. Wren
; Origin: http://www.kd4dts.com/download/zip/crc16.zip  [Crc16.asm] {broken link}
; Modified from MCS-51 to AVR by Göran Nordahl on 2000-0717
; Modified for AttoBASIC by K. Scott Vitale on 2013-1007
;****************************************************************************
CRC16:
	ldi		H,HIGH(CRC16Init)	;initialize CRC16 accumulator
	ldi		T,LOW(CRC16Init)

CRC16a:
	ldi		temp,8				;seed bit counter
	mov		bytecntr,temp
	ld		U,-Z				;load data and decrement pointer
	eor		H,U

CRC16b:
	lsl		T					;shift a bit position
	rol		H
	brcc	CRC16c				;branch if MSB = 0
	ldi		temp,HIGH(CRC16Poly);use high byte of polynomial 
	eor		H,temp
	ldi		temp,LOW(CRC16Poly)	;use low byte of polynomial 
	eor		T,temp
CRC16c:
	dec		bytecntr			;decrement bit counter
	brne	CRC16b				;loop till zero

	dec		r0					;decrement byte counter
	brne	CRC16a				;loop till byte count reached

	ret
.endif
;
;*****************************************************************	
; Get1stParm: used by many commands to interpret the command line
;	parameters and return with the last value in U
;*****************************************************************	
Get1stParm:
	GoSub	interpretlinev
	GoSub	popU				;fetch data
	ret
;
;*****************************************************************	
; Calls line interpreter, fetches 1st parameter, places it in 
;	temp, fetches the 2nd parameter and returns with it in U. 
;*****************************************************************	
GetTwoParm:
	rcall	Get1stParm		;fetch 1st parameter
	mov		temp,U			;save in temp
	GoSub	popU			;fetch 2nd parameter
	ret
;

