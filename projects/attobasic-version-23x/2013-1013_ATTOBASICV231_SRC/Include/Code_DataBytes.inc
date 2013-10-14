;*******************************************************************************
;DATAcommand DATA - saves DATA for later program use
;	- Uses Y, r1, r0, and temp
;	- Destroys r1, r0 and temp
;	- Preserves Y
;*******************************************************************************
DATAcommand:
	PUSHY						;save Y pointer
	GoSub	ChkCmdParms			;process line
	breq	DATAcommand9		;no data available, error
	ldi		temp,READSTKSZ+1	;get max stack elements
	cp		DStkPtr,temp		;is there too many elements?
	brsh	DATAcommand8		;branch if too many

	clr		r1					;zero register
	mov		r0,DStkPtr			;move # of elements to r0
	STORE	DATAptr+1,DStkPtr	;save the # of elements
	SetYPtr	READSTOR+READSTKSZ	;point to DATA space
	SUB16	YH,YL,r1,r0			;calculate offset for # of elements

DATAcommand1:
	GoSub	popU				;get a value
	st		Y+,U				;store in DATA space
	dec		r0					;decrement counter
	brne	DATAcommand1		;loop till 

	POPY						;restore Y
	rjmp	RESTcommand			;reset the DATA pointer and exit

DATAcommand8:	;too many arguments!
	GoSub	DATAPtrRst			;reset pointer & element count
	STOREI	ERRCode,$06			;error code "6"
	Goto	error

DATAcommand9:	;no arguments!
	GoSub	DATAPtrRst			;reset pointer & element count
	STOREI	ERRCode,$07			;error code "7"
	Goto	error
;	
;*******************************************************************************
DATAPtrRst:	;resets the DATA pointer and zero the element count
	STOREI	DATAptr+1,0			;zero the element count
	;falls into "RESTcommand" below
;
;*******************************************************************************
;RESTcommand: RESTore - resets the pointer to the DATA array and returns the 
;	number of bytes in the DATA attay.
;*******************************************************************************
RESTcommand:
	STOREI	DATAptr,0			;reset the DATA pointer to "0"
	LOAD	U,DATAptr+1			;fetch the current array size
	GoSub	pushU				;save it on the dstack
	ret
;
;*******************************************************************************
;READcommand: READ - read next data byte from DATA statement.  If there is a
;	value on the data stack, it is assumed to be an element number so it is
;	checked against the number of elements originally stored.
;*******************************************************************************
READcommand:
	PUSHY						;save Y
								; called
	GoSub	ChkCmdParms			;process line
	;set up some pointers first
	clr		r1					;zero a register
	LOAD 	H,DATAptr+1			;get the # of elements stored
	LOAD	T,DATAptr			;get the pointer into T
	SetYPtr	(READSTOR+READSTKSZ)-1;Y = TOS - 1

	tst		cmdparms			;is there an index provided?
	breq	READcommand2		;no index on data stack, continue

;process the index number
	sbrs	XL,VPTRSTK			;yes, process stack data as index
	rjmp	READcommand1		;no variable assignment, process as index
	mov		temp,DStkPtr		;get a copy dstack pointer
	dec		temp				;decrement by 1 to cover variable pointer
	breq	READcommand2		;if zero, process with no index

READcommand1:
	GoSub	popU				;get the index
	dec		H					;# of elements less 1, [0..N] inclusive
	cp		H,U					;is it a good index?
	brmi	READcommand9		;not a good index, error
	mov		T,U					;move index to T
	rjmp	READcommand3		;continue

READcommand2:	;check for end of data
	cp		H,T					;out of data?
	breq	READcommand9		;yes, out of data

READcommand3:	;fetch the data and save on dstack
	SUB16	YH,YL,r1,T			;point to data
	ld		U,Y					;fetch the data
	GoSub	pushU				;save on dstack
	inc		T					;increment pointer
	STORE	DATAptr,T			;save the pointer

READcommand7:
	POPY						;restore Y
	ret

READcommand9:	;;invalid index or out of data
	STOREI	ERRCode,$07			;error code "7"
	Goto	error
;	
;*******************************************************************************
; PEEKcommand:	Replace value on TOS (Top Of Stack) with contents of data space 
;	from the location specified.
;	- Calls SetByteRegs to fetch the values from TOS
;	- Preserves Y
;*******************************************************************************
PEEKcommand:
	PUSHY						;save Y
	CLRB	GPIOR1,BIO_RW,temp	;clear READ address flag
	GoSub	interpretlinev
	ldi		temp,1				;looking for 1 data byte
	rcall	SetByteRegs			;set up address pointer
	ld		U,Y
	GoSub	pushU				;put value on the stack
	POPY						;restore Y
	ret
;
;*******************************************************************************
; POKEcommand:	Store data in data space at parameter location two or three 
;	down at the location pointed to one or two parameters down from TOS.
;	- Calls SetByteRegs to fetch the values from TOS
;	- Preserves Y
;*******************************************************************************
POKEcommand:
	PUSHY						;save Y
	SETB	GPIOR1,BIO_RW,temp	;set WRITE address flag
	GoSub	interpretlinev
	ldi		temp,2				;looking for 2 data bytes
	rcall	SetByteRegs			;set up address pointer
	st		Y,U					;store data
	POPY						;restore Y
	ret
;	
;*******************************************************************************
; EERcommand:	Replace value on TOS (Top Of Stack) with contents of EEP 
;	from the location specified.
;	- Calls SetByteRegs to fetch the values from TOS
;	- Preserves Y
;*******************************************************************************
EERcommand:
	PUSHY						;save Y
	CLRB	GPIOR1,BIO_RW,temp	;clear READ address flag
	GoSub	interpretlinev
	ldi		temp,1				;looking for 1 data byte
	rcall	SetByteRegs			;set up address pointer
	GoSub	ReadEEP				;fetch the byte from EEP
	mov		U,temp				;save it in U
	GoSub	pushU				;put value on the stack
	POPY						;restore Y
	ret
;
;*******************************************************************************
; EEWcommand:	Store data in EEP at parameter location two or three down at the 
;	location pointed to one or two parameters down from TOS.
;	- If address is equal to E2END+1 then the entire EEP is erased.
;	- Calls SetByteRegs to fetch the values from TOS
;	- Preserves Y
;*******************************************************************************
EEWcommand:
	PUSHY						;save Y
	CLRB	GPIOR1,EEP_WE,inchar;set EEP erase/write flag
	SETB	GPIOR1,BIO_RW,inchar;set WRITE address flag
	GoSub	interpretlinev
	ldi		temp,2				;looking for 2 data bytes
	rcall	SetByteRegs			;set up address pointer
	mov		temp,U				;xfer to temp
	CPI16	YH,YL,(E2END+1)		;requesting full EEP erase?
	breq	EEWcommand1			;yes, branch and process loop
	GoSub	WriteEEP			;write the data
	rjmp	EEWcommand9			;done,exit

EEWcommand1:
	SETZPTR	(2*MSG_EEPERASE)	;inform user
	GoSub	sendromstring		;sent it.

	clr		U					;zero U for 256 byte page counter
	SETB	GPIOR1,EEP_WE,inchar;set EEP erase only flag

EEWcommand1a:
	SUBI16	YH,YL,1				;decrement EEP address pointer
	GoSub	WriteEEP			;write the data
	dec		U					;decrement page counter
	brne	EEWcommand1b		;not 0, continue
	ldi		outchar,'.'			;toss in period for indicator
	GoSub	emitchar			;send the character

EEWcommand1b:
	CPI16	YH,YL,0				;bottom of EEP?
	brne	EEWcommand1a		;loop till zero
	GoSub	crlf				;emit a CR/LF combo

EEWcommand9:
	POPY						;restore Y
	ret
;		
;*******************************************************************************
; SetByteRegs:	Used by PEEK, POKE, EER, EEW and PWM routines to set up the 
;	address pointer to the appropriate memory space location.
;	- Returns with address in YH:YL and if READ, data in U.
;	- Uses the 	BIO_RW flag in GPIOR1 to determine if it is a READ (2 byte) or 
;		WRITE (3 byte) address needed.
;	- returns with address pulled from stack in Y.
;*******************************************************************************
SetByteRegs:
	sub		temp,DStkPtr		;DStkPtr = 1?
	breq	SetByteRegs2		;only one byte on stack, page zero only
	GoSub	popU				;get page index (destroys Z)
	push	U					;save index into page
	GoSub	popU				;get page # (destroys Z)
	SKBS	GPIOR1,BIO_RW,temp	;skip next if WRITE address flag set
	rjmp	SetByteRegs1		;this is a WRITE address
	push	U					;save page #
	GoSub	popU				;get data byte into U (destroys Z)
	pop		YH					;restore page #
	pop		YL					;restore page index
	ret							;pointer set, return

SetByteRegs1:
	mov		YH,U				;point to page #
	pop		YL					;restore index within page
	ret							;pointer set, return

SetByteRegs2:
	GoSub	popU				;get data to store
	SKBS	GPIOR1,BIO_RW,temp	;skip next if WRITE address flag set
	rjmp	SetByteRegs3		;READ address needed
	mov		temp,U				;Pop uses Z register, so initialize 
								; after all the popping is done.
	GoSub	popU				;get page #
	mov		YL,temp
	clr		YH					;page zero
	ret							;pointer set, return
	
SetByteRegs3:
	mov		YL,U
	clr		YH					;EER is limited to page zero 

	ret							;pointer set, return
;
;************************************************************************
; Swap2Stk: Swaps the last two bytes on the data stack
;************************************************************************
;Swap2Dstk:
;	ldi		ZH,HIGH(DATASTK)	;set page into ZH
;	mov		ZL,DStkPtr			;set current stack pointer into ZL
;	ADDI16	ZH,ZL,1				;point to actual data
;	ld		T,Z+				;get 1st byte into T
;	ld		H,Z+				;get 2nd byte into H
;	st		-Z,T				;save 1st as 2nd
;	st		-Z,H				;save 2nd as 1st
;	ret
;
.if	INTREG
;************************************************************************
; VPGcommand: Returns the page of internal registers (RAM).
;	We need to be mindful of another value existing on the datastack, so
;	we check for it, retreive it and place our page # before if there is
;	one.  Because the locations of the variables can cross page 
;	boundaries, a look-up table is used to determine the page number.
;************************************************************************
VPGcommand:
	PUSHZ						;save Z
	ldi		U,HIGH(RNDSeed)		;get the page number of "RNDSeed"
	GoSub	pushU				;save on the stack
	push	DStkPtr				;save current DStkPtr 
	GoSub	interpretlinev		;fetch the desired offset value

	SetZPtr	(2*VPGtbl)			;point to LUT

VPGcommand1:
	lpm		T,Z+				;fetch low and high bytes
	lpm		H,Z+
	tst		H					;end of table?
	brne	VPGcommand2			;no, continue
	
	pop		r0					;pop DStkPtr to clear stack
	rjmp	VPGcommand9			;use default page address, exit

VPGcommand2:
	cp		T,U					;match?
	brne	VPGcommand1			;loop till match

	clr		r1					;clear a register
	pop		r0					;restore previous DStkPtr 
	SetZptr	(DATASTK+DATASTKSZ)
	SUB16	ZH,ZL,r1,r0
	st		Z,H					;save variable's page number

VPGcommand9:
	POPZ						;restore Z
	ret
;
VPGtbl:
.if RTC
	.dw		RTCReg
	.dw		RTCReg+1
	.dw		RTCReg+2
	.dw		RTCReg+3
.endif
.if DATA
	.dw		DFLaddr
.endif
.if DHT
	.dw		RH_I
	.dw		RH_D
	.dw		RT_I
	.dw		RT_D
.endif
	.dw		0					;end of table
;
;************************************************************************
; PGMReadByte: Fetches a bute from FLASH and places it on the data stack.
;	- Enter with byte address in ZH:ZL and the index offset in U.
;	- Returns with ZH:ZL pointing to the actual byte addres in FLASH.
;	- Returns with data in U and on top of stack.
;	- Destroys r1.
;************************************************************************
PGMReadByte:
	clr		r1					;zero the high register
	ADD16	ZH,ZL,r1,U			;add offset to pointer
	lpm		U,Z					;fetch the value
	GoSub	pushU				;save on the stack
	ret
;
.endif	;from INTREG conditional

