;------------------------------------------------;
; Constants

.equ	RAMTOP = 0x60		; SRAM top address
.equ	RAMTOP100 = 0x100	; For memory mapped I/O devices


.equ	bit0	= 0b00000001
.equ	bit1	= 0b00000010
.equ	bit2	= 0b00000100
.equ	bit3	= 0b00001000
.equ	bit4	= 0b00010000
.equ	bit5	= 0b00100000
.equ	bit6	= 0b01000000
.equ	bit7	= 0b10000000


.def	T0L	= r0
.def	T0H	= r1
.def	T2L	= r2
.def	T2H	= r3
.def	T4L	= r4
.def	T4H	= r5
.def	T6L	= r6
.def	T6H	= r7
.def	T8L	= r8
.def	T8H	= r9
.def	T10L	= r10
.def	T10H	= r11
.def	T12L	= r12
.def	T12H	= r13
.def	T14L	= r14
.def	T14H	= r15


.def	AL	= r16
.def	AH	= r17
.def	BL	= r18
.def	BH	= r19
.def	CL	= r20
.def	CH	= r21
.def	DL	= r22
.def	DH	= r23
.def	EL	= r24
.def	EH	= r25



;------------------------------------------------;
; Push/Pop register pair
;
; 	pushw	Z

.macro	pushw
	push	@0H
	push	@0L
.endm

.macro	popw
	pop	@0L
	pop	@0H
.endm


;------------------------------------------------;
; Load/store word from/to direct memory/immediate
;
;	ldsw	Z,mem
;	ldiw	Z,imm

.macro	ldiw
	ldi	@0L,low(@1)
	ldi	@0H,high(@1)
.endm

.macro	ldsw
	lds	@0L,@1
	lds	@0H,@1+1
.endm

.macro	lddw
	ldd	@0L,@1
	ldd	@0H,@1+1
.endm

.macro	ldw
	ld	@0L,@1
	ld	@0H,@1
.endm

.macro	stsw
	sts	@0,@1L
	sts	@0+1,@1H
.endm

.macro	stdw
	std	@0,@1L
	std	@0+1,@1H
.endm

.macro	stw
	st	@0,@1L
	st	@0,@1H
.endm

.macro	outw
	out	@0H,@1H
	out	@0L,@1L
.endm

.macro	inw
	in	@0L,@1L
	in	@0H,@1H
.endm

.macro	outwex
	sts	@0H,@1H
	sts	@0L,@1L
.endm
;------------------------------------------------;
; Store immediate into indirect memory via r16
;
;	sti	Z,imm
;	stdi	Z+d,imm

.macro	sti
	ldi	r16,@1
	st	@0,r16
.endm

.macro	stdi
	ldi	r16,@1
	std	@0,r16
.endm

.macro	muli
	ldi	r16,@1
	mul	@0,r16
.endm


;------------------------------------------------;
; add/sub/subc/cp/cpc/lsl/lsr/rol/ror to register pair
;

.macro	addiw
	subi	@0L,low(-(@1))
	sbci	@0H,high(-(@1))
.endm

.macro	subiw
	subi	@0L,low(@1)
	sbci	@0H,high(@1)
.endm

.macro	addw
	add	@0L,@1L
	adc	@0H,@1H
.endm

.macro	adcw
	adc	@0L,@1L
	adc	@0H,@1H
.endm

.macro	subw
	sub	@0L,@1L
	sbc	@0H,@1H
.endm

.macro	sbcw
	sbc	@0L,@1L
	sbc	@0H,@1H
.endm

.macro	cpw
	cp	@0L,@1L
	cpc	@0H,@1H
.endm

.macro	cpcw
	cpc	@0L,@1L
	cpc	@0H,@1H
.endm

.macro	cpiw
	cpi	@0L,low(@1)
	ldi	r16,high(@1) 
	cpc	@0H,r16
.endm

.macro	andw
	and	@0L,@1L
	and	@0H,@1H
.endm

.macro	andiw
	andi	@0L,low(@1)
	andi	@0H,high(@1)
.endm

.macro	orw
	or	@0L,@1L
	or	@0H,@1H
.endm

.macro	oriw
	ori	@0L,low(@1)
	ori	@0H,high(@1)
.endm

.macro	lslw
	lsl	@0L
	rol	@0H
.endm

.macro	lsrw
	lsr	@0H
	ror	@0L
.endm

.macro	asrw
	asr	@0H
	ror	@0L
.endm

.macro	rolw
	rol	@0L
	rol	@0H
.endm

.macro	rorw
	ror	@0H
	ror	@0L
.endm

.macro	clrw
	clr	@0L
	clr	@0H
.endm

.macro	comw
	com	@0L
	com	@0H
.endm

.macro	negw
	com	@0H
	neg	@0L
	brne	PC+2
	inc	@0H
.endm

.macro	movew
	mov	@0L, @1L
	mov	@0H, @1H
.endm

.macro	lpmw
	lpm	@0L, @1
	lpm	@0H, @1
.endm

.macro	movww
	movw @0H:@0L,@1H:@1L
.endm

;------------------------------------------------;
; Store immediate into direct memory via r16
;
;	stsi	var,imm

.macro	stsi
	ldi	r16,@1
	sts	@0,r16
.endm


;------------------------------------------------;
; Output port immediate via r16
;
;	outi	port,var

.macro	outi
	ldi	r16,@1
	out	@0,r16
.endm
.macro	outiex
	ldi	r16,@1
	sts	@0,r16
.endm

;------------------------------------------------;
; Add immediate to register

.macro	addi
	subi	@0,-(@1)
.endm


;------------------------------------------------;
; Long branch


.macro	rjne
	breq	PC+2
	rjmp	@0
.endm

.macro	rjeq
	brne	PC+2
	rjmp	@0
.endm

.macro	rjcc
	brcs	PC+2
	rjmp	@0
.endm

.macro	rjcs
	brcc	PC+2
	rjmp	@0
.endm

.macro	rjtc
	brts	PC+2
	rjmp	@0
.endm

.macro	rjts
	brtc	PC+2
	rjmp	@0
.endm


.macro	retcc
	brcs	PC+2
	ret
.endm

.macro	retcs
	brcc	PC+2
	ret
.endm

.macro	reteq
	brne	PC+2
	ret
.endm

.macro	retne
	breq	PC+2
	ret
.endm


;------------------------------------------------;
; Move single bit between two registers
;
;	bmov	dstreg,dstbit,srcreg.srcbit

.macro	bmov
	bst	@2,@3
	bld	@0,@1
.endm


