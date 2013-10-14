 .if DATAREC	
;**********************************************************
;DIGcommand - Digital OUT for AVR Data Recorder
;**********************************************************
DIGcommand:
	rcall	Get1stParm		;get the last parameter
	cpi		U,1+1			;only "0" and "1" allowed
	brpl	DIGcommand9		;branch if allowable exceeded 

	SetZPtr	DIGPORT+$20		;Point Z to port (+ offset)
	mov		T,U				;copy to T
	ldi		U,(1<<DIGOUT)	;set bit mask
	sbrc	T,0				;skip next if clearing port bit
	rjmp	ORandSTORE		;OR U and data@port then store it
	rjmp	ANDandSTORE		;AND U and data@port then store it

DIGcommand9:
	STOREI	ERRCode,$0C			;error code "C"
	Goto	error
.endif
;
;***********************************************************
inDirectionX:
	rcall	GetPortOffset	;calculate port offset
.ifndef PORTH
	ADDI16	ZH,ZL,$21		;add $21 for DDRx offset
.else
	ADDI16	ZH,ZL,1			;add 1 for DDRx offset
.endif
	ld		U,Z				;get data into U
	Gosub	pushU			;save on the stack
	ret
;
;***********************************************************
outdXcommand:				;Output to DDRx
	rcall	GetPortOffset	;calculate port offset
	PUSHZ					;save z pointer
	rcall	Get1stParm		;get the last parameter
	POPZ					;restore Z
.ifndef PORTH
	ADDI16	ZH,ZL,$21		;add $21 for DDRx offset
.else
	ADDI16	ZH,ZL,1			;add 1 for DDRx offset
.endif
	st		Z,U				;store to port
	ret
;
;***********************************************************
setdirectionX:				;Set bit in position inidicated at top of stack.
	rcall	GetPortOffset	;calculate port offset
	PUSHZ					;save z pointer
	Gosub	interpretlinev
	Gosub	bitpositiontoormask
	Gosub	popU
	POPZ					;restore Z
.ifndef PORTH
	ADDI16	ZH,ZL,$21		;add $21 for DDRx offset
.else
	ADDI16	ZH,ZL,1			;add 1 for DDRx offset
.endif
	rjmp	ORandSTORE		;OR U and data@port then store it
;
;***********************************************************
cleardirectionX:			;Clear bit in position inidicated at top of stack.
	rcall	GetPortOffset	;calculate port offset
	PUSHZ					;save z pointer
	Gosub	interpretlinev
	Gosub	bitpositiontoormask
	Gosub	popU
	POPZ					;restore Z
.ifndef PORTH
	ADDI16	ZH,ZL,$21		;add $21 for DDRx offset
.else
	ADDI16	ZH,ZL,1			;add 1 for DDRx offset
.endif
	rjmp	ANDandSTORE		;AND U and data@port then store it
;
;***********************************************************
outpXcommand:				;Output to PORTx
	rcall	GetPortOffset	;calculate port offset
	PUSHZ					;save z pointer
	rcall	Get1stParm		;get the last parameter
	POPZ					;restore Z
.ifndef PORTH
	ADDI16	ZH,ZL,$22		;add $22 for PORTx offset
.else
	ADDI16	ZH,ZL,2			;add 2 for PORTx offset
.endif
	st		Z,U				;store to port
	ret
;
;***********************************************************
inXcommand:
	rcall	GetPortOffset	;calculate port offset
.ifndef PORTH
	ADDI16	ZH,ZL,$20		;add $20 for PINx offset
.endif
	ld		U,Z				;get data into U
	Gosub	pushU			;save on the stack
	ret
;
;***********************************************************
setbitinX:					;Set bit in position inidicated at top of stack.
	rcall	GetPortOffset	;calculate port offset
	PUSHZ					;save z pointer
	Gosub	interpretlinev
	Gosub	bitpositiontoormask
	Gosub	popU
	POPZ					;restore Z
.ifndef PORTH
	ADDI16	ZH,ZL,$22		;add $22 for PORTx offset
.else
	ADDI16	ZH,ZL,2			;add 2 for PORTx offset
.endif
	rjmp	ORandSTORE		;OR U and data@port then store it
;
;***********************************************************
clearbitinX:				;Clear bit in position inidicated at top of stack.
	rcall	GetPortOffset	;calculate port offset
	PUSHZ					;save z pointer
	Gosub	interpretlinev
	Gosub	bitpositiontoormask
	Gosub	popU
	POPZ					;restore Z
.ifndef PORTH
	ADDI16	ZH,ZL,$22		;add $22 for PORTx offset
.else
	ADDI16	ZH,ZL,2			;add 2 for PORTx offset
.endif
	rjmp	ANDandSTORE		;AND U and data@port then store it

;***********************************************************
xorbitinX:					;XOR bit in position inidicated at top of stack.
	rcall	GetPortOffset	;calculate port offset
	PUSHZ					;save z pointer
	Gosub	interpretlinev
	Gosub	bitpositiontoormask
	Gosub	popU
	POPZ					;restore Z
.ifndef PORTH
	ADDI16	ZH,ZL,$22		;add $22 for PORTx offset
.else
	ADDI16	ZH,ZL,2			;add 2 for PORTx offset
.endif
	rjmp	XORandSTORE		;XOR U and data@port then store it
;
;***********************************************************
testbitinX:					;Test bit in specified position indicated at top of stack.
	rcall	GetPortOffset	;calculate port offset
	PUSHZ					;save z pointer
	Gosub	interpretlinev
	Gosub	bitpositiontoormask
	Gosub	popU
	POPZ					;restore Z
.ifndef PORTH
	ADDI16	ZH,ZL,$20		;add $20 for PINx offset
.endif
	ld		temp,Z			;get PINx data into temp
	and		temp,U			;strip mask desired bit from PINx data
	ldi		U,0				;preload U = 0
	tst		temp			;PINx bit cleared?
	SKBS	SREG,SREG_Z,r0	;skip next if Z is set
	ldi		U,1				;set U = 1
	Gosub	pushU			;save on datastack
	ret
;
;****************************************************************************
GetPortOffset:				;U holds port [A..L]
	subi	U,'A'			;get port addressed by subtracting 'A'
.ifdef PORTH
	clr		r1				;clear r1
	SetZPtr	(2*PortOffsetTbl);point ZH:ZL to lookup table
	lsl		U				;multiply U by 2 for index
	ADD16	ZH,ZL,r1,U		;add to Z-Ptr
	lpm		r0,Z+			;fetch port low address to r0
	lpm		r1,Z			;fetch port high address to r1
.else
	ldi		temp,3			;load port to port offset address
	mul		U,temp			;get port index into r1:r0
.endif
	movw	ZH:ZL,r1:r0		;move r1:r0 into Z pointer
	ret						;return
;
;****************************************************************************
.ifdef PORTH
PortOffsetTbl:	;table of port locations for "GetPortOffset" routine
	.dw		PINA+32			;Port A
	.dw		PINB+32			;Port B
	.dw		PINC+32			;Port C
	.dw		PIND+32			;Port D
	.dw		PINE+32			;Port E
	.dw		PINF+32			;Port F
	.dw		PING+32			;Port G
	.dw		PINH			;Port H
	.dw		0x0000			;Port I (no port I ! )
	.dw		PINJ			;Port J
	.dw		PINK			;Port K
	.dw		PINL			;Port L
.endif
;
;****************************************************************************
ORandSTORE:					;Z holds port pointer.  OR U with data@port
	ld		temp,Z			;get data in register
	or		temp,U			;set the appropriate bit
	st		Z,temp			;save it back in the register
	ret
;
;****************************************************************************
ANDandSTORE:				;Z holds port pointer.  AND U with data@port
	com		U				;compliment for AND mask
	ld		temp,Z			;get data in register
	and		temp,U			;clear the appropriate bit
	st		Z,temp			;save it back in the register
	ret

;****************************************************************************
XORandSTORE:				;Z holds port pointer. XOR U with data@port
	ld		temp,Z			;get data in register
	eor		temp,U			;set the appropriate bit
	st		Z,temp			;save it back in the register
	ret
;
;****************************************************************************
