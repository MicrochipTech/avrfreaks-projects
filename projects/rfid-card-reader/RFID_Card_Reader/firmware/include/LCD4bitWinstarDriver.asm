
;4 bit WINSTAR LCD driver
;Delay is tested on 11Mhz
;NON intrusive driver -> NO 'out' instruction on DATA port!

.equ	LCD_RS	= 0   ;PORTA.0
.equ	LCD_RW	= 1   ;PORTA.1
.equ	LCD_E	= 2   ;PORTA.2

.equ	DATA4	= 4	; PORTA.4
.equ	DATA5	= 5	; PORTA.5
.equ	DATA6	= 6	; PORTA.6
.equ	DATA7	= 7	; PORTA.7


.equ LCD_DATA = PORTA
.equ LCD_DATA_DIR = DDRA
.equ LCD_DATA_IN=PINA

.equ LCD_CNTRL = PORTA
.equ LCD_CNTRL_DIR = DDRA


.equ LCD_FUNCTION_SET   = 0b00101000;function set,4-bit transfer,2-lines display & 5*8 dot characteristic
.equ LCD_DISPLAY_SHIFT_RIGHT =0b00011100
.equ LCD_DISPLAY_SHIFT_LEFT =0b00011000

.equ LCD_DISPLAY_OFF    = 0x08 
.equ LCD_DISPLAY_ON     = 0x0C ;hide cursor and blinking
.equ LCD_DISPLAY_CLEAR  = 0x01 
.equ LCD_DISPLAY_HOME  = 0x02 
.equ LCD_ENTRY_MODE_SET = 0x06 
.equ LCD_CURSOR_HOME    = 0x02 
.equ LCD_CURSOR_ON    = 0x0E  


.equ LCD_LINE_1      =0x80
.equ LCD_LINE_2      =0xC0 

;*****************Wait for response from LCD********************
;@USAGE: return
;******************************************************

lcd4_wait:
	;make sure the data lines are inputs	
	cbi LCD_DATA_DIR,DATA7
	cbi LCD_DATA_DIR,DATA6
	cbi LCD_DATA_DIR,DATA5
	cbi LCD_DATA_DIR,DATA4

	cbi	LCD_CNTRL, LCD_RS		;RS=0
	sbi	LCD_CNTRL, LCD_RW		;R/W=1
	

lcd4bs:
	sbi	LCD_CNTRL, LCD_E		;while E is high
	nop
	nop
	in	return, LCD_DATA_IN 
	clt
	sbrc return,DATA7
	set
	cbi	LCD_CNTRL, LCD_E		;while E is low
	
	sbi	LCD_CNTRL, LCD_E		;dummy
	cbi	LCD_CNTRL, LCD_E
	nop
	brts lcd4bs
	
	;back to output
	sbi LCD_DATA_DIR,DATA7
	sbi LCD_DATA_DIR,DATA6
	sbi LCD_DATA_DIR,DATA5
	sbi LCD_DATA_DIR,DATA4

ret

;******************Init LCD in 4 bit mode**************
;@USAGE: temp,argument -command
;@WARNING: look at sub calls reg usage
;**********/

lcd4_init:
	rcall lcd4_wait_15ms 
	rcall lcd4_wait_15ms

	sbi LCD_CNTRL_DIR,LCD_E
	sbi LCD_CNTRL_DIR,LCD_RW
	sbi LCD_CNTRL_DIR,LCD_RS

	sbi LCD_DATA_DIR,DATA7
	sbi LCD_DATA_DIR,DATA6
	sbi LCD_DATA_DIR,DATA5
	sbi LCD_DATA_DIR,DATA4

	ldi	argument,0x00
	rcall lcd4_instruction

	ldi	argument,0x00
	rcall lcd4_instruction

	ldi	argument,0x00
	rcall lcd4_instruction

	ldi	argument,0x00
	rcall lcd4_instruction

	ldi	argument,0x00
	rcall lcd4_instruction

	ldi argument,0x20
	rcall lcd4_instruction

	ldi argument,LCD_FUNCTION_SET
	rcall lcd4_command

	ldi argument,LCD_DISPLAY_ON
	rcall lcd4_command
	
	ldi argument,LCD_ENTRY_MODE_SET
	rcall lcd4_command

	ldi argument,LCD_CURSOR_HOME
	rcall lcd4_command
	
	
	ldi argument,LCD_DISPLAY_CLEAR
	rcall lcd4_command

ret

;********************Write instruction**********
;@INPUT: argument -command
;@USAGE: argument
;***********************************************

lcd4_instruction:
    ;strob control 
	cbi LCD_CNTRL,LCD_E
	cbi LCD_CNTRL,LCD_RW
	cbi LCD_CNTRL,LCD_RS
	
	andi argument,0xF0
	rcall write_io
	
	sbi	LCD_CNTRL, LCD_E		;set strobe E
	rcall lcd4_wait_1us
	cbi	LCD_CNTRL, LCD_E        ;unset E
	rcall lcd4_wait_1us
	 

ret

;****************Write 4 bits in IO port*******************
;write 4 bits of data in IO port without touching the other 4 bits
;@INPUT: argument
;**********************************************************

write_io:
	cbi LCD_DATA,DATA7
	sbrc argument,7
	sbi LCD_DATA,DATA7

	cbi LCD_DATA,DATA6
	sbrc argument,6
	sbi LCD_DATA,DATA6

	cbi LCD_DATA,DATA5
	sbrc argument,5
	sbi LCD_DATA,DATA5

	cbi LCD_DATA,DATA4
	sbrc argument,4
	sbi LCD_DATA,DATA4

ret

;*****************Write command******************
;@USAGE:argument
;@STACK:
;************************************************

lcd4_command:
	push argument
	;strob control 
	cbi LCD_CNTRL,LCD_RW
	cbi LCD_CNTRL,LCD_RS

	andi argument,0xF0
	rcall write_io


	sbi	LCD_CNTRL, LCD_E		;set strobe E
	rcall lcd4_wait_1us
	cbi	LCD_CNTRL, LCD_E        ;unset E
	rcall lcd4_wait_1us

	pop argument
	swap argument
	andi argument,0xF0
	rcall write_io


	sbi	LCD_CNTRL, LCD_E		;set strobe E
	rcall lcd4_wait_1us
	cbi	LCD_CNTRL, LCD_E        ;unset E
	rcall lcd4_wait_1us

	rcall lcd4_wait

ret

;*******************Write character*****************
;@INPUT: argument - char
;@USAGE: argument
;@STACK:1 level
;**************************************************

lcd4_putchar:

	sbi	LCD_CNTRL, LCD_RS		;now take RS high for LCD char data register access
	cbi LCD_CNTRL,LCD_RW


	push argument
	andi argument,0xF0
	rcall write_io

	sbi	LCD_CNTRL, LCD_E		;strobe Enable
	nop
	cbi	LCD_CNTRL, LCD_E

	pop argument
	swap argument
	andi argument,0xF0
	rcall write_io

	sbi	LCD_CNTRL, LCD_E		;strobe Enable
	nop
	cbi	LCD_CNTRL, LCD_E

	rcall lcd4_wait

ret

;*************************************************************************
;				Display byte as 1..3 digits 0..255
;check for leading zeros and remove them using T flag in SREG
;@INPUT:argument
;@USAGE:temp,argument
;STACK: 1 level
;*************************************************************************

lcd4_dec_out:
	set   ;used to fascilitate leading ziro removal
	ldi temp, -1 + '0' 

_ask1: 
	inc temp 
	subi argument, 100 
	brcc _ask1
;write out first digit
	push argument
	mov argument,temp
;no need of leading ziro
	cpi argument,'0'
	breq _ask11 
	rcall lcd4_putchar
	clt 

_ask11:		 
	pop argument

	ldi temp, 10 + '0' 

_ask2: 
	dec temp 
	subi argument, -10 
	brcs _ask2
	sbci argument, -'0' 
;write out second digit
	push argument
	mov argument,temp           
;test for leading zero - if T is clear stop testing - it is not leading zero
	brtc _ask222
	cpi argument,'0'		 
	breq _ask22         

_ask222:		 
	rcall lcd4_putchar

_ask22:		 
	pop argument
;write out third digit
	rcall lcd4_putchar
	ret  
  
;------------------------------------------------------
;Input buffer in decimal format.Convert to Ascii
;INPUT: argument
;USAGE:	argument,temp
;STACK: 1 level
;------------------------------------------------------

lcd4_hex_out:
	push argument
	ldi argument,'0'
	rcall lcd4_putchar
	ldi argument,'x'
	rcall lcd4_putchar
	pop argument

       
	push argument
	andi argument,0xF0
	swap argument       
	cpi argument,10       
	brlo lao1
	ldi temp,0x37
	rjmp lao2

lao1:	   
	ldi temp,0x30

lao2:	   	    
	add argument,temp
	rcall lcd4_putchar

	   
	pop argument
	andi argument,0x0F
	cpi argument,10       
	brlo lao3
	ldi temp,0x37
	rjmp lao4

lao3:	   
	ldi temp,0x30

lao4:	   	    	   
	add argument,temp
	rcall lcd4_putchar 

ret
;-------------------------------------------------------------------------
;				Input buffer packed BCD 
;@INPUT:argument
;@USAGE:temp,argument
;@STACK:1 level
;-------------------------------------------------------------------------

lcd4_bcd_out:
	ldi temp,0x30
	push argument
	andi argument,0xF0
	swap argument
	add argument,temp
	rcall lcd4_putchar

	ldi temp,0x30
	pop argument
	andi argument,0x0F
	add argument,temp
	rcall lcd4_putchar 

ret
;------------------------------------------------------
;Input buffer packed BCD remove leading rezo if present
;@INPUT:argument
;@USAGE:temp,argument
;@STACK: 1 - level
;------------------------------------------------------

lcd4_bcd_out_remove_leading_zero:
	ldi temp,0x30
	push argument
	andi argument,0xF0
	swap argument
	tst argument
	breq lcdnlz1
	add argument,temp
	rcall lcd4_putchar

lcdnlz1:
	ldi temp,0x30
	pop argument
	andi argument,0x0F
	add argument,temp
	rcall lcd4_putchar 

ret
;**********************MIND THE CPU Frequency************
;@USAGE:t1,t2,temp
;********************************************************

lcd4_wait_15ms:               
	ldi  temp,0x60
	mov  t1,temp 

lcd4wt0:     
	ldi  temp,0xFF
	mov t2,temp 

lcd4wt1: 
	dec  t2  
	brne lcd4wt1 
	dec  t1 
	brne lcd4wt0 
	ret 
;**********************MIND THE CPU Frequency************
;@USAGE:t1,t2,temp
;********************************************************

lcd4_wait_1ms:               
	ldi  temp,0x08
	mov  t1,temp 

lcd4wt2:     
	ldi  temp,0xFF
	mov t2,temp 

lcd4wt3: 
	dec  t2 
	brne lcd4wt3 
	dec  t1 
	brne lcd4wt2 

ret

//**********************MIND THE CPU Frequency************
//
//********************************************************

lcd4_wait_1us:               
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	ret  