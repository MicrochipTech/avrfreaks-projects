;*******************************************************************   
; RENUmcommand:	Renumber the program currently held in memory.  The
;	beginning number of program lines and increment is prese to 5 and
;	5.  However, if the ending line number will exceed 255 then the
;	starting line number will be 1 and the increment will be 
;	according to the number of program lines.
;	- Enter with starting line number and increment on dstack.
;	- Uses X, Y and Z pointers.
;	- Uses inbyteh, inbytel, temp, bytecntr
;	- Uses H, T and U
;	- Preserves X, Y and Z pointers
;		X = Temporary pointer
;		Y = PROGMEM pointer 
;		Z = Line buffer pointer
;*******************************************************************   
RENUMcommand:
	;set/get the start and increment, even if no program in RAM
	ldi		U,STLNNO			;default starting line number
	GoSub	pushU				;push on stack
	ldi		U,LNINC				;default line increment
	GoSub	pushU				;push on stack

	CLRB	GPIOR1,BIO_RW,temp	;clear READ address flag
	GoSub	interpretlinev		;interpret rest of the line
	ldi		temp,dstacktop-1	;top of data stack pointer less one (2 data bytes)
	rcall	SetByteRegs			;set up address pointer

	mov		loopreturnH,YH		;move start # into loopreturnH
	mov		loopreturnL,YL		;move increment into loopreturnHL

	;check to see if there is a program in RAM
	SetYPtr PROGMEM+PROGMEMSZ+1	;Y = 1st byte of user program  (+1)
	LOAD16	ZH,ZL,PCLO			;Z = last byte of user program
	CP16	YH,YL,ZH,ZL			;is there even a program?
	brne	RENUMcommand1		;yes, continue
	ret							;return to caller

RENUMcommand1:
	;Clear contents of table in RAM
	SetZPtr	EFSbuff				;point Z to start of table in RAM
	clr		temp				;set fill value
	clr		inbyteh				;clear upper register
	ldi		inbytel,EFSBLKSZ	;load lower register w/ count
	GoSub	FillMem				;fill table with 0's

	;Make the was/is table
	rcall	RENUMmktbl			

RENUMcommand2:
nop


;RENUMcommand2:
;	PUSHX						;save X pointer
;	SetXPtr PROGMEM				;X = last byte of user program 
;	SetYPtr PROGMEM+PROGMEMSZ+1	;Y = 1st byte of user program  (+1)
;	LOAD16	ZH,ZL,PCLO			;Z = last byte of user program
;	SUB16	YH,YL,ZH,ZL			;subtract for program size

;move program to bottom of memory so we have some room to change/add lines
;RENUMcommand2a:
;	ld		temp,Z+				;get a byte
;	st		X+,temp				;store it
;	SUBI16	YH,YL,1				;decrement counter
;	brne	RENUMcommand2a		;loop till all byte copied

;X-ptr (+1) now points to the 1st line of our program in "Low RAM"
RENUMcommand3:
;	SetYPtr	LNBUFF+LNBUFFSZ+1	;Y = 1st char of line buffer (+1)
;for test!!!
	COPY16	YH,YL,XH,XL			;copy top of program to Y-ptr
;for test!!!

RENUMcommand3a:	;copy 1st line number into line buffer
;	ld		temp,-X				;get byte
;	st		-Y,temp				;save it in the line buffer
;	cpi		temp,CR				;was it a CR?
;	brne	RENUMcommand3a		;no, continue to copy



;	GoSub	FormWord			;fetch 1st word
;	mov		temp,currentcast	;get the cast into temp
;	cpi		temp,QC_NUM			;is it a letter
;	brne	RENUMcommand9		;this is an error condition!!
;	GoSub	makeHTUdecimal		;Its a number so make binary
;	mov		currlooptgt,U		;save the target in register

RENUMcommand5:

;QC_OPER	= 0		;Operator (not one of the other casts
;QC_DELIM	= 1		;Delimiter -space ($20) or comma ($2C)
;QC_NUM		= 2		;Numeral 0..9 ($30 through $39)
;QC_LTR		= 3		;Letter [A..Z] uppercase ($41 throught $5A)
;QC_CR		= 4		;Carriage return
;QC_QUOT	= 5		;quote character (string support) 
					;  or "@" for a constant

;	GoSub	Interpretelinebuffer;use to store the program line
;	SetXPtr	LNBUFF+LNBUFFSZ		;X = 1st char of line buffer
;	GoSub	GETPROGLINENO		;get program line # into U
;	GoSub	Countlinehchars		;character count in temp
;	nop

RENUMcommand9:


RENUMcommand_out:
	POPX						;restore X pointer
	ret
;


;************************************************************************
; RENUMmktbl:	Scan PROGMEM looking for all occurances of GOTO/GOSUB.
;	If one is found, increment counter as index and store in table.
;	- Uses X, Y, Y, bytecntre, inbyteh, inbytel, temp, currlooptgt & U.
;************************************************************************
RENUMmktbl:
	SetYPtr PROGMEM+PROGMEMSZ	;Y = 1st byte of user program
	ori		XH,(1<<NOEXEC) 		;set NOEXEC flag
	clr		bytecntr			;clear GOT/GOSUB counter
;	GoSub	GETPROGLINENO		;get 1st line #

RENUMmktbl1:	;use the line interpreter to scan for GOTO/GOSUB's
	GoSub	Interpretline		;interpret the line for GOTO/GOSUB's
	CPI16	ZH,ZL,(gotocommand)	;is it a GOTO?
	breq	RENUMmktbl1a
	CPI16	ZH,ZL,(gosubcommand);is it a GOSUB?
	breq	RENUMmktbl1a
	LOAD16	inbyteh,inbytel,PCLO;bottom of program
	CP16	YH,YL,inbyteh,inbytel;bottom of program?
	breq	RENUMmktbl2			;yes, exit
	cpi		U,CR				;was the last character a CR?
	brne	RENUMmktbl1			;no, continue
;	GoSub	popU				;clear the datastack
;	GoSub	GETPROGLINENO		;get program line #
	rjmp	RENUMmktbl1			;loop and process some more

RENUMmktbl1a:	;found a GOTO or GOSUB so next "word" must be a target line #
	GoSub	FormWord			;fetch the target line #
	mov		temp,currentcast	;get the cast into temp
	cpi		temp,QC_NUM			;is it a number
	brne	RENUMmktbl2			;this is an error condition!!
	GoSub	makeHTUdecimal		;Its a number so make binary
	push	U					;save value
	GoSub	popU				;fetch the current program line # from stack
	SetZPtr	EFSbuff				;point Z to start of table in RAM
	clr		r1					;zero register
	mov		r0,bytecntr			;copy of pointer in 
	lsl		r0					;multiply by 2
	ADD16	ZH,ZL,r1,r0			;add index
	st		Z+,U				;store current program line # in table
	pop		U					;restore target line # 
	st		Z,U					;store target line # in table
	inc		bytecntr			;increment GOTO/GOSUB pointer
	rjmp	RENUMmktbl1			;no, loop for more

RENUMmktbl2:
	ret							;return to caller
