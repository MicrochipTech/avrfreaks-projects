; LCD subroutine test

; include files
.include	"m8515def.inc"		; include file for ATmega8515 AVR

; equates
.equ LCDRS 	= 5					; LCD reg select pin, command/data
.equ LCDWR 	= 6					; LCD read/write pin
.equ LCDCLK	= 7					; lcd clock input pin (neg clk edge)

reset:	
	ldi		r16,high(RAMEND)	; stack setup
	out		SPH, r16			;
	ldi		r16,low(RAMEND)		;
	out 	SPL, r16			;
	ser		r16					;
	out		DDRC, r16			; setup PORTC for data out
	ldi 	r16, 0xF0			; PORTD pin 5,6,7 for data out, pin4 for +5V
	out 	DDRD, r16			;									to LCD
	rcall 	lcd_init			; initialize lcd display

;*********
; main:	this program simply uses lcd_cmd and lcd_prt to print "testuppr" and 
;		"testlowr" on a 2-line character LCD display.

main:
	ldi 	r16, 0x00|0x80		; place cursor in 1st row, 1st char
	rcall 	lcd_cmd				; 
	ldi 	r16, 't'	
	rcall 	lcd_prt	
	ldi 	r16, 'e'	
	rcall 	lcd_prt	
	ldi 	r16, 's'	
	rcall 	lcd_prt	
	ldi 	r16, 't'	
	rcall 	lcd_prt	
	ldi 	r16, 'u'	
	rcall 	lcd_prt	
	ldi 	r16, 'p'	
	rcall 	lcd_prt	
	ldi 	r16, 'p'	
	rcall 	lcd_prt	
	ldi 	r16, 'r'	
	rcall 	lcd_prt	
		
	ldi 	r16, 0x40|0x80		; place cursor in 2nd row, 1st char
	rcall 	lcd_cmd				; 
	ldi 	r16, 't'	
	rcall 	lcd_prt	
	ldi 	r16, 'e'	
	rcall 	lcd_prt	
	ldi 	r16, 's'	
	rcall 	lcd_prt	
	ldi 	r16, 't'	
	rcall 	lcd_prt	
	ldi 	r16, 'l'	
	rcall 	lcd_prt	
	ldi 	r16, 'o'	
	rcall 	lcd_prt	
	ldi 	r16, 'w'	
	rcall 	lcd_prt	
	ldi 	r16, 'r'	
	rcall 	lcd_prt	

end:
	rjmp	end


;*********************************************************************************
;                             	   lcd sub-routines

; OVERALL DESCRIPTION: This collection of routines writes commands and data
; to the Hitachi HD44780 LCD controller. Please refer to it's instruction set 
; for a description. 
; PORTD is the 8 DATA lines to the LCD and PORTC pins 0,3 are the control.
; For this controller it is possible to use 4-line DATA mode, saving 4 AVR pins.

;*********
lcd_init:					; lcd initialization routine 
	ldi 	r16, 0x38		; lcd function set reg:	8 bit data lines, 5x7 font
	rcall 	lcd_cmd			;						2 line display
	ldi 	r16, 0x0C		; lcd display on/off reg: on, show cursor, blink
	rcall 	lcd_cmd			;
	ldi 	r16, 0x01		; lcd reg: clear display, cursor home
	rcall	lcd_cmd			;
	ldi 	r16, 0x06		; lcd entry mode register: auto-increment cursor
	rcall	lcd_cmd			;
	ret	

;*********
; lcd_prt:	this subroutine prints to the LCD an ascii character in r16.
;			note that r16 data sent to this subroutine is lost during execution.
;			This can be fixed by using stack operations on r16 before and after 
;			calling lcd_cmd.

lcd_prt:					; lcd write data routine (data is in r16)
	sbi 	PORTD, LCDCLK	; set lcd enable pin (simulate pos clk edge)
	sbi 	PORTD, LCDRS	; set RS to enable lcd data register 
	out 	PORTC, r16		; put data on bus
	cbi 	PORTD, LCDCLK	; clear lcd enable (simulate neg clk edge)
	rjmp	lcd_wait		; go to check lcd busy flag routine
	ret

;*********
; lcd_cmd:	this subroutine sends instructions to the LCD from r16. These include
;			everything from character placement to custom characters.
;			Note r16 data sent to this subroutine is lost during execution.
;			This can be fixed by using stack operations on r16 before and after 
;			calling lcd_cmd.

lcd_cmd:					; write command to LCD routine (command is in r16)
	sbi 	PORTD, LCDCLK	; set lcd enable pin (simulate pos clk edge)
	cbi 	PORTD, LCDRS	; clear RS to enable lcd command register 
	out 	PORTC, r16		; put data on bus
	cbi 	PORTD, LCDCLK	; clear lcd enable (simulate neg clk edge)
	rjmp	lcd_wait		; go to check lcd busy flag routine
	ret	

;*********
; lcd_wait:	because the LCD has a much slower clock than the AVR, this subroutine
;			checks the LCD's busy-flag to make sure the last instruction was 
;			completed before continuing.
;			Stack operations are used within this subroutine therefore saving 
;			the need for use of a second register.

lcd_wait:					; wait for lcd busy flag routine
	cbi 	PORTD, LCDRS	; clear RS to enable lcd command register 
	sbi 	PORTD, LCDWR	; set RW for lcd read data enable
	sbi 	PORTD, LCDCLK	; set lcd enable pin
	clr 	r16				; 
	out 	DDRC, r16		; configure data port as input
	out 	PINC, r16		; clear data on input pins
	in  	r16, PINC		; get the address and busy flag
	cbi 	PORTD, LCDCLK	; clear lcd enable (simulate neg clk edge)
	push	r16
	ser 	r16				;
	out		DDRC, r16		; PORTC as output again
	cbi 	PORTD, LCDWR	; clear RW again
	pop		r16
	sbrc 	r16, 7			; if busy flag cleared, return  
	rjmp 	lcd_wait		; else repeat read/check
	ret						; return when busy flag cleared
