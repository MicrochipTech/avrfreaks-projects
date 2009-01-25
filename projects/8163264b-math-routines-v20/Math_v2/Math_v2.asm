


;*****************************************************************************
;								Misc Math Routines
;								   Release: 2.0
;
;	Contact:	Hal Foster, halfoster@gmail.com
;
;	Note:	All routines take mVar1_b<0..7> and mVar2_b<0..7> as inputs, and
;			need to be initialized before they are called.  Any result is always
;			returned in mVar1.  mVar2 is always left unchanged.
;
;			These routines have been designed not necessarily for speed or size
;			(except for conditional assemble) - they are designed primarily
;			for ease of use; i.e. no registers are stomped upun return.
;
;Revision:	1.0		Initial release
;			2.0		Major overhaul, added hardware support for multiplication,
;					increased granularity in all routines
;*****************************************************************************




;=============================================================================
;					Math Conditional Assembly Switches
;=============================================================================

.SET m_UseAdd64		= FALSE
.SET m_UseAdd32		= FALSE
.SET m_UseAdd16		= FALSE
.SET m_UseAdd8		= FALSE

.SET m_UseSub64		= FALSE
.SET m_UseSub32		= FALSE
.SET m_UseSub16		= FALSE
.SET m_UseSub8		= FALSE


.SET m_UseHWmul		= FALSE				; enables hardware multiplication for uc's that support it

.SET m_UseMul8 		= FALSE
.SET m_UseMul16 	= FALSE
.SET m_UseMul32 	= TRUE

.SET m_UseRound		= FALSE				; enables rounding function for division routines

.SET m_UseDiv8 		= FALSE
.SET m_UseDiv16 	= FALSE
.SET m_UseDiv32 	= FALSE
.SET m_UseDiv64 	= FALSE

.SET m_UseComp8  	= FALSE
.SET m_UseComp16 	= FALSE
.SET m_UseComp32 	= FALSE
.SET m_UseComp64 	= FALSE

.SET m_UseCopy		= FALSE
.SET m_UseSwap		= FALSE

;==============================================================================
;							Variable definitions (SRAM)
;
;	NOTE:  All variables are passed/returned through these.
;=============================================================================

.DSEG

mVar1_b7:	.BYTE	1
mVar1_b6:	.BYTE	1
mVar1_b5:	.BYTE	1
mVar1_b4:	.BYTE	1
mVar1_b3:	.BYTE	1
mVar1_b2:	.BYTE	1
mVar1_b1:	.BYTE	1
mVar1_b0:	.BYTE	1

mVar2_b7:	.BYTE	1
mVar2_b6:	.BYTE	1
mVar2_b5:	.BYTE	1
mVar2_b4:	.BYTE	1
mVar2_b3:	.BYTE	1
mVar2_b2:	.BYTE	1
mVar2_b1:	.BYTE	1
mVar2_b0:	.BYTE	1

mMod_b7:	.BYTE	1
mMod_b6:	.BYTE	1
mMod_b5:	.BYTE	1
mMod_b4:	.BYTE	1
mMod_b3:	.BYTE	1
mMod_b2:	.BYTE	1
mMod_b1:	.BYTE	1
mMod_b0:	.BYTE	1

.CSEG

;===============================================================================
;								Register definitions

.def	mulL	=	r0
.def	mulH	=	r1

.def	var10	=	r2
.def	var11	=	r3
.def	var12	=	r4
.def	var13	=	r5
.def	var14	=	r6
.def	var15	=	r7
.def	var16	=	r8
.def	var17	=	r9

.def	var20	=	r10
.def	var21	=	r11
.def	var22	=	r12
.def	var23	=	r13
.def	var24	=	r14
.def	var25	=	r15
.def	var26	=	r16
.def	var27	=	r17

.def	mod0	=	r18
.def	mod1	=	r19
.def	mod2	=	r20
.def	mod3	=	r21
.def	mod4	=	r22
.def	mod5	=	r23
.def	mod6	=	r24
.def	mod7	=	r25

.def	mTmp	=	r26
.def	mLC		=	r27

;================================ Math Macros ========================

.MACRO	StoreI64mVar1							; Loads mVar1 with <k>
	push	MTmp								;
	ldi		MTmp, byte1(@0)						; Useage:  StoreImVar1 <k>
	sts		mVar1_b0, MTmp						;
	ldi		MTmp, byte2(@0)						;  where:  <k> = 0 to an 8 byte (64 bit) number
	sts		mVar1_b1, MTmp
	ldi		MTmp, byte3(@0)
	sts		mVar1_b2, MTmp
	ldi		MTmp, byte4(@0)
	sts		mVar1_b3, MTmp
	ldi		MTmp, byte1(@0>>32)
	sts		mVar1_b4, MTmp
	ldi		MTmp, byte2(@0>>32)
	sts		mVar1_b5, MTmp
	ldi		MTmp, byte3(@0>>32)
	sts		mVar1_b6, MTmp
	ldi		MTmp, byte4(@0>>32)
	sts		mVar1_b7, MTmp
	pop		MTmp
.ENDM


.MACRO	StoreI32mVar1								; Loads mVar1 with <k>
	.IF @0>4294967295
		.error "Parameter too large"
	.ELSE
		push	MTmp								;
		rcall	ClrmVar1
		ldi		MTmp, byte1(@0)						; Useage:  StoreImVar1 <k>
		sts		mVar1_b0, MTmp						;
		ldi		MTmp, byte2(@0)						;  where:  <k> = 0 to an 8 byte (64 bit) number
		sts		mVar1_b1, MTmp
		ldi		MTmp, byte3(@0)
		sts		mVar1_b2, MTmp
		ldi		MTmp, byte4(@0)
		sts		mVar1_b3, MTmp
		pop		MTmp
	.ENDIF
.ENDM

.MACRO	StoreI16mVar1							; Loads mVar1 with <k>
	.IF @0>65535
		.error "Parameter too large"
	.ELSE
		push	MTmp								;
		rcall	ClrmVar1
		ldi		MTmp, byte1(@0)						; Useage:  StoreImVar1 <k>
		sts		mVar1_b0, MTmp						;
		ldi		MTmp, byte2(@0)						;  where:  <k> = 0 to an 8 byte (64 bit) number
		sts		mVar1_b1, MTmp
		pop		MTmp
	.ENDIF
.ENDM

.MACRO	StoreI8mVar1							; Loads mVar1 with <k>
	.IF @0>255
		.error "Parameter too large"
	.ELSE
		push	MTmp								;
		rcall	ClrmVar1
		ldi		MTmp, byte1(@0)						; Useage:  StoreImVar1 <k>
		sts		mVar1_b0, MTmp						;
		pop		MTmp
	.ENDIF
.ENDM

.MACRO	StoreI64mVar2								; Loads mVar2 with <k>
	push	MTmp
	ldi		MTmp, byte1(@0)						; Useage:  StoreImVar2 <k>
	sts		mVar2_b0, MTmp
	ldi		MTmp, byte2(@0)						;  where:  <k> = 0 to an 8 byte (64 bit) number
	sts		mVar2_b1, MTmp
	ldi		MTmp, byte3(@0)
	sts		mVar2_b2, MTmp
	ldi		MTmp, byte4(@0)
	sts		mVar2_b3, MTmp
	ldi		MTmp, byte1(@0>>32)
	sts		mVar2_b4, MTmp
	ldi		MTmp, byte2(@0>>32)
	sts		mVar2_b5, MTmp
	ldi		MTmp, byte3(@0>>32)
	sts		mVar2_b6, MTmp
	ldi		MTmp, byte4(@0>>32)
	sts		mVar2_b7, MTmp
	pop		MTmp
.ENDM

.MACRO	StoreI32mVar2								; Loads mVar2 with <k>
	.IF @0>4294967295
		.error "Parameter too large"
	.ELSE
		push	MTmp
		rcall	ClrmVar2
		ldi		MTmp, byte1(@0)						; Useage:  StoreImVar2 <k>
		sts		mVar2_b0, MTmp
		ldi		MTmp, byte2(@0)						;  where:  <k> = 0 to an 8 byte (64 bit) number
		sts		mVar2_b1, MTmp
		ldi		MTmp, byte3(@0)
		sts		mVar2_b2, MTmp
		ldi		MTmp, byte4(@0)
		sts		mVar2_b3, MTmp
		pop		MTmp
	.ENDIF
.ENDM

.MACRO	StoreI16mVar2								; Loads mVar2 with <k>
	.IF @0>65535
		.error "Parameter too large"
	.ELSE
		push	MTmp
		rcall	ClrmVar2
		ldi		MTmp, byte1(@0)						; Useage:  StoreImVar2 <k>
		sts		mVar2_b0, MTmp
		ldi		MTmp, byte2(@0)						;  where:  <k> = 0 to an 8 byte (64 bit) number
		sts		mVar2_b1, MTmp
		pop		MTmp
	.ENDIF
.ENDM

.MACRO	StoreI8mVar2								; Loads mVar2 with <k>
	.IF @0>255
		.error "Parameter too large"
	.ELSE
		push	MTmp
		rcall	ClrmVar2
		ldi		MTmp, byte1(@0)						; Useage:  StoreImVar2 <k>
		sts		mVar2_b0, MTmp
		pop		MTmp
	.ENDIF
.ENDM

;-----------------------------------------------------------------------------:
; Add mVar2 to mVar1 (8 bytes)

;****************************************************************************
;							 Addition Routines
;****************************************************************************

.IF m_UseAdd64
	mAdd64:											; Adds the 64 bit numbers in mVar1 and mVar2
		push	mLC									; Sets SREG flags for action after call
		push	var10
		push	var11
		push	var12
		push	var13
		push	var14
		push	var15
		push	var16
		push	var17
		Load	var10, mVar1_b0
		Load	var11, mVar1_b1
		Load	var12, mVar1_b2
		Load	var13, mVar1_b3
		Load	var14, mVar1_b4
		Load	var15, mVar1_b5
		Load	var16, mVar1_b6
		Load	var17, mVar1_b7
		Load	mLC, mVar2_b0
		add		var10, mLC
		Load	mLC, mVar2_b1
		adc		var11, mLC
		Load	mLC, mVar2_b2
		adc		var12, mLC
		Load	mLC, mVar2_b3
		adc		var13, mLC
		Load	mLC, mVar2_b4
		adc		var14, mLC
		Load	mLC, mVar2_b5
		adc		var15, mLC
		Load	mLC, mVar2_b6
		adc		var16, mLC
		Load	mLC, mVar2_b7
		adc		var17, mLC
		Store	mVar1_b0, var10
		Store	mVar1_b1, var11
		Store	mVar1_b2, var12
		Store	mVar1_b3, var13
		Store	mVar1_b4, var14
		Store	mVar1_b5, var15
		Store	mVar1_b6, var16
		Store	mVar1_b7, var17
		pop		var17
		pop		var16
		pop		var15
		pop		var14
		pop		var13
		pop		var12
		pop		var11
		pop		var10
		pop		mLC
		ret	
.ENDIF

.IF m_UseAdd32
	mAdd32:											; Adds the 32 bit numbers in mVar1 and mVar2
		push	mLC									; Sets SREG flags for action after call
		push	var10
		push	var11
		push	var12
		push	var13
		Load	var10, mVar1_b0
		Load	var11, mVar1_b1
		Load	var12, mVar1_b2
		Load	var13, mVar1_b3
		Load	mLC, mVar2_b0
		add		var10, mLC
		Load	mLC, mVar2_b1
		adc		var11, mLC
		Load	mLC, mVar2_b2
		adc		var12, mLC
		Load	mLC, mVar2_b3
		adc		var13, mLC
		Store	mVar1_b0, var10
		Store	mVar1_b1, var11
		Store	mVar1_b2, var12
		Store	mVar1_b3, var13
		pop		var13
		pop		var12
		pop		var11
		pop		var10
		pop		mLC
		ret	
.ENDIF

.IF m_UseAdd16
	mAdd16:											; Adds the 16 bit numbers in mVar1 and mVar2
		push	mLC									; Sets SREG flags for action after call
		push	var10
		push	var11
		Load	var10, mVar1_b0
		Load	var11, mVar1_b1
		Load	mLC, mVar2_b0
		add		var10, mLC
		Load	mLC, mVar2_b1
		adc		var11, mLC
		Store	mVar1_b0, var10
		Store	mVar1_b1, var11
		pop		var11
		pop		var10
		pop		mLC
		ret	
.ENDIF

.IF m_UseAdd8
	mAdd8:												; Adds the 8 bit numbers in mVar1 and mVar2
		push	mLC
		push	var10									; Sets SREG flags for action after call
		Load	var10, mVar1_b0
		Load	mLC, mVar2_b0
		add		var10, mLC
		Store	mVar1_b0, var10
		pop		var10
		pop		mLC
		ret	
.ENDIF

;****************************************************************************
;							 Subtraction Routines
;****************************************************************************

.IF m_UseSub64
	mSub64:											; Subtracts the 64 bit number in mVar2 from mVar1
		push	mLC									; Sets SREG flags for action after call
		push	var10
		push	var11
		push	var12
		push	var13
		push	var14
		push	var15
		push	var16
		push	var17
		Load	var10, mVar1_b0
		Load	var11, mVar1_b1
		Load	var12, mVar1_b2
		Load	var13, mVar1_b3
		Load	var14, mVar1_b4
		Load	var15, mVar1_b5
		Load	var16, mVar1_b6
		Load	var17, mVar1_b7
		Load	mLC, mVar2_b0
		sub		var10, mLC
		Load	mLC, mVar2_b1
		sbc		var11, mLC
		Load	mLC, mVar2_b2
		sbc		var12, mLC
		Load	mLC, mVar2_b3
		sbc		var13, mLC
		Load	mLC, mVar2_b4
		sbc		var14, mLC
		Load	mLC, mVar2_b5
		sbc		var15, mLC
		Load	mLC, mVar2_b6
		sbc		var16, mLC
		Load	mLC, mVar2_b7
		sbc		var17, mLC
		Store	mVar1_b0, var10
		Store	mVar1_b1, var11
		Store	mVar1_b2, var12
		Store	mVar1_b3, var13
		Store	mVar1_b4, var14
		Store	mVar1_b5, var15
		Store	mVar1_b6, var16
		Store	mVar1_b7, var17
		pop		var17
		pop		var16
		pop		var15
		pop		var14
		pop		var13
		pop		var12
		pop		var11
		pop		var10
		pop		mLC
		ret	
.ENDIF

.IF m_UseSub32
	mSub32:											; Subtracts the 32 bit number in mVar2 from mVar1
		push	mLC									; Sets SREG flags for action after call
		push	var10
		push	var11
		push	var12
		push	var13
		Load	var10, mVar1_b0
		Load	var11, mVar1_b1
		Load	var12, mVar1_b2
		Load	var13, mVar1_b3
		Load	mLC, mVar2_b0
		sub		var10, mLC
		Load	mLC, mVar2_b1
		sbc		var11, mLC
		Load	mLC, mVar2_b2
		sbc		var12, mLC
		Load	mLC, mVar2_b3
		sbc		var13, mLC
		Store	mVar1_b0, var10
		Store	mVar1_b1, var11
		Store	mVar1_b2, var12
		Store	mVar1_b3, var13
		pop		var13
		pop		var12
		pop		var11
		pop		var10
		pop		mLC
		ret	
.ENDIF

.IF m_UseSub16
	mSub16:											; Subtracts the 16 bit number in mVar2 from mVar1
		push	mLC									; Sets SREG flags for action after call
		push	var10
		push	var11
		Load	var10, mVar1_b0
		Load	var11, mVar1_b1
		Load	mLC, mVar2_b0
		sub		var10, mLC
		Load	mLC, mVar2_b1
		sbc		var11, mLC
		Store	mVar1_b0, var10
		Store	mVar1_b1, var11
		pop		var11
		pop		var10
		pop		mLC
		ret	
.ENDIF

.IF m_UseSub8
	mSub8:											; Subtracts the 8 bit number in mVar2 from mVar1
		push	mLC									; Sets SREG flags for action after call
		push	var10
		Load	var10, mVar1_b0
		Load	mLC, mVar2_b0
		sub		var10, mLC
		Store	mVar1_b0, var10
		pop		var10
		pop		mLC
		ret	
.ENDIF


;****************************************************************************
;							 Multiplication Routines
;****************************************************************************

;-----------------------------------------------------------------------------:
; 8bit x 8bit unsigned multiply
;
; Multiplies the first byte of mVar1 and mVar2 and returns a 2 byte result

.IF m_UseMul8
	.IF m_UseHWmul
		mul8:								; hardware multiplication routine
			push	mulL
			push	mulH
			Load	mulL, mVar1_b0
			Load	mulH, mVar2_b0
			mul		mulL, mulH
			Store	mVar1_b0, mulL
			Store	mVar1_b1, mulH
			pop		mulH
			pop		mulL
			ret
	.ELSE
		mul8:								; software multiplication routine
			push	var10
			push	var11
			push	var20
			push	mLC
			Load	var10, mVar1_b0
			Load	var20, mVar2_b0
			sub		var11, var11		;initialize variables
			ldi		mLC, 9				; mLC = 9;
		_mul8L0:	
			brcc	_mul8L1			;---- calcurating loop
			add		var11, var20			;
		_mul8L1:
			ror		var11				;
			ror		var10				;
			dec		mLC					;if (--mLC > 0)
			brne	_mul8L0			; continue loop;
			Store	mVar1_b0, var10
			Store	mVar1_b1, var11
			pop		mLC
			pop		var20
			pop		var11
			pop		var10
			ret
	.ENDIF
.ENDIF

;-----------------------------------------------------------------------------:
; 16bit x 16bit unsigned multiply
;
; Multiplies the first 2 bytes of mVar1 and mVar2 and returns a 4 byte result

.IF m_UseMul16
	.IF m_UseHWmul
		mul16:								; hardware multiplication routine
			push	mTmp
			push	mulL
			push	mulH
			push	var10
			push	var11
			push	var12
			push	var13
			clr		mTmp
			Load	mulL, mVar1_b0
			Load	mulH, mVar2_b0
			mul		mulL, mulH
			movw	var11:var10, mulH:mulL
			Load	mulL, mVar1_b1
			Load	mulH, mVar2_b1
			mul		mulL, mulH
			movw	var13:var12, mulH:mulL
			Load	mulL, mVar1_b0
			Load	mulH, mVar2_b1
			mul		mulL, mulH
			add		var11, mulL
			adc		var12, mulH
			adc		var13, mTmp
			Load	mulL, mVar1_b1
			Load	mulH, mVar2_b0
			mul		mulL, mulH
			add		var11, mulL
			adc		var12, mulH
			adc		var13, mTmp
			Store	mVar1_b0, var10
			Store	mVar1_b1, var11
			Store	mVar1_b2, var12
			Store	mVar1_b3, var13
			pop		var13
			pop		var12
			pop		var11
			pop		var10
			pop		mulH
			pop		mulL
			pop		mTmp
			ret
	.ELSE
		mul16:								; software multiplication routine
			push	var10
			push	var11
			push	var12
			push	var13
			push	var20
			push	var21
			push	mLC
			Load	var10, mVar1_b0
			Load	var11, mVar1_b1
			Load	var20, mVar2_b0
			Load	var21, mVar2_b1
			sub		var13, var13			;initialize variables
			sub		var12, var12			;
			ldi		mLC, 17					; mLC = 17;
		_mul16L0:	
			brcc	_mul16L1				;---- calcurating loop
			add		var12, var20			;
			adc		var13, var21			;
		_mul16L1:	
			ror		var13					;
			ror		var12					;
			ror		var11					;
			ror		var10					;
			dec		mLC						;if (--mLC > 0)
			brne	_mul16L0				; continue loop;
			Store	mVar1_b0, var10
			Store	mVar1_b1, var11
			Store	mVar1_b2, var12
			Store	mVar1_b3, var13
			pop		mLC
			pop		var21
			pop		var20
			pop		var13
			pop		var12
			pop		var11
			pop		var10
			ret
	.ENDIF
.ENDIF

;-----------------------------------------------------------------------------:
; 32bit x 32bit unsigned multiply
;
; Multiplies the first 4 bytes of mVar1 and mVar2 and returns a 8 byte result

.IF m_UseMul32
	.IF m_UseHWmul
		mul32:								; hardware multiplication routine
			push	mTmp
			push	mulL
			push	mulH
			push	var10
			push	var11
			push	var12
			push	var13
			push	var14
			push	var15
			push	var16
			push	var17
			push	var20
			push	var21
			push	var22
			push	var23
			push	var24
			push	var25
			push	var26
			push	var27
			Load	mTmp, mVar2_b0			;1
			Load	mulL, mVar1_b0			;A
			mul		mulL, mTmp
			movw	var11:var10, mulH:mulL	;1A
			Load	mulL, mVar1_b2			;C
			mul		mulL, mTmp
			movw	var13:var12, mulH:mulL	;1C
			Load	mTmp, mVar1_b3			;D
			Load	mulL, mVar2_b1			;2
			mul		mulL, mTmp
			movw	var15:var14, mulH:mulL	;2D
			Load	mulL, mVar2_b3			;4
			mul		mulL, mTmp
			movw	var17:var16, mulH:mulL	;4D
			clr		var20
			clr		var27
			Load	mulL, mVar2_b2			;3	(D already in mTmp)
			mul		mulL, mTmp
			mov		var26, mulH
			mov		var25, mulL				;3D
			Load	mulL, mVar2_b0			;1
			mul		mulL, mTmp
			mov		var24, mulH
			mov		var23, mulL				;1D
			Load	mulL, mVar2_b0			;1
			Load	mulH, mVar1_b1			;B
			mul		mulL, mulH
			mov		var22, mulH
			mov		var21, mulL				;1B
			rcall	_mul32Add
			Load	mTmp, mVar2_b1			;2
			Load	mulL, mVar1_b0			;A
			mul		mulL, mTmp
			mov		var22, mulH
			mov		var21, mulL				;2A
			Load	mulL, mVar1_b2			;C
			mul		mulL, mTmp
			mov		var24, mulH
			mov		var23, mulL				;2C
			Load	mulL, mVar1_b2			;C
			Load	mulH, mVar2_b3			;4
			mul		mulL, mulH
			mov		var26, mulH
			mov		var25, mulL				;4C
			rcall	_mul32Add
			clr		var21
			clr		var26
			Load	mTmp, mVar2_b2			;3
			Load	mulL, mVar1_b0			;A
			mul		mulL, mTmp
			movw	var23:var22, mulH:mulL	;3A
			Load	mulL, mVar1_b2			;C
			mul		mulL, mTmp
			movw	var25:var24, mulH:mulL	;3C
			rcall	_mul32Add
			Load	mTmp, mVar1_b1			;B
			Load	mulL, mVar2_b1			;2
			mul		mulL, mTmp
			movw	var23:var22, mulH:mulL	;2B
			Load	mulL, mVar2_b3			;4
			mul		mulL, mTmp
			movw	var25:var24, mulH:mulL	;4B
			rcall	_mul32Add
			clr		var22
			clr		var25
			Load	mulL, mVar2_b2			;3
			Load	mulH, mVar1_b1			;B
			mul		mulL, mulH				;3B
			mov		var24, mulH
			mov		var23, mulL
			rcall	_mul32Add
			Load	mulL, mVar2_b3			;4
			Load	mulH, mVar1_b0			;A
			mul		mulL, mulH				;4A
			mov		var24, mulH
			mov		var23, mulL
			rcall	_mul32Add
			Store	mVar1_b0, var10
			Store	mVar1_b1, var11
			Store	mVar1_b2, var12
			Store	mVar1_b3, var13
			Store	mVar1_b4, var14
			Store	mVar1_b5, var15
			Store	mVar1_b6, var16
			Store	mVar1_b7, var17
			pop		var27
			pop		var26
			pop		var25
			pop		var24
			pop		var23
			pop		var22
			pop		var21
			pop		var20
			pop		var17
			pop		var16
			pop		var15
			pop		var14
			pop		var13
			pop		var12
			pop		var11
			pop		var10
			pop		mulH
			pop		mulL
			pop		mTmp
			ret
		_mul32Add:
			add		var10, var20
			adc		var11, var21
			adc		var12, var22
			adc		var13, var23
			adc		var14, var24
			adc		var15, var25
			adc		var16, var26
			adc		var17, var27
			ret
	.ELSE
		mul32:								; software multiplication routine
			push	var10
			push	var11
			push	var12
			push	var13
			push	var14
			push	var15
			push	var16
			push	var17
			push	var20
			push	var21
			push	var22
			push	var23
			push	mLC
			Load	var10, mVar1_b0
			Load	var11, mVar1_b1
			Load	var12, mVar1_b2
			Load	var13, mVar1_b3
			Load	var20, mVar2_b0
			Load	var21, mVar2_b1
			Load	var22, mVar2_b2
			Load	var23, mVar2_b3
			sub		var17, var17		;initialize variables
			sub		var16, var16		;
			sub		var15, var15		;
			sub		var14, var14		;
			ldi		mLC, 33				; mLC = 33;
		_mul32L0:
			brcc	_mul32L1			;---- calcurating loop
			add		var14, var20		;
			adc		var15, var21		;
			adc		var16, var22		;
			adc		var17, var23		;
		_mul32L1:		
			ror		var17				;
			ror		var16				;
			ror		var15				;
			ror		var14				;
			ror		var13				;
			ror		var12				;
			ror		var11				;
			ror		var10				;
			dec		mLC					;if (--mLC > 0)
			brne	_mul32L0			; continue loop;
			Store	mVar1_b0, var10
			Store	mVar1_b1, var11
			Store	mVar1_b2, var12
			Store	mVar1_b3, var13
			Store	mVar1_b4, var14
			Store	mVar1_b5, var15
			Store	mVar1_b6, var16
			Store	mVar1_b7, var17
			pop		mLC
			pop		var23
			pop		var22
			pop		var21
			pop		var20
			pop		var17
			pop		var16
			pop		var15
			pop		var14
			pop		var13
			pop		var12
			pop		var11
			pop		var10
			ret
	.ENDIF
.ENDIF


;****************************************************************************
;							 Division Routines
;
;	NOTE:   All division routines can be called either by "div##" or "div##r"
;			When called via "div##r", the routine (if enabled by the conditional
;			assemble switch above) will automatically round the result returned
;			in mVar1 if the remainder is => 1/2 the divisor.
;****************************************************************************


;-----------------------------------------------------------------------------:
; 8bit/8bit Unsigned Division
;
; Divides the 1 byte value in mVar1 by the 1 byte value in mVar2, returns the result
; in mVar1 and the remainder in mMod

.IF	m_UseDiv8
	.IF m_UseRound
		div8r:
			set
			rjmp	_div8Start
	.ENDIF
	div8:
		clt
	_div8Start:
		push	var10
		push	var20
		push	mod0
		push	mLC
		Load	var10, mVar1_b0
		Load	var20, mVar2_b0
		clr		mod0				;initialize variables
		ldi		mLC, 8				;  mod = 0; mLC = 8;
	_div8L0:						;---- calcurating loop
		lsl		var10				;var1 = var1 << 1;
		rol		mod0				;mod = mod << 1 + carry;
		cp		mod0, var20			;if (mod => var2) {
		brcs	_div8L1				; mod -= var2; var1++;
		inc		var10				; }
		sub		mod0, var20			;/
	_div8L1:		
		dec		mLC					;if (--mLC > 0)
		brne	_div8L0				; continue loop;
		rcall	_CLRMod
		Store	mMod_b0, mod0
		.IF m_UseRound
			brtc	_div8End
			lsr		var20
			Load	mLC, mMod_b0				; compare to mMod
			cp		mLC, var20
			brcs	_div16End					; skip if rounding not needed
			ldi		mLC, 1
			add		var10, mLC
		.ENDIF
	_div8End:
		Store	mVar1_b0, var10
		pop		mLC
		pop		mod0
		pop		var20
		pop		var10
		ret
.ENDIF

;-----------------------------------------------------------------------------:
; 16bit/16bit Unsigned Division
;
; Divides the 2 byte value in mVar1 by the 2 byte value in mVar2, returns the result
; in mVar1 and the remainder in mMod

.IF	m_UseDiv16
	.IF m_UseRound
		div16r:
			set
			rjmp	_div16Start
	.ENDIF
	div16:
		clt
	_div16Start:
		push	var10
		push	var11
		push	var20
		push	var21
		push	mod0
		push	mod1
		push	mLC
		Load	var10, mVar1_b0
		Load	var11, mVar1_b1
		Load	var20, mVar2_b0
		Load	var21, mVar2_b1
		clr		mod0				;initialize variables
		clr		mod1				;  mod = 0;
		ldi		mLC, 16				;  mLC = 16;
	_div16L0:						;---- calcurating loop
		lsl		var10				;var1 = var1 << 1;
		rol		var11				;/
		rol		mod0				;mod = mod << 1 + carry;
		rol		mod1				;/
		cp		mod0, var20			;if (mod => var2) {
		cpc		mod1, var21			; mod -= var2; var1++;
		brcs	_div16L1			; }
		inc		var10				;
		sub		mod0, var20			;
		sbc		mod1, var21			;/
	_div16L1:
		dec		mLC					;if (--mLC > 0)
		brne	_div16L0			; continue loop;
		rcall	_CLRMod
		Store	mMod_b0, mod0
		Store	mMod_b1, mod1
		.IF m_UseRound
			brtc	_div16End
			lsr		var21
			ror		var20
			Load	mLC, mMod_b0				; compare to mMod
			cp		mLC, var20
			Load	mLC, mMod_b1
			cpc		mLC, var21
			brcs	_div16End					; skip if rounding not needed
			ldi		mLC, 1
			add		var10, mLC
			clr		mLC
			adc		var11, mLC
		.ENDIF
	_div16End:
		Store	mVar1_b0, var10
		Store	mVar1_b1, var11
		pop		mLC
		pop		mod1
		pop		mod0
		pop		var21
		pop		var20
		pop		var11
		pop		var10
		ret
.ENDIF

;-----------------------------------------------------------------------------:
; 32bit/32bit Unsigned Division
;
; Divides the 4 byte value in mVar1 by the 4 byte value in mVar2, returns the result
; in mVar1 and the remainder in mMod

.IF	m_UseDiv32
	.IF m_UseRound
		div32r:
			set
			rjmp	_div32Start
	.ENDIF
	div32:
		clt
	_div32Start:
		push	var10
		push	var11
		push	var12
		push	var13
		push	var20
		push	var21
		push	var22
		push	var23
		push	mod0
		push	mod1
		push	mod2
		push	mod3
		push	mLC
		Load	var10, mVar1_b0
		Load	var11, mVar1_b1
		Load	var12, mVar1_b2
		Load	var13, mVar1_b3
		Load	var20, mVar2_b0
		Load	var21, mVar2_b1
		Load	var22, mVar2_b2
		Load	var23, mVar2_b3
		clr		mod0				;initialize variables
		clr		mod1				;  mod = 0;
		clr		mod2				;  mLC = 32;
		clr		mod3				;
		ldi		mLC, 32				;/
	_div32L0:						;---- calcurating loop
		lsl		var10				;var1 = var1 << 1;
		rol		var11				;
		rol		var12				;
		rol		var13				;/
		rol		mod0				;mod = mod << 1 + carry;
		rol		mod1				;
		rol		mod2				;
		rol		mod3				;/
		cp		mod0, var20			;if (mod => var2) {
		cpc		mod1, var21			; mod -= var2; var1++;
		cpc		mod2, var22			; }
		cpc		mod3, var23			;
		brcs	_div32L1			;
		inc		var10				;
		sub		mod0, var20			;
		sbc		mod1, var21			;
		sbc		mod2, var22			;
		sbc		mod3, var23			;/
	_div32L1:
		dec		mLC					;if (--mLC > 0)
		brne	_div32L0			; continue loop;
		rcall	_CLRMod
		Store	mMod_b0, mod0
		Store	mMod_b1, mod1
		Store	mMod_b2, mod2
		Store	mMod_b3, mod3
		.IF m_UseRound
			brtc	_div32End
			lsr		var23
			ror		var22
			ror		var21
			ror		var20
			Load	mLC, mMod_b0				; compare to mMod
			cp		mLC, var20
			Load	mLC, mMod_b1
			cpc		mLC, var21
			Load	mLC, mMod_b2
			cpc		mLC, var22
			Load	mLC, mMod_b3
			cpc		mLC, var23
			brcs	_div32End					; skip if rounding not needed
			ldi		mLC, 1
			add		var10, mLC
			clr		mLC
			adc		var11, mLC
			adc		var12, mLC
			adc		var13, mLC
		.ENDIF
	_div32End:
		Store	mVar1_b0, var10
		Store	mVar1_b1, var11
		Store	mVar1_b2, var12
		Store	mVar1_b3, var13
		pop		mLC
		pop		mod3
		pop		mod2
		pop		mod1
		pop		mod0
		pop		var23
		pop		var22
		pop		var21
		pop		var20
		pop		var13
		pop		var12
		pop		var11
		pop		var10
		ret
.ENDIF

;-----------------------------------------------------------------------------:
; 64bit/64bit Unsigned Division
;
; Divides the 8 byte value in mVar1 by the 8 byte value in mVar2, returns the result
; in mVar1 and the remainder in mMod


.IF	m_UseDiv64 
	.IF m_UseRound
		div64r:
			set
			rjmp	_div64Start
	.ENDIF
	div64:
		clt
	_div64Start:
		push	var10
		push	var11
		push	var12
		push	var13
		push	var14
		push	var15
		push	var16
		push	var17
		push	var20
		push	var21
		push	var22
		push	var23
		push	var24
		push	var25
		push	var26
		push	var27
		push	mod0
		push	mod1
		push	mod2
		push	mod3
		push	mod4
		push	mod5
		push	mod6
		push	mod7
		push	mLC
		Load	var10, mVar1_b0
		Load	var11, mVar1_b1
		Load	var12, mVar1_b2
		Load	var13, mVar1_b3
		Load	var14, mVar1_b4
		Load	var15, mVar1_b5
		Load	var16, mVar1_b6
		Load	var17, mVar1_b7
		Load	var20, mVar2_b0
		Load	var21, mVar2_b1
		Load	var22, mVar2_b2
		Load	var23, mVar2_b3
		Load	var24, mVar2_b4
		Load	var25, mVar2_b5
		Load	var26, mVar2_b6
		Load	var27, mVar2_b7
		clr		mod0				;initialize variables
		clr		mod1				;  mod = 0;
		clr		mod2				;  mLC = 32;
		clr		mod3				;
		clr		mod4
		clr		mod5
		clr		mod6
		clr		mod7
		ldi		mLC,64				;/
	_div64L0:						;---- calcurating loop
		lsl		var10				;var1 = var1 << 1;
		rol		var11				;
		rol		var12				;
		rol		var13				;/
		rol		var14
		rol		var15
		rol		var16
		rol		var17
		rol		mod0				;mod = mod << 1 + carry;
		rol		mod1				;
		rol		mod2				;
		rol		mod3				;/
		rol		mod4
		rol		mod5
		rol		mod6
		rol		mod7
		cp		mod0, var20			;if (mod => var2) {
		cpc		mod1, var21			; mod -= var2; var1++;
		cpc		mod2, var22			; }
		cpc		mod3, var23			;
		cpc		mod4, var24
		cpc		mod5, var25
		cpc		mod6, var26
		cpc		mod7, var27
		brcs	_div64L1			;
		inc		var10				;
		sub		mod0, var20			;
		sbc		mod1, var21			;
		sbc		mod2, var22			;
		sbc		mod3, var23			;/
		sbc		mod4, var24
		sbc		mod5, var25
		sbc		mod6, var26
		sbc		mod7, var27
	_div64L1:
		dec		mLC					;if (--mLC > 0)
		brne	_div64L0			; continue loop;
		Store	mMod_b0, mod0
		Store	mMod_b1, mod1
		Store	mMod_b2, mod2
		Store	mMod_b3, mod3
		Store	mMod_b4, mod4
		Store	mMod_b5, mod5
		Store	mMod_b6, mod6
		Store	mMod_b7, mod7
		.IF m_UseRound
			brtc	_div64End
			lsr		var27								; divide by 2
			ror		var26
			ror		var25
			ror		var24
			ror		var23
			ror		var22
			ror		var21
			ror		var20
			Load	mLC, mMod_b0						; compare to mMod
			cp		mLC, var20
			Load	mLC, mMod_b1
			cpc		mLC, var21
			Load	mLC, mMod_b2
			cpc		mLC, var22
			Load	mLC, mMod_b3
			cpc		mLC, var23
			Load	mLC, mMod_b4
			cpc		mLC, var24
			Load	mLC, mMod_b5
			cpc		mLC, var25
			Load	mLC, mMod_b6
			cpc		mLC, var26
			Load	mLC, mMod_b7
			cpc		mLC, var27
			brcs	_div64End					; skip if rounding not needed
			ldi		mLC, 1
			add		var10, mLC
			clr		mLC
			adc		var11, mLC
			adc		var12, mLC
			adc		var13, mLC
			adc		var14, mLC
			adc		var15, mLC
			adc		var16, mLC
			adc		var17, mLC
		.ENDIF
	_div64End:
		Store	mVar1_b0, var10
		Store	mVar1_b1, var11
		Store	mVar1_b2, var12
		Store	mVar1_b3, var13
		Store	mVar1_b4, var14
		Store	mVar1_b5, var15
		Store	mVar1_b6, var16
		Store	mVar1_b7, var17
		pop		mLC
		pop		mod7
		pop		mod6
		pop		mod5
		pop		mod4
		pop		mod3
		pop		mod2
		pop		mod1
		pop		mod0
		pop		var27
		pop		var26
		pop		var25
		pop		var24
		pop		var23
		pop		var22
		pop		var21
		pop		var20
		pop		var17
		pop		var16
		pop		var15
		pop		var14
		pop		var13
		pop		var12
		pop		var11
		pop		var10
		ret
.ENDIF


;==============================================================================================
CLRmVar1:									; Clears all 8 bytes of mVar1
	push	mLC
	clr		mLC
	Store	mVar1_b0, mLC
	Store	mVar1_b1, mLC
	Store	mVar1_b2, mLC
	Store	mVar1_b3, mLC
	Store	mVar1_b4, mLC
	Store	mVar1_b5, mLC
	Store	mVar1_b6, mLC
	Store	mVar1_b7, mLC
	pop		mLC
	ret

;==============================================================================================
CLRmVar2:									; Clears all 8 bytes of mVar2
	push	mLC
	clr		mLC
	Store	mVar2_b0, mLC
	Store	mVar2_b1, mLC
	Store	mVar2_b2, mLC
	Store	mVar2_b3, mLC
	Store	mVar2_b4, mLC
	Store	mVar2_b5, mLC
	Store	mVar2_b6, mLC
	Store	mVar2_b7, mLC
	pop		mLC
	ret

;==============================================================================================
_CLRMod:									; Clears all 8 bytes of mMod
	push	mLC
	clr		mLC
	Store	mMod_b0, mLC
	Store	mMod_b1, mLC
	Store	mMod_b2, mLC
	Store	mMod_b3, mLC
	Store	mMod_b4, mLC
	Store	mMod_b5, mLC
	Store	mMod_b6, mLC
	Store	mMod_b7, mLC
	pop		mLC
	ret

;****************************************************************************
;							 Swap Routines
;****************************************************************************

;==============================================================================================

.IF m_UseSwap
	SwapmVar1mVar2:									; Swaps mVar1 with mVar2
		push	mTmp
		push	mLC
		Load	mTmp, mVar1_b0
		Load	mLC, mVar2_b0
		Store	mVar1_b0, mLC
		Store	mVar2_b0, mTmp
		Load	mTmp, mVar1_b1
		Load	mLC, mVar2_b1
		Store	mVar1_b1, mLC
		Store	mVar2_b1, mTmp
		Load	mTmp, mVar1_b2
		Load	mLC, mVar2_b2
		Store	mVar1_b2, mLC
		Store	mVar2_b2, mTmp
		Load	mTmp, mVar1_b3
		Load	mLC, mVar2_b3
		Store	mVar1_b3, mLC
		Store	mVar2_b3, mTmp
		Load	mTmp, mVar1_b4
		Load	mLC, mVar2_b4
		Store	mVar1_b4, mLC
		Store	mVar2_b4, mTmp
		Load	mTmp, mVar1_b5
		Load	mLC, mVar2_b5
		Store	mVar1_b5, mLC
		Store	mVar2_b5, mTmp
		Load	mTmp, mVar1_b6
		Load	mLC, mVar2_b6
		Store	mVar1_b6, mLC
		Store	mVar2_b6, mTmp
		Load	mTmp, mVar1_b7
		Load	mLC, mVar2_b7
		Store	mVar1_b7, mLC
		Store	mVar2_b7, mTmp
		pop		mLC
		pop		mTmp
		ret
.ENDIF


;****************************************************************************
;							 Copy Routines
;****************************************************************************

;==============================================================================================

.IF m_UseCopy
	CopymVar1to2:									; Copies mVar1 to mVar2
		push	mTmp
		Load	mTmp, mVar1_b0
		Store	mVar2_b0, mTmp
		Load	mTmp, mVar1_b1
		Store	mVar2_b1, mTmp
		Load	mTmp, mVar1_b2
		Store	mVar2_b2, mTmp
		Load	mTmp, mVar1_b3
		Store	mVar2_b3, mTmp
		Load	mTmp, mVar1_b4
		Store	mVar2_b4, mTmp
		Load	mTmp, mVar1_b5
		Store	mVar2_b5, mTmp
		Load	mTmp, mVar1_b6
		Store	mVar2_b6, mTmp
		Load	mTmp, mVar1_b7
		Store	mVar2_b7, mTmp
		pop		mTmp
		ret
.ENDIF
;==============================================================================================

.IF m_UseCopy
	CopymVar2to1:									; Copies mVar2 to mVar1
		push	mTmp
		Load	mTmp, mVar2_b0
		Store	mVar1_b0, mTmp
		Load	mTmp, mVar2_b1
		Store	mVar1_b1, mTmp
		Load	mTmp, mVar2_b2
		Store	mVar1_b2, mTmp
		Load	mTmp, mVar2_b3
		Store	mVar1_b3, mTmp
		Load	mTmp, mVar2_b4
		Store	mVar1_b4, mTmp
		Load	mTmp, mVar2_b5
		Store	mVar1_b5, mTmp
		Load	mTmp, mVar2_b6
		Store	mVar1_b6, mTmp
		Load	mTmp, mVar2_b7
		Store	mVar1_b7, mTmp
		pop		mTmp
		ret
.ENDIF

;****************************************************************************
;							 Compare Routines
;****************************************************************************

;==============================================================================================

.IF m_UseComp64
	mComp64:										; Performs an 64 bit compare between mVar1 and mVar2
		push	var10								; Sets SREG flags for action after call
		push	var20
		Load	var10, mVar1_b0
		Load	var20, mVar2_b0
		cp		Var10, Var20
		Load	var10, mVar1_b1
		Load	var20, mVar2_b1
		cpc		Var10, Var20
		Load	var10, mVar1_b2
		Load	var20, mVar2_b2
		cpc		Var10, Var20
		Load	var10, mVar1_b3
		Load	var20, mVar2_b3
		cpc		Var10, Var20
		Load	var10, mVar1_b4
		Load	var20, mVar2_b4
		cpc		Var10, Var20
		Load	var10, mVar1_b5
		Load	var20, mVar2_b5
		cpc		Var10, Var20
		Load	var10, mVar1_b6
		Load	var20, mVar2_b6
		cpc		Var10, Var20
		Load	var10, mVar1_b7
		Load	var20, mVar2_b7
		cpc		Var10, Var20
		pop		var20
		pop		var10
		ret
.ENDIF



;==============================================================================================
.IF m_UseComp32
	mComp32:									; Performs an 32 bit compare between mVar1 and mVar2
		push	var10							; Sets SREG flags for action after call
		push	var20
		Load	var10, mVar1_b0
		Load	var20, mVar2_b0
		cp		Var10, Var20
		Load	var10, mVar1_b1
		Load	var20, mVar2_b1
		cpc		Var10, Var20
		Load	var10, mVar1_b2
		Load	var20, mVar2_b2
		cpc		Var10, Var20
		Load	var10, mVar1_b3
		Load	var20, mVar2_b3
		cpc		Var10, Var20
		pop		var20
		pop		var10
		ret
.ENDIF



;==============================================================================================
.IF m_UseComp16
	mComp16:										; Performs an 16 bit compare between mVar1 and mVar2
		push	var10								; Sets SREG flags for action after call
		push	var20
		Load	var10, mVar1_b0
		Load	var20, mVar2_b0
		cp		Var10, Var20
		Load	var10, mVar1_b1
		Load	var20, mVar2_b1
		cpc		Var10, Var20
		pop		var20
		pop		var10
		ret
.ENDIF



;==============================================================================================
.IF m_UseComp8
	mComp8:											; Performs an 8 bit compare between mVar1 and mVar2
		push	var10								; Sets SREG flags for action after call
		push	var20
		Load	var10, mVar1_b0
		Load	var20, mVar2_b0
		cp		Var10, Var20
		pop		var20
		pop		var10
		ret
.ENDIF


