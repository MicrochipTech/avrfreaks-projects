;***************************************************************************
;*
;* Graphics Primitives Functions for ATMEL AVR
;*
;* Target: AVRs with hardware multiplier & internal RAM
;*
;* Graphics controller hardware: 2x SED1520, described in "G_LCD_Swap"
;* Note: Routines are easily adaptable to similar controllers (cf. below)
;*
;* The routines are primarily optimized for speed, especially the "draw
;* character" and "filled primitive" (circle, rectangle, ellipse etc.)
;* functions are typically 3 to 10 times faster than compiler generated code.
;* Example: bitmap (icon), 24 bytes graphics data: 897 cycles
;*	    character string, 12 chars 8x8: 5172 cycles
;*	    ellipse, a=50, b=10: filled 23000, outline 13900 cycles
;*	    circle, r=15: filled 10836, outline 3442 cycles
;* Due to 'assembler only' design the code space required also is typically
;* only 50% to 80% of its compiler equivalent.
;*
;* The functions preserve all registers used, so they can easily be embedded
;* inline in 'high-level language' sources.
;*
;* Note: maximum display size in this implementation is 255x255. The
;*	 functions assume a 'vertically aligned' and 'paged' video memory
;*	 organisation (Epson SED136x, SED15xx controllers, HD61202/KS0107,
;*	 KS0713 etc.):
;*
;*	|--------------------------------------------
;*	| L    (1. byte of video memory)
;*	| S
;*	| Bit
;*	|      -> first page, 8 pixels 'high', max_col pixels 'wide'
;*	| M
;*	| S
;*	| Bit
;*	|--------------------------------------------
;*	| L    (max_column x1)th byte of video memory)
;*	| S
;*	| Bit
;*	|      -> second page, etc.
;*	| M
;*	| S
;*	| Bit
;*	|--------------------------------------------
;*
;*
;* List of functions:
;*
;* Line (with clear/set/invert)
;* Rectangle (outline/filled w. clear/set/invert)
;* Circle (outline/filled w. clear/set/invert)
;* Ellipse (outline/filled w. clear/set/invert)
;* Polygon/Area Seed Fill(s) (pixel set only,
;*	fastest implementation with "checkered Fill" option)
;* Bitmap (Icon/Picture w. clear/set/invert)
;* Single Pixel R/W (w. clear/set/invert)
;* Single Character (w. clear/set/invert)
;* ASCIIZ String (w. clear/set/invert)
;* Print BCD as ASCII (w. clear/set/invert)
;* Print HEX as ASCII (w. clear/set/invert)
;* Clear/Fill/Invert Screen
;* Write Screen Buffer to Display (see above)
;*
;* Three fonts (8x8 bold, 8x8 thin, 5x6 small, full IBM enxtended character
;* set (code page 437) included, all routines extensively commented.
;*
;* This library is an enhanced re-arrangement and somewhat 'generalized'
;* compilation of modules I've written over the past two years for our
;* designs.
;*
;* I've included and adapted some code (Bresenham line, print BCD/Hex) from
;* Mr. Rolf R. Kalle's "Nokia 7110 LCD graphics library V2"; I've also
;* used some of his labels and other naming conventions as Mr. Kalle's
;* library actually provided the incentive to start this. By the way: in
;* "Nokia 7110 ..." (project#81) you'll also find a device driver for an
;* 8-page 96x64 organized serially controlled display (SED 1565, used by
;* the aforementioned Nokia 7110).
;*
;* I would highly appreciate any feedback from people who use the
;* routines presented here.
;* I'd also like to gather any additional graphics routines, bug fixes and
;* especially further device drivers and code adaptions for different HW
;* architectures you might happen to come up with into this archive.
;*
;* Author and copyright:
;* Andreas Lenze
;* EDV-Systeme Lenze
;* Munich, Germany 2003 - 2005
;* andreas.lenze@t-online.de
;*
;* BMP2BLV.EXE (c) Electronic Assembly
;*
;***************************************************************************

; register defs for graphics routines - ** DON'T CHANGE THESE! **
.def	x1	= r16	; screen x/y coordinates
.def	x2	= r17
.def	y1	= r18
.def	y2	= r19
.def	radius	= r19	; circle
.def	a	= r19	; ellipse, radius_x
.def	b	= r17	; ellipse, radius_y

; for most functions:
.def	pmode	= r15	; set/clear/invert pixels, that is
			; for a black line/fill set pmode=0
			; for a white line/fill set pmode=1
			; for an inverted line/fill set pmode=-1

; screen buffer parameters
.equ	max_r		=	32	; max. rows (max. 256)
.equ	max_c		=	120	; max. columns (max. 256)
.equ	gbuf_size	=	max_r * max_c / 8	; # of bytes in frame buffer

.equ	fillbuf_length	=	0x0100	; length or size of fill routine buffer,
					; 64-xxx bytes, must be divisible by 2 (flood)
					; and/or by 4 (seed)

; Arrays
.DSEG
.org	0x0100

g_btable:				; look-up table for g_plot etc. (SRAM *low byte* adr.
	.BYTE 8				; for this implementation *must* be at <= 0xF8 as we
					; have omitted several "adc/sbc YH,0" in drx_plot)

g_vscreen:				; frame buffer for 2 x SED1520 (120x32 screen size)
	.BYTE gbuf_size			; reserve max_r*max_c/8 bytes

g_struct:				; data struct for g_drawellipse (midpoint):
	.byte 26			; 00: P  (4 Bytes)
					; 04: r1cp / d1S  (4 Bytes)
					; 08: b2 (2 Bytes)
					; 10: a2 (2 Bytes)
					; 12: d2E (2 Bytes)
					; 14: d2S (2 Bytes)
					; 16: d2SE (2 Bytes)
					; 18: d1E (4 Bytes)
					; 22: d1SE (4 Bytes)
					; Note: this buffer could be combined with 'fill_buf_base'
					; in case you're running out of SRAM ...)

save_z:
	.BYTE 2				; buffer to save Z register pair (for LCD draw routines)

fillbuf_in:
	.byte 2				; pointer to address in buffer written to (init: fillbuf_base)

fillbuf_out:
	.byte 2				; pointer to address in buffer read from (init: fillbuf_base)

fillbuf_base:
	.byte fillbuf_length		; reserves fillbuf_length bytes for the buffer(s)
					; and is used as the buffer뭩 base address.
					; Put this buffer last - will grow towards stack!

; Tables
.CSEG
g_tab:	; look-up table data for pixel plot/read routines
	.db 0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80

g_phex:
	.db "0123456789ABCDEF"

g_ubin:
	.dw 10000
	.dw 1000
	.dw 100
	.dw 10
	.dw 1


smile_pic:	; an example picture
.db 0x0C, 0x0C
.db 0xF0, 0xFC, 0xFE, 0xFE, 0xF7, 0xBF, 0xBF, 0xF7, 0xFE, 0xFE, 0xFC, 0xF0
.db 0x00, 0x03, 0x07, 0x06, 0x0D, 0x0D, 0x0D, 0x0D, 0x06, 0x07, 0x03, 0x00

Test:		; an example ASCIIZ string
.db "abcdefgh12AB@",0x00


;****************************************************************
;* G_btabini - Must be called once before using the functions!! *
;****************************************************************

g_btabini:	; initialize look-up table (-> copy in SRAM)
	ldi	ZH,high(g_tab*2)	; table address (flash)
	ldi	ZL,low(g_tab*2)

	ldi	YL,low(g_btable)	; table address of copy in SRAM
	ldi	YH,high(g_btable)	; -> <= 0x00F8!
	ldi	Tmp1,8			; counter (any high reg)

g_bt_ini1:
	lpm	r0,Z+			; copy 8 bytes to SRAM
	st	Y+,r0
	dec	Tmp1
	brne	g_bt_ini1
	ret

;**** End function G_btabini ------------------------------------------****


;**************************************************************************
;*
;* Device driver modules for my example 2xSED1520 display (120x32)
;*
;**************************************************************************

;**************************************************************************
;*
;* +++++++++++++++ 2x SED1520 DEVICE DRIVER +++++++++++++++++++++++++++++++
;*
;* Function G_Lcd_Swap - writes frame buffer to LCD 120x32 display
;*
;* Call with: -
;*
;* Hardware setup information (Motorola parallel interface):
;*
;* PORTA -> Display Data (D0-D7)
;* PORTC -> Display Control: PC2 = E2, enable right half of display
;*			     PC3 = E1, enable left half of display
;*			     PC4 = R/W#, (write=0, read=1)
;*			     PC5 = A0, (0=command, 1=data)
;*
;* Signal timing: clock frequency is assumed to be 7.3727MHz (cycle ~136ns)
;* AVR is run at 5V (A0 cycle time should be minimally 1탎)
;* Execution time: 4941 cycles (~670탎), interrupts disabled
;*
;* g_vscreen address can be choosen freely
;* frame buffer (virtual screen) size is max_r*max_c/8 bytes (here: 480 bytes).
;*
;* This write routine deals with a master/slave SED1520 design (2 enable
;* signals). Of the "max_c" the 1st half is controlled by 1st SED1520, the
;* 2nd half by the 2nd SED1520.
;*
;* All regs saved
;*
;**************************************************************************

.equ	sed_l = 3	; E1 on PC3, left half of screen
.equ	sed_r = 2	; E2 on PC2, right half of screen
.equ	adr0  = 5	; A0 on PC5
.equ	RW#  = 4	; R/W# on PC4

;.macro set_page	; sets page#, column 0, SED1520 left/right (@0 -> left=3, right=2)
;	cbi	portc,adr0	; set command mode (A0=0)
;	nop			; needs 1 wait
;	sbi	portc,@0	; enable half
;	out	PORTA,r18	; command 'set page#'
;	cbi	portc,@0	; disable half
;	nop			; extend enable cycle time
;	sbi	portc,@0	; enable half
;	out	PORTA,r16	; command 'set column#'
;	cbi	portc,@0	; disable half
;	nop			; extend A0 cycle time to >1탎
;.endm

.macro wr_data	; writes data to 2xSED1520 left/right (@0, left=3, right=2)
	ldi 	r16,max_c / 2	; use as bytecounter (here: 120 dec. / 2),
				; will be cleared at loop exit
wr_loop:
	ld	r2,Y+		; fetch one data byte
	out	PORTA,r2	; set data
	sbi	portc,@0	; enable half
	dec	r16
	cbi	portc,@0	; disable half
	brne	wr_loop		; loop max_c/2 times
.endm

G_Lcd_Swap:
	cli			; disable ints
	push	yl
	push	yh
	push	r16
	push	r18
	push	r2

	ldi	yl,low(g_vscreen)
	ldi	yh,high(g_vscreen)
	cbi	PORTC,RW#	; make sure write mode (R/W#=0) is set
	ldi	r18,0xB8	; page number command & number (page0=B8h - page3=BBh)
	clr	r16		; reg var. column #0 + byte counter in write loop

g_lswap1:
	; set page#, column 0, 2xSED1520
	; both halves adressed in ||, A0 cycle time 1.6탎
	cbi	portc,adr0	; set command mode (A0=0)
	sbi	portc,sed_l	; enable l. half
	sbi	portc,sed_r	; enable r. half
	out	PORTA,r18	; command 'set page#', pages 0 to 3
	cbi	portc,sed_l	; disable l. half
	cbi	portc,sed_r	; disable r. half
;	nop			; extend enable cycle time
	sbi	portc,sed_l	; enable l. half
	sbi	portc,sed_r	; enable r. half
	out	PORTA,r16	; command 'set column#', column 0
	cbi	portc,sed_l	; disable l. half
	cbi	portc,sed_r	; disable r. half

	; write data to display halves
	sbi	PORTC,adr0	; set data mode (A0=1)
	wr_data sed_l
	wr_data sed_r

	inc	r18		; set next page (max. page # = BBh)
	cpi	r18,0xBC	; BCh = would be one page too many ...
	brne	g_lswap1

	pop	r2
	pop	r18
	pop	r16
	pop	yh
	pop	yl

	reti			; "ret" + "sei" -> save 1 cycle

;**** End function G_Lcd_Swap -----------------------------------------****


;**************************************************************************
;*
;* SED1520_ON - Switch on the display
;*
;* Must be called (once) before using the functions!!
;*
;**************************************************************************

SED1520_ON:	; switch on SED1520 LCD display
	push	r20
	cbi	portc,RW#	; set write mode (R/W#=0)
	cbi	portc,adr0	; set command mode (A0=0)
	sbi	portc,sed_l	; enable left half
	sbi	portc,sed_r	; enable right half
	ldi	r20,0xAF	; 'display on' command
	out	PORTA,r20	; send command
	cbi	portc,sed_l	; disable left half
	cbi	portc,sed_r	; disable right half
	pop	r20
	ret

;**** End function SED1520_ON -----------------------------------------****


;**************************************************************************
;*
;* Note for the graphics based 'print character' / 'print string routines':
;*
;* For a black character set pmode=0 (this also will erase any previously
;*				      set pixels of the character matrix
;*				      prior to writing a character; i.e.
;*				      allow overwriting)
;*
;* For a white character set pmode=1
;* For a reversed character set pmode=-1
;*				     (these modes will just clear resp.
;*				      invert all pixels which are 'on' in
;*				      the character bitmap)
;*
;**************************************************************************


;**************************************************************************
;*
;* Function G_str8x8t - prints ASCIIZ terminated string in 8x8 thin font
;*
;* Call with:
;*	 x1	-> start column of string (xpos top left)
;*	 y1	-> start row of string (ypos top left)
;*	 Z	-> must point to first byte of character string
;*
;* Example:
;* 	 ldi ZH,high(Test*2)
;* 	 ldi ZL,low(Test*2)
;* 	 ldi x1,0
;* 	 ldi y1,3
;* 	 call G_str8x8t
;*
;* All regs saved
;*
;**************************************************************************

G_str8x8t:
	push	r0
	push	r1
	push	r5
	push	r8
	push	r19
	push	r20

	push	ZH
	push	ZL
	push	x1		; will be incremented, save it

	ldi	r20,8
	clr	r8		; make sure # of data bytes (high) is cleared
	mov	r19,r20		; 8 bytes
	mov	r5,r20		; 8 columns width / increment

g_str88t_1:
	lpm	r0,Z+
	sts	save_z,ZL	; save Z reg pair
	sts	save_z + 1,ZH	; (pointer to string)
	tst	r0		; ASCIIZ?
	breq	g_str88t_2	; yes, exit

	ldi	ZH,high(g_font8x8t*2)	; load font table
	ldi	ZL,low(g_font8x8t*2)

	mul	r0,r20		; r20 bytes into table

	add	ZL,r0		; char offset into font table
	adc	ZH,r1

	rcall	g_drawchar	; print character

	lds	ZL,save_Z	; restore Z reg pair
	lds	ZH,save_Z + 1
	add	x1,r20		; increment char. position

	cpi	x1,max_c - 7	; boundary check
	brlo	g_str88t_1	; loop until ASCIIZ or off-screen

g_str88t_2:
	pop	x1
	pop	ZL
	pop	ZH
	pop	r20
	pop	r19
	pop	r8
	pop	r5
	pop	r1
	pop	r0
	ret

;**** End of G_str8x8t function -------------------------------------- ****


;**************************************************************************
;*
;* Function G_char8x8t - prints single character in 8x8 thin font
;*
;* Call with:
;*	 x1	-> start column of char (xpos top left)
;*	 y1	-> start row of char (ypos top left)
;*	 r0	-> ASCII character number
;*
;* Note: x1 incremented by character width, all other regs saved
;*
;**************************************************************************

G_char8x8t:
	cpi	x1,max_c - 7	; boundary check
	brlo	PC+2
	ret

	push	r0
	push	r1
	push	r5
	push	r8
	push	r19

	push	r20

	push	ZH
	push	ZL

	ldi	r20,8
	clr	r8		; make sure # of data bytes (high) is cleared
	mov	r19,r20		; 8 bytes
	mov	r5,r20		; 8 columns width / increment

	ldi	ZH,high(g_font8x8t*2)	; load font table
	ldi	ZL,low(g_font8x8t*2)

	mul	r0,r20		; r20 bytes into table

	add	ZL,r0		; char offset into font table
	adc	ZH,r1

	rcall	g_drawchar	; print character
	add	x1,r20		; advance xpos by 1 character width

	pop	ZL
	pop	ZH
	pop	r20
	pop	r19
	pop	r8
	pop	r5
	pop	r1
	pop	r0
	ret

;**** End of G_char8x8t function ------------------------------------- ****


;**************************************************************************
;*
;* Function G_str8x8b - prints ASCIIZ terminated string in 8x8 bold font
;*
;* Call with:
;*	 x1	-> start column of string (xpos top left)
;*	 y1	-> start row of string (ypos top left)
;*	 Z	-> must point to first byte of character string
;*
;* Example:
;* 	 ldi ZH,high(Test*2)
;* 	 ldi ZL,low(Test*2)
;* 	 ldi x1,0
;* 	 ldi y1,3
;* 	 call G_str8x8b
;*
;* All regs saved
;*
;**************************************************************************

G_str8x8b:
	push	r0
	push	r1
	push	r5
	push	r8
	push	r19

	push	r20

	push	ZH
	push	ZL
	push	x1		; will be incremented, save it

	ldi	r20,8
	clr	r8		; make sure # of data bytes (high) is cleared
	mov	r19,r20		; 8 bytes
	mov	r5,r20		; 8 columns width / increment

g_str88b_1:
	lpm	r0,Z+
	sts	save_z,ZL	; save Z reg pair
	sts	save_z + 1,ZH	; (pointer to string)
	tst	r0		; ASCIIZ?
	breq	g_str88b_2

	ldi	ZH,high(g_font8x8b*2)	; load font table
	ldi	ZL,low(g_font8x8b*2)

	mul	r0,r20		; r20 bytes into table

	add	ZL,r0		; char offset into font table
	adc	ZH,r1

	rcall	g_drawchar	; print character

	lds	ZL,save_Z	; restore Z reg pair
	lds	ZH,save_Z + 1
	add	x1,r20		; increment char. position

	cpi	x1,max_c - 7	; boundary check
	brlo	g_str88b_1	; loop until ASCIIZ or off-screen

g_str88b_2:
	pop	x1
	pop	ZL
	pop	ZH
	pop	r20
	pop	r19
	pop	r8
	pop	r5
	pop	r1
	pop	r0
	ret

;**** End of G_str8x8b function -------------------------------------- ****


;**************************************************************************
;*
;* Function G_char8x8b - prints single character in 8x8 bold font
;*
;* Call with:
;*	 x1	-> start column of char (xpos top left)
;*	 y1	-> start row of char (ypos top left)
;*	 r0	-> ASCII character number
;*
;* Note: x1 incremented by character width, all other regs saved
;*
;**************************************************************************

G_char8x8b:
	cpi	x1,max_c - 7	; boundary check
	brlo	PC+2
	ret

	push	r0
	push	r1
	push	r5
	push	r8
	push	r19

	push	r20

	push	ZH
	push	ZL

	ldi	r20,8
	clr	r8		; make sure # of data bytes (high) is cleared
	mov	r19,r20		; 8 bytes
	mov	r5,r20		; 8 columns width / increment

	ldi	ZH,high(g_font8x8b*2)	; load font table
	ldi	ZL,low(g_font8x8b*2)

	mul	r0,r20		; r20 bytes into table

	add	ZL,r0		; char offset into font table
	adc	ZH,r1

	rcall	g_drawchar	; print character
	add	x1,r20		; advance xpos by 1 character width

	pop	ZL
	pop	ZH
	pop	r20
	pop	r19
	pop	r8
	pop	r5
	pop	r1
	pop	r0
	ret

;**** End of G_char8x8b function ------------------------------------- ****


;**************************************************************************
;*
;* Function G_str5x6 - prints ASCIIZ terminated string in 5x6 small font
;*
;* Call with:
;*	 x1	-> start column of string (xpos top left)
;*	 y1	-> start row of string (ypos top left)
;*	 Z	-> must point to first byte of character string
;*
;* Example:
;* 	 ldi ZH,high(Test*2)
;* 	 ldi ZL,low(Test*2)
;* 	 ldi x1,0
;* 	 ldi y1,3
;* 	 call G_str5x6
;*
;* All regs saved
;*
;**************************************************************************

G_str5x6:
	push	r0
	push	r1
	push	r5
	push	r8
	push	r19

	push	r20

	push	ZH
	push	ZL
	push	x1		; will be incremented, save it

	ldi	r20,5
	clr	r8		; make sure # of data bytes (high) is cleared
	mov	r19,r20		; 5 bytes
	mov	r5,r20		; 5 columns width / increment

g_str56_1:
	lpm	r0,Z+
	sts	save_z,ZL	; save Z reg pair
	sts	save_z + 1,ZH	; (pointer to string)
	tst	r0		; ASCIIZ?
	breq	g_str56_2

	ldi	ZH,high(g_font5x6*2)	; load font table
	ldi	ZL,low(g_font5x6*2)

	mul	r0,r20		; r20 bytes into table

	add	ZL,r0		; char offset into font table
	adc	ZH,r1

	rcall	g_drawchar	; print character

	lds	ZL,save_Z	; restore Z reg pair
	lds	ZH,save_Z + 1
	add	x1,r20		; increment char. position

	cpi	x1,max_c - 4	; boundary check
	brlo	g_str56_1	; loop until ASCIIZ or off-screen

g_str56_2:
	pop	x1
	pop	ZL
	pop	ZH
	pop	r20
	pop	r19
	pop	r8
	pop	r5
	pop	r1
	pop	r0
	ret

;**** End of G_str5x6 function --------------------------------------- ****


;**************************************************************************
;*
;* Function G_char5x6 - prints single character in 5x6 small font
;*
;* Call with:
;*	 x1	-> start column of char (xpos top left)
;*	 y1	-> start row of char (ypos top left)
;*	 r0	-> ASCII character number
;*
;* Note: x1 incremented by character width, all other regs saved
;*
;**************************************************************************

G_char5x6:
	cpi	x1,max_c - 4	; boundary check
	brlo	PC+2
	ret

	push	r0
	push	r1
	push	r5
	push	r8
	push	r19

	push	r20

	push	ZH
	push	ZL

	ldi	r20,5
	clr	r8		; make sure # of data bytes (high) is cleared
	mov	r19,r20		; 5 bytes
	mov	r5,r20		; 5 columns width / increment

	ldi	ZH,high(g_font5x6*2)	; load font table
	ldi	ZL,low(g_font5x6*2)

	mul	r0,r20		; r20 bytes into table

	add	ZL,r0		; char offset into font table
	adc	ZH,r1

	rcall	g_drawchar	; print character
	add	x1,r20		; advance xpos by 1 character width

	pop	ZL
	pop	ZH
	pop	r20
	pop	r19
	pop	r8
	pop	r5
	pop	r1
	pop	r0
	ret

;**** End of G_char5x6 function -------------------------------------- ****


;**************************************************************************
;*
;* Function G_Printbcd -  prints a 16bit decimal number as 5 binary digits
;*			  starting at position (x1,y1)
;* (From code by R. Kalle)
;*
;* Call with: number in XH:XL, x1, y1, pmode
;*
;* All regs saved except x1 (incremented by five character widths)
;*
;* x1 is incremented with each digit
;*
;**************************************************************************

g_printbcd:
	push	XL
	push	XH
	push	YL
	push	YH
	push	ZL
	push	ZH

	push	r0

	push	r25
	push	r24
	push	x1

	ldi	ZL,low(g_ubin*2)	;point z at ubin table
	ldi	ZH,high(g_ubin*2)

	ldi	r25,5		;there is 5 digits in result

g_ub1:	ldi	r24,-1

	lpm	YL,z+		;get digit weight
	lpm	YH,z+

g_ub2:	inc	r24		;r18=int(x/y) ; x=mod(x/y)
	sub	XL,YL
	sbc	XH,YH

	brcc	g_ub2

	add	XL,YL
	adc	XH,YH

	subi	r24,-'0'	;offset r0 to character '0'

	mov	r0,r24

	rcall	g_char8x8b	;replace with g_char5x6 if you want smaller letters
;	rcall	g_char5x6
	dec	r25		;more digits?
	brne	g_ub1		;yes

	pop	x1
	pop	r24
	pop	r25

	pop	r0

	pop	ZH
	pop	ZL
	pop	YH
	pop	YL
	pop	XH
	pop	XL
	ret			; exit function

;**** End of G_Printbcd function ------------------------------------- ****


;**************************************************************************
;*
;* Function G_Printhex -  prints a 16bit hex number as 4 hex digits
;*			  starting at position (x1,y1)
;* (From code by R. Kalle)
;*
;* Call with: number in XH:XL, x1, y1, pmode
;*
;* All regs saved except x1 (incremented by four character widths)
;*
;* x1 is incremented with each digit
;*
;**************************************************************************

g_printhex:

	push	ZL
	push	ZH
	push	r20
	push	r0

	mov	r20,XH
	swap	r20
	rcall	g_ph1
	mov	r20,XH
	rcall	g_ph1
	mov	r20,XL
	swap	r20
	rcall	g_ph1
	mov	r20,XL
	rcall	g_ph1

	pop	r0
	pop	r20
	pop	ZH
	pop	ZL
	ret			; exit function

g_ph1:	ldi	ZL,low(g_phex*2)
	ldi	ZH,high(g_phex*2)
	andi	r20,0x0f
	add	ZL,r20
	ldi	r20,0
	adc	ZH,r20
	lpm	r0,z
	rcall	g_char8x8b	;replace with g_char5x6 if you want smaller letters
	ret

;**** End of G_Printhex function ------------------------------------- ****


;**************************************************************************
;*
;* Function G_Drawpic - draw a picture
;*
;* Call with:
;*	 x1	-> start column of picture (xpos top left)
;*	 y1	-> start row of picture (ypos top left)
;*	 Z	-> must point to first byte of picture bitmap (header)
;*
;* Example:
;* 	 ldi ZH,high(smile_pic*2)
;* 	 ldi ZL,low(smile_pic*2)
;* 	 ldi x1,0
;* 	 ldi y1,3
;* 	 call G_Drawpic
;*
;*
;* Subfunction G_Drawchar - draw a character (internally called from the
;*					      char/str print routines,
;*					      see note 2)
;* Call with:
;*	 r8:r19	-> # of databytes in char. bitmap
;*	 r5	-> # of columns
;*	 x1	-> start column (xpos)
;*	 y1	-> start row (ypos)
;*	 Z	-> must point to first data byte of char. bitmap
;*
;*	 All functions:
;* 	 For a black picture set pmode=0 (see note 2!)
;* 	 For a white picture set pmode=1
;* 	 For a reversed picture set pmode=-1
;*
;* All regs saved, T-flag cleared
;*	 28 bytes stack used
;*	 31 cycles code + 104 cycles stack overhead
;*	 24 (26 for "clear p.") cycles per graph data byte
;*
;* Note: 1. Picture graphics data must be preceeded by a 2 byte header
;*	    (bitmap width and heigth in pixels). Pixels in data bytes are
;*	    arranged vertically, 8 pixels per column, LSB -> MSB.
;*	    Create pictures with "BMP2BLV.EXE" (data + width/heigth header)
;*
;*	 2. In character draw mode, pmode=0 ("black picture") will clear the
;*	    character bitmap (= 'print a space') prior to writing a character
;*
;*	 3. Max virtual screen size 256x256 pixels
;*
;**************************************************************************

G_Drawchar: 	; draws a character
	set		; set T-Flag to skip matrix parameter set-up -
			; we'll call this with the parms already loaded
	rjmp	PC+2	; jump over "clear T-Flag"

G_Drawpic:	; draws a picture
	clt		; make sure T_flag is cleared on entry
	push	x1	; bitmap start xpos
	push	y1	; bitmap start ypos

	push	r19	; # of data bytes (low),y2
	push	r20	; scrap reg

	push	r25	; r25:r24 pointer to "sum(xpos+g_vscreen_start)"
	push	r24

	push	r0	; r0, r1 work regs (mul, fmul)
	push	r1
	push	r2	; max_c low
	push	r3	; max_c high (= 0 in this implementation)
	push	r4	; copy of # of columns
	push	r5	; # of columns
	push	r6	; copy of start xpos
	push	r7	; ypos offset (shift multiplier)
	push	r8	; # of data bytes (high)
	push	r9	; 1st display ram byte
	push	r10	; 2nd display ram byte
	push	r11	; ypos offset
	push	r12	; graph data fill space
	push	r13	; graph data
	push	r14	; indicates char. mode if =0
	push	ZL	; pointer graph data
	push	ZH
	push	YL	; secondary pointer display ram
	push	YH
	push	XL	; main pointer display ram
	push	XH

	; check boundaries on entry
	cpi	x1,max_c
	brlo	G_D1
	rjmp	g_drawexit
	cpi	y1,max_r
	brlo	G_D1
	rjmp	g_drawexit

G_D1:
	clr	r14		; r14 = 0, indicates char mode
	brts	G_D2		; character draw mode: skip header processing
	inc	r14		; r14 != 0, indicates graph mode

        ; calculate the number of graphics bytes ("data" w/o width/heigth info)
        lpm     r5,Z+		; get width info (first byte of data set)
        lpm     r20,Z+		; get heigth info (2nd byte of data set!)
        subi    r20,-7		; number of bytes = (((heigth+7)/mod 8)*width)+2
        lsr     r20		; add 7 to heigth and divide by mod 8
        lsr     r20
        lsr     r20
        mul     r20,r5		; previous x width, r8:r19 now contain # of
        mov	r19,r0		; data bytes, r5 # of columns
	mov	r8,r1

G_D2:	; entry point for character draw mode
	; entry parameters required for "draw character":
	; r19	-> # of databytes in char. bitmap
	; r5	-> # of columns
	; x1	-> start column (xpos)
	; y1	-> start row (ypos)
	; Z	-> must point to first data byte of char. bitmap
	clt			; make sure T_flag is cleared (again)
	mov	r4,r5		; copy of # of columns
	mov	r6,x1		; save copy of start column
	clr	r3		; max_c(high) =0, used as reg. var
	ldi	r20,max_c	; max_c(low) value as register variable
	mov	r2,r20

	; register variables status now::
	; r3	-> 0 ("(high)max_c")
	; r2	-> max_c
	; r8:r19-> # of databytes in bitmap, acts also as counter
	; r4,r5 -> # of columns (r5 as counter var, r4 backup)
	; r6,x1 -> start column (x1 as working var, r6 backup)
	; y1	-> start row (ypos)
	; Z	-> points to first data byte of bitmap (pic or char)

	; calculate bit offset start ypos <-> start 1st screen byte ypos
	mov	r20,y1		; r7=sram(bytab_offset+mod(ypos/8))
	andi	r20,0x07

	ldi	YL,low(g_btable)
	ldi	YH,high(g_btable)	; will be 0! (= 'clr YH')
	add	YL,r20		; offset into table
	ld	r7,Y		; load appropriate shift multiplier
	ldi	r20,16		; from here on r20 as reg var.

; **** main processing loop start ****

G_D3:
	cpi	y1,max_r-8 	; ypos 2nd byte off-screen now?
	brlo	G_D9		; no
	set			; yes, set T-flag
	cpi	y1,max_r	; ypos 1st byte off-screen now?
	brsh	G_D4a		; yes, exit (to intermediate label)

G_D9:
	ldi	r24,low(g_vscreen)	; on entry into module, init; later
	ldi	r25,high(g_vscreen)	; restore sum(xpos+g_vscreen_start)
	add	r24,x1
	adc	r25,r3		; r3 = constant 0

	; calculate & load vscreen data offset in XH:XL
	fmul	y1,r20		; x=int(ypos/8)*max_c, ypos in y1
	mul	r2,r1		; *low(max_c)
	movw	XL,r0
	add	XL,r24		; + sum(xpos+g_vscreen_start)
	adc	XH,r25
	movw	YL,XL		; calculate 2nd byte position from X
	add	YL,r2		; and copy it to Y (Y+max_c)
	adc	YH,r3

G_D10:	; get 2 bytes of vscreen which will receive new pixel values
	ld	r9,X		; get 1st display ram byte (r9)
	ld	r10,Y		; get 2nd display ram byte (r10)

	; determine pixel mode, change pixels accordingly
	tst	pmode		; pmode=0/1/-1 -> set/clear/XOR pixel
	brne	G_D4		; goto clear or invert

	tst	r14		; 'set pixel' mode - char or graph?
	brne	G_D11		; if !=0 we are in graph mode, continue normally
				; else: 'set' & character draw mode, clear all
				; pixels in char matrix (equals 'erase character')
	lpm	r13,Z		; get graphics data byte w/o Z-increment
	com	r13		; invert it

	; r13:r12 = 16 bit field, graph data
	; align screen data (r9:r10) <-> inverted graph data (r17:r12)
	mul     r13,r7		; shift left [r7] times
	movw	r12,r0		; r13 destroyed!

	com	r12		; clear character matrix except for those pixels
	and	r9,r12		; which make up the character
	com	r13
	and	r10,r13

G_D11:
	lpm	r13,Z+		; get graphics data byte (again if in char. mode)

	; r13:r12 = 16 bit field, graph data
	; align screen data (r9:r10) <-> graph data (r13:r12)
	mul     r13,r7		; shift left [r7] times
	movw	r12,r0

	or	r9,r12		; set (new char. or picture) pixels
	or	r10,r13
	rjmp	G_D6		; done

G_D4:	; pmode != 0 (clear or invert pixels, all draw modes)
	lpm	r13,Z+		; get graphics data byte

	; r13:r12 = 16 bit field, graph data
	; align screen data (r9:r10) <-> graph data (r13:r12)
	mul     r13,r7		; shift left [r7] times
	movw	r12,r0
	brmi	G_D5		; if pmode=-1, goto 'invert pixels', else

	com	r12		; pmode=1, -> clear pixels
	and	r9,r12
	com	r13
	and	r10,r13
	rjmp	G_D6		; done

G_D4a:	rjmp	G_Drawexit	; intermediate label needed because brxx

G_D5:
	eor	r9,r12		; XOR pixels
	eor	r10,r13

G_D6:				; one graph data byte's pixels now updated
	brts	G_D7		; set if ypos is off-screen: don't store this byte
	st	Y+,r10		; write 2nd byte back to display ram

G_D7:
	st	X+,r9		; write 1st byte back to display ram and increment pointer

	subi	r19,1		; one data byte processed
	sbc	r8,r3
	breq	G_Drawexit	; if none left, exit

	dec	r5		; "page" done?
	breq	G_D8		; yes, go to "new page"

	inc	x1		; increment xpos
	cpi	x1,max_c 	; off-screen now?
	brlo	G_D10		; ** no: same page: process next graph. data byte **

	; yes: xpos off screen: r8:r19 holds (possible) # of bytes still left in this
	; page, we'll skip these and - if  there's still data left -
	; correct the bookeeping (Z-pointer & # of remaining data bytes in r8:r19)
	sub	r19,r5
	sbc	r8,r3		; any data left (!= 0 or minus)?
	breq	G_Drawexit	; no, exit
	brmi	G_Drawexit	; no, exit

	add	ZL,r5		; increment data pointer
	adc	ZH,r3		; r3 = constant 0

G_D8:	; new page
	mov	r5,r4		; restore column counter
	mov	x1,r6		; restore x-position
	subi	y1,-8		; add 8 to y-position

	rjmp	G_D3		; ** next page: process next graph. data byte **

; **** main processing loop end ****

g_drawexit:
	clt
	pop	XH
	pop	XL
	pop	YH
	pop	YL
	pop	ZH
	pop	ZL
	pop	r14
	pop	r13
	pop	r12
	pop	r11
	pop	r10
	pop	r9
	pop	r8
	pop	r7
	pop	r6
	pop	r5
	pop	r4
	pop	r3
	pop	r2
	pop	r1
	pop	r0

	pop	r24
	pop	r25

	pop	r20
	pop	r19
	pop	y1
	pop	x1

	ret

;**** End of G_Drawpic/char function --------------------------------- ****



; speed optimized version (3 individual loops for pixel set mode)
;**************************************************************************
;*
;* Function G_Drawpic_1 - draw a picture
;*
;* Call with:
;*	 x1	-> start column of picture (xpos top left)
;*	 y1	-> start row of picture (ypos top left)
;*	 Z	-> must point to first byte of picture bitmap (header)
;*
;* Function G_Drawchar_1 - draw a character
;*
;* Call with:
;*	 r8:r19	-> # of databytes in char. bitmap
;*	 r5	-> # of columns
;*	 x1	-> start column (xpos)
;*	 y1	-> start row (ypos)
;*	 Z	-> must point to first data byte of char. bitmap
;*
;*	 All functions:
;* 	 For a black picture set pmode=0
;* 	 For a white picture set pmode=1
;* 	 For a reversed picture set pmode=-1
;*
;* All regs saved, T-flag cleared
;*	 26 bytes stack used
;*	 31 cycles code + 104 cycles stack overhead
;*	 24 (26 for "clear p.") cycles per graph data byte
;*
;* Note: 1. Picture graphics data must be preceeded by a 2 byte header
;*	    (bitmap width and heigth in pixels). Pixels in data bytes are
;*	    arranged vertically, 8 pixels per column, LSB -> MSB.
;*	    Create pictures with "BMP2BLV.EXE" (data + width/heigth header)
;*	    Max virtual screen size 256x256 pixels
;*
;*	 2. In character draw mode, pmode=0 ("black picture") will *not* clear
;*	    the character bitmap prior to writing a character!!
;*
;*	 3. Max virtual screen size 256x256 pixels
;*
;**************************************************************************

G_Drawchar_1: 		; draws a character
	set		; set T-Flag to skip matrix parameter set-up -
			; we'll call this with the parms already loaded
	rjmp	PC+2	; jump over "clear T-Flag"

G_Drawpic_1:		; draws a picture
	clt		; make sure T_flag is cleared on entry
	push	x1	; bitmap start xpos
	push	y1	; bitmap start ypos

	push	r19	; # of data bytes (low)
	push	r20	; scrap reg

	push	r25	; r25:r24 pointer to "sum(xpos+g_vscreen_start)"
	push	r24

	push	r0	; r0, r1 work regs (mul, fmul)
	push	r1
	push	r2	; max_c low
	push	r3	; max_c high (= 0 in this implementation)
	push	r4	; copy of # of columns
	push	r5	; # of columns
	push	r6	; copy of start xpos
	push	r7	; ypos offset (shift multiplier)
	push	r8	; # of data bytes (high)
	push	r9	; 1st display ram byte
	push	r10	; 2nd display ram byte
	push	r11	; ypos offset
	push	r12	; graph data fill space
	push	r13	; graph data

	push	ZL	; pointer graph data
	push	ZH
	push	YL	; secondary pointer display ram
	push	YH
	push	XL	; main pointer display ram
	push	XH


	; check boundaries on entry
	cpi	x1,max_c
	brlo	G_D00
	rjmp	g_drawexit

	cpi	y1,max_r
	brlo	G_D00
	rjmp	g_drawexit


G_D00:
	brts	G_D0		; character draw mode: skip header processing

        ; calculate the number of graphics bytes ("data" w/o width/heigth info)
        lpm     r5,Z+		; get width info (first byte of data set)
        lpm     r20,Z+		; get heigth info (2nd byte of data set!)
        subi    r20,-7		; number of bytes = (((heigth+7)/mod 8)*width)+2
        lsr     r20		; add 7 to heigth and divide by mod 8
        lsr     r20
        lsr     r20
        mul     r20,r5		; previous x width, r8:r19 now contain # of
        mov	r19,r0		; data bytes, r5 # of columns
	mov	r8,r1

G_D0:	; entry point for character draw mode
	; entry parameters required for "draw character":
	; r19	-> # of databytes in char. bitmap
	; r5	-> # of columns
	; x1	-> start column (xpos)
	; y1	-> start row (ypos)
	; Z	-> must point to first data byte of char. bitmap

	clt			; make sure T_flag is cleared (again)
	mov	r4,r5		; copy of # of columns
	mov	r6,x1		; save copy of start column
	clr	r3		; max_c(high) =0, used as reg. var
	ldi	r20,max_c	; max_c(low) value as register variable
	mov	r2,r20

	; register variables status now::
	; r3	-> 0 ("(high)max_c")
	; r2	-> max_c
	; r8:r19-> # of databytes in bitmap, acts also as counter
	; r4,r5 -> # of columns (r5 as counter var, r4 backup)
	; r6,x1 -> start column (x1 as working var, r6 backup)
	; y1	-> start row (ypos)
	; Z	-> points to first data byte of bitmap (pic or char)

	; calculate bit offset start ypos <-> start 1st screen byte ypos
	mov	r20,y1		; r7=sram(bytab_offset+mod(ypos/8))
	andi	r20,0x07

	ldi	YL,low(g_btable)
	ldi	YH,high(g_btable)	; will be 0! (= 'clr YH')
	add	YL,r20		; offset into table
	ld	r7,Y		; load appropriate shift multiplier
	subi	y1,8		; use the (incrementing) code from "next page ..."
				; to initialize the vscreen pointer offset

	ldi	r20,16		; from here on r20 as reg var.

	; for performance reasons the graph data processing loop is provided in
	; 3 versions (separately for each pixel draw mode (-> set, clear, invert)
	tst	pmode		; pixel draw mode (0/1/-1)
	brne	G_D1
	rjmp	G_D9s		; SET pixels: enter main byte processing loop

G_D1:
	brmi	G_D2
	rjmp	G_D9c		; CLEAR pixels: enter main byte processing loop

G_D2:
	rjmp	G_D9i		; INVERT pixels: enter main byte processing loop


; **** main processing loops start ****


G_D3s:	; ** SET PIXELS main byte processing loop version **
	; get 2 bytes of vscreen which will receive new pixel values
	ld	r9,X		; get 1st display ram byte (r9)
	ld	r10,Y		; get 2nd display ram byte (r10)
	lpm	r13,Z+		; get graphics data byte

	; r13:r12 = 16 bit field, graph data
	; align screen data (r9:r10) <-> graph data (r13:r12)
        mul     r13,r7		; shift left [r7] times
        mov	r13,r0
	mov	r12,r1

	; graph data are now aligned with display ram data
	or	r9,r13		; set pixels
	or	r10,r12

	brts	G_D7s		; set if ypos is off-screen: don't store this byte
	st	Y+,r10		; write 2nd byte back to display ram

G_D7s:
	st	X+,r9		; write 1st byte back to display ram and increment pointer

	subi	r19,1		; one data byte processed
	sbc	r8,r3
	breq	G_D_ex		; if none left, exit

	dec	r5		; "page" done?
	breq	G_D8s		; yes, go to "new page"

	inc	x1		; increment xpos
	cpi	x1,max_c 	; off-screen now?
	brlo	G_D3s		; ** no: same page: process next graph. data byte **

	; yes: xpos off screen: r8:r19 holds (possible) # of bytes still left in this
	; page, we'll skip these and - if  there's still data left -
	; correct the bookeeping (Z-pointer & # of remaining data bytes in r8:r19)
	sub	r19,r5
	sbc	r8,r3		; any data left (!= 0 or minus)?
	breq	G_D_ex		; no, exit
	brmi	G_D_ex		; no, exit

	add	ZL,r5		; increment data pointer
	adc	ZH,r3		; r3 = constant 0

G_D8s:	; new page
	mov	r5,r4		; restore column counter
	mov	x1,r6		; restore x-position

G_D9s:
	ldi	r24,low(g_vscreen)	; on entry into module, init; later
	ldi	r25,high(g_vscreen)	; restore sum(xpos+g_vscreen_start)
	add	r24,x1
	adc	r25,r3		; r3 = constant 0

	subi	y1,-8		; add 8 to y-position (compensated on entry)
	cpi	y1,max_r-8 	; ypos 2nd byte off-screen now?
	brlo	G_D10s		; no
	set			; yes, set T-flag
	cpi	y1,max_r	; ypos 1st byte off-screen now?
	brsh	G_D_ex		; yes, exit

G_D10s:
	; calculate & load vscreen data offset in XH:XL
	fmul	y1,r20		; x=int(ypos/8)*max_c, ypos in y1
	mul	r2,r1		; *low(max_c)
	movw	XL,r0
	add	XL,r24		; + sum(xpos+g_vscreen_start)
	adc	XH,r25
	movw	YL,XL		; calculate 2nd byte position from X
	add	YL,r2		; and copy it to Y (Y+max_c)
	adc	YH,r3
	rjmp	G_D3s		; ** next page: process next graph. data byte **

G_D_ex:	; intermediate exit point (compensate brxx boundary)
	rjmp	G_Drawexit



G_D3c:	; ** CLEAR PIXELS main byte processing loop version **
	; get 2 bytes of vscreen which will receive new pixel values
	ld	r9,X		; get 1st display ram byte (r9)
	ld	r10,Y		; get 2nd display ram byte (r10)
	lpm	r13,Z+		; get graphics data byte

	; r13:r12 = 16 bit field, graph data
	; align screen data (r9:r10) <-> graph data (r13:r12)
        mul     r13,r7		; shift left [r7] times
        mov	r13,r0
	mov	r12,r1

	; graph data are now aligned with display ram data
	com	r13		; clear pixels
	and	r9,r13
	com	r12
	and	r10,r12

	brts	G_D7c		; set if ypos is off-screen: don't store this byte
	st	Y+,r10		; write 2nd byte back to display ram

G_D7c:
	st	X+,r9		; write 1st byte back to display ram and increment pointer

	subi	r19,1		; one data byte processed
	sbc	r8,r3
	breq	G_D_ex		; if none left, exit

	dec	r5		; "page" done?
	breq	G_D8c		; yes, go to "new page"

	inc	x1		; increment xpos
	cpi	x1,max_c 	; off-screen now?
	brlo	G_D3c		; ** no: same page: process next graph. data byte **

	; yes: xpos off screen: r8:r19 holds (possible) # of bytes still left in this
	; page, we'll skip these and - if  there's still data left -
	; correct the bookeeping (Z-pointer & # of remaining data bytes in r8:r19)
	sub	r19,r5
	sbc	r8,r3		; any data left (!= 0 or minus)?
	breq	G_D_ex		; no, exit
	brmi	G_D_ex	; no, exit

	add	ZL,r5		; increment data pointer
	adc	ZH,r3		; r3 = constant 0

G_D8c:	; new page
	mov	r5,r4		; restore column counter
	mov	x1,r6		; restore x-position

G_D9c:
	ldi	r24,low(g_vscreen)	; on entry into module, init; later
	ldi	r25,high(g_vscreen)	; restore sum(xpos+g_vscreen_start)
	add	r24,x1
	adc	r25,r3		; r3 = constant 0

	subi	y1,-8		; add 8 to y-position (compensated on entry)
	cpi	y1,max_r-8 	; ypos 2nd byte off-screen now?
	brlo	G_D10c		; no
	set			; yes, set T-flag
	cpi	y1,max_r	; ypos 1st byte off-screen now?
	brsh	G_Drawexit	; yes, exit

G_D10c:
	; calculate & load vscreen data offset in XH:XL
	fmul	y1,r20		; x=int(ypos/8)*max_c, ypos in y1
	mul	r2,r1		; *low(max_c)
	movw	XL,r0
	add	XL,r24		; + sum(xpos+g_vscreen_start)
	adc	XH,r25
	movw	YL,XL		; calculate 2nd byte position from X
	add	YL,r2		; and copy it to Y (Y+max_c)
	adc	YH,r3
	rjmp	G_D3c		; ** next page: process next graph. data byte **



G_D3i:	; ** INVERT PIXELS main byte processing loop version **
	; get 2 bytes of vscreen which will receive new pixel values
	ld	r9,X		; get 1st display ram byte (r9)
	ld	r10,Y		; get 2nd display ram byte (r10)
	lpm	r13,Z+		; get graphics data byte

	; r13:r12 = 16 bit field, graph data
	; align screen data (r9:r10) <-> graph data (r13:r12)
        mul     r13,r7		; shift left [r7] times
        mov	r13,r0
	mov	r12,r1

	; graph data are now aligned with display ram data
	eor	r9,r13		; XOR pixels
	eor	r10,r12

	brts	G_D7i		; set if ypos is off-screen: don't store this byte
	st	Y+,r10		; write 2nd byte back to display ram

G_D7i:
	st	X+,r9		; write 1st byte back to display ram and increment pointer

	subi	r19,1		; one data byte processed
	sbc	r8,r3
	breq	G_Drawexit	; if none left, exit

	dec	r5		; "page" done?
	breq	G_D8i		; yes, go to "new page"

	inc	x1		; increment xpos
	cpi	x1,max_c 	; off-screen now?
	brlo	G_D3i		; ** no: same page: process next graph. data byte **

	; yes: xpos off screen: r8:r19 holds (possible) # of bytes still left in this
	; page, we'll skip these and - if  there's still data left -
	; correct the bookeeping (Z-pointer & # of remaining data bytes in r8:r19)
	sub	r19,r5
	sbc	r8,r3		; any data left (!= 0 or minus)?
	breq	g_drawexit	; no, exit
	brmi	g_drawexit	; no, exit

	add	ZL,r5		; increment data pointer
	adc	ZH,r3		; r3 = constant 0

G_D8i:	; new page
	mov	r5,r4		; restore column counter
	mov	x1,r6		; restore x-position

G_D9i:
	ldi	r24,low(g_vscreen)	; on entry into module, init; later
	ldi	r25,high(g_vscreen)	; restore sum(xpos+g_vscreen_start)
	add	r24,x1
	adc	r25,r3		; r3 = constant 0

	subi	y1,-8		; add 8 to y-position (compensated on entry)
	cpi	y1,max_r-8 	; ypos 2nd byte off-screen now?
	brlo	G_D10i		; no
	set			; yes, set T-flag
	cpi	y1,max_r	; ypos 1st byte off-screen now?
	brsh	G_Drawexit	; yes, exit

G_D10i:
	; calculate & load vscreen data offset in XH:XL
	fmul	y1,r20		; x=int(ypos/8)*max_c, ypos in y1
	mul	r2,r1		; *low(max_c)
	movw	XL,r0
	add	XL,r24		; + sum(xpos+g_vscreen_start)
	adc	XH,r25
	movw	YL,XL		; calculate 2nd byte position from X
	add	YL,r2		; and copy it to Y (Y+max_c)
	adc	YH,r3
	rjmp	G_D3i		; ** next page: process next graph. data byte **

; **** main processing loops end ****

g_drawexit:
	clt
	pop	XH
	pop	XL
	pop	YH
	pop	YL
	pop	ZH
	pop	ZL

	pop	r13
	pop	r12
	pop	r11
	pop	r10
	pop	r9
	pop	r8
	pop	r7
	pop	r6
	pop	r5
	pop	r4
	pop	r3
	pop	r2
	pop	r1
	pop	r0

	pop	r24
	pop	r25

	pop	r20
	pop	r19
	pop	y1
	pop	x1

	ret

;**** End of G_Drawpic(1) function ----------------------------------- ****





;**************************************************************************
;*
;* Function G_fillcircle - draw a filled circle
;*
;* Function G_drawcircle - draw a normal circle
;*
;* (Midpoint algorithm, 2nd order differences used to reduce overhead, no
;*  redundant pixel writes)
;*
;* Note: a straight Bresenham circle algorithm (~ 5% faster) is provided
;*	 optionally (see end of module)
;*
;* Call with:
;*	 x1	-> circle center (column), r16
;*	 y1	-> circle center (row),	r18
;*	 radius	-> radius (pixels), r19
;*	 pmode	-> 0=set, 1=clear, -1=invert pixel, r15
;*
;* void g_drawcircle(unsigned char xpos, unsigned char ypos, unsigned char radius)
;* {
;*  int     x, y, d, deltaU, deltaD
;*
;*  x = 0, r3
;*  y = radius; r2
;*  d = 1 - radius;
;*  deltaU  = 3;
;*  deltaD = 5 - radius * 2;
;*
;*  g_write8 ( x, y )
;*
;*  while (y > x) {
;*    if ( d < 0 ) { /* Select U */
;*       d       += deltaU;
;*	 deltaU  += 2;
;*	 deltaD	 += 2;
;*	 x++;
;*    }
;*    else {         /* Select D */
;*       d       += deltaD;
;*	 deltaU  += 2;
;*	 deltaD	 += 4;
;*	 x++;
;*	 y--;
;*      }
;*    g_write8 ( x, y )
;*  }
;* }
;*
;* void g_write8 (int x, int y)
;* {
;*    drx_plot(xpos + x, ypos + y); drx_plot(xpos + x, ypos - y),
;*    drx_plot(xpos - x, ypos + y); drx_plot(xpos - x, ypos - y);
;*    drx_plot(xpos + y, ypos + x); drx_plot(xpos + y, ypos - x);
;*    drx_plot(xpos - y, ypos + x); drx_plot(xpos - y, ypos - x);
;* }
;*
;* All regs saved
;*
;**************************************************************************

G_fillcircle:
	set
	rjmp	PC+2

G_drawcircle:
	clt

	; boundary checks
        cpi     x1,max_c
        brsh    g_drc_bex	; center off-screen, exit
        cpi     y1,max_r
        brsh    g_drc_bex	; center off-screen, exit

	sbrc	r19,7		; radius >= 127?

g_drc_bex:
	ret			; yes, exit

	push	xl	; pointer to frame buffer
	push	xh
	push	yl	; pointer low(g_btable)
	push	yh	; reg var ("0"), high(g_btable)

	push	r0
	push	r1
	push	r2	; y (radius work var)
	push	r3	; x (counter)
	push	r4	; d (Midpoint)
	push	r5	; reg var (0)
	push	r6	; "border" var for partial fills / y2 position
	push	r7	; deltaU (copy of current x for Bresenham)
	push	r8	; deltaD (not used for Bresenham)
	push	r9	; original "border" (left)

	push	r14	; max_c
	;push	r17	; (d for Bresenham, not used for Midpoint option)
	push	r19	; radius at entry
	push	r20	; scrap reg
	push	r23	; reg var ("16")
	push	r24	; v_screen pointer low
	push	r25	; v_screen pointer high

	push	r21	; xpos
	push	r22	; ypos

	; initialize vscreen pointer for drx_plot (as const)
	ldi     r24,low(g_vscreen)
	ldi     r25,high(g_vscreen)

	; initialize shift pointer for drx_plot (as const)
        ldi     YL,low(g_btable)
        ldi     YH,high(g_btable)

	ldi	r23,16		; reg var (16)
	ldi	r20,max_c
	mov	r14,r20		; reg var (max_c)
	clr	r5		; reg var (0)

	; check special case (radius = 0)
	tst	r19		; 0?
	brne	g_drc1		; no, continue
	brtc	g_drc0		; no "normal circle" with radius=0 -> exit
	mov	r21,x1		; filled circle (r=0) has center point
	mov	r22,y1
	rcall	drx_plot	; write single pixel at circle center

g_drc0:
	rjmp	g_drc_exit	; exit

g_drc1:
	rcall	g_drc_init	; init counters and constants
	brts	g_drc_f1	; T-flag set: jump to "filled circle" mode

g_drc2:	; ** normal circle loop -> write 8 pixels **
	; (x = r3, y = r2, xpos = r21, ypos = r22)
	;     xpos =	   ypos =
	; 1: (xcenter + x, ycenter + y)
	mov	r21,x1
	add	r21,r3
	mov	r22,y1
	add	r22,r2
	rcall	drx_plot

	; 2: (xcenter + x, ycenter - y)
	mov	r22,y1
	sub	r22,r2
	rcall	drx_plot

	; 3: (xcenter - x, ycenter + y)
	tst	r3		; avoid redundant writes (if x = 0)
	breq	g_drc_n1

	mov	r21,x1
	sub	r21,r3
	mov	r22,y1
	add	r22,r2
	rcall	drx_plot

	; 4: (xcenter - x, ycenter - y)
	mov	r22,y1
	sub	r22,r2
	rcall	drx_plot

g_drc_n1:
	; 5: (xcenter + y, ycenter + x)
	cp	r3,r2		; avoid redundant writes (if x = y)
	breq	g_drc_n2

	mov	r21,x1
	add	r21,r2
	mov	r22,y1
	add	r22,r3
	rcall	drx_plot

	; 6: (xcenter - y, ycenter + x)
	mov	r21,x1
	sub	r21,r2
	rcall	drx_plot

	; 7: (xcenter + y, ycenter - x)
	tst	r3		; avoid redundant writes (if x = 0)
	breq	g_drc_n2

	mov	r21,x1
	add	r21,r2
	mov	r22,y1
	sub	r22,r3
	rcall	drx_plot

	; 8: (xcenter - y, ycenter - x)
	mov	r21,x1
	sub	r21,r2
	rcall	drx_plot

g_drc_n2:
	rcall	g_drc_inc
	cp	r2,r3		; x <= y ?
	brsh	g_drc2		; ** yes, loop for the next 8 pixels **
	rjmp	g_drc_exit	; no, exit


g_drc_f1:
	; ** filled circle mode: fill inner segments with "v_lines" (byte mode) **
	push	YL		; save regs to be destroyed by drx_vline
	push	YH
	push	XL
	push	XH

	push	r10
	push	r11
	push	r12
	push	r13
	push	r17		; r17: push/pop if midpoint option used (else see below)!

g_drc_f2:			; ** lines loop start **
	push	r7
	push	r8
	;sts	g_struct+1,r17	; r17: store/load if Bresenham option used (else see above)!

	; fill inner right circle segment
	tst	r3		; avoid redundant vertical line at x-center
	breq	g_drc_f3	; if x=0

	mov	r21,x1		; center into xpos (1)
	add	r21,r3		; add current offset (x)
	mov	r22,y1		; ditto ypos + (y)
	sub	r22,r2
	mov	r6,y1		; copy of center (y)
	add	r6,r2		; (y)
	rcall	drx_vline	; draw a vertical line in "byte mode"

g_drc_f3:
	; fill inner left circle segment
	mov	r21,x1		; center into xpos (1)
	sub	r21,r3		; sub current offset (x)
	mov	r22,y1		; ditto ypos - (y)
	sub	r22,r2
	mov	r6,y1		; copy of center (y)
	add	r6,r2		; (y)
	sts	g_struct,r21	; save last inner segment x-pos (call destroys r21)
	rcall	drx_vline	; draw a vertical line in "byte mode"

	;lds	r17,g_struct+1	; r17: store/load if Bresenham option used (else see above)!
	pop	r8
	pop	r7
	rcall	g_drc_inc	; increment x/y positions
	cp	r2,r3		; x <= y ?
	brsh	g_drc_f2	; ** yes, loop for next line in inner segments **

	set			; re-set T-flag because we still are in "fill mode"
	pop	r17		; r17: push/pop if midpoint option used (else see above)!
	pop	r13
	pop	r12
	pop	r11
	pop	r10
	pop	XH
	pop	XL
	pop	YH
	pop	YL

	; x !<= y, continue with outer segments (horizontal fill in "pixel mode" to
	; prevent redundant writes - in second octant we may find a constant "x"
	; value for several pixels)
	rcall	g_drc_init	; reset counters
	lds	r9,g_struct	; fetch last inner segment x-pos as "border" var

g_drc_f5:
	; ** filled circle mode: fill outer segments loop **
	; fill lower left circle segment
	mov	r6,r9		; set up "border" var (left)
	mov	r22,y1		; (6)
	add	r22,r3
	mov	r21,x1
	sub	r21,r2

g_drc_f6:
	rcall	drx_plot	; write a pixel (start at utmost left)
	inc	r21
	cp	r21,r6		; reached "inner border" of line?
	brne	g_drc_f6	; no, loop

	; fill upper left circle segment
	tst	r3		; avoid redundant writes (if x = 0)
	breq	g_drc_f8

	mov	r22,y1		; (8)
	sub	r22,r3
	mov	r21,x1
	sub	r21,r2

g_drc_f7:
	rcall	drx_plot	; write a pixel (start at utmost left)
	inc	r21
	cp	r21,r6		; reached "inner border" of line?
	brne	g_drc_f7	; no, loop

g_drc_f8:
	; fill lower right circle segment
	mov	r6,x1		; set up "border" var (right)
	sub	r6,r9
	add	r6,x1

	mov	r21,x1		; (5)
	add	r21,r2
	mov	r22,y1
	add	r22,r3

g_drc_f9:
	rcall	drx_plot	; write a pixel (start at utmost right)
	dec	r21
	cp	r6,r21		; reached "inner border" of line?
	brlo	g_drc_f9	; no, loop

	; fill upper right circle segment
	tst	r3		; avoid redundant writes (if x = 0)
	breq	g_drc_f11

	mov	r21,x1		; (7)
	add	r21,r2
	mov	r22,y1
	sub	r22,r3

g_drc_f10:
	rcall	drx_plot	; write a pixel (start at utmost right)
	dec	r21
	cp	r6,r21		; reached "inner border" of line?
	brlo	g_drc_f10	; no, loop

g_drc_f11:
	rcall	g_drc_inc
	cp	r2,r3		; x <= y ?
	brsh	g_drc_f5	; ** yes, loop for next line in outer segments **
				; no, exit

g_drc_exit:
	clt			; make sure T-flag is cleared
	pop	r22
	pop	r21

	pop	r25
	pop	r24
	pop	r23
	pop	r20
	pop	r19
	;pop	r17		; (needed for straight Bresenham)
	pop	r14

	pop	r9
	pop	r8
	pop	r7
	pop	r6
	pop	r5
	pop	r4
	pop	r3
	pop	r2
	pop	r1
	pop	r0

	pop	yh
	pop	yl
	pop	xh
	pop	xl

	ret

;** Option1: midpoint algorithm (~ 5% slower, slightly better looking circles) **

g_drc_init:	; init constants and "delta d" constants (midpoint)
	clr	r3		; initialize counter (x)
	mov	r2,r19		; load radius (y) from call parameter
	clr	r4
	inc	r4
	sub	r4,r2		; load d = 1 - radius

	ldi	r20,3
	mov	r7,r20		; load deltaU = 3

	ldi	r20,5
	mov	r8,r20
	mov	r20,r2
	lsl	r20
	sub	r8,r20		; load deltaD = 5 - radius * 2
	ret

g_drc_inc:	; increment x-pos/y-pos logic (midpoint)
	tst	r4		; d < 0 ?
	brmi	g_drc_inc1

	; else d >= 0
	add	r4,r8		; d += deltaU
	inc	r7		; deltaD += 2
	inc	r7
	ldi	r20,4
	add	r8,r20		; deltaU += 4
	dec	r2		; y--
	rjmp	g_drc_inc2

g_drc_inc1:
	; d < 0
	add	r4,r7		; d += deltaD
	inc	r7		; deltaD += 2
	inc	r7
	inc	r8		; deltaU += 2
	inc	r8

g_drc_inc2:
	inc	r3		; x++
	ret


;** Option2: straight Bresenham algorithm (faster, circle shape not as good) **

;g_drc_init:	; init constants (straight Bresenham)
;	clr	r3		; initialize counter (x)
;	mov	r2,r19		; load radius (y) from call parameter

	; calculate d (r17) initial value from radius
;	mov	r17,r2		; d = 3 - (2 * radius)
;	lsl	r17
;	neg	r17
;	subi	r17,-3
;	ret

;g_drc_inc:	; increment x-pos/y-pos logic (straight Bresenham)
;	mov	r7,r3		; copy of current x-counter value
;	tst	r17
;	brmi	g_drc_inc1

	; else d >= 0
;	sub	r7,r2		; d += 4 * (x - y) + 10
;	lsl	r7
;	lsl	r7
;	add	r17,r7
;	subi	r17,-10
;	dec	r2		; y--
;	rjmp	g_drc_inc2

;g_drc_inc1:	; d < 0
;	lsl	r7		; d += (4 * x) + 6
;	lsl	r7
;	add	r17,r7
;	subi	r17,-6

;g_drc_inc2:
;	inc	r3		; x++
;	ret

;**** End functions G_Fillcircle & G_Drawcircle -----------------------****


;**************************************************************************
;*
;* Function G_fillellipse - draw a filled ellipse (no redundant writes)
;*
;* Function G_drawellipse - draw a normal ellipse
;*
;* (Midpoint algorithm)
;*
;*
;* Call with:
;*	 x1	-> ellipse center (column), r16
;*	 y1	-> ellipse center (row), r18
;*	 a	-> radius_X (pixels), r19, max 127
;*	 b	-> radius_Y (pixels), r17, max 127
;*	 pmode	-> 0=set, 1=clear, -1=invert pixel, r15
;*
;*
;* g_struct:				; data struct for g_drawellipse (midpoint):
;*	.BYTE 26			; 00: P  (4 Bytes), also used by "filled circle"
;*					; 04: r1cp / d1S  (4 Bytes)
;*					; 08: b2 (2 Bytes)
;*					; 10: a2 (2 Bytes)
;*					; 12: d2E (2 Bytes)
;*					; 14: d2S (2 Bytes)
;*					; 16: d2SE (2 Bytes)
;*					; 18: d1E (4 Bytes)
;*					; 22: d1SE (4 Bytes)
;*
;* DrawEllipse (xC, yC, a, b)
;* Declare integers x, y, P,		// var & const refer to value's behaviour
;*		    d1E, d1S, d1SE,	// during one call of the function
;*		    d2E, d2S, d2SE
;*
;*	// Common constants for both regions
;*	b2 = b*b			; const, 16bit
;*	a2 = a*a			; const, 16bit
;*	d2E = 2*b2			; const, 16bit
;*	d2S = 2*a2			; const, 16bit
;*	d2SE = d2E + d2S		; const, 16bit
;*	r1cp = d2S + 3*b2		; const, 32bit
;*
;*	// Set initial values in region I
;*	x = 0
;*	y = b
;*	P = b2 + (a2*(1 - 4b) - 2)/4			// -2 to round result
;*	d1E = 3*b2			; var, 32bit
;*	d1SE = d1E - d2S*(b - 1)	; var, 32bit
;*
;*	// Plot the pixels in region I
;*	while d1SE < r1cp
;*	g_write4
;*		if P < 0 then		// Select E
;*			P = P + d1E
;*			d1SE = d1SE + d2E
;*		else			// Select SE
;*			P = P + d1SE
;*			d1SE = d1SE + d2SE
;*			decrement y
;*		end if
;*		d1E = d1E + d2E
;*		increment x
;*	end while
;*
;*	// Set initial values in region II
;*	P = P - (a2*(4y - 3) + b2*(4x + 3) + 2)/4	// +2 to round result
;*	d1S = a2*(3 - 2y)		; var, 32bit
;*	d1SE = 2*b2 + 3*a2		; var, 32bit
;*
;*	// Plot the pixels in region II
;*	while y > 0
;*		if P > 0 then		// Select S
;*			P = P + d1S
;*			d1SE = d1SE + d2S
;*		else			// Select SE
;*			P = P + d1SE
;*			d1SE = d1SE + d2SE
;*			increment x
;*		end if
;*		d1E = d1E + d2S
;*		decrement y
;*		g_write4
;*	end while
;* end DrawEllipse
;*
;* g_write4 (x, y)
;*    drx_plot(xpos + x, ypos + y); drx_plot(xpos + x, ypos - y),
;*    drx_plot(xpos - x, ypos + y); drx_plot(xpos - x, ypos - y);
;*
;* All regs saved
;*
;**************************************************************************

.macro	push_mul
	push	r2
	push	r16
	push	r17
	push	r18
	push	r19
	push	r20
	push	r21
	push	r22
	push	r23
.endm

.macro	pop_mul
	pop	r23
	pop	r22
	pop	r21
	pop	r20
	pop	r19
	pop	r18
	pop	r17
	pop	r16
	pop	r2
.endm

G_fillellipse:
	set
	rjmp	PC+2

G_drawellipse:
	clt

	; boundary checks
        cpi     x1,max_c
        brsh    g_dre_bex	; center off-screen, exit
        cpi     y1,max_r
        brsh    g_dre_bex	; center off-screen, exit

	; prevent overflow
	sbrc	r19,7		; radius_X >= 127?

g_dre_bex:
	ret			; yes, exit
	sbrc	r17,7		; radius_Y >= 127?
	ret			; yes, exit

	; save regs
	push	xl	; pointer to frame buffer
	push	xh
	push	yl	; pointer low(g_btable)
	push	yh	; pointer high(g_btable)

	push	r0	; work regs for mul
	push	r1
	push	r2	; y (current y-offset from center)
	push	r3	; x (current x-offset from center)

	push	r5	; reg var (0)
	push	r6	; (low)P
	push	r7	;
	push	r8	;
	push	r9	; (high)P

	push	r10	;
	push	r11	;
	push	r12	;
	push	r13	;

	push	r14	; max_c
	push	r20	; scrap reg
	push	r23	; reg var ("16")
	push	r24	; pointer v_screen
	push	r25	; pointer v_screen

	push	r21	; xpos
	push	r22	; ypos

	; initialize vscreen pointer for drx_plot / drx_vline (as const)
	ldi     r24,low(g_vscreen)
	ldi     r25,high(g_vscreen)

	; initialize shift pointer for drx_plot (as const)
        ldi     YL,low(g_btable)
        ldi     YH,high(g_btable)

	ldi	r23,16		; reg var (16)
	ldi	r20,max_c
	mov	r14,r20		; reg var (max_c)

	clr	r5	; reg var (0)

	; check special case (one of the radii = 0)
	tst	r19		; rad_X=0?
	brne	g_dre1		; no, continue

	tst	r17		; rad_Y=0?
	brne	g_dre1		; no, continue

	rjmp	g_dre_exit	; exit

g_dre1:
	; init constants and initial values for region I, destroys r0, r1, r6-r13, r20
	; constants:
	; calculate and store a2 & d2S (2*a2)
	mov	r20,r19		; a*a
	mul	r19,r20
	sts	g_struct+10,r0	; store low byte (a2)
	sts	g_struct+11,r1	; store high byte (a2)
	movw	r10,r0		; save a2 to r11:r10

	lsl	r0
	rol	r1		; d2S (2*a2)
	sts	g_struct+14,r0	; store low byte
	sts	g_struct+15,r1	; store high byte
	movw	r6,r0		; save 2*a2 to r7:r6

	; calculate and store b2 & d2E (2*b2)
	mov	r20,r17		; b*b
	mul	r17,r20
	sts	g_struct+8,r0	; store low byte (b2)
	sts	g_struct+9,r1	; store high byte (b2)
	movw	r8,r0		; save b2 to r9:r8

	lsl	r0
	rol	r1		; d2E (2*b2)
	sts	g_struct+12,r0	; store low byte
	sts	g_struct+13,r1	; store high byte

	; calculate and store d2SE (2*b2+2*a2)
	add	r0,r6
	adc	r1,r7
	sts	g_struct+16,r0	; store low byte
	sts	g_struct+17,r1	; store high byte

	; calculate and store r1cp (d2SE + b2)
	clr	r6
	add	r0,r8
	adc	r1,r9
	adc	r6,r5		; r5 = 0; sum is < 24 bits
	sts	g_struct+4,r0	; store low byte (r1cp)
	sts	g_struct+5,r1
	sts	g_struct+6,r6
	sts	g_struct+7,r5 	; store high byte (r1cp)

	; calculate and store initial values for region I (x, y, d1E, d1SE, P)
	clr	r3		; x = 0
	mov	r2,r17		; y = b

	; d1E = 3*b2
	movw	r6,r8		; r9:r8 still has b2
	add	r6,r8
	adc	r7,r9
	add	r6,r8
	adc	r7,r9		; 3*b2

	sts	g_struct+18,r6	; store low byte (d1E)
	sts	g_struct+19,r7
	sts	g_struct+20,r5
	sts	g_struct+21,r5	; store high byte (d1E)

	push_mul		; save regs (r2, r16-r23)

	; d1SE = d1E - d2S*(b - 1)
	lds	r22,g_struct+14	; fetch d2S
	lds	r23,g_struct+15
	mov	r20,r17		; fetch b
	mov	r12,r17		; copy of b (needed for "P" calculation)
	dec	r20		; b - 1
	clr	r21
	call	muls16x16_32	; result in r19->r16

	clr	r8
	sub	r6,r16
	sbc	r7,r17
	sbc	r8,r18
	sbc	r21,r19
	sts	g_struct+22,r6	; store low byte (d1SE)
	sts	g_struct+23,r7
	sts	g_struct+24,r8
	sts	g_struct+25,r21	; store high byte (d1SE)

	; P = b2 + (a2*(1 - 4b) - 2) / 4
	clr	r13
	lsl	r12		; saved b*2
	lsl	r12		; 2b*2, plus carry
	rol	r13		; r13:r12 = 4b
	clr	r21
	ldi	r20,1		; r21:r20 = 0001
	sub	r20,r12
	sbc	r21,r13		; r21:r20 = (1 - 4b)
	movw	r22,r10		; saved a2 (r11:r10) to r23:r22
	call	muls16x16_32	; muls, product in r19->r16
	subi	r16,2
	sbc	r17,r5
	sbc	r18,r5
	sbc	r19,r5		; product - 2
	asr	r19
	ror	r18
	ror	r17
	ror	r16
	asr	r19
	ror	r18
	ror	r17
	ror	r16		; /4 (signed)

	lds	r10,g_struct+8	; fetch low byte (b2)
	lds	r11,g_struct+9	; fetch high byte (b2)
	add	r16,r10
	adc	r17,r11
	adc	r18,r5
	adc	r19,r5		; + b2

	sts	g_struct,r16
	sts	g_struct+1,r17
	sts	g_struct+2,r18
	sts	g_struct+3,r19	; store "P"

	pop_mul			; restore regs

g_dreRx:	; ** main draw module **
	rcall	g_write4

	; fetch current value "d1SE"
	lds	r6,g_struct+22	; low
	lds	r7,g_struct+23
	lds	r8,g_struct+24
	lds	r9,g_struct+25	; high

	; fetch current value "r1cp"
	lds	r10,g_struct+4	; low
	lds	r11,g_struct+5
	lds	r12,g_struct+6
	lds	r13,g_struct+7	; high

	cp	r6,r10
	cpc	r7,r11
	cpc	r8,r12
	cpc	r9,r13
	brlt	g_dreR1		; d1SE < r1cp: work
	rjmp	g_dreR2		; else branch to region 2

g_dreR1:	; ** region 1 loops **
	; fetch current value "P"
	lds	r6,g_struct	; low
	lds	r7,g_struct+1
	lds	r8,g_struct+2
	lds	r9,g_struct+3	; high

	tst	r9
	brmi	g_dreR1_1	; P < 0?

	; fetch current value "d1SE"
	lds	r10,g_struct+22	; low
	lds	r11,g_struct+23
	lds	r12,g_struct+24
	lds	r13,g_struct+25	; high

	; P = P + d1SE
	add	r6,r10
	adc	r7,r11
	adc	r8,r12
	adc	r9,r13

	lds	r0,g_struct+16	; low "d2SE"
	lds	r1,g_struct+17

	; d1SE = d1SE + d2SE
	add	r10,r0
	adc	r11,r1
	adc	r12,r5
	adc	r13,r5

	dec	r2		; dec y
	rjmp	g_dreR1_2

g_dreR1_1:	; P < 0
	; fetch current value "d1E"
	lds	r10,g_struct+18	; low
	lds	r11,g_struct+19
	lds	r12,g_struct+20
	lds	r13,g_struct+21	; high

	; P = P + d1E
	add	r6,r10
	adc	r7,r11
	adc	r8,r12
	adc	r9,r13

	; fetch current value "d1SE"
	lds	r10,g_struct+22	; low
	lds	r11,g_struct+23
	lds	r12,g_struct+24
	lds	r13,g_struct+25	; high

	lds	r0,g_struct+12	; low "d2E"
	lds	r1,g_struct+13

	add	r10,r0		; d1SE = d1SE + d2E
	adc	r11,r1
	adc	r12,r5
	adc	r13,r5

g_dreR1_2: ;endif
	; write back "d1SE"
	sts	g_struct+22,r10
	sts	g_struct+23,r11
	sts	g_struct+24,r12
	sts	g_struct+25,r13

	; write back "P"
	sts	g_struct,r6
	sts	g_struct+1,r7
	sts	g_struct+2,r8
	sts	g_struct+3,r9

	; fetch current value "d1E"
	lds	r6,g_struct+18	; low
	lds	r7,g_struct+19
	lds	r8,g_struct+20
	lds	r9,g_struct+21	; high

	lds	r0,g_struct+12	; low "d2E"
	lds	r1,g_struct+13
	add	r6,r0		; d1E = d1E + d2E
	adc	r7,r1
	adc	r8,r5
	adc	r9,r5

	; write back "d1E"
	sts	g_struct+18,r6
	sts	g_struct+19,r7
	sts	g_struct+20,r8
	sts	g_struct+21,r9
	inc	r3		; inc x
	rjmp	g_dreRx		; loop

g_dreR2:
	; init initial values for region II, destroys r0, r1, r6-r13, r20
	push_mul		; save regs

	push	r19		; save a
	push	r2		; save current y

	; P = P - (a2*(4y - 3) + b2*(4x + 3) + 2) / 4
	; 4y - 3
	clr	r21
	mov	r20,r2		; y into r20 (r2 still ok)
	lsl	r20
	lsl	r20
	rol	r21		; 4y
	subi	r20,3
	sbci	r21,0		; 4y - 3

	lds	r22,g_struct+10	; fetch low byte (a2)
	lds	r23,g_struct+11	; fetch high byte (a2)
	call	muls16x16_32	; muls, product in r19->r16

	movw	r10,r16		; a2*(4y - 3):
	movw	r12,r18		; store this to r13 -> r10

	; 4x + 3
	clr	r21
	mov	r20,r3		; x into r20
	lsl	r20
	lsl	r20
	rol	r21		; 4x
	ldi	r19,3
	add	r20,r19
	adc	r21,r5		; 4x + 3

	lds	r22,g_struct+8	; fetch low byte (b2)
	lds	r23,g_struct+9	; fetch high byte (b2)
	call	muls16x16_32	; muls, [b2*(4x + 3)] in r19->r16

	ldi	r20,2
	add	r16,r20
	adc	r17,r5
	adc	r18,r5
	adc	r19,r5		; b2*(4x + 3) + 2

	add	r16,r10		; add [a2*(4y - 3)] to previous
	adc	r17,r11
	adc	r18,r12
	adc	r19,r13

	asr	r19
	ror	r18
	ror	r17
	ror	r16
	asr	r19
	ror	r18
	ror	r17
	ror	r16		; sum /4 (signed)

	; fetch current value "P"
	lds	r6,g_struct	; low
	lds	r7,g_struct+1
	lds	r8,g_struct+2
	lds	r9,g_struct+3	; high

	sub	r6,r16
	sbc	r7,r17
	sbc	r8,r18
	sbc	r9,r19		; P = P - [sum /4 (signed)]

	; write back new "P" for region 2
	sts	g_struct,r6
	sts	g_struct+1,r7
	sts	g_struct+2,r8
	sts	g_struct+3,r9

	; d1S = a2*(3 - 2y)
	pop	r22		; saved y into r22
	lsl	r22		; 2y
	ldi	r20,3
	clr	r21		; r21:r20 = 0003
	sub	r20,r22
	sbc	r21,r5		; r21:r20 = (3 - 2y), signed

	lds	r22,g_struct+10	; fetch low byte (a2)
	lds	r23,g_struct+11	; fetch high byte (a2)
	call	muls16x16_32	; muls, [a2*(3 - 2y)] in r19->r16

	sts	g_struct+4,r16	; store low byte (r1cp/d1S)
	sts	g_struct+5,r17
	sts	g_struct+6,r18
	sts	g_struct+7,r19	; store high byte (r1cp/d1S)

	; d1SE = 2*b2 + 3*a2
	lsl	r22		; r23:r22 still has a2
	rol	r23		; 2*a2
	pop	r20		; retrieve a
	add	r22,r20
	adc	r23,r5		; r23:r22 = 3*a2

	lds	r20,g_struct+8	; fetch low byte (b2)
	lds	r21,g_struct+9	; fetch high byte (b2)
	add	r20,r22
	adc	r21,r23
	clr	r22
	adc	r22,r5		; r22:r21:r20 = 2*b2 + 3*a2

	sts	g_struct+22,r20	; store low byte (d1SE)
	sts	g_struct+23,r21
	sts	g_struct+24,r22
	sts	g_struct+25,r5	; store high byte (d1SE)

	pop_mul			; restore regs

g_dreR2_1:	; ** region 2 loops **
	tst	r2
	brne	g_dreR2_2	; y > 0, work
	rjmp	g_dre_exit	; else exit

g_dreR2_2:
	; fetch current value "P"
	lds	r6,g_struct	; low
	lds	r7,g_struct+1
	lds	r8,g_struct+2
	lds	r9,g_struct+3	; high

	cp	r6,r5		; r9 to r6 = 0 ?
	cpc	r7,r5
	cpc	r8,r5
	cpc	r9,r5
	breq	g_dreR2_3	; P = 0
	brmi	g_dreR2_3	; P < 0

	; P > 0
	; fetch current value "d1S"
	lds	r10,g_struct+4	; low
	lds	r11,g_struct+5
	lds	r12,g_struct+6
	lds	r13,g_struct+7	; high

	add	r6,r10
	adc	r7,r11
	adc	r8,r12
	adc	r9,r13		; P = P + d1S

	; fetch current value "d1SE"
	lds	r10,g_struct+22	; low
	lds	r11,g_struct+23
	lds	r12,g_struct+24
	lds	r13,g_struct+25	; high

	; fetch d2S
	lds	r0,g_struct+14
	lds	r1,g_struct+15
	add	r10,r0
	adc	r11,r1
	adc	r12,r5
	adc	r13,r5		; d1SE = d1SE + d2S
	rjmp	g_dreR2_4

g_dreR2_3:	; P <= 0
	; fetch current value "d1SE"
	lds	r10,g_struct+22	; low
	lds	r11,g_struct+23
	lds	r12,g_struct+24
	lds	r13,g_struct+25	; high

	add	r6,r10
	adc	r7,r11
	adc	r8,r12
	adc	r9,r13		; P = P + d1SE

	; fetch d2SE
	lds	r0,g_struct+16
	lds	r1,g_struct+17
	add	r10,r0
	adc	r11,r1
	adc	r12,r5
	adc	r13,r5		; d1SE = d1SE + d2SE

	inc	r3		; inc x

g_dreR2_4:	;endif
	; write back "d1SE"
	sts	g_struct+22,r10
	sts	g_struct+23,r11
	sts	g_struct+24,r12
	sts	g_struct+25,r13

	; write back "P"
	sts	g_struct,r6
	sts	g_struct+1,r7
	sts	g_struct+2,r8
	sts	g_struct+3,r9

	; fetch current value "d1E"
	lds	r6,g_struct+18	; low
	lds	r7,g_struct+19
	lds	r8,g_struct+20
	lds	r9,g_struct+21	; high

	lds	r0,g_struct+14	; low "d2S"
	lds	r1,g_struct+15
	add	r6,r0		; d1E = d1E + d2S
	adc	r7,r1
	adc	r8,r5
	adc	r9,r5

	; write back "d1E"
	sts	g_struct+18,r6
	sts	g_struct+19,r7
	sts	g_struct+20,r8
	sts	g_struct+21,r9

	dec	r2		; dec y
	rcall	g_write4
	rjmp	g_dreR2_1	; loop

g_dre_exit:			; exit functions
	clt			; make sure T-flag is reset
	pop	r22
	pop	r21

	pop	r25
	pop	r24
	pop	r23
	pop	r20
	pop	r14
	pop	r13
	pop	r12
	pop	r11
	pop	r10
	pop	r9
	pop	r8
	pop	r7
	pop	r6
	pop	r5
	pop	r3
	pop	r2
	pop	r1
	pop	r0

	pop	yh
	pop	yl
	pop	xh
	pop	xl

	ret

g_write4:	; ** write pixels **
	brts	g_fill4		; go to "fill ellipse" if T-flag is set

	; (x = r3, y = r2, xpos = r21, ypos = r22)
	;     xpos =	   ypos =
	; 1: (xcenter + x, ycenter + y)
	mov	r21,x1
	add	r21,r3
	mov	r22,y1
	add	r22,r2
	rcall	drx_plot

	; 2: (xcenter + x, ycenter - y)
	tst	r2		; avoid redundant writes (if y = 0)
	breq	g_dre_w2
	mov	r22,y1
	sub	r22,r2
	rcall	drx_plot

g_dre_w2:
	; 3: (xcenter - x, ycenter + y)
	tst	r3		; avoid redundant writes (if x = 0)
	breq	g_dre_w1

	mov	r21,x1
	sub	r21,r3
	mov	r22,y1
	add	r22,r2
	rcall	drx_plot

	; 4: (xcenter - x, ycenter - y)
	tst	r2		; avoid redundant writes (if y = 0)
	breq	g_dre_w1
	mov	r22,y1
	sub	r22,r2
	rcall	drx_plot

g_dre_w1:
	ret

g_fill4:	; ** filled ellipse loop -> write 2 v_lines per pixel **
	tst	r2		; skip drawing lines if y = 0
	breq	g_dre_f3

	push	r17		; save regs to be destroyed by drx_vline
	push	YL
	push	YH
	push	XL
	push	XH

	; Line1: (xcenter + x, ycenter - y) to (xcenter + x, ycenter + y)
	; calc endpoint in 2nd quadrant
	mov	r6,y1		; get y-center (re-use r6 as counter)
	add	r6,r2		; add current y = end point ypos
	mov	r21,x1
	mov	r22,y1
	sub	r22,r2
	add	r21,r3		; start point 1st quadrant xpos,ypos -> r21:r22
	rcall	drx_vline	; write line in "byte mode"

g_dre_f1:
	; Line2: (xcenter - x, ycenter - y) to (xcenter - x, ycenter + y)
	; calc endpoint in 3rd quadrant
	tst	r3		; skip drawing line if x = 0
	breq	g_dre_f2

	mov	r6,y1		; get y-center (re-use r6 as counter)
	add	r6,r2		; add current y = end point ypos
	mov	r21,x1
	mov	r22,y1
	sub	r22,r2
	sub	r21,r3		; start point 4th quadrant xpos,ypos -> r21:r22
	rcall	drx_vline	; write line in "byte mode"

g_dre_f2:
	set			; re-set T-flag because we are still in "fill mode"
	pop	XH
	pop	XL
	pop	YH
	pop	YL
	pop	r17

g_dre_f3:
	ret

;*** End of Midpoint ellipse ----------------------------------------------


;**************************************************************************
;*
;* Function G_fillellipse - draw a filled ellipse
;*
;* Function G_drawellipse - draw a normal ellipse
;*
;* (Bresenham algorithm, 1/3 slower than midpoint, but also ~1/3 less code)
;*
;*
;* Call with:
;*	 x1	-> ellipse center (column), r16
;*	 y1	-> ellipse center (row),	r18
;*	 a	-> radius_X (pixels), r19
;*	 b	-> radius_Y (pixels), r17
;*	 pmode	-> 0=set, 1=clear, -1=invert pixel, r15
;*
;*
;* g_struct:				; data struct for g_drawellipse:
;*	.BYTE 18			; 00: S  (4 Bytes)
;*					; 04: T  (4 Bytes)
;*					; 08: 2*a2 (2 Bytes)
;*					; 10: 2*b2 (2 Bytes)
;*					; 12: 4*a2 (2 Bytes)
;*					; 14: 4*b2 (2 Bytes)
;*					; 16: ref. copy of x
;*					; 17: ref. copy of y
;*
;* void bresenham_ellipse(int a, int b, int cx, int cy, int pmode)
;* {
;*  long int S, T,a2,b2;
;*  int x,y; (x=r3, y=r2)
;*
;*  a2 = a*a;			/* squares as constants */
;*  b2 = b*b;
;*  x = 0;
;*  y = b;
;*  S = a2*(1-2*b) + 2*b2;	/* initial value */
;*  T = b2 - 2*a2*(2*b-1);	/* initial value */
;*
;*  g_write4 (x, y);
;*
;*  do
;*    {
;*     if (S<0)
;*        {
;*         S += 2*b2*(2*x+3);	/* module ST1 */
;*         T += 4*b2*(x+1);
;*         x++;
;*        }
;*       else if (T<0)
;*           {
;*            S += 2*b2*(2*x+3) - 4*a2*(y-1);	/* module ST2 */
;*            T += 4*b2*(x+1) - 2*a2*(2*y-3);
;*            g_write4 (x, y);
;*            x++;
;*            y--;
;*
;*           }
;*          else
;*           {
;*            S -= 4*a2*(y-1);	/* module ST3 */
;*            T -= 2*a2*(2*y-3);
;*            g_write4 (x, y);
;*            y--;
;*           }
;*    }
;*  while (y>0);
;* }
;*
;*
;* void g_write4 (int x, int y)
;* {
;*    drx_plot(xpos + x, ypos + y); drx_plot(xpos + x, ypos - y),
;*    drx_plot(xpos - x, ypos + y); drx_plot(xpos - x, ypos - y);
;* }
;*
;* All regs saved
;*
;**************************************************************************

.macro	push_mul
	push	r2
	push	r16
	push	r17
	push	r18
	push	r19
	push	r20
	push	r21
	push	r22
	push	r23
.endm

.macro	pop_mul
	pop	r23
	pop	r22
	pop	r21
	pop	r20
	pop	r19
	pop	r18
	pop	r17
	pop	r16
	pop	r2
.endm


.macro	Load_S
	; fetch current value "S"
	lds	r6,g_struct	; low
	lds	r7,g_struct+1
	lds	r8,g_struct+2
	lds	r9,g_struct+3	; high
.endm

.macro	Store_S
	; write back "S"
	sts	g_struct,r6
	sts	g_struct+1,r7
	sts	g_struct+2,r8
	sts	g_struct+3,r9
.endm

.macro	Load_T
	; fetch current value "T"
	lds	r6,g_struct+4	; low
	lds	r7,g_struct+5
	lds	r8,g_struct+6
	lds	r9,g_struct+7	; high
.endm

.macro	Store_T
	; write back "T"
	sts	g_struct+4,r6
	sts	g_struct+5,r7
	sts	g_struct+6,r8
	sts	g_struct+7,r9
.endm



G_fillellipse:
	set
	rjmp	PC+2

G_drawellipse:
	clt

	; boundary checks
        cpi     x1,max_c
        brsh    g_dre_bex	; center off-screen, exit
        cpi     y1,max_r
        brsh    g_dre_bex	; center off-screen, exit

	; prevent overflow during math
	sbrc	r19,7		; radius_X >= 127?

g_dre_bex:
	ret			; yes, exit
	sbrc	r17,7		; radius_Y >= 127?
	ret			; yes, exit

	; save regs
	push	xl	; pointer to frame buffer
	push	xh
	push	yl	; pointer low(g_btable)
	push	yh	; reg var ("0"), high(g_btable)

	push	r0	; work regs for mul
	push	r1
	push	r2	; y (current y-offset from center)
	push	r3	; x (current x-offset from center)
	push	r4	; const. reg
	push	r5	; const. reg (0)

	push	r6	; (low)S/T
	push	r7	;
	push	r8	;
	push	r9	; (high)S/T
	push	r10	; reg var

	push	r14	; (low)max_c
	push	r20	; scrap reg
	push	r23	; reg var ("16")
	push	r24	; pointer v_screen
	push	r25	; pointer v_screen

	push	r21	; xpos
	push	r22	; ypos

	; initialize vscreen pointer for drx_plot (as const)
	ldi     r24,low(g_vscreen)
	ldi     r25,high(g_vscreen)

	; initialize shift pointer for drx_plot (as const)
        ldi     YL,low(g_btable)
        ldi     YH,high(g_btable)

	ldi	r23,16		; reg var (16)
	ldi	r20,max_c
	mov	r14,r20		; reg var (max_c)
	clr	r5		; reg var (0)

	; check special case (radii = 0)
	tst	r19		; rad_X=0?
	brne	g_dre_init	; no, continue

	tst	r17		; rad_Y=0?
	brne	g_dre_init	; no, continue
	rjmp	g_dre_exit	; exit

g_dre_init:	; init constants and initial values, destroys r6-r9, r20
	; calculate and store 2*a2 & 4*a2
	mov	r20,r19		; a*a
	mul	r19,r20
	movw	r6,r0		; a2
	lsl	r6
	rol	r7		; 2*a2
	sts	g_struct+8,r6	; store low byte
	sts	g_struct+9,r7	; store high byte
	lsl	r6
	rol	r7		; 4*a2
	sts	g_struct+12,r6	; store low byte
	sts	g_struct+13,r7	; store high byte

	; calculate and store 2*b2 & 4*b2
	mov	r20,r17		; b*b
	mul	r17,r20
	movw	r6,r0		; b2
	lsl	r6
	rol	r7		; 2*b2
	sts	g_struct+10,r6	; store low byte
	sts	g_struct+11,r7	; store high byte
	lsl	r6
	rol	r7		; 4*b2
	sts	g_struct+14,r6	; store low byte
	sts	g_struct+15,r7	; store high byte

	; calculate and store initial values for S & T (32 bit #s, signed)
	; S = a2*(1-2*b) + 2*b2
	push_mul		; save regs
	mov	r20,r19		; a*a
	mul	r19,r20
	movw	r22,r0		; r23:r22 = a2
	mov	r6,r17
	lsl	r6
	clr	r7
	rol	r7		; r7:r6 = 2*b
	clr	r9
	clr	r8
	inc	r8		; r9:r8 = 0001
	sub	r8,r6
	sbc	r9,r7		; r9:r8 = 1-2*b
	movw	r20,r8		; mov it to r21:r20 for mul signed
	call	muls16x16_32	; result in r19->r16
	lds	r20,g_struct+10	; load 2*b2
	lds	r21,g_struct+11
	add	r16,r20		; add it to signed product
	clr	r20
	adc	r17,r21
	adc	r18,r20
	adc	r19,r20
	sts	g_struct,r16
	sts	g_struct+1,r17
	sts	g_struct+2,r18
	sts	g_struct+3,r19	; store initial value "S"

	; T = b2 - 2*a2*(2*b-1)
	lds	r20,g_struct+8	; load 2*a2
	lds	r21,g_struct+9
	movw	r22,r6		; load 2*b to r23:r22
	subi	r22,1
	sbci	r23,0		; r23:r22 = 2*b-1
	call	muls16x16_32
	lds	r20,g_struct+10	; load 2*b2
	lds	r21,g_struct+11
	lsr	r21
	ror	r20		; r21:r20 = b2
	clr	r22
	clr	r23
	sub	r20,r16
	sbc	r21,r17
	sbc	r22,r18
	sbc	r23,r19		; b2 - product into r23->r20 (result minus)
	sts	g_struct+4,r20
	sts	g_struct+5,r21
	sts	g_struct+6,r22
	sts	g_struct+7,r23	; store initial value "T"

	pop_mul			; restore regs

	ldi	r20,3		; constants for additions (STx-modules)
	mov	r4,r20
	ldi	r20,1
	mov	r10,r20

	clr	r3		; x = 0
	sts	g_struct+16,r3	; make copy
	mov	r2,r17		; y = b
	sts	g_struct+17,r2	; make copy

g_dre2:	; ** main loop **
	cp	r2,r5		; y > 0 ?
	brpl	g_write4	; ** yes, loop for the next increment **
	rjmp	g_dre_exit	; no, exit

g_write4:	; ** normal ellipse: write 4 pixels **
	brts	g_fill4		; filled ellipse mode if T-flag set

	; (x = r3, y = r2, xpos = r21, ypos = r22)
	;     xpos =	   ypos =
	; 1: (xcenter + x, ycenter + y)
	mov	r21,x1
	add	r21,r3
	mov	r22,y1
	add	r22,r2
	rcall	drx_plot

	; 2: (xcenter + x, ycenter - y)
	tst	r2		; avoid redundant writes (if y = 0)
	breq	g_dre_w2
	mov	r22,y1
	sub	r22,r2
	rcall	drx_plot

g_dre_w2:
	; 3: (xcenter - x, ycenter + y)
	tst	r3		; avoid redundant writes (if x = 0)
	breq	g_dre_w1

	mov	r21,x1
	sub	r21,r3
	mov	r22,y1
	add	r22,r2
	rcall	drx_plot

	; 4: (xcenter - x, ycenter - y)
	tst	r2		; avoid redundant writes (if y = 0)
	breq	g_dre_w1
	mov	r22,y1
	sub	r22,r2
	rcall	drx_plot

g_dre_w1:
	rjmp	g_dre_f3	; continue after fill mode code


g_fill4:	; ** filled ellipse loop -> write 2 v_lines per pixel **
	tst	r2		; skip drawing lines if y = 0
	breq	g_dre_f3

	push	r17		; save regs to be destroyed by drx_vline
	push	YL
	push	YH
	push	XL
	push	XH

	; Line1: (xcenter + x, ycenter - y) to (xcenter + x, ycenter + y)
	; calc endpoint in 2nd quadrant
	mov	r6,y1		; get y-center (re-use r6 as counter)
	add	r6,r2		; add current y = end point ypos
	mov	r21,x1
	mov	r22,y1
	sub	r22,r2
	add	r21,r3		; start point 1st quadrant xpos,ypos -> r21:r22
	rcall	drx_vline	; write line in "byte mode"

g_dre_f1:
	; Line2: (xcenter - x, ycenter - y) to (xcenter - x, ycenter + y)
	; calc endpoint in 3rd quadrant
	tst	r3		; skip drawing line if x = 0
	breq	g_dre_f2

	mov	r6,y1		; get y-center (re-use r6 as counter)
	add	r6,r2		; add current y = end point ypos
	mov	r21,x1
	mov	r22,y1
	sub	r22,r2
	sub	r21,r3		; start point 4th quadrant xpos,ypos -> r21:r22
	rcall	drx_vline	; write line in "byte mode"

g_dre_f2:
	set			; re-set T-flag because we are still in "fill mode"
	pop	XH
	pop	XL
	pop	YH
	pop	YL
	pop	r17


g_dre_f3:			; end of normal/filled ellipse pixel writes

	lds	r20,g_struct+3	; fetch MSB "S"
	sbrs	r20,7		; <0 (negative) if bit 7 set
	rjmp	g_dre_n1

	;module	ST1
	; S += 2*b2*(2*x+3);	/* module ST1 */
	; T += 4*b2*(x+1);
	push_mul

	Load_S			; fetch current value "S"
	lds	r20,g_struct+10	; fetch 2*b2
	lds	r21,g_struct+11
	clr	r23		; "x" (high) = 0
	lds	r22,g_struct+16	; fetch "x" (low)
	lsl	r22		; 2*x
	rol	r23		; word *2 to be safe ...
	add	r22,r4		; 2*x + 3
	adc	r23,r5
	call	muls16x16_32
	add	r6,r16		; add result to previous "S"
	adc	r7,r17
	adc	r8,r18
	adc	r9,r19
	Store_S			; write back new "S"

	Load_T			; fetch current value "T"
	lds	r20,g_struct+14	; fetch 4*b2
	lds	r21,g_struct+15
	clr	r23		; "x" (high) = 0
	lds	r22,g_struct+16	; fetch "x" (low)
	add	r22,r10		; x + 1
	adc	r23,r5
	call	muls16x16_32
	add	r6,r16		; add result to previous "T"
	adc	r7,r17
	adc	r8,r18
	adc	r9,r19
	Store_T			; write back new "T"

	pop_mul
	;endmodule

	inc	r3
	sts	g_struct+16,r3	; save new value
	rjmp	g_dre2		; loop

g_dre_n1:
	lds	r20,g_struct+7	; fetch MSB "T"
	sbrs	r20,7		; <0 (negative) if bit 7 set
	rjmp	g_dre_n2

	;module	ST2
	; S += 2*b2*(2*x+3) - 4*a2*(y-1);	/* module ST2 */
	; T += 4*b2*(x+1) - 2*a2*(2*y-3);
	push_mul

	Load_S			; fetch current value "S"
	lds	r20,g_struct+10	; fetch 2*b2
	lds	r21,g_struct+11
	clr	r23		; "x" (high) = 0
	lds	r22,g_struct+16	; fetch "x" (low)
	lsl	r22		; 2*x
	rol	r23		; word *2 to be safe ...
	add	r22,r4		; 2*x + 3
	adc	r23,r5
	call	muls16x16_32
	add	r6,r16		; add result to previous "S"
	adc	r7,r17
	adc	r8,r18
	adc	r9,r19

	lds	r20,g_struct+12	; fetch 4*a2
	lds	r21,g_struct+13
	clr	r23		; "y" (high) = 0
	lds	r22,g_struct+17	; fetch "y" (low)
	subi	r22,1		; y - 1
	sbci	r23,0
	call	muls16x16_32
	sub	r6,r16		; subtract result from previous "S"
	sbc	r7,r17
	sbc	r8,r18
	sbc	r9,r19
	Store_S			; write back new "S"

	Load_T			; fetch current value "T"
	lds	r20,g_struct+14	; fetch 4*b2
	lds	r21,g_struct+15
	clr	r23		; "x" (high) = 0
	lds	r22,g_struct+16	; fetch "x" (low)
	add	r22,r10		; x + 1
	adc	r23,r5
	call	muls16x16_32
	add	r6,r16		; add result to previous "T"
	adc	r7,r17
	adc	r8,r18
	adc	r9,r19

	lds	r20,g_struct+8	; fetch 2*a2
	lds	r21,g_struct+9
	clr	r23		; "y" (high) = 0
	lds	r22,g_struct+17	; fetch "y" (low)
	lsl	r22		; 2*y
	rol	r23		; word *2 to be safe ...
	subi	r22,3		; 2*y - 3
	sbci	r23,0
	call	muls16x16_32
	sub	r6,r16		; subtract result from previous "T"
	sbc	r7,r17
	sbc	r8,r18
	sbc	r9,r19
	Store_T			; write back new "T"

	pop_mul
	;endmodule

	inc	r3
	sts	g_struct+16,r3	; save new value
	dec	r2
	sts	g_struct+17,r2	; save new value
	rjmp	g_dre2		; loop

g_dre_n2:
	;module	ST3
	; S -= 4*a2*(y-1);	/* module ST3 */
	; T -= 2*a2*(2*y-3);
	push_mul

	Load_S			; fetch current value "S"
	lds	r20,g_struct+12	; fetch 4*a2
	lds	r21,g_struct+13
	clr	r23		; "y" (high) = 0
	lds	r22,g_struct+17	; fetch "y" (low)
	subi	r22,1		; y - 1
	sbci	r23,0
	call	muls16x16_32
	sub	r6,r16		; subtract result from previous "S"
	sbc	r7,r17
	sbc	r8,r18
	sbc	r9,r19
	Store_S			; write back new "S"

	Load_T			; fetch current value "T"
	lds	r20,g_struct+8	; fetch 2*a2
	lds	r21,g_struct+9
	clr	r23		; "y" (high) = 0
	lds	r22,g_struct+17	; fetch "y" (low)
	lsl	r22		; 2*y
	rol	r23		; word *2 to be safe ...
	subi	r22,3		; 2*y - 3
	sbci	r23,0
	call	muls16x16_32
	sub	r6,r16		; subtract result from previous "T"
	sbc	r7,r17
	sbc	r8,r18
	sbc	r9,r19
	Store_T			; write back new "T"

	pop_mul
	;endmodule

	dec	r2
	sts	g_struct+17,r2	; save new value
	rjmp	g_dre2		; loop

g_dre_exit:			; ** exit functions **
	clt			; make sure T-flag is reset
	pop	r22
	pop	r21

	pop	r25
	pop	r24
	pop	r23
	pop	r20
	pop	r14

	pop	r10
	pop	r9
	pop	r8
	pop	r7
	pop	r6
	pop	r5
	pop	r4
	pop	r3
	pop	r2
	pop	r1
	pop	r0

	pop	yh
	pop	yl
	pop	xh
	pop	xl

	ret

;*** End of Bresenham ellipse ---------------------------------------------


;**************************************************************************
;*
;* Subfunction drx_vline - vertical line draw routine
;*
;* Internally called by g_fill[ellipse/circle]
;* Call with x-pos (r21), y-pos1 (r22), y-pos2 (r6)
;*
;* Not to be called separately!
;*
;* Destroys r0, r1, r6-r13, r20, r21, r22 (if push/pop = true)
;* Note: r25:r24 must point to "g_vscreen"
;*
;**************************************************************************

drx_vline:
	; check x-boundary on entry
	cpi	r21,max_c
	brlo	Drx_V1
	ret

Drx_V1:
; These regs already saved by caller
;	push	r17	; # of pixels in last data byte
;	push	YL	; secondary pointer display ram
;	push	YH
;	push	XL	; main pointer display ram
;	push	XH

	clt			; make sure T-flag is cleared

Drx_V2:	; calculate the number of graphics bytes ("line data")
        mov	r20,r6		; calculate "heigth" info
	sub	r20,r22		; # of pixels
	inc	r20		; needed because ypos start offset = 0

	; calculate # of line pixels in last data byte
	mov	r17,r20
	andi	r17,7		; r17 = # of line pixels in last data byte
				; (if r17 = 0, all pixels are set!)
        subi    r20,-7		; number of bytes = (heigth+7)/mod 8
        lsr     r20		; add 7 to heigth and divide by mod 8
        lsr     r20
        lsr     r20
	mov	r6,r20		; r6 = # of line data bytes

	ldi	r20,max_c	; max_c value as register variable
	mov	r11,r20

	; register variables status now::
	; r5	-> 0
	; r11	-> max_c
	; r6	-> # of databytes in bitmap, acts also as counter
	; r22	-> start row (ypos)

	; calculate bit offset start ypos <-> start 1st screen byte ypos
	mov	r20,r22		; r8=sram(bytab_offset+mod(ypos/8))
	andi	r20,0x07

	ldi	YL,low(g_btable)
	ldi	YH,high(g_btable)
	add	YL,r20		; offset into table
	ld	r8,Y		; load appropriate shift multiplier

	; calculate & load vscreen data offset in XH:XL
	ldi	r20,16		; value for /8 with fmul.
	fmul	r22,r20		; x=int(ypos/8)*max_c, ypos in r22
	mul	r11,r1		; *low(max_c)
	movw	XL,r0
	mov	r20,r25
	add	r21,r24		; xpos
	adc	r20,r5
	add	XL,r21		; + sum(xpos+g_vscreen_start)
	adc	XH,r20
	movw	YL,XL		; calculate 2nd byte position from X

	add	YL,r11		; and copy it to Y (Y+max_c)
	adc	YH,r5

	ldi	r20,1		; =1, new reg var value

Drx_D1:	; **** main processing loop start ****
	cpi	r22,max_r-8 	; ypos 2nd byte off-screen?
	brlo	Drx_D2		; no
	set			; yes, set T-flag
	cpi	r22,max_r	; ypos 1st byte off-screen?
	brsh	Drx_vl_exit	; yes, exit

Drx_D2:	; get 2 bytes of vscreen which will receive new pixel values
	ld	r9,X		; get 1st display ram byte (r9)
	ld	r10,Y		; get 2nd display ram byte (r10)
	clr	r13		; set line data
	com	r13
	cp	r6,r20		; r20=1 - already last data byte?
	brne	Drx_D4		; no, assume all pixels set
	tst	r17		; last data byte, no remainder?
	breq	Drx_D4		; yes, assume all pixels set
	subi	r17,8
	neg	r17		; (8 - r17) for lsr

Drx_D3:	lsr	r13		; else set remaining pixels
	dec	r17		; (actually: clear surplus pixels)
	brne	Drx_D3

Drx_D4:
	; r13:r12 = 16 bit field, graph data
	; align screen data (r9:r10) <-> line data (r13:r12)
        mul     r13,r8		; shift left [r8] times
        movw	r12,r0

	; line data are now aligned with display ram data
	; determine pixel mode, change pixels accordingly
	tst	pmode		; pmode=0/1/-1 -> set/clear/XOR pixel
	brne	Drx_D5
	or	r9,r12		; set pixels
	or	r10,r13
	rjmp	Drx_D7

Drx_D5:
	brmi	Drx_D6
	com	r12		; clear pixels
	and	r9,r12
	com	r13
	and	r10,r13
	rjmp	Drx_D7

Drx_D6:
	eor	r9,r12		; XOR pixels
	eor	r10,r13

Drx_D7:
	brts	Drx_D8		; set if ypos is off-screen: don't store this byte
	st	Y,r10		; write 2nd byte back to display ram

Drx_D8:
	st	X,r9		; write 1st byte back to display ram
	dec	r6		; one line data byte gone, decrement counter
	breq	Drx_vl_exit	; if none left, exit

	subi	XL,-max_c	; else increment vscreen pointers to next page
	sbci	XH,0xFF		; at same xpos
	subi	YL,-max_c
	sbci	YH,0xFF

	subi	r22,-8		; add 8 to y-position
	rjmp	Drx_D1		; ** process next line data byte **

; **** main processing loop end ****

Drx_vl_exit:
;	set			; needed here because we are in "fill mode"
;	pop	XH
;	pop	XL
;	pop	YH
;	pop	YL
;	pop	r17

	ret

;*** End of subfunction drx_vline -----------------------------------------


;**************************************************************************
;*
;* Subfunction drx_plot - pixel draw routine
;*
;* Internally called by g_draw/fill[ellipse/circle]
;*
;* Not to be called separately!
;* Destroys r0, r1, r20, X
;*
;**************************************************************************

drx_plot:
        cpi     r21,max_c	; boundary checks
        brsh    dre_pl3
        cpi     r22,max_r
        brsh    dre_pl3

	; x=int(ypos/8)*max_c+xpos+g_vscreen_start
	fmul	r22,r23		; int(ypos/8) -> r23 = 16
	mul	r14,r1		; * max_c -> r14 = max_c
        movw    XL,r0
        add     XL,r21		; + xpos
	adc	XH,r5		; -> r5 = 0

        add     XL,r24		; + g_vscreen_start
        adc     XH,r25

        ld      r0,X            ; get display ram byte

        mov     r20,r22		; r22 = y_pos
        andi    r20,0x07
        add     YL,r20          ; pointer to g_btable offset
				; NOTE: table's SRAM low byte adr. must be <=0xF8
				; as "adc YH,0" is omitted for performance reasons
        ld      r1,Y		; r1 now =sram(bytab_offset+mod(ypos/8))
	sub	YL,r20		; reset pointer offset, cf. NOTE above

        tst     pmode           ; pmode=0  set pixel
        brne    dre_pl1         ; pmode=1  clear pixel
                                ; pmode=-1 XOR pixel
        or      r0,r1           ; set pixel
	st      X,r0            ; stuff it back to display ram
	ret

dre_pl1:
	brmi    dre_pl2
        com     r1              ; clear pixel
        and     r0,r1
	st      X,r0            ; stuff it back to display ram
	ret

dre_pl2:
	eor     r0,r1           ; XOR pixel
       	st      X,r0            ; stuff it back to display ram

dre_pl3:
        ret

;*** End of subfunction drx_plot ------------------------------------------


;**************************************************************************
;*
;* Function G_drawline - draws a line (Bresenham algorithm)
;* (This asm code by R. Kalle)
;*
;* Call with: x1,y1,x2,y2,pmode
;*
;* All regs saved
;*
;* Draws a line between (x1,y1) and (x2,y2). X and Y range is 0x00 to 0xff,
;* (only linesegments inside the X and Y range of 0-(max_c-1) and 0-(max_r-1)
;* are drawn)
;*
;* For a black line set pmode=0
;* For a white line set pmode=1
;* For a reversed line set pmode=-1
;*
;**************************************************************************

g_drawline:
	push	XL
	push	XH
	push	YL
	push	YH
	push	ZL
	push	ZH

	push	r0
	push	r1
	push	r2
	push	r3
	push	r4
	push	r5
	push	r6
	push	r7
	push	r8
	push	r9
	push	r10
	push	r11

	push	r20
	push	r23

	push	x1
	push	y1

	; initialize vscreen pointer for drc_plot (will not be modified)
	ldi     YL,low(g_vscreen)
	ldi     YH,high(g_vscreen)

	; initialize shift pointer for drc_plot (will not be modified)
        ldi     ZL,low(g_btable)
        ldi     ZH,high(g_btable)

	ldi	r23,16		; reg var
	ldi	r20,low(max_c)
	mov	r10,r20		; reg var
	clr	r11		; reg var (0)

	ldi	r20,1
	mov	r2,r20
	mov	r3,r20
	mov	r4,r20
	mov	r5,r20
	ldi	r20,-1

	mov	r6,x2		; r6=x2-x1
	sub	r6,x1
	brpl	g_dr1
	mov	r2,r20
	mov	r3,r20
	neg	r6		; r6=-r6

g_dr1:	mov	r7,y2		; r7=y2-y1
	sub	r7,y1
	brpl	g_dr2
	mov	r4,r20
	mov	r5,r20
	neg	r7		; r7=-r7

g_dr2:	cp	r6,r7
	brsh	g_dr3		; r6>=r7 ?
	clr	r2		; r2=0
	mov	r0,r6		; swap r6,r7
	mov	r6,r7
	mov	r7,r0

g_dr5:	mov	r9,r6		; r9=r6+r7
	add	r9,r7
	mov	r8,r6		; r8=r6
	rjmp	g_dr4

g_dr3:	clr	r4		; r4=0
	rjmp	g_dr5

g_dr4: 	lsl	r6		; r6=r6*2
	lsl	r7		; r7=r7*2

g_dr6:	; ** pixel draw routine, destroys r0, r1, r20, X **
        cpi     x1,max_c	; boundary checks
        brsh    g_dr13
        cpi     y1,max_r
        brsh    g_dr13

	; x=int(y1/8)*max_c+x1+g_vscreen_start
	fmul	y1,r23		; int(y1/8) -> r23 = 16
	mul	r10,r1		; *low(max_c) -> r10 = (low)max_c
        movw    XL,r0

        add     XL,x1		; +x1
	adc	XH,r11		; r11 = 0!
        add     XL,YL		; +g_vscreen_start
        adc     XH,YH

        ld      r0,X            ; get display ram byte

        mov     r20,y1
        andi    r20,0x07
        add     ZL,r20          ; pointer to table offset
        ld      r1,Z		; r1 now = sram(bytab_offset+mod(y1/8))
        sub	ZL,r20		; reset pointer
        tst     pmode           ; pmode=0  set pixel
        brne    g_dr10          ; pmode=1  clear pixel
                                ; pmode=-1 XOR pixel
        or      r0,r1           ; set pixel
        rjmp    g_dr12

g_dr10:
	brmi    g_dr11
        com     r1              ; clear pixel
        and     r0,r1
        rjmp    g_dr12

g_dr11:
	eor     r0,r1           ; XOR pixel

g_dr12:
	st      X,r0            ; stuff it back to display ram

g_dr13:	; ** end pixel draw routine **
       	clr	r20
	tst	r9
	breq	g_dr8		; r9=0 ?

	sub	r8,r7		; r8=r8-r7
	sbci	r20,0
	brpl	g_dr7		; r8>0?

	add	x1,r3		; x1=x1+r3
	add	y1,r5		; y1=y1+r5
	add	r8,r6		; r8=r8+r6
	sbci	r20,-0
	dec	r9		; r9=r9-2
	dec	r9
	rjmp	g_dr6

g_dr7:
	add	x1,r2		; x1=x1+r2
	add	y1,r4		; y1=y1+r4
	dec	r9		; r9=r9-1
	rjmp	g_dr6

g_dr8:
	pop	y1
	pop	x1
	pop	r23
	pop	r20

	pop	r11
	pop	r10
	pop	r9
	pop	r8
	pop	r7
	pop	r6
	pop	r5
	pop	r4
	pop	r3
	pop	r2
	pop	r1
	pop	r0

	pop	ZH
	pop	ZL
	pop	YH
	pop	YL
	pop	XH
	pop	XL

	ret

;**** End function G_Drawline -----------------------------------------****


;**************************************************************************
;*
;* Function G_Vline - draw a vertical line
;*
;* Call with:
;*	 x1	-> start column of line (xpos), r16
;*	 y1	-> start row of line (ypos1), r18
;*	 y2	-> end row of line (ypos2), r19
;*
;* 	 For a black line set pmode=0
;* 	 For a white line set pmode=1
;* 	 For a reversed line set pmode=-1
;*
;* All regs saved, T-flag destroyed
;*
;* Note: Line is regarded as "1 column wide" picture
;*	 (bitmap width and heigth in pixels). Pixels in data bytes are
;*	 arranged vertically, 8 pixels per column, LSB -> MSB.
;*	 ypos1 doesn't need to be < ypos2
;*	 Max virtual screen size 256x256 pixels
;*
;*
;**************************************************************************

G_Vline:
	; check x-boundary on entry
	cpi	x1,max_c
	brlo	VL_V1
	ret

VL_V1:
	push	y1	; line start ypos
	push	r17	; # of pixels in last data byte
	push	r19	; line end ypos (y2, bottom) / # of data bytes
	push	r20	; scrap reg

	push	r25	; r25:r24 pointer to "sum(xpos+VL_vscreen_start)"
	push	r24

	push	r0	; r0, r1 work regs (mul, fmul)
	push	r1
	push	r2	; max_c low
	push	r3	; max_c high (= 0 in this implementation)
	push	r8	; ypos offset (shift multiplier)
	push	r9	; 1st display ram byte
	push	r10	; 2nd display ram byte
	push	r12	; line data fill space
	push	r13	; line data

	push	YL	; secondary pointer display ram
	push	YH
	push	XL	; main pointer display ram
	push	XH

	clt		; make sure T-flag is cleared

	; make sure y1<y2
	mov	r20,r19		; y2 into r20
	cp	y2,y1
	brsh	VL_V2		; ok, continue
	breq	VL_V2		; ypos = 0 and y1 = y2 are legal, too
	mov	r20,y1
	mov	y1,r19		; r19 = y2
	mov	r19,r20

VL_V2:	; calculate the number of graphics bytes ("line data")
        			; calculate "heigth" info
	sub	r20,y1		; # of pixels
	inc	r20		; needed because ypos start offset = 0

	; calculate # of line pixels in last data byte
	mov	r17,r20
	andi	r17,7		; r17 = # of line pixels in last data byte
				; (if r17 = 0, all pixels are set!)
        subi    r20,-7		; number of bytes = (heigth+7)/mod 8
        lsr     r20		; add 7 to heigth and divide by mod 8
        lsr     r20
        lsr     r20
	mov	r19,r20		; r19 = # of line data bytes

	clr	r3		; max_c(high) =0, used as reg. var
	ldi	r20,max_c	; max_c(low) value as register variable
	mov	r2,r20

	; register variables status now::
	; r3	-> 0 ("(high)max_c")
	; r2	-> max_c
	; r19	-> # of databytes in bitmap, acts also as counter
	; y1	-> start row (ypos)

	; calculate bit offset start ypos <-> start 1st screen byte ypos
	mov	r20,y1		; r8=sram(bytab_offset+mod(ypos/8))
	andi	r20,0x07

	ldi	YL,low(g_btable)
	ldi	YH,high(g_btable)
	add	YL,r20		; offset into table
	ld	r8,Y		; load appropriate shift multiplier
	ldi	r20,16		; from here on r20 as reg var.

	ldi	r24,low(g_vscreen)	; init sum(xpos+g_vscreen_start)
	ldi	r25,high(g_vscreen)
	add	r24,x1
	adc	r25,r3		; r3 = constant 0

	; calculate & load vscreen data offset in XH:XL
	fmul	y1,r20		; x=int(ypos/8)*max_c, ypos in y1
	mul	r2,r1		; *low(max_c)
	movw	XL,r0
	add	XL,r24		; + sum(xpos+g_vscreen_start)
	adc	XH,r25
	movw	YL,XL		; calculate 2nd byte position from X
	add	YL,r2		; and copy it to Y (Y+max_c)
	adc	YH,r3

VL_D1:	; **** main processing loop start ****
	cpi	y1,max_r-8 	; ypos 2nd byte off-screen?
	brlo	VL_D2		; no
	set			; yes, set T-flag
	cpi	y1,max_r	; ypos 1st byte off-screen?
	brsh	vline_exit	; yes, exit

VL_D2:	; get 2 bytes of vscreen which will receive new pixel values
	ld	r9,X		; get 1st display ram byte (r9)
	ld	r10,Y		; get 2nd display ram byte (r10)
	clr	r13		; set line data
	com	r13
	cpi	r19,1		; already last data byte?
	brne	VL_D4		; no, assume all pixels set
	tst	r17		; last data byte, no remainder?
	breq	VL_D4		; yes, assume all pixels set
	subi	r17,8
	neg	r17		; (8 - r17) for lsr

VL_D3:	lsr	r13		; else set remaining pixels
	dec	r17		; (actually: clear surplus pixels)
	brne	VL_D3

VL_D4:
	; r13:r12 = 16 bit field, graph data
	; align screen data (r9:r10) <-> line data (r13:r12)
        mul     r13,r8		; shift left [r8] times
        movw	r12,r0

	; line data are now aligned with display ram data
	; determine pixel mode, change pixels accordingly
	tst	pmode		; pmode=0/1/-1 -> set/clear/XOR pixel
	brne	VL_D5
	or	r9,r12		; set pixels
	or	r10,r13
	rjmp	VL_D7

VL_D5:
	brmi	VL_D6
	com	r12		; clear pixels
	and	r9,r12
	com	r13
	and	r10,r13
	rjmp	VL_D7

VL_D6:
	eor	r9,r12		; XOR pixels
	eor	r10,r13

VL_D7:
	brts	VL_D8		; set if ypos is off-screen: don't store this byte
	st	Y,r10		; write 2nd byte back to display ram

VL_D8:
	st	X,r9		; write 1st byte back to display ram
	dec	r19		; one line data byte gone, decrement counter
	breq	Vline_exit	; if none left, exit

	subi	XL,-max_c	; else increment vscreen pointers to next page
	sbci	XH,0xFF		; at same xpos
	subi	YL,-max_c
	sbci	YH,0xFF

	subi	y1,-8		; add 8 to y-position
	rjmp	VL_D1		; ** process next line data byte **

; **** main processing loop end ****

Vline_exit:
	clt
	pop	XH
	pop	XL
	pop	YH
	pop	YL

	pop	r13
	pop	r12
	pop	r10
	pop	r9
	pop	r8
	pop	r3
	pop	r2
	pop	r1
	pop	r0

	pop	r24
	pop	r25

	pop	r20
	pop	r19
	pop	r17
	pop	y1

	ret

;**** End of G_Vline function ---------------------------------------- ****


;**************************************************************************
;*
;* Function G_Hline - draw a horizontal line ("generic version")
;*
;* Call with:
;*	 x1	-> start column of line (xpos1), r16
;*	 x2	-> end column of line (xpos2), r17
;*	 y1	-> start row of line (ypos), r18
;*
;* 	 For a black line set pmode=0
;* 	 For a white line set pmode=1
;* 	 For a reversed line set pmode=-1
;*
;* All regs saved
;*
;* Note: xpos1 doesn't need to be < xpos2
;*	 Max virtual screen size 256x256 pixels
;*
;*
;**************************************************************************

G_Hline:
	; check y-boundary on entry
	cpi	y1,max_r
	brlo	HL_V1
	ret

HL_V1:
	push	r22	; line start ypos (r18)
	push	r21	; current x-pos
	push	x1	; r16
	push	x2	; r17
	push	r20	; scrap reg

	push	r25	; r25:r24 pointer to "sum(xpos+g_vscreen_start)"
	push	r24
	push	r23	; reg var
	push	r14	; reg var

	push	r0	; r0, r1 work regs (mul, fmul)
	push	r1
	push	r2
	push	r5	; reg var (0)
	push	YL	; secondary pointer display ram
	push	YH
	push	XL	; main pointer display ram
	push	XH

	; generic H-line function: make sure x1<x2
	mov	r20,x2		; x2 into r20
	cp	x2,x1
	brsh	HL_V2		; ok, continue
	breq	HL_V2		; xpos = 0 and x1 = x2 are legal, too
	mov	r20,x1		; else exchange x1 <-> x2
	mov	x1,x2		; r17 = x2
	mov	x2,r20

HL_V2:	; initialize vscreen pointer for drx_plot (as const)
	ldi     r24,low(g_vscreen)
	ldi     r25,high(g_vscreen)

	; initialize shift pointer for drx_plot (as const)
        ldi     YL,low(g_btable)	; low byte SRAM address must be <=0xF8 (!)
        ldi     YH,high(g_btable)

	ldi	r23,16		; reg var (16)
	ldi	r20,max_c
	mov	r14,r20		; reg var (max_c)
	mov	r22,y1		; load y-position (const. for horizontal line)
	mov	r21,x1		; load 1st x-position
	clr	r5		; reg var (0) for drx_plot

	; calculate # of pixels to write
	mov	r2,x2
	sub	r2,x1

HL_D1:	; write r2 pixels
	rcall	drx_plot
	inc	r21
	dec	r2
	brpl	HL_D1

Hline_exit:
	pop	XH
	pop	XL
	pop	YH
	pop	YL

	pop	r5
	pop	r2
	pop	r1
	pop	r0

	pop	r14
	pop	r23
	pop	r24
	pop	r25

	pop	r20
	pop	x2
	pop	x1
	pop	r21
	pop	r22

	ret

;**** End of G_Hline function ---------------------------------------- ****


;**************************************************************************
;*
;* Function G_drawframe - draws a rectangle
;*
;* Call with: x1,y1,x2,y2,pmode
;*
;* All regs saved
;*
;* Draws a rectangle between opposing corners (x1,y1)|(x2,y2).
;* X and Y range is 0x00 to 0xff,
;* (only linesegments inside the X and Y range of 0-(max_c-1) and 0-(max_r-1)
;* are drawn)
;*
;* For a black rectangle set pmode=0
;* For a white rectangle set pmode=1
;* For a reversed rectangle set pmode=-1
;*
;**************************************************************************

g_drawframe:
	push	XL
	push	XH
	push	YL
	push	YH
	push	x1
	push	x2
	push	y1
	push	y2

	mov	XL,x1
	mov	XH,x2
	mov	YL,y1
	mov	YH,y2

	mov	x1,xl
	mov	x2,xh
	mov	y1,yl
	mov	y2,yl

	inc	x1		; short the hor. lines, g_vline is faster
	dec	x2
	rcall	g_hline
	mov	y1,yh
	mov	y2,yh

	rcall	g_hline
	mov	x1,xl
	mov	x2,xl
	mov	y1,yl
	mov	y2,yh

	rcall	g_vline
	mov	x1,xh
	mov	x2,xh
	rcall	g_vline

	pop	y2
	pop	y1
	pop	x2
	pop	x1
	pop	YH
	pop	YL
	pop	XH
	pop	XL
	ret

;**** End function G_Drawframe ----------------------------------------****


;**************************************************************************
;*
;* Function G_Fillframe - draws a filled rectangle
;*
;* Call with: x1,y1,x2,y2,pmode
;*
;* All regs saved
;*
;* Draws a filled rectangle between opposite corners (x1,y1 | x2,y2)
;* X and Y range is 0x00 to 0xff, only linesegments inside the X and Y
;* range of 0-(max_c-1) and 0-(max_r-1) are drawn
;*
;* For a black filled rectangle set pmode=0
;* For a white filled rectangle set pmode=1
;* For a reversed filled rectangle set pmode=-1
;*
;**************************************************************************

G_Fillframe:
	push	r2
	push	x1
	push	y1
	push	y2

	cp	x2,x1		; determine x-direction
	brsh	g_fifi

g_fifd:	; dec x value
	mov	r2,x1
	sub	r2,x2
	rcall	g_vline
	dec	x1
	dec	r2
	brpl	PC-3
	rjmp	g_fifex

g_fifi:	; inc x value
	mov	r2,x2
	sub	r2,x1
	rcall	g_vline
	inc	x1
	dec	r2
	brpl	PC-3

g_fifex:
	pop	y2
	pop	y1
	pop	x1
	pop	r2
	ret

;**** End function G_Fillframe ----------------------------------------****


;**************************************************************************
;*
;* Function G_readxy - reads a single pixel
;*
;* Call with:
;*	 x1,y1	-> pixel coordinates
;*
;* Returns:
;*	 c=1 if pixel is set
;*	 c=0 if pixel cleared
;*
;* Function G_plotxy - writes a single pixel
;*
;* Call with:
;*	 x1,y1	-> pixel coordinates
;* 	 For a black pixel set pmode=0
;* 	 For a white pixel set pmode=1
;* 	 For a reversed pixel set pmode=-1
;*
;* Returns:
;*	 -
;*
;* All regs saved, T-flag cleared
;*
;**************************************************************************

G_readxy:
	set
	rjmp	PC+2

G_plotxy:
	clt
	push	XL
	push	XH
	push	YH
	push	ZL
	push	ZH

	push	r0
	push	r1
	push	r5

	cpi	x1,max_c	; boundary checks
	brsh	g_pl4
	cpi	y1,max_r
	brsh	g_pl4

	ldi	ZL,low(g_btable)
	ldi	ZH,high(g_btable)
	clr	r5		; reg var (0)

	mov	r0,y1		; x=int(y1/8)*max_c+x1+g_vscreen_start
	lsr	r0
	lsr	r0
	lsr	r0
	ldi	YH,max_c
	mul	YH,r0
	movw	XL,r0

	add	XL,x1
	adc	XH,r5		; r5 = 0
	ldi	YH,low(g_vscreen)
	add	XL,YH
	ldi	YH,high(g_vscreen)
	adc	XH,YH

	ld	r0,X		; get display ram byte

	mov	YH,y1		; YH=sram(bytab_offset+mod(y1/8))
	andi	YH,0x07
	add	ZL,YH		; offset into table
	ld	r1,Z

	brtc	g_pl0		; T-flag cleared: write pixel
	clt
	and	r0,r1		; T-flag set: read pixel and ...
	clc
	breq	g_pl4		; ... exit with carry cleared

	sec
	rjmp	g_pl4		; ... exit with carry set

g_pl0:
	tst	pmode		; pmode=0  set pixel
	brne	g_pl1		; pmode=1  clear pixel
				; pmode=-1 XOR pixel
	or	r0,r1		; set pixel
	rjmp	g_pl3

g_pl1:	brmi	g_pl2

	com	r1		; clear pixel
	and	r0,r1
	rjmp	g_pl3

g_pl2:	eor	r0,r1		; XOR pixel

g_pl3:	st	X,r0		; stuff it back to display ram

g_pl4:
	pop	r5
	pop	r1
	pop	r0
	pop	ZH
	pop	ZL
	pop	YH
	pop	XH
	pop	XL

	ret

;**** End functions G_plot/readxy -------------------------------------****


;**************************************************************************
;*
;* Several flood/seed fill routines (G_Seedp_V is the fastest one with the
;* SED 13/15xx family)
;*
;**************************************************************************

;**************************************************************************
;*
;* Function G_Seedp - *fast* polygon fill
;*
;* Author: Andreas Lenze, 6/2003
;* ('straight assembler' implementation of P. Heckbert's seed fill algorithm)
;*
;* Call with:
;*	 x1,y1	-> seed pixel coordinates (must be inside area)
;*
;* Returns:
;*	 -
;*
;* Note: Mr Heckbert's 'seed fill' is  per default at least twice
;*	 as fast as any recursive 4-way-fill. Stack size demands also
;*	 are extremely low when compared to the usual textbook flood fill
;*	 algorithms ("4-way fill", "boundary scan fill" etc.) which
;*	 tend to gobble up most of an MCU's internal RAM for their
;*	 stack requirements.
;*
;*	 This 'straight assembler' version does the seed fill 'pixel by
;*	 pixel' (Heckbert's original design) on horizontal scan lines
;*	 and is the (so to speak) 'least hardware-dependent' implementation.
;*	 It is comparatively slow (as compared to "G_Seedp_V" or even
;*	 "G_Seedp_H") and provided here mainly to show a 'clean' assembler
;*	 implementation of the algorithm.
;*
;*	 For highest speed, go for "G_Seedp_V" which takes full advantage
;*	 of the SED family memory organization design (227% speed-up).
;*
;*	 If you feel really brave, some or all boundary checks could be
;*	 omitted, too (another ~20% speed-up). With all the checks active,
;*	 the routine is 'crash-proof' in every respect.
;*
;* Note 2:
;* Define the additional array for this module:
;*
;* .equ	fillbuf_length=0x0100	; size of circular buffer (64-xxx bytes,
;*				  *must* be divisible by 4!)
;*
;* fillbuf_base:
;*	.byte fillbuf_length	; reserves fillbuf_length bytes for the
;*				; buffer and is used as the buffer뭩 base address
;*
;* All regs saved
;*
;* ------------------------------------------------------------------------
;*
;* Paul Heckbert's 'seed fill' algorithm ('fill.c' from "Graphics Gems", 1990)
;*
;* typedef struct {		/* window: a discrete 2-D rectangle */
;*     int x0, y0;		/* xmin and ymin */
;*     int x1, y1;		/* xmax and ymax (inclusive) */
;* } Window;
;*
;* typedef int Pixel;		/* 1-channel frame buffer assumed */
;*
;* Pixel pixelread();
;*
;* typedef struct {short y, xl, xr, dy;} Segment;
;* /*
;*  * Filled horizontal segment of scanline y for xl<=x<=xr.
;*  * Parent segment was on line y-dy.  dy=1 or -1
;*  */
;* #define MAX 10000		/* max depth of stack -> extremely conservative figure, */
;*				/* on a small embedded system expect ~ 1/50 of that */
;*
;* #define PUSH(Y, XL, XR, DY)	/* push new segment on stack */ \
;*     if (sp<stack+MAX && Y+(DY)>=win->y0 && Y+(DY)<=win->y1) \
;*     {sp->y = Y; sp->xl = XL; sp->xr = XR; sp->dy = DY; sp++;}
;*
;* #define POP(Y, XL, XR, DY)	/* pop segment off stack */ \
;*     {sp--; Y = sp->y+(DY = sp->dy); XL = sp->xl; XR = sp->xr;}
;*
;* fill(x, y, win, nv)
;* int x, y;	/* seed point */
;* Window *win;	/* screen window, resp. screen boundaries */
;* Pixel nv;	/* new pixel value */
;* {
;*     int l, x1, x2, dy;
;*     Pixel ov;	/* old pixel value */
;*     Segment stack[MAX], *sp = stack;	/* stack of filled segments */
;*
;*     ov = pixelread(x, y);		/* read pv at seed point */
;*     if (ov==nv || x<win->x0 || x>win->x1 || y<win->y0 || y>win->y1) return;
;*     PUSH(y, x, x, 1);			/* needed in some cases */
;*     PUSH(y+1, x, x, -1);		/* seed segment (popped 1st) */
;*
;*     while (sp>stack) {
;* 	/* pop segment off stack and fill a neighboring scan line */
;* 	POP(y, x1, x2, dy);
;* 	/*
;* 	 * segment of scan line y-dy for x1<=x<=x2 was previously filled,
;* 	 * now explore adjacent pixels in scan line y
;* 	 */
;* 	for (x=x1; x>=win->x0 && pixelread(x, y)==ov; x--)
;* 	    pixelwrite(x, y, nv);
;* 	if (x>=x1) goto skip;
;* 	l = x+1;
;* 	if (l<x1) PUSH(y, l, x1-1, -dy);		/* leak on left? */
;* 	x = x1+1;
;* 	do {
;* 	    for (; x<=win->x1 && pixelread(x, y)==ov; x++)
;* 		pixelwrite(x, y, nv);
;* 	    PUSH(y, l, x-1, dy);
;* 	    if (x>x2+1) PUSH(y, x2+1, x-1, -dy);	/* leak on right? */
;* skip:	    for (x++; x<=x2 && pixelread(x, y)!=ov; x++);
;* 	    l = x;
;* 	} while (x<=x2);
;*     }
;* }
;*
;* internal equates:
;* x = r16, -> x1 from caller
;* y = r18, -> y1 from caller
;* x1 = r6
;* x2 = r7
;* l = r5
;* dy = r17
;*
;**************************************************************************

.macro Get_Pix	; read a pixel
	; read pixel
	fmul	r18,r23		; int(ypos/8) -> r23 = 16, r18 = y1
	mul	r8,r1		; * max_c -> r8 = max_c
        movw    XL,r0
        add     XL,r16		; + xpos
	adc	XH,r9		; -> r9 = 0
        add     XL,r24		; + g_vscreen_start
        adc     XH,r25

        mov     r20,r18
        andi    r20,0x07	; ypos bit # in display RAM
        add     YL,r20          ; pointer to table offset
        ld      r1,Y		; r1= sram(bytab_offset+mod(ypos/8))
	sub	YL,r20		; reset pointer offset

        ld      r0,X            ; get display ram byte
	mov	r20,r0		; saves 1 cycle (no reload of display ram byte)
	and	r20,r1		; read pixel status, equal=cleared, not equal=set
.endm

.macro push_s	; push a segment
	; stack ('fillbuf') overflow check
	;ldi	r4,high(fillbuf_base + fillbuf_length) ; provided as reg var!
	cpi	ZL,low(fillbuf_base + fillbuf_length)
	cpc	ZH,r4		; check private stack -
	brlo	PC+2
	rjmp	mp_sex		; if full, exit macro w/o store

	; boundary checks
	mov	r19,@0		; copy Y to scrap
	add	r19,@3		; Y+DY
	brmi	mp_sex		; if Y+DY <0 (y_min) exit macro w/o store

mp_s1:
	cpi	r19,max_r	; if Y+DY <=max_r-1 (y_max), cont.
	brlo	mp_s2
	rjmp	mp_sex		; else exit macro w/o store

mp_s2:
	st	Z+,@0		; Y
	st	Z+,@1		; XL
	st	Z+,@2		; XR
	st	Z+,@3		; DY
mp_sex:
.endm

.macro pop_s	; pop a segment
	ld	@3,-Z		; DY
	ld	@2,-Z		; XR
	ld	@1,-Z		; XL
	ld	@0,-Z		; Y
	add	@0,@3		; Y=Y+DY
.endm


G_Seedp:
	cpi	r16,max_c-1		; boundary checks,
	brsh	g_fl1			; exit if seed pixel is off-screen
	cpi	r18,max_r-1
	brsh	g_fl1
	rjmp	PC+2			; continue
g_fl1:	ret

	push	XL
	push	XH
	push	YL	; pointer (low)g_table
	push	YH	; pointer (high)g_table
	push	ZL	; Z = private stack pointer
	push	ZH

	push	r0	; mul/fmul results
	push	r1
	push	r2	; r3:r2 fillbuf_base
	push	r3
	push	r4	; reg var
	push	r5	; internal l
	push	r6	; internal x1
	push	r7	; internal x2
	push	r8	; reg var
	push	r9	; reg var (0)
	push	r16	; internal x, (caller's x1 -> seed pixel xpos
	push	r17	; internal dy
	push	r18	; internal y, (caller's y1 -> seed pixel xpos
	push	r19	; scrap inside push_s macro
	push	r20	; general scrap
	push	r23	; reg var
	push	r24	; r25:24 pointer v_screen start
	push	r25


	; common constants - don't touch! (Y, r2-r4, r8, r9, r23, r24, r25)
	ldi	YL,low(g_btable)
	ldi	YH,high(g_btable)
	clr	r9			; reg var (0)

	ldi	r20,max_c
	mov	r8,r20			; reg var

	ldi	r20,high(fillbuf_base + fillbuf_length)
	mov	r4,r20			; reg var for push_s macro

	; initialize vscreen pointer
	ldi     r24,low(g_vscreen)
	ldi     r25,high(g_vscreen)

	ldi	r23,16			; reg var for Get_Pix macro

	ldi	ZL,low(fillbuf_base)	; start private stack
	ldi	ZH,high(fillbuf_base)

	mov	r3,ZH			; copy for stack compare
	mov	r2,ZL

	; check seed pixel, must be cleared
	Get_Pix	; read pixel status, equal=cleared, not equal=set
	breq	g_fl2			; if seed pixel cleared, continue
	rjmp	g_flexit		; else exit

g_fl2:	; seed pixel is clear, continue
	ldi	r17,1			; dy
	push_s 	r18, r16, r16, r17	; PUSH(y, x, x, 1): seed pixel, filled last

	inc	r18
	ldi	r17,-1
	push_s 	r18, r16, r16, r17	; PUSH(y+1, x, x, -1): seed segment (popped 1st)


g_fl3:	; ** main loop start ('while' stack not empty) **
	cp	ZL,r2		; stack pointer low/high:
	cpc	ZH,r3		; check if stack empty
	brne	g_fl4		; no, continue
	rjmp	g_flexit	; stack empty, exit

g_fl4:
	pop_s	r18, r6, r7, r17	; POP(y, x1, x2, dy): pop a segment off stack

	; segment of scan line y-dy for x1<=x<=x2 was previously filled,
	; now explore adjacent pixels in current scan line y
	mov	r16,r6		; x=x1

g_fl5:	; start of 1st 'for' loop
	tst	r16
	brmi	g_fl6		; exit first 'for' loop if xpos < min_c (0)

	Get_Pix	; read pixel status, equal=cleared, not equal=set
	brne	g_fl6		; if pixel already set, exit 1st 'for' loop

	or	r0,r1		; else set pixel
	st	X,r0		; store it back to display ram

	dec	r16		; x--
	rjmp	g_fl5		; else loop through line

g_fl6:
	cp	r16,r6		; x>=x1 ?
	brlt	g_fl6a
	rjmp	g_fl11		; goto 'skip' (too far for 'brxx')

g_fl6a:
	mov	r5,r16
	inc	r5		; l = x+1

	cp	r5,r6		; ** leak on left: l<x1 ?
	brsh 	g_fl7		; no, skip push job

	dec	r6		; dec x1, push segment
	neg	r17		; -dy
	push_s	r18, r5, r6, r17	; PUSH(y, l, x1-1, -dy)
	neg	r17		; reset dy
	inc	r6		; reset x1

g_fl7:
	mov	r16,r6
	inc	r16		; x = x1+1

g_fl8:	; start of 'do' loop
	; start of 2nd 'for' loop
	cpi	r16,max_c	; x off-screen?
	brsh	g_fl9		; yes, exit 2nd 'for' loop

	Get_Pix	; read pixel status, equal=cleared, not equal=set
	and	r20,r1		; read pixel status, equal=cleared, not equal=set
	brne	g_fl9		; if pixel already set, exit 2nd 'for' loop
	or	r0,r1		; else set pixel
	st	X,r0		; store it back to display ram

	inc	r16		; x++
	rjmp	g_fl8		; loop through line

g_fl9:
	dec	r16		; x-1, push segment data
	push_s	r18, r5, r16, r17	; PUSH(y, l, x-1, dy)

	cp	r7,r16		; ** leak on right: x-1>x2 ? (-> same as x>x2+1)
	brsh	g_fl10		; no, skip next push job
	inc	r7		; x2+1
	neg	r17		; -dy
	push_s	r18, r7, r16, r17	; PUSH(y, x2+1, x-1, -dy)
	neg	r17		; reset dy
	dec	r7		; reset x2

g_fl10:
	inc	r16		; reset x-1 to x

g_fl11:	; 'skip' label, also start of 3rd 'for' loop
	inc	r16		; x++

g_fl12:
	cp	r7,r16		; x<=x2 ?
	brlt	g_fl13		; no, exit 3rd 'for' loop

	Get_Pix	; read pixel status, equal=cleared, not equal=set
	breq	g_fl13		; if pixel cleared, exit 3rd 'for' loop
	inc	r16		; x++
	rjmp	g_fl12		; loop through 3rd 'for' loop

g_fl13:
	mov	r5,r16		; l=x
	cp	r7,r16		; x<=x2 ?
	brsh	PC+2		; yes, loop (rjmps needed, too far for 'brxx')
	rjmp	g_fl3		; no, pop next segment (** main loop end **)
	rjmp	g_fl8		; x<=x2, continue 'do' loop

g_flexit:
	pop	r25
	pop	r24
	pop	r23
	pop	r20
	pop	r19
	pop	r18
	pop	r17
	pop	r16

	pop	r9
	pop	r8
	pop	r7
	pop	r6
	pop	r5
	pop	r4
	pop	r3
	pop	r2
	pop	r1
	pop	r0

	pop	ZH
	pop	ZL
	pop	YH
	pop	YL
	pop	XH
	pop	XL

	ret

;**** End function G_Seedp --------------------------------------------****


;**************************************************************************
;*
;* Function G_Seedp_H - *faster* polygon fill (some hardware optimization)
;*
;* Author: Andreas Lenze, 6/2003
;* (SED13/15xx family implementation of P. Heckbert's seed fill algorithm)
;*
;* Call with:
;*	 x1,y1	-> seed pixel coordinates (must be inside area)
;*
;* Returns:
;*	 -
;*
;* Note: Mr Heckbert's 'seed fill' is  per default at least twice
;*	 as fast as any recursive 4-way-fill. Stack size demands also
;*	 are extremely low when compared to the usual textbook flood fill
;*	 algorithms ("4-way fill", "boundary scan fill" etc.) which
;*	 tend to gobble up most of an MCU's internal RAM for their
;*	 stack requirements.
;*
;*	 This 'horizontal scan line' variant is optimized for the SED13/15xx
;*	 vertical video memory organization: although *horizontal* scan lines
;*	 can only be filled in 'single bit mode' we still can reduce overhead
;*	 by directly accessing the 'next' display ram byte w/o having to re-
;*	 calculate ypos and v_screen byte position ('y' stays constant while
;*	 moving through the same scan line, the next byte's position is easily
;*	 deducted if the memory organization is known). Using these principles
;*	 we can speed up the routine by factor 1.67 (compared to the 'straight
;*	 assembler' variant). Of course, even the 'straight' assembler version
;*	 will typically be considerably faster than compiler generated code ...
;*
;*	 If you feel really brave, some or all boundary checks could be
;*	 omitted, too (another ~20% speed-up). With all the checks active,
;*	 the routine is 'crash-proof' in every respect.
;*
;* Note 2:
;* Define the additional array for this module:
;*
;* .equ	fillbuf_length=0x0100	; size of circular buffer (64-xxx bytes,
;*				  *must* be divisible by 4!)
;*
;* fillbuf_base:
;*	.byte fillbuf_length	; reserves fillbuf_length bytes for the
;*				; buffer and is used as the buffer뭩 base address
;*
;* All regs saved
;*
;* ------------------------------------------------------------------------
;*
;* Paul Heckbert's 'seed fill' algorithm ('fill.c' from "Graphics Gems", 1990)
;*
;* typedef struct {		/* window: a discrete 2-D rectangle */
;*     int x0, y0;		/* xmin and ymin */
;*     int x1, y1;		/* xmax and ymax (inclusive) */
;* } Window;
;*
;* typedef int Pixel;		/* 1-channel frame buffer assumed */
;*
;* Pixel pixelread();
;*
;* typedef struct {short y, xl, xr, dy;} Segment;
;* /*
;*  * Filled horizontal segment of scanline y for xl<=x<=xr.
;*  * Parent segment was on line y-dy.  dy=1 or -1
;*  */
;* #define MAX 10000		/* max depth of stack -> extremely conservative figure, */
;*				/* on a small embedded system expect ~ 1/50 of that */
;*
;* #define PUSH(Y, XL, XR, DY)	/* push new segment on stack */ \
;*     if (sp<stack+MAX && Y+(DY)>=win->y0 && Y+(DY)<=win->y1) \
;*     {sp->y = Y; sp->xl = XL; sp->xr = XR; sp->dy = DY; sp++;}
;*
;* #define POP(Y, XL, XR, DY)	/* pop segment off stack */ \
;*     {sp--; Y = sp->y+(DY = sp->dy); XL = sp->xl; XR = sp->xr;}
;*
;* fill(x, y, win, nv)
;* int x, y;	/* seed point */
;* Window *win;	/* screen window, resp. screen boundaries */
;* Pixel nv;	/* new pixel value */
;* {
;*     int l, x1, x2, dy;
;*     Pixel ov;	/* old pixel value */
;*     Segment stack[MAX], *sp = stack;	/* stack of filled segments */
;*
;*     ov = pixelread(x, y);		/* read pv at seed point */
;*     if (ov==nv || x<win->x0 || x>win->x1 || y<win->y0 || y>win->y1) return;
;*     PUSH(y, x, x, 1);			/* needed in some cases */
;*     PUSH(y+1, x, x, -1);		/* seed segment (popped 1st) */
;*
;*     while (sp>stack) {
;* 	/* pop segment off stack and fill a neighboring scan line */
;* 	POP(y, x1, x2, dy);
;* 	/*
;* 	 * segment of scan line y-dy for x1<=x<=x2 was previously filled,
;* 	 * now explore adjacent pixels in scan line y
;* 	 */
;* 	for (x=x1; x>=win->x0 && pixelread(x, y)==ov; x--)
;* 	    pixelwrite(x, y, nv);
;* 	if (x>=x1) goto skip;
;* 	l = x+1;
;* 	if (l<x1) PUSH(y, l, x1-1, -dy);		/* leak on left? */
;* 	x = x1+1;
;* 	do {
;* 	    for (; x<=win->x1 && pixelread(x, y)==ov; x++)
;* 		pixelwrite(x, y, nv);
;* 	    PUSH(y, l, x-1, dy);
;* 	    if (x>x2+1) PUSH(y, x2+1, x-1, -dy);	/* leak on right? */
;* skip:	    for (x++; x<=x2 && pixelread(x, y)!=ov; x++);
;* 	    l = x;
;* 	} while (x<=x2);
;*     }
;* }
;*
;* internal equates:
;* x = r16, -> x1 from caller
;* y = r18, -> y1 from caller
;* x1 = r6
;* x2 = r7
;* l = r5
;* dy = r17
;*
;**************************************************************************

.macro Get_PixH	; read a pixel
;call g_lcd_swap	; testcode to slow down pixel writes
;call Delay_10ms	; testcode
	; read pixel
	fmul	r18,r23		; int(ypos/8) -> r23 = 16, r18 = y1
	mul	r8,r1		; * max_c -> r8 = max_c
        movw    XL,r0
        add     XL,r16		; + xpos
	adc	XH,r9		; -> r9 = 0
        add     XL,r24		; + g_vscreen_start
        adc     XH,r25

        mov     r20,r18
        andi    r20,0x07	; ypos bit # in display RAM
        add     YL,r20          ; pointer to table offset
        ld      r1,Y		; r1= sram(bytab_offset+mod(ypos/8))
	sub	YL,r20		; reset pointer offset

        ld      r0,X            ; get display ram byte
	mov	r20,r0		; saves 1 cycle (no reload of display ram byte)
	and	r20,r1		; read pixel status, equal=cleared, not equal=set
.endm

.macro pushh_s	; push a segment
	; stack ('fillbuf') overflow check
	;ldi	r4,high(fillbuf_base + fillbuf_length) ; provided as reg var!
	cpi	ZL,low(fillbuf_base + fillbuf_length)
	cpc	ZH,r4		; check private stack -
	brlo	PC+2
	rjmp	mph_sex		; if full, exit macro w/o store

	; boundary checks
	mov	r19,@0		; copy Y to scrap
	add	r19,@3		; Y+DY
	brmi	mph_sex		; if Y+DY <0 (y_min) exit macro w/o store

mph_s1:
	cpi	r19,max_r	; if Y+DY <=max_r-1 (y_max), cont.
	brlo	mph_s2
	rjmp	mph_sex		; else exit macro w/o store

mph_s2:
	st	Z+,@0		; Y
	st	Z+,@1		; XL
	st	Z+,@2		; XR
	st	Z+,@3		; DY
mph_sex:
.endm

.macro poph_s	; pop a segment
	ld	@3,-Z		; DY
	ld	@2,-Z		; XR
	ld	@1,-Z		; XL
	ld	@0,-Z		; Y
	add	@0,@3		; Y=Y+DY
.endm


G_Seedp_H:
	cpi	r16,max_c-1		; boundary checks,
	brsh	g_flh1			; exit if seed pixel is off-screen
	cpi	r18,max_r-1
	brsh	g_flh1
	rjmp	PC+2			; continue
g_flh1:	ret

	push	XL
	push	XH
	push	YL	; pointer (low)g_table
	push	YH	; pointer (high)g_table
	push	ZL	; Z = private stack pointer
	push	ZH

	push	r0	; mul/fmul results
	push	r1
	push	r2	; r3:r2 fillbuf_base
	push	r3
	push	r4	; reg var
	push	r5	; internal l
	push	r6	; internal x1
	push	r7	; internal x2
	push	r8	; reg var
	push	r9	; reg var (0)
	push	r16	; internal x, (caller's x1 -> seed pixel xpos
	push	r17	; internal dy
	push	r18	; internal y, (caller's y1 -> seed pixel xpos
	push	r19	; scrap inside pushh_s macro
	push	r20	; general scrap
	push	r23	; reg var
	push	r24	; r25:24 pointer v_screen start
	push	r25


	; common constants - don't touch! (Y, r2-r4, r8, r9, r23, r24)
	ldi	YL,low(g_btable)
	ldi	YH,high(g_btable)
	clr	r9

	ldi	r20,max_c
	mov	r8,r20			; reg var

	ldi	r20,high(fillbuf_base + fillbuf_length)
	mov	r4,r20			; reg var for pushh_s macro

	; initialize vscreen pointer
	ldi     r24,low(g_vscreen)
	ldi     r25,high(g_vscreen)

	ldi	r23,16			; reg var for Get_PixH macro

	ldi	ZL,low(fillbuf_base)	; start private stack
	ldi	ZH,high(fillbuf_base)

	mov	r3,ZH			; copy for stack compare
	mov	r2,ZL

	; check seed pixel, must be cleared
	Get_PixH	; read pixel status, equal=cleared, not equal=set
	breq	g_flh2			; if seed pixel cleared, continue
	rjmp	g_flhexit		; else exit

g_flh2:	; seed pixel is clear, continue
	ldi	r17,1		; dy
	pushh_s 	r18, r16, r16, r17	; PUSH(y, x, x, 1): seed pixel, filled last

	inc	r18
	ldi	r17,-1
	pushh_s 	r18, r16, r16, r17	; PUSH(y+1, x, x, -1): seed segment (popped 1st)


g_flh3:	; ** main loop start ('while' stack not empty) **
	cp	ZL,r2		; stack pointer low/high:
	cpc	ZH,r3		; check if stack empty
	brne	g_flh4		; no, continue
	rjmp	g_flhexit	; stack empty, exit

g_flh4:
	poph_s	r18, r6, r7, r17	; POP(y, x1, x2, dy): pop a segment off stack

	; segment of scan line y-dy for x1<=x<=x2 was previously filled,
	; now explore adjacent pixels in current scan line y
	mov	r16,r6		; x=x1

g_flh5:	; start of 1st 'for' loop
	tst	r16
	brmi	g_flh6		; exit first 'for' loop if xpos < min_c (0)

	Get_PixH		; read pixel status, equal=cleared, not equal=set

	brne	g_flh6		; if pixel already set, exit 1st 'for' loop

g_flh5a:
	or	r0,r1		; else set pixel
	st	X,r0		; store it back to display ram
	dec	r16		; x--
	tst	r16		; x now off-screen?
	brmi	g_flh6		; exit first 'for' loop if xpos < min_c (0)
	sbiw	XL,1		; dec v_screen pointer
	ld      r0,X            ; get next display ram byte
	mov	r20,r0		; saves 1 cycle (no reload of display ram byte)
	and	r20,r1		; read pixel status, equal=cleared, not equal=set
	brne	g_flh6		; if pixel already set, exit 1st 'for' loop
	rjmp	g_flh5a		; else loop through line


g_flh6:
	cp	r16,r6		; x>=x1 ?
	brlt	g_flh6a
	rjmp	g_flh11		; goto 'skip' (too far for 'brxx')

g_flh6a:
	mov	r5,r16
	inc	r5		; l = x+1

	cp	r5,r6		; ** leak on left: l<x1 ?
	brsh 	g_flh7		; no, skip push job

	dec	r6		; dec x1, push segment
	neg	r17		; -dy
	pushh_s	r18, r5, r6, r17	; PUSH(y, l, x1-1, -dy)
	neg	r17		; reset dy
	inc	r6		; reset x1

g_flh7:
	mov	r16,r6
	inc	r16		; x = x1+1

g_flh8:	; start of 'do' loop
	; start of 2nd 'for' loop
	cpi	r16,max_c	; x off-screen?
	brsh	g_flh9		; yes, exit 2nd 'for' loop

	Get_PixH		; read pixel status, equal=cleared, not equal=set
	brne	g_flh9		; if pixel already set, exit 2nd 'for' loop

g_flh8a:
	or	r0,r1		; else set pixel
	st	X,r0		; store it back to display ram
	inc	r16		; x++
	cpi	r16,max_c	; x now off-screen?
	brsh	g_flh9		; yes, exit 2nd 'for' loop
	adiw	XL,1		; inc v_screen pointer
	ld      r0,X            ; get next display ram byte
	mov	r20,r0		; saves 1 cycle (no reload of display ram byte)
	and	r20,r1		; read pixel status, equal=cleared, not equal=set
	brne	g_flh9		; if pixel already set, exit 1st 'for' loop
	rjmp	g_flh8a		; else loop through line

g_flh9:
	dec	r16		; x-1, push segment data
	pushh_s	r18, r5, r16, r17	; PUSH(y, l, x-1, dy)

	cp	r7,r16		; ** leak on right: x-1>x2 ? (-> same as x>x2+1)
	brsh	g_flh10		; no, skip next push job
	inc	r7		; x2+1
	neg	r17		; -dy
	pushh_s	r18, r7, r16, r17	; PUSH(y, x2+1, x-1, -dy)
	neg	r17		; reset dy
	dec	r7		; reset x2

g_flh10:
	inc	r16		; reset x-1 to x

g_flh11:	; 'skip' label, also start of 3rd 'for' loop
	inc	r16		; x++

g_flh12:
	cp	r7,r16		; x<=x2 ?
	brlt	g_flh13		; no, exit 3rd 'for' loop

	Get_PixH		; read pixel status, equal=cleared, not equal=set
	breq	g_flh13		; if pixel cleared, exit 3rd 'for' loop

g_flh12a:
	inc	r16		; x++
	cp	r7,r16		; x<=x2 ?
	brlt	g_flh13		; no, exit 3rd 'for' loop
	adiw	XL,1		; inc v_screen pointer
	ld      r0,X            ; get next display ram byte
	and	r0,r1		; read pixel status, equal=cleared, not equal=set
	breq	g_flh13		; if pixel already set, exit 1st 'for' loop
	rjmp	g_flh12a	; loop through 3rd 'for' loop

g_flh13:
	mov	r5,r16		; l=x
	cp	r7,r16		; x<=x2 ?
	brsh	PC+2		; yes, loop (rjmps needed, too far for 'brxx')
	rjmp	g_flh3		; no, pop next segment (** main loop end **)
	rjmp	g_flh8		; x<=x2, continue 'do' loop

g_flhexit:
	pop	r25
	pop	r24
	pop	r23
	pop	r20
	pop	r19
	pop	r18
	pop	r17
	pop	r16

	pop	r9
	pop	r8
	pop	r7
	pop	r6
	pop	r5
	pop	r4
	pop	r3
	pop	r2
	pop	r1
	pop	r0

	pop	ZH
	pop	ZL
	pop	YH
	pop	YL
	pop	XH
	pop	XL

	ret

;**** End function G_Seedp_H ------------------------------------------****


; Maximum HW optimized variant!
;**************************************************************************
;*
;* Function G_Seedp_V - *very* fast polygon fill (max. speed optimized)
;* (P. Heckbert, assembler version hardware-optimized for SED15xx family)
;*
;* Author: Andreas Lenze, 6/2003
;*
;* Call with:
;*	 x1,y1	-> seed pixel coordinates (must be inside area)
;*
;* Returns:
;*	 -
;*
;* Note: Mr Heckbert's 'seed fill' is  per default at least twice
;*	 as fast as any recursive 4-way-fill. Stack size demands also
;*	 are extremely low when compared to the usual textbook flood fill
;*	 algorithms ("4-way fill", "boundary scan fill" etc.) which
;*	 tend to gobble up most of an MCU's internal RAM for their
;*	 stack requirements.
;*
;*	 This 'vertical scan line' variant is maximally optimized for the
;*	 SED13/15xx vertical video memory organization: *vertical* scan lines
;*	 can be filled partially in 'multi bit mode' as opposed to 'single pixel
;*	 mode', resulting in a factor 2.27 (227%) speed-up when compared to
;*	 the 'straight assembler' variant. Of course, even the 'straight'
;*	 assembler version will typically be considerably faster than
;*	 compiler generated code.
;*
;*	 If you feel really brave, some or all boundary checks could be omitted,
;*	 too, providing some more speed-up (another 26% ...).
;*	 With all checks active, the routine is 'crash-proof' in every respect.
;*
;* Note 2:
;* Define the additional array for this module:
;*
;* .equ	fillbuf_length=0x0100	; size of circular buffer (64-xxxx bytes,
;*				  *must* be divisible by 4!)
;*
;* fillbuf_base:
;*	.byte fillbuf_length	; reserves fillbuf_length bytes for the
;*				; buffer and is used as the buffer뭩 base address
;*
;* All regs saved
;*
;* ------------------------------------------------------------------------
;*
;* Paul Heckbert's 'seed fill' algorithm ('fill.c' from "Graphics Gems", 1990;
;* original code adapted for vertical scan lines):
;*
;* typedef struct {		/* window: a discrete 2-D rectangle */
;*     int x0, y0;		/* xmin and ymin */
;*     int x1, y1;		/* xmax and ymax (inclusive) */
;* } Window;
;*
;* typedef int Pixel;		/* 1-channel frame buffer assumed */
;*
;* Pixel pixelread();
;*
;* typedef struct {short x, yt, yb, dx;} Segment; /* YT=Y_Top, YB=Y_Bottom */
;* /*
;*  * Filled vertical segment of scanline x for yt<=y<=yb.
;*  * Parent segment was on line x-dx.  dx=1 or -1
;*  */
;* #define MAX 10000		/* max depth of stack -> extremely conservative figure, */
;*				/* on a small embedded system expect ~ 1/50 of that */
;*
;* #define PUSH(X, YT, YB, DX)	/* push new segment on stack */ \
;*     if (sp<stack+MAX && X+(DX)>=win->x0 && X+(DX)<=win->x1) \
;*     {sp->x = X; sp->yt = YT; sp->yb = YB; sp->dx = DX; sp++;}
;*
;* #define POP(X, YT, YB, DX)	/* pop segment off stack */ \
;*     {sp--; X = sp->x+(DX = sp->dx); YT = sp->yt; YB = sp->yb;}
;*
;* fill(x, y, win, nv)
;* int x, y;	/* seed point */
;* Window *win;	/* screen window, resp. screen boundaries */
;* Pixel nv;	/* new pixel value (old and new values here are cleared/set only) */
;* {
;*     int l, y1, y2, dx;
;*     Pixel ov;	/* old pixel value */
;*     Segment stack[MAX], *sp = stack;	/* stack of filled segments */
;*
;*     ov = pixelread(x, y);		/* read pv at seed point */
;*     if (ov==nv || y<win->y0 || y>win->y1 || x<win->x0 || x>win->x1) return;
;*     PUSH(x, y, y, 1);		/* needed in some cases */
;*     PUSH(x+1, y, y, -1);		/* seed segment (popped 1st) */
;*
;*     while (sp>stack) {
;* 	/* pop segment off stack and fill a neighboring scan line */
;* 	POP(x, y1, y2, dx);
;* 	/*
;* 	 * segment of scan line x-dx for y1<=y<=y2 was previously filled,
;* 	 * now explore adjacent pixels in scan line x
;* 	 */
;* 	for (y=y1; y>=win->y0 && pixelread(x, y)==ov; y--)
;* 	    pixelwrite(x, y, nv);
;* 	if (y>=y1) goto skip;
;* 	l = y+1;
;* 	if (l<y1) PUSH(x, l, y1-1, -dx);		/* leak on top? */
;* 	y = y1+1;
;* 	do {
;* 	    for (; y<=win->y1 && pixelread(x, y)==ov; y++)
;* 		pixelwrite(x, y, nv);
;* 	    PUSH(x, l, y-1, dx);
;* 	    if (y>y2+1) PUSH(x, y2+1, x-1, -dx);	/* leak on bottom? */
;* skip:	    for (y++; y<=y2 && pixelread(x, y)!=ov; y++);
;* 	    l = y;
;* 	} while (y<=y2);
;*     }
;* }
;*
;* internal equates:
;* x = r16, -> x1 from caller
;* y = r18, -> y1 from caller
;* y1 = r8
;* y2 = r9
;* l = r5
;* dx = r17
;*
;**************************************************************************

.macro Get_PixV	; read a pixel
;call g_lcd_swap	; testcode to slow down pixel writes
;call Delay_10ms	; testcode
	; read pixel
	mul	r18,r11		; int(ypos/8) -> r11 = 32 = 4xSHL, r18 = y1
	mul	r10,r1		; * max_c -> r10 = max_c
        movw    XL,r0
        add     XL,r16		; + xpos
	adc	XH,r6		; -> r6 = 0
        add     XL,r24		; + g_vscreen_start
        adc     XH,r25
        ld      r20,X           ; get display ram byte

        mov     r21,r18
        andi    r21,0x07	; ypos bit # in display RAM
.endm	; returns: r20 = display RAM byte, r21 = ypos # in byte

.macro pushv_s	; push a segment
	; stack ('fillbuf') overflow check
	;ldi	r4,high(fillbuf_base + fillbuf_length) ; provided as reg var!
	cpi	YL,low(fillbuf_base + fillbuf_length)
	cpc	YH,r4		; check private stack:
	brlo	PC+2
	rjmp	mpv_sex		; if full, exit macro w/o store

	; boundary checks for x-direction
	mov	r19,@0		; copy X to scrap
	add	r19,@3		; X+DX
	brmi	mpv_sex		; if X+DX <0 (x_min) exit macro w/o store

mpv_s1:
	cpi	r19,max_c	; if X+DX <=max_c-1 (x_max), cont.
	brlo	mpv_s2
	rjmp	mpv_sex		; else exit macro w/o store

mpv_s2:
	st	Y+,@0		; X
	st	Y+,@1		; YT
	st	Y+,@2		; YB
	st	Y+,@3		; DX
mpv_sex:
.endm

.macro popv_s	; pop a segment
	ld	@3,-Y		; DX
	ld	@2,-Y		; YB
	ld	@1,-Y		; YT
	ld	@0,-Y		; X
	add	@0,@3		; X=X+DX
.endm


G_Seedp_V:
	cpi	r16,max_c-1	; boundary checks,
	brsh	g_flv1		; exit if seed pixel is off-screen
	cpi	r18,max_r-1
	brsh	g_flv1
	rjmp	PC+2		; continue
g_flv1:	ret

	push	XL	; X = v_screen and bit lookup table access pointer
	push	XH
	push	YL	; Y = private stack pointer
	push	YH
	push	ZL	; Z = jump table pointer
	push	ZH

	push	r0	; mul/fmul results
	push	r1
	push	r2	; r3:r2 fillbuf_base
	push	r3
	push	r4	; reg var
	push	r5	; internal l
	push	r6	; reg var (0)
	push	r7	; reg var (3)
	push	r8	; internal y1
	push	r9	; internal y2
	push	r10	; reg var (max_c)
	push	r11	; reg var (32)

	push	r16	; internal x, (caller's x1 -> seed pixel xpos
	push	r17	; internal dx
	push	r18	; internal y, (caller's y1 -> seed pixel xpos
	push	r19	; scrap inside pushv_s macro
	push	r20	; general scrap
	push	r21	; ypos # in byte (Get_Pix macro)
	push	r24	; r25:24 pointer v_screen start
	push	r25


	; common constants - don't touch! (r2-r4, r6, r7, r10, r11, r24, r25, Y)
	ldi	r20,max_c
	mov	r10,r20			; reg var for bounds check

	ldi	r20,3
	mov	r7,r20			; reg var for ijmps

	ldi	r20,high(fillbuf_base + fillbuf_length)
	mov	r4,r20			; reg var for pushv_s macro

	ldi	r20,32
	mov	r11,r20			; reg var for Get_Pix macro

	clr	r6			; we need another reg with "0"

	ldi     r24,low(g_vscreen)	; initialize vscreen pointer
	ldi     r25,high(g_vscreen)


	ldi	YL,low(fillbuf_base)	; base address private stack
	ldi	YH,high(fillbuf_base)
	movw	r2,YL			; copy for stack position compare

	; check seed pixel, must be cleared
	Get_PixV	; r20 = display RAM byte, r21 = ypos # in byte
	ldi	XL,low(g_btable)
	ldi	XH,high(g_btable)	; will be 0!
        add     XL,r21          ; pointer to table offset
        ld      r1,X
	and	r20,r1		; read pixel status, equal=cleared, not equal=set
	breq	g_flv2		; if seed pixel cleared, continue
	rjmp	g_flvexit	; else exit routine

g_flv2:	; seed pixel is clear, continue
	ldi	r17,1		; dx
	pushv_s r16, r18, r18, r17	; PUSH(x, y, y, 1): seed pixel, filled last

	inc	r16
	ldi	r17,-1
	pushv_s r16, r18, r18, r17	; PUSH(x+1, y, y, -1): seed segment (popped 1st)

g_flv3:	; ** main loop start ('while' stack not empty) **
	cp	YL,r2		; stack pointer low/high:
	cpc	YH,r3		; check if stack empty
	brne	g_flv4		; no, continue
	rjmp	g_flvexit	; stack empty, exit

g_flv4:
	popv_s	r16, r8, r9, r17	; POP(x, y1, y2, dx): pop a segment off stack

	; segment of scan line x-dx for y1<=y<=y2 was previously filled,
	; now explore adjacent pixels in current (vertical) scan line x
	mov	r18,r8		; y=y1

	; start of 1st 'for' loop
	; walk through display ram byte, set cleared pixels
	Get_PixV	; read pixel (should be after test, but brxx>63!)
	tst	r18
	brmi	g_flv6		; exit first 'for' loop if ypos < min_r (0)

	; don't build a 'real' jumptable (would be much too slow),
	; calculate the dest. address directly into Z
	ldi	ZH,high(ft1_0)	; load 1st label group base address
	ldi	ZL,low(ft1_0)
	lsl	r21		; distance label/label = 4
	lsl	r21		; r21 = bit pos. #
	sub	ZL,r21		; set up pointer
	sbc	ZH,r6

	; start processing at correct bit position
	ijmp			; indirect jump

ft1_7:	sbrc	r20,7		; border reached?
	rjmp	g_flv5		; yes, exit loop
	ori	r20,128		; else: set pixel
	dec	r18		; and dec y

ft1_6:	sbrc	r20,6
	rjmp	g_flv5
	ori	r20,64
	dec	r18

ft1_5:	sbrc	r20,5
	rjmp	g_flv5
	ori	r20,32
	dec	r18

ft1_4:	sbrc	r20,4
	rjmp	g_flv5
	ori	r20,16
	dec	r18

ft1_3:	sbrc	r20,3
	rjmp	g_flv5
	ori	r20,8
	dec	r18

ft1_2:	sbrc	r20,2
	rjmp	g_flv5
	ori	r20,4
	dec	r18

ft1_1:	sbrc	r20,1
	rjmp	g_flv5
	ori	r20,2
	dec	r18

ft1_0:	sbrc	r20,0
	rjmp	g_flv5
	ori	r20,1
	dec	r18

	; continue through line: get next byte from prior v_screen page
ft1_00:	brmi	g_flv5		; exit if y now off-screen
	st	X,r20		; store previous byte back to display ram
	subi	XL,max_c	; subtract max_c from v_screen pointer
	sbci	XH,0
	ld	r20,X		; get new display ram byte

	tst	r20		; no borders in this byte? (all bits clear)
	brne	ft1_7		; no, process byte normally
	ori	r20,0xFF	; else set all bits
	subi	r18,8		; dec y accordingly and
	rjmp	ft1_00		; go for the next byte

g_flv5:
	st	X,r20		; store byte back to display ram

g_flv6:
	cp	r18,r8		; y>=y1 ?
	brlt	PC+2
	rjmp	g_flv11		; goto 'skip' (too far for 'brxx')

	mov	r5,r18
	inc	r5		; l = y+1

	cp	r5,r8		; ** leak on top: l<y1 ?
	brsh 	g_flv7		; no, skip push job

	dec	r8		; dec y1, push segment
	neg	r17		; -dx
	pushv_s	r16, r5, r8, r17	; PUSH(x, l, y1-1, -dx)
	neg	r17		; reset dx
	inc	r8		; reset y1

g_flv7:
	mov	r18,r8
	inc	r18		; y = y1+1

g_flv8:	; start of 'do' loop / start of 2nd 'for' loop
	; walk through display ram byte, set cleared pixels
	Get_PixV		; read pixel (should be after test, but brxx>63!)
	cpi	r18,max_r	; y now off-screen?
	brsh	g_flv9a		; yes, exit 2nd 'for' loop

	; don't build a 'real' jumptable (would be much too slow),
	; calculate the dest. address directly into Z
	ldi	ZH,high(ft2_0)	; load 2nd label group base address
	ldi	ZL,low(ft2_0)
	lsl	r21		; distance label/label = 4
	lsl	r21		; r21 = bit pos. #
	add	ZL,r21		; set pointer
	adc	ZH,r6

	; start processing at correct bit position
	ijmp			; indirect jump

ft2_0:	sbrc	r20,0		; border reached?
	rjmp	g_flv9		; yes
	ori	r20,1		; else set pixel
	inc	r18		; and inc y

ft2_1:	sbrc	r20,1
	rjmp	g_flv9
	ori	r20,2
	inc	r18

ft2_2:	sbrc	r20,2
	rjmp	g_flv9
	ori	r20,4
	inc	r18

ft2_3:	sbrc	r20,3
	rjmp	g_flv9
	ori	r20,8
	inc	r18

ft2_4:	sbrc	r20,4
	rjmp	g_flv9
	ori	r20,16
	inc	r18

ft2_5:	sbrc	r20,5
	rjmp	g_flv9
	ori	r20,32
	inc	r18

ft2_6:	sbrc	r20,6
	rjmp	g_flv9
	ori	r20,64
	inc	r18

ft2_7:	sbrc	r20,7
	rjmp	g_flv9
	ori	r20,128
	inc	r18

ft2_00:	cpi	r18,max_r	; y off-screen now?
	brsh	g_flv9		; yes, exit 2nd 'for' loop

	; continue through line: get next byte from next v_screen page
	st	X,r20		; store previous byte back to display ram
	subi	XL,-max_c	; add max_c to v_screen pointer
	sbci	XH,0xFF
	ld	r20,X		; get next display ram byte

	tst	r20		; no borders in this byte? (all bits clear)
	brne	ft2_0		; no, process byte normally
	ori	r20,0xFF	; else set all bits
	subi	r18,-8		; inc y accordingly and
	rjmp	ft2_00		; go for the next byte

g_flv9:
	st	X,r20		; store byte back to display ram

g_flv9a:
	dec	r18		; y-1, push segment data
	pushv_s	r16, r5, r18, r17	; PUSH(x, l, y-1, dx)

	cp	r9,r18		; ** leak on bottom: y-1>y2 ? (-> same as y>y2+1)
	brsh	g_flv10		; no, skip next push job

	inc	r9		; y2+1
	neg	r17		; -dx
	pushv_s	r16, r9, r18, r17	; PUSH(x, y2+1, y-1, -dx)
	neg	r17		; reset dx
	dec	r9		; reset y2

g_flv10:
	inc	r18		; reset y-1 to y

g_flv11:	; 'skip' label, also start of 3rd 'for' loop
	inc	r18		; y++
	cp	r9,r18		; y<=y2 ?
	brlt	g_flv13		; no, exit 3rd 'for' loop

	Get_PixV		; read pixel

g_flv12:	; walk through display ram byte, exit at first cleared pixel
	; don't build a 'real' jumptable (would be much too slow),
	; calculate the dest. address directly into Z
	ldi	ZH,high(ft3_0)	; load 3rd label group base address
	ldi	ZL,low(ft3_0)
	mul	r21,r7		; distance label/label = 3,
				; r21 = bit pos. #, r7 = 3 (reg var)
	add	ZL,r0		; set pointer
	adc	ZH,r1

	; start processing at correct bit position
	ijmp			; indirect jump

ft3_0:	sbrs	r20,0		; pixel cleared?
	rjmp	g_flv13		; yes, exit loop
	inc	r18		; else inc y

ft3_1:	sbrs	r20,1
	rjmp	g_flv13
	inc	r18

ft3_2:	sbrs	r20,2
	rjmp	g_flv13
	inc	r18

ft3_3:	sbrs	r20,3
	rjmp	g_flv13
	inc	r18

ft3_4:	sbrs	r20,4
	rjmp	g_flv13
	inc	r18

ft3_5:	sbrs	r20,5
	rjmp	g_flv13
	inc	r18

ft3_6:	sbrs	r20,6
	rjmp	g_flv13
	inc	r18

ft3_7:	sbrs	r20,7
	rjmp	g_flv13
	inc	r18

ft3_00:	cp	r9,r18		; y<=y2 ?
	brlt	g_flv13		; no, exit 3rd 'for' loop

	; continue through line: get next byte from next v_screen page
	subi	XL,-max_c	; add max_c to v_screen pointer
	sbci	XH,0xFF
	ld	r20,X		; get next display ram byte

	cpi	r20,0xFF	; a leak in this byte? (cleared pixel)
	brne	ft3_0		; yes, process byte normally
	subi	r18,-8		; no, inc y accordingly and
	rjmp	ft3_00		; go fo the next byte

g_flv13:
	mov	r5,r18		; l=y
	cp	r9,r18		; y<=y2 ?
	brsh	PC+2		; yes, loop (rjmps needed, too far for 'brxx')
	rjmp	g_flv3		; no, pop next segment (** main loop end **)
	rjmp	g_flv8		; y<=y2, continue 'do' loop

g_flvexit:
	pop	r25
	pop	r24
	pop	r21
	pop	r20
	pop	r19
	pop	r18
	pop	r17
	pop	r16

	pop	r11
	pop	r10
	pop	r9
	pop	r8
	pop	r7
	pop	r6
	pop	r5
	pop	r4
	pop	r3
	pop	r2
	pop	r1
	pop	r0

	pop	ZH
	pop	ZL
	pop	YH
	pop	YL
	pop	XH
	pop	XL

	ret

;**** End function G_Seedp_V ------------------------------------------****


; Maximum HW optimized variant with (updated!) "checkered fill" option!
;**************************************************************************
;*
;* Function G_Seedp_Vc - *very* fast polygon fill with "checkered fill"
;* option (max. speed optimized)
;*
;* (P. Heckbert, assembler version hardware-optimized for SED15xx family)
;*
;* Author: Andreas Lenze, 6/2003
;* Update 7/2005: added "checkered fill" functionality
;*
;* Call with:
;*	 x1,y1	-> seed pixel coordinates (must be inside area)
;*	 pmode = 0 for solid fill, pmode = -1 for checkered fill
;*
;* Returns:
;*	 -
;*
;* Note: Mr Heckbert's 'seed fill' is  per default at least twice
;*	 as fast as any recursive 4-way-fill. Stack size demands also
;*	 are extremely low when compared to the usual textbook flood fill
;*	 algorithms ("4-way fill", "boundary scan fill" etc.) which
;*	 tend to gobble up most of an MCU's internal RAM for their
;*	 stack requirements.
;*
;*	 This 'vertical scan line' variant is maximally optimized for the
;*	 SED13/15xx vertical video memory organization: *vertical* scan lines
;*	 can be filled partially in 'multi bit mode' as opposed to 'single pixel
;*	 mode', resulting in a factor 2.27 (227%) speed-up when compared to
;*	 the 'straight assembler' variant. Of course, even the 'straight'
;*	 assembler version will typically be considerably faster than
;*	 compiler generated code.
;*
;*	 If you feel really brave, some or all boundary checks could be omitted,
;*	 too, providing some more speed-up (another 26% ...).
;*	 With all checks active, the routine is 'crash-proof' in every respect.
;*
;* Note 2 (update!):
;* This variant provides a "checkered fill" mode if pmode is set to -1
;*
;* Note 3:
;* Define the additional array for this module:
;*
;* .equ	fillstack_length=0x0100	; size of buffer (64-xxxx bytes,
;*				  *must* be divisible by 4!)
;*
;* fillstack_base:
;*	.byte fillstack_length	; reserves fillstack_length bytes for the
;*				; buffer and is used as the buffer뭩 base address
;*
;* All regs saved
;*
;* ------------------------------------------------------------------------
;*
;* Paul Heckbert's 'seed fill' algorithm ('fill.c' from "Graphics Gems", 1990;
;* original code adapted for vertical scan lines):
;*
;* typedef struct {		/* window: a discrete 2-D rectangle */
;*     int x0, y0;		/* xmin and ymin */
;*     int x1, y1;		/* xmax and ymax (inclusive) */
;* } Window;
;*
;* typedef int Pixel;		/* 1-channel frame buffer assumed */
;*
;* Pixel pixelread();
;*
;* typedef struct {short x, yt, yb, dx;} Segment; /* YT=Y_Top, YB=Y_Bottom */
;* /*
;*  * Filled vertical segment of scanline x for yt<=y<=yb.
;*  * Parent segment was on line x-dx.  dx=1 or -1
;*  */
;* #define MAX 10000		/* max depth of stack -> extremely conservative figure, */
;*				/* on a small embedded system expect ~ 1/50 of that */
;*
;* #define PUSH(X, YT, YB, DX)	/* push new segment on stack */ \
;*     if (sp<stack+MAX && X+(DX)>=win->x0 && X+(DX)<=win->x1) \
;*     {sp->x = X; sp->yt = YT; sp->yb = YB; sp->dx = DX; sp++;}
;*
;* #define POP(X, YT, YB, DX)	/* pop segment off stack */ \
;*     {sp--; X = sp->x+(DX = sp->dx); YT = sp->yt; YB = sp->yb;}
;*
;* fill(x, y, win, nv)
;* int x, y;	/* seed point */
;* Window *win;	/* screen window, resp. screen boundaries */
;* Pixel nv;	/* new pixel value (old and new values here are cleared/set only) */
;* {
;*     int l, y1, y2, dx;
;*     Pixel ov;	/* old pixel value */
;*     Segment stack[MAX], *sp = stack;	/* stack of filled segments */
;*
;*     ov = pixelread(x, y);		/* read pv at seed point */
;*     if (ov==nv || y<win->y0 || y>win->y1 || x<win->x0 || x>win->x1) return;
;*     PUSH(x, y, y, 1);		/* needed in some cases */
;*     PUSH(x+1, y, y, -1);		/* seed segment (popped 1st) */
;*
;*     while (sp>stack) {
;* 	/* pop segment off stack and fill a neighboring scan line */
;* 	POP(x, y1, y2, dx);
;* 	/*
;* 	 * segment of scan line x-dx for y1<=y<=y2 was previously filled,
;* 	 * now explore adjacent pixels in scan line x
;* 	 */
;* 	for (y=y1; y>=win->y0 && pixelread(x, y)==ov; y--)
;* 	    pixelwrite(x, y, nv);
;* 	if (y>=y1) goto skip;
;* 	l = y+1;
;* 	if (l<y1) PUSH(x, l, y1-1, -dx);		/* leak on top? */
;* 	y = y1+1;
;* 	do {
;* 	    for (; y<=win->y1 && pixelread(x, y)==ov; y++)
;* 		pixelwrite(x, y, nv);
;* 	    PUSH(x, l, y-1, dx);
;* 	    if (y>y2+1) PUSH(x, y2+1, x-1, -dx);	/* leak on bottom? */
;* skip:	    for (y++; y<=y2 && pixelread(x, y)!=ov; y++);
;* 	    l = y;
;* 	} while (y<=y2);
;*     }
;* }
;*
;* internal equates:
;* x = r16, -> x1 from caller
;* y = r18, -> y1 from caller
;* y1 = r8
;* y2 = r9
;* l = r5
;* dx = r17
;*
;**************************************************************************

.macro Get_PixVc	; read a pixel
;call Delay_ms 10	; testcode
;call g_lcd_swap	; testcode to slow down pixel writes
	; read pixel
	mul	r18,r11		; int(ypos/8) -> r11 = 32 = 4xSHL, r18 = y1
	mul	r10,r1		; * max_c -> r10 = max_c
        movw    XL,r0
        add     XL,r16		; + xpos
	adc	XH,r6		; -> r6 = 0
        add     XL,r24		; + g_vscreen_start
        adc     XH,r25
        ld      r20,X           ; get display ram byte
        mov     r21,r18
        andi    r21,0x07	; ypos bit # in display RAM
.endm	; returns: r20 = display RAM byte, r21 = ypos # in byte

.macro push_sc	; push a segment
	; stack ('fillstack') overflow check
	;ldi	r4,high(fillstack_base + fillstack_length) ; provided as reg var!
	cpi	YL,low(fillstack_base + fillstack_length)
	cpc	YH,r4		; check private stack:
	brlo	PC+2
	rjmp	mpc_ex		; if full, exit macro w/o store

	; boundary checks for x-direction
	mov	r19,@0		; copy X to scrap
	add	r19,@3		; X+DX
	brmi	mpc_ex		; if X+DX <0 (x_min) exit macro w/o store
	cpi	r19,max_c	; if X+DX <=max_c-1 (x_max), cont.
	brlo	mpc_s0
	rjmp	mpc_ex		; else exit macro w/o store

mpc_s0:
	st	Y+,@0		; X
	st	Y+,@1		; YT
	st	Y+,@2		; YB
	st	Y+,@3		; DX
mpc_ex:
.endm

.macro pop_sc	; pop a segment
	ld	@3,-Y		; DX
	ld	@2,-Y		; YB
	ld	@1,-Y		; YT
	ld	@0,-Y		; X
	add	@0,@3		; X=X+DX
.endm


G_Seedp_Vc:
	cpi	r16,max_c-1	; boundary checks,
	brsh	gc_flv1		; exit if seed pixel is off-screen
	cpi	r18,max_r-1
	brsh	gc_flv1
	rjmp	PC+2		; continue
gc_flv1:
	ret

	push	XL	; X = v_screen and bit lookup table access pointer
	push	XH
	push	YL	; Y = private stack pointer
	push	YH
	push	ZL	; Z = jump table pointer
	push	ZH

	push	r0	; mul/fmul results
	push	r1
	push	r2	; r3:r2 fillstack_base
	push	r3
	push	r4	; reg var
	push	r5	; internal l
	push	r6	; reg var (0)
	push	r7	; reg var (3)
	push	r8	; internal y1
	push	r9	; internal y2
	push	r10	; reg var (max_c)
	push	r11	; reg var (32)
	push	r12	; reg var (5)

	push	r15	; pmode
	push	r16	; internal x, (caller's x1 -> seed pixel xpos
	push	r17	; internal dx
	push	r18	; internal y, (caller's y1 -> seed pixel xpos
	push	r19	; scrap inside push_sc macro
	push	r20	; general scrap
	push	r21	; ypos # in byte (Get_Pix macro)
	push	r24	; r25:24 pointer v_screen start
	push	r25


	; common constants - don't touch! (r2-r4, r6, r7, r10, r11, r12, r24, r25, Y)
	ldi	r20,max_c
	mov	r10,r20			; reg var for bounds check

	ldi	r20,3
	mov	r7,r20			; reg var for ijmps

	ldi	r20,5
	mov	r12,r20			; reg var for ijmps

	ldi	r20,high(fillstack_base + fillstack_length)
	mov	r4,r20			; reg var for push_sc macro

	ldi	r20,32
	mov	r11,r20			; reg var for Get_PixVc macro

	clr	r6			; we need another reg with "0"

	ldi     r24,low(g_vscreen)	; initialize vscreen pointer
	ldi     r25,high(g_vscreen)


	ldi	YL,low(fillstack_base)	; base address private stack
	ldi	YH,high(fillstack_base)
	movw	r2,YL			; copy for stack position compare

	; check seed pixel, must be cleared
	Get_PixVc		; r20 = display RAM byte, r21 = ypos # in byte
	ldi	XL,low(g_btable)
	ldi	XH,high(g_btable)
        add     XL,r21          ; pointer to table offset
	adc	XH,r6
        ld      r1,X
	and	r20,r1		; read pixel status, equal=cleared, not equal=set
	breq	gc_flv2		; if seed pixel cleared, continue
	rjmp	gc_flvexit	; else exit routine

gc_flv2:	; seed pixel is clear, continue
	ldi	r17,1		; dx
	push_sc r16, r18, r18, r17	; PUSH(x, y, y, 1): seed pixel, filled last

	inc	r16
	ldi	r17,-1
	push_sc r16, r18, r18, r17	; PUSH(x+1, y, y, -1): seed segment (popped 1st)

gc_flv3:	; ** main loop start ('while' stack not empty) **
	cp	YL,r2		; stack pointer low/high:
	cpc	YH,r3		; check if stack empty
	brne	gc_flv4		; no, continue
	rjmp	gc_flvexit	; stack empty, exit

gc_flv4:
	pop_sc	r16, r8, r9, r17	; POP(x, y1, y2, dx): pop a segment off stack

	; segment of scan line x-dx for y1<=y<=y2 was previously filled,
	; now explore adjacent pixels in current (vertical) scan line x
	mov	r18,r8		; y=y1

	; start of 1st 'for' loop
	; walk through display ram byte, set cleared pixels
	tst	r18
	brmi	ftc1a_int 	; exit first 'for' loop if ypos < min_r (0) (intermediate for gc_flv6)
	Get_PixVc		; read pixel

	; don't build a 'real' jumptable (would be much too slow),
	; calculate the dest. address directly into Z
	sbrs	pmode,7		; -1 -> checkered fill,
	rjmp	gc_sfill0	; 	else solid fill

	ldi	ZH,high(ftc1a_0); load 1st label group base address (checkered fill)
	ldi	ZL,low(ftc1a_0)
	mul	r21,r12		; distance label/label = 5,
				; r21 = bit pos. #, r12 = 5 (reg var)
	sub	ZL,r0		; set pointer
	sbc	ZH,r1

	; start processing at correct bit position
	ijmp			; indirect jump

gc_sfill0:
	ldi	ZH,high(ftc1_0)	; load 1st label group base address (solid fill)
	ldi	ZL,low(ftc1_0)
	lsl	r21		; distance label/label = 4
	lsl	r21		; r21 = bit pos. #
	sub	ZL,r21		; set up pointer
	sbc	ZH,r6

	; start processing at correct bit position
	ijmp			; indirect jump

ftc1a_int:
	rjmp	gc_flv6		; intermediate label

	; checkered fill routine
ftc1a_7:
	sbrc	r20,7		; border reached?
	rjmp	gc_flv5		; yes, exit loop
	sbrs	XL,0
	ori	r20,128		;	else set pixel depending on XL being odd or even
	dec	r18		;	and dec y

ftc1a_6:
	sbrc	r20,6
	rjmp	gc_flv5
	sbrc	XL,0
	ori	r20,64
	dec	r18

ftc1a_5:
	sbrc	r20,5
	rjmp	gc_flv5
	sbrs	XL,0
	ori	r20,32
	dec	r18

ftc1a_4:
	sbrc	r20,4
	rjmp	gc_flv5
	sbrc	XL,0
	ori	r20,16
	dec	r18

ftc1a_3:
	sbrc	r20,3
	rjmp	gc_flv5
	sbrs	XL,0
	ori	r20,8
	dec	r18

ftc1a_2:
	sbrc	r20,2
	rjmp	gc_flv5
	sbrc	XL,0
	ori	r20,4
	dec	r18

ftc1a_1:
	sbrc	r20,1
	rjmp	gc_flv5
	sbrs	XL,0
	ori	r20,2
	dec	r18

ftc1a_0:
	sbrc	r20,0
	rjmp	gc_flv5
	sbrc	XL,0
	ori	r20,1
	dec	r18
	rjmp	ftc1_00

ftc1a_int2:
	rjmp	ftc1a_7		; intermediate label

	; solid fill routine
ftc1_7:	sbrc	r20,7		; border reached?
	rjmp	gc_flv5		; yes, exit loop
	ori	r20,128		;	else: set pixel
	dec	r18		;	and dec y

ftc1_6:	sbrc	r20,6
	rjmp	gc_flv5
	ori	r20,64
	dec	r18

ftc1_5:	sbrc	r20,5
	rjmp	gc_flv5
	ori	r20,32
	dec	r18

ftc1_4:	sbrc	r20,4
	rjmp	gc_flv5
	ori	r20,16
	dec	r18

ftc1_3:	sbrc	r20,3
	rjmp	gc_flv5
	ori	r20,8
	dec	r18

ftc1_2:	sbrc	r20,2
	rjmp	gc_flv5
	ori	r20,4
	dec	r18

ftc1_1:	sbrc	r20,1
	rjmp	gc_flv5
	ori	r20,2
	dec	r18

ftc1_0:	sbrc	r20,0
	rjmp	gc_flv5
	ori	r20,1
	dec	r18

	; continue through line: get next byte from prior v_screen page
ftc1_00:
	brmi	gc_flv5		; exit if y now off-screen
	st	X,r20		; store previous byte back to display ram
	subi	XL,max_c	; subtract max_c from v_screen pointer
	sbci	XH,0
	ld	r20,X		; get new display ram byte

	tst	r20		; no borders in this byte? (all bits clear)
	sbrc	pmode,7		; -1 -> checkered fill,
	brne	ftc1a_int2 	; no, process byte normally (intermediate for ftc1a_7)
	brne	ftc1_7		; no, process byte normally

	ldi	r20,0x55	;	else set every 2nd bit in byte
	sbrs	XL,0		;	(pattern depending on XL being odd or even)
	ldi	r20,0xAA

	sbrs	pmode,7		; -1 -> checkered fill,
	ldi	r20,0xFF	; 	else set all bits accordingly,
	subi	r18,8		; dec y and
	rjmp	ftc1_00		; go for the next byte

gc_flv5:
	st	X,r20		; store byte back to display ram

gc_flv6:
	cp	r18,r8		; y>=y1 ?
	brlt	PC+2
	rjmp	gc_flv11	; goto 'skip' (too far for 'brxx')

	mov	r5,r18
	inc	r5		; l = y+1

	cp	r5,r8		; ** leak on top: l<y1 ?
	brsh 	gc_flv7		; no, skip push job

	dec	r8		; dec y1, push segment
	neg	r17		; -dx
	push_sc	r16, r5, r8, r17	; PUSH(x, l, y1-1, -dx)
	neg	r17		; reset dx
	inc	r8		; reset y1

gc_flv7:
	mov	r18,r8
	inc	r18		; y = y1+1

gc_flv8:
	; start of 'do' loop / start of 2nd 'for' loop
	; walk through display ram byte, set cleared pixels
	cpi	r18,max_r	; y now off-screen?
	brsh	ftc2a_int 	; yes, exit 2nd 'for' loop (intermediate for gc_flv9a)
	Get_PixVc		; read pixel

	; don't build a 'real' jumptable (would be much too slow),
	; calculate the dest. address directly into Z
	sbrs	pmode,7		; -1 -> checkered fill,
	rjmp	gc_sfill1	; 	else solid fill

	ldi	ZH,high(ftc2a_0); load 2nd label group base address (checkered fill)
	ldi	ZL,low(ftc2a_0)
	mul	r21,r12		; distance label/label = 5,
				; r21 = bit pos. #, r12 = 5 (reg var)
	add	ZL,r0		; set pointer
	adc	ZH,r1

	; start processing at correct bit position
	ijmp			; indirect jump

gc_sfill1:
	ldi	ZH,high(ftc2_0)	; load 2nd label group base address (solid fill)
	ldi	ZL,low(ftc2_0)
	lsl	r21		; distance label/label = 4
	lsl	r21		; r21 = bit pos. #
	add	ZL,r21		; set pointer
	adc	ZH,r6

	; start processing at correct bit position
	ijmp			; indirect jump

ftc2a_int:
	rjmp	gc_flv9a	; intermediate label

	; checkered fill routine
ftc2a_0:
	sbrc	r20,0		; border reached?
	rjmp	gc_flv9		; yes, exit loop
	sbrc	XL,0		;	else set every other pixel
	ori	r20,1		;	(depending on XL being odd or even)
	inc	r18		;	and inc y

ftc2a_1:
	sbrc	r20,1
	rjmp	gc_flv9
	sbrs	XL,0
	ori	r20,2
	inc	r18

ftc2a_2:
	sbrc	r20,2
	rjmp	gc_flv9
	sbrc	XL,0
	ori	r20,4
	inc	r18

ftc2a_3:
	sbrc	r20,3
	rjmp	gc_flv9
	sbrs	XL,0
	ori	r20,8
	inc	r18

ftc2a_4:
	sbrc	r20,4
	rjmp	gc_flv9
	sbrc	XL,0
	ori	r20,16
	inc	r18

ftc2a_5:
	sbrc	r20,5
	rjmp	gc_flv9
	sbrs	XL,0
	ori	r20,32
	inc	r18

ftc2a_6:
	sbrc	r20,6
	rjmp	gc_flv9
	sbrc	XL,0
	ori	r20,64
	inc	r18

ftc2a_7:
	sbrc	r20,7
	rjmp	gc_flv9
	sbrs	XL,0
	ori	r20,128
	inc	r18
	rjmp	ftc2_00

ftc2a_int2:
	rjmp	ftc2a_0		; intermediate label

	; solid fill routine
ftc2_0:	sbrc	r20,0		; border reached?
	rjmp	gc_flv9		; yes, exit loop
	ori	r20,1		;	else set pixel
	inc	r18		;	and inc y

ftc2_1:	sbrc	r20,1
	rjmp	gc_flv9
	ori	r20,2
	inc	r18

ftc2_2:	sbrc	r20,2
	rjmp	gc_flv9
	ori	r20,4
	inc	r18

ftc2_3:	sbrc	r20,3
	rjmp	gc_flv9
	ori	r20,8
	inc	r18

ftc2_4:	sbrc	r20,4
	rjmp	gc_flv9
	ori	r20,16
	inc	r18

ftc2_5:	sbrc	r20,5
	rjmp	gc_flv9
	ori	r20,32
	inc	r18

ftc2_6:	sbrc	r20,6
	rjmp	gc_flv9
	ori	r20,64
	inc	r18

ftc2_7:	sbrc	r20,7
	rjmp	gc_flv9
	ori	r20,128
	inc	r18

ftc2_00:
	cpi	r18,max_r	; y off-screen now?
	brsh	gc_flv9		; yes, exit 2nd 'for' loop

	; continue through line: get next byte from next v_screen page
	st	X,r20		; store previous byte back to display ram
	subi	XL,-max_c	; add max_c to v_screen pointer
	sbci	XH,0xFF
	ld	r20,X		; get next display ram byte

	tst	r20		; no borders in this byte? (all bits clear)
	sbrc	pmode,7		; -1 -> checkered fill,
	brne	ftc2a_int2	; no, process byte normally (intermediate for ftc2a_0)
	brne	ftc2_0		; no, process byte normally

	ldi	r20,0x55	;	else set every 2nd bit in byte
	sbrs	XL,0		;	(pattern depending on XL being odd or even)
	ldi	r20,0xAA

	sbrs	pmode,7		; -1 -> checkered fill,
	ldi	r20,0xFF	; 	else solid fill, set all bits accordingly,
	subi	r18,-8		; inc y and
	rjmp	ftc2_00		; go for the next byte

gc_flv9:
	st	X,r20		; store byte back to display ram

gc_flv9a:
	dec	r18		; y-1, push segment data
	push_sc	r16, r5, r18, r17	; PUSH(x, l, y-1, dx)

	cp	r9,r18		; ** leak on bottom: y-1>y2 ? (-> same as y>y2+1)
	brsh	gc_flv10	; no, skip next push job

	inc	r9		; y2+1
	neg	r17		; -dx
	push_sc	r16, r9, r18, r17	; PUSH(x, y2+1, y-1, -dx)
	neg	r17		; reset dx
	dec	r9		; reset y2

gc_flv10:
	inc	r18		; reset y-1 to y

gc_flv11:
	; 'skip' label, also start of 3rd 'for' loop
	inc	r18		; y++
	cp	r9,r18		; y<=y2 ?
	brlt	gc_flv13	; no, exit 3rd 'for' loop
	Get_PixVc		; read pixel

gc_flv12:
	; walk through display ram byte, exit at first cleared pixel
	; don't build a 'real' jumptable (would be much too slow),
	; calculate the dest. address directly into Z
	ldi	ZH,high(ftc3_0)	; load 3rd label group base address
	ldi	ZL,low(ftc3_0)
	mul	r21,r7		; distance label/label = 3,
				; r21 = bit pos. #, r7 = 3 (reg var)
	add	ZL,r0		; set pointer
	adc	ZH,r1

	; start processing at correct bit position
	ijmp			; indirect jump

ftc3_0:	sbrs	r20,0		; pixel cleared?
	rjmp	gc_flv13	; yes, exit loop
	inc	r18		;	else inc y

ftc3_1:	sbrs	r20,1
	rjmp	gc_flv13
	inc	r18

ftc3_2:	sbrs	r20,2
	rjmp	gc_flv13
	inc	r18

ftc3_3:	sbrs	r20,3
	rjmp	gc_flv13
	inc	r18

ftc3_4:	sbrs	r20,4
	rjmp	gc_flv13
	inc	r18

ftc3_5:	sbrs	r20,5
	rjmp	gc_flv13
	inc	r18

ftc3_6:	sbrs	r20,6
	rjmp	gc_flv13
	inc	r18

ftc3_7:	sbrs	r20,7
	rjmp	gc_flv13
	inc	r18

ftc3_00:
	cp	r9,r18		; y<=y2 ?
	brlt	gc_flv13	; no, exit 3rd 'for' loop

	; continue through line: get next byte from next v_screen page
	subi	XL,-max_c	; add max_c to v_screen pointer
	sbci	XH,0xFF
	ld	r20,X		; get next display ram byte

	cpi	r20,0xFF	; a leak in this byte? (cleared pixel)
	brne	ftc3_0		; yes, process byte normally
	subi	r18,-8		; no, inc y accordingly and
	rjmp	ftc3_00		; go fo the next byte

gc_flv13:
	mov	r5,r18		; l=y
	cp	r9,r18		; y<=y2 ?
	brsh	PC+2		; yes, loop (rjmps needed, too far for 'brxx')
	rjmp	gc_flv3		; no, pop next segment (** main loop end **)
	rjmp	gc_flv8		; y<=y2, continue 'do' loop

gc_flvexit:
	pop	r25
	pop	r24
	pop	r21
	pop	r20
	pop	r19
	pop	r18
	pop	r17
	pop	r16
	pop	r15

	pop	r12
	pop	r11
	pop	r10
	pop	r9
	pop	r8
	pop	r7
	pop	r6
	pop	r5
	pop	r4
	pop	r3
	pop	r2
	pop	r1
	pop	r0

	pop	ZH
	pop	ZL
	pop	YH
	pop	YL
	pop	XH
	pop	XL

	ret

;**** End function G_Seedp_Vc ------------------------------------------****


;**************************************************************************
;*
;* Function "G_Flood_NR" -  non-recursive FIFO queued flood fill
;*
;* Call with:
;*	 x1,y1	-> seed pixel coordinates (must be inside polygon/area)
;*
;* Returns:
;*	 -
;*
;* All regs saved
;*
;* Note: This routine is about 5 times slower than Mr. Heckbert's 'Seed Fill'
;*	 ('straight' version - the circular buffer AND dumb algorithm
;*	 - textbook stuff - take their toll).
;*	 This implementation is provided here to demonstrate a 16
;*	 bit FIFO queue buffer on an 8 bit MCU and its benefits - if for
;*	 whatever reasons you don't want to go for Heckbert's scanline seed,
;*	 queue size demands here are the lowest possible when using flood fill
;*	 (textbook stuff) algorithms.
;*
;* Note 2:
;* Define the additional array for this module:
;*
;* .equ	fillbuf_length=0x0200	; size of circular buffer (128-512 bytes,
;*				  *must* be an even number!)
;*
;* fillbuf_base:
;*	.byte fillbuf_length	; reserves fillbuf_length bytes for the
;*				; buffer and is used as the buffer뭩 base address
;*
;**************************************************************************

.macro fl_roll	; checks bounds and wraps the circular buffer (4/5 cycles)
	;ldi	r21,high(fillbuf_base + fillbuf_length) ; reg var
	cpi	ZL,low(fillbuf_base + fillbuf_length)
	cpc	ZH,r21
	brne	PC+3			; not yet end of buffer ram reached
	ldi	ZL,low(fillbuf_base)	; buffer rollover: load fillbuf_in with
	ldi	ZH,high(fillbuf_base)	; fillbuf_base
.endm


G_Flood_NR:
	push	XL
	push	XH
	push	YL
	push	YH
	push	ZL
	push	ZH

	push	r0
	push	r1
	push	r2
	push	r3
	push	r4
	push	r5
	push	r6
	push	r7

	push	r20	; scrap
	push	r16	; x1
	push	r17
	push	r18	; y1
	push	r19
	push	r23
	push	r24
	push	r25
	push	r21
	push	r19


	; common constants - don't touch! (r4 - r7, r16, r17, r21, r19, r23)
	clr	r6			; reg var (0)
	ldi	r20,max_c
	mov	r7,r20			; reg var (max_c)

	ldi	r21,high(fillbuf_base + fillbuf_length)

	; initialize vscreen pointer
	ldi     r17,low(g_vscreen)
	ldi     r19,high(g_vscreen)

	ldi	r23,16			; reg var

	ldi	r20,low(fillbuf_length)	; reg vars
	mov	r4,r20
	ldi	r20,high(fillbuf_length)
	mov	r5,r20


	ldi	ZL,low(fillbuf_base)	; init buffer
	ldi	ZH,high(fillbuf_base)

	movw	YL,ZL			; store fillbuf_out pointer to Y

	; store first pixel to buffer
	st	Z+,x1			; ** store x1 **
	st	Z+,y1			; ** store y1 **
	clr	r25			; set counter (2 bytes in buffer)
	ldi	r24,2

fl_nr1:	; ** main loop: retrieve pixel if buffer not empty **
	cp	r24,r6			; buffer empty?
	cpc	r25,r6			; r6 = 0
	breq	fl_nr2			; yes, EXIT function

	; else get a pixel from circular buffer
	ld	x1,Y+			; ** get x1 **
	ld	y1,Y+			; ** get y1 **

	; check if we have to wrap fillbuf_out pointer (4/5 cycles)
	cpi	YL,low(fillbuf_base + fillbuf_length)
	cpc	YH,r21
	brne	PC+3			; if end of buffer ram reached,
	ldi	YL,low(fillbuf_base)	; rollover: load fillbuf_out with
	ldi	YH,high(fillbuf_base)	; fillbuf_base

	sbiw	r24,2			; decrement counter by 2

	cpi	x1,max_c		; boundary checks,
	brsh	fl_nr1			; loop if pixel is off-screen
	cpi	y1,max_r		; (prevents crash if polygon 'leaks')
	brsh	fl_nr1

	; if pixel cleared, set it
	rcall	chk_pix		; fetch and check a pixel
	brne	fl_nr1		; if pixel set, loop, else ->
	or	r0,r2		; set pixel
	st	X,r0		; stuff it back to display ram

	; if buffer not full, store the neighboring pixel's coordinates
	cp	r24,r4		; counter low/high:
	cpc	r25,r5		; check if buffer full
	brlo	fl_nr3
fl_nr2:	rjmp	fl_nr_exit	; if full, exit function

fl_nr3:
	inc	y1		; ** x,y+1 **

	rcall	chk_pix		; neighbor already set?
	brne	fl_pck1		; if pixel set, don't store pixel coordinates
	st	Z+,x1		; store x1
	st	Z+,y1		; store y1
	fl_roll			; do we have to wrap (circ. buffer)?
	adiw	r24,2		; 2 bytes added to buffer, inc counter

fl_pck1:
	subi	y1,2		; ** x,y-1 **

	rcall	chk_pix		; neighbor already set?
	brne	fl_pck2		; if pixel set, don't store pixel coordinates
	st	Z+,x1		; store x1
	st	Z+,y1		; store y1
	fl_roll			; do we have to wrap (circ. buffer)?
	adiw	r24,2		; 2 bytes added to buffer, inc counter

fl_pck2:
	inc	y1
	inc	x1		; ** x+1,y **

	rcall	chk_pix		; neighbor already set?
	brne	fl_pck3		; if pixel set, don't store pixel coordinates
	st	Z+,x1		; store x1
	st	Z+,y1		; store y1
	fl_roll			; do we have to wrap (circ. buffer)?
	adiw	r24,2		; 2 bytes added to buffer, inc counter

fl_pck3:
	subi	x1,2		; ** x-1,y **

	rcall	chk_pix		; neighbor already set?
	brne	fl_pck4		; if pixel set, don't store pixel coordinates
	st	Z+,x1		; store x1
	st	Z+,y1		; store y1
	fl_roll			; do we have to wrap (circ. buffer)?
	adiw	r24,2		; 2 bytes added to buffer, inc counter

fl_pck4:
	rjmp	fl_nr1		; loop

chk_pix:	; fetches a pixel and checks if it is set or cleared
        mov     r20,r18		; r2=sram(bytab_offset+mod(ypos/8))
        andi    r20,0x07
	ldi	XL,low(g_btable)
	ldi	XH,high(g_btable)	; will become 0!
        add     XL,r20          ; pointer to table offset
        ld      r2,X		; fetch bit mask

	fmul	r18,r23		; int(ypos/8) -> r23 = 16, r18 = y1
	mul	r7,r1		; * max_c -> r7 = max_c
        movw    XL,r0
        add     XL,r16		; + xpos
	adc	XH,r6		; -> r6 = 0
        add     XL,r17		; + g_vscreen_start
        adc     XH,r19

        ld      r0,X            ; fetch display ram byte
	mov	r20,r0		; saves 1 cycle (no reload of display ram byte)
	and	r20,r2		; read pixel status: equal = clear, not equal = set
	ret

fl_nr_exit:
	pop	r19
	pop	r21
	pop	r25
	pop	r24
	pop	r23
	pop	r16
	pop	r18
	pop	r17
	pop	r16
	pop	r20

	pop	r7
	pop	r6
	pop	r5
	pop	r4
	pop	r3
	pop	r2
	pop	r1
	pop	r0
	pop	ZH
	pop	ZL
	pop	YH
	pop	YL
	pop	XH
	pop	XL

	ret

;**** End function G_Flood_NR -----------------------------------------****


; non-recursive boundary fill, 4-way flood
;**************************************************************************
;* Function "G_Flood" -  non-recursive (non FIFO) flood fill
;*
;* Call with:
;*	 x1,y1	-> seed pixel coordinates (must be inside area)
;*
;* Returns:
;*	 -
;*
;* All regs saved
;*
;* Note: This routine is about 3 times slower than Mr. Heckbert's 'seed fill'
;*	 ('straight' version) It also requires about 4 to 5 times more buffer
;*	 space (depending on area shape and size) than the similar FIFO
;*	 queued version "G_Flood_NR". Code space requirements are minimized,
;*	 but the private stack needed is bigger than in G_Flood_NR.
;*
;* Note2:
;* Define a sufficient private stack for the pixel coordinates buffer:
;*
;* fillbuf_base:	; stack for non-recursive calls
;*	.BYTE 768	; put this buffer last - will grow towards stack!
;*
;**************************************************************************

.macro RWS_Pixel	; read/write/store routine, 22 words
	; x=int(ypos/8)*max_c+xpos+g_vscreen_start
	fmul	r18,r23		; int(ypos/8) -> r23 = 16, r18 = y1
	mul	r14,r1		; * max_c -> r14 = max_c
        movw    XL,r0
        add     XL,r16		; + xpos
	adc	XH,r4		; -> r4 = 0
        add     XL,r24		; + g_vscreen_start
        adc     XH,r25

        mov     r20,r18		; r1=sram(bytab_offset+mod(ypos/8))
        andi    r20,0x07
        add     YL,r20          ; pointer to table offset
        ld      r1,Y
	sub	YL,r20		; reset pointer offset

        ld      r0,X            ; get display ram byte
	mov	r20,r0		; saves 1 cycle (no reload of display ram byte)

	and	r20,r1		; read pixel status
	brne	PC+5		; if pixel set, exit
	or	r0,r1		;	else set pixel
	st	X,r0		; stuff it back to display ram
	st	Z+,r16		; ... and store coordinates to queue
	st	Z+,r18
	; ** read/write pixel exit **
.endm

G_Flood:
	push	XL
	push	XH
	push	YL
	push	YH
	push	ZL
	push	ZH

	push	r0
	push	r1
	push	r2
	push	r3
	push	r4	; reg var (0)
	push	r14
	push	r20	; scrap
	push	r16	; x1, xpos (x1/y1 not destroyed)
	push	r18	; y1, ypos
	push	r23
	push	r24
	push	r25


	; common constants - don't touch! (Y, r14, r23, r24)
	ldi	YL,low(g_btable)
	ldi	YH,high(g_btable)
	clr	r4		; reg var (0)

	ldi	r24,max_c
	mov	r14,r24		; reg var

	; initialize vscreen pointer
	ldi     r24,low(g_vscreen)
	ldi     r25,high(g_vscreen)

	ldi	r23,16

	ldi	ZL,low(fillbuf_base)
	ldi	ZH,high(fillbuf_base)

	mov	r3,ZH		; save Z-pointer origin for queue compare
	mov	r2,ZL

	st	Z+,r16		; store first pixel to queue
	st	Z+,r18

ff4_nr1:
	; check queue, exit if empty
	cp	ZL,r2
	cpc	ZH,r3
	brne	ff4_nr2
	rjmp	ff4_nr3		; exit

ff4_nr2:
	ld	r18,-Z		; retrieve pixel from queue
	ld	r16,-Z

	cpi	x1,max_c-1	; boundary checks
	brsh	ff4_nr1		; next pixel if this one would be off-screen
	cpi	y1,max_r-1
	brsh	ff4_nr1

	inc	y1		; x,y+1
	RWS_Pixel

	subi	y1,2		; x,y-1
	RWS_Pixel

	inc	y1
	inc	x1		; x+1,y
	RWS_Pixel

	subi	x1,2		; x-1,y
	RWS_Pixel

	rjmp	ff4_nr1

ff4_nr3:
	pop	r25
	pop	r24
	pop	r23
	pop	r18
	pop	r16
	pop	r20
	pop	r14

	pop	r4
	pop	r3
	pop	r2
	pop	r1
	pop	r0
	pop	ZH
	pop	ZL
	pop	YH
	pop	YL
	pop	XH
	pop	XL
	ret


ff4_nrW:	; alternative: callable module read/write/store pixel with 22
		; (+7 cycles overhead), could be used instead of macro "RWS_Pixel"
	; x=int(ypos/8)*max_c+xpos+g_vscreen_start
	fmul	r18,r23		; int(ypos/8) -> r23 = 16, r18 = y1
	mul	r14,r1		; * max_c -> r14 = max_c
        movw    XL,r0
        add     XL,r16		; + xpos
	adc	XH,r4		; -> r4 = 0
        add     XL,r24		; + g_vscreen_start
        adc     XH,r25

        mov     r20,r18		; r1=sram(bytab_offset+mod(ypos/8))
        andi    r20,0x07
        add     YL,r20          ; pointer to table offset
        ld      r1,Y
	sub	YL,r20		; reset pointer offset

        ld      r0,X            ; get display ram byte
        mov	r20,r0		; copy into r20 (destroyed by "and")
	and	r20,r1		; read pixel:
	breq	ff_nrW1		; if pixel cleared, go to "set pixel"
	ret			; else if pixel already set, exit

ff_nrW1:
	or	r0,r1		; set pixel
	st	X,r0		; stuff it back to display ram

	st	Z+,r16		; ... and store its coordinates to queue
	st	Z+,r18

	ret

;**** End function G_Flood --------------------------------------------****


;**************************************************************************
;*
;* A standard flood fill for comparison: Recursive Boundary Fill
;*
;* This is straight textbook stuff, used by many compilers.
;*
;* As a common denominator algorithms like this one make heavy use of
;* recursion ("call from call from call ...") guaranteeing very nice looking
;* and very 'compiler-friendly' compact C-code (also trying hard
;* to achieve stack overflows, especially on MCUs w/o external RAM).
;*
;* The push/pop orgies assist in slowing down the code, too - not
;* recommended for practical purposes like feeding this to a real MCU.
;*
;* Provided here for comparison is just the code backbone, no regs
;* saved, no reg var setup etc. - cf. e.g. "G_Flood" for further
;* details in that respect if you'd like to do some benchmarking.
;*
;**************************************************************************

; conventional recursive boundary fill, heavy stack usage (will likely cause
; overflow problems on MCUs)
;
; public void boundaryFill(int x, int y, int fill, int boundary) {
;        if ((x < 0) || (x >= width)) return; /* boundary checks */
;        if ((y < 0) || (y >= height)) return;
;        int current = raster.getPixel(x, y);
;        if ((current != boundary) && (current != fill)) {
;            raster.setPixel(fill, x, y);
;            boundaryFill(x+1, y, fill, boundary);
;            boundaryFill(x, y+1, fill, boundary);
;            boundaryFill(x-1, y, fill, boundary);
;            boundaryFill(x, y-1, fill, boundary);
;        }
;    }

Bound_fill:
	; Note: reg var setup and boundary checks omitted

	; ** read/write pixel start **
	; x=int(ypos/8)*max_c+xpos+g_vscreen_start
	fmul	r18,r23		; int(ypos/8) -> r23 = 16, r18 = y1
	mul	r14,r1		; * max_c -> reg var r14 = max_c
        movw    XL,r0
        add     XL,r16		; + xpos
	adc	XH,r4		; -> reg var r4 = 0
        add     XL,r24		; + g_vscreen_start
        adc     XH,r25

        mov     r20,r18		; r1=sram(bytab_offset+mod(ypos/8))
        andi    r20,0x07
        add     YL,r20          ; pointer to table offset
        ld      r1,Y
	sub	YL,r20		; reset pointer offset

        ld      r0,X            ; get display ram byte
        mov	r20,r0
	and	r20,r1		; read pixel:
	breq	b_fill1		; if pixel cleared, go to "set pixel"
	rjmp	b_fill2		; else if pixel already set, exit with carry set

b_fill1:
	or	r0,r1		; set pixel
	st	X,r0		; stuff it back to display ram

	inc	x1
	rcall	bound_fill
	dec	x1

	inc	y1
	rcall	bound_fill
	dec	y1

	dec	x1
	rcall	bound_fill
	inc	x1

	dec	y1
	rcall	bound_fill
	inc	y1
b_fill2:
	ret

;**** End Bound_Fill ------------------------------------------------------


;**************************************************************************
;*
;* Function G_Drawbutton - draws a 3D button
;*
;* Call with:
;*	 XL,YL,XH,YH	-> corner coordinates
;*
;*			   Upper left corner: (XL,YL)
;* 			   Lower right corner: (XH,YH)
;*
;* Note: button face and sides are drawn in white, outline in black.
;*
;* All regs saved, T-flag cleared
;*
;**************************************************************************

G_Drawbutton:
	push	r20
	push	pmode
	push	x1
	push	x2
	push	y1
	push	y2

	clr	pmode
	inc	pmode

	mov	r20,YH
	sub	r20,YL
	inc	r20

	mov	x1,XL
	mov	x2,XH
	subi	x2,-2
	mov	y1,YL
	mov	y2,YL

g_bu1:
	rcall	g_hline
	inc	y1
	inc	y2
	dec	r20
	brpl	g_bu1

	clr	pmode

	mov	x1,XL
	mov	y1,YL
	mov	x2,XH
	mov	Y2,YH
	rcall	g_drawframe

	mov	x1,XL
	subi	x1,-2
	mov	x2,XH
	subi	x2,-2
	mov	y1,YH
	subi	y1,-2
	mov	y2,YH
	subi	y2,-2
	rcall	g_drawline
	mov	x1,XH
	subi	x1,-2
	mov	y1,YL
	subi	y1,-2
	rcall	g_drawline

	mov	x1,XL
	inc	x1
	mov	y1,YH
	inc	y1
	rcall	g_plotxy
	mov	x1,XH
	inc	x1
	rcall	g_plotxy
	mov	y1,YL
	inc	y1
	rcall	g_plotxy

	pop	y2
	pop	y1
	pop	x2
	pop	x1
	pop	pmode
	pop	r20

	ret

;**** End function G_Drawbutton ---------------------------------------****


;**************************************************************************
;*
;* Function G_Lcd_inv - invert screen (all pixels reversed)
;*
;* Function G_Lcd_set - fill screen (all pixels set)
;*
;* Function G_Lcd_clr - clear screen (all pixels cleared)
;*
;* Call with: -
;*
;* All regs saved, T-flag cleared
;*
;**************************************************************************

g_lcd_inv:
	push	XL
	set
	rjmp	g_lc1

g_lcd_set:
	push	XL
	ser	XL		; FFh: set pixels
	rjmp	g_lc0

g_lcd_clr:
	push	XL
	clr	XL		; 00h: clear pixels

g_lc0:
	clt

g_lc1:
	push	XH
	push	YL
	push	YH
	push	ZL
	push	ZH

	ldi	YL,low(g_vscreen)
	ldi	YH,high(g_vscreen)
	ldi	ZL,low(gbuf_size)
	ldi	ZH,high(gbuf_size)

	brtc	g_lc3		; T-flag cleared -> set/clear screen
				; else invert screen
	ser	XH

g_lc2:	ld 	XL,Y
	eor	XL,XH
	st 	Y+,XL
	sbiw 	ZL,1
	brne 	g_lc2

	clt
	rjmp	g_lc4

g_lc3:
	; set/clear all pixels
	st	Y+,XL
	sbiw	ZL,1
	brne	g_lc3

g_lc4:
	pop	ZH
	pop	ZL
	pop	YH
	pop	YL
	pop	XH
	pop	XL
	ret

;**** End functions G_lcd_set, g_lcd_clr & g_lcd_inv -----------------****


; 'Graphics mode' fonts, code page 437 (complete 'IBM PC extended character set',
; char. 00='trademark symbol', char. 127= 'copyright symbol')
; Note: The character matrix is column orientated (8 pixels/byte vertically organized)!
; If you don't want the 'full' character set you'll have to add some bounds checking
; to the character/string print functions.

g_font8x8b:	; bold 8x8 font, IBM/ASCII # 00-FF
.db 0x7e, 0x81, 0xbd, 0x95, 0xad, 0x81, 0x7e, 0x00, 0x7e, 0x81, 0x95, 0xb1, 0xb1, 0x95, 0x81, 0x7e
.db 0x7e, 0xff, 0xeb, 0xcf, 0xcf, 0xeb, 0xff, 0x7e, 0x0e, 0x1f, 0x3f, 0x7e, 0x3f, 0x1f, 0x0e, 0x00
.db 0x08, 0x1c, 0x3e, 0x7f, 0x3e, 0x1c, 0x08, 0x00, 0x38, 0x38, 0x9f, 0xff, 0x9f, 0x38, 0x38, 0x00
.db 0x10, 0x38, 0xbc, 0xff, 0xbc, 0x38, 0x10, 0x00, 0x00, 0x00, 0x18, 0x3c, 0x3c, 0x18, 0x00, 0x00
.db 0xff, 0xff, 0xe7, 0xc3, 0xc3, 0xe7, 0xff, 0xff, 0x00, 0x3c, 0x66, 0x42, 0x42, 0x66, 0x3c, 0x00
.db 0xff, 0xc3, 0x99, 0xbd, 0xbd, 0x99, 0xc3, 0xff, 0x70, 0xf8, 0x88, 0xfd, 0x7f, 0x07, 0x0f, 0x00
.db 0x4e, 0x5f, 0xf1, 0xf1, 0x5f, 0x4e, 0x00, 0x00, 0xc0, 0xe0, 0xff, 0x7f, 0x05, 0x07, 0x07, 0x00
.db 0xc0, 0xff, 0x7f, 0x05, 0x65, 0x7f, 0x3f, 0x00, 0x5a, 0x5a, 0x3c, 0xe7, 0xe7, 0x3c, 0x5a, 0x5a
.db 0x7f, 0x3e, 0x3e, 0x1c, 0x1c, 0x08, 0x08, 0x00, 0x08, 0x08, 0x1c, 0x1c, 0x3e, 0x3e, 0x7f, 0x00
.db 0x00, 0x24, 0x66, 0xff, 0x66, 0x24, 0x00, 0x00, 0x5f, 0x5f, 0x00, 0x00, 0x5f, 0x5f, 0x00, 0x00
.db 0x06, 0x0f, 0x39, 0x7f, 0x01, 0x3f, 0x7f, 0x00, 0x4a, 0x9f, 0xb5, 0xad, 0xf9, 0x52, 0x00, 0x00
.db 0x70, 0x70, 0x70, 0x70, 0x70, 0x70, 0x70, 0x00, 0x00, 0x94, 0xbe, 0xff, 0xbe, 0x94, 0x00, 0x00
.db 0x08, 0x0c, 0x7e, 0x7f, 0x7e, 0x0c, 0x08, 0x00, 0x08, 0x18, 0x3f, 0x7f, 0x3f, 0x18, 0x08, 0x00
.db 0x1c, 0x1c, 0x1c, 0x7f, 0x3e, 0x1c, 0x08, 0x00, 0x08, 0x1c, 0x3e, 0x7f, 0x1c, 0x1c, 0x1c, 0x00
.db 0x3c, 0x3c, 0x20, 0x20, 0x20, 0x20, 0x20, 0x00, 0x10, 0x38, 0x7c, 0x38, 0x7c, 0x38, 0x10, 0x00
.db 0x30, 0x38, 0x3c, 0x3e, 0x3c, 0x38, 0x30, 0x00, 0x06, 0x0e, 0x1e, 0x3e, 0x1e, 0x0e, 0x06, 0x00
.db 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x5f, 0x5f, 0x06, 0x00, 0x00, 0x00
.db 0x07, 0x07, 0x00, 0x07, 0x07, 0x00, 0x00, 0x00, 0x14, 0x7f, 0x7f, 0x14, 0x7f, 0x7f, 0x14, 0x00
.db 0x04, 0x2e, 0x6f, 0x2a, 0x7b, 0x3a, 0x10, 0x00, 0x46, 0x66, 0x30, 0x18, 0x0c, 0x66, 0x62, 0x00
.db 0x30, 0x7a, 0x4f, 0x5d, 0x37, 0x72, 0x48, 0x00, 0x00, 0x04, 0x07, 0x03, 0x00, 0x00, 0x00, 0x00
.db 0x00, 0x00, 0x1c, 0x3e, 0x63, 0x41, 0x00, 0x00, 0x00, 0x41, 0x63, 0x3e, 0x1c, 0x00, 0x00, 0x00
.db 0x00, 0x24, 0x18, 0x3c, 0x18, 0x24, 0x00, 0x00, 0x00, 0x08, 0x08, 0x3e, 0x08, 0x08, 0x00, 0x00
.db 0x00, 0x80, 0xe0, 0x60, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x08, 0x08, 0x08, 0x08, 0x00, 0x00
.db 0x00, 0x60, 0x60, 0x00, 0x00, 0x00, 0x00, 0x00, 0x60, 0x30, 0x18, 0x0c, 0x06, 0x03, 0x01, 0x00
.db 0x3e, 0x7f, 0x41, 0x49, 0x41, 0x7f, 0x3e, 0x00, 0x42, 0x43, 0x7f, 0x7f, 0x40, 0x40, 0x00, 0x00
.db 0x62, 0x73, 0x59, 0x49, 0x6f, 0x66, 0x00, 0x00, 0x22, 0x63, 0x49, 0x49, 0x7f, 0x36, 0x00, 0x00
.db 0x18, 0x1c, 0x16, 0x53, 0x7f, 0x58, 0x00, 0x00, 0x27, 0x67, 0x45, 0x45, 0x7d, 0x39, 0x00, 0x00
.db 0x3c, 0x7e, 0x4b, 0x49, 0x79, 0x30, 0x00, 0x00, 0x03, 0x03, 0x71, 0x79, 0x0f, 0x07, 0x00, 0x00
.db 0x36, 0x7f, 0x49, 0x49, 0x7f, 0x36, 0x00, 0x00, 0x06, 0x4f, 0x49, 0x69, 0x3f, 0x1e, 0x00, 0x00
.db 0x00, 0x00, 0x66, 0x66, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0xe6, 0x66, 0x00, 0x00, 0x00, 0x00
.db 0x00, 0x08, 0x1c, 0x36, 0x63, 0x41, 0x00, 0x00, 0x24, 0x24, 0x24, 0x24, 0x24, 0x24, 0x00, 0x00
.db 0x00, 0x41, 0x63, 0x36, 0x1c, 0x08, 0x00, 0x00, 0x06, 0x03, 0x51, 0x59, 0x0f, 0x06, 0x00, 0x00
.db 0x3c, 0x66, 0xdb, 0xa5, 0xa5, 0xbd, 0x82, 0x00, 0x7c, 0x7e, 0x13, 0x13, 0x7e, 0x7c, 0x00, 0x00
.db 0x41, 0x7f, 0x7f, 0x49, 0x49, 0x7f, 0x36, 0x00, 0x1c, 0x3e, 0x63, 0x41, 0x41, 0x63, 0x22, 0x00
.db 0x41, 0x7f, 0x7f, 0x41, 0x63, 0x3e, 0x1c, 0x00, 0x41, 0x7f, 0x7f, 0x49, 0x5d, 0x41, 0x63, 0x00
.db 0x41, 0x7f, 0x7f, 0x49, 0x1d, 0x01, 0x03, 0x00, 0x1c, 0x3e, 0x63, 0x41, 0x51, 0x33, 0x72, 0x00
.db 0x7f, 0x7f, 0x08, 0x08, 0x7f, 0x7f, 0x00, 0x00, 0x00, 0x41, 0x7f, 0x7f, 0x41, 0x00, 0x00, 0x00
.db 0x30, 0x70, 0x40, 0x41, 0x7f, 0x3f, 0x01, 0x00, 0x41, 0x7f, 0x7f, 0x08, 0x1c, 0x77, 0x63, 0x00
.db 0x41, 0x7f, 0x7f, 0x41, 0x40, 0x60, 0x70, 0x00, 0x7f, 0x7f, 0x0e, 0x1c, 0x0e, 0x7f, 0x7f, 0x00
.db 0x7f, 0x7f, 0x06, 0x0c, 0x18, 0x7f, 0x7f, 0x00, 0x3e, 0x7f, 0x41, 0x41, 0x41, 0x7f, 0x3e, 0x00
.db 0x41, 0x7f, 0x7f, 0x49, 0x09, 0x0f, 0x06, 0x00, 0x1e, 0x3f, 0x21, 0x61, 0x71, 0x5f, 0x5e, 0x00
.db 0x41, 0x7f, 0x7f, 0x09, 0x19, 0x7f, 0x66, 0x00, 0x22, 0x67, 0x4d, 0x49, 0x59, 0x73, 0x22, 0x00
.db 0x03, 0x41, 0x7f, 0x7f, 0x41, 0x03, 0x00, 0x00, 0x3f, 0x7f, 0x40, 0x40, 0x7f, 0x3f, 0x00, 0x00
.db 0x1f, 0x3f, 0x60, 0x60, 0x3f, 0x1f, 0x00, 0x00, 0x7f, 0x78, 0x30, 0x18, 0x30, 0x78, 0x7f, 0x00
.db 0x41, 0x63, 0x3e, 0x1c, 0x3e, 0x63, 0x41, 0x00, 0x07, 0x4e, 0x78, 0x78, 0x4e, 0x07, 0x00, 0x00
.db 0x47, 0x63, 0x71, 0x59, 0x4d, 0x67, 0x73, 0x00, 0x00, 0x00, 0x7f, 0x7f, 0x41, 0x41, 0x00, 0x00
.db 0x01, 0x03, 0x06, 0x0c, 0x18, 0x30, 0x60, 0x00, 0x00, 0x41, 0x41, 0x7f, 0x7f, 0x00, 0x00, 0x00
.db 0x00, 0x04, 0x06, 0x03, 0x06, 0x04, 0x00, 0x00, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80
.db 0x00, 0x00, 0x00, 0x03, 0x07, 0x04, 0x00, 0x00, 0x20, 0x74, 0x54, 0x54, 0x3c, 0x78, 0x40, 0x00
.db 0x41, 0x7f, 0x3f, 0x44, 0x44, 0x7c, 0x38, 0x00, 0x38, 0x7c, 0x44, 0x44, 0x6c, 0x28, 0x00, 0x00
.db 0x38, 0x7c, 0x44, 0x45, 0x3f, 0x7f, 0x40, 0x00, 0x38, 0x7c, 0x54, 0x54, 0x5c, 0x18, 0x00, 0x00
.db 0x48, 0x7e, 0x7f, 0x49, 0x03, 0x02, 0x00, 0x00, 0x98, 0xbc, 0xa4, 0xa4, 0xf8, 0x7c, 0x04, 0x00
.db 0x41, 0x7f, 0x7f, 0x08, 0x04, 0x7c, 0x78, 0x00, 0x00, 0x44, 0x7d, 0x7d, 0x40, 0x00, 0x00, 0x00
.db 0x60, 0xe0, 0x80, 0x80, 0xfd, 0x7d, 0x00, 0x00, 0x41, 0x7f, 0x7f, 0x10, 0x38, 0x6c, 0x44, 0x00
.db 0x00, 0x41, 0x7f, 0x7f, 0x40, 0x00, 0x00, 0x00, 0x7c, 0x7c, 0x18, 0x38, 0x1c, 0x7c, 0x78, 0x00
.db 0x7c, 0x7c, 0x04, 0x04, 0x7c, 0x78, 0x00, 0x00, 0x38, 0x7c, 0x44, 0x44, 0x7c, 0x38, 0x00, 0x00
.db 0x84, 0xfc, 0xf8, 0xa4, 0x24, 0x3c, 0x18, 0x00, 0x18, 0x3c, 0x24, 0xa4, 0xf8, 0xfc, 0x84, 0x00
.db 0x44, 0x7c, 0x78, 0x4c, 0x04, 0x0c, 0x08, 0x00, 0x48, 0x5c, 0x54, 0x54, 0x74, 0x24, 0x00, 0x00
.db 0x00, 0x04, 0x3e, 0x7f, 0x44, 0x24, 0x00, 0x00, 0x3c, 0x7c, 0x40, 0x40, 0x3c, 0x7c, 0x40, 0x00
.db 0x1c, 0x3c, 0x60, 0x60, 0x3c, 0x1c, 0x00, 0x00, 0x3c, 0x70, 0x60, 0x30, 0x60, 0x70, 0x3c, 0x00
.db 0x44, 0x6c, 0x38, 0x10, 0x38, 0x6c, 0x44, 0x00, 0x9c, 0xbc, 0xa0, 0xa0, 0xfc, 0x7c, 0x00, 0x00
.db 0x4c, 0x64, 0x74, 0x5c, 0x4c, 0x64, 0x00, 0x00, 0x00, 0x08, 0x3e, 0x77, 0x41, 0x41, 0x00, 0x00
.db 0x00, 0x00, 0x77, 0x77, 0x00, 0x00, 0x00, 0x00, 0x00, 0x41, 0x41, 0x77, 0x3e, 0x08, 0x00, 0x00
.db 0x08, 0x0c, 0x04, 0x0c, 0x08, 0x0c, 0x04, 0x00, 0x7c, 0x44, 0x44, 0x44, 0x44, 0x44, 0x7c, 0x00
.db 0x0e, 0x9f, 0x91, 0xb1, 0xfb, 0x4a, 0x00, 0x00, 0x3a, 0x7a, 0x40, 0x40, 0x7a, 0x7a, 0x40, 0x00
.db 0x38, 0x7c, 0x54, 0x55, 0x5d, 0x19, 0x00, 0x00, 0x22, 0x75, 0x55, 0x55, 0x7d, 0x7b, 0x42, 0x00
.db 0x21, 0x75, 0x54, 0x54, 0x7d, 0x79, 0x40, 0x00, 0x21, 0x75, 0x55, 0x54, 0x7c, 0x78, 0x40, 0x00
.db 0x20, 0x74, 0x57, 0x57, 0x7c, 0x78, 0x40, 0x00, 0x18, 0x3c, 0xa4, 0xa4, 0xe4, 0x40, 0x00, 0x00
.db 0x3b, 0x7d, 0x55, 0x55, 0x5d, 0x1b, 0x00, 0x00, 0x39, 0x7d, 0x54, 0x54, 0x5d, 0x19, 0x00, 0x00
.db 0x39, 0x7d, 0x55, 0x54, 0x5c, 0x18, 0x00, 0x00, 0x01, 0x45, 0x7c, 0x7c, 0x41, 0x01, 0x00, 0x00
.db 0x02, 0x03, 0x45, 0x7d, 0x7d, 0x43, 0x02, 0x00, 0x01, 0x45, 0x7d, 0x7c, 0x40, 0x00, 0x00, 0x00
.db 0x79, 0x7d, 0x16, 0x12, 0x16, 0x7d, 0x79, 0x00, 0x78, 0x7c, 0x15, 0x15, 0x15, 0x7c, 0x78, 0x00
.db 0x44, 0x7c, 0x7c, 0x55, 0x55, 0x45, 0x00, 0x00, 0x74, 0x54, 0x54, 0x3c, 0x7c, 0x54, 0x54, 0x00
.db 0x7c, 0x7e, 0x0b, 0x09, 0x7f, 0x7f, 0x49, 0x00, 0x32, 0x7b, 0x49, 0x49, 0x7b, 0x32, 0x00, 0x00
.db 0x32, 0x7a, 0x48, 0x48, 0x7a, 0x32, 0x00, 0x00, 0x32, 0x7a, 0x4a, 0x48, 0x78, 0x30, 0x00, 0x00
.db 0x3a, 0x7b, 0x41, 0x41, 0x7b, 0x7a, 0x40, 0x00, 0x3a, 0x7a, 0x42, 0x40, 0x78, 0x78, 0x40, 0x00
.db 0x9a, 0xba, 0xa0, 0xa0, 0xfa, 0x7a, 0x00, 0x00, 0x19, 0x3c, 0x66, 0x66, 0x3c, 0x19, 0x01, 0x00
.db 0x3d, 0x7d, 0x40, 0x40, 0x7d, 0x3d, 0x00, 0x00, 0xb8, 0x7c, 0x64, 0x54, 0x4c, 0x6c, 0x2a, 0x00
.db 0x68, 0x7e, 0x7f, 0x49, 0x43, 0x66, 0x20, 0x00, 0x01, 0x13, 0x56, 0x7c, 0x7c, 0x56, 0x13, 0x01
.db 0xff, 0xff, 0x09, 0x09, 0x2f, 0xf6, 0xa0, 0x00, 0x40, 0xc0, 0x88, 0xfe, 0x7f, 0x09, 0x03, 0x02
.db 0x20, 0x74, 0x54, 0x55, 0x7d, 0x79, 0x40, 0x00, 0x00, 0x44, 0x7d, 0x7d, 0x41, 0x00, 0x00, 0x00
.db 0x30, 0x78, 0x48, 0x4a, 0x7a, 0x32, 0x00, 0x00, 0x38, 0x78, 0x40, 0x42, 0x7a, 0x7a, 0x40, 0x00
.db 0x7a, 0x7a, 0x0a, 0x0a, 0x7a, 0x70, 0x00, 0x00, 0x7d, 0x7d, 0x19, 0x31, 0x7d, 0x7d, 0x00, 0x00
.db 0x26, 0x2f, 0x29, 0x2f, 0x2f, 0x28, 0x00, 0x00, 0x00, 0x26, 0x2f, 0x29, 0x2f, 0x26, 0x00, 0x00
.db 0x30, 0x78, 0x4d, 0x45, 0x60, 0x20, 0x00, 0x00, 0x38, 0x38, 0x08, 0x08, 0x08, 0x08, 0x00, 0x00
.db 0x08, 0x08, 0x08, 0x08, 0x38, 0x38, 0x00, 0x00, 0x4f, 0x6f, 0x30, 0x18, 0xcc, 0xee, 0xbb, 0x91
.db 0x4f, 0x6f, 0x30, 0x18, 0x6c, 0x76, 0xdb, 0xf9, 0x00, 0x30, 0x7d, 0x7d, 0x30, 0x00, 0x00, 0x00
.db 0x00, 0x08, 0x1c, 0x22, 0x08, 0x1c, 0x22, 0x00, 0x00, 0x22, 0x1c, 0x08, 0x22, 0x1c, 0x08, 0x00
.db 0xaa, 0x00, 0x55, 0x00, 0xaa, 0x00, 0x55, 0x00, 0xaa, 0x55, 0xaa, 0x55, 0xaa, 0x55, 0xaa, 0x55
.db 0x22, 0x00, 0x55, 0x88, 0x22, 0x55, 0x00, 0x88, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00
.db 0x10, 0x10, 0x10, 0xff, 0xff, 0x00, 0x00, 0x00, 0x14, 0x14, 0x14, 0xff, 0xff, 0x00, 0x00, 0x00
.db 0x10, 0x10, 0xff, 0xff, 0x00, 0xff, 0xff, 0x00, 0x10, 0x10, 0xf0, 0xf0, 0x10, 0xf0, 0xf0, 0x00
.db 0x14, 0x14, 0x14, 0xfc, 0xfc, 0x00, 0x00, 0x00, 0x14, 0x14, 0xf7, 0xf7, 0x00, 0xff, 0xff, 0x00
.db 0x00, 0x00, 0xff, 0xff, 0x00, 0xff, 0xff, 0x00, 0x14, 0x14, 0xf4, 0xf4, 0x04, 0xfc, 0xfc, 0x00
.db 0x14, 0x14, 0x17, 0x17, 0x10, 0x1f, 0x1f, 0x00, 0x10, 0x10, 0x1f, 0x1f, 0x10, 0x1f, 0x1f, 0x00
.db 0x14, 0x14, 0x14, 0x1f, 0x1f, 0x00, 0x00, 0x00, 0x10, 0x10, 0x10, 0xf0, 0xf0, 0x00, 0x00, 0x00
.db 0x00, 0x00, 0x00, 0x1f, 0x1f, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x1f, 0x1f, 0x10, 0x10, 0x10
.db 0x10, 0x10, 0x10, 0xf0, 0xf0, 0x10, 0x10, 0x10, 0x00, 0x00, 0x00, 0xff, 0xff, 0x10, 0x10, 0x10
.db 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0xff, 0xff, 0x10, 0x10, 0x10
.db 0x00, 0x00, 0x00, 0xff, 0xff, 0x14, 0x14, 0x14, 0x00, 0x00, 0xff, 0xff, 0x00, 0xff, 0xff, 0x10
.db 0x00, 0x00, 0x1f, 0x1f, 0x10, 0x17, 0x17, 0x14, 0x00, 0x00, 0xfc, 0xfc, 0x04, 0xf4, 0xf4, 0x14
.db 0x14, 0x14, 0x17, 0x17, 0x10, 0x17, 0x17, 0x14, 0x14, 0x14, 0xf4, 0xf4, 0x04, 0xf4, 0xf4, 0x14
.db 0x00, 0x00, 0xff, 0xff, 0x00, 0xf7, 0xf7, 0x14, 0x14, 0x14, 0x14, 0x14, 0x14, 0x14, 0x14, 0x14
.db 0x14, 0x14, 0xf7, 0xf7, 0x00, 0xf7, 0xf7, 0x14, 0x14, 0x14, 0x14, 0x17, 0x17, 0x14, 0x14, 0x14
.db 0x10, 0x10, 0x1f, 0x1f, 0x10, 0x1f, 0x1f, 0x10, 0x14, 0x14, 0x14, 0xf4, 0xf4, 0x14, 0x14, 0x14
.db 0x10, 0x10, 0xf0, 0xf0, 0x10, 0xf0, 0xf0, 0x10, 0x00, 0x00, 0x1f, 0x1f, 0x10, 0x1f, 0x1f, 0x10
.db 0x00, 0x00, 0x00, 0x1f, 0x1f, 0x14, 0x14, 0x14, 0x00, 0x00, 0x00, 0xfc, 0xfc, 0x14, 0x14, 0x14
.db 0x00, 0x00, 0xf0, 0xf0, 0x10, 0xf0, 0xf0, 0x10, 0x10, 0x10, 0xff, 0xff, 0x10, 0xff, 0xff, 0x10
.db 0x14, 0x14, 0x14, 0xff, 0xff, 0x14, 0x14, 0x14, 0x10, 0x10, 0x10, 0x1f, 0x1f, 0x00, 0x00, 0x00
.db 0x00, 0x00, 0x00, 0xf0, 0xf0, 0x10, 0x10, 0x10, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff
.db 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00
.db 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f
.db 0x38, 0x7c, 0x44, 0x6c, 0x38, 0x6c, 0x44, 0x00, 0xf8, 0xfc, 0x54, 0x54, 0x7c, 0x28, 0x00, 0x00
.db 0x7f, 0x7f, 0x01, 0x01, 0x03, 0x03, 0x00, 0x00, 0x06, 0x7f, 0x7f, 0x03, 0x3f, 0x7f, 0x03, 0x00
.db 0x63, 0x77, 0x5d, 0x49, 0x63, 0x63, 0x00, 0x00, 0x38, 0x7c, 0x44, 0x7c, 0x3c, 0x04, 0x04, 0x00
.db 0x80, 0xfc, 0x7c, 0x20, 0x20, 0x3c, 0x1c, 0x00, 0x08, 0x0c, 0x04, 0x7c, 0x78, 0x0c, 0x04, 0x00
.db 0x49, 0x5d, 0x77, 0x77, 0x5d, 0x49, 0x00, 0x00, 0x1c, 0x3e, 0x6b, 0x49, 0x6b, 0x3e, 0x1c, 0x00
.db 0x4c, 0x7e, 0x33, 0x01, 0x33, 0x7e, 0x4c, 0x00, 0x30, 0x7a, 0x4f, 0x4f, 0x79, 0x31, 0x00, 0x00
.db 0x38, 0x6c, 0x6c, 0x38, 0x6c, 0x6c, 0x38, 0x00, 0xb8, 0xcc, 0x62, 0x5a, 0x46, 0x33, 0x1d, 0x00
.db 0x36, 0x6b, 0x49, 0x49, 0x63, 0x22, 0x00, 0x00, 0x7e, 0x7f, 0x01, 0x01, 0x7f, 0x7e, 0x00, 0x00
.db 0x2a, 0x2a, 0x2a, 0x2a, 0x2a, 0x2a, 0x00, 0x00, 0x44, 0x44, 0x5f, 0x5f, 0x44, 0x44, 0x00, 0x00
.db 0x40, 0x51, 0x5b, 0x4e, 0x44, 0x40, 0x00, 0x00, 0x40, 0x44, 0x4e, 0x5b, 0x51, 0x40, 0x00, 0x00
.db 0x00, 0x00, 0x00, 0xfe, 0xff, 0x01, 0x07, 0x06, 0x60, 0xe0, 0x80, 0xff, 0x7f, 0x00, 0x00, 0x00
.db 0x08, 0x08, 0x6b, 0x6b, 0x08, 0x08, 0x00, 0x00, 0x24, 0x36, 0x12, 0x36, 0x24, 0x36, 0x12, 0x00
.db 0x06, 0x0f, 0x09, 0x0f, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1c, 0x1c, 0x1c, 0x00, 0x00, 0x00
.db 0x00, 0x18, 0x3c, 0x7e, 0x3c, 0x18, 0x00, 0x00, 0x10, 0x30, 0x70, 0xc0, 0xff, 0xff, 0x01, 0x01
.db 0x1f, 0x1f, 0x01, 0x1f, 0x1e, 0x00, 0x00, 0x00, 0x19, 0x1d, 0x17, 0x12, 0x00, 0x00, 0x00, 0x00
.db 0x00, 0x3c, 0x3c, 0x3c, 0x3c, 0x3c, 0x00, 0x00, 0x3e, 0x41, 0x5d, 0x55, 0x55, 0x41, 0x3e, 0x00


g_font8x8t:	; thin 8x8 font, IBM/ASCII # 00-FF
.db 0x7e, 0x81, 0xbd, 0x95, 0xad, 0x81, 0x7e, 0x00, 0x7e, 0x81, 0x95, 0xb1, 0xb1, 0x95, 0x81, 0x7e
.db 0x7e, 0xff, 0xeb, 0xcf, 0xcf, 0xeb, 0xff, 0x7e, 0x0e, 0x1f, 0x3f, 0x7e, 0x3f, 0x1f, 0x0e, 0x00
.db 0x08, 0x1c, 0x3e, 0x7f, 0x3e, 0x1c, 0x08, 0x00, 0x38, 0x38, 0x9f, 0xff, 0x9f, 0x38, 0x38, 0x00
.db 0x10, 0x38, 0xbc, 0xff, 0xbc, 0x38, 0x10, 0x00, 0x00, 0x00, 0x18, 0x3c, 0x3c, 0x18, 0x00, 0x00
.db 0xff, 0xff, 0xe7, 0xc3, 0xc3, 0xe7, 0xff, 0xff, 0x00, 0x3c, 0x66, 0x42, 0x42, 0x66, 0x3c, 0x00
.db 0xff, 0xc3, 0x99, 0xbd, 0xbd, 0x99, 0xc3, 0xff, 0x70, 0x88, 0x88, 0x89, 0x7d, 0x03, 0x0f, 0x00
.db 0x4e, 0x51, 0xf1, 0xf1, 0x51, 0x4e, 0x00, 0x00, 0xc0, 0xe0, 0x7f, 0x05, 0x05, 0x05, 0x07, 0x00
.db 0xc0, 0xe0, 0x7f, 0x05, 0x65, 0x75, 0x3f, 0x00, 0x5a, 0x5a, 0x3c, 0xe7, 0xe7, 0x3c, 0x5a, 0x5a
.db 0x7f, 0x3e, 0x3e, 0x1c, 0x1c, 0x08, 0x08, 0x00, 0x08, 0x08, 0x1c, 0x1c, 0x3e, 0x3e, 0x7f, 0x00
.db 0x00, 0x24, 0x66, 0xff, 0x66, 0x24, 0x00, 0x00, 0x00, 0x5f, 0x00, 0x00, 0x5f, 0x00, 0x00, 0x00
.db 0x06, 0x09, 0x09, 0x7f, 0x01, 0x01, 0x7f, 0x00, 0x4a, 0x9e, 0xa5, 0xa5, 0xf9, 0x52, 0x00, 0x00
.db 0x70, 0x70, 0x70, 0x70, 0x70, 0x70, 0x70, 0x00, 0x00, 0x94, 0xb6, 0xff, 0xb6, 0x94, 0x00, 0x00
.db 0x00, 0x0c, 0x06, 0x7f, 0x06, 0x0c, 0x00, 0x00, 0x00, 0x18, 0x30, 0x7f, 0x30, 0x18, 0x00, 0x00
.db 0x10, 0x10, 0x10, 0x54, 0x7c, 0x38, 0x10, 0x00, 0x10, 0x38, 0x7c, 0x54, 0x10, 0x10, 0x10, 0x00
.db 0x3c, 0x20, 0x20, 0x20, 0x20, 0x20, 0x00, 0x00, 0x10, 0x38, 0x7c, 0x10, 0x7c, 0x38, 0x10, 0x00
.db 0x30, 0x38, 0x3c, 0x3e, 0x3c, 0x38, 0x30, 0x00, 0x06, 0x0e, 0x1e, 0x3e, 0x1e, 0x0e, 0x06, 0x00
.db 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x5f, 0x02, 0x00, 0x00, 0x00, 0x00
.db 0x00, 0x07, 0x00, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x14, 0x7f, 0x14, 0x7f, 0x14, 0x00, 0x00
.db 0x04, 0x2e, 0x6b, 0x2a, 0x6b, 0x3a, 0x10, 0x00, 0x46, 0x26, 0x10, 0x08, 0x64, 0x62, 0x00, 0x00
.db 0x30, 0x4a, 0x45, 0x4d, 0x32, 0x50, 0x48, 0x00, 0x00, 0x04, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00
.db 0x00, 0x00, 0x00, 0x1c, 0x22, 0x41, 0x00, 0x00, 0x00, 0x41, 0x22, 0x1c, 0x00, 0x00, 0x00, 0x00
.db 0x00, 0x24, 0x18, 0x3c, 0x18, 0x24, 0x00, 0x00, 0x00, 0x08, 0x08, 0x3e, 0x08, 0x08, 0x00, 0x00
.db 0x00, 0x80, 0x60, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x08, 0x08, 0x08, 0x08, 0x00, 0x00
.db 0x00, 0x60, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x00, 0x00
.db 0x3e, 0x41, 0x49, 0x49, 0x41, 0x3e, 0x00, 0x00, 0x42, 0x43, 0x7f, 0x40, 0x40, 0x00, 0x00, 0x00
.db 0x62, 0x51, 0x51, 0x49, 0x49, 0x66, 0x00, 0x00, 0x22, 0x41, 0x49, 0x49, 0x49, 0x36, 0x00, 0x00
.db 0x10, 0x18, 0x14, 0x52, 0x7f, 0x50, 0x00, 0x00, 0x27, 0x45, 0x45, 0x45, 0x45, 0x39, 0x00, 0x00
.db 0x3c, 0x4a, 0x49, 0x49, 0x49, 0x30, 0x00, 0x00, 0x03, 0x01, 0x71, 0x09, 0x05, 0x03, 0x00, 0x00
.db 0x36, 0x49, 0x49, 0x49, 0x49, 0x36, 0x00, 0x00, 0x06, 0x49, 0x49, 0x49, 0x29, 0x1e, 0x00, 0x00
.db 0x00, 0x00, 0x66, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x66, 0x00, 0x00, 0x00, 0x00, 0x00
.db 0x00, 0x00, 0x08, 0x14, 0x22, 0x41, 0x00, 0x00, 0x24, 0x24, 0x24, 0x24, 0x24, 0x24, 0x00, 0x00
.db 0x00, 0x41, 0x22, 0x14, 0x08, 0x00, 0x00, 0x00, 0x06, 0x01, 0x01, 0x51, 0x09, 0x06, 0x00, 0x00
.db 0x3c, 0x42, 0x99, 0xa5, 0xa5, 0xbd, 0x82, 0x00, 0x7c, 0x12, 0x11, 0x11, 0x12, 0x7c, 0x00, 0x00
.db 0x41, 0x7f, 0x49, 0x49, 0x49, 0x36, 0x00, 0x00, 0x1c, 0x22, 0x41, 0x41, 0x41, 0x22, 0x00, 0x00
.db 0x41, 0x7f, 0x41, 0x41, 0x22, 0x1c, 0x00, 0x00, 0x41, 0x7f, 0x49, 0x5d, 0x41, 0x63, 0x00, 0x00
.db 0x41, 0x7f, 0x49, 0x1d, 0x01, 0x03, 0x00, 0x00, 0x1c, 0x22, 0x41, 0x41, 0x51, 0x32, 0x76, 0x00
.db 0x7f, 0x08, 0x08, 0x08, 0x08, 0x7f, 0x00, 0x00, 0x00, 0x00, 0x41, 0x7f, 0x41, 0x00, 0x00, 0x00
.db 0x30, 0x40, 0x40, 0x41, 0x3f, 0x01, 0x00, 0x00, 0x41, 0x7f, 0x08, 0x14, 0x22, 0x41, 0x40, 0x00
.db 0x41, 0x7f, 0x41, 0x40, 0x40, 0x60, 0x00, 0x00, 0x7f, 0x01, 0x02, 0x04, 0x02, 0x01, 0x7f, 0x00
.db 0x7f, 0x02, 0x04, 0x08, 0x10, 0x20, 0x7f, 0x00, 0x3e, 0x41, 0x41, 0x41, 0x41, 0x41, 0x3e, 0x00
.db 0x41, 0x7f, 0x49, 0x09, 0x09, 0x06, 0x00, 0x00, 0x1e, 0x21, 0x21, 0x31, 0x21, 0x5e, 0x40, 0x00
.db 0x41, 0x7f, 0x49, 0x19, 0x29, 0x46, 0x00, 0x00, 0x26, 0x49, 0x49, 0x49, 0x49, 0x32, 0x00, 0x00
.db 0x03, 0x01, 0x41, 0x7f, 0x41, 0x01, 0x03, 0x00, 0x3f, 0x40, 0x40, 0x40, 0x40, 0x3f, 0x00, 0x00
.db 0x0f, 0x10, 0x20, 0x40, 0x20, 0x10, 0x0f, 0x00, 0x7f, 0x40, 0x20, 0x18, 0x20, 0x40, 0x7f, 0x00
.db 0x41, 0x22, 0x14, 0x08, 0x14, 0x22, 0x41, 0x00, 0x01, 0x02, 0x44, 0x78, 0x44, 0x02, 0x01, 0x00
.db 0x43, 0x61, 0x51, 0x49, 0x45, 0x43, 0x61, 0x00, 0x00, 0x00, 0x7f, 0x41, 0x41, 0x41, 0x00, 0x00
.db 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x00, 0x00, 0x41, 0x41, 0x41, 0x7f, 0x00, 0x00, 0x00
.db 0x00, 0x04, 0x02, 0x01, 0x02, 0x04, 0x00, 0x00, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80
.db 0x00, 0x00, 0x00, 0x00, 0x03, 0x04, 0x00, 0x00, 0x20, 0x54, 0x54, 0x54, 0x78, 0x40, 0x00, 0x00
.db 0x01, 0x7f, 0x38, 0x48, 0x48, 0x48, 0x30, 0x00, 0x38, 0x44, 0x44, 0x44, 0x44, 0x28, 0x00, 0x00
.db 0x30, 0x48, 0x48, 0x48, 0x39, 0x7f, 0x40, 0x00, 0x38, 0x54, 0x54, 0x54, 0x54, 0x18, 0x00, 0x00
.db 0x00, 0x48, 0x7e, 0x49, 0x01, 0x02, 0x00, 0x00, 0x98, 0xa4, 0xa4, 0xa4, 0xa4, 0x78, 0x04, 0x00
.db 0x41, 0x7f, 0x08, 0x04, 0x04, 0x78, 0x00, 0x00, 0x00, 0x44, 0x7d, 0x40, 0x00, 0x00, 0x00, 0x00
.db 0x60, 0x80, 0x80, 0x84, 0x7d, 0x00, 0x00, 0x00, 0x01, 0x7f, 0x10, 0x28, 0x44, 0x40, 0x00, 0x00
.db 0x00, 0x41, 0x7f, 0x40, 0x00, 0x00, 0x00, 0x00, 0x7c, 0x04, 0x04, 0x78, 0x04, 0x04, 0x78, 0x00
.db 0x7c, 0x08, 0x04, 0x04, 0x04, 0x78, 0x00, 0x00, 0x38, 0x44, 0x44, 0x44, 0x44, 0x38, 0x00, 0x00
.db 0x84, 0xfc, 0x98, 0x24, 0x24, 0x18, 0x00, 0x00, 0x18, 0x24, 0x24, 0x98, 0xfc, 0x84, 0x00, 0x00
.db 0x44, 0x7c, 0x48, 0x04, 0x04, 0x18, 0x00, 0x00, 0x48, 0x54, 0x54, 0x54, 0x24, 0x00, 0x00, 0x00
.db 0x04, 0x04, 0x3f, 0x44, 0x44, 0x20, 0x00, 0x00, 0x3c, 0x40, 0x40, 0x40, 0x20, 0x7c, 0x00, 0x00
.db 0x1c, 0x30, 0x60, 0x30, 0x1c, 0x00, 0x00, 0x00, 0x3c, 0x40, 0x40, 0x38, 0x40, 0x40, 0x3c, 0x00
.db 0x44, 0x28, 0x10, 0x28, 0x44, 0x00, 0x00, 0x00, 0x9c, 0xa0, 0xa0, 0xa0, 0x7c, 0x00, 0x00, 0x00
.db 0x44, 0x64, 0x54, 0x4c, 0x44, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x36, 0x41, 0x41, 0x00, 0x00
.db 0x00, 0x00, 0x77, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x41, 0x41, 0x36, 0x08, 0x00, 0x00, 0x00
.db 0x08, 0x04, 0x04, 0x08, 0x08, 0x04, 0x00, 0x00, 0x7c, 0x44, 0x44, 0x44, 0x44, 0x44, 0x7c, 0x00
.db 0x0e, 0x91, 0x91, 0xb1, 0xb1, 0x4a, 0x00, 0x00, 0x3a, 0x40, 0x40, 0x40, 0x7a, 0x40, 0x00, 0x00
.db 0x38, 0x54, 0x54, 0x55, 0x55, 0x18, 0x00, 0x00, 0x22, 0x55, 0x55, 0x55, 0x79, 0x42, 0x00, 0x00
.db 0x21, 0x54, 0x54, 0x54, 0x78, 0x41, 0x00, 0x00, 0x20, 0x55, 0x55, 0x54, 0x78, 0x40, 0x00, 0x00
.db 0x20, 0x54, 0x55, 0x54, 0x78, 0x40, 0x00, 0x00, 0x18, 0x24, 0xa4, 0xa4, 0xe4, 0x40, 0x00, 0x00
.db 0x3a, 0x55, 0x55, 0x55, 0x55, 0x1a, 0x00, 0x00, 0x39, 0x54, 0x54, 0x54, 0x54, 0x19, 0x00, 0x00
.db 0x38, 0x55, 0x55, 0x54, 0x54, 0x18, 0x00, 0x00, 0x00, 0x01, 0x44, 0x7c, 0x41, 0x00, 0x00, 0x00
.db 0x01, 0x45, 0x7d, 0x41, 0x01, 0x02, 0x00, 0x00, 0x00, 0x01, 0x45, 0x7c, 0x40, 0x00, 0x00, 0x00
.db 0x79, 0x14, 0x12, 0x12, 0x14, 0x79, 0x00, 0x00, 0x78, 0x14, 0x15, 0x15, 0x14, 0x78, 0x00, 0x00
.db 0x44, 0x7c, 0x54, 0x55, 0x45, 0x00, 0x00, 0x00, 0x20, 0x54, 0x54, 0x58, 0x38, 0x54, 0x54, 0x00
.db 0x7c, 0x0a, 0x09, 0x09, 0x7f, 0x49, 0x49, 0x00, 0x30, 0x4a, 0x49, 0x49, 0x4a, 0x30, 0x00, 0x00
.db 0x32, 0x48, 0x48, 0x48, 0x48, 0x32, 0x00, 0x00, 0x30, 0x49, 0x4a, 0x48, 0x48, 0x30, 0x00, 0x00
.db 0x38, 0x42, 0x41, 0x41, 0x42, 0x38, 0x00, 0x00, 0x38, 0x41, 0x42, 0x40, 0x40, 0x38, 0x00, 0x00
.db 0x1a, 0xa0, 0xa0, 0xa0, 0xa0, 0x7a, 0x00, 0x00, 0x19, 0x24, 0x42, 0x42, 0x24, 0x19, 0x00, 0x00
.db 0x3d, 0x40, 0x40, 0x40, 0x40, 0x3d, 0x00, 0x00, 0x40, 0x38, 0x64, 0x54, 0x54, 0x4c, 0x28, 0x04
.db 0x68, 0x5e, 0x49, 0x41, 0x42, 0x20, 0x00, 0x00, 0x01, 0x12, 0x54, 0x78, 0x54, 0x12, 0x01, 0x00
.db 0x81, 0xff, 0x85, 0x05, 0x17, 0xfa, 0x90, 0x00, 0x40, 0x88, 0x88, 0x7f, 0x09, 0x09, 0x02, 0x00
.db 0x20, 0x54, 0x54, 0x55, 0x79, 0x40, 0x00, 0x00, 0x00, 0x00, 0x44, 0x7d, 0x41, 0x00, 0x00, 0x00
.db 0x30, 0x48, 0x48, 0x4a, 0x49, 0x30, 0x00, 0x00, 0x38, 0x40, 0x40, 0x44, 0x42, 0x38, 0x00, 0x00
.db 0x7a, 0x09, 0x09, 0x0a, 0x0a, 0x71, 0x00, 0x00, 0x7a, 0x09, 0x11, 0x22, 0x42, 0x79, 0x00, 0x00
.db 0x26, 0x29, 0x29, 0x29, 0x2f, 0x28, 0x00, 0x00, 0x26, 0x29, 0x29, 0x29, 0x26, 0x00, 0x00, 0x00
.db 0x30, 0x48, 0x45, 0x40, 0x40, 0x20, 0x00, 0x00, 0x38, 0x08, 0x08, 0x08, 0x08, 0x08, 0x00, 0x00
.db 0x08, 0x08, 0x08, 0x08, 0x08, 0x38, 0x00, 0x00, 0x42, 0x2f, 0x10, 0x08, 0xcc, 0xaa, 0xb9, 0x00
.db 0x4a, 0x2f, 0x18, 0x28, 0x34, 0x2a, 0xfd, 0x20, 0x00, 0x20, 0x7a, 0x20, 0x00, 0x00, 0x00, 0x00
.db 0x00, 0x08, 0x14, 0x22, 0x08, 0x14, 0x22, 0x00, 0x00, 0x22, 0x14, 0x08, 0x22, 0x14, 0x08, 0x00
.db 0xaa, 0x00, 0x55, 0x00, 0xaa, 0x00, 0x55, 0x00, 0xaa, 0x55, 0xaa, 0x55, 0xaa, 0x55, 0xaa, 0x55
.db 0x22, 0x00, 0x55, 0x88, 0x22, 0x55, 0x00, 0x88, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0x00
.db 0x10, 0x10, 0x10, 0xff, 0x00, 0x00, 0x00, 0x00, 0x14, 0x14, 0x14, 0xff, 0x00, 0x00, 0x00, 0x00
.db 0x10, 0x10, 0x10, 0xff, 0x00, 0xff, 0x00, 0x00, 0x10, 0x10, 0x10, 0xf0, 0x10, 0xf0, 0x00, 0x00
.db 0x14, 0x14, 0x14, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x14, 0x14, 0x14, 0xf7, 0x00, 0xff, 0x00, 0x00
.db 0x00, 0x00, 0x00, 0xff, 0x00, 0xff, 0x00, 0x00, 0x14, 0x14, 0x14, 0xf4, 0x04, 0xfc, 0x00, 0x00
.db 0x14, 0x14, 0x14, 0x17, 0x10, 0x1f, 0x00, 0x00, 0x10, 0x10, 0x10, 0x1f, 0x10, 0x1f, 0x00, 0x00
.db 0x14, 0x14, 0x14, 0x1f, 0x00, 0x00, 0x00, 0x00, 0x10, 0x10, 0x10, 0xf0, 0x00, 0x00, 0x00, 0x00
.db 0x00, 0x00, 0x00, 0x1f, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x1f, 0x10, 0x10, 0x10, 0x10
.db 0x10, 0x10, 0x10, 0xf0, 0x10, 0x10, 0x10, 0x10, 0x00, 0x00, 0x00, 0xff, 0x10, 0x10, 0x10, 0x10
.db 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0xff, 0x10, 0x10, 0x10, 0x10
.db 0x00, 0x00, 0x00, 0xff, 0x14, 0x14, 0x14, 0x14, 0x00, 0x00, 0x00, 0xff, 0x00, 0xff, 0x10, 0x10
.db 0x00, 0x00, 0x00, 0x1f, 0x10, 0x17, 0x14, 0x14, 0x00, 0x00, 0x00, 0xfc, 0x04, 0xf4, 0x14, 0x14
.db 0x14, 0x14, 0x14, 0x17, 0x10, 0x17, 0x14, 0x14, 0x14, 0x14, 0x14, 0xf4, 0x04, 0xf4, 0x14, 0x14
.db 0x00, 0x00, 0x00, 0xff, 0x00, 0xf7, 0x14, 0x14, 0x14, 0x14, 0x14, 0x14, 0x14, 0x14, 0x14, 0x14
.db 0x14, 0x14, 0x14, 0xf7, 0x00, 0xf7, 0x14, 0x14, 0x14, 0x14, 0x14, 0x17, 0x14, 0x14, 0x14, 0x14
.db 0x10, 0x10, 0x10, 0x1f, 0x10, 0x1f, 0x10, 0x10, 0x14, 0x14, 0x14, 0xf4, 0x14, 0x14, 0x14, 0x14
.db 0x10, 0x10, 0x10, 0xf0, 0x10, 0xf0, 0x10, 0x10, 0x00, 0x00, 0x00, 0x1f, 0x10, 0x1f, 0x10, 0x10
.db 0x00, 0x00, 0x00, 0x1f, 0x14, 0x14, 0x14, 0x14, 0x00, 0x00, 0x00, 0xfc, 0x14, 0x14, 0x14, 0x14
.db 0x00, 0x00, 0x00, 0xf0, 0x10, 0xf0, 0x10, 0x10, 0x10, 0x10, 0x10, 0xff, 0x10, 0xff, 0x10, 0x10
.db 0x14, 0x14, 0x14, 0xff, 0x14, 0x14, 0x14, 0x14, 0x10, 0x10, 0x10, 0x1f, 0x00, 0x00, 0x00, 0x00
.db 0x00, 0x00, 0x00, 0xf0, 0x10, 0x10, 0x10, 0x10, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff
.db 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00
.db 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f
.db 0x38, 0x44, 0x28, 0x10, 0x28, 0x44, 0x00, 0x00, 0xf8, 0x54, 0x54, 0x54, 0x28, 0x00, 0x00, 0x00
.db 0x7f, 0x01, 0x01, 0x01, 0x01, 0x03, 0x00, 0x00, 0x02, 0x01, 0x7f, 0x01, 0x7f, 0x01, 0x01, 0x00
.db 0x63, 0x77, 0x5d, 0x49, 0x41, 0x63, 0x00, 0x00, 0x38, 0x44, 0x44, 0x44, 0x3c, 0x04, 0x04, 0x00
.db 0x80, 0x7c, 0x20, 0x20, 0x20, 0x1c, 0x00, 0x00, 0x08, 0x04, 0x04, 0x78, 0x08, 0x04, 0x00, 0x00
.db 0x49, 0x55, 0x77, 0x77, 0x55, 0x49, 0x00, 0x00, 0x1c, 0x2a, 0x49, 0x49, 0x49, 0x2a, 0x1c, 0x00
.db 0x4c, 0x72, 0x21, 0x01, 0x21, 0x72, 0x4c, 0x00, 0x30, 0x4a, 0x4d, 0x49, 0x31, 0x00, 0x00, 0x00
.db 0x38, 0x44, 0x6c, 0x38, 0x6c, 0x44, 0x38, 0x00, 0xb8, 0x44, 0x62, 0x5a, 0x46, 0x22, 0x1d, 0x00
.db 0x14, 0x3e, 0x49, 0x49, 0x49, 0x22, 0x00, 0x00, 0x7e, 0x01, 0x01, 0x01, 0x01, 0x7e, 0x00, 0x00
.db 0x00, 0x2a, 0x2a, 0x2a, 0x2a, 0x2a, 0x2a, 0x00, 0x00, 0x44, 0x44, 0x5f, 0x44, 0x44, 0x00, 0x00
.db 0x40, 0x40, 0x51, 0x4a, 0x44, 0x40, 0x00, 0x00, 0x40, 0x44, 0x4a, 0x51, 0x40, 0x40, 0x00, 0x00
.db 0x00, 0x00, 0x00, 0xfe, 0x01, 0x01, 0x06, 0x00, 0x60, 0x80, 0x80, 0x7f, 0x00, 0x00, 0x00, 0x00
.db 0x08, 0x08, 0x6b, 0x6b, 0x08, 0x08, 0x00, 0x00, 0x24, 0x12, 0x12, 0x24, 0x24, 0x12, 0x00, 0x00
.db 0x06, 0x09, 0x09, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x18, 0x00, 0x00, 0x00, 0x00
.db 0x00, 0x08, 0x1c, 0x3e, 0x1c, 0x08, 0x00, 0x00, 0x20, 0x20, 0x40, 0x80, 0xff, 0x01, 0x01, 0x01
.db 0x1f, 0x01, 0x01, 0x01, 0x1e, 0x00, 0x00, 0x00, 0x12, 0x19, 0x15, 0x12, 0x00, 0x00, 0x00, 0x00
.db 0x00, 0x3c, 0x3c, 0x3c, 0x3c, 0x3c, 0x00, 0x00, 0x7e, 0x81, 0x99, 0xa5, 0xa5, 0x81, 0x7e, 0x00


g_font5x6:	; small font 5x6, IBM/ASCII # 00-FF
.db 0x1e, 0x21, 0x35, 0x29, 0x1e, 0x1e, 0x29, 0x1f, 0x29, 0x1e
.db 0x1e, 0x29, 0x0f, 0x29, 0x1e, 0x0e, 0x1f, 0x3e, 0x1f, 0x0e
.db 0x0c, 0x1e, 0x3f, 0x1e, 0x0c, 0x0c, 0x0a, 0x3f, 0x0a, 0x0c
.db 0x0c, 0x0e, 0x3f, 0x0e, 0x0c, 0x00, 0x0c, 0x1e, 0x0c, 0x00
.db 0x3f, 0x33, 0x21, 0x33, 0x3f, 0x00, 0x1e, 0x12, 0x1e, 0x00
.db 0x3f, 0x21, 0x2d, 0x21, 0x3f, 0x18, 0x25, 0x25, 0x1b, 0x00
.db 0x16, 0x39, 0x39, 0x16, 0x00, 0x20, 0x3f, 0x05, 0x07, 0x00
.db 0x20, 0x3f, 0x05, 0x1f, 0x00, 0x15, 0x0e, 0x1b, 0x0e, 0x15
.db 0x3f, 0x1e, 0x0c, 0x00, 0x00, 0x00, 0x0c, 0x1e, 0x3f, 0x00
.db 0x12, 0x3f, 0x12, 0x00, 0x00, 0x2f, 0x00, 0x2f, 0x00, 0x00
.db 0x03, 0x3f, 0x01, 0x3f, 0x00, 0x24, 0x2a, 0x15, 0x09, 0x00
.db 0x18, 0x18, 0x18, 0x18, 0x00, 0x20, 0x2a, 0x3f, 0x2a, 0x20
.db 0x04, 0x02, 0x3f, 0x02, 0x04, 0x08, 0x10, 0x3f, 0x10, 0x08
.db 0x08, 0x08, 0x2a, 0x1c, 0x08, 0x08, 0x1c, 0x2a, 0x08, 0x08
.db 0x1e, 0x10, 0x10, 0x10, 0x00, 0x08, 0x1c, 0x08, 0x1c, 0x08
.db 0x10, 0x18, 0x1c, 0x18, 0x10, 0x02, 0x06, 0x0e, 0x06, 0x02
.db 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x17, 0x00, 0x00, 0x00
.db 0x00, 0x03, 0x00, 0x03, 0x00, 0x0a, 0x1f, 0x0a, 0x1f, 0x0a
.db 0x12, 0x35, 0x15, 0x09, 0x00, 0x12, 0x08, 0x04, 0x12, 0x00
.db 0x08, 0x14, 0x1a, 0x14, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00
.db 0x00, 0x00, 0x0e, 0x11, 0x00, 0x11, 0x0e, 0x00, 0x00, 0x00
.db 0x0a, 0x04, 0x0a, 0x00, 0x00, 0x04, 0x0e, 0x04, 0x00, 0x00
.db 0x20, 0x10, 0x00, 0x00, 0x00, 0x04, 0x04, 0x04, 0x04, 0x00
.db 0x00, 0x10, 0x00, 0x00, 0x00, 0x10, 0x08, 0x04, 0x02, 0x01
.db 0x0e, 0x15, 0x15, 0x0e, 0x00, 0x12, 0x1f, 0x10, 0x00, 0x00
.db 0x19, 0x15, 0x15, 0x16, 0x00, 0x0a, 0x11, 0x15, 0x0a, 0x00
.db 0x0c, 0x0a, 0x1f, 0x08, 0x00, 0x17, 0x15, 0x1d, 0x00, 0x00
.db 0x0e, 0x15, 0x15, 0x08, 0x00, 0x01, 0x19, 0x05, 0x03, 0x00
.db 0x0a, 0x15, 0x15, 0x0a, 0x00, 0x02, 0x15, 0x15, 0x0e, 0x00
.db 0x00, 0x14, 0x00, 0x00, 0x00, 0x20, 0x14, 0x00, 0x00, 0x00
.db 0x04, 0x0a, 0x00, 0x00, 0x00, 0x14, 0x14, 0x14, 0x14, 0x00
.db 0x00, 0x0a, 0x04, 0x00, 0x00, 0x01, 0x15, 0x05, 0x02, 0x00
.db 0x0e, 0x11, 0x17, 0x16, 0x00, 0x1e, 0x05, 0x05, 0x1e, 0x00
.db 0x1f, 0x15, 0x15, 0x0a, 0x00, 0x0e, 0x11, 0x11, 0x0a, 0x00
.db 0x1f, 0x11, 0x11, 0x0e, 0x00, 0x1f, 0x15, 0x15, 0x11, 0x00
.db 0x1f, 0x05, 0x05, 0x01, 0x00, 0x0e, 0x11, 0x15, 0x0c, 0x00
.db 0x1f, 0x04, 0x04, 0x1f, 0x00, 0x11, 0x1f, 0x11, 0x00, 0x00
.db 0x09, 0x11, 0x0f, 0x00, 0x00, 0x1f, 0x04, 0x0a, 0x11, 0x00
.db 0x1f, 0x10, 0x10, 0x10, 0x00, 0x1f, 0x06, 0x02, 0x1f, 0x00
.db 0x1f, 0x06, 0x08, 0x1f, 0x00, 0x0e, 0x11, 0x11, 0x0e, 0x00
.db 0x1f, 0x05, 0x05, 0x02, 0x00, 0x0e, 0x11, 0x19, 0x16, 0x00
.db 0x1f, 0x05, 0x0d, 0x12, 0x00, 0x12, 0x15, 0x15, 0x09, 0x00
.db 0x01, 0x1f, 0x01, 0x00, 0x00, 0x0f, 0x10, 0x10, 0x0f, 0x00
.db 0x0f, 0x10, 0x08, 0x07, 0x00, 0x1f, 0x0c, 0x08, 0x1f, 0x00
.db 0x1b, 0x04, 0x04, 0x1b, 0x00, 0x07, 0x18, 0x04, 0x03, 0x00
.db 0x19, 0x15, 0x15, 0x13, 0x00, 0x00, 0x1f, 0x11, 0x11, 0x00
.db 0x01, 0x02, 0x04, 0x08, 0x10, 0x11, 0x11, 0x1f, 0x00, 0x00
.db 0x00, 0x02, 0x01, 0x02, 0x00, 0x20, 0x20, 0x20, 0x20, 0x20
.db 0x00, 0x00, 0x03, 0x00, 0x00, 0x0c, 0x12, 0x12, 0x1e, 0x00
.db 0x1f, 0x12, 0x12, 0x0c, 0x00, 0x0c, 0x12, 0x12, 0x00, 0x00
.db 0x0c, 0x12, 0x12, 0x1f, 0x00, 0x0c, 0x1a, 0x1a, 0x14, 0x00
.db 0x04, 0x1e, 0x05, 0x00, 0x00, 0x04, 0x2a, 0x2a, 0x1c, 0x00
.db 0x1f, 0x02, 0x02, 0x1c, 0x00, 0x04, 0x1d, 0x10, 0x00, 0x00
.db 0x00, 0x24, 0x1d, 0x00, 0x00, 0x1f, 0x04, 0x04, 0x1a, 0x00
.db 0x01, 0x1f, 0x10, 0x00, 0x00, 0x1e, 0x04, 0x02, 0x1e, 0x00
.db 0x1e, 0x02, 0x02, 0x1c, 0x00, 0x0c, 0x12, 0x12, 0x0c, 0x00
.db 0x3e, 0x12, 0x12, 0x0c, 0x00, 0x0c, 0x12, 0x12, 0x3e, 0x00
.db 0x02, 0x1e, 0x04, 0x02, 0x00, 0x10, 0x16, 0x1a, 0x02, 0x00
.db 0x02, 0x1f, 0x12, 0x00, 0x00, 0x0e, 0x10, 0x10, 0x1e, 0x00
.db 0x0e, 0x10, 0x08, 0x06, 0x00, 0x1e, 0x08, 0x10, 0x1e, 0x00
.db 0x12, 0x0c, 0x0c, 0x12, 0x00, 0x06, 0x28, 0x28, 0x1e, 0x00
.db 0x12, 0x1a, 0x16, 0x12, 0x00, 0x00, 0x04, 0x1b, 0x11, 0x00
.db 0x00, 0x1b, 0x00, 0x00, 0x00, 0x11, 0x1b, 0x04, 0x00, 0x00
.db 0x08, 0x04, 0x08, 0x04, 0x00, 0x1e, 0x12, 0x12, 0x1e, 0x00
.db 0x0e, 0x31, 0x11, 0x00, 0x00, 0x0d, 0x10, 0x10, 0x1d, 0x00
.db 0x0c, 0x1a, 0x1b, 0x15, 0x00, 0x08, 0x15, 0x15, 0x1d, 0x00
.db 0x09, 0x14, 0x14, 0x1d, 0x00, 0x09, 0x15, 0x14, 0x1c, 0x00
.db 0x08, 0x14, 0x15, 0x1c, 0x00, 0x0c, 0x32, 0x12, 0x00, 0x00
.db 0x0c, 0x1b, 0x1b, 0x14, 0x00, 0x0c, 0x1b, 0x1a, 0x15, 0x00
.db 0x0d, 0x1b, 0x1a, 0x14, 0x00, 0x01, 0x1c, 0x11, 0x00, 0x00
.db 0x02, 0x19, 0x02, 0x00, 0x00, 0x01, 0x1d, 0x10, 0x00, 0x00
.db 0x1d, 0x0a, 0x0a, 0x1d, 0x00, 0x1c, 0x0b, 0x0b, 0x1c, 0x00
.db 0x1e, 0x15, 0x15, 0x11, 0x00, 0x0c, 0x12, 0x0e, 0x14, 0x00
.db 0x1f, 0x05, 0x1f, 0x15, 0x00, 0x08, 0x15, 0x15, 0x08, 0x00
.db 0x09, 0x14, 0x14, 0x09, 0x00, 0x09, 0x15, 0x14, 0x08, 0x00
.db 0x0c, 0x11, 0x11, 0x1c, 0x00, 0x0d, 0x11, 0x10, 0x1c, 0x00
.db 0x25, 0x28, 0x28, 0x1d, 0x00, 0x0d, 0x12, 0x12, 0x0d, 0x00
.db 0x0d, 0x10, 0x10, 0x0d, 0x00, 0x2c, 0x12, 0x12, 0x01, 0x00
.db 0x14, 0x0f, 0x15, 0x10, 0x00, 0x01, 0x0a, 0x1c, 0x0b, 0x00
.db 0x1f, 0x05, 0x0b, 0x1c, 0x00, 0x14, 0x1f, 0x05, 0x00, 0x00
.db 0x08, 0x14, 0x15, 0x1d, 0x00, 0x00, 0x1d, 0x11, 0x00, 0x00
.db 0x08, 0x14, 0x15, 0x09, 0x00, 0x0c, 0x10, 0x11, 0x1d, 0x00
.db 0x1c, 0x05, 0x05, 0x18, 0x00, 0x1e, 0x05, 0x09, 0x1e, 0x00
.db 0x2c, 0x32, 0x32, 0x3e, 0x00, 0x14, 0x1a, 0x1a, 0x14, 0x00
.db 0x08, 0x14, 0x15, 0x10, 0x00, 0x0e, 0x02, 0x02, 0x02, 0x00
.db 0x02, 0x02, 0x02, 0x0e, 0x00, 0x13, 0x08, 0x04, 0x1a, 0x15
.db 0x13, 0x08, 0x04, 0x0e, 0x19, 0x00, 0x1d, 0x00, 0x00, 0x00
.db 0x04, 0x0a, 0x00, 0x04, 0x0a, 0x0a, 0x04, 0x00, 0x0a, 0x04
.db 0x00, 0x15, 0x00, 0x15, 0x00, 0x15, 0x2a, 0x15, 0x2a, 0x00
.db 0x2a, 0x02, 0x28, 0x0a, 0x20, 0x00, 0x00, 0x3f, 0x00, 0x00
.db 0x08, 0x08, 0x3f, 0x00, 0x00, 0x0a, 0x0a, 0x3f, 0x00, 0x00
.db 0x08, 0x3f, 0x00, 0x3f, 0x00, 0x08, 0x38, 0x08, 0x38, 0x00
.db 0x0a, 0x0a, 0x3e, 0x00, 0x00, 0x0a, 0x3b, 0x00, 0x3f, 0x00
.db 0x00, 0x3f, 0x00, 0x3f, 0x00, 0x0a, 0x3a, 0x02, 0x3e, 0x00
.db 0x0a, 0x0b, 0x08, 0x0f, 0x00, 0x08, 0x0f, 0x08, 0x0f, 0x00
.db 0x0a, 0x0a, 0x0f, 0x00, 0x00, 0x08, 0x08, 0x38, 0x00, 0x00
.db 0x00, 0x00, 0x0f, 0x08, 0x08, 0x08, 0x08, 0x0f, 0x08, 0x08
.db 0x08, 0x08, 0x38, 0x08, 0x08, 0x00, 0x00, 0x3f, 0x08, 0x08
.db 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x3f, 0x08, 0x08
.db 0x00, 0x00, 0x3f, 0x0a, 0x0a, 0x00, 0x3f, 0x00, 0x3f, 0x08
.db 0x00, 0x0f, 0x08, 0x0b, 0x0a, 0x00, 0x3e, 0x02, 0x3a, 0x0a
.db 0x0a, 0x0b, 0x08, 0x0b, 0x0a, 0x0a, 0x3a, 0x02, 0x3a, 0x0a
.db 0x00, 0x3f, 0x00, 0x3b, 0x0a, 0x0a, 0x0a, 0x0a, 0x0a, 0x0a
.db 0x0a, 0x3b, 0x00, 0x3b, 0x0a, 0x0a, 0x0a, 0x0b, 0x0a, 0x0a
.db 0x08, 0x0f, 0x08, 0x0f, 0x08, 0x0a, 0x0a, 0x3a, 0x0a, 0x0a
.db 0x08, 0x38, 0x08, 0x38, 0x08, 0x00, 0x0f, 0x08, 0x0f, 0x08
.db 0x00, 0x00, 0x0f, 0x0a, 0x0a, 0x00, 0x00, 0x3e, 0x0a, 0x0a
.db 0x00, 0x38, 0x08, 0x38, 0x08, 0x08, 0x3f, 0x08, 0x3f, 0x08
.db 0x0a, 0x0a, 0x3f, 0x0a, 0x0a, 0x08, 0x08, 0x0f, 0x00, 0x00
.db 0x00, 0x00, 0x38, 0x08, 0x08, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f
.db 0x38, 0x38, 0x38, 0x38, 0x38, 0x3f, 0x3f, 0x3f, 0x00, 0x00
.db 0x00, 0x00, 0x3f, 0x3f, 0x3f, 0x07, 0x07, 0x07, 0x07, 0x07
.db 0x0c, 0x12, 0x0c, 0x12, 0x00, 0x3e, 0x15, 0x0a, 0x00, 0x00
.db 0x1f, 0x01, 0x01, 0x03, 0x00, 0x1f, 0x01, 0x1f, 0x01, 0x00
.db 0x1b, 0x15, 0x11, 0x11, 0x00, 0x0c, 0x12, 0x0e, 0x02, 0x00
.db 0x3e, 0x10, 0x0e, 0x10, 0x00, 0x02, 0x1e, 0x12, 0x02, 0x00
.db 0x15, 0x1b, 0x1b, 0x15, 0x00, 0x0e, 0x15, 0x15, 0x15, 0x0e
.db 0x16, 0x19, 0x01, 0x19, 0x16, 0x0a, 0x15, 0x15, 0x08, 0x00
.db 0x0c, 0x12, 0x0c, 0x12, 0x0c, 0x2c, 0x12, 0x12, 0x12, 0x0d
.db 0x0a, 0x15, 0x11, 0x0a, 0x00, 0x1e, 0x01, 0x01, 0x1e, 0x00
.db 0x2a, 0x2a, 0x2a, 0x2a, 0x00, 0x00, 0x12, 0x17, 0x12, 0x00
.db 0x15, 0x15, 0x12, 0x12, 0x00, 0x12, 0x12, 0x15, 0x15, 0x00
.db 0x00, 0x00, 0x3f, 0x01, 0x06, 0x60, 0x80, 0x7f, 0x00, 0x00
.db 0x00, 0x04, 0x15, 0x04, 0x00, 0x09, 0x12, 0x09, 0x12, 0x00
.db 0x02, 0x05, 0x02, 0x00, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00
.db 0x00, 0x0c, 0x0c, 0x00, 0x00, 0x08, 0x18, 0x3f, 0x01, 0x00
.db 0x07, 0x01, 0x06, 0x00, 0x00, 0x0d, 0x0b, 0x00, 0x00, 0x00
.db 0x1e, 0x1e, 0x1e, 0x1e, 0x00, 0x1e, 0x21, 0x2d, 0x2d, 0x1e

*** End of library --------------------------------------------------------
