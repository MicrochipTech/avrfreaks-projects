;***************************************************************
; Simple Traffic Light Controller
; Written by:  Dax Ryn
; 
; This project is intended as a very simple traffic light 
; controller.  It can be used to switch 3 LED's in sequence
; for the purpose of a miniture train set or etc.  
;
; PORTB is used for the outputs. PB0 is used for Red.  PB1 is 
; used for Amber/Yellow.  And, PB2 is used for Green.
;
; The timing delays are accomplished using Delay and Delay2 
; variables in a 33ms delay loop.  DelayNum is the variable which
; contains the number 33ms delays.  Input clock is assumed to be 
; 3.69MHz (maximum of STK500 software generated clock).


.include "8515def.inc"


.def	Temp	=r16	; Temporary register
.def	Delay	=r17	; Delay variable 1
.def	Delay2	=r18	; Delay variable 2
.def	DelayNum =r19

;***** Vector Set
.org	0x0000
	rjmp	RESET		; Jump to RESET if reset occurs


;***** Initialization

.org	0x0010			; Set RESET just above Vectors

RESET:

	ser	Temp
	out	DDRB,Temp	; Set PORTB to output
	out	PORTB,Temp	; Set all PORTB pins to high (off)

	ldi	Temp,RAMEND>>8	; Load Upper Stack Pointer addr
	out	SPH,Temp	; Set Upper Stack Pointer addr
	ldi	Temp,RAMEND&$00FF	; Load Lower Stack Pointer addr
	out	SPL,Temp	; Set Lower Stack Pointer addr

	rjmp	MAIN


;***** Main Program Loop
MAIN:
	ser	Temp		; Set Temp to $FF

LOOP:
	cbr	Temp,$01	; Clear Bit0 for Red ON
	out	PORTB,Temp	; Set LEDS
	ldi	DelayNum,$78	; Delay Loop of ~4 sec (33ms*120)
	rcall	DELAY_T
	sbr	Temp,$01	; Set Bit0 for Red OFF

	cbr	Temp,$02	; Clear Bit1 for Amber/Yellow ON
	out	PORTB,Temp	; Set LEDS
	ldi	DelayNum,$1E	; Delay Loop of ~1 sec (33ms*30)
	rcall	DELAY_T
	sbr	Temp,$02	; Set Bit1 for Amber/Yellow OFF

	cbr	Temp,$04	; Clear Bit3 for Green ON
	out	PORTB,Temp	; Set LEDS
	ldi	DelayNum,$98	; Delay Loop of ~5 sec (33ms*152)
	rcall	DELAY_T
	sbr	Temp,$04	; Set Bit3 for Green OFF

	rjmp	LOOP		; Repeat loop forever



;***** Delay Time Loop
; This loop uses the DelayNum variable as the number of
; times to call the 33ms delay loop.
;
; The Total Time delayed is calculated as follows:
;	Total Delay Time = DelayNum * 33ms

DELAY_T:
	rcall	DELAY_33ms	;
	dec	DelayNum	;
	brne	DELAY_T	;
	ret			



;***** 33ms Delay Loop
; Delay time calculation is as follows:
; 
; Delay Loop Cycles = 255*3 + 2 = 767 cycles
; Delay2 Loop Cycles+C+R = (DelayCycles+3)*Delay2 + 8 
;	= 122438 cycles = TotalDelaycycles
; Time = (1/CLK MHz) * TotalDelayCycles
; = (1/3.686MHz) * 122438 cycles = 33127 us = 33.127 ms


DELAY_33ms:
	ldi	Delay,$00	;<1> Loads the value 0 to Delay
	ldi	Delay2,$EE	;<1> Loads the value 238 to Delay2
DELAY_L:
	dec	Delay		;<1> Count down 256 times, if $00
	brne	DELAY_L	;<T=2,F=1>
	dec	Delay2		;<1> Count down Delay2 times
	brne	DELAY_L	;<T=2,F=1>
	ret			;<4> (rcall <3>)

