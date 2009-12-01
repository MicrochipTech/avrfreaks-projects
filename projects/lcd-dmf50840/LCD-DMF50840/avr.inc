;------------------------------------------------;
; Constants

#define	bit0	0b00000001
#define	bit1	0b00000010
#define	bit2	0b00000100
#define	bit3	0b00001000
#define	bit4	0b00010000
#define	bit5	0b00100000
#define	bit6	0b01000000
#define	bit7	0b10000000

#if 0
#define	T0L	= r0
#define	T0H	= r1
#define	T2L	= r2
#define	T2H	= r3
#define	T4L	= r4
#define	T4H	= r5
#define	T6L	= r6
#define	T6H	= r7
#define	T8L	= r8
#define	T8H	= r9
#endif

#define	AL	r16
#define	AH	r17
#define	BL	r18
#define	BH	r19
;#define	CL	r20
;#define	CH	r21
;#define	DL	r22
;#define	DH	r23
;#define	EL	r24
;#define	EH	r25


#define	_BV(arg) (1 << (arg) )

;------------------------------------------------;
; Push/Pop register pair
;
; 	pushw	Z

.macro	pushw arg0, arg1
	push	\arg1
	push	\arg0
.endm

.macro	popw arg0, arg1
	pop		\arg0
	pop		\arg1
.endm


;------------------------------------------------;
; Load/store word from/to direct memory/immediate
;
;	ldsw	Z,mem
;	ldiw	Z,imm

.macro	ldiw arg0, arg1, arg2
	ldi	\arg0,lo8(\arg2)
	ldi	\arg1,hi8(\arg2)		//fail
.endm

.macro	ldsw arg0, arg1, arg2
	lds	\arg0,\arg2
	lds	\arg1,\arg2+1
.endm

.macro	lddw arg0, arg1, arg2
	ldd	\arg0,\arg2
	ldd	\arg1,\arg2+1
.endm

.macro	stsw arg0, arg1, arg2
	sts	\arg0,\arg1
	sts	\arg0+1,\arg2
.endm

.macro	stdw arg0, arg1, arg2
	std	\arg0,\arg1
	std	\arg0+1,\arg2
.endm


;------------------------------------------------;
; Store immediate into indirect memory via r16
;
;	sti	Z,imm
;	stdi	Z+d,imm

.macro	sti arg0, arg1
	ldi		r16,\arg1
	st		\arg0,r16
.endm

.macro	stdi arg0, arg1
	ldi		r16,\arg1
	std		\arg0,r16
.endm


;------------------------------------------------;
; add/sub/subc/cp/cpc/lsl/lsr/rol/ror to register pair
;

.macro	addiw arg0, arg1, arg2
	subi	\arg0,lo8(-(\arg2))
	sbci	\arg1,hi8(-(\arg2))
.endm

.macro	subiw arg0, arg1, arg2
	subi	\arg0,lo8(\arg2)
	sbci	\arg1,hi8(\arg2)
.endm

.macro	addw arg0, arg1, arg2, arg3
	add	\arg0,\arg2
	adc	\arg1,\arg3
.endm

.macro	adcw arg0, arg1, arg2, arg3
	adc	\arg0,\arg2
	adc	\arg1,\arg3
.endm

.macro	subw arg0, arg1, arg2, arg3
	sub	\arg0,\arg2
	sbc	\arg1,\arg3
.endm

.macro	sbcw arg0, arg1, arg2, arg3
	sbc	\arg0,\arg2
	sbc	\arg1,\arg3
.endm

.macro	cpw arg0, arg1, arg2, arg3
	cp	\arg0,\arg2
	cpc	\arg1,\arg3
.endm

.macro	cpcw arg0, arg1, arg2, arg3
	cpc	\arg0,\arg2
	cpc	\arg1,\arg3
.endm

.macro	andw arg0, arg1, arg2, arg3
	and	\arg0,\arg2
	and	\arg1,\arg3
.endm

.macro	andiw arg0, arg1, arg2
	andi	\arg0,lo8(\arg2)
	andi	\arg1,hi8(\arg2)
.endm

.macro	orw arg0, arg1, arg2, arg3
	or	\arg0,\arg2
	or	\arg1,\arg3
.endm

.macro	oriw arg0, arg1, arg2
	ori	\arg0,lo8(\arg2)
	ori	\arg1,hi8(\arg2)
.endm

.macro	lslw arg0, arg1
	lsl	\arg0
	rol	\arg1
.endm

.macro	lsrw arg0, arg1
	lsr	\arg0
	ror	\arg1
.endm

.macro	rolw arg0, arg1
	rol	\arg0
	rol	\arg1
.endm

.macro	rorw arg0, arg1
	ror	\arg0
	ror	\arg1
.endm

.macro	clrw arg0, arg1
	clr	\arg0
	clr	\arg1
.endm

.macro	comw arg0, arg1
	com	\arg0
	com	\arg1
.endm

.macro	movew arg0, arg1, arg2
	mov	\arg0, \arg2
	mov	\arg1, \arg3
.endm


;------------------------------------------------;
; Store immediate into direct memory via r16
;
;	stsi	var,imm

.macro	stsi arg0, arg1
	ldi	r16,\arg1
	sts	\arg0,r16
.endm


;------------------------------------------------;
; Output port immediate via r16
;
;	outi	port,var

.macro	outi arg0, arg1
	ldi	r16,\arg1
	out	\arg0,r16
.endm


;------------------------------------------------;
; Add immediate to register

.macro	addi arg0, arg1
	subi	\arg0,-(\arg1)
.endm


;------------------------------------------------;
; Long branch


.macro	rjne arg0
	breq	PC+2
	rjmp	\arg0
.endm

.macro	rjeq arg0
	brne	PC+2
	rjmp	\arg0
.endm

.macro	rjcc arg0
	brcs	PC+2
	rjmp	\arg0
.endm

.macro	rjcs arg0
	brcc	PC+2
	rjmp	\arg0
.endm

.macro	rjtc arg0
	brts	PC+2
	rjmp	\arg0
.endm

.macro	rjts arg0
	brtc	PC+2
	rjmp	\arg0
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

.macro	bmov arg0, arg1, arg2, arg3
	bst	\arg2,\arg3
	bld	\arg0,\arg1
.endm


