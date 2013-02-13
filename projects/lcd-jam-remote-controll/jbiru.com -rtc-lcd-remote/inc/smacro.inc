;=======================================
;ooooooooooooooooooooooooooooooooooooooo
;---------------------------------------
; definisi macro konversi mcs-51 ke avr
;~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ 
.macro	table
	ldi	ZL, low(@0*2)
	ldi	ZH, high(@0*2)
.endm

.macro	syptr
	ldi	YL, low(@0)
	ldi	YH, high(@0)
.endm

.macro	sxptr
	ldi	XL, low(@0)
	ldi	XH, high(@0)
.endm
;~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
.macro	outp
	ldi	A, @1
	out	@0, A
.endm

.macro	djnz
	dec	@0
	brne	@1
.endm

.macro	sdjnz
	lds	A, @0
	dec	A
	sts	@0, A
	brne	@1
.endm

.macro	smov
	ldi	A, @1
	sts	@0, A
.endm

.macro	sdec
	lds	A, @0
	dec	A
	sts	@0, A
.endm

.macro	sinc
	lds	A, @0
	inc	A
	sts	@0, A
.endm

.macro	setb
	mov	B, @0
	sbr	B, (1<<@1)
	mov	@0, B
.endm

.macro	clrb
	mov	B, @0
	cbr	B, (1<<@1) 
	mov	@0, B
.endm
.macro	jnb
	sbrs	@0, @1
	rjmp	@2
.endm

.macro	jb
	sbrc	@0, @1
	rjmp	@2
.endm

.macro	jbi
	sbic	@0, @1
	rjmp	@2
.endm

.macro	jnbi
	sbis	@0, @1
	rjmp	@2
.endm

.macro	jz
	cpi	@0, 0
	breq	@1
.endm

.macro	jnz
	cpi	@0, 0
	brne	@1
.endm

.macro	cjne
	cpi	@0, @1
	brne	@2
.endm

.macro	cpli
	sbis	@0, @1
	rjmp	__cplis
	cbi	@0, @1
	rjmp	__cplir
__cplis:	sbi	@0, @1
__cplir:	
.endm

;~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
;	end macro
;=======================================
;---------------------------------------
