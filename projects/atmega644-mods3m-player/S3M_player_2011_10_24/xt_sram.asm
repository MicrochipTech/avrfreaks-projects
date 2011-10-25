;compiler: avrasm

;************************************************************************************************
; Embeded MOD/S3M player for AVR ATmega644.
;
; module: external SRAM routines
;************************************************************************************************
; Licence: This source code is free to use for any non-comercial purposes.
;          (C) 2011, Stanislav Maslan, s.maslan@seznam.cz 
;************************************************************************************************





;************************************************************************************************
;	External SRAM
;************************************************************************************************
;------------------------------------------------------------------------------------------------
;	Get byte: R16=SRAM(R2:R3:R4), R2:R3:R4+=R5:R6:R7 
;------------------------------------------------------------------------------------------------
RamGetByteA:	out	porta,R4
	cbi	pRHA,RHA
	sbi	pRHA,RHA
	out	porta,R3
	cbi	pRMA,RMA
	sbi	pRMA,RMA
	out	porta,R2
	.if	HCOC==1
	nop
	.endif
	addt	R2,R5	;address+=R5:R6:R7
	in	R16,pinc
	ret
	
;------------------------------------------------------------------------------------------------
;	Put byte: SRAM(R2:R3:R4)=R16, R2:R3:R4+=R5:R6:R7 
;------------------------------------------------------------------------------------------------
RamPutByteA:	out	portc,R16
	out	porta,R4
	cbi	pRHA,RHA
	sbi	pRHA,RHA
	out	porta,R3
	cbi	pRMA,RMA
	sbi	pRMA,RMA
	out	porta,R2
	addt	R2,R5	;address+=R5:R6:R7
	cbi	pRWR,RWR	;rw=0
	nop
	sbi	pRWR,RWR	;rw=1
	ret	


;------------------------------------------------------------------------------------------------
;	External SRAM check 
;------------------------------------------------------------------------------------------------
SRAMtest:	;config ports/signals
	BtInh	1
	outi	ddra,0xFF	;outputs
	
	.if	LCDON!=0
	call	LCDclrAll
	ldiw	R30,TSRchk*2
	call	LCDwrStrFlash
	.endif
	
	;------ fast mem size detection in 64kiB blocks ------
	clr	R3
	clrt	R5	;SRAM address step
	inc	R5

	ldi	R17,0x00	;initial block
	ldi	R25,128	;max count of tested blocks
	;write mark to new block {R17}
SRAMtestBTL2:	sbi	pROE,ROE	;SRAM OE=1
	outi	ddrc,0xFF	;outputs
	;
	clr	R2
	mov	R4,R17	;select block
	;
	mov	R16,R17
	rcall	RamPutByteA
	mov	R16,R17
	com	R16
	rcall	RamPutByteA
	inc	R17	;next block

	;check all marks of all written blocks {0...(R17-1)}
	outi	ddrc,0x00	;inputs
	outi	portc,0xFF	;pull-ups
	cbi	pROE,ROE	;SRAM OE=0
	;
	ldi	R18,0x00
	mov	R24,R17
SRAMtestBTL1:	clr	R2
	mov	R4,R18	;select block
	rcall	RamGetByteA
	cp	R16,R18
	brne	SRAMtestBTF	;done if not eq.
	rcall	RamGetByteA
	com	R18
	cp	R16,R18
	brne	SRAMtestBTF	;done if not eq.
	com	R18
	inc	R18
	;	
	dbnz	R24,SRAMtestBTL1
	;
	dbnz	R25,SRAMtestBTL2
	;all successfully done, R17 contains valid block count
	rjmp	PC+1+1
SRAMtestBTF:	dec	R17
	sts	SRAMsize,R17
	cpi	R17,0
	.if	MEMTST!=0
	rjeq	SRAMtestRF	;skip if no mem detected
	.else
	brne	SRAMtestRS
	.endif


	
	
	;------ full mem test of detected size ------
	.if	MEMTST!=0
	clr	R0	;0
	clr	R1
	inc	R1	;1
	;write ids ...
	sbi	pROE,ROE	;SRAM OE=1
	outi	ddrc,0xFF	;outputs
	;
	clrd	R2	
	lds	R18,SRAMsize
SRAMtestWL3:	out	porta,R4	;high addr
	cbi	pRHA,RHA
	sbi	pRHA,RHA
	
	ldi	R17,256
SRAMtestWL2:	out	porta,R3	;med addr
	cbi	pRMA,RMA
	sbi	pRMA,RMA
	;
	ldi	R16,256/4
	;store DWORD
SRAMtestWL1:	out	porta,R2	;low addr
	out	portc,R2	;send out
	adiw	R30,0
	cbi	pRWR,RWR	;rw=0
	nop
	sbi	pRWR,RWR	;rw=1
	inc	R2
	;
	out	porta,R2	;low addr
	out	portc,R3	;send out
	adiw	R30,0
	cbi	pRWR,RWR	;rw=0
	nop
	sbi	pRWR,RWR	;rw=1
	inc	R2
	;
	out	porta,R2	;low addr
	out	portc,R4	;send out
	adiw	R30,0
	cbi	pRWR,RWR	;rw=0
	nop
	sbi	pRWR,RWR	;rw=1
	inc	R2
	;
	out	porta,R2	;low addr
	out	portc,R5	;send out
	adiw	R30,0
	cbi	pRWR,RWR	;rw=0
	nop
	sbi	pRWR,RWR	;rw=1
	inc	R2
	;
	dbnz	R16,SRAMtestWL1
	;
	add	R3,R1
	adc	R4,R0
	adc	R5,R0
	;
	dbnz	R17,SRAMtestWL2
	;
	dbnz	R18,SRAMtestWL3
	
	;read and check ids
	outi	ddrc,0x00	;inputs
	outi	portc,0xFF	;pull-ups
	cbi	pROE,ROE	;SRAM OE=0
	;
	ldi	R24,3
	ldi	R25,4
	clrd	R2	
	lds	R18,SRAMsize
SRAMtestRL3:	out	porta,R4	;high addr
	cbi	pRHA,RHA
	sbi	pRHA,RHA
	
	ldi	R17,256
SRAMtestRL2:	out	porta,R3	;med addr
	cbi	pRMA,RMA
	sbi	pRMA,RMA
	;
	ldi	R16,256/4
	;
SRAMtestRL1:	out	porta,R2	;low addr
	inc	R2
	nop
	nop	
	.if	HCOC==1
	nop
	.endif
	in	R20,pinc
	;
	out	porta,R2	;low addr
	inc	R2
	nop
	nop	
	.if	HCOC==1
	nop
	.endif
	in	R21,pinc
	;
	out	porta,R2	;low addr
	inc	R2
	nop
	nop	
	.if	HCOC==1
	nop
	.endif
	in	R22,pinc
	;
	out	porta,R2	;low addr
	sub	R2,R24
	cp	R20,R2
	cpc	R21,R3
	.if	HCOC==1
	nop
	.endif
	in	R23,pinc
	
	cpc	R22,R4
	cpc	R23,R5
	brne	SRAMtestRF
	add	R2,R25
	;
	dbnz	R16,SRAMtestRL1
	;
	add	R3,R1
	adc	R4,R0
	adc	R5,R0
	;
	dbnz	R17,SRAMtestRL2
	;
	dbnz	R18,SRAMtestRL3
	;success
	rjmp	SRAMtestRS
	;failed
	.endif
SRAMtestRF:	.if	LCDON!=0
	ldiw	R30,TSRfail*2
	call	LCDwrStrFlash
	.endif
	rjmp	PC
	
	;passed - print found size
SRAMtestRS:	.if	LCDON!=0
	lds	R16,SRAMsize
	ldi	R17,64
	mul	R16,R17
	movw	R16,R0
	clr	R18
	call	LCDwrBCD8sz
	ldiw	R30,TUkB*2
	call	LCDwrStrFlash
	ldi	R16,70
	rcall	Delay10ms
	.endif

	;safe port configuration
	sbi	pROE,ROE	;SRAM OE=1
	outi	ddra,0x00	;inputs
	outi	ddrc,0x00	;inputs
	
	BtInh	0
	ret