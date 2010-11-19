	
; sets the output stream to either null/serial/ram
.macro	stream
	push	temp1
	ldi	temp1,@0
	sts	stream_destination,temp1
	pop	temp1
.endmacro


.macro	stopRF
	ext0_OFF		; turn off int
	RFcommand RF_IDLE_MODE	; xtal on, (RX, TX, low-bat detect, clk out) all off
	clr	state		; set to zero
.endmacro

;=======================================
; macro pair for timed code blocks
.macro	msloop_for
	push	temp2
	ldi	temp2,@0
	mov	smalltime,temp2
	pop	temp2
.endmacro

.macro	msloop_to
	tst	smalltime
	breq	m1
	rjmp	@0
m1:
.endmacro
;=======================================


;=======================================
; macro pair for timed code blocks
.macro	csloop_for
	push	temp2
	ldi	temp2,@0
	mov	bigtime,temp2
	pop	temp2
.endmacro

.macro	csloop_to
	tst	bigtime
	breq	m1
	rjmp	@0
m1:
.endmacro
;=======================================


.macro	string
	.db	@0,0
.endmacro


; Jump if Equal
.macro	jeq
	cpi	@0,@1
	brne	m1
	rjmp	@2
m1:
.endmacro

; Jump if Not Equal
.macro	jne
	cpi	@0,@1
	brne	@2
.endmacro


; milliseconds delay. 8-bit
.macro	msdelay
	ldi	temp1,@0
	call	delay1
.endmacro

; centi-seconds delay (1/10th sec) 8-bit
.macro	csdelay
	ldi	temp1,@0
	rcall	delay2
.endmacro

; seconds delay 8-bit
.macro	secdelay
	ldi	temp1,@0
	rcall	delay3
.endmacro


; sends a string to the output stream
.macro	stringout
	call	sendstring
	.db	@0,0
.endmacro

; sends a CR/LF to the o/p stream
.macro	newline
	call	newln
.endmacro


; 
.macro	ldx
	.if @0==1
	ldi	xl,low(@1)
	ldi	xh,high(@1)
	.elif @0==2
	ldi	xl,low(2*@1)
	ldi	xh,high(2*@1)
	.else
	.error "must be ram or rom"
	.endif
.endmacro

.macro	ldy
	.if @0==1
	ldi	yl,low(@1)
	ldi	yh,high(@1)
	.elif @0==2
	ldi	yl,low(2*@1)
	ldi	yh,high(2*@1)
	.else
	.error "must be ram or rom"
	.endif
.endmacro

.macro	ldz
	.if @0==1
	ldi	ZL,low(@1)
	ldi	ZH,high(@1)
	.elif @0==2
	ldi	ZL,low(2*@1)
	ldi	ZH,high(2*@1)
	.else
	.error "must be ram or rom"
	.endif
.endmacro


.macro	loadX
	lds	xl,@0
	lds	xh,@0+1
.endmacro

.macro	storeX
	sts	@0,xl
	sts	@0+1,xh
.endmacro

.macro	loadY
	lds	yl,@0
	lds	yh,@0+1
.endmacro

.macro	storeY
	sts	@0+1,yl
	sts	@0+1,yh
.endmacro

.macro	loadZ
	lds	zl,@0
	lds	zh,@0+1
.endmacro

.macro	storeZ
	sts	@0,zl
	sts	@0+1,zh
.endmacro

; sends a single char to the output stream
.macro	send
	ldi	temp1,@0
	call	stream_out
.endmacro


; sends a command to the RFM12
.macro	RFcommand
	ldi	accH,high(@0)
	ldi	accL,low(@0)
	call	sendword
.endmacro


.macro	ext0_ON
	ldi	temp1,  (1<<INT0)
	out	EIFR, temp1		; clear any pending INT0 interrupt flag
	out	EIMSK,temp1		; enable external INT0
.endmacro

.macro	ext0_OFF
	clr	temp1
	out	EIMSK,temp1		; disable external INT0
.endmacro


.macro	error
	ldi	temp1,@0
	sts	errorcode,temp1
.endmacro




;************************************************************************/
;*                                                                      */
;*                      Delay Macro 8 ... 65543 Cycle                   */
;*                                                                      */
;*              Author: Peter Dannegger                                 */
;*                                                                      */
;************************************************************************/
;cydelay 8 ... 65543 cycle
.macro  cydelay
        ldi     r24, low( @0 - 8 )
        ldi     r25, high( @0 - 8 )
        sbiw    r24, 4
        brcc    pc - 1
        cpi     r24, 0xFD
        brcs    pc + 4
        breq    pc + 3
        cpi     r24, 0xFF
        breq    pc + 1
.endmacro










;*****************************************************************
;*	file: macros.inc
;*
;*	Description:
;*	Source file for application note AVR001 - Conditional Assembly
;*	and Portability Macros.
;*
;*	Defines a number of macros that makes it easier to access
;*	IO registers and extended IO registers (or SRAM locations up
;*  to adress $FF if applicable).
;*	The macros can be used to produce code that assembles to
;*	any target AVR, without considering if the accessed IO
;*	registers are located in low, standard or extended IO space
;*
;* $Revision: 2.2 $
;* $Author: jllassen $
;* $Date: Wednesday, January 26, 2005 10:55:18 UTC $
;*****************************************************************

;*********************************************************
;*	BIT access anywhere in IO or lower $FF of data space
;*	SETB - SET Bit in IO of data space
;*	CLRB - CLeaR Bit in IO of data space
;*********************************************************

.macro SETB 		;Arguments: Address, Bit, Register
	.if @1>7
		.message "Only values 0-7 allowed for Bit parameter"
	.endif
	.if @0>0x3F
		lds  @2, @0
		sbr  @2, (1<<@1)
		sts  @0, @2
	.elif @0>0x1F
		in   @2, @0
		sbr  @2, (1<<@1)
		out  @0, @2
	.else
		sbi  @0, @1
	.endif
.endmacro

.macro CLRB 		;Arguments: Address, Bit, Register
	.if @1>7
		.message "Only values 0-7 allowed for Bit parameter"
	.endif
	.if @0>0x3F
		lds  @2, @0
		cbr  @2, (1<<@1)
		sts  @0, @2
	.elif @0>0x1F
		in   @2, @0
		cbr  @2, (1<<@1)
		out  @0, @2
	.else
		cbi  @0, @1
	.endif
.endmacro

;*********************************************************
;*	Bit test anywhere in IO or in lower $FF of data space
;*  SKBS : SKip if Bit Set
;*  SKBC : SKip if Bit Cleared
;*********************************************************
.macro SKBS  		;Arguments: Address, Bit, Register
	.if @1>7
		.message "Only values 0-7 allowed for Bit parameter"
	.endif
	.if @0>0x3F
		lds  @2, @0
		sbrs @2, @1
	.elif @0>0x1F
		in   @2, @0
		sbrs @2, @1
	.else
		sbis @0, @1
	.endif
.endmacro

.macro SKBC  		;Arguments: Address, Bit, Register
	.if @1>7
		.message "Only values 0-7 allowed for Bit parameter"
	.endif
	.if @0>0x3F
		lds	 @2, @0
		sbrc @2, @1
	.elif @0>0x1F
		in	 @2, @0
		sbrc @2, @1
	.else
		sbic @0, @1
	.endif
.endmacro

;*********************************************************
;*	Byte access anywhere in IO or lower $FF of data space
;* 	STORE - Store register in IO or data space
;* 	LOAD  - Load register from IO or data space
;*********************************************************

.macro store 		;Arguments: Address, Register
	.if	@0>0x3F
		sts	@0, @1
	.else
		out	@0, @1
	.endif
.endmacro

.macro load 		;Arguments: Register, Address
	.if	@1>0x3F
		lds	@0, @1
	.else
		in	@0, @1
	.endif
.endmacro
