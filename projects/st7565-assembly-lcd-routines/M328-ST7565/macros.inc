
.macro	sb
	sbr	@0,(1<<@1)
.endmacro

.macro	cb
	cbr	@0,(1<<@1)
.endmacro


; sets the output stream to either null/serial/ram
.macro	stream
	push	temp1
	ldi	temp1,@0
	sts	stream_destination,temp1
	pop	temp1
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
	call	sd1
.endmacro

; centi-seconds delay (1/10th sec) 8-bit
.macro	csdelay
	ldi	temp1,@0
	call	sd2
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

.macro	ldACC
	.if @0==1
	ldi	accL,low(@1)
	ldi	accH,high(@1)
	.elif @0==2
	ldi	accL,low(2*@1)
	ldi	accH,high(2*@1)
	.else
	.error "must be ram or rom"
	.endif
.endmacro


; sends a single char to the output stream
.macro	send
	push	temp1
	ldi	temp1,@0
	call	stream_out
	pop	temp1
.endmacro



.macro	lcd_clear
	call	LCD_clearscreen
.endmacro

.macro	lcd_set		; sets cursor position row,coll
	push	temp1
	ldi	temp1,@0
	sts	row,temp1
	ldi	temp1,@1
	sts	coll,temp1
	pop	temp1
.endmacro

.macro	lcd_page	; use with put_LCDchar for direct row writes, no curs pos
	ldi	temp1,@0
	sts	Lpage,temp1
	call	lcd_set_page
.endmacro

.macro	lcd_coll		; sets the pixel col in the page
	ldi	temp1,@0
	sts	Lcoll,temp1
	call	lcd_set_coll
.endmacro



.macro	lcd_invert
	sb	bitflags,invert
.endmacro


.macro	lcd_normal
	cb	bitflags,invert
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
