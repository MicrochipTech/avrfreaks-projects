;*****************************************************************
; Project		: HD44780 compatible LCD Driver with Dallas DS2408
;                 1-Wire,8-Channel addressable Switch.
;
;                 ------------------------------------------
;				  -> ROM-Code of DS2408 must not be known <-
;                 ------------------------------------------
;
; Target CPU	: all AVR´s with SRAM
;				  Tested with 1 MHz up to 8 MHz Clock
;
; Test Setup    : AVR Studio V4.12,SP4 connected to STK500
; 
; Date          : 28.02.2007
;
; Author        : Robert Ibener (r.ibener@inode.at)
;
;                 1-wire Code "1wire_11mhz.inc"written
;				  by Peter Dannegger(danni@specs.de)
;
; Things you must adapt to your Hardware : 
;
;           ->	Change Constant "xtal" in file "1wire_lcd.h"
;               to your actual CPU-Clock.
;			->	Change the Cal-Byte according to your CPU
;				(can be eliminated, when your AVR-Device
;				load the CAL-Byte automatically after a
;				Power-On-Reset)
;
;				For Hardware-Connection,Register-Definitions and
;				Constants see file "1wire_lcd.h"    
;*****************************************************************

.nolist
.include "tn45def.inc"
.list
.include "1wire_lcd.h" 


.CSEG
.ORG 0x0000			;Start of Flash-RAM
	rjmp	RESET	;Reset Handler
	reti			;INT0 (External Interrupt Request_0) Handler
	reti			;PCINT0 (Pin Change Interrupt Request_0) Handler
	reti			;Timer1_Comp_A (Timer1 Compare Match A) Handler
	reti			;Timer1_OVF (Timer1 Overflow) Handler
	reti			;Timer0_OVF (Timer0 Overflow) handler
	reti			;EE_RDY (EEPROM Ready) Handler
	reti			;ANA_Comp (Analog Comparator) Handler
	reti			;ADC (ADC Conversion complete) Handler 
	reti			;Timer1_Comp_B (Timer1 Compare Match B) Handler
	reti			;Timer0_Comp_A (Timer0 Compare Match A) Handler
	reti			;Timer0_Comp_B (Timer0 Compare Match B) Handler
	reti			;WDT (Watchdog Time-out) Handler
	reti			;USI-Start (USI Start) Handler
	reti			;USI_OVF (USI Overflow) Handler
		
;xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
;Start Program
;xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

RESET:
	;ldi a0,0xXX	;Load OSCCAL-Value
	;out OSCCAL,a0
	ldi		a1,low(RAMEND)
	ldi		a0,high(RAMEND)
	out		SPL,a1
	out		SPH,a0
	ldi		a1,led_off			;Setup Error-LED on PB3
	out 	led_ddr,a1
	out		led_port,a1
	rcall	Init_2408
	rcall	Init_Display

;***********************************
; Output some Test-Data
;***********************************

Loop:
	rcall 	LCD_Line1			;set LCD-Cursor to Line 1
	ldi		ZH,high(2*Text1)	;set pointer to text	
	ldi 	ZL,low(2*Text1)	    
	rcall 	LCD_Send_String		;Output to LCD
	rcall 	LCD_Line2			;set LCD-Cursor to Line 2
	ldi 	ZH,high(2*Text2)	
	ldi 	ZL,low(2*Text2)	    
	rcall 	LCD_Send_String		;Output to LCD
	ldi	a0,50
wait1:							;wait 1 second to show text on lcd
	rcall	Del20ms
	dec a0
	brne wait1
	rcall 	LCD_Line1			;show second part on lcd
	ldi		ZH,high(2*Text3)
	ldi 	ZL,low(2*Text3)
	rcall 	LCD_Send_String
	rcall 	LCD_Line2
	ldi		ZH,high(2*Text4)
	ldi 	ZL,low(2*Text4)
	rcall 	LCD_Send_String
	ldi	a0,50
wait2:							;wait 1 second to show text on lcd
	rcall	Del20ms
	dec a0
	brne wait2		
	rjmp loop

;------------------------------------------------
;      write content of "Data" to DS2408 I/O-Pins
;------------------------------------------------
Out_DS2408:
	rcall	Select_DS2408
	ldi	a0,CHANN_ACC_WRITE	;Command:Write to PIO-Pins
	rcall w1_byte_wr
	mov	a0,Data				;send Data to I/O-Pins
	rcall w1_byte_wr
	mov a0,Data
	com	a0					;send complement Data
	rcall w1_byte_wr
	rcall w1_byte_rd		;DS2408 sends Strobe		
	ret

;------------------------------------------------
;  Send out Command/Character to the 1Wire-LCD
;------------------------------------------------
W1_LCD_Char:
	ldi     temp,0x10	; Character.R/S = H
	rjmp	Disp_Out
W1_LCD_CMD: 
	ldi     temp,0x00	; Command.R/S = L 
Disp_Out:
	mov 	Save, Data 	; first save Data
	mov		a0,Save
	swap 	a0			; exchange Nibbles
	andi	a0,0x0f		; clear upper nibble
	or 		a0,Temp		; set/clear R/S
	mov		Data,a0
	rcall	Out_DS2408	; send high nibble
	mov 	a0,Save 	; restore Data
	andi	a0,0x0f		; process low nibble
	or 		a0,temp		; set/clear R/S
	mov		Data,a0
	rcall	Out_DS2408	; send low nibble
	mov		Data,Save 
	ret

;------------------------------------------------
;      Send unique Romcode to DS2408
;------------------------------------------------
Select_DS2408:
	rcall	Wire_Reset
	ldi	a0,MATCH_ROM				;Match ROM Command
	rcall w1_byte_wr
	ldi		a3,ROMCODE_SIZE			;load Byte Counter
	ldi		ZL,low(romcode_ds2408)	;start with Device-Code
	ldi		ZH,high(romcode_ds2408)
writerom:
	ld	a0,z+
	rcall	w1_byte_wr		;Write Byte to 1Wire-Line
	dec		a3				;all 8 Bytes done?
	breq	exit_out_rom	;yes
	rjmp	writerom		;no, do next Byte
exit_out_rom:
	ret

;-------------------------------------------------
; Perform 1-Wire Reset and check for Presence-Puls
;-------------------------------------------------
Wire_Reset:
	rcall	w1_reset		;reset 1-Wire Line
	in	a0,SREG
	sbrs	a0,0			;Carry set?
	rjmp	Error1			;No Device on Bus detected!
	ret

;-------------------------------------------------
;                   Error-Handler
;-------------------------------------------------
Error1:				;No Device on Bus!
	nop				;here you can insert your Error-Routine
Err_Out:
	ldi a0, led_on 
	out led_port,a0	;switch on Error-LED on PB3
Err_Loop:
	nop				;here you can insert your Error-Routine
	rjmp Err_Loop	;stop Program
	

;------------------------------------------------
;      Clear LCD Display
;------------------------------------------------
LCD_Clear:
	ldi   Data,0x01	;Clear Display
	rcall W1_LCD_CMD
	rjmp  Del5ms		;5 msec Delay and return


;------------------------------------------------
;     Goto LCD Line1/Line2/Line3/Line4
;------------------------------------------------
LCD_Line1:
		ldi  Data,0x80	;Bit 7 is allways "1" !
		rjmp W1_LCD_CMD
; Comment out the following Lines if only 1-Line LCD is used
;---------------------------------------------
LCD_Line2:
		ldi  Data,0xC0
		rjmp W1_LCD_CMD
; Comment out the following Lines if only 2-Line LCD is used
;---------------------------------------------
;LCD_Line3:
;		ldi  Data,0x94
;		rjmp W1_LCD_CMD
;LCD_Line4:
;		ldi  Data,0xD4
;		rjmp W1_LCD_CMD

;------------------------------------------------
;   Sends a Text-String pointed to by ZL:ZH
;   String ->MUST<- be Zero (0x00)-terminated
;------------------------------------------------
LCD_Send_String:	
	lpm	  Data,Z			; Load byte pointed to by ZH:ZL	
	tst	  Data				; Check if we've reached the end of the message	
	breq  LCD_String_done	; If so, terminate
	push ZH					; save Z
	push ZL	
	rcall W1_LCD_Char		; send Character to LCD
	pop	ZL					; restore Z
	pop ZH
	adiw	ZH:ZL,1			; point to next byte
	rjmp  LCD_Send_String
LCD_String_done:
	ret

Init_2408:
;------------------------------------------------
;          Read ROM-Code,Select and Configure DS2408
;------------------------------------------------
	rcall	Wire_Reset			;check if any Device present
	ldi		a0, search_first	;at least 1 Device present	
_search1:
	rcall	w1_rom_search
	mov		a3, a0
	ldi		zl, romcode_ptr
	ld		a1, z				;get device code
	cpi		a1, 0x29			;is it DS2408?
	breq	_search2			;yes				
	mov	a0, a3
	cpi		a0, LAST_DEVICE		;more 1Wire devices on Bus?
	brne	_search1			;yes,look for next Device
_search2:
	cpi		a1, 0x29			;any DS2408 found ?
	brne	Error1				;no,Error!
;----- DS2408 found and selected,now configure DS2408 ------
	ldi	a0,WR_COND_SEARCH	;Write Conditional Search Registers command
	rcall w1_byte_wr
	ldi	a0,STATUS_REGISTER	;point to Status-Register low Adress
	rcall w1_byte_wr
	ldi	a0,0x00				;point to Status-Register high Adress
	rcall w1_byte_wr
	ldi	a0,1<<ROS			;configure RSTZ-Pin as Strobe
	rcall w1_byte_wr
;---- now save DS2408 ROMCODE for later use -----
	ldi ZH, high(romcode_ptr)		;Source 
	ldi ZL, low(romcode_ptr) 
	ldi XH, high(romcode_ds2408)	;Destination
	ldi XL, low(romcode_ds2408) 
	ldi temp, ROMCODE_SIZE			;Number of Bytes to transfer
MoveROM_Loop: 
	ld save, Z+ 
	st X+, save 
	dec temp 
	brne MoveROM_Loop 
	rjmp w1_reset


;------------------------------------------------
;       initialize LCD after Power-Up
;------------------------------------------------
Init_Display:
	rcall	Del20ms
	ldi 	Data, 0x03		
	rcall	Out_DS2408
	rcall 	Del5ms
	rcall	Out_DS2408
	rcall 	Del5ms
	rcall	Out_DS2408 		
	ldi 	Data, 0x02		;Function Set 4Bit		
	rcall	Out_DS2408
;	ldi 	Data,0x20		;Single Line Display,5x7 Char.Font
	ldi 	Data,0x28		;Multiple Line Display,5x7 Char.Font
	rcall	W1_LCD_CMD
	ldi 	Data,0x08		;Display off
	rcall 	W1_LCD_CMD
	ldi 	Data,0x01		;Display clear
	rcall 	W1_LCD_CMD
	rcall	Del5ms
	ldi 	Data,0x06		;Increment Mode,no Shift 
	rcall 	W1_LCD_CMD
	ldi		Data,0x0C		;Display on,no Cursor,no blink
	rcall	W1_LCD_CMD
	ret	

;------------------------------------------------
;               Delay Routines
;------------------------------------------------
Del50us:						;50us Delay
		ldi  a1, 50 * xtal /1000000 /3 -2
_del5u_1:
		dec  a1
		brne _del5u_1
		ret                          
Del5ms:                         ;5ms Delay
		ldi  a2, 100			;50us x 100=5ms
_del5m_1:
		rcall Del50us
		dec  a2
		brne _del5m_1
		ret
Del20ms:
		ldi	a3,0x04				;4 x 5ms = 20ms
_del20m_1:
		rcall	Del5ms
		dec	a3
		brne	_del20m_1
		ret


Text1:
;    12345678901234567890 = Byte-Count
.db	"*1-Wire*",0,0
Text2:
.db "* LCD  *",0,0
Text3:
.db "*Line 1*",0,0
Text4:
.db "*Line 2*",0,0


.include "1wire_11mhz.inc"

.dseg
.org 0x60
romcode_ptr: 	.byte 8	;ROMCode of any 1-Wire Device
						;returned by "w1_rom_search" Routine
romcode_ds2408: .byte 8	;Save Place for our DS2408 ROMCode

