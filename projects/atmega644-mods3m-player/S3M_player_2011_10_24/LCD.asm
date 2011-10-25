;compiler: avrasm

;************************************************************************************************
; Embeded MOD/S3M player for AVR ATmega644.
;
; module: LCD routines 
;************************************************************************************************
; Licence: This source code is free to use for any non-comercial purposes.
;          (C) 2011, Stanislav Maslan, s.maslan@seznam.cz 
;************************************************************************************************



	
;************************************************************************************************
;	8-bit HD44780 compatibile LCD routines
;************************************************************************************************
;------------------------------------------------------------------------------------------------
;	send R16 byte, wait ~42us
;------------------------------------------------------------------------------------------------
LCDwrByte:	.if	LCDON!=0
	pusht	R16
	in	R17,pROE
	sbi	pROE,ROE	;SRAM OE=1
	in	R18,ddrc
	out	portc,R16	;put byte on port
	outi	ddrc,0xFF	;set it as output
	adiw	R30,0
	sbi	pLCK,LCK
	adiw	R30,0	
	cbi	pLCK,LCK	;clock
	ldi	R16,41*XTAL/(7*1000000)	;delay
	adiw	R30,0
	adiw	R30,0
	dbnz	R16,PC-1-1-1
	out	ddrc,R18	;set port back
	tst	R18
	brne	PC+1+2
	outi	portc,0xFF	;pull-ups if configured as inputs
	out	pROE,R17
	popt	R16
	ret
	.endif
	
;------------------------------------------------------------------------------------------------
;	send R16 instruction
;------------------------------------------------------------------------------------------------
LCDwrInstr:	.if	LCDON!=0
	cbi	pLRS,LRS	;RS=0
	rjmp	LCDwrByte	;send byte
	.endif

;------------------------------------------------------------------------------------------------
;	send R16 data
;------------------------------------------------------------------------------------------------
LCDwrData:	.if	LCDON!=0
	sbi	pLRS,LRS	;RS=1
	rjmp	LCDwrByte	;send byte
	.endif

;------------------------------------------------------------------------------------------------
;	clear entire DDRAM
;------------------------------------------------------------------------------------------------
LCDclrFull:	.if	LCDON!=0
	ldi	R16,0|128
	rcall	LCDwrInstr	;reset cursor
	;write spaces
	ldi	R18,0x80
LCDclrFullL1:	ldi	R16,' '
	rcall	LCDwrData
	dbnz	R18,LCDclrFullL1
	ldi	R16,0|128
	rcall	LCDwrInstr	;reset cursor	
	ret
	.endif
	
;------------------------------------------------------------------------------------------------
;	clear 2x20 chars (write ' ')
;------------------------------------------------------------------------------------------------
LCDclrAll:	.if	LCDON!=0
	;write spaces line 1
	ldi	R16,0|128
	rcall	LCDwrInstr	;reset cursor
	ldi	R17,20
LCDclrAllL1:	ldi	R16,' '
	rcall	LCDwrData
	dbnz	R17,LCDclrAllL1
	;write spaces line 2
	ldi	R16,0x40|128
	rcall	LCDwrInstr	;reset cursor
	ldi	R17,20
LCDclrAllL2:	ldi	R16,' '
	rcall	LCDwrData
	dbnz	R17,LCDclrAllL2
	ldi	R16,0|128
	rcall	LCDwrInstr	;reset cursor	
	ret
	.endif	

;------------------------------------------------------------------------------------------------
;	clear first line
;------------------------------------------------------------------------------------------------
LCDclrL1:	.if	LCDON!=0
	ldi	R16,0|128
	rcall	LCDwrInstr	;reset cursor
	ldi	R17,20
LCDclrL1L1:	ldi	R16,' '
	rcall	LCDwrData
	dbnz	R17,LCDclrL1L1
	ldi	R16,0|128
	rcall	LCDwrInstr	;reset cursor
	ret
	.endif
	
;------------------------------------------------------------------------------------------------
;	clear second line
;------------------------------------------------------------------------------------------------
LCDclrL2:	.if	LCDON!=0
	ldi	R16,0x40|128
	rcall	LCDwrInstr	;reset cursor
	ldi	R17,20
LCDclrL2L1:	ldi	R16,' '
	rcall	LCDwrData
	dbnz	R17,LCDclrL2L1
	ldi	R16,0x40|128
	rcall	LCDwrInstr	;reset cursor
	ret
	.endif	
	
;------------------------------------------------------------------------------------------------
;	set pos to line 1
;------------------------------------------------------------------------------------------------
LCDposL1:	.if	LCDON!=0
	push	R16
	ldi	R16,0x00|128
	rcall	LCDwrInstr
	pop	R16
	ret
	.endif

;------------------------------------------------------------------------------------------------
;	set pos to line 2
;------------------------------------------------------------------------------------------------	
LCDposL2:	.if	LCDON!=0
	push	R16
	ldi	R16,0x40|128
	rcall	LCDwrInstr
	pop	R16
	ret
	.endif	

;------------------------------------------------------------------------------------------------
;	init LCD as 8-bit 2x20 characters
;------------------------------------------------------------------------------------------------
LCDinit:	.if	LCDON!=0
	ldi	R16,0b00001100
	rcall	LCDwrInstr	;display on
	ldi	R16,0b00111000
	rcall	LCDwrInstr	;2lines, 8-bit, 5x8 char
	rcall	GenerovatZnaky	;generate VUs
	rcall	LCDclrFull	;clear all DDRAM
	ret
	.endif
	
;------------------------------------------------------------------------------------------------
;	send Z pointed FLASH string (0 terminated)
;------------------------------------------------------------------------------------------------
LCDwrStrFlash:	.if	LCDON!=0
	lpm	R16,Z+
	cpi	R16,0x0D
	brne	PC+1+1+1+1
	ldi	R16,0x40|128
	rcall	LCDwrInstr	;goto second line
	rjmp	LCDwrStrFlash
	cpi	R16,0
	brne	PC+1+1
	ret		;leave if R16=0
	rcall	LCDwrData
	rjmp	LCDwrStrFlash
	.endif
	
;------------------------------------------------------------------------------------------------
;	send Z pointed FLASH string (0 terminated), usage: LwrFS TText
;------------------------------------------------------------------------------------------------	
	.macro	LwrFS
	ldi	R31,high(@0*2)
	ldi	R30,low(@0*2)
	rcall	LCDwrStrFlash
	.endm
	
;------------------------------------------------------------------------------------------------
;	send Z pointed RAM string
;------------------------------------------------------------------------------------------------
LCDwrStrRam:	.if	LCDON!=0
	ld	R16,Z+
	rcall	LCDwrData
	dbnz	R17,LCDwrStrRam
	ret
	.endif
	
;------------------------------------------------------------------------------------------------
;	vygeneruje znaky pro bargrafy do CGRAM
;------------------------------------------------------------------------------------------------
GenerovatZnaky:	.if	LCDON!=0
	ldi	R19,0b01000000
	ldi	R17,8
	ldi	R18,1
GenZnaky1:	;cbi	portb,1
	mov	R16,R19
	inc	R19
	rcall	LCDwrInstr
	;call	ByteToLCD
	;sbi	portb,1
	ldi	R16,0b00000000
	cp	R18,R17
	brcs	NeBlack
	ldi	R16,0b00011111
NeBlack:	;call	ByteToLCD
	rcall	LCDwrData
	inc	R18
	cpi	R18,9
	brne	DalsiBarCh
	ldi	R18,1
	dec	R17
DalsiBarCh:	cpi	R19,0b10000000
	brne	GenZnaky1
	;cbi	portb,1
	ret
	.endif


;------------------------------------------------------------------------------------------------
;	print 2 digits of R16 number, convert leading zeroes to ' '
;------------------------------------------------------------------------------------------------
LCDwrBCD2:	.if	LCDON!=0
	call	bin2BCD8u2
	push	R16
	mov	R16,R17
	clt
	rcall	LCDwrBCD3sc
	pop	R16
	set
	rcall	LCDwrBCD3sc
	ret
	.endif

;------------------------------------------------------------------------------------------------
;	print 3 digits of R16 number, convert leading zeroes to ' '
;------------------------------------------------------------------------------------------------	
LCDwrBCD3:	.if	LCDON!=0
	call	bin2BCD8u3
	push	R16
	push	R17
	clt
	mov	R16,R18
	rcall	LCDwrBCD3sc
	pop	R16
	rcall	LCDwrBCD3sc
	set
	pop	R16
	rcall	LCDwrBCD3sc
	ret
LCDwrBCD3sc:	cpi	R16,0
	breq	PC+1+1
	set
	brts	PC+1+1
	ldi	R16,' '-'0'
	subi	R16,-'0'
	rcall	LCDwrData
	ret
	.endif

;------------------------------------------------------------------------------------------------
;	print 3 digits of R16 number, skip leading zeroes
;------------------------------------------------------------------------------------------------
LCDwrBCD3sz:	.if	LCDON!=0
	call	bin2BCD8u3
	pushw	R16
	clt
	mov	R16,R18
	rcall	LCDwrBCD3szc
	pop	R16	
	rcall	LCDwrBCD3szc
	set
	pop	R16
	rcall	LCDwrBCD3szc
	ret	
LCDwrBCD3szc:	brts	PC+1+1+1+1
	cpi	R16,0
	breq	PC+1+1+1+1
	set
	subi	R16,-'0'
	rcall	LCDwrData
	ret
	.endif

;------------------------------------------------------------------------------------------------
;	print 8 digits of R16:R17:R18 number, skip leading zeroes
;------------------------------------------------------------------------------------------------
LCDwrBCD8sz:	.if	LCDON!=0
	call	bin2BCD24p8	;convert to BCD
	clt
	mov	R16,R15
	swap	R16
	rcall	LCDwrBCDsz
	mov	R16,R15
	rcall	LCDwrBCDsz
	mov	R16,R14
	swap	R16
	rcall	LCDwrBCDsz
	mov	R16,R14
	rcall	LCDwrBCDsz
	mov	R16,R13
	swap	R16
	rcall	LCDwrBCDsz
	mov	R16,R13
	rcall	LCDwrBCDsz
	mov	R16,R12
	swap	R16
	rcall	LCDwrBCDsz
	set
	mov	R16,R12
	rcall	LCDwrBCDsz
	ret
LCDwrBCDsz:	andi	R16,0x0F
	brts	PC+1+1+1
	brne	PC+1+1
	ret
	set
	subi	R16,-'0'
	rcall	LCDwrData
	ret
	.endif

;------------------------------------------------------------------------------------------------
;	send Z pointed RAM string, usage: LwrRS len TText
;------------------------------------------------------------------------------------------------	
	.macro	LwrRS
	ldi	R17,@0
	ldi	R31,high(@1)
	ldi	R30,low(@1)
	rcall	LCDwrStrRam
	.endm	
	

;------------------------------------------------------------------------------------------------
;	Draw MOD play status
;------------------------------------------------------------------------------------------------
LCDwrMODln:	.if	LCDON!=0
	ldi	R16,0x40|128
	rcall	LCDwrInstr	;second line
	
	;print line
	lds	R16,MODpln
	dec	R16
	rcall	LCDwrBCD2
	ldi	R16,'/'
	rcall	LCDwrData
	
	;print pattern
	lds	R16,MODpat
	inc	R16
	rcall	LCDwrBCD3
	ldi	R16,'/'
	rcall	LCDwrData
	
	;print total pattern count
	lds	R16,MODpatn
	rcall	LCDwrBCD3
	ret
	.endif
	
	
;------------------------------------------------------------------------------------------------
;	Draw MOD vu's
;------------------------------------------------------------------------------------------------
LCDwrMODvu:	.if	LCDON!=0
	ldi	R16,(0x40+20)|128
	lds	R17,MODchnn
	cpi	R17,10+1
	brlo	PC+1+1
	ldi	R17,10
	sub	R16,R17
	push	R17	;channels to print
	rcall	LCDwrInstr	;second line
	;
	ldiw	R28,MODbars+MAXCH	;sample peak detection buffer
	ldiw	R26,MODbarsInt	;internal peak buffer
	ldiw	R30,MODpbck	;playback structure
	pop	R24	;channels to print
LCDwrMODlnBGL1:	;detect real peak value
	ld	R16,-Y	;get peak from peak detection buffer
	ldi	R17,0
	st	Y,R17	;clear peak detector
	cpi	R16,0
	brpl	PC+1+1
	ldi	R16,0	;peak to <0;127>
	ldd	R17,Z+1	;get channel volume
	lsl	R17	;channel volume <0;128>
	fmul	R16,R17	;apply volume R1=real channel peak <-128;+127>
	ld	R16,X	;last value peak
	cp	R1,R16
	brlo	PC+1+1
	mov	R16,R1	;new peak value
	st	X,R16	;store actual peak value
	
	;write peak to LCD	
	swap	R16
	andi	R16,0x0F	;real channel peak <0;7>
	rcall	LCDwrData	;write peak
	
	;falling peaks
	ld	R16,X
	subi	R16,20	;fall rate
	brpl	PC+1+1
	ldi	R16,0	
	st	X+,R16

	;next channel
	adiw	R30,MODpbckSize
	dbnz	R24,LCDwrMODlnBGL1
	ret
	.endif	
	