;compiler: avrasm

;************************************************************************************************
; Embeded MOD/S3M player for AVR ATmega644.
;
; module: USART routines
;************************************************************************************************
; Licence: This source code is free to use for any non-comercial purposes.
;          (C) 2011, Stanislav Maslan, s.maslan@seznam.cz 
;************************************************************************************************



		
;************************************************************************************************
;	USART routines
;************************************************************************************************
;------------------------------------------------------------------------------------------------
;	Tx R16 char
;------------------------------------------------------------------------------------------------
UsTxChar:	lds	R17,ucsr0a
	sbrs	R17,udre0
	rjmp	UsTxChar
	sts	udr0,R16
	ret
	
;------------------------------------------------------------------------------------------------
;	Tx R16 char
;------------------------------------------------------------------------------------------------
UsTxWord:	push	R17
	rcall	UsTxChar
	pop	R16
	rjmp	UsTxChar
	
;------------------------------------------------------------------------------------------------
;	Tx R16 char - UsTxC Char 
;------------------------------------------------------------------------------------------------
	.macro	UsTxC
	ldi	R16,@0
	rcall	UsTxChar
	.endm

;------------------------------------------------------------------------------------------------
;	Tx flash string (Z pointed)
;------------------------------------------------------------------------------------------------
	;wait for Tx ready
UsTxStrFlash:	lds	R16,ucsr0a
	sbrs	R16,udre0
	rjmp	UsTxStrFlash
	;get char
	lpm	R16,Z+
	;test for line end
	cpi	R16,0x00
	breq	UsTxStrFlashF
	sts	udr0,R16	;tx char
	;test for line break
	cpi	R16,0x0D
	brne	UsTxStrFlash
	;break line - print lf	
UsTxStrFlashBW1:	lds	R16,ucsr0a
	sbrs	R16,udre0
	rjmp	UsTxStrFlashBW1
	stsi	udr0,0x0A
	rjmp	UsTxStrFlash
UsTxStrFlashF:	ret

;------------------------------------------------------------------------------------------------
;	Tx flash string - UsTxFS TText
;------------------------------------------------------------------------------------------------
	.macro	UsTxFS
	ldi	R30,low(@0*2)
	ldi	R31,high(@0*2)
	rcall	UsTxStrFlash
	.endm

;------------------------------------------------------------------------------------------------
;	Tx RAM string (Z pointed)
;------------------------------------------------------------------------------------------------
	;wait for Tx ready
UsTxStrRam:	lds	R17,ucsr0a
	sbrs	R17,udre0
	rjmp	UsTxStrRam
	;get char
	ld	R17,Z+
	sts	udr0,R17
	push	R16
	ldi	R16,1
	call	Delay1ms
	pop	R16
	
	dbnz	R16,UsTxStrRam
	ret
	

;------------------------------------------------------------------------------------------------
;	Tx RAM string - UsTxRS TText
;------------------------------------------------------------------------------------------------
	.macro	UsTxRS
	ldi	R16,@0
	ldi	R30,low(@1)
	ldi	R31,high(@1)
	rcall	UsTxStrRam
	.endm
	
	
;------------------------------------------------------------------------------------------------
;	Receive file via USART, automatic end detection
;	- all irq's longer than few cycles should be disabled! 
;------------------------------------------------------------------------------------------------	
LoadData:	;port configuration
	.if	LCDON!=0
	call	LCDclrAll
	ldiw	R30,TRData*2
	call	LCDwrStrFlash
	.endif
	
	BtInh	1
	sbi	pROE,ROE	;OE=1
	outi	ddra,0xFF	;ouputs
	outi	ddrc,0xFF	;ouputs

	;SRAM pointer
	ldit	R18,0
	
	;SRAM size
	lds	R21,SRAMsize
	
	;flush USART recieved data
	lds	R16,udr0
	ldi	R16,1
	call	Delay1ms
	lds	R16,udr0
	ldi	R16,1
	call	Delay1ms
	lds	R16,udr0
		
	ToTime	150	;1.5s timeout
	ToRes
	ToRun		;start timeout timer
	
	;wait for first byte
;LoadDataWF:	rjsc	ucsr0a,rxc0,LoadDataWF
;	rjmp	LoadDataS	;first byte in usart rx FIFO 
	
	;wait for next byte, check timeout
LoadDataWN:	lds	R16,TimOut
	cpi	R16,0xFF
	breq	LoadDataTO
	rjsc	ucsr0a,rxc0,LoadDataWN
	
	;store byte into SRAM, addr++
LoadDataS:	lds	R17,udr0	;read byte
	out	portc,R17	;send out
	out	porta,R20	;high addr
	cbi	pRHA,RHA
	sbi	pRHA,RHA
	out	porta,R19	;med addr
	cbi	pRMA,RMA
	sbi	pRMA,RMA
	out	porta,R18	;low addr
	subit	R18,-1	;inc address
	cbi	pRWR,RWR	;rw=0
	cp	R20,R21	;test for memory overflow
	sbi	pRWR,RWR	;rw=1
	
	brsh	LoadDataTO	;memory full - abort!	
		
	ToRes		;reset timeout
	rjmp	LoadDataWN	
	
	;timeout or full memory
LoadDataTO:	ToStop		;stop timeout timer	
	
	
	clr	R16
	tstt	R18,R16
	clt
	breq	PC+1+1+6
	set
	stst	FileLen,R18	;store file len
		
	;safe port configuration
	outi	ddra,0x00	;inputs
	outi	ddrc,0x00	;inputs
	BtInh	0
	ret
