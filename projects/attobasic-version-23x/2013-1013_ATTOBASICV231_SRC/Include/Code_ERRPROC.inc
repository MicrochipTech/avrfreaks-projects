;**********************************************************
;ERROR: Error Processing routine
;***********************************************************	
Error:
	GoSub	crlf
	clr		r1					;for 16-bit addition to Z-ptr

	LOAD	r0,ERRCode			;get error code into temp
	mov		ecode,r0			;save a copy
	lsl		r0					;multiply by 2 for byte address
	SetZPtr	(2*ERRORTbl)		;point to string table
	ADD16	ZH,ZL,r1,r0			;add to Z-ptr
	lpm		temp,Z+				;get low word of error string
	lpm		ZH,Z				;get high word of error string
	mov		ZL,temp

	rol		ZL					;multiply address by two for byte offset
	rol		ZH

	GoSub	sendromstring		;sent it.
	GoSub	crlf

	GoSub	PrnLnNo				;print the line number of the error

Error9:
	InitSP						;init stack pointer to RAMEND
	INITDSTACK					;Initialize data stack pointer
	clr		XL					;Clear interpreter mode flags
	clr		XH
	STOREI	CurrLine,0			;clear current line counter
	GoSub	crlf
	Goto	main

ERRORTbl:
	.dw		emessage00				;Error code 0
	.dw		emessage01				;Error code 1
	.dw		emessage02				;Error code 2
	.dw		emessage03				;Error code 3
	.dw		emessage04				;Error code 4
	.dw		emessage05				;Error code 5
	.dw		emessage06				;Error code 6
	.dw		emessage07				;Error code 7
	.dw		emessage08				;Error code 8
	.dw		emessage09				;Error code 9
	.dw		emessage0A				;Error code A
	.dw		emessage0B				;Error code B
	.dw		emessage0C				;Error code C
	.dw		emessage0D				;Error code D
	.dw		emessage0E				;Error code E
	.dw		emessage0F				;Error code F
	.dw		emessage10				;Error code 10
	.dw		emessage11				;Error code 11
	.dw		emessage12				;Error code 12
	.dw		emessage13				;Error code 13
	.dw		emessage14				;Error code 14
	.dw		emessage15				;Error code 15
	.dw		emessage16				;Error code 16
	.dw		emessage17				;Error code 17
	.dw		emessage18				;Error code 18
	.dw		emessage19				;Error code 19
	.dw		emessage1A				;Error code 1A
	.dw		emessage1B				;Error code 1B
	.dw		emessage1C				;Error code 1C
	.dw		emessage1D				;Error code 1D
	.dw		emessage1E				;Error code 1D
	.dw		emessage1F				;Error code 1F
