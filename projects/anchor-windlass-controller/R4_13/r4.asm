
;********************************************************************************
;* Description:	Winch Controller with AT90S2313 AVR RISC Processor		*
;*      		LCD display, Using 4-wire interface with EEPROM backup.	*
;*										*								                  *	
;* Version:			4.13		     				*					               *
;*                                                                      	*
;* First writing:	07/11/2005						*					            *
;* Last Updated:	3/9/2006						*  				               *
;*										*								                  *
;* Target:			AT 90S2313					*						            *
;*										*								                  *
;* Author:			Bob Charlton                                    *
;* Notes:                                                              		*
;		4.10	Divide by 3 added, with 16 bit arithmetic               *
;		4.11	Stop at 195 ft  added                                   *
;		4.12	2/9/06 Intro screen added                               *
;		4.13	Watchdog Timer added					*				            *
;										*								                  *	
;********************************************************************************
;   Micro I/O
;LCD connections:
; PB6		RS				Pin 18
; PB5		R/W				Pin 17
; PB4		E				Pin 16
; PB3		D7			   	Pin 15
; PB2		D6			    	Pin 14
; PB1		D5			    	Pin 13
; PB0		D4			     	Pin 12

;  Other Connections

;PD0  Lower I/P Active Hi			Pin 2
;PD1  Raise I/P Active Hi       		Pin 3
;PD2  Count I/P Hi to Lo 			Pin 6
;PD3  I/P Lo to clear units        		Pin 7
;PD4  Stop O/P active Hi 	(LED D2)  	Pin 8
;PD5  0 ft 			(LED D3)  	Pin 9
;PD6  O/P Micro active Lo 	(LED D1)   	Pin 11
;

;**************  START OF ASSEMBLY LISTING **********************

.nolist					;Suppress listing of include file
.include "2313def.inc"			;Define chip particulars
.list

; **** Global register variables
.def	ldir			=R28	;last direction
.def	Zflag			=R15	;
.DEF  	TEMP  	 		=R16	;General use working register
.def	Dir			=R21	;Current direction - bit 1 set raising,
					; bit 2 set lowering

; **** BCD Conversion register variables

.def 	UnitL		=R17		;capstan reveloutions 1 pulse = 4 inches
.def	UnitH		=R18
.def	rBin2L		=R19
.def	rBin2h		=R20

; **** Divide by 3 Registers
;
.DEF rd1u	 	= R2 		; interim register
.DEF divisor	 	= R3 		; 8-bit-number to divide with
.DEF ResultL	 	= R4 		; LSB result
.DEF ResultH	 	= R5 		; MSB result
.def divendL		= R8
.def divendH		= R9

; **** EEPROM register variables

.def	EEdrd		=R1		;data byte from EEPROM
.def	EEard		=R2
.def	EEdwr		=R22		;data byte to write to EEPROM
.def	EEawr		=R23

; ****  LCD Register assignments

.DEF  PARAM  		= R26  		; Parameter transfer register
.DEF  DELAY  		= R12
.DEF  DELAY1 		= R24		; To generate long delays
.DEF  DELAY2 		= R25
.DEF	CHAR 		= R0
.DEF	STCMD		= R27  	 	; Stop counter

; **** LCD equates

.EQU 	E		= 0b00010000   	; bit 4 Port B LCD strobe lines
.EQU 	RS 		= 0b01000000   	; bit 6 Port B
.equ	counter 	= 0x0001

.cseg
.org 0

;	Interrupt vectors
;
	rjmp 	reset 		;Reset Vector location - 		$0000
	
.org		INT0addr	;External Interrupt0 Vector - 		$0001
	rjmp	Turning
.org		INT1addr	;External Interrupt1 Vector - 		$0002
	reti
.org		ICP1addr	;Input Capture1 Interrupt Vector -	$0003
	reti
.org		OC1addr 	;Output Compare1 Interrupt Vector -	$0004
	reti
.org		OVF1addr	;Overflow1 Interrupt Vector - 		$0005
   	reti
.org		OVF0addr	;Overflow0 Interrupt Vector - 		$0006
	reti	
.org		URXCaddr	;UART Receive Complete Interrupt Vector - $0007
	reti
.org		UDREaddr	;UART Data Register Empty Interrupt Vector - $0008
	reti
.org		UTXCaddr	;UART Transmit Complete Interrupt Vector - $0009
	reti
.org		ACIaddr		;Analog Comparator Interrupt Vector - 	$000A
	reti

;            MAIN PROGRAM
;  program entry point on reset

reset:
 	ldi		temp,RAMEND		;Init Stack Pointer
	out		SPL,temp
	ldi		temp,$00
	mov		zflag,temp
	mov		stcmd,temp
	mov		ldir,temp
	mov		unith,temp
	
	ldi		temp,$10
	rcall		read			;restore "old" units
	mov 		UnitL,temp
	ldi		temp,$11
	rcall		read			;restore "old" units
	mov 		UnitH,temp
	ldi 		TEMP, 0xFF   		; 1111:1111 (FF) setting bits 0-7 as outputs
  	 out 		DDRB, TEMP   		; set Port B direction as above
  	 ldi		TEMP, 0x80   		; 0000:0000,
  	 out 		PORTB, TEMP

	rcall		Blockcopy		;copy last chain reading from EEPROM to RAM
			
;  LCD display Initalize
  	rcall 		lcdinit 		;Initialize LCD module
  	ldi		temp,$FF
	rcall		wait_2M
	ldi		temp,$FF
	rcall		Wait_2M
	ldi		temp, 0x70		; PORTD BITS 4,5,6 AS OUTPUTS
	out		PORTD, temp
	LDI		DIR,0X00
	ldi		temp, 0xF0  		; PORTD BITS 0 - 3 as inputs
	out		DDRD, temp
 	cbi		portd,6     		; clear running light
 		
; Display intro mesage and wait for winch command
Intro:
	ldi 		ZH,high(Intro1*2)
	ldi 		ZL,low(Intro1*2)	;Init Z-pointer
	
next1:
 	lpm 					;Get next character from ROM use Z pointer & put in R2
	tst 		R0 			;See if at end of message
	breq 		Iline2 			;If so, do next message
	mov 		param,R0 		;Send it
	rcall 		LCD_DATA
	adiw 		ZL,1 			;Increment Z-pointer
	rjmp 		next1 			;Loop for more
	
Iline2:
 	ldi 		param,0xC0 		;Set address to line 2
	rcall 		LCD_CMD
	ldi		ZH, high(Intro2*2)	; Start address data mem
	ldi		ZL, low(Intro2*2)
	
next2:
	lpm 					;Get next character from ROM use Z pointer & put in R2
	tst 		R0 			;See if at end of message
	breq 		WStart			;If so, go to wait start
	mov 		param,R0 		;Send it
	rcall 		LCD_DATA
	adiw 		ZL,1 			;Increment Z-pointer
	rjmp 		next2 			;Loop for more
	
WStart:
	IN		DIR,PIND
	andi		dir,3
	tst		dir          		;check for button pushed to reset count
	breq		WStart         		; No, loop
	ldi 		PARAM, 0b00000001   	; Display clear
  	rcall 		LCD_CMD
  	ldi		temp,120			
  	rcall 		WAIT_2M         	; Wait 2 msec after clear
  	sbi		portd,6     		; clear running LED

; Enable Watchdog timer
	ldi		temp,0b00001101
	wdr
	out		WDTCR,temp

;Enable Int0
	ldi		temp,$03		;set interrupt control register for rising edge
	out		MCUCR,temp
	ldi		temp,$40		;set int mask register for Int 0
	out		GIMSK,temp			
	sbi 		ACSR,ACD 		;shut down comparator to save power
	sei					;enable interrupts
	
;*****************************************************************************
;           Main Program Loop
;*****************************************************************************	
	
Main:
  	IN		DIR,PIND        ;get direction
	andi		dir,3           ; mask off high bits
	tst		UnitL		; 0ft ?
	brne		M1
	tst		zflag		;1st. time through?
	brne		M3              ;No - then skip
	cbi		PORTD,5 	; Set PortD low energize 0 ft relay (inhibit anchor raise)
	ldi		temp,$01 	;Set 0ft flag
	mov		ZFlag,temp
	rjmp		M3
	
M1:
	tst		zflag
	breq		M3
	sbi		PORTD,5 	;close 0 ft Relay
	ldi		temp,$00
	mov		zflag,temp
	                                  	
 M3:
	ldi 		PARAM, 0b00000010   	; Display home
  	rcall	 	LCD_CMD			;reset LCD to line 1
   	ldi		temp,40
   	rcall 		WAIT_2M        		; Wait after clear

   rcall		DirInd
	ldi 		param,0x80 		;Set address to line 1
	rcall 		LCD_CMD
	rcall		divide3			;divide "rev" by 3 and put in "Unit"
	rcall		Bin2ToAsc		;Convert Units to ASCII
	
	cpi		DIR,0
	breq		StopCmd
	cpi		Dir,1			;Check direction
	breq		Lower
	cpi		Dir,2
	breq		Raise
	cpi		dir,3
	breq		error

;****************************************************************
	
nextc:
	lpm 					;Get next character from ROM use Z pointer & put in R2
	tst 		R0 			;See if at end of message
	breq 		line2 			;If so, do next message
	mov 		param,R0 		;Send it
	rcall 		LCD_DATA
	adiw 		ZL,1 			;Increment Z-pointer
	rjmp 		nextc 			;Loop for more

line2:
	ldi 		param,0xC0 		;Set address to line 2
	rcall 		LCD_CMD
	ldi		ZH, high(00)		; Start address data mem
	ldi		ZL, low($70)

nextc2:
	LD		PARAM,Z			;Get next character from ROM use Z pointer & put in R2
	tst 		PARAM 			;See if at end of message
	breq 		main			;If so, refresh display
	rcall 		LCD_DATA
	adiw 		ZL,1 			;Increment Z-pointer
	rjmp 		nextc2 			;Loop for more
	
TWENTY:
	cpi		stcmd,60 		;when lowering stop winch
	BRLT		BACK 			;every 20 ft
	cbi		portd,4     		;Stop pulse
	ret
	
One95:
	mov		temp,resultl 		;get current chain reading
	cpi		temp,$C3     		;less than 195 ft then skip
	brlo		back
   	cbi		portd,4  		;stop lowering chain
Back:
	ret	
	
StopCmd:
	SBI		PORTD,4        		 ; Stop command
	clr		STCMD			 ;PD3 input clear command, test bit 3
; 						reset counters on PB								
	sbic		PIND,3          	;check for button pushed to reset count
	rjmp		Store1         		; No check to see if current value saved in EEPROM
  	ldi		UNITL,0			;Clear units
	ldi		UnitH,0
	ldi 		ZH,high(Stopped*2)
	ldi 		ZL,low(Stopped*2)	;Init Z-pointer to Stopped message
	rjmp		nextc
	
Lower:
	RCALL	  	TWENTY 			; stop every 20 ft when lowering
	rcall		One95			; check for end of chain
 	ldi 		ZH,high(Lowering*2)
	ldi 		ZL,low(Lowering*2)	;Init Z-pointer to lowering message
	RJMP		nextc

Raise:
  	ldi 		ZH,high(Raising*2)
	ldi 		ZL,low(Raising*2)	;Init Z-pointer to raising message
	RJMP		nextc
	
Error:
	ldi		ZH, high(MERROR*2)  	;init Z-pointer to Error message
	ldi		ZL, low(MERROR*2)
	rjmp		nextc
	
Store1:
	rcall		write			;store chain length in EEPROM if changed
	ldi		ZH, high(Stopped*2)
	ldi		ZL, low(Stopped*2)
	rjmp		nextc

;****************************************
; 			Subroutines
;
; 2.3 ms delay with 1 MHz clock.
;
WAIT_2M:
  	 mov 		DELAY1, TEMP
W2_1:
  	 clr 		DELAY2
  	 dec 		DELAY2         		; Start at 0xFF
W2_2:
  	 dec 		DELAY2
  	 brne		W2_2
  	 dec 		DELAY1
  	 brne		W2_1
  	 ret

;Set Direction Indicator	
DirInd:
	LDI		ZL,$7C
	LDI		ZH,00
	LDI		temp,01
	cp		ldir,temp
	BREQ		Minus
	ldi		temp,$2D		; "+"
	rjmp		Dend1
	
Minus:  	
	ldi		temp,$2B		; "-"
	ST		Z,temp
	
Dend1:
	ST		Z+,temp
	ldi		temp,03
	and		temp,UnitL
	subi 		temp,-'0' 		; Add ASCII-0
	st 		Z+,temp 		; store and inc pointer
		
dir0:
	RET
	
;*******************************************	
;Binary to ASCII  Conversion Routines
;*******************************************
; Bin2ToAsc
; =========

Bin2ToAsc:
	LDI		ZL,0x73
	LDI		ZH,0x00			;Init Z-pointer
	rcall 		Bin2ToAsc5 		; Convert binary to ASCII
	ldi 		temp,5 			; Counter is 5
	mov 		rBin2L,temp

Bin2ToAsca:
	dec 		rBin2L 			; decrement counter
	ld 		temp,Z+ 		; read char and inc pointer
	cpi 		temp,' ' 		; was a blank?
	breq 		Bin2ToAsca 		; Yes, was a blank
	sbiw 		ZL,1 			; one char backwards
	ret 					; done
;--------------------------------------------------------
; Bin2ToAsc5
; ==========

Bin2ToAsc5:
	rcall 		Bin2ToBcd5 		; convert binary to BCD
	ldi 		temp,4			; Counter is 4 leading digits
	mov 		rBin2L,temp

Bin2ToAsc5a:
	ld 		temp,Z 			; read a BCD digit
	tst 		temp 			; check if leading zero
	brne 		Bin2ToAsc5b 		; No, found digit >0
	ldi 		temp,' ' 		; overwrite with blank
	st 		Z+,temp 		; store and set to next position
	dec 		rBin2L 			; decrement counter
	brne 		Bin2ToAsc5a 		; further leading blanks
	ld 		temp,Z 			; Read the last BCD

Bin2ToAsc5b:
	inc 		rBin2L 			; one more char

Bin2ToAsc5c:
	subi 		temp,-'0' 		; Add ASCII-0
	st 		Z+,temp 		; store and inc pointer
	ld 		temp,Z 			; read next char
	dec 		rBin2L 			; more chars?
	brne 		Bin2ToAsc5c 		; yes, go on
	sbiw 		ZL,3 			; Pointer to beginning of the BCD
	ret 					; done

;--------------------------------------------------------------
;From binary to BCD
;
; Bin2ToBcd5
; ==========

Bin2ToBcd5:
	push 		ResultH			; Save number
	push 		ResultL
	ldi 		temp,HIGH(10000) 	; Start with tenthousands
	mov 		rBin2H,temp
	ldi 		temp,LOW(10000)
	mov 		rBin2L,temp
	rcall 		Bin2ToDigit 		; Calculate digit
	ldi 		temp,HIGH(1000) 	; Next with thousands
	mov 		rBin2H,temp
	ldi 		temp,LOW(1000)
	mov 		rBin2L,temp
	rcall 		Bin2ToDigit 		; Calculate digit
	ldi 		temp,HIGH(100) 		; Next with hundreds
	mov 		rBin2H,temp
	ldi 		temp,LOW(100)
	mov 		rBin2L,temp
	rcall 		Bin2ToDigit 		; Calculate digit
	ldi 		temp,HIGH(10) 		; Next with tens
	mov 		rBin2H,temp
	ldi 		temp,LOW(10)
	mov 		rBin2L,temp
	rcall 		Bin2ToDigit 		; Calculate digit
	st 		Z,ResultL 		; Remainder are ones
	sbiw 		ZL,4			; Put pointer to first BCD
	pop 		ResultL 		; Restore original binary
	pop 		ResultH
	ret 					; and return
;
;------------------------------------------------------------------------
; Bin2ToDigit
; ===========

Bin2ToDigit:
	clr 		temp 			; digit count is zero

Bin2ToDigita:
	cp 		ResultH,rBin2H 		; Compare 2H to 1H, bin to decimal
	brcs 		Bin2ToDigitc 		; bin MSB smaller than decimal branch
	brne 		Bin2ToDigitb 		; bin MSB bigger than decimal then branch
	cp 		ResultL,rBin2L 		; LSB bigger or equal decimal
	brcs 		Bin2ToDigitc 		; LSB smaller than decimal

Bin2ToDigitb:
	sub 		ResultL,rBin2L 		; Subtract LSB decimal
	sbc 		ResultH,rBin2H 		; Subtract MSB decimal
	inc 		temp 			; Increment digit count
	rjmp 		Bin2ToDigita 		; Next loop

Bin2ToDigitc:
	st 		Z+,temp 		; Save digit and increment
	ret 					; done

;******************************************************************************
;                       LCD Routines
;******************************************************************************
;
;	Control 2-line LCD,	Using 4-wire interface
;
;   Initialize LCD module

lcdinit:
   ldi 		TEMP,240        ; Wait about 20msec after powerup
   rcall 	WAIT_2M

   ldi 		TEMP,  0b00010010   ; Set 4 bit interface (but we are
   out 		PORTB, TEMP         ; still in 8 bits!)
   nop
   nop
   nop
   nop   	                    ; Data write cycle must be > 1 ms
   cbi 		PORTB,4        		; E low to clock in data
   ldi		temp,12
   rcall 	WAIT_2M

;      *** !! From now on, interface is 4 bits !! ***

   ldi 		PARAM, 0b00101000    ; Send again to catch the bit N
   rcall 	LCD_CMD           	; Display is 2 lines, so N = 1

   ldi 		PARAM, 0b00001000    ; Display off, cursor off, and blink off
   rcall 	LCD_CMD

   ldi 		PARAM, 0b00000001   ; Display clear
   rcall 	LCD_CMD
   ldi		temp,120			
   rcall 	WAIT_2M             ; Wait 2 msec after clear

   ldi 		PARAM, 0b00001100   ;Address increment, no scrolling
   rcall 	LCD_CMD
   ret

;********************************************************
;
;  Sends a control function to the display (comes in PARAM)
;
LCD_CMD:
   push		PARAM
   mov 		TEMP, PARAM
   andi		PARAM, 0xF0      ; Mask off lower 4 bits
   swap		param
   sbr 		PARAM, 0x10      ; E bit high (bit 4)
   out 		PORTB, PARAM     ; Send upper 4 bits to display
   nop                  	  ; Brief delay
   nop
   nop
   nop
   nop	               		
   cbi 		PORTB,4          ; E goes low to clock in data
   mov 		PARAM, TEMP      ; Data back
   andi		PARAM, 0x0F
   sbr 		PARAM, 0x10      ; E high (bit 4, port D)
   out 		PORTB, PARAM     ; Write lower 4 bits to LCD
   ldi 		TEMP,4			
   rcall 	WAIT_2M  	; Brief delay to give reasonable E
 		               	; pulse width
   cbi 		PORTB,4         ; E clock low
   ldi		temp,4
   rcall	WAIT_2M
   pop 		PARAM
   ret

;*******************************************************
;
; Sends an ASCII character to the display (comes in PARAM)
;
LCD_DATA:
   wdr				;reset watchdog
   push		PARAM
   push		TEMP
   mov 		TEMP, PARAM
   andi		PARAM, 0xF0      ; Mask off lower 4 bits
   swap		param
   sbr 		PARAM, E         ; E bit high
   sbr 		PARAM, RS        ; Data/command bit high
   out 		PORTB, PARAM     ; Write upper 4 bits to display
   nop                  	  ; Brief delay
   nop
   nop
   nop
   nop
   cbi 		PORTB, 4         ; E low to clock data
   andi		TEMP, 0x0F       ; Masked off
   sbr 		TEMP, E         ; E high
   sbr 		TEMP, RS
   out 		PORTB, TEMP      ; Lower 4 bits to LCD
   nop
   nop
   cbi 		PORTB, 4         ; E low to clock data
   ldi		temp,2     	  ; 50 usec
   rcall	WAIT_2M
   pop 		TEMP
   pop 		PARAM
   ret

; *********************************************
;      Int0 handler

Turning:
	push	TEMP
	RCALL	debounce
	sbic	pind,0
	rjmp	down
	sbic	pind,1
	rjmp	up
	rjmp	stop
	
Down:
	inc		stcmd
	ldi		temp,1
	add		unitl,temp
	brcc    	done
	adc		unith,unitl
	rjmp		done
	
Up:
	subi		unitl,low(counter)
	sbci		unith,high(counter)
		
done:
   IN			DIR,PIND
   andi		dir,3
   cpi		dir,3			;On Error skip count update
   breq		D1	
   cpi		dir,0			;update 'last direction'
   breq		D1  			;if stopped
   mov		ldir,dir		; Update 'Last Direction"
   rjmp		D1

Stop:                      		; continue counting based on last direction
	cpi		ldir,1
	breq		down
	cpi		ldir,2
	breq		up

D1:	
	pop		temp
	sei 					;Enable interrupts
	reti
	
; ****  Switch Debounce
debounce:
	SBIS		PIND,2
	RJMP		DEBOUNCE
	RET					;Done, return

;
;***************************************************************************
;*
;* EEWrite
;*
;***************************************************************************

Write:
	cli					;suspend global interrupts
	ldi		temp,$10
	rcall		read
	cp		temp,UnitL
	breq		nochange		;skip in no change in count
	ldi		EEawr,$10		;set location
	mov		EEdwr,unitL
	
	rcall		EEWrite			;store data in EEPROM location $0010
	ldi		EEawr,$11
	mov		EEDwr,unitH
	rcall		EEWrite
	
nochange:
	sei					;enable global interrupts
	ret

EEWrite:
	sbic		EECR,EEWE		;if EEWE not clear
	rjmp		EEWrite			;wait more
	
	out		EEARL,EEawr		;output address low byte
	out		EEDR,EEdwr		;output data
	sbi 		EECR,EEMWE		;set master write enable, remove if 90S1200 is used	
	sbi		EECR,EEWE		;set EEPROM Write strobe
						;This instruction takes 4 clock cycles since
						;it halts the CPU for two clock cycles
	ret

;***************************************************************************
;*
;* EERead
;*
;***************************************************************************

Read:
	mov		EEard,temp		;set location
	rcall		EERead			;read address $10
	ret

EERead:
	sbic		EECR,EEWE		;if EEWE not clear
	rjmp		EERead			;wait more
	out		EEARL,EEard		;output address low byte
	sbi		EECR,EERE		;set EEPROM Read strobe
						;This instruction takes 4 clock cycles since
						;it halts the CPU for two clock cycles
	in		temp,EEDR		;get data
	ret
	
;***** Copy 16 bytes ROM -> RAM
; Initialize chain count to last value stored in ROM
Blockcopy:
	ldi		ZH,high(linetwo*2)
	ldi		ZL,low(linetwo*2)	;init Z-pointer
	ldi		YH,high(00)
	ldi		YL,low(0X70)		;init Y-pointer
	ldi		temp,18
	rcall		flash2ram		;copy 20 bytes
	ret
    			
flash2ram:
	lpm					;get constant pointed to by Z and load into R0
	st		Y+,r0			;store in SRAM and increment Y-pointer
	adiw		ZL,1			;increment Z-pointer
	dec		temp
	brne		flash2ram		;if not end of table, loop more
	ret
;******************************************
;Divide by 3 Routines  16 bit no by 8 bit no

; Load the numbers to the appropriate registers
;
Divide3:	
	mov 		DivendL,UnitL
	mov 		DivendH,UnitH
	ldi 		temp,0x03 		; 0x55 to be divided with
	mov 		divisor,temp
;
; Divide UnitH:UnitL by divisor
;
div8:
	clr 		rd1u 			; clear interim register
	clr 		ResultH			; clear result (the result registers
	clr 		ResultL			 ; are also used to count to 16 for the
	inc 		ResultL			 ; division steps, is set to 1 at start)
;
; Division loop
;
div8a:
	clc 					; clear carry-bit
	rol 		divendL			; rotate the next-upper bit of the number
	rol 		divendH			; to the interim register (multiply by 2)
	rol 		rd1u
	brcs		div8b			; a one has rolled left, so subtract
	cp 		rd1u,divisor		; Division result 1 or 0?
	brcs		div8c 			; jump over subtraction, if smaller
	
div8b:
	sub 		rd1u,divisor		; subtract number to divide with
	sec 					; set carry-bit, result is a 1
	rjmp		div8d 			; jump to shift of the result bit
	
div8c:
	clc 					; clear carry-bit, resulting bit is a 0
	
div8d:
	rol 		ResultL 		; rotate carry-bit into result registers
	rol 		ResultH
	brcc		div8a 			; as long as zero rotate out of the result
					 	; registers: go on with the division loop
	ret					; End of the division reached
	

;******************************************
; Message Data

.org 0x0300
;DATA MEM Starting at 0076

LineTwo:	.db	">> 00000 ft   ",0	;current chain count location.
;						initialized on start up from ROM	
Raising:	.db	" Raising ",0
Lowering:	.db	" Lowering",0
Stopped:	.db	" Stopped ",0
MERROR:		.db	" *ERROR* ",0
Intro1:		.db	"Winch Controller",0
Intro2:		.db	"Version 4.13",0

;******************************************************************************************

.eseg
.org $10
EEConst:		.db 0,0,0,0,0,0  				;establishes ROM area on chip burn
; ************************************* END ***********************************************
































































































