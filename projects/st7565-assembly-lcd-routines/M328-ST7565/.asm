

/*
*******************************************************************************************
Use for whatever you want. I make no claims to suitability of purpose or functionality.
Your on your own with this. It you dont like how it does something then change it.
Supplied to the public domain, no restrictions.
StewieTee
*******************************************************************************************




Its rough and not optimised but it suits my needs. Feel free to clean it up
and make it a work of art :)

There is no actual cursor on the screen, it just means where the next char
gets written to.

assorted macros in the macros.inc file to ease use

This module enables a serial ST7565 128x64 graphic LCD to be used without
having to map the whole thing into ram. It writes the data (characters)
direct to the screen using character cell row/collum adressing.
Its for use in text-mode, not full graphic.
You get 8 rows of 21 chars. Limited double size display, numbers only
(see towards end of file)

To bit-map the complete display would use 1024 bytes of ram, this lets you do
neat graphics. Using the routines below gives you no 'real' graphics except
what you can do with box-drawing chars. Not as fancy but only takes 5-bytes
of ram. Many projects just need to show text so this is fine.

This example uses software SPI so you dont have to share the hardware 
SPI port, which in my case is running a RFM12 in interrupt routines.
The display is a low priority item so if the write process is constantly 
interrupted by the RFM12 stuff and whatever other interrupts are running,
it does not really matter.

It uses 5 bytes of ram and 5 port bits for the assorted control signals

A 'normal' 16x2 lcd display needs 7 signals (if in 4-bit mode). This only
uses 5 and you have a bigger display to play with.

You can only write to the screen, not read from it. Its a limitation
of the cheap serial (and very usefull) screens.
Available from:
http://www.adafruit.com/products/250

The displays run at 3.3V and I'm using an ATMEGA328P @ 8Mhz, also at 3.3V, + RFM12B
so interfacing is easy, no levelshifting required.

If you just need text/numbers, the font table can be reduced to loose the box-drawing
and other special chars.

*/

; pins used for control (bit-banged SPI)
; change to suit your needs
#define	LCD_CS		PORTC,0		; chip-select
#define	LCD_CS_DIR	DDRC,0		; chip-select pin direction

#define	LCD_RST		PORTC,1		; reset pin
#define	LCD_RST_DIR	DDRC,1		; reset pin direction

#define	LCD_A0		PORTC,2		; register/screen select
#define	LCD_A0_DIR	DDRC,2		; register/screen select pin direction

#define	LCD_SCLK	PORTC,3		; serial clock
#define	LCD_SCLK_DIR	DDRC,3		; serial clock pin direction

#define	LCD_SID		PORTD,7		; serial data (m-s-bit 1st)
#define	LCD_SID_DIR	DDRD,7		; serial data pin direction


; commands etc for screen setup and driving
#define CMD_DISPLAY_OFF   0xAE
#define CMD_DISPLAY_ON    0xAF

#define CMD_SET_DISP_START_LINE  0x40
#define CMD_SET_PAGE  0xB0

#define CMD_SET_COLUMN_UPPER  0x10
#define CMD_SET_COLUMN_LOWER  0x01

#define CMD_SET_ADC_NORMAL  0xA0
#define CMD_SET_ADC_REVERSE 0xA1

#define CMD_SET_DISP_NORMAL 0xA6
#define CMD_SET_DISP_REVERSE 0xA7

#define CMD_SET_ALLPTS_NORMAL 0xA4
#define CMD_SET_ALLPTS_ON  0xA5
#define CMD_SET_BIAS_9 0xA2 
#define CMD_SET_BIAS_7 0xA3

#define CMD_RMW  0xE0
#define CMD_RMW_CLEAR 0xEE
#define CMD_INTERNAL_RESET  0xE2
#define CMD_SET_COM_NORMAL  0xC0
#define CMD_SET_COM_REVERSE  0xC8
#define CMD_SET_POWER_CONTROL  0x28
#define CMD_SET_RESISTOR_RATIO  0x20
#define CMD_SET_VOLUME_FIRST  0x81
#define CMD_SET_VOLUME_SECOND  0
#define CMD_SET_STATIC_OFF  0xAC
#define CMD_SET_STATIC_ON  0xAD
#define CMD_SET_STATIC_REG  0x0
#define CMD_SET_BOOSTER_FIRST  0xF8
#define CMD_SET_BOOSTER_234  0
#define CMD_SET_BOOSTER_5  1
#define CMD_SET_BOOSTER_6  3
#define CMD_NOP  0xE3
#define CMD_TEST  0xF0


.dseg
row:		.byte	1	; LCD row pos for text cell positioning
coll:		.byte	1	; collum as above for text cell positioning

Lpage:		.byte	1	; LCD page. Display is 8 pages (rows of pixels, 8 bits high, vertical)
Lcoll:		.byte	1	; actual LCD collum address. Which byte of pixels (0...127) along the row
Lwidth:		.byte	1	; width of big LCD chars. Nums and dot/comma are different




.cseg

; call this on power-up to initialise the display
init_st7565:
	sbi	LCD_CS_DIR	; output
	sbi	LCD_CS		; select pin high (not selected)

	sbi	LCD_A0_DIR	; outputs
	sbi	LCD_A0		; set high

	sbi	LCD_RST_DIR	; output
	sbi	LCD_RST		; reset pin high

	sbi	LCD_SID_DIR	; output
	sbi	LCD_SID		; serial pin high

	sbi	LCD_SCLK_DIR	; output
	cbi	LCD_SCLK	; clk pin low

	cbi	LCD_CS		; LCD now selected/active
; control pins now in startup state, ready to do a hard reset on display
	cbi	LCD_RST		; pull low to reset
	msdelay	10		; wait 10ms
	sbi	LCD_RST		; and back high again
	sbi	LCD_CS		; and de-select
	msdelay	2		; 2ms

; now start the initialisation of internal registers
	ldi	temp1,CMD_SET_BIAS_7
	rcall	LCD_command

	ldi	temp1,CMD_SET_ADC_NORMAL
	rcall	LCD_command

	ldi	temp1,CMD_SET_COM_REVERSE
	rcall	LCD_command

	ldi	temp1,CMD_SET_DISP_START_LINE | 0x20
	rcall	LCD_command

	ldi	temp1,(CMD_SET_POWER_CONTROL | 0x4)
	rcall	LCD_command
	msdelay	50

	ldi	temp1,(CMD_SET_POWER_CONTROL | 0x6)
	rcall	LCD_command
	msdelay	50

	ldi	temp1,(CMD_SET_POWER_CONTROL | 0x7)
	rcall	LCD_command
	msdelay	50

	ldi	temp1,(CMD_SET_RESISTOR_RATIO | 0x6)
	rcall	LCD_command

; next two are sent as a pair, 1st one enters a mode, 2nd is the value
	ldi	temp1,CMD_SET_VOLUME_FIRST
	rcall	LCD_command
	ldi	temp1,30		; 0..63 for contrast
	rcall	LCD_command

	rcall	LCD_clearscreen		; clears and sets to page-0, display coll-0
					; (and row/coll=0)

	ldi	temp1,CMD_DISPLAY_ON	; display ON
	rcall	LCD_command

	ret


; **********************************************************************************
; this is the basic "write a char to the screen" routine.
; if you write past the end of the row, it wraps to the next one.
; If you write off the bottom of the screen, it wraps to the top. No Scroll.
; use the macro [lcd_set row,coll] to position the write location
; then the char in temp1 goes to row/coll on screen
; takes 195uSec to write a char or about 33ms per full-screen
; (1-char at 57.6k = 173uSec)
; The chacter positions are calculated each time from the current row/coll
; values so the chars are on a fixed grid on the screen.
; for exact horizontal pixel postioning, see 'lcd_set_coll' below.
; If you write off-grid then another write (grid based write) can corrupt an existing screen char
; if they overlap.
lcd_char:
	push	r0
	push	r1
	push	temp1
; filter-out and action cr/lf 
	jne	temp1,0x0d,tlf
	rcall	lcd_cr
	rjmp	chardone
tlf:	jne	temp1,0x0a,rest
	rcall	lcd_lf
	rjmp	chardone

; 1st set the collum address. Each char is 6-pixels wide
rest:	lds	temp1,coll	; get the char position (0..20)
	ldi	temp2,6		; 6 wide
	mul	temp1,temp2	; get pixel collum pos
	sts	Lcoll,r0	; save in ram
	rcall	lcd_set_coll	; set it
; now set the page, 0..7
	lds	temp1,row
	sts	Lpage,temp1
	rcall	lcd_set_page
; restore the char to show
	pop	temp1		; char to display
	push	temp1		; and resave
	rcall	put_LCDchar	; send to screen
; now update cursor pos
	lds	temp1,coll
	inc	temp1
	cpi	temp1,21
	brlo	nocolinc
	clr	temp1
nocolinc:
	sts	coll,temp1	; save back to ram
	tst	temp1		; see if zero so as to adjust row
	brne	chardone
; if falls thru then adjust row value
	rcall	lcd_lf
chardone:
	pop	temp1
	pop	r1
	pop	r0
	ret

; process a CR
lcd_cr:	push	temp1
	clr	temp1
	sts	coll,temp1
	pop	temp1
	ret

; process a Line-feed
lcd_lf:	push	temp1
	lds	temp1,row
	inc	temp1
	cpi	temp1,8
	brlo	norowinc
	clr	temp1
norowinc:
	sts	row,temp1
	pop	temp1
	ret






; char in temp1, count in temp2. Works in the curent PAGE (row)
; this does not use character-cell row/coll addressing as in lcd_putchar.
; instead it uses the current page and horizontal pixel position
put_many:
	rcall	put_LCDchar
	dec	temp2
	brne	put_many
	ret





; to send comands to the display.
LCD_command:
	cbi	LCD_A0		; register select
	rjmp	reg
LCD_send:
	sbrc	bitflags,invert	; invert
	com	temp1

reg:	push	temp1
	push	loopcounter
	ldi	loopcounter,8
	cbi	LCD_CS
LClp:	cbi	LCD_SCLK
	rol	temp1
	brcs	LC1
	cbi	LCD_SID		; clear op bit
	rjmp	doclk
LC1:	sbi	LCD_SID		; set o/p bit
doclk:	nop
	sbi	LCD_SCLK
	dec	loopcounter
	brne	LClp
	sbi	LCD_CS		; deselect display
	pop	loopcounter
	sbi	LCD_A0
	pop	temp1
	ret



; takes about 120uSec at 8Mhz. 1-char at 57.6 = 173uSec
; if the byte represents an alph-num char then a vertical 8-pixel strip is inserted
; between each char. If a graphic then no 'blank' strip so the graphics
; join together
; This writes to the current page.
put_LCDchar:
	push	zl
	push	zh
	push	temp1
	push	temp2
	push	r0
	push	r1

	cpi	temp1,0xb0	; decide if to send a vertical 8-pixel strip
	brlo	doextra		; between chars
	cpi	temp1,0xe0
	brsh	doextra
	rjmp	posit

doextra:
	push	temp1
	clr	temp1		; inserts a leading pixel space between chars
	rcall	LCD_send	; ie a vertical row. 8 pixels between alpha-num chars
	pop	temp1

; work out the start of a char bit pattern
posit:	ldZ	rom,font_base
	ldi	temp2,5		; font pattern is 5-bytes per char
	mul	temp1,temp2
	add	zl,r0
	adc	zh,r1		; Z points at char pattern

	ldi	temp2,5		; 5 bytes per char

dochar:	lpm	temp1,z+	; send the 5 bytes (a complete character pattern) 
	rcall	LCD_send
	dec	temp2
	brne	dochar

	pop	r1
	pop	r0
	pop	temp2
	pop	temp1
	pop	zh
	pop	zl
	ret


; wipes the screen and sets start position to 0,0 (top left) for chars
LCD_clearscreen:
	ldi	temp1,CMD_SET_PAGE		; sets to page-0
	rcall	LCD_command
	ldi	temp1,0				; fill value (all bits off)
	rcall	fillpage			; write it to a complete page (text line)
	ldi	temp1,(CMD_SET_PAGE +1)		; page 1
	rcall	LCD_command
	ldi	temp1,0
	rcall	fillpage
	ldi	temp1,(CMD_SET_PAGE + 2)	; page 2
	rcall	LCD_command
	ldi	temp1,0
	rcall	fillpage
	ldi	temp1,(CMD_SET_PAGE + 3)
	rcall	LCD_command
	ldi	temp1,0
	rcall	fillpage
	ldi	temp1,(CMD_SET_PAGE + 4)
	rcall	LCD_command
	ldi	temp1,0
	rcall	fillpage
	ldi	temp1,(CMD_SET_PAGE + 5)
	rcall	LCD_command
	ldi	temp1,0
	rcall	fillpage
	ldi	temp1,(CMD_SET_PAGE + 6)
	rcall	LCD_command
	ldi	temp1,0
	rcall	fillpage
	ldi	temp1,(CMD_SET_PAGE + 7)
	rcall	LCD_command
	ldi	temp1,0
	rcall	fillpage
; set to top-LHS of screen
	lcd_coll 0
	lcd_page 0
	clr	temp1
	sts	row,temp1
	sts	coll,temp1
	ret


; fills a page (row) with the byte value (vertical 8-pixel strip) in temp1
fillpage:
	push	temp1
	ldi	temp1,CMD_SET_COLUMN_UPPER
	rcall	LCD_command
	ldi	temp1,CMD_SET_COLUMN_LOWER
	rcall	LCD_command
	pop	temp1

	ldi	loopcounter,129
lloop:	rcall	LCD_send
	dec	loopcounter
	brne	lloop
	ret




; sets the collum pixel location (0...127)
; addr in Lcol
; use the macro lcd_col. ie 'lcd_col 83' to set the 
; horizontal position to 83 pixels along the current page (row)
; only useful for exact horizontal character position writes.
; 'lcd_putchar' fits chars onto an exact regular grid based on
; the row/col co-ordinate which is character based.
; by using lcd_set_col, this specifies an exact horizontal position
; to write a char using 'put_LCDchar' which writes a character at the
; current horizontal location (0...127) on the current page (row) 0...7
lcd_set_coll:
	push	temp1
	lds	temp1,Lcoll
	push	temp1
	andi	temp1,0b11110000
	swap	temp1
	ori	temp1,CMD_SET_COLUMN_UPPER
	rcall	LCD_command
	pop	temp1
	andi	temp1,0b00001111
	ori	temp1,CMD_SET_COLUMN_LOWER
	rcall	LCD_command
	pop	temp1
	ret

; sets the page (row). A page is 128 vertical sections, 8-pixels high each
; use the macro lcd_page
lcd_set_page:
	lds	temp1,Lpage
	ori	temp1,CMD_SET_PAGE
	rcall	LCD_command
	ret




; ************************************************************************
; The large numbers section below was a bit of a messy afterthought
;
; ************************************************************************
;
; double size numbers and space/comma/plus/minus/decimal-point
;
; to use this, use the macros...
; 	lcd_coll 0
;	lcd_page 0
; to set the display address. each number is double width & height
; so the above sets to top left of display
; the next double hieght/width row is
;	lcd_coll 0
;	lcd_page 2
; 
; This is only for large numbers, not text. The text is only small chars.
;
; Use actual numbers, not ascii. 1=1, not '1'
; ***********************************************************************
put_num16:
	push	zl
	push	zh
	push	temp1
	push	temp2
	push	r0
	push	r1

	ldZ	rom,num16
	ldi	temp2,28
	mul	temp1,temp2
	add	zl,r0
	adc	zh,r1		; z points at char pattern
	ldi	temp2,14

dochar1:
	lpm	temp1,z+
	rcall	LCD_send
	dec	temp2
	brne	dochar1

	lds	temp1,Lcoll	; is unchanged from writing 1st 2 blocks
	rcall	lcd_set_coll	; 2nd batch of 14 bytes starts at same coll

	lds	temp1,Lpage
	inc	temp1		; next row
	sts	Lpage,temp1	; update in ram
	rcall	lcd_set_page	; and set it

	ldi	temp2,14
dochar3:
	lpm	temp1,z+
	rcall	LCD_send
	dec	temp2
	brne	dochar3

	lds	temp1,Lcoll
	ldi	temp2,14
	add	temp1,temp2
	sts	Lcoll,temp1
	rcall	lcd_set_coll	; 

	lds	temp1,Lpage	; adjust to the page above for start of next char
	dec	temp1
	sts	Lpage,temp1
	rcall	lcd_set_page

	pop	r1
	pop	r0
	pop	temp2
	pop	temp1
	pop	zh
	pop	zl
	ret






; call the relevant routine to insert the big char
lcd_comma:
	push	zl
	push	zh
	push	temp1
	push	temp2
	ldi	temp2,8		; width
	sts	Lwidth,temp2
	ldZ	rom,comma
	rjmp	dpnk

lcd_space:
	push	zl
	push	zh
	push	temp1
	push	temp2
	ldi	temp2,14		; width
	sts	Lwidth,temp2
	ldZ	rom,space
	rjmp	dpnk

lcd_point:
	push	zl
	push	zh
	push	temp1
	push	temp2
	ldi	temp2,8		; width
	sts	Lwidth,temp2
	ldZ	rom,point
dpnk:	rcall	punk		; insert punctuation
	
	pop	temp2
	pop	temp1
	pop	zh
	pop	zl
	ret	








punk:
	ldi	temp2,8
dochar4:
	lpm	temp1,z+
	rcall	LCD_send
	dec	temp2
	brne	dochar4

	lds	temp1,Lcoll
	rcall	lcd_set_coll	; 2nd batch of 14 bytes
	lds	temp1,Lpage
	push	temp1
	inc	temp1		; next row
	andi	temp1,0b00000111
	sts	Lpage,temp1
	rcall	lcd_set_page

	ldi	temp2,8
dochar5:
	lpm	temp1,z+
	rcall	LCD_send
	dec	temp2
	brne	dochar5

	lds	temp1,Lcoll
	subi	temp1,-8
	sts	Lcoll,temp1
	rcall	lcd_set_coll

	pop	temp1
	sts	Lpage,temp1
	rcall	lcd_set_page
	ret







; Table is referenced by the number 0...9 in temp1 for 'put_num16' routine.
; 10 gives a large space, 11 is a plus sign, 12 a minus

num16:
;0
	.db	0x00, 0x00, 0xE0, 0xF8, 0x1C, 0x06, 0x06, 0x06,\
		0x06, 0x1C, 0xF8, 0xE0, 0x00, 0x00, 0x00, 0x00,\
		0x0F, 0x3F, 0x70, 0xC0, 0xC0, 0xC0, 0xC0, 0x70,\
		0x3F, 0x0F, 0x00, 0x00                         

;1
	.db	0x00, 0x00, 0x18, 0x18, 0x1C, 0x0C, 0xFC, 0xFE, \
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
		0xC0, 0xC0, 0xC0, 0xC0, 0xFF, 0xFF, 0xC0, 0xC0, \
		0xC0, 0xC0, 0x00, 0x00                         

;2
	.db	0x00, 0x38, 0x3C, 0x0C, 0x06, 0x06, 0x06, 0x06, \
		0x06, 0x8C, 0xFC, 0x70, 0x00, 0x00, 0x00, 0xC0, \
		0xE0, 0xF0, 0xD8, 0xCC, 0xC6, 0xC6, 0xC3, 0xC1, \
		0xC0, 0xC0, 0x00, 0x00                         

;3
	.db	0x00, 0x00, 0x00, 0x0C, 0x0C, 0x86, 0x86, 0x86, \
		0xCE, 0x7C, 0x38, 0x00, 0x00, 0x00, 0x00, 0x00, \
		0x60, 0xE0, 0xC0, 0xC1, 0xC1, 0xC1, 0xC3, 0x63, \
		0x7E, 0x3C, 0x00, 0x00                         

;4
	.db	0x00, 0x00, 0x00, 0x80, 0xE0, 0x70, 0x1C, 0x0E, \
		0xFE, 0xFE, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1C, \
		0x1E, 0x1B, 0x19, 0xD8, 0xD8, 0xD8, 0xFF, 0xFF, \
		0xD8, 0xD8, 0x00, 0x00                         

;5
	.db	0x00, 0x00, 0xFE, 0xFE, 0x86, 0xC6, 0xC6, 0xC6, \
		0xC6, 0x86, 0x86, 0x00, 0x00, 0x00, 0x00, 0x60, \
		0x61, 0xC1, 0xC1, 0xC0, 0xC0, 0xC0, 0xC0, 0x61, \
		0x7F, 0x1E, 0x00, 0x00                         

;6
	.db	0x00, 0x00, 0xC0, 0xF0, 0x38, 0x9C, 0x8C, 0x86, \
		0x86, 0x06, 0x06, 0x06, 0x00, 0x00, 0x00, 0x00, \
		0x1F, 0x7F, 0x63, 0xC1, 0xC1, 0xC1, 0xC1, 0xE3, \
		0x7F, 0x3C, 0x00, 0x00                         

;7
	.db	0x00, 0x00, 0x1E, 0x1E, 0x06, 0x06, 0x06, 0x06, \
		0x86, 0xF6, 0xFE, 0x1E, 0x00, 0x00, 0x00, 0x00, \
		0x00, 0x00, 0x00, 0x00, 0xE0, 0xFC, 0x3F, 0x07, \
		0x00, 0x00, 0x00, 0x00                         

;8                                                               
	.db	0x00, 0x00, 0x38, 0x7C, 0xCE, 0x86, 0x86, 0x86, \
		0x86, 0xCE, 0x7C, 0x38, 0x00, 0x00, 0x00, 0x00, \
		0x3C, 0x7E, 0xE3, 0xC1, 0xC1, 0xC1, 0xC1, 0xE3, \
		0x7E, 0x3C, 0x00, 0x00                          

;9                                                               
	.db	0x00, 0x00, 0x78, 0xFC, 0x8E, 0x06, 0x06, 0x06, \
		0x06, 0x8C, 0xFC, 0xF0, 0x00, 0x00, 0x00, 0x00, \
		0xC0, 0xC1, 0xC1, 0xC3, 0xC3, 0x63, 0x73, 0x39, \
		0x1F, 0x07, 0x00, 0x00
;10
space:	.db	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
		0x00, 0x00, 0x00, 0x00                         

;11
plus:	.db	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF0, 0xF0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 
	.db	0x00, 0x06, 0x06, 0x06, 0x06, 0x06, 0xFF, 0xFF, 0x06, 0x06,0x06, 0x06, 0x06, 0x00

;12
minus:	.db	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
	.db	0x00, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x00


;, 
comma:	.db	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,\
		0x00, 0x80, 0xE0, 0x70, 0x70, 0x20, 0x00, 0x00
		
		
;. point
point:	.db 	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,\
		0x00, 0x00, 0x20, 0x70, 0x70, 0x20, 0x00, 0x00





; 256 patterns, 5 bytes wide per char
font_base:
	.db	0x00, 0x00, 0x00, 0x00, 0x00,\
		0x3E, 0x5B, 0x4F, 0x5B, 0x3E,\
		0x3E, 0x6B, 0x4F, 0x6B, 0x3E,\
		0x1C, 0x3E, 0x7C, 0x3E, 0x1C,\
		0x18, 0x3C, 0x7E, 0x3C, 0x18,\
		0x1C, 0x57, 0x7D, 0x57, 0x1C,\
		0x1C, 0x5E, 0x7F, 0x5E, 0x1C,\
		0x00, 0x18, 0x3C, 0x18, 0x00,\
		0xFF, 0xE7, 0xC3, 0xE7, 0xFF,\
		0x00, 0x18, 0x24, 0x18, 0x00,\
		0xFF, 0xE7, 0xDB, 0xE7, 0xFF,\
		0x30, 0x48, 0x3A, 0x06, 0x0E,\
		0x26, 0x29, 0x79, 0x29, 0x26,\
		0x40, 0x7F, 0x05, 0x05, 0x07,\
		0x40, 0x7F, 0x05, 0x25, 0x3F,\
		0x5A, 0x3C, 0xE7, 0x3C, 0x5A,\
		0x7F, 0x3E, 0x1C, 0x1C, 0x08,\
		0x08, 0x1C, 0x1C, 0x3E, 0x7F,\
		0x14, 0x22, 0x7F, 0x22, 0x14,\
		0x5F, 0x5F, 0x00, 0x5F, 0x5F,\
		0x06, 0x09, 0x7F, 0x01, 0x7F,\
		0x00, 0x66, 0x89, 0x95, 0x6A,\
		0x60, 0x60, 0x60, 0x60, 0x60,\
		0x94, 0xA2, 0xFF, 0xA2, 0x94,\
		0x08, 0x04, 0x7E, 0x04, 0x08,\
		0x10, 0x20, 0x7E, 0x20, 0x10,\
		0x08, 0x08, 0x2A, 0x1C, 0x08,\
		0x08, 0x1C, 0x2A, 0x08, 0x08,\
		0x1E, 0x10, 0x10, 0x10, 0x10,\
		0x0C, 0x1E, 0x0C, 0x1E, 0x0C,\
		0x30, 0x38, 0x3E, 0x38, 0x30,\
		0x06, 0x0E, 0x3E, 0x0E, 0x06,\
		0x00, 0x00, 0x00, 0x00, 0x00,\
		0x00, 0x00, 0x5F, 0x00, 0x00,\
		0x00, 0x07, 0x00, 0x07, 0x00,\
		0x14, 0x7F, 0x14, 0x7F, 0x14,\
		0x24, 0x2A, 0x7F, 0x2A, 0x12,\
		0x23, 0x13, 0x08, 0x64, 0x62,\
		0x36, 0x49, 0x56, 0x20, 0x50,\
		0x00, 0x08, 0x07, 0x03, 0x00,\
		0x00, 0x1C, 0x22, 0x41, 0x00,\
		0x00, 0x41, 0x22, 0x1C, 0x00,\
		0x2A, 0x1C, 0x7F, 0x1C, 0x2A,\
		0x08, 0x08, 0x3E, 0x08, 0x08,\
		0x00, 0x80, 0x70, 0x30, 0x00,\
		0x08, 0x08, 0x08, 0x08, 0x08,\
		0x00, 0x00, 0x60, 0x60, 0x00,\
		0x20, 0x10, 0x08, 0x04, 0x02,\
		0x3E, 0x51, 0x49, 0x45, 0x3E,\
		0x00, 0x42, 0x7F, 0x40, 0x00,\
		0x72, 0x49, 0x49, 0x49, 0x46,\
		0x21, 0x41, 0x49, 0x4D, 0x33,\
		0x18, 0x14, 0x12, 0x7F, 0x10,\
		0x27, 0x45, 0x45, 0x45, 0x39,\
		0x3C, 0x4A, 0x49, 0x49, 0x31,\
		0x41, 0x21, 0x11, 0x09, 0x07,\
		0x36, 0x49, 0x49, 0x49, 0x36,\
		0x46, 0x49, 0x49, 0x29, 0x1E,\
		0x00, 0x00, 0x14, 0x00, 0x00,\
		0x00, 0x40, 0x34, 0x00, 0x00,\
		0x00, 0x08, 0x14, 0x22, 0x41,\
		0x14, 0x14, 0x14, 0x14, 0x14,\
		0x00, 0x41, 0x22, 0x14, 0x08,\
		0x02, 0x01, 0x59, 0x09, 0x06,\
		0x3E, 0x41, 0x5D, 0x59, 0x4E,\
		0x7C, 0x12, 0x11, 0x12, 0x7C,\
		0x7F, 0x49, 0x49, 0x49, 0x36,\
		0x3E, 0x41, 0x41, 0x41, 0x22,\
		0x7F, 0x41, 0x41, 0x41, 0x3E,\
		0x7F, 0x49, 0x49, 0x49, 0x41,\
		0x7F, 0x09, 0x09, 0x09, 0x01,\
		0x3E, 0x41, 0x41, 0x51, 0x73,\
		0x7F, 0x08, 0x08, 0x08, 0x7F,\
		0x00, 0x41, 0x7F, 0x41, 0x00,\
		0x20, 0x40, 0x41, 0x3F, 0x01,\
		0x7F, 0x08, 0x14, 0x22, 0x41,\
		0x7F, 0x40, 0x40, 0x40, 0x40,\
		0x7F, 0x02, 0x1C, 0x02, 0x7F,\
		0x7F, 0x04, 0x08, 0x10, 0x7F,\
		0x3E, 0x41, 0x41, 0x41, 0x3E,\
		0x7F, 0x09, 0x09, 0x09, 0x06,\
		0x3E, 0x41, 0x51, 0x21, 0x5E,\
		0x7F, 0x09, 0x19, 0x29, 0x46,\
		0x26, 0x49, 0x49, 0x49, 0x32,\
		0x03, 0x01, 0x7F, 0x01, 0x03,\
		0x3F, 0x40, 0x40, 0x40, 0x3F,\
		0x1F, 0x20, 0x40, 0x20, 0x1F,\
		0x3F, 0x40, 0x38, 0x40, 0x3F,\
		0x63, 0x14, 0x08, 0x14, 0x63,\
		0x03, 0x04, 0x78, 0x04, 0x03,\
		0x61, 0x59, 0x49, 0x4D, 0x43,\
		0x00, 0x7F, 0x41, 0x41, 0x41,\
		0x02, 0x04, 0x08, 0x10, 0x20,\
		0x00, 0x41, 0x41, 0x41, 0x7F,\
		0x04, 0x02, 0x01, 0x02, 0x04,\
		0x40, 0x40, 0x40, 0x40, 0x40,\
		0x00, 0x03, 0x07, 0x08, 0x00,\
		0x20, 0x54, 0x54, 0x78, 0x40,\
		0x7F, 0x28, 0x44, 0x44, 0x38,\
		0x38, 0x44, 0x44, 0x44, 0x28,\
		0x38, 0x44, 0x44, 0x28, 0x7F,\
		0x38, 0x54, 0x54, 0x54, 0x18,\
		0x00, 0x08, 0x7E, 0x09, 0x02,\
		0x18, 0xA4, 0xA4, 0x9C, 0x78,\
		0x7F, 0x08, 0x04, 0x04, 0x78,\
		0x00, 0x44, 0x7D, 0x40, 0x00,\
		0x20, 0x40, 0x40, 0x3D, 0x00,\
		0x7F, 0x10, 0x28, 0x44, 0x00,\
		0x00, 0x41, 0x7F, 0x40, 0x00,\
		0x7C, 0x04, 0x78, 0x04, 0x78,\
		0x7C, 0x08, 0x04, 0x04, 0x78,\
		0x38, 0x44, 0x44, 0x44, 0x38,\
		0xFC, 0x18, 0x24, 0x24, 0x18,\
		0x18, 0x24, 0x24, 0x18, 0xFC,\
		0x7C, 0x08, 0x04, 0x04, 0x08,\
		0x48, 0x54, 0x54, 0x54, 0x24,\
		0x04, 0x04, 0x3F, 0x44, 0x24,\
		0x3C, 0x40, 0x40, 0x20, 0x7C,\
		0x1C, 0x20, 0x40, 0x20, 0x1C,\
		0x3C, 0x40, 0x30, 0x40, 0x3C,\
		0x44, 0x28, 0x10, 0x28, 0x44,\
		0x4C, 0x90, 0x90, 0x90, 0x7C,\
		0x44, 0x64, 0x54, 0x4C, 0x44,\
		0x00, 0x08, 0x36, 0x41, 0x00,\
		0x00, 0x00, 0x77, 0x00, 0x00,\
		0x00, 0x41, 0x36, 0x08, 0x00,\
		0x02, 0x01, 0x02, 0x04, 0x02,\
		0x3C, 0x26, 0x23, 0x26, 0x3C,\
		0x1E, 0xA1, 0xA1, 0x61, 0x12,\
		0x3A, 0x40, 0x40, 0x20, 0x7A,\
		0x38, 0x54, 0x54, 0x55, 0x59,\
		0x21, 0x55, 0x55, 0x79, 0x41,\
		0x21, 0x54, 0x54, 0x78, 0x41,\
		0x21, 0x55, 0x54, 0x78, 0x40,\
		0x20, 0x54, 0x55, 0x79, 0x40,\
		0x0C, 0x1E, 0x52, 0x72, 0x12,\
		0x39, 0x55, 0x55, 0x55, 0x59,\
		0x39, 0x54, 0x54, 0x54, 0x59,\
		0x39, 0x55, 0x54, 0x54, 0x58,\
		0x00, 0x00, 0x45, 0x7C, 0x41,\
		0x00, 0x02, 0x45, 0x7D, 0x42,\
		0x00, 0x01, 0x45, 0x7C, 0x40,\
		0xF0, 0x29, 0x24, 0x29, 0xF0,\
		0xF0, 0x28, 0x25, 0x28, 0xF0,\
		0x7C, 0x54, 0x55, 0x45, 0x00,\
		0x20, 0x54, 0x54, 0x7C, 0x54,\
		0x7C, 0x0A, 0x09, 0x7F, 0x49,\
		0x32, 0x49, 0x49, 0x49, 0x32,\
		0x32, 0x48, 0x48, 0x48, 0x32,\
		0x32, 0x4A, 0x48, 0x48, 0x30,\
		0x3A, 0x41, 0x41, 0x21, 0x7A,\
		0x3A, 0x42, 0x40, 0x20, 0x78,\
		0x00, 0x9D, 0xA0, 0xA0, 0x7D,\
		0x39, 0x44, 0x44, 0x44, 0x39,\
		0x3D, 0x40, 0x40, 0x40, 0x3D,\
		0x3C, 0x24, 0xFF, 0x24, 0x24,\
		0x48, 0x7E, 0x49, 0x43, 0x66,\
		0x2B, 0x2F, 0xFC, 0x2F, 0x2B,\
		0xFF, 0x09, 0x29, 0xF6, 0x20,\
		0xC0, 0x88, 0x7E, 0x09, 0x03,\
		0x20, 0x54, 0x54, 0x79, 0x41,\
		0x00, 0x00, 0x44, 0x7D, 0x41,\
		0x30, 0x48, 0x48, 0x4A, 0x32,\
		0x38, 0x40, 0x40, 0x22, 0x7A,\
		0x00, 0x7A, 0x0A, 0x0A, 0x72,\
		0x7D, 0x0D, 0x19, 0x31, 0x7D,\
		0x26, 0x29, 0x29, 0x2F, 0x28,\
		0x26, 0x29, 0x29, 0x29, 0x26,\
		0x30, 0x48, 0x4D, 0x40, 0x20,\
		0x38, 0x08, 0x08, 0x08, 0x08,\
		0x08, 0x08, 0x08, 0x08, 0x38,\
		0x2F, 0x10, 0xC8, 0xAC, 0xBA,\
		0x2F, 0x10, 0x28, 0x34, 0xFA,\
		0x00, 0x00, 0x7B, 0x00, 0x00,\
		0x08, 0x14, 0x2A, 0x14, 0x22,\
		0x22, 0x14, 0x2A, 0x14, 0x08,\
		0xAA, 0x00, 0x55, 0x00, 0xAA,\
		0xAA, 0x55, 0xAA, 0x55, 0xAA,\
		0x55, 0xAA, 0x55, 0xAA, 0x55,\
		0x00, 0x00, 0x00, 0xFF, 0x00,\
		0x10, 0x10, 0x10, 0xFF, 0x00,\
		0x14, 0x14, 0x14, 0xFF, 0x00,\
		0x10, 0x10, 0xFF, 0x00, 0xFF,\
		0x10, 0x10, 0xF0, 0x10, 0xF0,\
		0x14, 0x14, 0x14, 0xFC, 0x00,\
		0x14, 0x14, 0xF7, 0x00, 0xFF,\
		0x00, 0x00, 0xFF, 0x00, 0xFF,\
		0x14, 0x14, 0xF4, 0x04, 0xFC,\
		0x14, 0x14, 0x17, 0x10, 0x1F,\
		0x10, 0x10, 0x1F, 0x10, 0x1F,\
		0x14, 0x14, 0x14, 0x1F, 0x00,\
		0x10, 0x10, 0x10, 0xF0, 0x00,\
		0x00, 0x00, 0x00, 0x1F, 0x10,\
		0x10, 0x10, 0x10, 0x1F, 0x10,\
		0x10, 0x10, 0x10, 0xF0, 0x10,\
		0x00, 0x00, 0x00, 0xFF, 0x10,\
		0x10, 0x10, 0x10, 0x10, 0x10,\
		0x10, 0x10, 0x10, 0xFF, 0x10,\
		0x00, 0x00, 0x00, 0xFF, 0x14,\
		0x00, 0x00, 0xFF, 0x00, 0xFF,\
		0x00, 0x00, 0x1F, 0x10, 0x17,\
		0x00, 0x00, 0xFC, 0x04, 0xF4,\
		0x14, 0x14, 0x17, 0x10, 0x17,\
		0x14, 0x14, 0xF4, 0x04, 0xF4,\
		0x00, 0x00, 0xFF, 0x00, 0xF7,\
		0x14, 0x14, 0x14, 0x14, 0x14,\
		0x14, 0x14, 0xF7, 0x00, 0xF7,\
		0x14, 0x14, 0x14, 0x17, 0x14,\
		0x10, 0x10, 0x1F, 0x10, 0x1F,\
		0x14, 0x14, 0x14, 0xF4, 0x14,\
		0x10, 0x10, 0xF0, 0x10, 0xF0,\
		0x00, 0x00, 0x1F, 0x10, 0x1F,\
		0x00, 0x00, 0x00, 0x1F, 0x14,\
		0x00, 0x00, 0x00, 0xFC, 0x14,\
		0x00, 0x00, 0xF0, 0x10, 0xF0,\
		0x10, 0x10, 0xFF, 0x10, 0xFF,\
		0x14, 0x14, 0x14, 0xFF, 0x14,\
		0x10, 0x10, 0x10, 0x1F, 0x00,\
		0x00, 0x00, 0x00, 0xF0, 0x10,\
		0xFF, 0xFF, 0xFF, 0xFF, 0xFF,\
		0xF0, 0xF0, 0xF0, 0xF0, 0xF0,\
		0xFF, 0xFF, 0xFF, 0x00, 0x00,\
		0x00, 0x00, 0x00, 0xFF, 0xFF,\
		0x00, 0x00, 0x00, 0x00, 0x00,\
		0x38, 0x44, 0x44, 0x38, 0x44,\
		0x7C, 0x2A, 0x2A, 0x3E, 0x14,\
		0x7E, 0x02, 0x02, 0x06, 0x06,\
		0x02, 0x7E, 0x02, 0x7E, 0x02,\
		0x63, 0x55, 0x49, 0x41, 0x63,\
		0x38, 0x44, 0x44, 0x3C, 0x04,\
		0x40, 0x7E, 0x20, 0x1E, 0x20,\
		0x06, 0x02, 0x7E, 0x02, 0x02,\
		0x99, 0xA5, 0xE7, 0xA5, 0x99,\
		0x1C, 0x2A, 0x49, 0x2A, 0x1C,\
		0x4C, 0x72, 0x01, 0x72, 0x4C,\
		0x30, 0x4A, 0x4D, 0x4D, 0x30,\
		0x30, 0x48, 0x78, 0x48, 0x30,\
		0xBC, 0x62, 0x5A, 0x46, 0x3D,\
		0x3E, 0x49, 0x49, 0x49, 0x00,\
		0x7E, 0x01, 0x01, 0x01, 0x7E,\
		0x2A, 0x2A, 0x2A, 0x2A, 0x2A,\
		0x44, 0x44, 0x5F, 0x44, 0x44,\
		0x40, 0x51, 0x4A, 0x44, 0x40,\
		0x40, 0x44, 0x4A, 0x51, 0x40,\
		0x00, 0x00, 0xFF, 0x01, 0x03,\
		0xE0, 0x80, 0xFF, 0x00, 0x00,\
		0x08, 0x08, 0x6B, 0x6B, 0x08,\
		0x36, 0x12, 0x36, 0x24, 0x36,\
		0x06, 0x0F, 0x09, 0x0F, 0x06,\
		0x00, 0x00, 0x18, 0x18, 0x00,\
		0x00, 0x00, 0x10, 0x10, 0x00,\
		0x30, 0x40, 0xFF, 0x01, 0x01,\
		0x00, 0x1F, 0x01, 0x01, 0x1E,\
		0x00, 0x19, 0x1D, 0x17, 0x12,\
		0x00, 0x3C, 0x3C, 0x3C, 0x3C,\
		0x00, 0xFF, 0xFF, 0xFF, 0xFF








