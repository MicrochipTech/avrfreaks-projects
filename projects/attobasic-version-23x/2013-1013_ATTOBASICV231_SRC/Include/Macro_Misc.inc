#if ((__CORE_VERSION__ == V2E )) || ((__CORE_VERSION__ == V3 ))
.MACRO Copy16
	movw	@0:@1,@2:@3
.ENDMACRO

.MACRO CopyZtoX
	movw	XH:XL,ZH:ZL
.ENDMACRO

.MACRO CopyXtoZ
	movw	ZH:ZL,XH:XL
.ENDMACRO

.MACRO CopyZtoY
	movw	YH:YL,ZH:ZL
.ENDMACRO

.MACRO CopyYtoZ
	movw	ZH:ZL,YH:YL
.ENDMACRO

.MACRO CopyYtoX
	movw	XH:XL,YH:YL
.ENDMACRO

.MACRO CopyXtoY
	movw	YH:YL,XH:XL
.ENDMACRO

#else	;older cores prior to V1
.MACRO Copy16
	mov		@1,@3
	mov		@0,@2
.ENDMACRO

.MACRO CopyZtoX
	mov		XL,ZL
	mov		XH,ZH
.ENDMACRO

.MACRO CopyXtoZ
	mov	ZL,XL
	mov	ZH,XH
.ENDMACRO

.MACRO CopyZtoY
	mov	YL,ZL
	mov	YH,ZH
.ENDMACRO

.MACRO CopyYtoZ
	mov	ZL,YL
	mov	ZH,YH
.ENDMACRO

.MACRO CopyYtoX
	mov	XL,YL
	mov	XH,YH
.ENDMACRO

.MACRO CopyXtoY
	mov	YL,XL
	mov	YH,XH
.ENDMACRO

#endif

.MACRO BCD2ASC		;convert BCD digit in @0 to ASCII
	andi	@0,0x0F				;strip of MSD
	subi	@0,-0x30			;add 0x30 for ASCII
.ENDMACRO

.MACRO m_SwapReg	; Swap value in register (Assume: @0=a, @1=b)
	eor @0,@1		; @0 := @0 xor @1    (@0: a -> ab
	eor @1,@0		; @1 := @1 xor @0    (@1: b -> bab -> a)
	eor @0,@1		; @0 := @0 xor @1    (@0  ab -> aba ->b)
.ENDMACRO 

.MACRO m_SwapXY
	eor YL,XL  ; assembler does not allow using m_SwapReg in macro
	eor XL,YL
	eor YL,XL
	eor YH,XH  ; assembler does not allow using m_SwapReg in macro
	eor XH,YH
	eor YH,XH
.ENDMACRO 

.MACRO m_SwapYZ
	eor YL,ZL  ; assembler does not allow using m_SwapReg in macro
	eor ZL,YL
	eor YL,ZL
	eor YH,ZH  ; assembler does not allow using m_SwapReg in macro
	eor ZH,YH
	eor YH,ZH
.ENDMACRO 

.MACRO SetZPtr ;(adress)
	ldi	ZL,low(@0)
	ldi	ZH,high(@0)
.ENDMACRO

.MACRO SetXPtr ;(adress)
	ldi	XL,low(@0)
	ldi	XH,high(@0)
.ENDMACRO

.MACRO SetYPtr ;(adress)
	ldi	YL,low(@0)
	ldi	YH,high(@0)
.ENDMACRO

.MACRO PushStat	;save the SREG on the stack, uses r16
	in		r16,SREG
	push	r16
.ENDMACRO

.MACRO PopStat	;restores the SREG from the stack, uses r16
	pop		r16
	out		SREG,r16
.ENDMACRO

.MACRO PushX
	push	XL
	push	XH
.ENDMACRO

.MACRO PopX
	pop	XH
	pop	XL
.ENDMACRO

.MACRO PushY
	push	YL
	push	YH
.ENDMACRO

.MACRO PopY
	pop	YH
	pop	YL
.ENDMACRO

.MACRO PushZ
	push	ZL
	push	ZH
.ENDMACRO

.MACRO PopZ
	pop	ZH
	pop	ZL
.ENDMACRO

.MACRO Add32	; Add32 U3,U2,U1,U0,V3,V2,V1,V0  U:=U+V
	add	@3,@7
	adc	@2,@6
	adc	@1,@5
	adc	@0,@4
.ENDMACRO

.MACRO Add24	; Add24 UH,UM,UL,VH,VM,VL  U:=U+V
	add	@2,@5
	adc	@1,@4
	adc	@0,@3
.ENDMACRO

;16bit - from App note 202

.MACRO Add16	; Add16 UH,UL,VH,VL      U:=U+V
	add	@1,@3
	adc	@0,@2
.ENDMACRO

.MACRO Addi16	; Addi16 UH,UL,immediate   U:=U+immediate
	subi	@1,low(-@2)
	sbci	@0,high(-@2)
.ENDMACRO

.MACRO Addi24	; Addi24 UH,UM,UL,immediate   U:=U+immediate
	subi	@2,low(-@3)
	sbci	@1,high(-@3)
	sbci	@0,byte3(-@3)
.ENDMACRO

.MACRO Addi32	; Addi32 U3,U2,U1,U0,immediate   U:=U+immediate
	subi	@3,low(-@4)
	sbci	@2,high(-@4)
	sbci	@1,byte3(-@4)
	sbci	@0,byte4(-@4)
.ENDMACRO

.MACRO Sub16	; Sub16 UH,UL,VH,VL	U:=U-V
	sub	@1,@3
	sbc	@0,@2
.ENDMACRO

.MACRO Subi16	; Subi16 UH,UL,immediate  U:=U-immediate
.IF @2 < 64
        sbiw    @0:@1,@2
.ELSE
        subi    @1,low(@2)
        sbci    @0,high(@2)
.ENDIF
.ENDMACRO

.MACRO Subi32	; Subi32 U3,U2,U1,U0,immediate   U:=U+immediate
	subi	@3,low(@4)
	sbci	@2,high(@4)
	sbci	@1,byte3(@4)
	sbci	@0,byte4(@4)
.ENDMACRO

.MACRO Cp16	; CP16 UH,UL,VH,VL	U-V
	cp	@1,@3
	cpc	@0,@2
.ENDMACRO

.MACRO Cpi16	; CPI16 UH,UL,immediate	U-immediate
	push r16
	cpi	@1,low(@2)
	ldi	r16,high(@2)
	cpc	@0,r16
	pop r16
.ENDMACRO

.MACRO Cp24	; CP16 UH,UM,UL,VH,VM,Vl	U-V
	cp	@2,@5
	cpc	@1,@4
	cpc	@0,@3
.ENDMACRO

.MACRO Com16	; Com16 UL,UH
	com	@0
	com	@1
.ENDMACRO

.MACRO Neg16	; Neg16 UL,UH
	com	@0
	com	@1
	subi	@0,low(-1)
	sbci	@1,high(-1)
.ENDMACRO

.MACRO GoSub	; substitute the appropriate call/rcall @0
.if (FLASHEND > 0x0fff)
	call	@0
.else
	rcall	@0
.endif
.ENDMACRO

.MACRO Goto		; substitute the appropriate jmp/rjmp @0
.if FLASHEND > 0x0fff
	jmp		@0
.else
	rjmp	@0
.endif
.ENDMACRO

.MACRO InitSP	; initialize stack pointer RegH, RegL
	ldi		r16,LOW(RAMEND)
	STORE	SPL,r16
.ifdef SPH
	ldi		r16,HIGH(RAMEND)
	STORE	SPH,r16
.endif
.ENDMACRO
;
;***********************************************************************
;COPY STRING FROM RAM TO RAM.  Uses X and Y pointer, R16 and R17.
;* RAM Source		: @0 (16-bit pointer)
;* RAM Destination	: @1 (16-bit pointer)
;* number of bytes	: @2 (8-bit)
;***********************************************************************
.MACRO COPYRAM		; CopyRAM	Source(16),Destination(16),count(8)
	ldi		XH,high(@0)
	ldi		XL,low(@0)
	ldi		YH,high(@0)
	ldi		YL,low(@0)
	ldi		r16,@2				;get fill value in r16
_COPYRAM1:
	ld		r17,X+				;get byte, increment source pointer
	st		Y+,r17				;save byte, increment destination pointer
	dec		r16					;decrement and test for zero
	brne	_COPYRAM1			;loop for more
.ENDMACRO
;
;***********************************************************************
;FILL THE RAM.  Uses Z pointer and R16
;* start location	: @0 (16-bit pointer)
;* end location		: @1 (16-bit)
;* fill value		: @2 (8-bit)
;***********************************************************************
.MACRO FillRAM
	ldi		ZH,high(@0)
	ldi		ZL,low(@0)
	ldi		r16,@2				;get fill value in r16

	st		Z+,r16				;out in SRAM and decrement X-pointer
	cpi		ZH,high(@1)
	brcs	PC-0x02				;if not end of table, loop more
	cpi		ZL,low(@1)
	brne	PC-0x04				;if not end of table, loop more
.ENDMACRO
