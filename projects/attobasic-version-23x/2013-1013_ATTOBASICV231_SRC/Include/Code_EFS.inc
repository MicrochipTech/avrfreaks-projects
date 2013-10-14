;*******************************************************************   
; EEPROM and File System routines
;*******************************************************************   
; Savecommand:	
; Single-file system: Save contents of PROGMEM to EEPROM.  Saving 
;	is from top down.  Once the PC is reached, a null byte is written 
;	signaling the end of the program.  This reduces the amount of 
;	time taken to save a program to EEPROM because the entire EEPROM 
;	is not written to all the time.  Also reduces the number of 
;	erase/write cycles to each EEP cell.
; EFS: Save the contents of PROGMEM to the file handle specified on
;	the command line.
;*******************************************************************   
SAVEcommand:
.if !EFS	
	SETYPTR (PROGMEM+PROGMEMSZ)-1
	LOAD16	ZH,ZL,PCLO			;get PROGMEM pointer into Z
	SUB16	YH,YL,ZH,ZL			;subtract for program size
	SETZPTR	E2PPROGTOP			;Z = 1st byte of EEP PROG space
	CP16	ZH,ZL,YH,YL			;is EEP size larger that program?
	brge	SAVEcommand1		;if so, we have room to save

	SETZPTR	(2*MSG_E2PROMSAVE2BIG)	;program too large, inform the user
	GoSub	sendlromline		;sent it.
	ret
;
SAVEcommand1:	;we have enough room in EEP to save the current program
	SETZPTR	(2*MSG_E2PROMSAVE)	;inform the user
	GoSub	sendlromline		;sent it.

	INITDSTACK					;Initialize data stack pointer

	SETZPTR (PROGMEM+PROGMEMSZ)-1
	SETYPTR	E2PPROGTOP			;Y = 1st byte of EEP PROG space
	LOAD16	H,T,PCLO			;get current PC into H:T
	CLRB	GPIOR1,EEP_WE,temp	;set erase/write combo flag
	set							;set T-Flag to use SLEEP mode for writes

SAVEcommand2:	;top of EEP storage loop
	ld		temp,Z				;Put contents of program memory into temp
	rcall	WriteEEP			;write the data to EEP
	CPI16	ZH,ZL,PROGMEM		;compare end of RAM
	breq	SAVEcommand3		;end of PROGMEM, stop saving

	CP16	ZH,ZL,H,T			;compare end of program	
	breq	SAVEcommand3

SAVEcommand2a:	;decrement pointers
	SUBI16	YH,YL,1				;decrement EEP pointer
	CPI16	YH,YL,0				;test Y for bottom of EEP
	breq	SAVEcommand3		;bottom of EEP, stop saving
	SUBI16	ZH,ZL,1				;decrement PROGMEM pointer

SAVEcommand2b:
	sleep						;sleep while EEPROM programming
	nop
	SKBC	EECR,EEPE			;check EEPROM write has finished
	rjmp	SAVEcommand2b		;must have been another IRQ source
	rjmp	SAVEcommand2		;store some more characters

SAVEcommand3:	;program saved, add null byte for end of program
	clr		temp				;null byte = end of program
SAVEcommand3a:
	rcall	WriteEEP			;write the data to EEP
	ret
;
;++++++++++++++++++++++++++++++++++++++++++++
.else	; EFS support starts here
;++++++++++++++++++++++++++++++++++++++++++++
	PUSHX							;save X 
	clr		U						;default to program '0'
	Gosub	PushU					;save on datastack
	GoSub	Get1stParm			;get the last parameter

	cpi		U,HANDLES_MAX			;is it larger than acceptable?
	brlt	SAVEcommand1			;no, continue

	STOREI	ERRCode,$16				;error code "16"
	Goto	error

SAVEcommand1:	;figure out if there is room for the program
	rcall	ERAcommand1				;erase the file first but skip
									; the line interpreter
	SETXPTR (PROGMEM+PROGMEMSZ)-1
	LOAD16	ZH,ZL,PCLO				;get PROGMEM pointer into Z
	SUB16	XH,XL,ZH,ZL				;subtract for program size
	ADDI16	XH,XL,1					;add a byte for EOP

.if USBSUPP
	GoSub	D_USBINT				;disable USB ints
.endif
	rcall	AlocBlks				;fetch free bytes in Z

	SUB16	ZH,ZL,XH,XL				;subtract program from free bytes
	POPX							;restore X
	brpl	SAVEcommand2			;branch if result is positive

	SETZPTR	(2*MSG_E2PROMSAVE2BIG)	;program too large, inform the user
	GoSub	sendlromline			;sent it.
	ret

	;We have room to store the program in EEP so we need to store it.
	;XH:XL still holds the size of the current program.
	;U still holds the requested file handle.
SAVEcommand2:
	SETZPTR	(2*MSG_SAVEPROGNUM)		;inform the user
	GoSub	sendromstring			;sent it.

	mov		inbytel,U				;print program #
	GoSub	sendbyte	
	GoSub	CRLF					;print CR/LF combo

	;fetch the index block, find the next available block and 
	;re-write the index
	STOREI	EFSblkadr,0,temp		;set start scan block
	rcall	NextFreeBlk				;check for next free block

	clr		r1						;clear high 
	SetYPtr	EFSIDX0					;Z = start of file index
	ADD16	YH,YL,r1,U				;add file index offset	
	mov		temp,bytecntr			;move start block to temp
	rcall	WriteEEP				;write it to the file's index 

SAVEcommand3:	;fill the RAM buffer with the program
;	SETXPTR (PROGMEM+PROGMEMSZ+1)	;X = top of PROGMEM+1
;+++added
	SETZPTR (PROGMEM+PROGMEMSZ)	;Z = top of PROGMEM
;+++added

SAVEcommand3a:		
	ldi		temp,EFSDATREC			;data record indicator
	ldi		inchar,EFSPAYSZ			;load payload size
	SetYPtr	EFSbuff					;point Y to byte 0 in RAM buffer
	st		Y+,temp					;save data record indicator

SAVEcommand3b:		
;	ld		temp,-X					;copy from PROGMEM to buffer
;+++added
	ld		temp,-Z					;copy from PROGMEM to buffer
;+++added
	st		Y+,temp					;save it
	tst		temp					;is it EOP?
	breq	SAVEcommand4			;yes, exit
	dec		inchar					;decrement payload counter
	brne	SAVEcommand3b			;loop till payload loaded
	rcall	NextFreeBlk				;check for next free block
	st		Y,bytecntr				;save it as last byte of block

	push	bytecntr				;save next block number
	LOAD	temp,EFSlab				;get last block number
	STORE	EFSblkadr,temp			;save it to RAM@EFSblkadr

	rcall	WriteBlk				;write the block to EEP

	pop		temp					;restore next block number
	STORE	EFSblkadr,temp			;save it to RAM@EFSblkadr

	rjmp	SAVEcommand3a			;loop till end of program

SAVEcommand4:	;EOP detected, finish writing buffer with nulls
	st		Y+,temp					;save null it
	dec		inchar					;decrement payload counter
	brne	SAVEcommand4			;loop till payload loaded
	rcall	WriteBlk				;write the block to EEP

SAVEcommand9:
.if USBSUPP
	GoSub	E_USBINT				;re-enable USB ints
.endif
;	POPX							;restore X
	ret
;
.endif
;
;*******************************************************************   
; Loadcommand:	Load contents of EEPROM memory into RAM.
;	This routine supports both EFS and single file system. 
;	- Y is pointer to buffer in RAM (EFS)
;	- Z is pointer to Program RAM

;	- Y is pointer to EEP (single-file system)
;	- Z is pointer to program memory (single-file system)
;*******************************************************************   
LOADcommand:		
.if !EFS	;single-file system support
	GoSub	NEWprogram				;erase program memory
	SETZPTR	(2*MSG_E2PROMLOAD)		;inform the user
	GoSub	sendlromline			;sent it.

	SetZPtr (PROGMEM+PROGMEMSZ)		;Z = top of PROGMEM
	SetYPtr	E2PPROGTOP				;Y = 1st byte of EEP PROG space

LOADcommand1:
	rcall	ReadEEP					;fetch data from EEP
	st		-Z,temp					;store and decrement PROGMEM pointer
	tst		temp					;is byte zero? Null = end of program
	breq	LOADcommand7			;if yes, exit
	SUBI16	YH,YL,1					;decrement EEP pointer
	brne	LOADcommand1			;loop for more
;
;++++++++++++++++++++++++++++++++++++++++++++
.else	; EFS support starts here
;++++++++++++++++++++++++++++++++++++++++++++
	sbrc	XH,RUNACT				;skip next if not called from the 
	rjmp	LOADcommanda			;RUN command

	clr		U						;default to program '0'
	Gosub	PushU					;save on datastack
	Gosub	interpretlinev			;save parameter to datastack
	Gosub	popU					;retrieve parameter

	cpi		U,HANDLES_MAX			;is it larger than acceptable?
	brge	LOADcommand9			;yes, error!

LOADcommanda:
	GoSub	NEWprogram				;erase program memory
	SETZPTR	(2*MSG_LOADPROGNUM)		;inform the user
	GoSub	sendromstring			;sent it.

.if USBSUPP
	GoSub	D_USBINT				;disable USB ints
.endif
	mov		inbytel,U				;copy to send
	GoSub	sendbyte				;inform the user of the program #
	GoSub	CRLF					;print CR/LF combo

	rcall	ReadBlkOne				;get 1st data block of file handle

	SetZPtr	(PROGMEM+PROGMEMSZ)		;Z = top of PROGMEM

LOADcommand1:	;read block and store in PROGMEM
	ldi		inchar,EFSPAYSZ			;load byte counter
	SetYPtr	EFSbuff+1				;point Y to 1st data byte in RAM buffer
LOADcommand1a:
	ld		temp,Y+					;get byte, increment source pointer
	st		-Z,temp					;store and deccrement PROGMEM pointer
	tst		temp					;is byte zero? Null = end of program
	breq	LOADcommand7			;if yes, exit
	dec		inchar					;decrement and test for zero
	brne	LOADcommand1a			;loop for more

	ld		temp,Y					;get next block from current block
	STORE	EFSblkadr,temp			;point to next block
	rcall	ReadBlk					;fetch it
	rjmp	LOADcommand1			;loop and copy next block
.endif

LOADcommand7:	;finished loading program, save PC
	INITDSTACK						;Initialize data stack pointer
	STORE16	PCLO,ZH,ZL				;save X-Ptr to PC

LOADcommand8:	;print free space and return to caller
.if USBSUPP
	GoSub	E_USBINT				;re-enable USB ints
.endif
	sbrs	XH,RUNACT				;skip next if called from the 
									; RUN command
	GoSub	FREEcommand				;print free RAM
	ret

LOADcommand9:	;program # too large
	STOREI	ERRCode,$16				;error code "16"
	Goto	error
;
.if EFS
;*********************************************************************
; INITcommand:	Initializes the EEP file system, clearing all program
;	file pointers and adjusting the file lengths to "0".
;	Does not overwrite all of EEP memory, just the pointers and 
;	record type byte.
;	- Destroys Y and Z pointers and inchar
;*********************************************************************
INITcommand:
	PUSHY						;save Y
	ldi		inchar,EFSMAXBLK-1	;load max blocks

INITcommand1:	;top of the counting loop
	STORE	EFSblkadr,inchar	;set block #
	rcall	DelBlk				;delete the record type	
	dec		inchar				;increment block number
	brne	INITcommand1		;block 0 doesn't count

INITcommand2:
	rcall	InitIdx				;clear file indexes
	POPY						;restore Y
	ret							;return to caller
;
;**********************************************************************
;ERAcommand:  Marks each record allocated to the file handle specified
;	as a free record and zeros the index.  
;	- Destroys Y, Y, 
;**********************************************************************
ERAcommand:
	clr		U					;default to program '0'
	Gosub	PushU				;save on datastack
	Gosub	interpretlinev		;save parameter to datastack
	Gosub	popU				;retrieve parameter

	cpi		U,HANDLES_MAX		;is it larger than acceptable?
	brlt	ERAcommand1			;no, continue

	STOREI	ERRCode,$16			;error code "16"
	Goto	error

ERAcommand1:	;fetch the index block
	clr		YH					;insure high register zero
	mov		YL,U				;YL = file handle to index
	rcall	ReadEEP				;fetch the starting block
	tst		temp				;starting block = 0
	brne	ERAcommand1a		;no, branch and continue
	ret							;return to caller

ERAcommand1a:	
	STORE	EFSblkadr,temp		;point to next block
	rcall	ReadBlkOne			;fetch 1st data block

ERAcommand2:	;fetch each program block
	rcall	DelBlk				;delete the record type byte
	SetYPtr	EFSbuff				;point Y to RAM buffer
	ldd		temp,Y+(EFSBLKSZ-1)	;get next record pointer
	STORE	EFSblkadr,temp		;point to next block
	tst		temp				;next record = 0?
	breq	ERAcommand2a		;yes, exit
	com		temp				;compliment, is 0?
	breq	ERAcommand2a		;yes, exit
	rcall	ReadBlk				;fetch it
	rjmp	ERAcommand2			;loop back

ERAcommand2a:
	rcall	ClrIdx				;clear the file's index pointer
	ret							;return to caller
;
;*********************************************************************
; CATcommand:	Print the 1st line of each program stored in EEP.
;	If the length of a program is "0" then "Empty" is printed.
;	- Uses U, H, T, temp, inbytel, outchar, Y and Z pointers. 
;*********************************************************************
CATcommand:
	PUSHY						;save Y
	clr		U					;clear file handle counter

CATcommand1:	;cycle through each file handle
	mov		inbytel,U			;set up to xmit value of file handle
	GoSub	sendbyte			;send it
	ldi		outchar,':'			;now a colon character
	GoSub	emitchar
	ldi		outchar,' '			;and a space character

.if USBSUPP
	GoSub	D_USBINT			;disable USB ints
.endif
	GoSub	AlocByHndl			;fetch the # of used bytes for handle
	COPY16	inbyteh,inbytel,ZH,ZL;set up for bin16 to BCD conversion
	push	U					;save U
	GoSub	BIN2BCD16			;convert, H:T:U has file size
.if USBSUPP
	GoSub	E_USBINT			;re-enable USB ints
.endif
	GoSub	SEND_4				;send the four digits

	SETZPTR	(2*MSG_BYTES)		;print "bytes"
	GoSub	sendromstring		;sent it.

	CPI16	T,U,0				;is file size "0"?
	brne	CATcommand1a		;no, print the 1st program line

	SETZPTR	(2*MSG_CATEMPTY)	;empty program, inform user
	GoSub	sendromstring		;sent it.
	pop		U					;restore U
	rjmp	CATcommand2			;process next file handle

CATcommand1a:	;print the 1st line of the program
	pop		U					;restore U
	rcall	ReadBlkOne			;fetch it
CATcommand1b:
	SetYPtr	EFSbuff+1			;point Y to 1st data byte in RAM buffer
	ldi		T,EFSPAYSZ			;set up payload size counter + next block
CATcommand1c:
	ld		outchar,Y+			;load the byte
	GoSub	emitchar			;send it
	cpi		outchar,CR			;was it a CR?
	breq	CATcommand2			;yes, finished printing 1st line of program
	dec		T					;decrement byte counter
	breq	CATcommand1d		;branch if we need the next block
	rjmp	CATcommand1c		;loop some more

CATcommand1d:
	ld		outchar,Y			;get next block from current block
	STORE	EFSblkadr,outchar	;point to next block
	rcall	ReadBlk				;fetch it
	rjmp	CATcommand1b		;loop and fetch next block

CATcommand2:	;finished printing 1st line, inc file handle and loop
	GoSub	crlf				;send CR/LF combo
.if USBSUPP
	clr		r1					;insure r1 = 0
	GoSub	usb_serial_flush_output ;flush output buffer
	GoSub	Delay10ms			;delay 10ms to flush USB buffer
.endif
	inc		U					;next file handle
	cpi		U,HANDLES_MAX		;is it max?
	breq	CATcommand3			;yes, exit
	rjmp	CATcommand1			;no, loop for more

CATcommand3:	;finished printing each file handle, print used and free
	GoSub	crlf				;send CR/LF combo
.if USBSUPP
	GoSub	D_USBINT			;call routine					;disable USB ints
.endif
	GoSub	AlocBlks			;get used/free bytes
	PUSHZ						;save Z (free bytes)

	mov		inbyteh,YH			;set up for bin16 to BCD conversion
	mov		inbytel,YL
	GoSub	BIN2BCD16			;convert, H:T:U has file size
.if USBSUPP
	GoSub	E_USBINT			;re-enable USB ints
.endif
	GoSub	SEND_4				;send the four digits

	SETZPTR	(2*usedmem)			;used space
	GoSub	sendromstring		;sent it.

.if USBSUPP
	GoSub	D_USBINT			;disable USB ints
.endif
	pop		inbyteh				;restore Z into inbyteh and inbytel
	pop		inbytel

	GoSub	BIN2BCD16			;convert, H:T:U has file size
.if USBSUPP
	GoSub	E_USBINT			;re-enable USB ints
.endif
	GoSub	SEND_4				;send the four digits

	SETZPTR	(2*freemem)			;free space
	GoSub	sendromstring		;sent it.
	GoSub	crlf				;send CR/LF combo

	POPY						;restore Y
	ret
;
;*********************************************************************
; ReadBlk:	Reads a block of data from EEP.  
; WriteBlk:	Writes a block of data to EEP.  
;	- Enter with block number in RAM@EFSblkadr
;	- Calls the ReadEEP or WriteEEP routine.
;	- Preserves Y and Z pointer
;*********************************************************************
ReadBlk:
	CLRB	GPIOR1,BIO_RW,temp	;insure READ flag clear
	rjmp	RWBlk				;continue
WriteBlk:
	SETB	GPIOR1,BIO_RW,temp	;insure WRITE flag set

RWBlk:
	PUSHZ						;save all pointers
	PUSHY
	ldi		temp,EFSBLKSZ		;set up block size counter
	mov		bytecntr,temp
	SetZPtr	EFSbuff				;point to buffer
	LOAD	temp,EFSblkadr		;fetch desired block number
	rcall	MkBlkPtr			;get the EEP address pointer in Y
RWBlk1:
	SKBS	GPIOR1,BIO_RW,temp	;skip next if WRITE flag set		
	rjmp	RWBlk2				;want to READ
	ld		temp,Z+				;fetch byte from buffer
	rcall	WriteEEP			;write the byte to EEP
	rjmp	RWBlk3				;continue
RWBlk2:
	rcall	ReadEEP				;get a byte into temp
	st		Z+,temp				;store the byte in the buffer
RWBlk3:
	ADDI16	YH,YL,1				;increment EEP pointer
	dec		bytecntr			;decrement buffer counter
	brne	RWBlk1				;loop till zero

	POPY						;restore all pointers
	POPZ
	ret							;return to caller
;
;*********************************************************************
; DelBlk:	Deletes a block of data in EEP by erasing the
;	1st byte of the record.  
;	- Enter with block number in RAM@EFSblkadr
;	- Calls the WriteEEP routine.
;	- Destroys temp and Z
;	- Preserves Y
;*********************************************************************
DelBlk:
	PUSHY						;save Y
	rcall	MkBlkPtr			;get the EEP address pointer in Y
	SETB	GPIOR1,EEP_WE,temp	;set erase only flag
	set							;set T-flag, wsleep till write complete
	rcall	WriteEEP			;write the byte to EEP
	POPY						;restore Y
	ret							;return to caller
;
;*********************************************************************
; MkBlkPtr:	Creates the pointer to EEP.
;	- Enter with block number in RAM@EFSblkadr
;	- Returns with the EEP block address in YH:YL
;	- Destroys r1, r0 , temp and outchar
;*********************************************************************
MkBlkPtr:
	ldi		temp,EFSBLKSZ		;get block size
	LOAD	outchar,EFSblkadr	;get block number
	mul		temp,outchar		;get address in r1:r0	
	COPY16	YH,YL,r1,r0			;move to YH:YL
	ret
;
;*********************************************************************
; InitIdx:	Initialize the EFS file index pointers by writing "0"
;  to the starting block and size.
;*********************************************************************
InitIdx:
	ldi		inbytel,HANDLES_MAX*EFSHNDSZ	;load byte counter
	SETYPTR	EFSIDX0				;Y = 1st byte of file index pointers
	CLRB	GPIOR1,EEP_WE,temp	;clear flag to erase/write
	set							;set T-Flag to wait for EEP write 
InitIdx1:
	clr		temp				;insure the byte is zero
	GoSub	WriteEEP			;write it
	ADDI16	YH,YL,1				;increment EEP pointer
	dec		inbytel				;decrement loop counter
	brne	InitIdx1			;loop 
	ret							;return to caller
;
;*********************************************************************
; ClrIdx:	Clear the file index pointer by writing "0"
;  to the starting block for the requested file.
;	- Enter with index in U
;	- Preserves U	
;*********************************************************************
ClrIdx:
	CLRB	GPIOR1,EEP_WE,temp	;clear flag for erase/write combo
	clr		temp				;clear register
	clr		YH					;insure high register zero
	mov		YL,U				;YL = file handle to index
	set							;set T-Flag to wait for EEP write 
ClrIdx1:
	GoSub	WriteEEP			;write it
	ret							;return to caller
;
;*********************************************************************
; ReadBlkZero:	Reads block 0 of the file index pointer into the
;	RAM buffer.
;	- Return with block 0 in RAM buffer	
;	- Return with Y pointing to beginning of the RAM buffer
;*********************************************************************
ReadBlkZero:
	SetYPtr	EFSbuff				;point Y to RAM buffer
	STOREI	EFSblkadr,0			;point to block 0
	rcall	ReadBlk				;fetch it
	ret							;return to caller
;
;*********************************************************************
; WriteBlkZero:	Writes the RAM buffer into block 0 of the file system.
;	- Enter with contents of block 0 in RAM buffer	
;	- Return with Y pointing to beginning of the RAM buffer
;*********************************************************************
WriteBlkZero:
	SetYPtr	EFSbuff				;point Y to RAM buffer
	STOREI	EFSblkadr,0			;point to block 0
	rcall	WriteBlk			;save it
	ret							;return to caller
;
;*********************************************************************
; ReadBlkOne:	Reads the first data block of the file handle into 
;	the RAM buffer.
;	- Enter with index in U
;	- Return with 1st data block in RAM buffer	
;	- Destroys temp, r1, r0 and Y.
;*********************************************************************
ReadBlkOne:
	rcall	ReadBlkZero			;get block zero 
	mov		r0,U				;get file handle into temp
	ADD16	YH,YL,r1,r0			;get file index offset
	ld		temp,Y				;get the starting block
	STORE	EFSblkadr,temp		;point to next block
	rcall	ReadBlk				;fetch it
	ret							;return to caller
;
;*********************************************************************
; NextFreeBlk:	Scans EEP for the next available block.starting with
;	the block held in RAM@EFSblkadr.
;	- Enter with starting block number in RAM@EFSblkadr
;	- Returns with next free block number in RAM@EFSnfb (and bytecntr)
;	- Uses temp, nytecntr and Y pointer. 
;*********************************************************************
NextFreeBlk:
	PUSHY						;save Y
	LOAD	bytecntr,EFSblkadr	;load start block from RAM@EFSblkadr
	STORE	EFSlab,bytecntr		;place in RAM@EFSlab
NextFreeBlk1:
	inc		bytecntr			;increment block counter
	STORE	EFSblkadr,bytecntr	;save new block # to RAM@EFSblkadr
	rcall	MkBlkPtr			;get the EEP address into Y
	rcall	ReadEEP				;fetch the byte from EEP
	cpi		temp,EFSERAREC		;is it erased?
	brne	NextFreeBlk1		;loop till free block found
	POPY						;restore Y
	ret							;return to caller
;
;*********************************************************************
; AlocBlks:	Scans EEP and returns the number of free and used bytes.
;	- Calls the ReadEEP routine.
;	- Returns with free bytes in Z
;	- Returns with used bytes in Y
;	- Uses X, Y, Z and U
;	- Preserves U
;*********************************************************************
AlocBlks:
	PUSHX						;save X
	push	U					;save U
	clr		XH
	clr		XL
	ldi		U,HANDLES_MAX-1		;load U with max # of handles avail.

AlocBlks1:	;top of the counting loop
	GoSub	AlocByHndl			;get file's size for this handle
	ADD16	XH,XL,ZH,ZL			;add to total count
	dec		U					;decrement file handle 
	brpl	AlocBlks1			;loop till roll-under

AlocBlks2:	;finished the count loop, calculate free bytes.
	SetZPtr	(EFSPAYSZ*(EFSMAXBLK-1));load max available
	SUB16	ZH,ZL,XH,XL			;leave free bytes in Z	
	COPYXtoY					;move used bytes to Y
	pop		U					;restore U
	POPX						;restore X
	ret							;return to caller
;
;*********************************************************************
; AlocByHndl:	Calculates the amount of space taken by the given
;	file handle.
;	- Enter with handle in U
;	- Returns with number of bytes used in ZH:ZL.
;	- Uses U, H, T, temp, inbytel, outchar, Y and Z pointers. 
;*********************************************************************
AlocByHndl:
	push	U					;save U
	SetZPtr	EFSbuff				;point to buffer
	LOADI16	inbyteh,inbytel,0	;clear byte counter
	LOADI16	H,T,EFSPAYSZ		;load data payload size
	rcall	ReadBlkZero			;fetch data block 0

	mov		r0,U				;get file handle into temp
	ADD16	ZH,ZL,r1,r0			;get file index offset
	ld		temp,Z				;get the starting block
	tst		temp				;is it empty?
	breq	AlocByHndl9			;yes, exit
	SetZPtr	EFSbuff				;reset buffer pointer

AlocByHndl1:	;top of the counting loop
	STORE	EFSblkadr,temp		;point to next block
	rcall	ReadBlk				;fetch next data block
	ld		temp,Z				;get the record type
	cpi		temp,EFSDATREC		;is it a data record?
	SKBS	SREG,SREG_Z,temp	;skip next if data record
	rjmp	AlocByHndl9			;empty block, exit
	ADD16	inbyteh,inbytel,H,T	;add to free byte counter
	ldd		temp,Z+(EFSBLKSZ-1)	;get next record pointer
	tst		temp				;is next block empty?
	brne	AlocByHndl1			;loop, loop for more

AlocByHndl9:
	COPY16	ZH,ZL,inbyteh,inbytel;copy count to ZH:ZL
	pop		U					;restore U
	ret							;return to caller
;
.endif
;
;*********************************************************************
; ReadEEP:	Reads a byte from EEP.  
;	- Enter with address in Y pointer
;	- Returns with data in temp
;*********************************************************************
ReadEEP:
	STORE16	eearL,YH,YL			;set EEPROM address register
	SETB	eecr,eere			;Trigger the read
	LOAD	temp,eedr			;Get the data into temp
	ret							;return to caller
;
;*********************************************************************
; WriteEEP:	Writes a byte to EEP.  
;	- Enter with address in Y pointer
;	- Enter with data in temp
;	- If T-Flag set upon entry, will sleep till erase/write is finished
;*********************************************************************
WriteEEP:
	STORE16	eearL,YH,YL			;setup the address
	STORE	eedr,temp			;setup the  write data
	PUSHSTAT					;save SREG
	sei							;insure interrupts enabled
	ldi		outchar,(1<<EEMPE)	;setup for erase/write combo
	SKBC	GPIOR1,EEP_WE,r0	;skip next if erase/write combo
	ldi		outchar,((1<<EEPM0) | \
					(1<<EEMPE))	;set up for erase only

	STORE	EECR,outchar		;write the mode and MPE flag	
	SETB	EECR,EEPE			;Trigger the write
	brtc	WriteEEPout			;T-Flag clear, exit

WriteEEPa:
	brid	WriteEEPb			;do not sleep unless INT enabled
#if !TEST	;do not sleep if testing
	sleep						;sleep while EEPROM programming
	nop
#endif
WriteEEPb:
	SKBC	EECR,EEPE			;check EEPROM write has finished
	rjmp	WriteEEPa			;must have been another IRQ source

WriteEEPout:
	POPSTAT						;restore SREG
	ret							;return to caller
;
