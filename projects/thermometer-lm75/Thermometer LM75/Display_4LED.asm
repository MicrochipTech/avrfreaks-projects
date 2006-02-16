;*** Service LED Display **************************************************
;
; Filename		:Display_4LED.asm
;
; Project		;Brick For Programs
; First Target	:Roulette With Fiskal Counter (Roulette_F.asm)
; Device		:AT90S2313
; Created		:2003.03.30
; Last Updated	:
; Version		:1.0
;
; DESCRIPTION	:Service of the LED (8-digits) display
;				with BCD-to-decimal decoder/driver 74xx145.
;**************************************************************************

.equ lenght_display = 4
.def position_displ	= r22
		.equ	DC1 = 0
		.equ	DC2	= 1

.cseg
TIM_OVF0:
;***** Multiplexing LED Display. Call From Interruption TIMER0_OVF ********

;------- SCAN DISPLAY ------
;		scan period 2 ms

	clr temp
	out B_port, temp			;Turn OFF segments
	inc position_displ
	cpi position_displ, lenght_display
	brlo INC_ANODE
;----- New cycle of the display -----
	clr position_displ

;----- The following position -----
INC_ANODE:
	sbrs position_displ, DC1
	cbi PORTD, BIN1pinD
	sbrc position_displ, DC1
	sbi PORTD, BIN1pinD
	sbrs position_displ, DC2
	cbi PORTD, BIN2pinD
	sbrc position_displ, DC2
	sbi PORTD, BIN2pinD

	clr ZH
	mov zl,position_displ
	ldi temp, 0x60
	add zl, temp
	ld r0,z
	rcall ATTACH_SYM
	out B_port,r0				;Turn ON segments
;	out B_port, i2cdata
	reti
;------- END SCAN DISPLAY -----
;***** End Route Of Multiplexing LED Display ******************************
