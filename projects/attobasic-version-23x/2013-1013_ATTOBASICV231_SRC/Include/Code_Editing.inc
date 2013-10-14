;************ Editing error trapping *****************************
buffendhit:
	STOREI	ERRCode,$04
	Goto	error
;	
linenotoolong:
	STOREI	ERRCode,$0B	;Error: Line number has too many characters.
	Goto	error	
;
;******************************************************************
; storeline:	Store line in appropriate place in PROGMEM.
;	- Uses H, T, U, inchar, inbyteh, inbytel, temp
;	- Uses currlooptgt, currloopvar, 
;	- Uses Y & Z
;*********************************************************************************
; Note: These flags used to be held in XH:XL but as they are only used "locally"
;	by the line editing routines, they were moved out of XH:XL to free up run-time
;	flag bits for other purposes.  They are temporarily held in the "outchar" register..
.equ	LNBFNH	= 0				;1 to indicate line no higher than linebuff encountered
.equ	LN1TOSS	= 1				;1 to indicate that first line number was thrown away
.equ	LNNONNC	= 2				;1 to indicated nonnumeric char found in line
;*********************************************************************************
;
storeline:	;Store line in appropriate place.
	clr		outchar				;clear editing flags
	GoSub	Countlinehchars		;Reject line if line number contains more than three numerals.
	cpi 	U,4
	brsh	linenotoolong

	LOAD16	inbyteh,inbytel,PCLO	
	CPI16	inbyteh,inbytel,PROGMEM+PROGMEMSZ-1;compare top of PROGMEM
	brne	pcnotattop
	rjmp	storelineatend

pcnotattop:
	SetYPtr	LNBUFF+LNBUFFSZ-1	;Initialize line buffer pointer to first char 
	
	;Find out what line number in line buffer is and put in currlooptgt
	clr		U
	clr		T
	clr		H
				
fetchanothernum1:	
	CPI16	YH,YL,LNBUFF-1		;end of buffer?
	brmi	buffendhit			;Error if past end of buffer
	ld		inchar,Y
	GoSub	qcast				;Find out what kind of char this is
	cpi		temp,QC_NUM			;is it a number? (ecode in temp)
	brne	notanumeral1
	mov		H,T					;Its a numeral so shift it in
	mov		T,U
	mov		U,inchar
	SUBI16	YH,YL,1				;decrement pointer
	rjmp	fetchanothernum1
notanumeral1:			;The pointer is not poiting to a numeral, so test the number.
	rcall	makeHTUdecimal
	mov		currlooptgt,U	;Put number of line in linebuffer into currlooptgt

		;//GOT LINE NUMBER in the LINE BUFFER ///
	
;////SCAN PROGRAM MEMORY////
;Scan from lowest to top of program memory to find the place to
;  insert the input line.
scanpmem:
	LOAD16	ZH,ZL,PCLO			;Get current program memory location from PC high and PC low
	andi	outchar,~(1<<LNBFNH | 1<<LN1TOSS)	;Clear "higherthan flag" and "first line no thrown out flag"
newnumber:
	clr		H					;Clear digits buffer
	clr		T
	clr		U
	clr		currloopvar		;Clear count of number of digits
nextchar:
	adiw	ZH:ZL,1	
	CPI16	ZH,ZL,PROGMEM+PROGMEMSZ
	breq 	justifynumber		;If Z exceeds top of buffer, its the end of the line number
notattop:
	ld		inchar,Z
   	GoSub	Qcast				;Find out what kind of character Z is pointing to
	cpi		temp,QC_CR			;is it a CR?
	breq 	justifynumber		;if yes, then its the end of the line number
	cpi		temp,QC_NUM			;is it a number?
	brne	newnumber			;if not then dump numeral registers and start over.
	inc		currloopvar			;Increment count of numerals shifted in
	mov		U,T					;Shift current char in
	mov		T,H
	mov		H,inchar
	rjmp	nextchar		;Get next char
;	
noinsertlowerline:				;Don't insert a line in the midst of pmem -keep scanning
	;(DEFAULT LOOP BACK)
	CPI16	ZH,ZL,PROGMEM+PROGMEMSZ
	brne	newnumber
	ret		
;
justifynumber:	;Right justify the number in HTU
	mov		temp,currloopvar
	cpi		temp,3				;There are three registers: H,T,and U.	
	breq	nowjustified
	inc		currloopvar
	mov		U,T
	mov		T,H
	ldi		H,0
	rjmp	justifynumber

nowjustified:
	sbrc	outchar,LN1TOSS		;If this is the first line number, skip it (only responding to last CR in pmem).
	rjmp	notfirstlineno
	ori		outchar,(1<<LN1TOSS)
	rjmp	newnumber

notfirstlineno:
	rcall	makeHTUdecimal		;Put value of HTU into U.
	cp		currlooptgt,U		;DIVERT IF EQUAL
	breq	deleteandgo			;If current line no is equal to target, then delete and/or replace the line
	brsh	nothigher			;If current line no is higher than target, set flag XH:LNBFNH
	ori		outchar,(1<<LNBFNH)	
	COPY16	loopreturnH,loopreturnL,ZH,ZL	;and save address of CR following higher line number

nothigher:
	CPI16	ZH,ZL,PROGMEM+PROGMEMSZ	;Test for Z being past end of memory
	brne	notattop2			;If not past end of memory, test to see if line higher than in line buffer,
	sbrs	outchar,LNBFNH		;and if no higher number found, store line at end of buffer
	rjmp	storelineatend 		;If at end and no higher found, store at end.

notattop2:
			;DIVERT IF HIGHER PRVIOUSLY FOUND AND (HIGHER OR END)
	sbrs	outchar,LNBFNH			;Was a higher line number found?
	rjmp	noinsertlowerline
	
	cp		U,currlooptgt		;Is current line number lower than target?
	brsh	notlowerline
	rjmp	insertinmiddle	
;
notlowerline:
	CPI16	ZH,ZL,PROGMEM+PROGMEMSZ;Test for Z being past end of memory		
	brne	notattop3			;If not past end of memory, test to see if line higher than in line buffer,
	rjmp	insertinmiddle
notattop3:
	rjmp	noinsertlowerline
;
storelineatend:	;Store line in next available locatoin in program memory from first char to $0D
		;Enter with YH, YL pointing to first char of line
	GoSub	Countlinehchars		;If its a zero length line, don't store it.
	cpi		temp,$00
	brne	writelineatend
	ret	

writelineatend:
	SetYPtr	LNBUFF+LNBUFFSZ-1	;Initialize line buffer pointer to first char 
	LOAD16	ZH,ZL,PCLO			;Get current program memory location from PC high and PC low

anotherlinchar2:
	CPI16	ZH,ZL,PROGMEM		;Check to see if pointer is past end of PROGMEM
	brne	pmemnotfull			;no, continue
	rjmp	memoryfull2			;yep!  Out of PROGMEM

pmemnotfull:
	ld		temp,Y				;Put contents of line buffer into temp
	st		Z,temp				;Put temp into the program memory
	sbiw	ZH:ZL,$01
	sbiw	YH:YL,$01
	cpi		temp,$0D
	brne	anotherlinchar2

	STORE16	PCLO,ZH,ZL			;save Program pointer
	ret

;itsanemptylineRELAY: rjmp itsanemptyline 	;can this be displosed of? ****
	
;****************************************************************************
; deleteandgo:	Delete a line in prorgam memory.  Enter with Z pointing to
;	the CR char of the line before the one to be deleted.
;	Moves everything from the first char of the line following the 
;	line to be deleted to the  end of the program memory to 
;	overwrite the deleted line. ZH must be set to the correct page
;****************************************************************************
deleteandgo:
	sbiw	ZH:ZL,1				;point to beggining character of line
	movw	YH:YL,ZH:ZL			;Copy Z into Y
;	
	;Scan Y down the program memory until either end of memory
continuescan:			;or CR is encountered. If end of memory, claim error.
	CPI16	ZH,ZL,PROGMEM
	brne	Nothitpmembot
	rjmp	buffendhit
Nothitpmembot:
	ld		temp,Y
	sbiw	YH:YL,1
	cpi		temp,$0D
	brne	continuescan
	COPY16	H,T,ZH,ZL			;copy ZH:ZL to temp registers
	SUB16	H,T,YH,YL			;subtract Y from Z for difference in H:T
	LOAD16	inbyteh,inbytel,PCLO;get PC
	ADD16	inbyteh,inbytel,H,T	;add difference to PC
	STORE16	PCLO,inbyteh,inbytel;save PC

noinc:

keepoverwriting:
	CPI16	ZH,ZL,PROGMEM		;bottom of PROGMEM?
	breq	finishedoverwrite	;yes, stop writing PROGMEM
	ld		temp,Y				;Copy memory(Y) to memory (Z)
	st		Z,temp				;  until end of memory is reached.
	sbiw	YH:YL,1				;decrement Y-ptr
	sbiw	ZH:ZL,1				;decrement Z-ptr
	rjmp	keepoverwriting		;loop till finished

finishedoverwrite:
	GoSub	Countlinehchars
	cpi		temp,$00
	breq	dontwriteline

	rjmp	storeline
;	rjmp	scanpmem		;**** changed to storeline from scanpmem - seems to have 
							;  fixed reluctance to try and re-write line after deleting it.
dontwriteline:
	ret		
;
;************ Editing error trapping ********************
memoryfull:
memoryfull2:
	STOREI	ERRCode,$02
	Goto	error	
;	
;*******************************************************	
insertinmiddle:	;MAKE ROOM FOR LINE IN MIDST OF TEXT AND INSERT
	GoSub	Countlinehchars 
	cpi		temp,$00			;If its a zero length line, don't store it.
	breq	itsanemptyline

	LOAD	temp,PCHI	 		;But first, check to see if there is enough memory				
	cpi		temp,HIGH(PROGMEM) 	;PC in same page as bottom of program memory?
	brne	cantbefull

	LOAD	temp,PCLO			;get PCLO into temp
	subi	temp,LOW(PROGMEM)	;Temp has remaining memory bytes (in last page of memory)	
	cp		temp,inbytel		;Compare bytes remaining with line size
	brlo	memoryfull2

cantbefull:
	;Make room to insert line
	LOAD16	ZH,ZL,PCLO			;Point Z to bottom of program [the source]
	mov		YH,ZH				;Set Y = Z - size of line buffer content) [the destination]		
	mov		YL,ZL
	sub		YL,inbytel
	sbci	YH,$00
	STORE16	PCLO,YH,YL			;Adjust PC (pointer to end of program)
	
movemore:
	cp		ZH,loopreturnH
	brne	notfinishedmoving
	cp		ZL,loopreturnL
	brne	notfinishedmoving
	rjmp	movingfinished

notfinishedmoving:
	ld		temp,Z
	st		Y,temp
	adiw	YH:YL,1
	adiw	ZH:ZL,1
	rjmp	movemore
	
movingfinished:			;The text has been moved to make room for line buffer contents
		;Insert contents of line buffer into program memory
	SetYPtr	LNBUFF+LNBUFFSZ-1	;Initialize line buffer pointer to first char 
	mov		ZH,loopreturnH		;Get address of CR after higher line
	mov		ZL,loopreturnL			
	sbiw	ZH:ZL,$01			;Move pointer to first char of line to be moved	
	
fillmore:
	ld		temp,Y
	st		Z,temp
	cpi		temp,$0D
	breq	finishedfilling
	sbiw	ZH:ZL,1
	sbiw	YH:YL,1
	rjmp	fillmore
finishedfilling:	 	
	   	
itsanemptyline:   	
	   ret
;
;*******************
;Count total chars and number of non-numeric, non-delimiter chars
;  If not empty, there will be more than zero non-numeric
;  and non-delimiter characters in the line before the 
;  carriage return. Returns count in temp. The number of numerals
;  in the line number is contained in U.
;
Countlinehchars:		
	SetYPtr	LNBUFF+LNBUFFSZ-1	;Initialize line buffer pointer to first char 
	clr		U
	clr		inbytel
	clr		inbyteh
	andi	outchar,~(1<<LNNONNC)	;Clear nonnumeric char found flag
morchars: 			
	ld		inchar,Y
	SUBI16	YH,YL,1				;decrement buffer pointer
	inc		inbytel
	cpi		inchar,$0D			;is it a CR?
	breq	donecounting		;if so, end of the line
	GoSub	Qcast
	cpi		temp,QC_NUM			;is it a number? (ecode in temp)
	breq	anothernum			;if yes, 
	ori		outchar,(1<<LNNONNC)
	cpi		temp,QC_DELIM		;is it a delimiter? (ecode in temp)
	breq	morchars
	inc		inbyteh
	rjmp	morchars	
donecounting:
	mov		temp,inbyteh  		;Return with non-numeric, non-delimer count in temp
	ret							;Return with char total count in inbytel

anothernum:
	sbrs	outchar,LNNONNC		;skip next if non-numeric char found in line
	inc		U					;increment count
	rjmp	morchars
;
;****************************************
storelbufatZ:
	SetYPtr	LNBUFF+LNBUFFSZ-1	;Initialize line buffer pointer to first char 
anotherlinchar:
	cpi		ZL,LOW(PROGMEM)
	breq	anotherlinchar1
	rjmp	memoryfull
anotherlinchar1:
	ld		temp,Y				;Put contents of line buffer into temp
	st		Z,temp				;Put temp into the program memory
	SUBI16	ZH,ZL,1
	SUBI16	YH,YL,1
	cpi		temp,$0D
	brne	anotherlinchar
	STORE16	PCLO,ZH,ZL		;save Program pointer
	ret
;	

