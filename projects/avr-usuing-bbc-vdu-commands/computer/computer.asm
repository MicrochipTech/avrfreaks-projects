.include "m16def.inc"
.include "characters.inc"
.include "sram.inc"


;0.9.0	getting it to display text	
; revisions
;1.4.0	Now does all basic test operations except windowing
;1.5.0	add user defined characters
;1.6.0	add mode zero - 32 characters wide
;1.7.0	Use vector table for Control codes
;1.8.0	Add windowing - still buggy
;Major rewrite
;2.0.0	Make row absolute, not relative to scroll
;2.1.0	Windowing now works, but not added scrolling yet
;2.2.0	All text movement and windowing fully functional
;2.3.0	Added modes:
;		0		32 characters wide
;		1,4		16 characters wide
;		2,3,5,6,7	8 characters wide
;2.4.0	Added Osbyte Emulation using VDU27,A,X,Y
;2.5.0	Recognise structure of plot commands
;2.6.0	Set up graphic colour commands and defaults
;2.7.0	Combine default graphic colours and plot to get plotting style.
;Major addition
;3.0.0	Add PLOT primitives
;3.1.0	Breakthrough! Fill vertical line works, but slow plotting single pixels.
;3.1.1	Huge speed increase by building plot mask byte.
;3.2.0	Added plot a point
;3.3.0	Problems implementing rectangle plot
;Major revision
;4.0.0	Changed way variables are used for more efficient plotting
;4.1.0	Rectangle plotting implemented OK!
;4.2.0	Bresenham Line Algrithm working in one quadrant!
;4.3.0	Now working lines all around the clock.
;4.3.1	Added dotted lines
;4.3.2	Independent graphic and text windows proven.
;Major addition
;5.1.1	Outline circles
;5.2.0	Filled circles
;5.3.0	Fixed character reads in all modes
;5.3.1	Now character read also returns mode
;5.4.0	Cursor shape can be read and changed
;5.5.1	Cursor tab position may be read
;5.5.2	Graphics palette may be read
;Major addition
;6.0.0	Printing at graphics cursor, but not point movement yet.
;6.1.0	Four directions of graphics cursor move, but CR not working!
;6.2.0	Added CR and graphics print/cursor wrapping. DEF!
;Major addition
;7.0.0	Added graphic colours affecting characters at graphic cursor
;7.0.1	Debugged text size setting
;7.1.0	Debug	circle endpoints for reliable EOR plots
;7.1.1	Debugged circles entirely outside window
;7.2.0	Added user defined characters at graphic cursor
;Major addition
;8.0.0	Filled triangles added.
;8.0.1	Redundant extra tab call removed
;Major addition
;9.0.0	Keyboard support added
;9.1.0	unshifted,shifted and symbol codes added
;Major addition
;10.0.0	SPI slave support added

;*****These are my pin and register names***************************
					;r0,r1 used for multiplication resultby default
.def	pixel_mask	=r2
.def	timer1h	=r3		;used to update Timer 1
.def	timer1l	=r4		;for 1 second tick
.def	display_byte=r5
.def	key_char	=r6		;for character from keyboard
.def	vdu_queue	=r7		;length of VDU queue in chars
.def	char_width	=r8
.def	scroll	=r9		;first display line
.def	left		=r10		;text window left
.def	right		=r11		;dimensions	right in PIXELS plus one
.def	top		=r12		; top	
.def	bottom	=r13		; bottom+1 in ROWS
.def	b_pointer	=r14		;buffer pointer
.def	i_pointer	=r15		;input pointer
.def	temp		=r16
.def	temp1		=r17
.def	fx_x		=r17
.def	more_flags	=r18		;
.def	pointer	=r19		;pointer for counting
.def	char		=r20		;Character to be sent to screen
.def	row		=r21		;current text row
.def	column	=r22		;text column
.def	flags		=r23		;Flag register
.def	colflags	=r24		;flags for colours and plotting
.def	temp2		=r25		;for use in non-overlapping interrupts
.def	fx_y		=r25
.def	temp3		=r26		;
.def	pageY		=r27		;we aren't using X-register (much)

;Flag register definitions

;flags
.equ	cursor	=0		;set when cursor visible
.equ	pending	=1		;set when a scroll is pending
.equ	windowed	=2		;set when a graphics window (no fast scrolls)
.equ	paging	=3		;set when in paged mode
.equ	printer_on	=4		;set when in printer to 232 mode
.equ	screen_off	=5		;CLEAR when displaying
.equ	graphics	=6		;set when in graphics cursor mode
.equ	dotted	=7		;for dotted lines/also flag for bitmap load

;colflags
.equ	endx_less	=0		;for line plotting
.equ	endy_less	=1		;	do
.equ	dx_less	=2		;	do
.equ	omit_first	=3		;don't plot first dot
.equ	omit_last	=4		;don't plot last dot
.equ	clear			=5	;set when graphics plot colour clear
.equ	black			=6	;set when graphics plot colour black
.equ	plot_invert		=7	;set when graphics inverts (overides gcol)	

;more_flags
.equ	flash_state		=0	;set according to ideal cursor state
.equ	char_here		=1	;set when character recieved from keyboard
.equ	shift			=2	;set when shift pressed
.equ	key_up		=3	;set to show next data is key released
.equ	symbol		=4	;set when symbol key pressed

;Port C pins - the data bus for the screen

.equ	db0	=0
.equ	db1	=1
.equ	db2	=2
.equ	db3	=3
.equ	db4	=4
.equ	db5	=5
.equ	db6	=6
.equ	db7	=7

;Port D pins - control lines for the screen plus RS232 and an interrupt line

.equ RXD	=0	;RS232 receive pin
.equ TXD	=1	;RS232 transmit pin
.equ CTS	=2	;For RS232 data flow control
.equ enable	=3	;Toggle to clock data into display
.equ data	=4	;data high/instruction low
.equ read	=5	;read high/write low
.equ cs2	=6	;low select chip 2
.equ cs1	=7	;low select chip 1

;Port A pins - inputs for A/D conversion




;Port B pins - for SPI / Programming interface and keyboard interface

.equ	KCK	=2	;INT2 pin, Keyboard ClocK
.equ	KDT	=1	;Keyboard data - configure as input

.equ	SS	=4	;/Slave Select
.equ	MOSI	=5	
.equ	MISO	=6
.equ	SCK	=7


;VDU variables - locations in SRAM



.eseg
;EEPROM
.org 	0x00
BOOT_MESSAGE:
.db	"AVR Computer 32K",0x00
VERSION_MESSAGE:
.db	"I/O Processor "
.db	"Version 10.0.0.0",'\n','\r',0x00
PROMPT:
.db	"Phragsoft OS",'\r','\n',0x00

.org	0x0100
;**********	Upper page of EEPROM generously allocated to the user!
USER_EEPROM:	.byte	256
.cseg

;********	Interrupt jump vectors

.org 00

	jmp RESET ; Reset Handler
	jmp EXT_INT0 ; IRQ0 Handler
	jmp EXT_INT1 ; IRQ1 Handler
	jmp TIM2_COMP ; Timer2 Compare Handler
	jmp TIM2_OVF ; Timer2 Overflow Handler
	jmp TIM1_CAPT ; Timer1 Capture Handler
	jmp TIM1_COMPA ; Timer1 CompareA Handler
	jmp TIM1_COMPB ; Timer1 CompareB Handler
	jmp TIM1_OVF ; Timer1 Overflow Handler
	jmp TIM0_OVF ; Timer0 Overflow Handler
	jmp SPI_STC ; SPI Transfer Complete Handler
	jmp USART_RXC ; USART RX Complete Handler
	jmp USART_UDRE ; UDR Empty Handler
	jmp USART_TXC ; USART TX Complete Handler
	jmp ADCC ; ADC Conversion Complete Handler
	jmp EE_RDY ; EEPROM Ready Handler
	jmp ANA_COMP ; Analog Comparator Handler
	jmp TWSI ; Two-wire Serial Interface Handler
	jmp EXT_INT2 ; IRQ2 Handler
	jmp TIM0_COMP ; Timer0 Compare Handler
	jmp SPM_RDY ; Store Program Memory Ready Handler

RESET:

;*********	Initialise stack to end of SRAM

	ldi temp,high(RAMEND) ; Main program start
	out SPH,temp ; Set Stack Pointer to top of RAM
	ldi temp,low(RAMEND)
	out SPL,temp
 
MAIN:

;*********	Set I/O status of Ports NB: for trial ports A & C  used instead of port C & D

	ldi	temp,0xFF		;all high
	out	DDRC,temp		;Port C all outputs
	out	PORTC,temp		;Outputs high for now
	out	DDRD,temp		;Port D is all outputs
	ldi	temp,0xF0		;set bottom four bits low
	out	PORTD,temp		;enable low, data,read, no chip selected
	cbi	PORTD,enable
	ldi	temp,(0<<KCK|0<<KDT|1<<MISO|0<<MOSI|0<<SCK|0<<SS|1<<PB0)	;
	out	DDRB,temp
;*********  Flash the sole status LED during boot-up

	sbi	PORTB,PB0


;*********	Initialise variables

	clr	b_pointer		;clear both buffer pointers
	clr	i_pointer
	clr	flags			;default of all flags is clear
	clr	more_flags		;default of all more flags is clear!
	clr	vdu_queue		;when multi-byte VDU received, put queue length here
	ldi	temp,0x08
	mov	char_width,temp
	ldi	temp,0x00
	sts	pattern,temp
	sts	tv,temp
	ldi	temp,0x80
	sts	cursor_pattern,temp		;set default cursor
	sts	old_cursor_pattern,temp
	ldi	temp,0x01
	sts	mode,temp
;*********	Initialise	display

	call	DISPLAY_ON
	clr		scroll
	call	SCROLL_DISPLAY
	call	RST_WINDOWS				;set default text and graphic windows
	call	DEFAULT_COLOURS			;set default text and graphic colours
	call	CLEAR_SCREEN

;rjmp TEST						;uncomment to speed up boot for simulator

;*********	Display boot message

	ldi		YH,high(BOOT_MESSAGE)
	ldi		YL,low(BOOT_MESSAGE)
	call	EEPROM_STRING			;display boot message
	ldi		temp,0x04
	mov		char_width,temp
	call	EEPROM_STRING			;display version number
	ldi		temp,0x08
	mov		char_width,temp
	call	EEPROM_STRING			;display prompt

;*********	Initialise	interrupt routines

TEST:

	call	USART_Init				;set up RS232
	call	TIMER2_INIT				;set up cursor click
	call	TIMER1_INIT				;set up time click
	call	TIMER0_INIT				;set up time click
	call	INT2_INIT				;set up keyboard interrupt
	call	SPI_SlaveInit			;set up as SPI slave

	sei ; Enable interrupts

	cbi	PORTB,PB0				;unflash status LED


;*********	Now just hang around waiting for something to happen

WAIT_FOR_DATA:


	cpse	i_pointer,b_pointer		;has buffer got ahead of us?
	rcall	PROCESS_BUFFER			;yes, process it
	sbrc	more_flags,flash_state		;do we need to flash?
	rcall	flash_cursor
	rjmp	WAIT_FOR_DATA

;*********	Keep debug routines here

TESTLOOP:	;output ASCII character set at a gentle pace
	call DELAY
	call DELAY

	call	OSWRCH 
	inc	char
	cpi	char,0x7F
	brne	TESTLOOP
	ret

;*********	Primitive operations

CLEAR_SCREEN:			;Clears text window
	lds	temp,pattern
	mov	row,top			;is window top left
NEXT_row:
	mov	column,left
	rcall	TAB
	out	PORTC,temp
NEXT_column:
	cpi	column,0x40		;which side of display?
	brlo	l_side
	rcall TAB
	out	PORTC,temp
	r_side:
	rcall CLOCK
	inc	column
	cp	column,right
	brne	r_side
	rjmp	increment_row
	l_side:
	rcall CLOCK
	inc	column
	cp	column,right
	brne	NEXT_column
	increment_row:
	inc	row
	cp	row,bottom
	brne	NEXT_row

	mov	row,top
	mov	column,left
	rcall	TAB				;Reposition cursor (so it flasheth correctlty)

	cbr	flags,(1<<pending)	;clear any pending scroll
	ret

;-------------------------------------------------------------------------------------------

SEND_SCROLL:			;scrols without clearing anything

	rcall	TWO_SIDE
	rcall	SEND_INSTRUCTION
	mov	temp,scroll
	ori	temp,0xC0
	mov	scroll,temp
	out	PORTC,scroll
	rcall CLOCK			;first we scroll the screen

	ret

;-------------------------------------------------------------------------------------------

SCROLL_DISPLAY:		;scrolls to line specified, clearing wrapped line

	rcall	SEND_SCROLL

CLEAR_LINE:			;clears current line
	push	column
	mov	column,left
	lds	temp2,pattern
NEXT_COLUMN_CLEAR:
	rcall TAB
	mov	temp,char_width		;Lots to delete
NEXT_BYTE:
	out	PORTC,temp2			;the background bit pattern
	rcall	CLOCK
	dec	temp
	brne	NEXT_BYTE
	add	column,char_width
	cp	column,right
	brlo	NEXT_COLUMN_CLEAR
	pop	column
	ret


;-------------------------------------------------------------------------------------------

;*********	Routines for data transfer to display etc.

SEND_INSTRUCTION:		;set lines for an instruction
	cbi	PORTD,data
	cbi	PORTD,read		;write
	ret

;-------------------------------------------------------------------------------------------


SEND_DATA:			;set lines for data
	sbi	PORTD,data
	cbi	PORTD,read		;write
	ret

;-------------------------------------------------------------------------------------------


FETCH_DATA:			;set lines for data fetch
	sbi	PORTD,data
	sbi	PORTD,read		;read
	ret

;-------------------------------------------------------------------------------------------


DISPLAY_ON:			;activate display
	rcall TWO_SIDE
	rcall	SEND_INSTRUCTION
	ldi	temp,0x3F
	out	PORTC,temp
	rcall CLOCK
	ret

;-------------------------------------------------------------------------------------------


DISPLAY_OFF:		;disable display
	rcall TWO_SIDE
	rcall	SEND_INSTRUCTION
	ldi	temp,0x3E
	out	PORTC,temp
	rcall CLOCK
	ret

;-------------------------------------------------------------------------------------------


TWO_SIDE:		;address both chips

	cbi	PORTD,cs2
	cbi	PORTD,cs1
	ret

;-------------------------------------------------------------------------------------------


NO_SIDE:		;don't address either chip

	sbi	PORTD,cs2
	sbi	PORTD,cs1
	ret

;-------------------------------------------------------------------------------------------


LEFT_SIDE:			;address left side chip

	sbi	PORTD,cs2
	cbi	PORTD,cs1
	ret

;-------------------------------------------------------------------------------------------


RIGHT_SIDE:			;address right side chip

	sbi	PORTD,cs1
	cbi	PORTD,cs2
	ret

;-------------------------------------------------------------------------------------------


CLOCK:		;clock display data

	sbi	PORTD,enable ;this many nops needed for reliable clocking at 8 MHz
	nop
	nop			;perhaps I could put some sort of task in here?
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	cbi	PORTD,enable
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	ret

;-------------------------------------------------------------------------------------------


;**********	More primitives

TAB:				;go to current tab position
	push	temp
	rcall SEND_INSTRUCTION
	rcall SET_row
	rcall SET_column
	rcall	SEND_DATA
	pop	temp
	ret 

;-------------------------------------------------------------------------------------------


SET_row:			;sets row from 0 - 15
	rcall TWO_SIDE
	mov	temp,scroll		;create a copy (we must allow for scrolling)
	andi	temp,~0xC0		;mask off top bits
	asr	temp
	asr	temp
	asr	temp			;divide by eight - temp is top row
	add	temp,row
	andi	temp,0x07		;keep in scope - reduce modulo height
	ori	temp,0xB8		;create instruction		
	out	PORTC,temp
	rcall	CLOCK
	ret

;-------------------------------------------------------------------------------------------


SET_column:			;sets column from 0 to 127
	rcall RIGHT_SIDE
	mov	temp,column		;make a copy
	sbrs	temp,6
	rcall	LEFT_SIDE
	andi	temp,0x3F
	ori	temp,0x40		;create instruction
	out	PORTC,temp
	rcall	CLOCK
	ret


;-------------------------------------------------------------------------------------------


OSWRCH:				;character output routine

	sbrc	flags,graphics
	rjmp	GRAPHIC_CHAR_OUT		;character at graphics cursor
	
	sbrc	flags,pending		;Is scroll pending?
	rcall	PROCESS_SCROLL
	rcall TAB

	lds	temp2,pattern		;Load background pattern for use as a mask

	rcall	GET_CHARACTER_ADDRESS

	mov	temp,char_width		;char width columns per char
NEXT_SLICE:
	lpm	temp1,Z+		
	eor	temp1,temp2			;mask with background pattern byte
	out	PORTC,temp1
	rcall	CLOCK
	cpi	pointer,0x10		;we can't do cpi on char_width directly
	brne	NORMAL_CHARS
	out	PORTC,temp1			;double width, output twice
	rcall	CLOCK
	dec	temp
NORMAL_CHARS:
	dec	temp
	brne	NEXT_SLICE		;now output next column

	add	column,char_width	; Now advance column count 
	cp	column,right
	brlo	OSWRCH_OUT		; not last column, so finish
	mov	column,left		; go to left column
	inc	row
	cp	row,bottom		;bottom left character cell?
	brne	OSWRCH_OUT		;No!
	rjmp	PREPARE_PENDING
OSWRCH_OUT:
	ret


;-------------------------------------------------------------------------------------------


GRAPHIC_CHAR_OUT:			;display character at current graphic cursor

	push	row

	clr	pointer
	rcall	SET_GRAPHIC_TEXT_COLOUR
	tst	pointer
	breq	SKIP_37
	rjmp	RS232_DONE
SKIP_37:

	ldi	ZL,low(current)	
	ldi	ZH,high(current)	;get current point
	call	LOAD

	rcall	GET_CHARACTER_ADDRESS

	mov	pointer,char_width
	mov	row,char_width	;char width columns per character
NEXT_GRAPHIC_SLICE:
	lpm	char,Z+		;get first character slice	
	
	cpi	pointer,0x10
	brne	SKIP_64
	rcall	OUTPUT_GRAPHIC_SLICE
	inc	temp			;next column
	brne	SKIP_12
	inc	temp1
SKIP_12:
	dec	row
SKIP_64:
	rcall	OUTPUT_GRAPHIC_SLICE
	inc	temp			;next column
	brne	SKIP_14
	inc	temp1
SKIP_14:
	dec	row
	brne	NEXT_GRAPHIC_SLICE
	
GRAPHIC_CHAR_DONE:
	pop	row
	rcall	GCURS_RIGHT		;move new to current etc.
	ret

;-------------------------------------------------------------------------------------------


;User defined verion:

GRAPHIC_USER_OUT:			;display character at current graphic cursor

	push	row

	clr	pointer
	rcall	SET_GRAPHIC_TEXT_COLOUR
	tst	pointer
	breq	SKIP_36
	rjmp	RS232_DONE
SKIP_36:

	ldi	ZL,low(current)	
	ldi	ZH,high(current)	;get current point
	call	LOAD

	rcall	GET_USER_ADDRESS

	mov	pointer,char_width
	mov	row,char_width	;char width columns per character
NEXT_USER_SLICE:
	ld	char,Y+		;get first character slice	
	
	cpi	pointer,0x10
	brne	SKIP_34
	rcall	OUTPUT_GRAPHIC_SLICE
	inc	temp			;next column
	brne	SKIP_33
	inc	temp1
SKIP_33:
	dec	row
SKIP_34:
	rcall	OUTPUT_GRAPHIC_SLICE
	inc	temp			;next column
	brne	SKIP_35
	inc	temp1
SKIP_35:
	dec	row
	brne	NEXT_USER_SLICE
	
	pop	row
	rcall	GCURS_RIGHT		;move new to current etc.
	rjmp	RS232_DONE

GET_USER_ADDRESS:

	andi	char,~0xE0		;offset from base (0-31)
	lsl	char
	lsl	char
	lsl	char			;multiply by eight bytes wide
	ldi	YL,low(user_chars)
	ldi	YH,high(user_chars)
	add	YL,char
	brcc	SKIP_03
	inc	YH
	SKIP_03:

	ret

;-------------------------------------------------------------------------------------------


SET_GRAPHIC_TEXT_COLOUR:
lds	temp,fgcol
lds	temp1,fgact			;load foreground colours and settings

tst	temp
brne	T_BLACK_CHOICES		;Colour is black, now check options

cpi	temp1,0x00			;go through clear options
breq	T_PLOT_CLEAR
cpi	temp1,0x02
breq	T_PLOT_CLEAR
cpi	temp1,0x04
breq	T_PLOT_INVERTED
ldi	pointer,0xFF
ret					;must be null option

T_BLACK_CHOICES:

cpi	temp1,0x00			;go through clear options
breq	T_PLOT_BLACK
cpi	temp1,0x01
breq	T_PLOT_BLACK
cpi	temp1,0x02
brne	T_PLOT_INVERTED		;must be inverted
ldi	pointer,0xFF
ret					;must be null option
	
;so after all that endless faff, we have three practical plot options:
T_PLOT_INVERTED:
sbr	colflags,(1<<plot_invert)
cbr	colflags,(1<<clear)|(1<<black)
ret

T_PLOT_CLEAR:
cbr	colflags,(1<<plot_invert)|(1<<black)
sbr	colflags,(1<<clear)
ret

T_PLOT_BLACK:
cbr	colflags,(1<<plot_invert)|(1<<clear)
sbr	colflags,(1<<black)
ret

;-------------------------------------------------------------------------------------------


;********	Plots a slice of a character at the graphic cursor

OUTPUT_GRAPHIC_SLICE:
	
	push	row
	push	pointer
	push	char
	push	temp2
	push	temp3

	ldi	row,0x08
OUTPUT_GRAPHIC_DOT:
	ror	char	
	brcc	NO_DOT_TO_PLOT
		
	call WIND				;window.
	brne	NO_DOT_TO_PLOT

	call	GADDR				;address the point

	lds	pixel_mask,ZMASK		;load pixel mask

	call	PLBYTE			;and plot it!!!

	NO_DOT_TO_PLOT:
	dec	temp2
	brne	SKIP_11
	dec	temp3
SKIP_11:
	dec	row
	brne	OUTPUT_GRAPHIC_DOT

	pop	temp3
	pop	temp2
	pop	char
	pop	pointer
	pop	row
	ret

;-------------------------------------------------------------------------------------------


;********	For movement of the graphic cursor

GCURS_RIGHT:

	ldi	pointer,0x00
	mov	char,char_width
	lds	temp1,graphl
	lds	temp,graphr

	lds	temp2,current+2
	sts	old+2,temp2
	lds	temp2,current+3
	sts	old+3,temp2

	lds	temp2,current
	sts	old,temp2
	add	temp2,char

	lds	temp3,current+1
	sts	old+1,temp3
	adc	temp3,pointer

	tst	temp3
	brne	SKIP_19
	cp	temp2,temp
	brlo	SKIP_20
SKIP_19:
	clr	temp3
	mov	char,temp1			;set to bottom+7
	sts	current,char
	sts	current+1,temp3
	
	rcall	TIMES_16
	sts	current_ext,char
	sts	current_ext+1,temp3	;external graphics co-ordinates

	rcall	GCURS_DOWN	

	ret

SKIP_20:
	sts	current,temp2
	sts	current+1,temp3

	rcall	TIMES_16

	lds	temp,current_ext
	add	temp,char
	sts	current_ext,temp
	lds	temp,current_ext+1
	adc	temp1,temp3
	sts	current_ext+1,temp	;external graphics co-ordinates

	ret

GCURS_LEFT:

	ldi	pointer,0x00
	mov	char,char_width
	lds	temp1,graphl
	subi	temp1,0x07			;can be up to 7 pixels off the left
	lds	temp,graphr
	subi	temp,0x07

	lds	temp2,current+2
	sts	old,temp2
	lds	temp2,current+3
	sts	old+1,temp2

	lds	temp2,current
	sts	old,temp2
	sub	temp2,char

	lds	temp3,current+1
	sts	old+1,temp3
	sbc	temp3,pointer

	tst	temp3
	brne	SKIP_21
	cp	temp2,temp1
	brpl	SKIP_24
SKIP_21:
	clr	temp3
	mov	char,temp			;set to top
	sts	current,char
	sts	current+1,temp3
	
	rcall	TIMES_16
	sts	current_ext,char
	sts	current_ext+1,temp3	;external graphics co-ordinates

	rcall	GCURS_UP

	ret

SKIP_24:
	sts	current,temp2
	sts	current+1,temp3

	rcall	TIMES_16

	lds	temp,current_ext
	sub	temp,char
	sts	current_ext,temp
	lds	temp,current_ext+1
	sbc	temp1,temp3
	sts	current_ext+1,temp	;external graphics co-ordinates

	ret

GCURS_UP:

	ldi	pointer,0x00
	ldi	char,0x08
	lds	temp1,graphb
	subi	temp1,~0x06
	lds	temp,grapht
	subi	temp,~0x07			;can be up to 7 pixels off the top

	lds	temp2,current
	sts	old,temp2
	lds	temp2,current+1
	sts	old+1,temp2

	lds	temp2,current+2
	sts	old+2,temp2
	add	temp2,char

	lds	temp3,current+3
	sts	old+3,temp3
	adc	temp3,pointer

	tst	temp3
	brne	SKIP_17
	cp	temp2,temp
	brlo	SKIP_18
SKIP_17:
	clr	temp3
	mov	char,temp1			;set to bottom+7
	sts	current+2,char
	sts	current+3,temp3
	
	rcall	TIMES_16
	sts	current_ext+2,char
	sts	current_ext+3,temp3	;external graphics co-ordinates
	ret

SKIP_18:
	sts	current+2,temp2
	sts	current+3,temp3

	rcall	TIMES_16

	lds	temp,current_ext+2
	add	temp,char
	sts	current_ext+2,temp
	lds	temp,current_ext+3
	adc	temp1,temp3
	sts	current_ext+3,temp	;external graphics co-ordinates

	ret

GCURS_DOWN:

	ldi	pointer,0x00
	ldi	char,0x08
	lds	temp1,graphb
	lds	temp,grapht

	lds	temp2,current
	sts	old,temp2
	lds	temp2,current+1
	sts	old+1,temp2

	lds	temp2,current+2
	sts	old+2,temp2
	sub	temp2,char

	lds	temp3,current+3
	sts	old+3,temp3
	sbc	temp3,pointer

	tst	temp3
	brne	SKIP_15
	cp	temp2,temp1
	brpl	SKIP_16
SKIP_15:
	clr	temp3
	mov	char,temp			;set to top
	sts	current+2,char
	sts	current+3,temp3
	
	rcall	TIMES_16
	sts	current_ext+2,char
	sts	current_ext+3,temp3	;external graphics co-ordinates
	ret

SKIP_16:
	sts	current+2,temp2
	sts	current+3,temp3

	rcall	TIMES_16

	lds	temp,current_ext+2
	sub	temp,char
	sts	current_ext+2,temp
	lds	temp,current_ext+3
	sbc	temp1,temp3
	sts	current_ext+3,temp	;external graphics co-ordinates

	ret

;-------------------------------------------------------------------------------------------

;Useful fixed multiplication routine
TIMES_16:
	clr	temp3
	lsl	char
	rol	temp3
	lsl	char
	rol	temp3
	lsl	char
	rol	temp3
	lsl	char
	rol	temp3				;multiply by 16

ret

;-------------------------------------------------------------------------------------------


;********	Return to left

GCURS_RET:			;returns cursor to LHS of graphic window

	ldi	pointer,0x00
	lds	char,graphl			;NB: graphl, not  graphr

	lds	temp2,current
	sts	old,temp2
	sts	current,char
	lds	temp2,current+1
	sts	old+1,temp2
	sts	current+1,pointer
	lds	temp2,current+2
	sts	old+2,temp2
	lds	temp2,current+3
	sts	old+3,temp2	

	swap	char			;high byte is 0 so a swap = *16

	lsl	char
	rol	pointer
	lsl	char
	rol	pointer
	lsl	char
	rol	pointer
	lsl	char
	rol	pointer		;multiply by 16

	lds	temp,horigin
	lds	temp1,horigin+1
	add	char,temp
	adc	pointer,temp1

	sts	current_ext,char
	sts	current_ext+1,pointer	;so we have moved X-coord right

ret

;-------------------------------------------------------------------------------------------


;********	Used for fetching a character address from SRAM

GET_CHARACTER_ADDRESS:
	push	temp
	push	char
	mov	pointer,char_width
	ldi	temp,0x04
	cp	char_width,temp
	breq	NARROW_CHARS
;This clever bit multiplies char by 8 and adds it to the Z register
;note snazzy trick that takes advantage of ZL=0
	ldi	ZH,0x37
	mov	ZL,char
	clr	temp
	lsl	ZL
	rol	temp
	lsl	ZL
	rol	temp
	lsl	ZL
	rol	temp			; multiply by 8
	add	ZH,temp		;and put extras into high byte

	rjmp	DONE_CHARACTER_ADDRESS


NARROW_CHARS:			;use a different character table
;This clever bit multiplies char by 4 and adds it to the Z register
;note snazzy trick that takes advantage of ZL=0
	ldi	ZH,0x3B
	mov	ZL,char
	clr	temp
	lsl	ZL
	rol	temp
	lsl	ZL
	rol	temp			; multiply by 8
	add	ZH,temp		;and put extras into high byte

DONE_CHARACTER_ADDRESS:
	pop	char
	pop	temp
	ret

;-------------------------------------------------------------------------------------------


;********	Scroll Processing



PREPARE_PENDING:			;if we have just written bottom right
	mov	column,right
	sub	column,char_width	;temporary leave column as it is
	dec	row			;and also leave row as was
	sbr	flags,(1<<pending); A scroll is pending
	ret

;-------------------------------------------------------------------------------------------


PROCESS_SCROLL:		;do this to process a pending scroll
	cbr	flags,(1<<pending);clear flag
	mov	column,left		;left column

SCROLL_NOW:				;chooses full or window scroll			
	sbrc	flags,windowed	;can we do full screen scrolls?
	rjmp	SLOW_SCROLL_DOWN	;no do window scroll
	ldi	temp,0x08
	add	scroll,temp		;scroll screen eight lines
	rcall	SCROLL_DISPLAY	;implements the scroll
	ret

SLOW_SCROLL_DOWN:			;scrolls a text window one line
	push	column
	push	row

	mov	row,top
	mov	column,left			;top left

SCROLL_NEXT_CHAR:
	inc	row				;move down one
	cp	row,bottom			;off the bottom?
	breq	SKIP_30			;done
	rcall	TAB				;no

	rcall	COPY_CHARACTER

	dec	row				;back up a row to paste
	rcall	TAB

	rcall	PASTE_CHARACTER

	add	column,char_width		;move along a character
	cp	column,right
	brne	SCROLL_NEXT_CHAR
	mov	column,left
	inc	row
	rjmp	SCROLL_NEXT_CHAR

SKIP_30:

	pop	row
	pop	column
	rcall	CLEAR_LINE
	ret

;-------------------------------------------------------------------------------------------


SLOW_SCROLL_UP:			;scrolls a text window one line
	push	column
	push	row

	mov	row,bottom
	dec	row
	mov	column,left			;bottom left

SCROLL_UP_NEXT_CHAR:
	cp	row,top			;reached top row?
	breq	SKIP_31			;yes,done
	dec	row				;move up one
	rcall	TAB				;no

	rcall	COPY_CHARACTER

	inc	row				;back down a row to paste
	rcall	TAB

	rcall	PASTE_CHARACTER

	add	column,char_width		;move along a character
	cp	column,right
	brne	SCROLL_UP_NEXT_CHAR
	mov	column,left
	dec	row
	rjmp	SCROLL_UP_NEXT_CHAR

SKIP_31:

	pop	row
	pop	column
	rcall	CLEAR_LINE
	ret

;-------------------------------------------------------------------------------------------


COPY_CHARACTER:				;copy character at cursor to table at char_table

	ldi	YH,high(char_table)
	ldi	YL,low(char_table)
	mov	pointer,char_width		;number of bytes to fill
	clr	temp2
	out	DDRC,temp2
	out	PORTC,temp2
	rcall FETCH_DATA
	rcall CLOCK
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
NEXT_BiTE:
	sbi	PORTD,enable
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	in	display_byte,PINC
	cbi	PORTD,enable
	st	Y+,display_byte
	dec	pointer
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	brne	NEXT_BiTE

	ser	temp2
	out	DDRC,temp2

	ret


;-------------------------------------------------------------------------------------------


PASTE_CHARACTER:				;paste the character held at char_table

	ldi	YH,high(char_table)
	ldi	YL,low(char_table)
	mov	pointer,char_width		;blank a char - number of bytes to fill

	rcall SEND_DATA
NEXT_BLOTE:
	ld	display_byte,Y+
	out	PORTC,display_byte
	rcall CLOCK
	dec	pointer
	brne	NEXT_BLOTE
	ret

;-------------------------------------------------------------------------------------------


SCROLL_UP:			;choose full or window scroll
	sbrc	flags,windowed	;can we do full screen scrolls?
	rjmp	SLOW_SCROLL_UP	;no do window scroll
	ldi	temp,0x08
	sub	scroll,temp		;scroll screen eight lines
	rcall	SCROLL_DISPLAY	;implements the scroll
	ret

;-------------------------------------------------------------------------------------------


FLASH_CURSOR:		;flashes the cursor by copying and eoring character with cursor
	push	YL
	push	YH
	push pointer
	push	temp2
	push	display_byte

	ldi	temp,(1<<cursor)		;drawing cursor sets flag, next time clears it
	eor	flags,temp
	cbr	more_flags,(1<<flash_state)

	rcall	TAB
	rcall	COPY_CHARACTER

	ldi	YH,high(char_table)
	ldi	YL,low(char_table)
	rcall	TAB
	mov	pointer,char_width		;blank a char - number of bytes to fill
	rcall SEND_DATA


NEXT_BLYTE:
	ld	display_byte,Y+
	ldi	temp2,0xFF				;block cursor
	sbrs	flags,pending			;use if a pending scroll
	lds	temp2,cursor_pattern		;normal cursor
	eor	display_byte,temp2
	out	PORTC,display_byte
	rcall CLOCK
	dec	pointer
	brne	NEXT_BLYTE

	pop	display_byte
	pop	temp2
	pop	pointer
	pop	YH
	pop	YL

	ret


;-------------------------------------------------------------------------------------------


;*********	Interrupt Routines

EXT_INT0: ; IRQ0 Handler

reti

;-------------------------------------------------------------------------------------------


EXT_INT1: ; IRQ1 Handler

reti

;-------------------------------------------------------------------------------------------


TIM2_COMP: ; Timer2 Compare Handler

reti

;-------------------------------------------------------------------------------------------


TIM2_OVF: ; Timer2 Overflow Handler

;used to time keyboard clicks, set for about 1 ms

	push	temp
	in	temp,SREG
	push	temp

;When a start bit is recieved, keyboard_time is set to 3
;if it counts down past 0, then must be a timeout

;decrement keyboard timeout
	lds	temp,keyboard_time
	dec	temp
	cpi	temp,0xFF
	brne	KEYTIME_DONE
	clr	temp			
	sts	keyboard_bit,temp	;set keyboard bit to zero

KEYTIME_DONE:

	pop	temp
	out	SREG,temp
	pop	temp
	reti

;-------------------------------------------------------------------------------------------


TIM1_CAPT: ; Timer1 Capture Handler

reti

;-------------------------------------------------------------------------------------------

TIM1_COMPA: ; Timer1 CompareA Handler

reti

;-------------------------------------------------------------------------------------------


TIM1_COMPB: ; Timer1 CompareB Handler

reti

;-------------------------------------------------------------------------------------------


TIM1_OVF: ; Timer1 Overflow Handler

	cli

	out	TCNT1H,timer1h
	out	TCNT1L,timer1l	;reset timer
	sbr	more_flags,(1<<flash_state)
	sei
	reti

;-------------------------------------------------------------------------------------------


TIM0_OVF: ; Timer0 Overflow Handler
	cli
;load for	0.01s 
	push	temp
	in	temp,SREG
	push	temp
	ldi	temp,0xB1
	out	TCNT0,temp
	sei				;The rest is not time critical, so allow it to be interrupted
					;so this routine doesn't mess up keyboard input

	lds	temp,time
	inc	temp
	sts	time,temp
	brne	DONE_CLICK
	lds	temp,time+1
	inc	temp
	sts	time+1,temp
	brne	DONE_CLICK
	lds	temp,time+1
	inc	temp
	sts	time+2,temp
	brne	DONE_CLICK
	lds	temp,time+1
	inc	temp
	sts	time+3,temp
	brne	DONE_CLICK
	lds	temp,time+1
	inc	temp
	sts	time+4,temp

DONE_CLICK:


	pop	temp
	out	SREG,temp
	pop	temp
	reti

;-------------------------------------------------------------------------------------------


SPI_STC: ; SPI Transfer Complete Handler

;If we are here we have recieved a byte from SPI

	push	YH
	push	YL			;save Y register
	in	YL,SREG
	push	YL			;save status register
	push	char			;save char
	in	char,SPDR		;get byte

	ldi	YH,HIGH(buffer)
	mov	YL,b_pointer

	st	Y,char		;save it
	inc	b_pointer		;increment pointer


	pop	char
	pop	YL
	out	SREG,YL
	set				;set T flag after restoring status register
	pop	YL
	pop	YH

reti
;-------------------------------------------------------------------------------------------


USART_RXC: ; USART RX Complete Handler

;If we are here we have recieved a byte from RS232
cli

	push	YH
	push	YL			;save Y register
	in	YL,SREG
	push	YL			;save status register
	push	char			;save char
	in	char,UDR		;get byte

	ldi	YH,HIGH(buffer)
	mov	YL,b_pointer

	st	Y,char		;save it
	inc	b_pointer		;increment pointer
	
	sub	YL,i_pointer	;how full is buffer
	cpi	YL,0xCF		;16 bytes left
	brlo	SKIP_39

;Buffer full routine in 'ere
	cbi	PORTC,CTS
	rjmp	SKIP_44
SKIP_39:
	sbi	PORTC,CTS
	
SKIP_44:
	pop	char
	pop	YL
	out	SREG,YL
	pop	YL
	pop	YH

sei
reti

;-------------------------------------------------------------------------------------------


PROCESS_BUFFER:
	
	push	YH
	push	YL
	ldi	YH,HIGH(buffer)
	mov	YL,i_pointer	;load address
	ld	char,Y		;get byte
	pop	YL
	pop	YH
	rcall	OTHER_WAY_IN
	inc	i_pointer		;increment pointer
ret


;-------------------------------------------------------------------------------------------


OTHER_WAY_IN:

push	temp
push	temp1			;so we can use them
sbrc	flags,screen_off
rjmp	VDU_CHECK

sbrc	flags,dotted
jmp	BITMAP

clr	temp
cp	vdu_queue,temp	;are we filling the queue?
breq	NO_QUEUE
rjmp	PROCESS_QUEUE	;yes, process it

NO_QUEUE:

sbrc	flags,cursor
rcall	FLASH_CURSOR		;if drawn, go to flash and clear it.

cpi	char,0x7F
breq	BS_DELETE1		;backspace and delete

cpi	char,0x13
breq	BITMAP1		;prepare for bitmap

cpi	char,0x20		;control code?		
brlo	CONTROL1

cpi	char,0x90		; upper page (user characters)
brlo	SKIP_32
rjmp	USER_DEF
SKIP_32:
rcall	OSWRCH

RS232_DONE:
pop	temp1
pop	temp				;restore temp registers
ret

CONTROL1:
jmp	CONTROL

BS_DELETE1:				;This is a table of vectors to cope with limitations of breq
rjmp	BS_DELETE

BITMAP1:
sbr	flags,(1<<dotted)
ldi	temp3,0x00
ldi	pageY,0x00			;bottom left of bitmap
clr	temp
rcall	SEND_INSTRUCTION
rcall	SET_PAGE				;sets page to pagey (0-7)
rcall	SET_BYTE				;sets byte to temp (0-127)
rjmp	RS232_DONE

BACKSPACE:
sbrc	flags,graphics
jmp	GCURS_LEFT			;graphics cursor implementation
sbrs	flags,pending
rjmp	SKIP_23
cbr	flags,(1<<pending)
ret
SKIP_23:
cp	column,left
breq	SKIP_22
sub	column,char_width		;as in tab left
ret
SKIP_22:
mov	column,right
sub	column,char_width		;right margin and in one char
cp	row,top
brne	NOT_TOP_ROW
rjmp	SCROLL_UP
NOT_TOP_ROW:
dec	row
ret

BS_DELETE:
rcall BACKSPACE		;then delete

DELETE:	
sbrc	flags,graphics
rjmp	RS232_DONE			;no delete if at graphics cursor
rcall	TAB
rcall	SEND_DATA
mov	temp,char_width		;blank a char - number of bytes to fill
lds	temp1,pattern
NEXT_BOTE:
out	PORTC,temp1
rcall	CLOCK
dec	temp
brne	NEXT_BOTE
rjmp	RS232_DONE

VDU_CHECK:

cpi	char,0x06		;is it a screen on command?
brne	VDU_OFF		;no, ignore
cbr	flags,(1<<screen_off)	;yes, screen back on
VDU_OFF:
;test for non VDU activity here
rjmp	RS232_DONE		


RST_WINDOWS:
	
	clr	left
	clr	top				;home cursor
	ldi	temp,0x80
	mov	right,temp
	ldi	temp,0x08
	mov	bottom,temp			;bottom +1
	mov	column,left
	mov	row,top
	cbr	flags,(1<<windowed)	;set default text window

	clr	temp
	sts	current,temp
	sts	current+1,temp
	sts	current+2,temp
	sts	current+3,temp		;reset current graphics cursor
	sts	current_ext,temp
	sts	current_ext+1,temp
	sts	current_ext+2,temp
	sts	current_ext+3,temp	;and copy in external co-ordinates
	sts	old,temp
	sts	old+1,temp
	sts	old+2,temp
	sts	old+3,temp		;reset old graphics cursor
	sts	horigin,temp
	sts	horigin+1,temp
	sts	vorigin,temp
	sts	vorigin+1,temp		;set graphics origin to 0,0 (external)
	sts	graphl,temp
	sts	graphb,temp
	ldi	temp,0x7F
	sts	graphr,temp
	ldi	temp,0x3F
	sts	grapht,temp			;set default graphic window
	
ret

;*********  Back to interrupt Handlers

USART_UDRE: ; UDR Empty Handler

reti

USART_TXC: ; USART TX Complete Handler

reti

ADCC: ; ADC Conversion Complete Handler

reti


EE_RDY: ; EEPROM Ready Handler

reti

ANA_COMP: ; Analog Comparator Handler

reti

TWSI: ; Two-wire Serial Interface Handler

reti

;EXT_INT2: ; IRQ2 Handler USE VERSION IN keyboard.inc

;reti

TIM0_COMP: ; Timer0 Compare Handler

reti

SPM_RDY: ; Store Program Memory Ready Handler

reti

;============================

SET_UP_QUEUE:		;start filling a VDU Queue; length in vdu_queue

	ldi	YH,high(vduque)
	ldi	YL,low(vduque)	;pointer to VDU queue in SRAM
	st	Y+,char		;place command at head of queue

rjmp	RS232_DONE			;and finish

PROCESS_QUEUE:		;multi-byte VDU routine
	st	Y+,char		;place next data byte in queue
	dec	vdu_queue		;reduce count
	brne	FINISH_PROCESS		;still some data to fetch

;if here we have a full queue so:

sbrc	flags,cursor
rcall	FLASH_CURSOR		;if drawn, go to flash and clear it.

	ldi	YH,high(vduque)
	ldi	YL,low(vduque)	;pointer to VDU queue in SRAM
	ld	char,Y+		;fetch command from head of queue

;check commands to find which to process
cpi	char,0x11
breq	PROCESS_TEXT_COLOUR	;text colour
cpi	char,0x12
breq	PROCESS_GRAPHIC_COLOUR1
cpi	char,0x1F
breq	PROCESS_TAB			;is a tab to command
cpi	char,0x16
breq	PROCESS_MODE
cpi	char,0x17
breq	PROCESS_DEFINE1		;define user character
cpi	char,0x18
breq	PROCESS_GRAPHIC_WINDOW1
cpi	char,0x19
breq	PROCESS_PLOT1
cpi	char,0x1B
breq	FX_DECODE1			;A *FX code has been received
cpi	char,0x1C
breq	PROCESS_TEXT_WINDOW1	;text window
cpi	char,0x1D
breq	PROCESS_GRAPHIC_ORIGIN1
FINISH_PROCESS:
rjmp	RS232_DONE

PROCESS_GRAPHIC_COLOUR1:
rjmp	PROCESS_GRAPHIC_COLOUR

PROCESS_GRAPHIC_WINDOW1:
rjmp	PROCESS_GRAPHIC_WINDOW

PROCESS_GRAPHIC_ORIGIN1:
rjmp	PROCESS_GRAPHIC_ORIGIN

PROCESS_TEXT_WINDOW1:
rjmp	PROCESS_TEXT_WINDOW

PROCESS_PLOT1:
rjmp	PROCESS_PLOT

FX_DECODE1:
rjmp	FX_DECODE

PROCESS_DEFINE1:
rjmp	PROCESS_DEFINE

PROCESS_TEXT_COLOUR:
ld	temp,Y+
sbrs	temp,0
rjmp	BLACK_BACK
clr	temp1
sts	pattern,temp1
rjmp	RS232_DONE
BLACK_BACK:
ser	temp1
sts	pattern,temp1
rjmp	RS232_DONE

PROCESS_MODE:

	clr	temp
	sts	pattern,temp		;CLEAR BACKGROUND, BLACK TEXT
	rcall	RST_WINDOWS
	rcall	CLEAR_SCREEN			

	ld	temp1,Y+
	andi	temp1,0x07			;reduce modulo 8
	sts	mode,temp1			;save a copy
	ldi	temp,0x07
	cpi	temp1,2
	breq	MODE_2
	cpi	temp1,5
	breq	MODE_2
	cpi	temp1,3
	breq	MODE_0
	and	temp1,temp
	breq	MODE_0
	ldi	temp1,0x08
	mov	char_width,temp1
	rjmp	RS232_DONE
MODE_0:
	ldi	temp1,0x04
	mov	char_width,temp1
	rjmp	RS232_DONE
MODE_2:
	ldi	temp1,0x10
	mov	char_width,temp1
	rjmp	RS232_DONE

PROCESS_TAB:
	push	row
	push	column		;in case it's a duff command
	ld	column,Y+
	ld	row,Y+
	mul	column,char_width
	tst	r1
	brne	DUFF_TAB
	mov	column,r0		;convert to pixels
	add	column,left
	cp	column,right
	brsh	DUFF_TAB

	cp	row,top
	brlo	DUFF_TAB
	cp	row,bottom
	brsh	DUFF_TAB
	rcall TAB
	pop temp
	pop temp			;to keep the stack in order
rjmp	FINISH_PROCESS

DUFF_TAB:
	pop	column
	pop	row			;out of window, restore
	rjmp	FINISH_PROCESS
	
PROCESS_TEXT_WINDOW:
	push	left
	push	bottom
	push	right
	push	top

	ld	left,Y+
	ld	bottom,Y+
	ld	right,Y+
	ld	top,Y+

	inc	right
	inc	bottom

	mul	left,char_width
	mov	left,r0

	mul	right,char_width
	mov	right,r0

	ldi	temp,0x81
	cp	right,temp
	brsh	WINDOW_DUFF
	cp	left,right
	brsh	WINDOW_DUFF

	ldi	temp,0x09
	cp	bottom,temp
	brsh	WINDOW_DUFF
	cp	top,bottom
	brsh	WINDOW_DUFF

	sbr	flags,(1<<windowed)
	pop	temp				;keep stack in order
	pop	temp
	pop	temp
	pop	temp
	rjmp	RS

WINDOW_DUFF:
	pop	top				;restore old values
	pop	right
	pop	bottom
	pop	left
	rjmp	FINISH_PROCESS

PROCESS_DEFINE:

	ld	char,Y+		;get user define character number
	andi	char,~0xE0		;offset from base (0-31)
	lsl	char
	lsl	char
	lsl	char			;multiply by eight bytes wide
	ldi	XL,low(user_chars)
	ldi	XH,high(user_chars)
	add	XL,char
	brcc	SKIP_01
	inc	XH
	SKIP_01:

	ldi	temp,0x08		;eight bytes wide
SKIP_02:
	ld	char,Y+
	st	X+,char		;copy to character space
	dec	temp
	brne	SKIP_02

rjmp	FINISH_PROCESS

USER_DEF:


	sbrc	flags,graphics
	rjmp	GRAPHIC_USER_OUT		;character at graphics cursor

sbrc	flags,pending		;Is scroll pending?
rcall	PROCESS_SCROLL
rcall TAB
mov	pointer,char_width
rcall SEND_DATA

rcall	GET_USER_ADDRESS

	mov	temp,char_width		;eight bytes wide
SKIP_04:
	ld	temp1,Y+		;get character slice		
out	PORTC,temp1
rcall	CLOCK
cpi	pointer,0x10
brne	NORMAL_USER_CHARS
out	PORTC,temp1
rcall	CLOCK
dec	temp
NORMAL_USER_CHARS:
dec	temp
brne	SKIP_04		;now output next column

add	column,char_width		; Now advance column count 
cp	column,right
brlo	USERDEF_OUT		; not last column, so finish
mov	column,left		; go to left column
inc	row
cp	row,bottom		;will we wrap round?
brne	USERDEF_OUT	
rcall PREPARE_PENDING	;Yes, so prepare pending scroll
USERDEF_OUT:
rjmp	RS232_DONE

;*********


PROCESS_GRAPHIC_COLOUR:
ld	temp,Y+
cpi	temp,0x05
brsh	GCOL_DONE			;out of range, abort
ld	temp1,Y+
sbrc	temp1,7			;is it background?
rjmp	BACKGROUND_COLOUR
andi	temp1,0x01			;no
sts	fgcol,temp1
sts	fgact,temp
GCOL_DONE:
rjmp	RS232_DONE

BACKGROUND_COLOUR:
andi	temp1,0x01
sts	bgcol,temp1
sts	bgact,temp
rjmp	RS232_DONE


PROCESS_GRAPHIC_ORIGIN:
;NB in external co-ordinates to avoid rounding errors
ld	temp,Y+
ld	temp1,Y+			;X-co-ordinates, low byte first.
sts	horigin,temp
sts	horigin+1,temp1

ld	temp,Y+
ld	temp1,Y+			;Y-co-ordinates, low byte first.
sts	vorigin,temp
sts	vorigin+1,temp1

rjmp	RS232_DONE

PROCESS_GRAPHIC_WINDOW:
ld	temp,Y+
ld	temp1,Y+			;left, low byte first.
rcall	CONVERT_TO_PIXELS
tst	temp1
breq	SKIP_50			;must be clear if valid
clr	temp1
clr	temp
SKIP_50:
cpi	temp,0x7F
brlo	SKIP_51			;must be less than 127
clr	temp
SKIP_51:
sts	graphl,temp

ld	temp,Y+
ld	temp1,Y+			;bottom, low byte first.
rcall	CONVERT_TO_PIXELS
tst	temp1
breq	SKIP_52			;must be clear if valid
clr	temp1
clr	temp
SKIP_52:
cpi	temp,0x3F
brlo	SKIP_53			;must be less than 63
clr	temp
SKIP_53:
sts	graphb,temp

ld	temp,Y+
ld	temp1,Y+			;right, low byte first.
rcall	CONVERT_TO_PIXELS
tst	temp1
breq	SKIP_54			;must be clear if valid
clr	temp1
clr	temp
SKIP_54:
cpi	temp,0x80
brlo	SKIP_55			;must be no more than 128
ldi	temp,0x7F
SKIP_55:
lds	temp2,graphl
cp	temp,temp2
brsh	SKIP_56			;must be more than left
ldi	temp,0x7F	
SKIP_56:
sts	graphr,temp

ld	temp,Y+
ld	temp1,Y+			;top, low byte first.
rcall	CONVERT_TO_PIXELS
tst	temp1
breq	SKIP_57			;must be clear if valid
clr	temp1
clr	temp
SKIP_57:
cpi	temp,0x40
brlo	SKIP_58			;must be no more than 64
ldi	temp,0x3F
SKIP_58:
lds	temp2,graphb
cp	temp,temp2
brsh	SKIP_59			;must be more than bottom
ldi	temp,0x3F	
SKIP_59:
sts	grapht,temp

rjmp	RS232_DONE

TEMPOUT:

WAIT_1:
sbis	UCSRA,UDRE
rjmp	WAIT_1
out	UDR,temp

WAIT_2:
sbis	UCSRA,UDRE
rjmp	WAIT_2
out	UDR,temp1

WAIT_3:
sbis	UCSRA,UDRE
rjmp	WAIT_3
out	UDR,temp1

WAIT_4:
sbis	UCSRA,UDRE
rjmp	WAIT_4
out	UDR,temp1

ret


PROCESS_PLOT:
;This captures the Plot code and pre-processes it
push	row
push	column			;I had to do it in the end!

ld	char,Y+			;Plot code
mov	pointer,char			;create a copy
andi	pointer,0x07		;now temp holds plot style
lsr	char
lsr	char
lsr	char				
sts	plot_command,char		;the plot command	

;Now take each two-byte ordinate,
;convert it to pixels
;adjust it if relative to the previous
;point and than add in any origin offset


ld	temp,Y+
sts	new_ext,temp		;save to ensure no cumulative errors
ld	temp1,Y+			;X-co-ordinates, low byte first.
sts	new_ext+1,temp1		;save to ensure no cumulative errors
sbrs	pointer,2			;if clear is absolute
rcall ADD_X_OFFSET
lds	temp2,horigin
lds	temp3,horigin+1
add	temp,temp2
adc	temp1,temp3			;Add origin offset (Done once then forget)
rcall	CONVERT_TO_PIXELS
sts	new,temp
sts	new+1,temp1			;and put in new co-ordinate store


ld	temp,Y+
sts	new_ext+2,temp		;save to ensure no cumulative errors
ld	temp1,Y+			;Y-co-ordinates, low byte first.
sts	new_ext+3,temp1		;save to ensure no cumulative errors
sbrs	pointer,2			;if clear is absolute
rcall ADD_Y_OFFSET
lds	temp2,vorigin
lds	temp3,vorigin+1
add	temp,temp2
adc	temp1,temp3			;Add origin offset (Done once then forget)
rcall	CONVERT_TO_PIXELS
sts	new+2,temp
sts	new+3,temp1			;and put in co-ordinate store


;Now the new co-ordinates are in adjusted internal co-ordinates
;at new to new+3

;Note that, in practice there are only four plot options, one is
;do nothing. But in typical Acorn style there are a feast of hoops to
;go through to determine the right style.

andi	pointer,0x03		;modulo 4 extracts style
brne	WORK_OUT_PLOT_STYLE	
rjmp	FINISH_PLOT			;The easy one, a relative move.

WORK_OUT_PLOT_STYLE:
;other settings depend on GCOL as well

cpi	pointer,0x02
breq	PLOT_INVERTED		;always invert

cpi	pointer,0x03
breq	BACKGROUND_SETTINGS
lds	temp,fgcol
lds	temp1,fgact			;load foreground colours and settings
rjmp	CHECK_COLOUR

BACKGROUND_SETTINGS:
lds	temp,bgcol
lds	temp1,bgact			;load background colours and settings

CHECK_COLOUR:
tst	temp
brne	BLACK_CHOICES		;Colour is black, now check options

cpi	temp1,0x00			;go through clear options
breq	PLOT_CLEAR
cpi	temp1,0x02
breq	PLOT_CLEAR
cpi	temp1,0x04
breq	PLOT_INVERTED
rjmp	FINISH_PLOT			;must be null option

BLACK_CHOICES:

cpi	temp1,0x00			;go through clear options
breq	PLOT_BLACK
cpi	temp1,0x01
breq	PLOT_BLACK
cpi	temp1,0x02
brne	PLOT_INVERTED		;must be inverted
rjmp	FINISH_PLOT	
	
;so after all that endless faff, we have three practical plot options:
PLOT_INVERTED:
sbr	colflags,(1<<plot_invert)
cbr	colflags,(1<<clear)|(1<<black)
rjmp	PLOT_STYLE

PLOT_CLEAR:
cbr	colflags,(1<<plot_invert)|(1<<black)
sbr	colflags,(1<<clear)
rjmp	PLOT_STYLE

PLOT_BLACK:
cbr	colflags,(1<<plot_invert)|(1<<clear)
sbr	colflags,(1<<black)

PLOT_STYLE:

cbr	flags,(1<<dotted)
cbr	colflags,(1<<omit_first)|(1<<omit_last)

lds	char,plot_command		;retrieve command

ldi	ZH,high(PLOT_TABLE)
ldi	ZL,low(PLOT_TABLE)

add	ZL,char
brcc	SKIP_61
inc	ZH
SKIP_61:

ijmp

PLOT_TABLE:
rjmp	PLOT_LINE			;Draw a line
rjmp	OMIT_FINAL			;omit final point
rjmp	DOTTED_LINE			;Dotted line
rjmp	DOT_OMIT_FINAL		;dotted omit final point
rjmp	OMIT_INITIAL		;omit first point
rjmp	OMIT_BOTH			;omit both ends
rjmp	DOT_OMIT_INITIAL		;dotted omit first point
rjmp	DOT_OMIT_BOTH		;dotted omit both ends
rjmp	PLOT_POINT			;Draw a single point
rjmp	PLOT_TRIANGLE		;plot filled triangle  :((
rjmp	FINISH_PLOT
rjmp	FINISH_PLOT
rjmp	PLOT_RECTANGLE		;Fill a rectangle
rjmp	FINISH_PLOT
rjmp	FINISH_PLOT
rjmp	FINISH_PLOT
rjmp	FINISH_PLOT
rjmp	FINISH_PLOT
rjmp	PLOT_CIRCLE			;Plot circle outline
rjmp	FILL_CIRCLE			;plot filled circle
rjmp	FINISH_PLOT
rjmp	FINISH_PLOT
rjmp	FINISH_PLOT
rjmp	FINISH_PLOT
rjmp	FINISH_PLOT
rjmp	FINISH_PLOT
rjmp	FINISH_PLOT
rjmp	FINISH_PLOT
rjmp	FINISH_PLOT
rjmp	FINISH_PLOT
rjmp	FINISH_PLOT
rjmp	FINISH_PLOT

LAST_CIRCLE_DOT:

brne	SKIP_38
rcall	PLOT_OTHER_DOTS

;rcall	PLOT_CIRCLE_DOT

SKIP_38:
rjmp	FINISH_PLOT
;keep these adjacent please
PLOT_CIRCLE:

	rcall	INITIALISE_CIRCLE

rcall	PLOT_FOUR_DOTS

lds	temp,cendy
lds	temp1,cendy+1
rjmp	INTO_LOOP

CIRCLE_LOOP:


lds	temp2,cendx			;check for last dot
lds	temp,cendy
lds	temp3,cendx+1
lds	temp1,cendy+1			;changing y-ordinate
sub	temp,temp2
sbc	temp1,temp3
brmi	INTO_LOOP
rjmp	LAST_CIRCLE_DOT				;is not last dot


INTO_LOOP:

rcall	PLOT_OTHER_DOTS
rcall	PLOT_CIRCLE_DOT


lds	temp,cendy
lds	temp1,cendy+1
;temp etc now contains cendy

clr	temp2
lsl	temp
rol	temp1
rol	temp2					; dy := y * 2
inc	temp					; add 1
brne	SKIP_302
inc	temp1
brne	SKIP_302
inc	temp2
SKIP_302:
clr	temp3
;   temp:temp1:temp2 is dy

lds	char,error
sub	char,temp
sts	error,char
lds	char,error+1
sbc	char,temp1
sts	error+1,char
lds	char,error+2
sbc	char,temp2
sts	error+2,char			;error := error - dy
brpl	INCREASE_CIRCLE_Y

lds	temp,cendx
lds	temp1,cendx+1

clr	temp2
lsl	temp
rol	temp1
rol	temp2					; x * 2

ldi	temp3,0x01
sub	temp3,temp
clr	temp
sbc	temp,temp1
clr	temp1
sbc	temp1,temp2				;temp3;temp;temp1 =1-x*2=dx


lds	char,error
sub	char,temp3
sts	error,char
lds	char,error+1
sbc	char,temp
sts	error+1,char
lds	char,error+2
sbc	char,temp1
sts	error+2,char

lds	temp,cendx
lds	temp1,cendx+1
subi	temp,0x01				; less 1
sbci	temp1,0x00
sts	cendx,temp
sts	cendx+1,temp1			;decrement x



INCREASE_CIRCLE_Y:
lds	temp,cendy
inc	temp
sts	cendy,temp
brne	INCREASE_CIRCLE_Y_DONE
lds	temp,cendy+1
inc	temp
sts	cendy+1,temp1
INCREASE_CIRCLE_Y_DONE:
rjmp	CIRCLE_LOOP


PLOT_CIRCLE_DOT:

lds	temp,cstartx
lds	temp1,cstartx+1
lds	temp2,cstarty
lds	temp3,cstarty+1
lds	char,cendx
add	temp,char
lds	char,cendx+1
adc	temp1,char
lds	char,cendy
add	temp2,char
lds	char,cendy+1
adc	temp3,char			;now we have first quadrant pixel

rcall	WIND
brne	SKIP_301			;test pointer
rcall	GADDR				;address the point
lds	pixel_mask,ZMASK		;load pixel mask
rcall	PLBYTE			;and plot it!!!
SKIP_301:

push	temp
push	temp1				;save x value

lds	temp,cstartx
lds	temp1,cstartx+1
lds	char,cendx
sub	temp,char
lds	char,cendx+1
sbc	temp1,char			;now we have fourth quadrant pixel

rcall	WIND
brne	SKIP_303			;test pointer
rcall	GADDR				;address the point
lds	pixel_mask,ZMASK		;load pixel mask
rcall	PLBYTE			;and plot it!!!
SKIP_303:

lds	temp2,cstarty
lds	temp3,cstarty+1
lds	char,cendy
sub	temp2,char
lds	char,cendy+1
sbc	temp3,char			;now we have fifth quadrant pixel

rcall	WIND
brne	SKIP_304			;test pointer
rcall	GADDR				;address the point
lds	pixel_mask,ZMASK		;load pixel mask
rcall	PLBYTE			;and plot it!!!
SKIP_304:

pop	temp1
pop	temp				;now we have eighth quadrant pixel

rcall	WIND
brne	SKIP_305			;test pointer
rcall	GADDR				;address the point
lds	pixel_mask,ZMASK		;load pixel mask
rcall	PLBYTE			;and plot it!!!
SKIP_305:

ret

PLOT_OTHER_DOTS:

lds	temp,cstartx
lds	temp1,cstartx+1
lds	temp2,cstarty
lds	temp3,cstarty+1
lds	char,cendy
add	temp,char
lds	char,cendy+1
adc	temp1,char
lds	char,cendx
add	temp2,char
lds	char,cendx+1
adc	temp3,char			;now we have second quadrant pixel

rcall	WIND
brne	SKIP_306			;test pointer
rcall	GADDR				;address the point
lds	pixel_mask,ZMASK		;load pixel mask
rcall	PLBYTE			;and plot it!!!
SKIP_306:

push	temp
push	temp1				;save x value

lds	temp,cstartx
lds	temp1,cstartx+1
lds	char,cendy
sub	temp,char
lds	char,cendy+1
sbc	temp1,char			;now we have third quadrant pixel

rcall	WIND
brne	SKIP_307			;test pointer
rcall	GADDR				;address the point
lds	pixel_mask,ZMASK		;load pixel mask
rcall	PLBYTE			;and plot it!!!
SKIP_307:

lds	temp2,cstarty
lds	temp3,cstarty+1
lds	char,cendx
sub	temp2,char
lds	char,cendx+1
sbc	temp3,char			;now we have sixth quadrant pixel

rcall	WIND
brne	SKIP_308			;test pointer
rcall	GADDR				;address the point
lds	pixel_mask,ZMASK		;load pixel mask
rcall	PLBYTE			;and plot it!!!
SKIP_308:


pop	temp1
pop	temp				;now we have seventh quadrant pixel

rcall	WIND
brne	SKIP_309			;test pointer
rcall	GADDR				;address the point
lds	pixel_mask,ZMASK		;load pixel mask
rcall	PLBYTE			;and plot it!!!
SKIP_309:

CIRCLE_DOT_DONE:
ret

PLOT_FOUR_DOTS:

lds	temp,cstartx
lds	temp1,cstartx+1
lds	temp2,cstarty
lds	temp3,cstarty+1
lds	char,cendx
add	temp,char
lds	char,cendx+1
adc	temp1,char			;now we have right pixel

rcall	WIND
brne	SKIP_310			;test pointer
rcall	GADDR				;address the point
lds	pixel_mask,ZMASK		;load pixel mask
rcall	PLBYTE			;and plot it!!!
SKIP_310:

lds	temp,cstartx
lds	temp1,cstartx+1

lds	char,cendx
sub	temp,char
lds	char,cendx+1
sbc	temp1,char			;now we have left pixel

rcall	WIND
brne	SKIP_311			;test pointer
rcall	GADDR				;address the point
lds	pixel_mask,ZMASK		;load pixel mask
rcall	PLBYTE			;and plot it!!!
SKIP_311:

lds	temp,cstartx
lds	temp1,cstartx+1
lds	temp2,cstarty
lds	temp3,cstarty+1
lds	char,cendx
add	temp2,char
lds	char,cendx+1
adc	temp3,char			;top pixel	
		
rcall	WIND
brne	SKIP_312			;test pointer
rcall	GADDR				;address the point
lds	pixel_mask,ZMASK		;load pixel mask
rcall	PLBYTE			;and plot it!!!
SKIP_312:

lds	temp2,cstarty
lds	temp3,cstarty+1
lds	char,cendx
sub	temp2,char
lds	char,cendx+1
sbc	temp3,char			;bottom pixel

rcall	WIND
brne	SKIP_313			;test pointer
rcall	GADDR				;address the point
lds	pixel_mask,ZMASK		;load pixel mask
rcall	PLBYTE			;and plot it!!!
SKIP_313:

ret

FILL_CIRCLE:

	rcall	INITIALISE_CIRCLE
	rcall	FILL_MIDDLE_ONE

FILL_CIRCLE_LOOP:

	lds	temp2,cendx			;check for last dot
	lds	temp,cendy
	lds	temp3,cendx+1
	lds	temp1,cendy+1			;changing y-ordinate
	sub	temp,temp2
	sbc	temp1,temp3
	brmi	INTO_FILL_LOOP				;is not last dot
rjmp	FINISH_FILL

INTO_FILL_LOOP:
	rcall	FILL_MIDDLE

	lds	temp,cendy
	lds	temp1,cendy+1
;temp etc now contains cendy

	clr	temp2
	lsl	temp
	rol	temp1
	rol	temp2					; dy := y * 2
	inc	temp					; add 1
	brne	SKIP_322
	inc	temp1
	brne	SKIP_322
	inc	temp2
SKIP_322:
	clr	temp3
;   temp:temp1:temp2 is dy

	lds	char,error
	sub	char,temp
	sts	error,char
	lds	char,error+1
	sbc	char,temp1
	sts	error+1,char
	lds	char,error+2
	sbc	char,temp2
	sts	error+2,char			;error := error - dy
	brpl	INCREASE_FILL_Y

;LO! If we are here then we have reached the end of a line

	rcall	FILL_SIDES

	lds	temp,cendx
	lds	temp1,cendx+1

	clr	temp2
	lsl	temp
	rol	temp1
	rol	temp2					; x * 2

	ldi	temp3,0x01
	sub	temp3,temp
	clr	temp
	sbc	temp,temp1
	clr	temp1
	sbc	temp1,temp2				;temp3;temp;temp1 =1-x*2=dx


	lds	char,error
	sub	char,temp3
	sts	error,char
	lds	char,error+1
	sbc	char,temp
	sts	error+1,char
	lds	char,error+2
	sbc	char,temp1
	sts	error+2,char

	lds	temp,cendx
	lds	temp1,cendx+1
	subi	temp,0x01				; less 1
	sbci	temp1,0x00
	sts	cendx,temp
	sts	cendx+1,temp1			;decrement x



INCREASE_FILL_Y:
	lds	temp,cendy
	inc	temp
	sts	cendy,temp
	brne	INCREASE_FILL_Y_DONE
	lds	temp,cendy+1
	inc	temp
	sts	cendy+1,temp1
	INCREASE_FILL_Y_DONE:
	rjmp	FILL_CIRCLE_LOOP

FINISH_FILL:

lds	temp,cendy
lds	temp1,cendx
cpse	temp,temp1
rjmp	FINISH_PLOT
	rcall	FILL_MIDDLE
rjmp	FINISH_PLOT



FILL_SIDES:

	lds	temp,cstartx
	lds	temp1,cstartx+1
	lds	char,cendx
	add	temp,char
	lds	char,cendx+1
	adc	temp1,char
	lds	temp2,cstarty
	lds	temp3,cstarty+1
	lds	char,cendy
	sub	temp2,char
	lds	char,cendy+1
	sbc	temp3,char			;bottom end of line

	sts	fill_block+4,temp
	sts	fill_block+5,temp1
	sts	fill_block+6,temp2
	sts	fill_block+7,temp3

	lds	temp2,cstarty
	lds	temp3,cstarty+1
	lds	char,cendy
	add	temp2,char
	lds	char,cendy+1
	adc	temp3,char			;top end of line

	sts	fill_block+2,temp2
	sts	fill_block+3,temp3

	ldi	ZH,high(fill_block)
	ldi	ZL,low(fill_block)

	rcall	VERTICAL_LINE_LOAD

	lds	temp,cstartx
	lds	temp1,cstartx+1
	lds	char,cendx
	sub	temp,char
	lds	char,cendx+1
	sbc	temp1,char
	
	sts	fill_block+4,temp
	sts	fill_block+5,temp1

	ldi	ZH,high(fill_block)
	ldi	ZL,low(fill_block)

	rcall	VERTICAL_LINE_LOAD

	ret

FILL_MIDDLE_ONE:
	lds	temp,cstartx
	lds	temp1,cstartx+1
	lds	char,cendy
	add	temp,char
	lds	char,cendy+1
	adc	temp1,char
	lds	temp2,cstarty
	lds	temp3,cstarty+1
	lds	char,cendx
	sub	temp2,char
	lds	char,cendx+1
	sbc	temp3,char			;bottom end of line

	sts	fill_block+4,temp
	sts	fill_block+5,temp1
	sts	fill_block+6,temp2
	sts	fill_block+7,temp3

	lds	temp2,cstarty
	lds	temp3,cstarty+1
	lds	char,cendx
	add	temp2,char
	lds	char,cendx+1
	adc	temp3,char			;top end of line

	sts	fill_block+2,temp2
	sts	fill_block+3,temp3

	ldi	ZH,high(fill_block)
	ldi	ZL,low(fill_block)

	rcall	VERTICAL_LINE_LOAD
	ret

FILL_MIDDLE:
	rcall	FILL_MIDDLE_ONE

	lds	temp,cstartx
	lds	temp1,cstartx+1
	lds	char,cendy
	sub	temp,char
	lds	char,cendy+1
	sbc	temp1,char
	
	sts	fill_block+4,temp
	sts	fill_block+5,temp1

	ldi	ZH,high(fill_block)
	ldi	ZL,low(fill_block)

	rcall	VERTICAL_LINE_LOAD
	ret


INITIALISE_CIRCLE:		;common to filled and outline circles

	ldi	ZL,low(new)
	ldi	ZH,high(new)

	rcall LOAD				;load into temp-temp3 and window.
	sts	cendx,temp			;we will use cendy and cendx to track the point
	sts	cendx+1,temp1		;cendx will be radius once adjusted for cstartx
	clr	temp				;so we start at radius,0
	sts	cendy,temp
	sts	cendy+1,temp			;save into working locations

	rcall	LOAD
	sts	cstartx,temp
	sts	cstartx+1,temp1
	sts	cstarty,temp2
	sts	cstarty+1,temp3		;save into working locations

	lds	temp2,cendx
	lds	temp3,cendx+1

	sub	temp,temp2
	sbc	temp1,temp3			;now we have radius

	brpl	SKIP_300			;is it negative?

	com	temp1
	neg	temp
	brcs	SKIP_300
	inc	temp1
	SKIP_300:				;get absolute value of radius



	sts	cendx,temp
	sts	cendx+1,temp1
	sts	error,temp
	sts	error+1,temp1
	clr	temp2
	sts	error+2,temp2		;saved
	ret

;*************** Circle ends here, lines start here

DOT_OMIT_BOTH:
sbr	flags,(1<<dotted)
OMIT_BOTH:
sbr	colflags,(1<<omit_first)|(1<<omit_last)
rjmp	PLOT_LINE

DOT_OMIT_INITIAL:
sbr	flags,(1<<dotted)
OMIT_INITIAL:
sbr	colflags,(1<<omit_last)
rjmp	PLOT_LINE

DOT_OMIT_FINAL:
sbr	flags,(1<<dotted)
OMIT_FINAL:
cbr	colflags,(1<<omit_last)
rjmp	PLOT_LINE

DOTTED_LINE:
sbr	flags,(1<<dotted)
PLOT_LINE:
ldi	ZL,low(new)
ldi	ZH,high(new)

rcall WIND_LOAD			;load into temp-temp3 and window.
mov	char,pointer

sts	endx,temp
sts	endx+1,temp1
sts	endy,temp2
sts	endy+1,temp3		;save into working locations

rcall	WIND_LOAD
and	char,pointer
breq	LINE_OK			;line does not cross window
rjmp	FINISH_PLOT
LINE_OK:

sts	startx,temp
sts	startx+1,temp1
sts	starty,temp2
sts	starty+1,temp3		;save into working locations

lds	temp2,endx
lds	temp3,endx+1		;current x in temp,temp1, new x in temp2,3

cbr	colflags,(1<<endx_less)
sub	temp2,temp
sbc	temp3,temp1			;temp2,temp3 is dx
brpl	NEWX_MORE
sbr	colflags,(1<<endx_less)
com	temp3
neg	temp2
brcs	SKIP_201
inc	temp3
SKIP_201:				;get absolute value of dx
NEWX_MORE:


sts	dx,temp2
sts	dx+1,temp3			;store dx value for later


lds	temp,starty
lds	temp1,starty+1
lds	temp2,endy
lds	temp3,endy+1

cbr	colflags,(1<<endy_less)
sub	temp2,temp
sbc	temp3,temp1			;temp2,temp3 is dy
brpl	NEWY_MORE
com	temp3
neg	temp2
brcs	SKIP_200
inc	temp3
SKIP_200:				;get absolute value of dy
sbr	colflags,(1<<endy_less)

NEWY_MORE:

sts	dy,temp2
sts	dy+1,temp3			;store dy for later

lds	temp,dx
lds	temp1,dx+1

cbr	colflags,(1<<dx_less)	;compare absolute values of dx and dy
sub	temp2,temp
sbc	temp3,temp1			;temp2,temp3 is dy
brmi	DX_MORE
sbr	colflags,(1<<dx_less)
DX_MORE:

sbrs	colflags,dx_less
rjmp	HORIZONTAL_QUADRANT

VERTICAL_QUADRANT:
sbrc	colflags,endy_less
rcall	SWAP_PARAMETERS

lds	char,dy
lds	pointer,dy+1
lsr	pointer
ror	char			;divide by two - char:pointer are error

sbrs	colflags,omit_first
rcall	PLOT_FIRST_DOT

V_U_NEXT_POINT:
inc	temp2
brne	SKIP_120
inc	temp3				;startx := startx+1
SKIP_120:
lds	row,dx
lds	column,dx+1			;dx
sub	char,row
sbc	pointer,column		;error := error - dx
tst	pointer
brsh	V_ERROR_POSITIVE		;if error<0

sbrc	colflags,endx_less
rcall	DECREASE_X			;going left
sbrs	colflags,endx_less
rcall	INCREASE_X			;going right

lds	row,dy
lds	column,dy+1			;now dx
add	char,row
adc	pointer,column		;error := error+dy
V_ERROR_POSITIVE:
lds	row,endy			;check for last dot (temp2:temp3 = endy)
cpse	temp2,row		
rjmp	V_NOT_LAST_DOT		;is not last dot
lds	column,endy+1
cpse	column,temp3
rjmp	V_NOT_LAST_DOT
rjmp	LAST_DOT





HORIZONTAL_QUADRANT:
sbrc	colflags,endx_less
rcall	SWAP_PARAMETERS

lds	char,dx
lds	pointer,dx+1
lsr	pointer
ror	char			;divide by two - char:pointer are error

sbrs	colflags,omit_first
rcall	PLOT_FIRST_DOT

H_U_NEXT_POINT:
inc	temp
brne	SKIP_100
inc	temp1				;startx := startx+1
SKIP_100:
lds	row,dy
lds	column,dy+1			;dy
sub	char,row
sbc	pointer,column		;error := error - dy
tst	pointer
brsh	ERROR_POSITIVE		;if error<0

sbrc	colflags,endy_less
rcall	DECREASE_Y			;going down
sbrs	colflags,endy_less
rcall	INCREASE_Y			;going up

lds	row,dx
lds	column,dx+1			;now dx
add	char,row
adc	pointer,column		;error := error+dx
ERROR_POSITIVE:
lds	row,endx			;check for last dot (temp:temp1 = endx)
cpse	temp,row
rjmp	NOT_LAST_DOT		;is not last dot
lds	column,endx+1
cpse	column,temp1
rjmp	NOT_LAST_DOT
rjmp	LAST_DOT

NOT_LAST_DOT:
sbrc	flags,dotted
sbrc	temp,0			;if this is an odd pixel, don't plot
rcall	PLOT_NEXT_DOT
rjmp	H_U_NEXT_POINT

V_NOT_LAST_DOT:
sbrc	flags,dotted
sbrc	temp2,0			;if this is an odd pixel, don't plot
rcall	PLOT_NEXT_DOT
rjmp	V_U_NEXT_POINT

LAST_DOT:
lds	temp2,endy
lds	temp3,endy+1
sbrs	colflags,omit_last
rcall	PLOT_NEXT_DOT
rjmp	FINISH_PLOT

PLOT_FIRST_DOT:
ldi	ZL,low(startx)
ldi	ZH,high(startx)
push	pointer
rcall	WIND_LOAD
breq	H_U_POINT_OK
pop	pointer
ret
H_U_POINT_OK:
pop	pointer			;we have just pushed pointer and called WIND
rcall	GADDR				;address the point
lds	pixel_mask,ZMASK		;load pixel mask
rcall	PLBYTE			;and plot it!!!
ret

INCREASE_Y:
inc	temp2
brne	SKIP_101
inc	temp3	
SKIP_101:				;y := y+1
ret

DECREASE_Y:
subi	temp2,0x01
sbci	temp3,0x00			;y := y-1
ret

INCREASE_X:
inc	temp
brne	SKIP_121
inc	temp1	
SKIP_121:				;x := x+1
ret

DECREASE_X:
subi	temp,0x01
sbci	temp1,0x00			;x := x-1
ret

PLOT_NEXT_DOT:
push	pointer
rcall	WIND
breq	H_U_POINT_OK
pop	pointer
ret

SWAP_PARAMETERS:

ldi	temp,(1<<endx_less)|(1<<endy_less)
eor	colflags,temp				;invert flags to reflect swap

lds	temp,startx		;Time for an unmitigated stackfest!
push	temp
lds	temp,startx+1
push	temp
lds	temp,starty
push	temp
lds	temp,starty+1
push	temp
lds	temp,endx
push	temp
lds	temp,endx+1
push	temp
lds	temp,endy
push	temp
lds	temp,endy+1
sts	starty+1,temp
pop	temp
sts	starty,temp
pop	temp
sts	startx+1,temp
pop	temp
sts	startx,temp
pop	temp
sts	endy+1,temp
pop	temp
sts	endy,temp
pop	temp
sts	endx+1,temp
pop	temp
sts	endx,temp

clr	temp
sbrc	colflags,omit_first
sbr	temp,(1<<omit_last)
sbrc	colflags,omit_last
sbr	temp,(1<<omit_first)					;make swapped copy
cbr	colflags,(1<<omit_first)|(1<<omit_last)		;blank both flags
or	colflags,temp						;load new copy


ldi	ZL,low(startx)			;reload parameters
ldi	ZH,high(startx)
rcall	WIND_LOAD

ret



PLOT_POINT:

ldi	ZL,low(new)
ldi	ZH,high(new)


rcall WIND_LOAD			;load into temp-temp3 and window.
breq	POINT_OK
rjmp	FINISH_PLOT			;point lies outside the window
POINT_OK:

rcall	GADDR				;address the point

lds	pixel_mask,ZMASK		;load pixel mask

rcall	PLBYTE			;and plot it!!!

rjmp	FINISH_PLOT			;We need to start worrying about this Finnish Plot!

PLOT_RECTANGLE:

;new cursor point held at ZL:ZH

ldi	ZH,high(new)
ldi	ZL,low(new)

;First check new position 
rcall	WIND_LOAD
sts	endx,temp
sts	endy,temp2			;store ready to use

tst	pointer			;is it in window?
breq	RECT2_IN_WINDOW		;yes

lds	temp,graphl
sbrc	pointer,0
sts	endx,temp			;off the left, move to edge
lds	temp,graphb
sbrc	pointer,2
sts	endy,temp				;off the bottom		
lds	temp,graphr
sbrc	pointer,1
sts	endx,temp			;off the right
lds	temp,grapht
sbrc	pointer,3
sts	endy,temp			;off the top set to top line
					;NB high bytes can now be ignored.
RECT2_IN_WINDOW:

;Then ensure current x and y lie in the window and move to startx, starty

mov	char,pointer

rcall	WIND_LOAD
push	pointer
and	pointer,char		;any points off window in same zone?
breq	RECTANGLE_OK
pop pointer
rjmp	FINISH_PLOT					;we have nothing to draw
RECTANGLE_OK:
pop	pointer
lds	temp,current
lds	temp2,current+2		;get old co-ordinates low bytes

sts	startx,temp
sts	starty,temp2

tst	pointer
breq	RECT1_IN_WINDOW

lds	temp,graphl
sbrc	pointer,0
sts	startx,temp			;off the left, move to edge
lds	temp,graphb
sbrc	pointer,2
sts	starty,temp			;off the bottom	
lds	temp,graphr	
sbrc	pointer,1
sts	startx,temp			;off the right
lds	temp,grapht
sbrc	pointer,3
sts	starty,temp			;off the top set to top line
					;NB high bytes can now be ignored.

RECT1_IN_WINDOW:

lds	temp,startx
lds	temp1,endx
cp	temp1,temp			;Is end x bigger than start?

brsh	X_OK
sts	startx,temp1
sts	endx,temp			;No swap and store
X_OK:

lds	temp,starty
lds	temp1,endy
cp	temp1,temp			;Is end y bigger than start?

brsh	Y_OK
sts	starty,temp1
sts	endy,temp			;No swap and store
Y_OK:

rcall	FILL_SAFE_RECTANGLE

rjmp	FINISH_PLOT

FILL_SAFE_RECTANGLE:
;we can jump in here if startx,starty, endx,endy are all legal.

lds	pointer,startx
lds	char,endx			;get start and end points
inc	char				;to plot last row
NEXT_RECTANGLE_SLICE:
rcall	VPLOT				;plot from startx,starty to endy
inc	pointer
sts	startx,pointer
cp	char,pointer
brne	NEXT_RECTANGLE_SLICE	;are we done?

ret

;********************

FINISH_PLOT:			;Shuffle new into current and current into old

lds	temp,new_ext
sts	current_ext,temp
lds	temp,new_ext+1
sts	current_ext+1,temp
lds	temp,new_ext+2
sts	current_ext+2,temp
lds	temp,new_ext+3
sts	current_ext+3,temp	;external graphics co-ordinates


lds	temp2,current
sts	old,temp2
lds	temp2,current+1
sts	old+1,temp2
lds	temp2,current+2
sts	old+2,temp2
lds	temp2,current+3
sts	old+3,temp2			;replace old with current

lds	temp2,new
sts	current,temp2
lds	temp2,new+1
sts	current+1,temp2
lds	temp2,new+2
sts	current+2,temp2
lds	temp2,new+3
sts	current+3,temp2		;replace current with new

cbr	flags,(1<<dotted)		;as it is also used as bitmap flag

pop	column
pop	row
rjmp	RS232_DONE

ADD_X_OFFSET:
lds	temp2,current_ext
lds	temp3,current_ext+1
add	temp,temp2
adc	temp1,temp3			;implement relative plot
ret

ADD_Y_OFFSET:
lds	temp2,current_ext+2
lds	temp3,current_ext+3
add	temp,temp2
adc	temp1,temp3			;implement relative plot
ret

CONVERT_TO_PIXELS:		;divides temp1;temp by 16. Temp is low byte.
asr	temp1				;used to pre-process all external graphics units
ror	temp
asr	temp1
ror	temp
asr	temp1
ror	temp
asr	temp1
ror	temp
ret

;********	These routine emulate the '8 PLOT entry points' of the BBC Micro MOS

VERTICAL_LINE_LOAD:

;Uses Z-register as an index if point needs to be loaded.

ld	temp,Z+
ld	temp1,Z+
ld	temp2,Z+
ld	temp3,Z+			;point held at ZL:ZH

VERTICAL_LINE:				;fast vertical line
;replaces HPLOT (because of different memory arrangement)
;draws a line joining two points.
;start point must be in temp to temp3

;First check new position 

clr	temp
clr	temp1			;only interested in y-ordinate

rcall	WIND

tst	pointer
breq	IN_WINDOW_TOO

sbrc	pointer,2
lds	temp2,graphb			;off the bottom
sbrc	pointer,3
lds	temp2,grapht			;off the top	


IN_WINDOW_TOO:
sts	endy,temp2			;store at endy
;Uses Z-register as an index if point needs to be loaded.

ld	temp,Z+
ld	temp1,Z+
ld	temp2,Z+
ld	temp3,Z+			;point held at ZL:ZH

rcall	WIND

;Here we test new x to see if it is lined up with the window

tst	pointer
breq	IN_WINDOW

sbrc	pointer,0
ret					;off the left, nothing to plot
sbrc	pointer,1
ret					;off the right

;Then ensure new y lies in the window

sbrc	pointer,2
lds	temp2,graphb			;off the bottom, set to bottom
sbrc	pointer,3
ldi	temp2,grapht			;off the top set to top 

IN_WINDOW:

sts	startx,temp
lds	temp1,endy
cp	temp1,temp2
brpl	SKIP_77
push	temp1
mov	temp1,temp2
pop	temp2

SKIP_77:
sts	endy,temp1
sts	starty,temp2

VPLOT:
;This routine plots a vertical line from startx,starty to startx,endy
;starty must be less than endy
;corrupts temp,temp2,temp3, and pixel_mask.
;char,pointer and temp1 preserved
push	char
push	pointer
lds	temp,startx
lds	temp2,starty
QUICK_IN:
lds	char,endy


sub	char,temp2
inc	char				;char is now number of bytes to fill
					;note we must plot a 'zero' pixel
rcall	GADDR
;pageY is now start page
;temp is the column
;ZMASK contains first pixel to plot
lds	temp3,ZMASK			;7-0 depending on position in column
clr	pixel_mask			;we will move temp3 to make pixel mask
NEXT_PIXEL:
or	pixel_mask,temp3		;pop in that bit!
lsr	temp3				;next pixel up
breq	NEXT_GRAPHIC_BYTE		;got to top of byte
dec	char
brne	NEXT_PIXEL			;still pixels to go
jmp	V_DONE
NEXT_GRAPHIC_BYTE:
dec	char
breq	V_DONE
rcall	PLBYTE
dec	pageY				;next row up
call	SEND_INSTRUCTION
call	SET_PAGE
call	SET_BYTE
ldi	temp3,0x80			;top bit set
clr	pixel_mask			;ready for next byte
rjmp	NEXT_PIXEL

V_DONE:
rcall	PLBYTE
pop	pointer
pop	char
ret					;all done

PLBYTE:
;plots the mask in pixel_mask at the page and column addressed by GADDR
push	pointer
push	char
clr	char
out	DDRC,char
out	PORTC,char
call FETCH_DATA
call CLOCK
nop
nop
nop
	nop
nop
nop
sbi	PORTD,enable
nop
nop
nop
	nop
nop
nop
nop
nop
nop
nop
nop
nop
nop
nop
nop
nop
in	display_byte,PINC
cbi	PORTD,enable
ser	char
sbrc	colflags,plot_invert
eor	display_byte,pixel_mask			;invert what is there using pixel mask
sbrc	colflags,black
or	display_byte,pixel_mask			;set pixels in pixel mask
sbrs	colflags,clear
rjmp	MASKING_DONE
com	pixel_mask
and	display_byte,pixel_mask			;clear pixels in original pixel mask
com	pixel_mask					;restore
MASKING_DONE:
nop
nop
nop
nop
out	DDRC,char
call	SEND_INSTRUCTION
call	SET_BYTE
call	SEND_DATA
out	PORTC,display_byte
call	CLOCK
pop	char
pop	pointer
ret


SET_PAGE:			;sets row from 0 - 7 according to pageY
call TWO_SIDE
mov	pointer,scroll		;create a copy (we must allow for scrolling)
andi	pointer,~0xC0		;mask off top bits
asr	pointer
asr	pointer
asr	pointer			;divide by eight - temp is top row
add	pointer,pageY
andi	pointer,0x07		;keep in scope - reduce modulo height
ori	pointer,0xB8		;create instruction		
out	PORTC,pointer
call	CLOCK
ret

SET_BYTE:			;sets column from 0 to 127 according to temp
call RIGHT_SIDE
mov	pointer,temp		;make a copy
sbrs	pointer,6
call	LEFT_SIDE
andi	pointer,0x3F
ori	pointer,0x40		;create instruction
out	PORTC,pointer
call	CLOCK
ret

GADDR_LOAD:

;Uses Z-register as an index if point needs to be loaded.

ld	temp,Z+
ld	temp1,Z+
ld	temp2,Z+
ld	temp3,Z+			;point held at ZL:ZH

GADDR:
;determines the byte pointed to by temp (x) and temp2 (y)
;NB! ensure that byte is in window first!
;pageY becomes the page to address
;ZMASK identifies the pixel to change
push	pointer
push	char
ldi	char,0x3F
sub	char,temp2			;we count from the top
mov	pageY,char
andi	char,0x07			;now char is number of pixels from top of column 
clr	pointer
sec					;carry ready to shift into top bit
SKIP_68:
rol	pointer				;shift pixel position one bit
dec	char
brpl	SKIP_68			;not rolled down to 0xFF yet	
sts	ZMASK,pointer			;pixel position fixed

asr	pageY
asr	pageY
asr	pageY				;divide by 8
andi	pageY,0x07			;now has page number

call	SEND_INSTRUCTION
call	SET_PAGE			;derive page (y div 8) from pageY
call	SET_BYTE			;derive column (x) from temp

pop	char
pop	pointer
ret

LOAD:
;Uses Z-register as an index to load a point in temp:...:temp3.

	ld	temp,Z+
	ld	temp1,Z+
	ld	temp2,Z+
	ld	temp3,Z+			;point held at ZL:ZH
	ret

WIND_LOAD:

rcall	LOAD

WIND:
;windows pixel co-ordinates if already in temp:temp1, temp2:temp3
; using the code 0 = OK in window, 1=left, 2=right,8=above,4=below.
;result into pointer
push	char
clr	pointer

tst	temp1	
brmi	WIND_LEFT			;if negative, then left
brne	WIND_RIGHT			;if upper byte positive, then right
					;must be a single byte value
lds	char,graphl
cp	temp,char			;test left
brlo	WIND_LEFT
lds	char,graphr
cp	char,temp
brsh	TEST_VERTICAL		;is horizontally in window 
WIND_RIGHT:
subi	pointer,-1			;adds one, then another one at next instruction
WIND_LEFT:
subi	pointer,-1

TEST_VERTICAL:
tst	temp3	
brmi	WIND_BELOW			;if negative, then left
brne	WIND_ABOVE			;if upper byte positive, then right
					;must be a single byte value
lds	char,graphb		
cp	temp2,char			;test bottom
brlo	WIND_BELOW
lds	char,grapht
cp	char,temp2
brsh	VERTICAL_OK			;is vertically in window 
WIND_ABOVE:
subi	pointer,-4			;adds one, then another one at next instruction
WIND_BELOW:
subi	pointer,-4

VERTICAL_OK:
pop	char
tst	pointer
ret					;returns result in pointer, but can be tested
					;by an immediate BREQ/BRNE. Snazzy eh?

EIGABS:
;replaced by CONVERT_TO_PIXELS and the two ADD_OFFSET routines

EIG:
;not used - all graphics calculations done in internal (pixel) units.

;********	*FX Code Decoding

FX_DECODE:

ld	char,Y+			;This code tells us which *FX/Osbyte it is
ld	fx_x,Y+
ld	fx_y,Y+			;These are temp1 and temp2 by other names

tst	char
breq	FX_ZERO
cpi	char,0x86
breq	READ_TAB
cpi	char,0x87
breq	CHARACTER_COMPARE1
cpi	char,144
breq	STAR_TV1
cpi	char,0x9A
breq	CHANGE_CURSOR1
cpi	char,0x9B
breq	READ_PALETTE1
cpi	char,0xA1
breq	READ_EEPROM1
cpi	char,0xA2
breq	WRITE_EEPROM1
cpi	char,0xB8
breq	CHANGE_CHAR_WIDTH
cpi	char,0xD9
breq	CHANGE_LINE_COUNT
cpi	char,0xF0
breq	COUNTRY_FLAG
cpi	char,0xF1
breq	CHANGE_USER_FLAG1


FX_DONE:
rcall	SPI_SEND_WAIT		;send via SPI
rjmp	RS232_DONE

STAR_TV1:
rjmp	STAR_TV
READ_PALETTE1:
rjmp	READ_PALETTE
CHANGE_CURSOR1:
rjmp	CHANGE_CURSOR
CHANGE_USER_FLAG1:
rjmp	CHANGE_USER_FLAG
READ_EEPROM1:
rjmp	READ_EEPROM
WRITE_EEPROM1:
rjmp	WRITE_EEPROM


FX_ZERO:
cp	fx_x,char
brne	NO_RESET
jmp	RESET
NO_RESET:
cpi	fx_x,0x01
brne	FX_DONE
ldi	char,10			;version number
breq	FX_DONE
rcall	SPI_SEND_WAIT		;send via SPI
rjmp	FX_DONE


READ_TAB:
mov	temp,char_width
mov	char,column			;But this is in columns, not character cells
lsr	char
lsr	char				;divide by 4
cpi	temp,0x04
breq	SKIP_06
lsr	char				;divide by 8
cpi	temp,0x08
breq	SKIP_06
lsr	char				;divide by 16
SKIP_06:
rcall	SPI_SEND_WAIT		;send via SPI

mov	char,row
rjmp	FX_DONE


CHARACTER_COMPARE1:
rjmp	CHARACTER_COMPARE



COUNTRY_FLAG:
ldi	char,0x1				;for UK
rjmp	FX_DONE

CHANGE_LINE_COUNT:
lds	char,line_count
rcall	FX_CHANGE
sts	line_count,char
rjmp	FX_DONE

CHANGE_CHAR_WIDTH:			;pretty self evident
cpi	fx_x,0x04
breq	NEW_WIDTH_OK
cpi	fx_x,0x08
breq	NEW_WIDTH_OK
cpi	fx_x,0x10
breq	NEW_WIDTH_OK
ldi	fx_x,0xFF				;Duff width flagged
rjmp	FX_DONE
NEW_WIDTH_OK:
mov	char,char_width
rcall	FX_CHANGE
mov	char_width,char
rjmp	FX_DONE

READ_EEPROM:
ldi	YH,0x01			;user area is upper page
mov	YL,fx_x
rcall	EEPROM_read
rjmp	FX_DONE

WRITE_EEPROM:
ldi	YH,0x01			;user area is upper page
mov	YL,fx_x
mov	char,fx_y
rcall	EEPROM_write
rjmp	FX_DONE

CHANGE_CURSOR:
lds	char,cursor_pattern
rcall	FX_CHANGE
sts	cursor_pattern,char
sts	old_cursor_pattern,char
rjmp	FX_DONE

READ_PALETTE:
lds	char,fgcol
rcall	SPI_SEND_WAIT		;send via SPI
lds	char,bgcol
rjmp	FX_DONE

CHANGE_USER_FLAG:
lds	char,user_flag
rcall	FX_CHANGE
sts	user_flag,char
rjmp	FX_DONE

STAR_TV:
lds	char,tv
sub	scroll,char			;remove current setting
rcall	FX_CHANGE
add	scroll,char			;add in new one
sts	tv,char
call	SEND_SCROLL
rjmp	FX_DONE

FX_CHANGE:
and	char,fx_y			;this is what reads or updates an FX flag
eor	char,fx_x
ret

CHARACTER_COMPARE:
call	TAB
call	COPY_CHARACTER
push	colflags
mov	colflags,char_width
ldi	char,0x20				;start with space
ldi	ZH,0x38				;character table, 16,8 column modes
ldi	ZL,0x00
cpi	colflags,0x04
brne	NEXT_CHARACTER
ldi	ZH,0x3B				;character table, 32 column modes
ldi	ZL,0x80
NEXT_CHARACTER:
clr	temp					;flag for match
clr	pointer				;column in copy
ldi	YH,high(char_table)		;this is the copy
ldi	YL,low(char_table)
NEXT_COMPARE:
ld	temp1,Y+
lpm	temp2,Z+
cp	temp1,temp2				;if both match
breq	SKIP_40
inc	temp					;if not temp no longer zero
SKIP_40:
cpi	colflags,0x10
brne	SKIP_42
inc	pointer
ld	temp1,Y+				;this is a dummy check for 8 column modes
SKIP_42:
inc	pointer
cp	pointer,char_width		;done whole char?
brne	NEXT_COMPARE
tst	temp
breq	FOUND					;it matched!
inc	char
cpi	char,0x7F
brne	NEXT_CHARACTER

ldi	char,0xE0				;start with lowest user character
ldi	ZH,high(user_chars)		;character table
ldi	ZL,low(user_chars)
NEXT_USER_CHARACTER:
clr	temp					;flag for match
clr	pointer				;column in copy
ldi	YH,high(char_table)		;this is the copy
ldi	YL,low(char_table)
NEXT_USER_COMPARE:
ld	temp1,Y+
ld	temp2,Z+
cp	temp1,temp2				;if both match
breq	SKIP_41
inc	temp					;if not temp no longer zero
SKIP_41:
cpi	colflags,0x10
brne	SKIP_43
inc	pointer
ld	temp1,Y+				;this is a dummy check for double-width characters
SKIP_43:
inc	pointer
cp	pointer,char_width		;done whole char?
brne	NEXT_USER_COMPARE
tst	temp
breq	FOUND					;it matched!
inc	char
cpi	char,0x00
brne	NEXT_USER_CHARACTER

clr	char					;run out of chars
FOUND:

rcall	SPI_SEND_WAIT		;send via SPI

pop	colflags
lds	char,mode
rjmp	FX_DONE

;*******	Use a table of vectors for Control characters 0-31

CONTROL:

ldi	ZH,high(CONTROL_TABLE)
ldi	ZL,low(CONTROL_TABLE)

add	ZL,char
brcc	SKIP_26
inc	ZH
SKIP_26:

ijmp


CONTROL_TABLE:
rjmp	NUL
rjmp	SOH
rjmp	STX
rjmp	ETX
rjmp	EOT
rjmp	ENQ
rjmp	ACK
rjmp	BEL
rjmp	BS
rjmp	HT
rjmp	LF
rjmp	VT
rjmp	FF
rjmp	CR
rjmp	SO
rjmp	SI
rjmp	DLE
rjmp	DC1
rjmp	DC2
rjmp	DC3
rjmp	DC4
rjmp	NAK
rjmp	SYN
rjmp	ETB
rjmp	CAN
rjmp	EM
rjmp	SUB_
rjmp	ESC
rjmp	FS
rjmp	GS
rjmp	RS
rjmp	US

;********	 CONTROL CODE ROUTINES

NUL:					;does nothing
rjmp	RS232_DONE	

SOH:					;next character to printer only
ldi	temp,0x01
mov	vdu_queue,temp
rjmp	SET_UP_QUEUE
	
STX:					;Printer enable
sbr	flags,(1<<printer_on)
jmp	RS232_DONE	

ETX:					;Printer disable
cbr	flags,(1<<printer_on)
jmp	RS232_DONE	

EOT:					;Text at text cursor
cbr	flags,(1<<graphics)
lds	temp,old_cursor_pattern
sts	cursor_pattern,temp
jmp	RS232_DONE	

ENQ:					;Text at graphics cursor
sbr	flags,(1<<graphics)
lds	temp,cursor_pattern
sts	old_cursor_pattern,temp
clr	temp
sts	cursor_pattern,temp
jmp	RS232_DONE	

ACK:					;Enable VDU drivers
cbr	flags,(1<<screen_off)	;yes, screen back on (in theory we ought never get here!)
jmp	RS232_DONE

BEL:					;Make a short beep
jmp	RS232_DONE

BS:					;Cursor left
call	BACKSPACE
jmp	RS232_DONE

HT:					;Cursor right
sbrc	flags,graphics
rjmp	GRAPHIC_HT
sbrc	flags,pending
call	PROCESS_SCROLL
add	column,char_width
cp	column,right
brne HT_DONE
mov	column,left
rjmp	LF
HT_DONE:
jmp	RS232_DONE

GRAPHIC_HT:
call	GCURS_RIGHT
jmp	RS232_DONE

LF:					;Cursor down
sbrc	flags,graphics
rjmp	GRAPHIC_LF
inc	row
cp	row,bottom		;will we wrap round?
brne	LF_DONE
dec	row
call	SCROLL_NOW		;yes!
LF_DONE:
jmp	RS232_DONE

GRAPHIC_LF:
call	GCURS_DOWN
jmp	RS232_DONE

VT:					;Cursor up
sbrc	flags,graphics
rjmp	GRAPHIC_VT
cbr	flags,(1<<pending)
cp	row,top
breq	AT_TOP
dec	row
jmp	RS232_DONE
AT_TOP:
call	SCROLL_UP
jmp	RS232_DONE

GRAPHIC_VT:
call	GCURS_UP
jmp	RS232_DONE

FF:					;Clear text area		
cbr	flags,(1<<pending)
call CLEAR_SCREEN
jmp	RS232_DONE

CR:
sbrc	flags,graphics
rjmp	GRAPHIC_CR
mov	column,left			;that's easy!
call TAB
jmp	RS232_DONE

GRAPHIC_CR:
call	GCURS_RET
jmp	RS232_DONE

SO:					;Page mode on
sbr	flags,(1<<paging)
jmp	RS232_DONE	

SI:					;Page mode off
cbr	flags,(1<<paging)
jmp	RS232_DONE	

DLE:					;Clear Graphics area

cbr	colflags,(1<<plot_invert)
lds	temp,bgcol
tst	temp
brne	CLEAR_TO_BLACK
sbr	colflags,(1<<clear)
cbr	colflags,(1<<black)

rjmp	CLG

CLEAR_TO_BLACK:
cbr	colflags,(1<<clear)
sbr	colflags,(1<<black)

CLG:

lds	temp,graphl
sts	startx,temp
lds	temp,graphb
sts	starty,temp
lds	temp,graphr
sts	endx,temp
lds	temp,grapht
sts	endy,temp

rcall	FILL_SAFE_RECTANGLE

jmp	RS232_DONE	

DC1:					;Define text colour
ldi	temp,0x01
mov	vdu_queue,temp
jmp	SET_UP_QUEUE

DC2:					;Define graphics colour
ldi	temp,0x02
mov	vdu_queue,temp
jmp	SET_UP_QUEUE

DC3:					;Define logical colour
ldi	temp,0x05
mov	vdu_queue,temp
jmp	SET_UP_QUEUE

DC4:					;Restore default logical colours
rcall DEFAULT_COLOURS
jmp	RS232_DONE

DEFAULT_COLOURS:
clr	temp1
sts	pattern,temp1		;restore text to black on white	
sts	bgcol,temp1
sts	bgact,temp1
sts	fgact,temp1
inc	temp1
sts	fgcol,temp1			;restore black on white graphics, default action
ret	

NAK:					;Disable VDU drivers
sbr	flags,(1<<screen_off)	;screen off
jmp	RS232_DONE	

SYN:					;Select screen mode
cbr	flags,(1<<pending)
ldi	temp,0x01
mov	vdu_queue,temp
jmp	SET_UP_QUEUE

ETB:					;Reprogram display character
ldi	temp,0x09
mov	vdu_queue,temp
jmp	SET_UP_QUEUE

CAN:					;Define graphics window
ldi	temp,0x08
mov	vdu_queue,temp
jmp	SET_UP_QUEUE

EM:					;PLOT K,x,y
ldi	temp,0x05
mov	vdu_queue,temp
jmp	SET_UP_QUEUE

SUB_:					;Restore default windows
call	RST_WINDOWS
jmp	RS232_DONE

ESC:					;Does nothing (officially) Unofficially gets three bytes for Osbyte
ldi	temp,0x03
mov	vdu_queue,temp
jmp	SET_UP_QUEUE

FS:					;Define text window
cbr	flags,(1<<pending)
ldi	temp,0x04
mov	vdu_queue,temp
jmp	SET_UP_QUEUE

GS:					;Define graphics origin
ldi	temp,0x04
mov	vdu_queue,temp
jmp	SET_UP_QUEUE

RS:					;Home cursor to top left
cbr	flags,(1<<pending)
mov	row,top		;top row
mov	column,left		;home cursor
call TAB
jmp	RS232_DONE

US:					;Move text cursor to x,y
cbr	flags,(1<<pending)
ldi	temp,0x02
mov	vdu_queue,temp
jmp	SET_UP_QUEUE


.include	"triangle.inc"
.include	"keyboard.inc"

;**************************************


DELAY:			;usual loopy delay	about 8 mS at 8MHz
;ret
ldi	temp,0xff
mov r0,temp
COLBACK:
ldi	temp,0xff
mov	r1,temp
ROWBACK:
dec	r1
brne	ROWBACK
dec	r0
brne	COLBACK
ret

SPI_SlaveInit:
; Set MISO output, all others input
;done under PB initialisation

	sbi	DDRB,MISO
	cbi	DDRB,MOSI
	cbi	DDRB,SCK
	cbi	DDRB,SS
	cbi	PORTB,SS
; Enable SPI
	ldi	temp,(1<<SPIE|1<<SPE)
	out	SPCR,temp

	ret

SPI_SEND_WAIT:
	out	SPDR,char
	clt			;clear T flag

SPI_Wait:			; Wait for send complete
	brts	SPI_Wait	;T flag set when complete
	ret

SPI_MasterInit:

;not used!!

; Set MOSI and SCK output, all others input

	in	temp,DDRB
	ori	temp,(1<<PB5)|(1<<PB7)
	andi	temp,~(1<<PB6)
	out	DDRB,temp

; Enable SPI, Master, set clock rate fck/16

	ldi	temp,(1<<SPE)|(1<<MSTR)|(1<<SPR0)
	out	SPCR,temp
	ret

USART_Init:			;sets up RS232
; Set baud rate at a mere 19200 BAUD
clr	temp
out	UBRRH, temp
ldi	temp,25		;19200 @ 8MHz   set to 34 for 14400 baud
out	UBRRL, temp
; Enable receiver and transmitter and enable interrupts
ldi	temp, (1<<RXEN)|(1<<TXEN)|(1<<RXCIE)
out	UCSRB,temp
; Set frame format: Asynchronous, 8data, 1stop bit
ldi	temp, (1<<URSEL)|(3<<UCSZ0)
out	UCSRC,r16
ret

TIMER2_INIT:			;fast for keyboard input timing

;set prescaler to 128, about 1 millisecond
ldi	temp,(4<<CS20)
out	TCCR2,temp
;enable overflow interrupts
in	temp,TIMSK
ori	temp,(1<<TOIE2)
out	TIMSK,temp

ret

TIMER1_INIT:		;slow system tick for flashing etc.
;set prescaler to 64
ldi	temp,(3<<CS10)
out	TCCR1B,temp
; Set TCNT1 to 0.5s		@ 8MHz
ldi	temp,0x0B
mov	timer1h,temp
ldi	temp,0xDC
mov	timer1l,temp
out	TCNT1H,timer1h
out	TCNT1L,timer1l
;enable overflow interrupts
in	temp,TIMSK
ori	temp,(1<<TOIE1)
out	TIMSK,temp
ret

TIMER0_INIT:		;for 10ms timer tick

;set prescaler to 256
	ldi	temp,(5<<CS00)	;7812.5 Hz
	out	TCCR0,temp
;load for	0.01s 
	ldi	temp,0xB1
	out	TCNT0,temp
;enable overflow interrupts		;Read, modify, write he grumped.
	in	temp,TIMSK
	ori	temp,(1<<TOIE0)	
	out	TIMSK,temp
;set elapsed time to zero
	ldi	temp,0x00
	sts	time,temp
	sts	time+1,temp
	sts	time+2,temp
	sts	time+3,temp
	sts	time+4,temp
	ret



EEPROM_STRING:		;address in temp,temp1
cli
call	TAB
rcall	EEPROM_read
adiw	YH:YL,1
cpi	char,0x00		;null terminated
brne	PRINT_CHAR
sei
ret	
PRINT_CHAR:
call	OTHER_WAY_IN
rjmp	EEPROM_STRING


EEPROM_read:
sbic EECR,EEWE		; Wait for completion of previous write
rjmp EEPROM_read
out EEARH, YH		; Set up address (r18:r17) in address register
out EEARL, YL
; Start eeprom read by writing EERE
sbi EECR,EERE
; Read data from data register
in char,EEDR
ret

EEPROM_write:
sbic EECR,EEWE ;if EEWE not clear
rjmp EEPROM_write ; wait more
out EEARH,YH ;output address high byte, remove if no high byte exist
out EEARL,YL ;output address low byte
out EEDR,char ;output data
cli ;disable global interrupts
sbi EECR,EEMWE ;set master write enable, remove if AT90S1200 is used
sbi EECR,EEWE ;set EEPROM Write strobe
sei ;enable global interrupts
ret





