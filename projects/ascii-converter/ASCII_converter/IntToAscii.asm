; ******************************************************************************
; Author : Nay Oo
; DESCRIPTION :
;				- This program convert an integer number to ASCII
;				  AVR Newbie Learning program
;				- The integer Range (0..127) 
;				- You can use AVRStudio 4 simulator to see the register
;				  contents (recommended)
;
; uController : AT90S8515
; Development Board : STK500
; 
;
;
; AUTHOR NOTE:
;				This file was written by me to educate myself in AVR
;				Initial ideas was gathered from the http://www.avrbeginners.net/
;				Please do refer to them for more indept theroritical aspects.
;
;				After the convertion is done, you can send these ASCII coded
;				numbers to UART,USB, LCD interface, etc.
;
;				If you have any thing to discuss about this file, you can contact
;               me via my personal email : nayoo1979@yahoo.com.sg
;
; ******************************************************************************

;* Include Microcontroller Device specific file
.include "8515def.inc"

;* Register Variable Declaration Here
.def	temp   = r16				; temporary storage register
.def	intval = r17				; the variable which stores the integer value
.def	HundredCounter = r18		; eg. 127 = '1'
.def	TenthCounter = r19			; eg. 127 = '2'
.def	OneCounter = r20			; eg. 127 = '7'

;* Integer Value 
.equ	Number=127					; You can change any number from 0 to 127


.org 0x0000
	rjmp	RESET					; program reset vector 


;* Integer to ASCII converter subrountine here
IntToASCII:
	cpi	    intVal,100					;
	brge	DivideBy100					; jump if (intVal>=100) 
	cpi		intVal,10
	brge	DivideBy10					; jump if (intVal>=10)
	mov		OneCounter,IntVal
	add		HundredCounter,temp			; here, we got there single digits
	add		TenthCounter,temp			; from that, we convert them to
	add		OneCounter,temp				; ASCII values !!!
	ret

DivideBy100:
	subi	intVal,100
	inc		HundredCounter
	rjmp	IntToASCII

DivideBy10:
	subi	intVal,10
	inc		TenthCounter
	rjmp	IntToASCII


;* Main Program Starts here
RESET:
	ldi		temp,low(RAMEND)			; STACK pointer location 
	out		SPL,temp					; started from the
	ldi		temp,high(RAMEND)			; end of SRAM
	out		SPH,temp					; RAMEND is declared in 8515def.inc file

	ldi		HundredCounter,0			; set counters values to zero
	ldi		TenthCounter,0				;	
	ldi		OneCounter,0				;
	ldi		temp,48						; ASCII = Number + 48
										; In ASCII table, we can see that '0' is 48 (0x30)
										; therefore, need to add 48 to the single digits
		
	ldi		intVal,Number
	rcall	IntToASCII					; subroutine call
										; which convert integer value to
										; ASCII value

forever: rjmp 	forever					; infinite loop