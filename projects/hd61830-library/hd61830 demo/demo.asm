.include "m32def.inc"
;---------------------------------------------------------
; those are hd61830 specyfic definitions
;---------------------------------------------------------
; glcd pin assigments
.equ		GLCD_DATA		=	portc
.equ		GLCD_DATAP		=	pinc
.equ		GLCD_DATAD		=	ddrc
.equ		GLCD_CONTROL	=	portd
.equ		GLCD_CS			=	pd7
.equ		GLCD_RS			=	pd6
.equ		GLCD_RESET		=	pd5
.equ		GLCD_RW			=	pd4
.equ		GLCD_EN			=	pd3
;---------------------------------------------------------
; glcd dislay constants
.equ		GLCD_SCANSTART	=	0
.equ		GLCD_TEXTWIDTH	=	40
.equ		GLCD_GRAPHWIDTH	=	30
.equ		GLCD_DUTY		=	64
;---------------------------------------------------------
; general constants

.equ		STACK_SIZE		=	0x40
.def		zero			=	r10
.dseg
.org		SRAM_START
line_buffer:
.byte		40
h_scroll:
.byte		1
scroll_pos:
.byte		1
Start_line:
.byte		1
text_start_line:
.byte		1
Start_prv:
.byte		1
_x:
.byte		1
_x1:
.byte		1
_x2:
.byte		1
.cseg
.org		0x0000
			rjmp	start
.org		ADCCaddr

.org		0x0040
start:
			
			ldi		r16,0
			mov		zero,r16
			sts		scroll_pos,r16
			ldi		r16,0x80
			sts		h_scroll,r16						; initialize variables
			ldi		r16,HIGH(RAMEND)
			out		SPH,r16
			ldi		r16,LOW(RAMEND)
			out		SPL,r16								; set the stack pointer
			ldi		r16,0xff
			out		DDRB,r16							; port B,C,D as output
			out		DDRC,r16
			out		DDRD,r16
			in		r16,MCUCSR
			ori		r16,0x80
			out		MCUCSR,r16
			out		MCUCSR,r16							; disable JTAG
			rcall	HD61830_Reset						; reset the HD61830 controller
			rcall	HD61830_GrInit						; initialize it for graphics mode
			rcall	HD61830_Cls							; clear screen
loop:		lds		r16,h_scroll
			lsr		r16
			brcc	Scroll_ok
			ror		r16									; if avr had rol instruction one would suffice, but it has only rcl and lsl, and we need to shift
			lds		r17,scroll_pos						; h_scroll bit by bit over and over again
			inc		r17
			andi	r17,0x0f							; after full cycle we also need to increment scroll_pos to enable scrolling by more than 8 pixels
			sts		scroll_pos,r17						; full line of our logo has 16 bytes, so valid numbers are 0-15
Scroll_ok:	sts		h_scroll,r16
			rcall	Sine								; update vertical scroll positions
			rcall	Scroller							; do the magic
end:		rjmp	loop
;---------------------------------------------------------
; LineShift
; since AVR doesn't have capability of shifting bits more than one, h_scroll is rather multiplicator than counter
; effect is bits are shifted as supposed to, and we have bits that wouldn't fit into byte just where we want them to be
; h_scroll - binary mask for the shift
LineShift:	push	r17
			push	r18
			ldi		xl,low(line_buffer)
			ldi		xh,high(line_buffer)				; get line_buffer pointer
			lds		r17,h_scroll						; load multiplicator
			ldi		r18,GLCD_GRAPHWIDTH+1				; how many bytes to shift?
			mov		r9,zero								; clear temporar storage for bits that didn't fit into byte anymore
LineShift_l1:
			ld		r16,x								; load byte to shift
			mul		r16,r17								; "shift" it  by arbitrary bit count
			or		r9,r0								; or it with previous byte
			st		x+,r9								; store the result on current position
			mov		r9,r1								; update our temporar register with new bits
			dec		r18
			brne	LineShift_l1						; repeat for all bytes
			pop		r18
			pop		r17
			ret
;---------------------------------------------------------
;Sine
;it calculates two functions for vertical scrolling procedure
;r16=sin(x+x1)+sin(x+x2)
;r15=sin(x+x1)
Sine:		lds		r16,Start_Line
			sts		Start_prv,r16
			ldi		zl,low(SinTab1*2)
			ldi		zh,high(SinTab1*2)
			lds		r16,_x
			inc		r16
			sts		_x,r16
			mov		r17,r16
			lds		r18,_x1
			subi	r18,-1
			sts		_x1,r18
			lds		r19,_x2
			subi	r19,7
			sts		_x2,r19
			add		r16,r18
			add		r17,r19
			add		zl,r16
			adc		zh,zero
			lpm		r16,z
			mov		r15,r16
			ldi		zl,low(SinTab2*2)
			ldi		zh,high(SinTab2*2)
			add		zl,r17
			adc		zh,zero
			lpm		r17,z
			add		r16,r17
			sts		Start_Line,r16
			sts		text_start_line,r15
			ret
;---------------------------------------------------------
; ATASCII to charset code
; fonts i use are from atari 8bit computer, it uses special mapping for fonts since there is only 6 bits for all letters and digits, thus some reorganisation is required
; i suppose one could convert data to standard ascii table before compilation, but then diffrent fonts had to be converted pririor to compilation
; this way it was simplier
ATASCII2CHR:
			cpi		r16,32
			brlo	ATASCII2CHR_p64
			cpi		r16,96
			brlo	ATASCII2CHR_m32
			ret
ATASCII2CHR_p64:
			subi	r16,-64
			ret
ATASCII2CHR_m32:
			subi	r16,32
			ret
;---------------------------------------------------------
;scroller
; scrolls the ATMEL logo and display static text overlayed on it
; text has some border generated to increase readibility
Scroller:	ldi		zl,low(Font_Data*2)
			ldi		zh,high(Font_Data*2)
			movw	r5:r4,zh:zl
			ldi		zl,low(Text*2)
			ldi		zh,high(Text*2)
			movw	r7:r6,zh:zl
			ldi		zl,low(logo*2)
			ldi		zh,high(logo*2)					; get logo pointer
			lds		r16,start_line
			ldi		r17,16
			mul		r16,r17
			add		zl,r0
			adc		zh,r1								; add start_line*line_size to logo pointer thus shifting it by start_line vertically
			lds		r16,scroll_pos
			add		zl,r16
			adc		zh,zero								; add scroll_pos to coarse shift the logo by scroll_pos bytes
			movw	r3:r2,zh:zl							; store z pointer
			clr		r19									; r19 will hold current line number
			ldi		r18,64								; we have 64 lines in display
Scroller_l1:
			ldi		xl,low(line_buffer)
			ldi		xh,high(line_buffer)
			ldi		r17,GLCD_GRAPHWIDTH+1				; reserve one byte for shifting
			movw	zh:zl,r3:r2							; restore z pointer
Scroller_l2:
			lpm		r16,z
			adiw	zl,1
			push	r17
			call	BitReverse							; bitmap is stored in normal bit order, hd61830 uses big endian encoding, so bits need reversing
			pop		r17
			st		x+,r16
			dec		r17
			brne	Scroller_l2							; repeat for whole line
			movw	zh:zl,r3:r2
			adiw	zl,16								; restore logo pointer and add line size
			movw	r3:r2,zh:zl							; store z pointer back
			rcall	LineShift							; shift line by h_scroll bits (fine shift)
			rcall	DisplayText							; displays one raster line of text that is going to be overlayed
			ldi		xl,low(line_buffer+1)
			ldi		xh,high(line_buffer+1)
			mov		r16,r19
			rcall	HD61830_LineOut						; transmit line buffer to lcd
			inc		r19
			dec		r18
			brne	Scroller_l1							; repeat for whole screen
			ret
;---------------------------------------------------------
;DisplayText
; display text given at Text adding strike out effect
; r19 - current line nuber
; r5:r4 - font data
; r7:r6 - text to be displayed
DisplayText_end:
			ret
DisplayText:
			lds		r16,text_start_line
			cp		r19,r16
			brlo	DisplayText_end						; should we start generating data?
			mov		r17,r19
			sub		r17,r16
			cpi		r17,32								; are we outside text frame area?
			brsh	DisplayText_end
			mov		r20,r17
			andi	r20,7								; get the raster line number inside a char
			lsr		r17
			lsr		r17
			lsr		r17									; get the line nuber
			ldi		r16,30
			mov		r21,r16								; remember for loop count
			mul		r17,r16
			movw	zh:zl,r7:r6
			add		zl,r0
			adc		zh,r1								; add line number to text pointer, each line consist of 30 bytes
			ldi		r16,8
			mov		r13,r16
			movw	r9:r8,zh:zl
			ldi		yl,low(line_buffer)
			ldi		yh,high(line_buffer)
DisplayText_l1:
			movw	zh:zl,r9:r8
			lpm		r16,z
			adiw	zl,1
			movw	r9:r8,zh:zl
			cpi		r16,32
			breq	DisplayText_next_char
			rcall	ATASCII2CHR
			mul		r16,r13
			movw	zh:zl,r5:r4							; restore font pointer
			add		zl,r0
			adc		zh,r1								; add char offset to font pointer
			add		zl,r20								; add raster line number to char offset
			adc		zh,zero
			lpm		r16,z								; get font data
			adiw	zl,1
			lpm		r17,z
			mov		r11,r16
			mov		r12,r16
			lsr		r11
			brcc	DisplayText_right_pixel_ok			; have we shifted lit pixel out of the byte?
			ldd		r23,y+2								; if yes, then correct nearest byte accordingly
			andi	r23,0xfe
			std		y+2,r23
DisplayText_right_pixel_ok:
			lsl		r12
			brcc	DisplayText_left_pixel_ok			; same for the left side
			ld		r23,y
			andi	r23,0x7f
			st		y,r23
DisplayText_left_pixel_ok:
			or		r12,r17
			or		r12,r11
			sbiw	zl,2
			lpm		r17,z
			or		r12,r16
			or		r17,r12								; expand our stencil mask a bit or two in each direction
			com		r17
			rcall	BitReverse							; again, atari uses big endian byte encoding, so char data and its stencil mask need to be bit reversed
			ldd		r22,y+1
			and		r22,r17
			or		r22,r16
			std		y+1,r22								; cut out byte at line_buffer[x] and put there our char
DisplayText_next_char:
			adiw	yl,1
			dec		r21
			brne	DisplayText_l1
			ret
;---------------------------------------------------------
;libraries
.include 	"hd61830.asm"
;---------------------------------------------------------
; tables
Text:
;			           1         2
;			 012345678901234567890123456789
.db			"       HD61830 lib demo       "
.db			"              by              "
.db			"          Candle/Sin          "
.db			"    hope it helps someone     ",0,0
.include	"sinus.inc"
Font_Data:
.include	"bzzz2.inc"
.include	"atmel_logo.inc"
