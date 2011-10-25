;compiler: avrasm
;************************************************************************************************************************
; Some usefull macros
;	
; author: Stanislav Maslan, s.maslan@seznam.cz
;************************************************************************************************************************
;------------------------------------------------------------------------------------------------------------------------	
;	register aliases eg.:
;	R16H  = R17
;	R16B3 = R18
;	R16B4 = R19 
;------------------------------------------------------------------------------------------------------------------------
	.def	R0H=R1
	.def	R0B3=R2
	.def	R0B4=R3
	.def	R1H=R2
	.def	R1B3=R3
	.def	R1B4=R4
	.def	R2H=R3
	.def	R2B3=R4
	.def	R2B4=R5
	.def	R3H=R4
	.def	R3B3=R5
	.def	R3B4=R6
	.def	R4H=R5
	.def	R4B3=R6
	.def	R4B4=R7
	.def	R5H=R6
	.def	R5B3=R7
	.def	R5B4=R8
	.def	R6H=R7
	.def	R6B3=R8
	.def	R6B4=R9
	.def	R7H=R8
	.def	R7B3=R9
	.def	R7B4=R10
	.def	R8H=R9
	.def	R8B3=R10
	.def	R8B4=R11
	.def	R9H=R10
	.def	R9B3=R11
	.def	R9B4=R12
	.def	R10H=R11
	.def	R10B3=R12
	.def	R10B4=R13
	.def	R11H=R12
	.def	R11B3=R13
	.def	R11B4=R14
	.def	R12H=R13
	.def	R12B3=R14
	.def	R12B4=R15
	.def	R13H=R14
	.def	R13B3=R15
	.def	R13B4=R16
	.def	R14H=R15
	.def	R14B3=R16
	.def	R14B4=R17
	.def	R15H=R16
	.def	R15B3=R17
	.def	R15B4=R18
	.def	R16H=R17
	.def	R16B3=R18
	.def	R16B4=R19
	.def	R17H=R18
	.def	R17B3=R19
	.def	R17B4=R20
	.def	R18H=R19
	.def	R18B3=R20
	.def	R18B4=R21
	.def	R19H=R20
	.def	R19B3=R21
	.def	R19B4=R22
	.def	R20H=R21
	.def	R20B3=R22
	.def	R20B4=R23
	.def	R21H=R22
	.def	R21B3=R23
	.def	R21B4=R24
	.def	R22H=R23
	.def	R22B3=R24
	.def	R22B4=R25
	.def	R23H=R24
	.def	R23B3=R25
	.def	R23B4=R26
	.def	R24H=R25
	.def	R24B3=R26
	.def	R24B4=R27
	.def	R25H=R26
	.def	R25B3=R27
	.def	R25B4=R28
	.def	R26H=R27
	.def	R26B3=R28
	.def	R26B4=R29
	.def	R27H=R28
	.def	R27B3=R29
	.def	R27B4=R30
	.def	R28H=R29
	.def	R28B3=R30
	.def	R28B4=R31
	.def	R29H=R30
	.def	R29B3=R31
	.def	R30H=R31
	
;------------------------------------------------------------------------------------------------------------------------
;	ldi macros
;------------------------------------------------------------------------------------------------------------------------	
	.macro	ldiw
	ldi	@0,low(@1)
	ldi	@0H,high(@1)
	.endm
	
	.macro	ldit
	ldi	@0,low(@1)
	ldi	@0H,high(@1)
	ldi	@0B3,byte3(@1)
	.endm
	
	.macro	ldid
	ldi	@0,low(@1)
	ldi	@0H,high(@1)
	ldi	@0B3,byte3(@1)
	ldi	@0B4,byte4(@1)
	.endm
	
	.macro clrw
	clr	@0
	clr	@0H
	.endm
	
	.macro clrt
	clr	@0
	clr	@0H
	clr	@0B3
	.endm
	
	.macro clrd
	clr	@0
	clr	@0H
	clr	@0B3
	clr	@0B4
	.endm
;------------------------------------------------------------------------------------------------------------------------
;	add/subb macros
;------------------------------------------------------------------------------------------------------------------------	
	.macro addw
	add	@0,@1
	adc	@0H,@1H
	.endm
	
	.macro addt
	add	@0,@1
	adc	@0H,@1H
	adc	@0B3,@1B3
	.endm
	
	.macro	addd
	add	@0,@1
	adc	@0H,@1H
	adc	@0B3,@1B3
	adc	@0B4,@1B4
	.endm
	
	.macro adcw
	adc	@0,@1
	adc	@0H,@1H
	.endm
	
	.macro subw
	sub	@0,@1
	sbc	@0H,@1H
	.endm
	
	.macro subt
	sub	@0,@1
	sbc	@0H,@1H
	sbc	@0B3,@1B3
	.endm
	
	.macro	subd
	sub	@0,@1
	sbc	@0H,@1H
	sbc	@0B3,@1B3
	sbc	@0B4,@1B4
	.endm
	
	.macro	subiw
	subi	@0,low(@1)
	sbci	@0H,high(@1)
	.endm
	
	.macro	subit
	subi	@0,low(@1)
	sbci	@0H,high(@1)
	sbci	@0B3,byte3(@1)
	.endm
	
	.macro	subid
	subi	@0,low(@1)
	sbci	@0H,high(@1)
	sbci	@0B3,byte3(@1)
	sbci	@0B4,byte4(@1)
	.endm
	
	.macro	negw
	com	@0H
	neg	@0
	brne	PC+2
	inc	@0H
	.endm
	
;------------------------------------------------------------------------------------------------------------------------
;	inc/dec for internal sram
;------------------------------------------------------------------------------------------------------------------------	
	.macro	incs
	lds	@0,@1
	inc	@0
	sts	@1,@0
	.endm
	
	.macro	decs
	lds	@0,@1
	dec	@0
	sts	@1,@0
	.endm
	
;------------------------------------------------------------------------------------------------------------------------
;	push/pop macros
;------------------------------------------------------------------------------------------------------------------------
	.macro	pushw
	push	@0
	push	@0H
	.endm
	
	.macro	popw
	pop	@0H
	pop	@0
	.endm
	
	.macro	pusht
	push	@0
	push	@0H
	push	@0B3
	.endm
	
	.macro	popt
	pop	@0B3
	pop	@0H
	pop	@0
	.endm
	
	.macro	pushd
	push	@0
	push	@0H
	push	@0B3
	push	@0B4
	.endm
	
	.macro	popd
	pop	@0B4
	pop	@0B3
	pop	@0H
	pop	@0
	.endm
	
;------------------------------------------------------------------------------------------------------------------------
;	load/store direct macros
;------------------------------------------------------------------------------------------------------------------------
	.macro	stsw
	sts	@0+1,@1H
	sts	@0+0,@1
	.endm
	
	.macro	ldsw
	lds	@0,@1+0
	lds	@0H,@1+1
	.endm
	
	.macro	stst
	sts	@0+2,@1B3
	sts	@0+1,@1H
	sts	@0+0,@1
	.endm
	
	.macro	ldst
	lds	@0,@1+0
	lds	@0H,@1+1
	lds	@0B3,@1+2
	.endm
	
	.macro	stsd
	sts	@0+3,@1B4
	sts	@0+2,@1B3
	sts	@0+1,@1H
	sts	@0+0,@1
	.endm
	
	.macro	ldsd
	lds	@0,@1+0
	lds	@0H,@1+1
	lds	@0B3,@1+2
	lds	@0B4,@1+3
	.endm
		
	.macro	stsi
	ldi	R16,@1
	sts	@0,R16
	.endm
	
	.macro stsiw
	ldi	R16,high(@1)
	sts	@0+1,R16
	ldi	R16,low(@1)
	sts	@0+0,R16
	.endm
	
	.macro stsit
	ldi	R16,byte3(@1)
	sts	@0+2,R16
	ldi	R16,high(@1)
	sts	@0+1,R16
	ldi	R16,low(@1)
	sts	@0+0,R16
	.endm
	
	.macro stsid
	ldi	R16,byte4(@1)
	sts	@0+3,R16
	ldi	R16,byte3(@1)
	sts	@0+2,R16
	ldi	R16,high(@1)
	sts	@0+1,R16
	ldi	R16,low(@1)
	sts	@0+0,R16
	.endm
	
	.macro	clrs
	clr	@1
	sts	@0,@1
	.endm
	
	.macro	clrsw
	clr	@1
	sts	@0+1,@1
	sts	@0+0,@1
	.endm
	
	.macro	clrst
	clr	@1
	sts	@0+2,@1
	sts	@0+1,@1
	sts	@0+0,@1
	.endm
	
	.macro	clrsd
	clr	@1
	sts	@0+3,@1
	sts	@0+2,@1
	sts	@0+1,@1
	sts	@0+0,@1
	.endm
	
;------------------------------------------------------------------------------------------------------------------------
;	in/out macros
;------------------------------------------------------------------------------------------------------------------------
	.macro	outw
	out	@0H,@1H
	out	@0L,@1
	.endm
	
	.macro	inw
	in	@0,@1L
	in	@0H,@1H
	.endm
	
	.macro	outi
	ldi	R16,@1
	out	@0,R16
	.endm
	
	.macro	outiw
	ldi	R16,high(@1)
	out	@0H,R16
	ldi	R16,low(@1)
	out	@0L,R16
	.endm
	
;------------------------------------------------------------------------------------------------------------------------
;	load/store indirect macros
;------------------------------------------------------------------------------------------------------------------------
	.macro	stw
	st	@0,@1
	st	@0,@1H
	.endm
	
	.macro	sttb
	st	@0,@1
	st	@0,@1H
	st	@0,@1B3
	.endm
	
	.macro	stqb
	st	@0,@1
	st	@0,@1H
	st	@0,@1B3
	st	@0,@1B4
	.endm
		
	.macro	ldw
	ld	@0,@1
	ld	@0H,@1
	.endm
	
	.macro	ldtb
	ld	@0,@1
	ld	@0H,@1
	ld	@0B3,@1
	.endm
	
	.macro	ldqb
	ld	@0,@1
	ld	@0H,@1
	ld	@0B3,@1
	ld	@0B4,@1
	.endm
	
	.macro	lpmw
	lpm	@0,@1
	lpm	@0H,@1
	.endm
	
	.macro	lpmt
	lpm	@0,@1
	lpm	@0H,@1
	lpm	@0B3,@1
	.endm
	
	.macro	lpmd
	lpm	@0,@1
	lpm	@0H,@1
	lpm	@0B3,@1
	lpm	@0B4,@1
	.endm
	
	.macro	lddw
	ldd	@0,@1+0
	ldd	@0H,@1+1
	.endm
	
	.macro	lddt
	ldd	@0,@1+0
	ldd	@0H,@1+1
	ldd	@0B3,@1+2
	.endm
	
	.macro	lddd
	ldd	@0,@1+0
	ldd	@0H,@1+1
	ldd	@0B3,@1+2
	ldd	@0B4,@1+3
	.endm
	
	.macro stdw
	std	@0+0,@1
	std	@0+1,@1H
	.endm
	
	.macro stdt
	std	@0+0,@1
	std	@0+1,@1H
	std	@0+2,@1B3
	.endm
	
	.macro stdd
	std	@0+0,@1
	std	@0+1,@1H
	std	@0+2,@1B3
	std	@0+3,@1B4
	.endm

;------------------------------------------------------------------------------------------------------------------------
;	bit rotation routines
;------------------------------------------------------------------------------------------------------------------------
	.macro	rolw
	rol	@0
	rol	@0H
	.endm
	
	.macro	rolt
	rol	@0
	rol	@0H
	rol	@0B3
	.endm
	
	.macro	rold
	rol	@0
	rol	@0H
	rol	@0B3
	rol	@0B4
	.endm
	
	.macro	rorw
	ror	@0H
	ror	@0
	.endm
	
	.macro	rort
	ror	@0B3
	ror	@0H
	ror	@0
	.endm
	
	.macro	rord
	ror	@0B4
	ror	@0B3
	ror	@0H
	ror	@0
	.endm
	
	.macro	lslw
	lsl	@0
	rol	@0H
	.endm
	
	.macro	lslt
	lsl	@0
	rol	@0H
	rol	@0B3
	.endm
	
	.macro	lsld
	lsl	@0
	rol	@0H
	rol	@0B3
	rol	@0B4
	.endm
	
	.macro	lsrw
	lsr	@0H
	ror	@0
	.endm
	
	.macro	lsrt
	lsr	@0B3
	ror	@0H
	ror	@0
	.endm
	
	.macro	lsrd
	lsr	@0B4
	ror	@0B3
	ror	@0H
	ror	@0
	.endm
	
	.macro asrw
	asr	@0H
	ror	@0
	.endm
	
	.macro asrt
	asr	@0B3
	ror	@0H
	ror	@0
	.endm
	
	.macro asrd
	asr	@0B4
	ror	@0B3
	ror	@0H
	ror	@0
	.endm
		
;------------------------------------------------------------------------------------------------------------------------
;	compare routines
;------------------------------------------------------------------------------------------------------------------------		
	.macro	cpw
	cp	@0,@1
	cpc	@0H,@1H
	.endm
	
	.macro	cpcw
	cpc	@0,@1
	cpc	@0H,@1H
	.endm
	
	.macro	cpt
	cp	@0,@1
	cpc	@0H,@1H
	cpc	@0B3,@1B3
	.endm
	
	.macro	cpct
	cpc	@0,@1
	cpc	@0H,@1H
	cpc	@0B3,@1B3
	.endm
		
	.macro	cpd
	cp	@0,@1
	cpc	@0H,@1H
	cpc	@0B3,@1B3
	cpc	@0B4,@1B4
	.endm
	
	.macro	cpcd
	cpc	@0,@1
	cpc	@0H,@1H
	cpc	@0B3,@1B3
	cpc	@0B4,@1B4
	.endm
	
	.macro	cpiwh
	cpi	@0,low(@2)
	ldi	@1,high(@2)
	cpc	@0H,@1
	.endm
	
	.macro	cpith
	cpi	@0,low(@2)
	ldi	@1,high(@2)
	cpc	@0H,@1
	ldi	@1,byte3(@2)
	cpc	@0B3,@1
	.endm
	
	.macro	cpidh
	cpi	@0,low(@2)
	ldi	@1,high(@2)
	cpc	@0H,@1
	ldi	@1,byte3(@2)
	cpc	@0B3,@1
	ldi	@1,byte4(@2)
	cpc	@0B4,@1
	.endm
	
	.macro	tstw
	cp	@0,@1
	cpc	@0H,@1
	.endm
	
	.macro	tstt
	cp	@0,@1
	cpc	@0H,@1
	cpc	@0B3,@1
	.endm
			
;------------------------------------------------------------------------------------------------------------------------
;	bit set/clr for RAM
;------------------------------------------------------------------------------------------------------------------------
	.macro	sbs
	lds	R16,@0
	sbr	R16,1<<@1
	sts	@0,R16
	.endm
	
	.macro	cbs
	lds	R16,@0
	cbr	R16,1<<@1
	sts	@0,R16
	.endm
	
;------------------------------------------------------------------------------------------------------------------------
;	bit set/clr for I/O non-bit addressable
;------------------------------------------------------------------------------------------------------------------------
	.macro	sbii
	in	R16,@0
	ori	R16,1<<@1
	out	@0,R16
	.endm
	
	.macro	cbii
	in	R16,@0
	cbr	R16,1<<@1
	out	@0,R16
	.endm
	
;------------------------------------------------------------------------------------------------------------------------
;	bit sbr/cbr direct for I/O addressable
;------------------------------------------------------------------------------------------------------------------------
	.macro	sbrii
	in	R16,@0
	sbr	R16,@1
	out	@0,R16
	.endm
	
	.macro	cbrii
	in	R16,@0
	cbr	R16,@1
	out	@0,R16
	.endm
	
	
;------------------------------------------------------------------------------------------------------------------------
;	bit sbr/cbr direct for RAM
;------------------------------------------------------------------------------------------------------------------------
	.macro	sbsi
	lds	R16,@0
	sbr	R16,@1
	sts	@0,R16
	.endm
	
	.macro	cbsi
	lds	R16,@0
	cbr	R16,@1
	sts	@0,R16
	.endm
	
	
;------------------------------------------------------------------------------------------------------------------------	
;	rjmp equivalents for branches and skips
;------------------------------------------------------------------------------------------------------------------------
	.macro	rjts
	brtc	PC+2
	rjmp	@0
	.endm

	.macro	rjtc
	brts	PC+2
	rjmp	@0
	.endm
	
	.macro	rjne
	breq	PC+2
	rjmp	@0
	.endm
	
	.macro	rjeq
	brne	PC+2
	rjmp	@0
	.endm
	
	.macro	rjcs
	brcc	PC+2
	rjmp	@0
	.endm
	
	.macro	rjcc
	brcs	PC+2
	rjmp	@0
	.endm
	
	.macro	rjsh
	brlo	PC+2
	rjmp	@0
	.endm
	
	.macro	rjlo
	brsh	PC+2
	rjmp	@0
	.endm
	
	.macro	rjmi
	brpl	PC+2
	rjmp	@0
	.endm
	
	.macro	rjpl
	brmi	PC+2
	rjmp	@0
	.endm
	
	.macro dbnz
	dec	@0
	brne	@1
	.endm
	
	.macro djnz
	dec	@0
	breq	PC+2
	rjmp	@1
	.endm
	
	.macro dbz
	dec	@0
	breq	@1
	.endm
	
	.macro djz
	dec	@0
	brne	PC+2
	rjmp	@1
	.endm
	
	.macro dbnzwh
	sbiw	@0,1
	brne	@1
	.endm
	
	.macro djnzwh
	sbiw	@0,1
	breq	PC+2
	rjmp	@1
	.endm
	
	.macro dbzwh
	sbiw	@0,1
	breq	@1
	.endm
	
	.macro djzwh
	sbiw	@0,1
	brne	PC+2
	rjmp	@1
	.endm
	
	.macro	dbnzw
	subi	@0,low(1)
	sbci	@0H,high(1)
	brne	@1
	.endm
	
	.macro	djnzw
	subi	@0,low(1)
	sbci	@0H,high(1)
	breq	PC+2
	rjmp	@1
	.endm
	
	.macro	dbzw
	subi	@0,low(1)
	sbci	@0H,high(1)
	breq	@1
	.endm
	
	.macro	djzw
	subi	@0,low(1)
	sbci	@0H,high(1)
	brne	PC+2
	rjmp	@1
	.endm
	
	.macro	ibne
	inc	@0
	cpi	@0,@1
	brne	@2
	.endm
	
	.macro	ijne
	inc	@0
	cpi	@0,@1
	breq	PC+2
	rjmp	@2
	.endm
	
	.macro rjss
	lds	R16,@0
	sbrc	R16,@1
	rjmp	@2
	.endm
	
	.macro rjsc
	lds	R16,@0
	sbrs	R16,@1
	rjmp	@2
	.endm
	
	.macro rjrs
	sbrc	@0,@1
	rjmp	@2
	.endm
	
	.macro rjrc
	sbrs	@0,@1
	rjmp	@2
	.endm
	
	.macro rjis
	sbic	@0,@1
	rjmp	@2
	.endm
	
	.macro rjic
	sbis	@0,@1
	rjmp	@2
	.endm