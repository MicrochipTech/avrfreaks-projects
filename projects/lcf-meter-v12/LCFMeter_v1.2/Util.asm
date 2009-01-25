

;============================= MISC ============================
.equ	F_CPU = 20 * 1000000 			; CPU frequency in hz

.equ	True = -1
.equ	False = 0


.def	UTmp = r16


;============================ MACROS ===========================


;------------------------------------------------;
; Add immediate to register

.MACRO	addi
	subi	@0, -(@1)
.ENDM

.MACRO	LDIW ; RegisterHL,ConstantW
	ldi	@0l,low(@1)
	ldi	@0h,high(@1)
.ENDM


;*********************************************************
;*	SETB  - SET Bit in IO of data space
;*	CLRB  - CLeaR Bit in IO of data space
;*  SKBS  - SKip if Bit Set
;*  SKBC  - SKip if Bit Cleared
;* 	STORE - Store register in IO or data space
;* 	LOAD  - Load register from IO or data space
;*********************************************************

.MACRO SETB 							;Arguments: Address, Bit
	.if @1>7
		.error "Only values 0-7 allowed for Bit parameter"
	.endif
	.if @0>0x3F
		push	UTmp
		lds  	UTmp, @0
		sbr  	UTmp, (1<<@1)
		sts  	@0, UTmp
		pop		UTmp
	.elif @0>0x1F
		push	UTmp
		in   	UTmp, @0
		sbr  	UTmp, (1<<@1)
		out  	@0, UTmp
		pop		UTmp
	.else
		sbi  	@0, @1
	.endif
.ENDM

.MACRO CLRB 							;Arguments: Address, Bit
	.if @1>7
		.error "Only values 0-7 allowed for Bit parameter"
	.endif
	.if @0>0x3F
		push	UTmp
		lds  	UTmp, @0
		cbr  	UTmp, (1<<@1)
		sts  	@0, UTmp
		pop		UTmp
	.elif @0>0x1F
		push 	UTmp
		in   	UTmp, @0
		cbr  	UTmp, (1<<@1)
		out  	@0, UTmp
		pop		UTmp
	.else
		cbi  	@0, @1
	.endif
.ENDM

.MACRO STOREI 							;Arguments: Address, Register
	push	UTmp
	ldi		UTmp, @1
	.if	@0>0x3F
		sts	@0, UTmp
	.else
		out	@0, UTmp
	.endif
	pop		UTmp
.ENDM


.MACRO STORE 							;Arguments: Address, Register
	.if	@0>0x3F
		sts	@0, @1
	.else
		out	@0, @1
	.endif
.ENDM

.MACRO LOAD 							;Arguments: Register, Address
	.if	@1>0x3F
		lds	@0, @1
	.else
		in	@0, @1
	.endif
.ENDM

.MACRO PushX
	push 	XH
	push 	XL
.ENDM

.MACRO PopX
	pop 	XL
	pop 	XH
.ENDM

.MACRO PushY
	push 	YH
	push 	YL
.ENDM

.MACRO PopY
	pop 	YL
	pop 	YH
.ENDM

.MACRO PushZ
	push 	ZH
	push 	ZL
.ENDM

.MACRO PopZ
	pop 	ZL
	pop 	ZH
.ENDM

.MACRO	LoadX
	ldi		XH, high(@0)
	ldi		XL, low(@0)
.ENDM

.MACRO	LoadY
	ldi		YH, high(@0)
	ldi		YL, low(@0)
.ENDM

.MACRO	LoadZ
	ldi		ZH, high(@0)
	ldi		ZL, low(@0)
.ENDM


.MACRO SwapReg
	eor		@0, @1
	eor		@1, @0
	eor		@0, @1
.ENDM




.MACRO Delay1ms							; Delay1ms <k> 		where k = 1..255
	push	r21							;
	push	r22							; Precision delay of 1..255 ms
	push	r23							;
	push	r24
	ldi		r21, byte1((F_CPU*@0)/(6*1000)-4)
	ldi		r22, byte2((F_CPU*@0)/(6*1000)-4)
	ldi		r23, byte3((F_CPU*@0)/(6*1000)-4)
	ldi		r24, byte4((F_CPU*@0)/(6*1000)-4)
	rcall	_DoDelay
	pop		r24
	pop		r23
	pop		r22
	pop		r21
.ENDM

.MACRO Delay10us						; Delay10us <k> 		where k = 1..255
	push	r21
	push	r22
	push	r23							; Precision delay of 10us..2.55ms
	push	r24
	ldi		r21, byte1((F_CPU*@0)/(6*100000)-4)
	ldi		r22, byte2((F_CPU*@0)/(6*100000)-4)
	ldi		r23, byte3((F_CPU*@0)/(6*100000)-4)
	ldi		r24, byte4((F_CPU*@0)/(6*100000)-4)
	rcall	_DoDelay
	pop		r24
	pop		r23
	pop		r22
	pop		r21
.ENDM

;================================= Subs ==============================

_DoDelay:								; Called by Delay1ms & Delay10us to generate actual delay
	subi	r21, 1					;1 clk
	sbci	r22, 0					;1 clk
	sbci	r23, 0					;1 clk
	sbci	r24, 0					;1 clk
	brne 	_DoDelay				;[2] clks
	ret

