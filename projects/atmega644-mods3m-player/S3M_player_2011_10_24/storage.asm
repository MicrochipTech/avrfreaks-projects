;compiler: avrasm

;************************************************************************************************
; Embeded MOD/S3M player for AVR ATmega644.
;
; module: IDE interface, FAT16 routines
;************************************************************************************************
; Licence: This source code is free to use for any non-comercial purposes.
;          (C) 2011, Stanislav Maslan, s.maslan@seznam.cz 
;************************************************************************************************




;************************************************************************************************
;	IDE device routines
;************************************************************************************************
	.if	STORA!=0
	.dseg
IDEsize:	.byte	4
	.cseg
	.endif

	;IDE register addresses for CS0=0, CS1=1
	.equ	IDEdataReg=	0<<IOA0|0<<IOA1|0<<IOA2
	.equ	IDEfeaturesReg=	1<<IOA0|0<<IOA1|0<<IOA2
	.equ	IDEscReg=	0<<IOA0|1<<IOA1|0<<IOA2
	.equ	IDEsnReg=	1<<IOA0|1<<IOA1|0<<IOA2
	.equ	IDEclReg=	0<<IOA0|0<<IOA1|1<<IOA2
	.equ	IDEchReg=	1<<IOA0|0<<IOA1|1<<IOA2
	.equ	IDEsdhReg=	0<<IOA0|1<<IOA1|1<<IOA2
	.equ	IDEcomReg=	1<<IOA0|1<<IOA1|1<<IOA2
	.equ	IDEstatReg=	1<<IOA0|1<<IOA1|1<<IOA2
	
	;IDE commands
	.equ	IDEcomIdent=0xEC
	.equ	IDEcomSetFeat=0xEF
	.equ	IDEcomRdSec=0x20
	.equ	IDEcomWrSec=0x30
	.equ	IDEcomSetPar=0x91
		

;------------------------------------------------------------------------------------------------
;	Set IDE device register address (A0:A1:A2)
;------------------------------------------------------------------------------------------------
IDEsetReg:	.if	STORA!=0
	BtInh	1
	push	R17
	out	porta,R16	;put device address out
	in	R17,ddra	;backup address port direction
	outi	ddra,0xFF	;set address port as output
	cbi	pRHA,RHA
	sbi	pRHA,RHA	;generate high address latch clock
	out	ddra,R17	;restore address port direction 
	pop	R17
	BtInh	0
	ret
	.endif

;------------------------------------------------------------------------------------------------
;	Wait for RDY flag 
;------------------------------------------------------------------------------------------------	
IDEwaitRDY:	.if	STORA!=0
	push	R16
	ldi	R16,IDEstatReg
	rcall	IDEsetReg	;set status register
	pushw	R17
	in	R18,pROE	;backup SRAM OE state
	sbi	pROE,ROE	;SRAM OE=1
	in	R17,ddrc	;backup data port direction
	outi	ddrc,0x00
	outi	portc,0xFF	;input with pull-ups
IDEwaitRDY1:	rcall	IDEiord	;read status
	bst	R16,7	;busy flag
	brts	IDEwaitRDY1
	bst	R16,6	;rdy flag
	brtc	IDEwaitRDY1
	out	ddrc,R17	;restore data port direction
	tst	R17
	brne	PC+1+2
	outi	portc,0xFF	;pull-ups if data port configured as input
	out	pROE,R18	;restore SRAM OE state
	popw	R17
	pop	R16
	ret
	.endif
	
;------------------------------------------------------------------------------------------------
;	Test for RDY flag (T=RDY) 
;------------------------------------------------------------------------------------------------	
IDEtestRDY:	.if	STORA!=0
	push	R16
	ldi	R16,IDEstatReg
	rcall	IDEsetReg	;set status register
	pushw	R17
	in	R18,pROE	;backup SRAM OE state
	sbi	pROE,ROE	;SRAM OE=1
	in	R17,ddrc	;backup data port direction
	outi	ddrc,0x00
	outi	portc,0xFF	;input with pull-ups
	rcall	IDEiord	;read status
	;busy?
	bst	R16,7	;busy flag
	brtc	PC+1+1+1
	;yeah - !RDY yet
	clt		;!RDY
	rjmp	PC+1+1
	bst	R16,6	;rdy flag
	out	ddrc,R17	;restore data port direction
	tst	R17
	brne	PC+1+2
	outi	portc,0xFF	;pull-ups if data port configured as input
	out	pROE,R18	;restore SRAM OE state
	popw	R17
	pop	R16
	ret
	.endif	
	

;------------------------------------------------------------------------------------------------
;	Wait for DRQ flag
;------------------------------------------------------------------------------------------------
IDEwaitDRQ:	.if	STORA!=0
	push	R16
	ldi	R16,IDEstatReg
	rcall	IDEsetReg	;set status register
	pushw	R17
	in	R18,pROE	;backup SRAM OE state
	sbi	pROE,ROE	;SRAM OE=1
	in	R17,ddrc	;backup data port direction
	outi	ddrc,0x00
	outi	portc,0xFF	;input with pull-ups
IDEwaitDRQ1:	rcall	IDEiord	;read status
	bst	R16,3	;busy flag
	brtc	IDEwaitDRQ1
	out	ddrc,R17	;restore data port direction
	tst	R17
	brne	PC+1+2
	outi	portc,0xFF	;pull-ups if data port configured as input
	out	pROE,R18	;restore SRAM OE state
	popw	R17
	pop	R16
	ret
	.endif	

;------------------------------------------------------------------------------------------------
;	Send command R16 to IDE device
;------------------------------------------------------------------------------------------------
IDEwrCom:	.if	STORA!=0
	rcall	IDEwaitRDY	;wait while !RDY
	push	R16
	ldi	R16,IDEcomReg
	rcall	IDEsetReg	;set command register
	pop	R16
	out	portc,R16	;put command on data port
	pushw	R17
	in	R17,pROE	;backup SRAM OE state
	sbi	pROE,ROE	;SRAM OE=1
	in	R18,ddrc	;backup data port state
	outi	ddrc,0xFF	;data port as output
	rcall	IDEiowr
	out	ddrc,R18	;restore data port state
	tst	R18
	brne	PC+1+2
	outi	portc,0xFF	;pull-ups if data port configured as input 
	out	pROE,R17	;restore SRAM OE state
	popw	R17
	ret
	.endif
	
;------------------------------------------------------------------------------------------------
;	Set register R16 with R17 value
;------------------------------------------------------------------------------------------------
IDEwrReg:	.if	STORA!=0
	rcall	IDEwaitRDY	;wait while !RDY
	rcall	IDEsetReg	;set register R16
	out	portc,R17	;put command on data port
	pushw	R17
	in	R17,pROE	;backup SRAM OE state
	sbi	pROE,ROE	;SRAM OE=1
	in	R18,ddrc	;backup data port state
	outi	ddrc,0xFF	;data port as output
	rcall	IDEiowr
	out	ddrc,R18	;restore data port state
	tst	R18
	brne	PC+1+2
	outi	portc,0xFF	;pull-ups if data port configured as input 
	out	pROE,R17	;restore SRAM OE state
	popw	R17	
	ret
	.endif
	

;------------------------------------------------------------------------------------------------
;	Read sector to Z pointed RAM
;------------------------------------------------------------------------------------------------
IDEreadSec:	.if	STORA!=0
	rcall	IDEwaitDRQ	;wait while !DRQ
	
	ldi	R16,IDEdataReg
	rcall	IDEsetReg	;set data register
	
	;16-bit data port as input with pull-ups
	BtInh	1
	in	R17,ddrc
	in	R18,ddra
	in	R19,pROE
	sbi	pROE,ROE
	ldi	R16,0x00
	out	ddrc,R16
	out	ddra,R16
	ldi	R16,0xFF
	out	portc,R16
	out	porta,R16
	
	;read 256 words in to Z pointed RAM
	pusht	R17
	ldi	R18,256
IDEreadSecL:	cbi	pIORD,IORD
	ldi	R16,2
	dbnz	R16,PC-1
	in	R16,pinc	;read byte
	in	R17,pina	;read byte
	sbi	pIORD,IORD
	stw	Z+,R16	;store it into RAM
	dbnz	R18,IDEreadSecL
	popt	R17
	
	;restore port configuration
	out	pROE,R19
	out	ddra,R18
	tst	R18
	brne	PC+1+2
	outi	porta,0xFF
	out	ddrc,R17
	brne	PC+1+2
	outi	portc,0xFF
	BtInh	0
	ret
	.endif

;------------------------------------------------------------------------------------------------
;	Generate IOWR pulse
;------------------------------------------------------------------------------------------------	
IDEiowr:	.if	STORA!=0
	cbi	pIOWR,IOWR
	push	R16
	ldi	R16,3
	dbnz	R16,PC-1
	pop	R16
	sbi	pIOWR,IOWR
	ret	
	.endif

;------------------------------------------------------------------------------------------------
;	Read byte to R16
;------------------------------------------------------------------------------------------------	
IDEiord:	.if	STORA!=0
	cbi	pIORD,IORD
	push	R16
	ldi	R16,3
	dbnz	R16,PC-1
	pop	R16
	in	R16,pinc	;read byte
	sbi	pIORD,IORD
	ret
	.endif
	
;------------------------------------------------------------------------------------------------
;	Read word to R16:R17
;------------------------------------------------------------------------------------------------
IDEiordW:	.if	STORA!=0
	cbi	pIORD,IORD
	push	R16
	ldi	R16,3
	dbnz	R16,PC-1
	pop	R16
	in	R16,pinc	;read byte
	in	R17,pina	;read byte
	sbi	pIORD,IORD
	ret
	.endif
	
;------------------------------------------------------------------------------------------------
;	Initialize IDE device
;------------------------------------------------------------------------------------------------
IDEinit:	.if	STORA!=0
	.if	LCDON!=0
	rcall	LCDclrAll
	LwrFS	TWDInit	;print waiting IDE
	.endif

	;wait for device ready
	ToTime	350	;timeout ~3.5s
	ToRes		;reset timeout timer
	ToRun		;start timeout timer
IDEinitRDYA:	lds	R16,TimOut
	tst	R16
	breq	IDEinitRDYTO
	;timeout - probably no device conected
	ToStop		;stop timeout timer
	;write init failed
	.if	LCDON!=0
	rcall	LCDclrL1
	LwrFS	TDIFail
	ldi	R16,150
	call	Delay10ms
	.endif
	clt		;T=0 - initialization failed
	ret		;leave
IDEinitRDYTO:	rcall	IDEtestRDY	;test for RDY
	brtc	IDEinitRDYA	;test again if !RDY yet
	;RDY - some device conected
		
	;identify drive
	ldi	R16,IDEcomIdent
	rcall	IDEwrCom	;identify command
	ldiw	R30,DBuff
	rcall	IDEreadSec	;read identify sector into DBuff
	
	;word swap (little-endian => big-endian)
	ldiw	R30,DBuff
	ldi	R24,256
IDEinitWSL:	lddw	R16,Z
	st	Z+,R17
	st	Z+,R16
	dbnz	R24,IDEinitWSL
	
	;set dvive parameters
	ldi	R16,IDEscReg	;sector count register
	lds	R17,DBuff+12+1	;sectors per track
	rcall	IDEwrReg	;sector/track parameter
	ldi	R16,IDEsdhReg	;drive/head register
	lds	R17,DBuff+6+1	;heads
	dec	R17	;heads-1
	ori	R17,0b10000000
	rcall	IDEwrReg	;heads parameter
	ldi	R16,IDEcomSetPar
	rcall	IDEwrCom	;set drive parameter command
		
	;write device name to 1. line
	.if	LCDON!=0
	rcall	LCDclrL1
	ldiw	R30,DBuff+54
	ldi	R17,20
	rcall	LCDwrStrRam
	
	ldi	R16,0x40|128
	rcall	LCDwrInstr	;second line
	;write size in MiB to 2. line
	ldiw	R30,DBuff+120
	ldd	R16,Z+1
	ldd	R17,Z+0
	ldd	R18,Z+3
	ldd	R19,Z+2	;sector count
	stst	IDEsize,R16	;store it
	cpiwh	R18,R20,16
	brsh	IDEinitWSM
	;lower than 524288kiB
	lsrd	R16
	rcall	LCDwrBCD8sz	;write size
	LwrFS	TUkB	;write "kB"
	rjmp	IDEinitWSK
	;same or higher than 524288kiB
IDEinitWSM:	lsrt	R17
	lsrt	R17
	lsrt	R17
	mov	R16,R17
	mov	R17,R18
	mov	R18,R19
	rcall	LCDwrBCD8sz	;write size
	LwrFS	TUMB	;write "MB"
IDEinitWSK:	ldi	R16,120
	call	Delay10ms	;wait
	.endif
	set		;success
	ret
	.endif

;------------------------------------------------------------------------------------------------
;	Set LBA28 address
;------------------------------------------------------------------------------------------------
IDEsetLBA:	.if	STORA!=0
	pusht	R17
	mov	R17,R16
	ldi	R16,IDEsnReg
	rcall	IDEwrReg	;set A0-A7
	pop	R17
	ori	R17,0b11000000	;CRC, LBA mode, master/single
	ldi	R16,IDEsdhReg
	rcall	IDEwrReg	;set A24-A27
	pop	R17
	ldi	R16,IDEchReg
	rcall	IDEwrReg	;set A16-A23
	pop	R17
	ldi	R16,IDEclReg
	rcall	IDEwrReg	;set A8-A15
	ldi	R16,IDEscReg
	ldi	R17,1
	rcall	IDEwrReg	;1 sector block
	ret
	.endif

;------------------------------------------------------------------------------------------------
;	Read single sector form LBA R16:R17:R18:R19 to Z pointed RAM
;------------------------------------------------------------------------------------------------	
IDErdSecLBA:	.if	STORA!=0
	pushd	R16	;push LBA
	rcall	IDEsetLBA	;set LBA address, 1 sector block
	ldi	R16,IDEcomRdSec
	rcall	IDEwrCom	;read sector command
	rcall	IDEreadSec	;read sector into Z pointed RAM
	popd	R16	;pop LBA
	subid	R16,-1	;LBA++
	ret
	.endif	
	


;************************************************************************************************
;	FAT routines
;************************************************************************************************	
	.if	STORA!=0
	.dseg
	.equ	FATclst=DBuff+512	;upto 256 cluster string overlapped with DBuff 	
FATsize:	.byte	4	;partiotion size in sectors
FATspc:	.byte	1	;sectors per cluster
FATftc:	.byte	1	;FAT tables
FATfsz:	.byte	2	;FAT table size in sectors
FATrsz:	.byte	2	;root sectors
FATfofs:	.byte	4	;1. FAT offset
FATrofs:	.byte	4	;root offset
FATdofs:	.byte	4	;data offset 
FATrfldc:	.byte	1	;root folder count
FATrfld:	.byte	1	;root folder
FATrflds:	.byte	1	;root folder sorted id 
FATflec:	.byte	1	;file count in folder
FATfle:	.byte	1	;file 
FATsrt:	.byte	256	;sort indexes buffer
	.cseg
	.endif

	;MBR file system idetifiers 
	.equ	FATfsf16l32M=0x04	;FAT16 upto 32MiB
	.equ	FATfsf16m32M=0x06	;FAT16 over 32MiB
	
;------------------------------------------------------------------------------------------------
;	Init FAT file system from primary partition
;------------------------------------------------------------------------------------------------
FATinit:	.if	STORA!=0
	ldid	R16,0x00000000
	ldiw	R30,DBuff
	rcall	IDErdSecLBA	;0. sector (MBR)
	ldsw	R16,DBuff+0x01FE
	cpiwh	R16,R18,0xAA55	;MBR executable mark?
	breq	FATinitMEM
	;missing MBR executable mark - failure
	rcall	LCDclrAll
	LwrFS	TFTnotMbr
	ldi	R16,150
	call	Delay10ms
	clt
	ret		;leave
FATinitMEM:	;MBR OK
	ldiw	R30,DBuff+0x01BE	;1. partition record
	ldd	R22,Z+4	;file system type
	cpi	R22,FATfsf16l32M
	breq	FATinitF16
	cpi	R22,FATfsf16m32M
	breq	FATinitF16
	;not a FAT16
	rcall	LCDclrAll
	LwrFS	TFTnotFAT16
	ldi	R16,150
	call	Delay10ms
	clt
	ret		;leave
	;FAT16 file system
FATinitF16:	lddd	R16,Z+8
	movw	R2,R16
	movw	R4,R18	;store BR offset
	ldiw	R30,DBuff
	rcall	IDErdSecLBA	;read first partition sector (boot record)
	ldsw	R16,DBuff+0x01FE	;BR executable mark?
	cpiwh	R16,R18,0xAA55
	breq	FATinitBRO
	;invalid boot record
	rcall	LCDclrAll
	LwrFS	TFTnotFBR
	ldi	R16,150
	call	Delay10ms
	clt
	ret		;leave
FATinitBRO:	;valid boot record		
	lds	R16,DBuff+0x000D	;sectors per cluster
	sts	FATspc,R16	;store it
	lds	R16,DBuff+0x0010	;FAT tables
	sts	FATftc,R16	;store it
	ldsw	R16,DBuff+0x0016	;FAT size
	stsw	FATfsz,R16	;store it
	ldsw	R16,DBuff+0x0011	;root entries
	swap	R16
	andi	R16,0x0F
	swap	R17
	mov	R18,R17
	andi	R17,0x0F
	andi	R18,0xF0
	or	R16,R18	;root_sectors=root_entries/16
	stsw	FATrsz,R16	;store it
	;now calculate offsets
	ldsw	R20,DBuff+0x000E
	clr	R6	;0
	addw	R2,R20
	adc	R4,R6
	adc	R5,R6	;add 1.FAT offset
	stsd	FATfofs,R2	;store 1.FAT offset
	lds	R20,FATftc	;load FAT count
	ldsw	R21,FATfsz	;load FAT size
	mul	R20,R21
	addw	R2,R0
	adc	R4,R6
	adc	R5,R6
	mul	R20,R22
	addw	R3,R0
	adc	R5,R6	;add FAT_size*FAT_count	
	stsd	FATrofs,R2	;store root offset
	ldsw	R20,FATrsz	;load root size
	addw	R2,R20
	adc	R4,R6
	adc	R5,R6	;add root_size
	stsd	FATdofs,R2	;store data offset
	ret
	.endif

;------------------------------------------------------------------------------------------------
;	Get R16:R17 cluster LBA (R16:R17:R18:R19)
;------------------------------------------------------------------------------------------------
FATgetClusLBA:	.if	STORA!=0
	movw	R20,R16
	subiw	R20,2	;-=2
	lds	R22,FATspc	;sectors per cluster
	ldsd	R16,FATdofs	;data offset
	clr	R2
	mul	R20,R22
	addw	R16,R0
	adc	R18,R2
	adc	R19,R2
	mul	R21,R22
	addw	R17,R0
	adc	R19,R2	;LBA=data_offset+(clus_id-2)*sec_per_clus
	ret
	.endif

;------------------------------------------------------------------------------------------------
;	Get cluster string from R16:R17 cluster
;	FATclst - cluster string destination buffer
;	R25 - cluster string len
;	    - R25=<1;255> means entire string read
;	    - R25=256 (=0 in 8-bit) means just part of string read - process 
;	      !255! clusters and call again with R16:R17=FATclst[255*2]  
;------------------------------------------------------------------------------------------------	
FATgetCluss:	.if	STORA!=0
	movw	R2,R16	;actual cluster id
	clr	R0	;0
	ldiw	R28,FATclst	;cluster string buffer (max 256 clusters)
	stw	Y+,R2	;store first cluster
	ldi	R25,1	;cluster string len (1 now) 
 	
	;get R3 FAT sector
FATgetClussOS:	ldsd	R16,FATfofs
	add	R16,R3
	adc	R17,R0
	adc	R18,R0
	adc	R19,R0	;cluster sector
	ldiw	R30,DBuff
	rcall	IDErdSecLBA	;read sector
	;get R2 item from FAT sector
FATgetClussSS:	mov	R30,R2
	clr	R31
	lslw	R30
	subiw	R30,-DBuff	;actual cluster pointer
	ldw	R16,Z+	;load its value - next cluster id
	cpiwh	R16,R18,0xFFF0	;>=0xFFF0 - end cluster
	brsh	FATgetClussD	;final - done (cluster string 0<len<256)
	;not final
	stw	Y+,R16	;store it
	inc	R25	;cluster string len++	
	cpi	R25,256	;max cluster count?
	breq	FATgetClussD	;yeah - done for this time (cluster string len=256 (=0 in 8-bit))
			;but not entire cluster string read!
	;not final and cluster string buffer not full - find next
	mov	R18,R3	;last sector id
	movw	R2,R16	;new cluster id
	cp	R18,R3	;new cluster lies in the same sector?  
	breq	FATgetClussSS	;yeah - take a shortcut
	;no - it lies in another sector - read that sector
	rjmp	FATgetClussOS
	;done
FATgetClussD:	ret
	.endif
	



;------------------------------------------------------------------------------------------------
;	Scan root folder/get folder entry
;	- scan root mode:
;	   - inputs: R10=0, R2:R3:R4=SRAM address to store folder names
;	   - outputs: R20=found folder count (upto 256)
;	- get folder mode:
;	   - inputs: R10!=0, R11=folder to find id (0-255)
;	   - outputs: Z=folder entry pointer 
;------------------------------------------------------------------------------------------------
FATscanRFldC:	.if	STORA!=0
	clr	R8
	inc	R8	;1
	clr	R9	;0
	ldsd	R12,FATrofs	;root LBA
	ldsw	R26,FATrsz	;root size
	ldi	R20,0	;folder count
	
	;for each root sector
FATscanRFldCNS:	movw	R16,R12
	movw	R18,R14	;load LBA
	ldiw	R30,DBuff
	rcall	IDErdSecLBA	;read root sector, LBA++
	movw	R12,R16
	movw	R14,R18	;store LBA

	ldiw	R30,DBuff	;sector data pointer
	ldi	R24,16	;16 entries per sector
	;for each entry from sector
FATscanRFldCNE:	ldd	R16,Z+0x00
	cpi	R16,0x00	;empty?
	breq	FATscanRFldCSE	;not folded - skip entry
	cpi	R16,0xE5	;deleted?
	breq	FATscanRFldCSE	;not folded - skip entry
	ldd	R16,Z+0x0B	;attribute
	cpi	R16,0x0F	;LFN?
	breq	FATscanRFldCSE	;not folder - leave
	bst	R16,3	;disk label?
	brts	FATscanRFldCSE	;not folder - leave
	bst	R16,4	;folder?
	brtc	FATscanRFldCSE	;not folder - leave
	;valid folder entry
	tst	R10	;if R10==0 skip folder finder 	
	brne	PC+1+1+1
	;scan mode - store name
	rcall	FATstName	;store folder name
	rjmp	PC+1+1+1
	cp	R20,R11	;selected folder reached?
	breq	FATscanRFldCMFC	;yeah R11==R20 - done
	inc	R20	;folder count++
	breq	FATscanRFldCMFC	;max folder count - done
FATscanRFldCSE:	adiw	R30,32	;to next entry
	dbnz	R24,FATscanRFldCNE
	dbnzwh	R26,FATscanRFldCNS
	;done
FATscanRFldCMFC:	ret
	.endif

;------------------------------------------------------------------------------------------------
;	Store 8-char name into SRAM addr++
;------------------------------------------------------------------------------------------------
FATstName:	.if	STORA!=0
	BtInh	1
	ldi	R17,8
FATstNameL:	ld	R16,Z+
	out	portc,R16
	out	porta,R4
	cbi	pRHA,RHA
	sbi	pRHA,RHA
	out	porta,R3
	cbi	pRMA,RMA
	sbi	pRMA,RMA
	out	porta,R2
	addw	R2,R8
	adc	R4,R9	;SRAM addr++
	cbi	pRWR,RWR
	nop
	sbi	pRWR,RWR
	dbnz	R17,FATstNameL
	sbiw	R30,8	;return name pointer back
	BtInh	0
	ret
	.endif
	

;------------------------------------------------------------------------------------------------
;	Scan root for folders
;------------------------------------------------------------------------------------------------
FATscanRFld:	.if	STORA!=0
	clrt	R2	;SRAM address
	clr	R10	;scan mode
	in	R16,pROE
	sbi	pROE,ROE
	in	R17,ddra
	in	R18,ddrc
	pusht	R16
	ldi	R16,0xFF
	out	ddra,R16
	out	ddrc,R16	;outputs
	rcall	FATscanRFldC	;scan root
	popt	R16
	out	ddrc,R18
	out	ddra,R17
	out	pROE,R16
	sts	FATrfldc,R20	;store folders count
	ret
	.endif
	
;------------------------------------------------------------------------------------------------
;	Z = R16 root folder entry 
;------------------------------------------------------------------------------------------------
FATgetRFldE:	.if	STORA!=0
	mov	R11,R16	;folder id in root
	clr	R10
	inc	R10	;get entry mode
	rcall	FATscanRFldC	;find folder in root
	ret
	.endif
	
;------------------------------------------------------------------------------------------------
;	Scan folder R16 for files
;------------------------------------------------------------------------------------------------	
FATscanFld:	.if	STORA!=0
	clr	R0
	add	R30,R16
	adc	R31,R0
	ld	R16,Z	;unsorted folder id to sorted one

	rcall	FATgetRFldE	;get folder entry pointer
	
	;print folder name to line 1
	rcall	LCDposL1
	movw	R14,R30
	ldi	R17,8
	rcall	LCDwrStrRam
	ldi	R17,20-8
	ldi	R16,' '
	rcall	LCDwrData
	dbnz	R17,PC-1-1
	movw	R30,R14
		
	stsi	FATflec,0	;0 files found yet
	lddw	R16,Z+0x1A	;first folder cluster id
	
	in	R18,pROE
	sbi	pROE,ROE
	in	R19,ddra
	in	R20,ddrc
	pusht	R18
	ldi	R18,0xFF
	out	ddra,R18
	out	ddrc,R18	;ports/signals config
	
	clrt	R2	;SRAM addr
	clr	R8
	inc	R8	;1
	clr	R9	;0
	
FATscanFldNCS:	pushw	R2
	rcall	FATgetCluss	;get some clusters for this folder
	popw	R2
	mov	R23,R25	;store cluster string len
	cpi	R25,256
	brne	PC+1+1
	ldi	R25,255	;if not all clusters in buffer process only 255 of 256 (256. is used as initial for next part)
	ldiw	R28,FATclst	;cluster buffer
	;get cluster form cluster string
FATscanFldNC:	ldw	R16,Y+	;load cluster id
	push	R2
	rcall	FATgetClusLBA	;get first sector LBA of this (R16:R17) cluster
	pop	R2
	lds	R26,FATspc	;sectors in cluster
	;get sector from cluster
FATscanFldNS:	ldiw	R30,DBuff
	rcall	IDErdSecLBA	;read sector
	movw	R12,R16
	movw	R14,R18	;backup LBA
	ldiw	R30,DBuff	;sector data pointer
	ldi	R24,16	;16 entries per sector
	;get entry from sector
FATscanFldNE:	ldd	R16,Z+0x00
	cpi	R16,0x00	;empty?
	breq	FATscanFldNF	;not file - skip entry
	cpi	R16,0xE5	;deleted?
	breq	FATscanFldNF	;not file - skip entry
	ldd	R16,Z+0x0B	;load attribute
	cpi	R16,0x0F	;LFN?
	breq	FATscanFldNF	;not file - skip entry
	andi	R16,1<<4|1<<3	;folder or volume?
	brne	FATscanFldNF	;not file - skip entry
	;valid file entry
	lddt	R16,Z+0x08	;extension	
	rcall	FATextIsMD	;check for valid extension
	brtc	FATscanFldNF	;invalid - skip entry
	;should be module file
	rcall	FATstName
	incs	R16,FATflec	;new file found
	cpi	R16,255
	breq	FATscanFldAD	;done if max files count reached! 
FATscanFldNF:	adiw	R30,32	;to next entry
	dbnz	R24,FATscanFldNE
	;next sector in cluster
	movw	R16,R12
	movw	R18,R14	;restore LBA
	dbnz	R26,FATscanFldNS	;next sector in cluster
	;to next cluster in cluster string
	dbnz	R25,FATscanFldNC	;next cluster
	;to next part of cluster string?
	cpi	R23,256	;all clusters played?
	brne	FATscanFldAD	;yeah - done
	;no - process next part of cluster string
	ldw	R16,Y+	;set last (unprocessed!) cluster as initial for next cluster string part
	rjmp	FATscanFldNCS 
	;done (hopefully)
FATscanFldAD:	
	popt	R16
	out	ddrc,R18
	out	ddra,R17
	out	pROE,R16
	ret
	.endif

	;valid extension?
FATextIsMD:	.if	STORA!=0
	clt
	cpi	R16,'M'
	brne	FATextIsMDn	;not "MOD" or "MD3"
	cpi	R17,'O'
	ldi	R16,'D'
	cpc	R18,R16
	breq	FATextIsMDy	;is "MOD"
	cpi	R17,'D'
	ldi	R16,'3'
	cpc	R18,R16
	brne	FATextIsMDn	;is not "MD3"
FATextIsMDy:	set
FATextIsMDn:	ret
	.endif

;------------------------------------------------------------------------------------------------
;	Get R17 file from R16 folder
;------------------------------------------------------------------------------------------------	
FATgetFleE:	.if	STORA!=0
	mov	R15,R17	;file to fined id
	push	R15
	rcall	FATgetRFldE	;get folder entry pointer
	pop	R15
	clr	R14	;0. file
	lddw	R16,Z+0x1A	;first folder cluster id
		
	
FATgetFleNCS:	rcall	FATgetCluss	;get some clusters for this folder
	mov	R23,R25	;store cluster string len
	cpi	R25,256
	brne	PC+1+1
	ldi	R25,255	;if not all clusters in buffer process only 255 of 256 (256. is used as initial for next part)
	ldiw	R28,FATclst	;cluster buffer
	;get cluster form cluster string
FATgetFleNC:	ldw	R16,Y+	;load cluster id
	rcall	FATgetClusLBA	;get first sector LBA of this (R16:R17) cluster
	lds	R26,FATspc	;sectors in cluster
	;get sector from cluster
FATgetFleNS:	ldiw	R30,DBuff
	rcall	IDErdSecLBA	;read sector
	movw	R2,R16
	movw	R4,R18	;backup LBA
	ldiw	R30,DBuff	;sector data pointer
	ldi	R24,16	;16 entries per sector
	;get entry from sector
FATgetFleNE:	ldd	R16,Z+0x00
	cpi	R16,0x00
	breq	FATgetFleNF	;not file - skip entry
	cpi	R16,0xE5	;deleted?
	breq	FATgetFleNF	;not file - skip entry
	ldd	R16,Z+0x0B	;load attribute
	cpi	R16,0x0F	;LFN?
	breq	FATgetFleNF	;not file - skip entry
	andi	R16,1<<4|1<<3	;folder or volume?
	brne	FATgetFleNF	;not file - skip entry
	;valid file entry
	lddt	R16,Z+0x08	;extension	
	rcall	FATextIsMD	;check for valid extension
	brtc	FATgetFleNF	;invalid - skip entry
	;should be module file
	cp	R14,R15
	breq	FATgetFleAD	;compare match - done
	inc	R14
	 
FATgetFleNF:	adiw	R30,32	;to next entry
	dbnz	R24,FATgetFleNE
	;next sector in cluster
	movw	R16,R2
	movw	R18,R4	;restore LBA
	dbnz	R26,FATgetFleNS	;next sector in cluster
	;to next cluster in cluster string
	dbnz	R25,FATgetFleNC	;next cluster
	;to next part of cluster string?
	cpi	R23,256	;all clusters played?
	brne	FATgetFleADF	;yeah - done
	;no - process next part of cluster string
	ldw	R16,Y+	;set last (unprocessed!) cluster as initial for next cluster string part
	rjmp	FATgetFleNCS
	;something has fucked up!
FATgetFleADF:	clt
	ret 
	;done (hopefully)
FATgetFleAD:	set
	ret
	.endif



;------------------------------------------------------------------------------------------------
;	Bubble sort entry names into Z buffer (sorted indexes), R25=entry names in SRAM
;------------------------------------------------------------------------------------------------
FATsortNLst:	.if	STORA!=0
	BtInh	1
	in	R16,pROE
	sbi	pROE,ROE
	in	R17,ddra
	in	R18,ddrc
	pusht	R16

	;fill buffer with unsorted indexes 
	pushw	R30
	ldi	R16,0
	mov	R24,R25
FATsortNLstII:	st	Z+,R16
	inc	R16
	dbnz	R24,FATsortNLstII
	popw	R30
	
	outi	ddra,0xFF	;address port as output
	outi	ddrc,0x00	;data port as input
	cbi	pROE,ROE	;SRAM OE=0
	
	
	;bubble sort
	cpi	R25,2
	brlo	FATsortNLstSS	;skip sorting if string_count<2
	;main sort
	push	R25	;push string count
	clr	R4	;SRAM high address (always 0)
	clrt	R5
	inc	R5	;SRAM addressing step (+1)
	ldi	R23,8	;8
FATsortNLstSA:	
	ldi	R19,0	;no bubble swap yet
	dec	R25
	mov	R24,R25
	cpi	R24,2
	brlo	FATsortNLstSD	;exit sorting if less than 2 items
	pushw	R30
FATsortNLstBS:	lddw	R20,Z	;get A and B string ids
	
	;compare strings
	mul	R20,R23
	movw	R8,R0	;R8:R9 = A string pointer 
	mul	R21,R23
	movw	R10,R0	;R10:R11 = B string pointer
	ldi	R22,8
FATsortNLstC1:	movw	R2,R8
	call	RamGetByteA
	mov	R17,R16	;string A char
	movw	R8,R2
	movw	R2,R10
	call	RamGetByteA	;string B char
	movw	R10,R2
	
	;compare A and B char
	cp	R17,R16
	breq	PC+1+1+1+1+1+1+1	;skip if A==B
	brlo	PC+1+1+1+1+1
	;A>B
	mov	R16,R20
	mov	R20,R21
	mov	R21,R16	;swap A and B string ids
	ldi	R19,1	;bubble swap
	rjmp	FATsortNLstCD	;A!=B - comparation done	
	;next char?
	dbnz	R22,FATsortNLstC1
	;strings compared
FATsortNLstCD:	stdw	Z,R20	;store A and B string ids
	adiw	R30,1	;next string pair
	dbnz	R24,FATsortNLstBS
	popw	R30
		
	cpi	R19,1	;some swap?
	breq	FATsortNLstSA	;yeah - again
FATsortNLstSD:	pop	R25	;pop string count
FATsortNLstSS:
	



;	pushw	R30
;	outi	ddra,0xFF
;	cbi	pROE,ROE
;	clrt	R5
;	inc	R5	;step
;	clr	R4
;	ldi	R23,8
;	;lds	R24,FATflec
;	mov	R24,R25
;FATsortNLstPS:	ld	R2,Z+
;	mul	R2,R23
;	movw	R2,R0
;	ldi	R25,8
;FATsortNLstPSN:	rcall	RamGetByteA
;	rcall	UsTxChar
;	dbnz	R25,FATsortNLstPSN
;	dbnz	R24,FATsortNLstPS
;	popw	R30

;	pushw	R30
;	pushw	R24
;	pushw	R17
;	mov	R24,R25
;	ld	R16,Z+
;	rcall	UsTxChar
;	dbnz	R24,PC-1-1-1
;	popw	R17
;	popw	R24
;	popw	R30
	

	popt	R16
	out	ddrc,R18
	out	ddra,R17
	out	pROE,R16
	BtInh	0
	ret
	.endif
	

;------------------------------------------------------------------------------------------------
;	Read file R17 from R16 folder into SRAM, Z = sorted indexes, R18 - only file name print
;------------------------------------------------------------------------------------------------	
FATrdFle:	.if	STORA!=0
	in	R19,pROE
	push	R19
	sbi	pROE,ROE
	in	R19,ddra
	push	R19
	in	R19,ddrc
	push	R19	;backup ports/signals state
	
	push	R18	;store parameter
		
	clr	R0
	add	R30,R17
	adc	R31,R0
	ld	R17,Z	;convert unsorted file id to sorted one
	
	rcall	FATgetFleE	;get file entry
	
	;print file name and extension
	movw	R14,R30
	rcall	LCDposL1
	ldi	R20,20
	ldi	R18,8
	ld	R16,Z+
	cpi	R16,' '
	breq	PC+1+1+1+1+1
	rcall	LCDwrData	;print non-blank chars only
	dec	R20
	dbnz	R18,PC-1-1-1-1-1-1
	movw	R30,R14
	adiw	R30,8
	ld	R16,Z
	cpi	R16,' '
	breq	FATrdFleFNPNE	;skip if no extension there
	ldi	R16,'.'
	rcall	LCDwrData	;prn separator
	ldi	R17,3
	rcall	LCDwrStrRam	;print full extension
	subi	R20,4
FATrdFleFNPNE:	ldi	R16,' '
	rcall	LCDwrData	;clear rest of line
	dbnz	R20,PC-1-1
	movw	R30,R14
	
	
	pop	R18
	tst	R18
	breq	FATrdFleNNM
	;print MOD title only
	lddw	R16,Z+0x1A	;load initial cluster id
	rcall	FATgetClusLBA	;get cluster LBA
	ldiw	R30,DBuff
	rcall	IDErdSecLBA	;read first sector
	ldiw	R28,DBuff
	call	MODprnName	;print MOD title 
	rjmp	FATrdFleADF	;da end
FATrdFleNNM:	
		
	
	lddd	R16,Z+0x1C	;load len in bytes
			
	lsrd	R16	;convert to sectors (R17:R18:R19 - full sector count, c:R16 - 9-bit rest)
	brcs	FATrdFleR	;non zero rest
	tst	R16
	brne	FATrdFleR	;non zero rest
	rjmp	PC+1+3	;zero rest
	;non zero rest in bytes
FATrdFleR:	subit	R17,-1	;+1 sector
	;check file len
	clr	R20
	lds	R21,SRAMsize
	clr	R22
	lsrw	R20	;SRAM size in sectors 
	cpt	R17,R20
	brlo	PC+1+2
	mov	R17,R20
	mov	R18,R21	;cut off file if bigger than SRAM
	mov	R6,R17
	mov	R7,R18	;R6:R7 - sectors to read
	
	
	;calc progress bar step
	ldiw	R16,20<<11
	movw	R18,R6
	call	div16u
	movw	R12,R16
	clr	R14
	rcall	LCDclrL2
	
		
	;SRAM addressing
	clrt	R2	;SRAM address (0)
	
	lddw	R16,Z+0x1A	;load initial cluster id
	;for each part of cluster string
FATrdFleNCS:	pushw	R2
	rcall	FATgetCluss	;get some clusters
	popw	R2
	mov	R23,R25	;backup cluster string len
	cpi	R25,256
	brne	PC+1+1
	ldi	R25,255	;process only 255 of 256 clusters if not entire clus. str. read (R25==256)
	ldiw	R28,FATclst	;cluster string buffer
	;for each cluster from cluster string 
FATrdFleNC:	ldw	R16,Y+	;load cluster id from cluster string
		
	push	R2
	rcall	FATgetClusLBA	;get cluster LBA (1. sector)
	pop	R2
	lds	R24,FATspc	;sectors per cluster
	;for each sector in cluster
FATrdFleNS:	ldiw	R30,DBuff
	rcall	IDErdSecLBA	;read sector, LBA++
		
	clr	R8	;0
	clr	R9
	inc	R9	;1
	ldi	R20,0xFF
	out	ddra,R20	;address port as output
	out	ddrc,R20	;data port as output
		
	
	;--- progress bar print ---
	pusht	R16
	mul	R6,R12
	movw	R16,R0
	mul	R7,R12
	add	R17,R0
	mul	R6,R13
	add	R17,R0
	lsr	R17
	lsr	R17
	lsr	R17	;calc position
	ldi	R16,20
	sub	R16,R17
	cp	R16,R14
	breq	FATrdFlePBS
	mov	R20,R16
	sub	R20,R14	;calc len of new bar fragment
	mov	R14,R16
	;print bar fragment
FATrdFlePBL:	ldi	R16,0xFF
	rcall	LCDwrData
	dbnz	R20,FATrdFlePBL
FATrdFlePBS:	popt	R16
	
	
	
	;write sector into SRAM
	BtInh	1	
	ldiw	R30,DBuff
	;
	out	porta,R4
	cbi	pRHA,RHA
	sbi	pRHA,RHA
	;
	ldi	R27,2
FATrdFleNHS:	out	porta,R3
	cbi	pRMA,RMA
	sbi	pRMA,RMA
	;
	ldi	R26,256	;256 words
FATrdFleNB:	ld	R20,Z+
	out	portc,R20
	out	porta,R2
	add	R2,R9	;addr++
	.if	HCOC==1
	nop
	.endif
	cbi	pRWR,RWR
	nop
	sbi	pRWR,RWR
	dbnz	R26,FATrdFleNB	;next byte into SRAM
	add	R3,R9
	adc	R4,R8	;SRAM addr++
	;
	dbnz	R27,FATrdFleNHS
	BtInh	0

	
	sub	R6,R9
	sbc	R7,R8	;cluster count --
	breq	FATrdFleADF	;all sectors in SRAM - done
		
	djnz	R24,FATrdFleNS	;next sector
	djnz	R25,FATrdFleNC	;next cluster
	;to next part of cluster string?
	cpi	R23,256	;all clusters played?
	brne	FATrdFleADF	;yeah - done
	;no - process next part of cluster string
	;ldsw	R16,FATclst+255*2	;set last (unprocessed!) cluster as initial for next cluster string part
	ldw	R16,Y+
	rjmp	FATrdFleNCS	;next part of cluster string

	;done
FATrdFleADF:	pop	R18
	out	ddrc,R18
	pop	R18
	out	ddra,R18	
	pop	R18
	out	pROE,R18	;restore ports/signals
	ret
	.endif
	
