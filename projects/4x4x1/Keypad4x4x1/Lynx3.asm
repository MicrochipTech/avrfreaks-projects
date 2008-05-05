; One-Wire 4x4 keypad project
	; With output lookup table

; By Ian Stedman
; (c) 2008

; Insert disclaimer and relevant software license here

; Rev. 0.1
	; Various test routines
		; RS-232 to PC works
		; Keypad works

.include "m168def.inc"			; Defines all the chip parameters from Atmel
								; For an ATmega168 chip
.include "reguse.asm"			; Defines how I use the registers

.cseg							; It's code
.org 0x0000						; Start at the beginning, and:
	rjmp	RESET				; Let's get started!

.org 0x0034						; To leave all the interrupt space available
								; we jump to 0x0034
RESET:
	cli							; Disable global interrupts

    ldi     temp,low(RAMEND)	; Init Stack Pointer Low
    out     SPL,temp
    ldi     temp,high(RAMEND)	; Init Stack Pointer High
	out     SPH,temp			; Only for chips with more than 256 bytes SRAM
								; otherwise your subroutine calls won't work
; Ports
; Inputs
; Make all ports inputs
	ldi temp, $00
	out DDRB, temp
	out DDRC, temp
	out DDRD, temp
; w/ pullups 
	ldi temp, $FF
	out PORTB, temp
	out PORTC, temp
	out PORTD, temp

; Make a few ports outputs
	sbi DDRC, 5

; Power Reduction? (Everything turned on, so No)
	ldi ZL, low(PRR)
	ldi ZH, high(PRR)
	clr temp
	st Z, temp

; Serial Port setup
	ldi ZL, $C0					; That's the base address of the serial port
	ldi temp, $00				; Clear all 
	st Z+, temp					; UCSR0A
	ldi temp, $18				; Turn on Tx and Rx w/o interrupts
	st Z+, temp					; UCSR0B
	ldi temp, $06				; Set to 8-N-1
	st Z+, temp					; UCSR0C
	inc ZL						; Skip 0xC3

; Set Baud Rate
	ldi temp, $38				; 9600 baud @ ~8.6MHz (int. osc.)

				; The internal oscillators vary a great deal in
				; how fast they are.  They have to be measured
				; using the CLKO pin, then this value needs adjusting
				; according to the formula for baud rate in the spec
				; sheets (the complete, not the summary).  Otherwise
				; you'll get serial port garbage.

	st Z+, temp					; UBRR0L
	clr temp
	st Z+, temp					; UBRR0H
; End Serial Port setup

; ADC setup
INIT_ADC:
	clr ZH
	ldi ZL, low(ADCSRA)
	ldi temp, $85				; ENA, ADIF, and /128 clock scale
	st Z, temp

	ldi ZL, low(ADCSRB)
	ldi temp, $00				; Write all zeros
	st Z, temp					; We want nothing here

	ldi ZL, low(ADMUX)
	ldi temp, $20				; Use AREF, MUX 000, ADLAR (for left adjust)
	st Z, temp					; And nothing else

	ldi ZL, low(DIDR0)
	ldi temp, $01				; Kill digital inputs on PORTC 0
	st Z, temp					; Just to save power (and heat waste).
; End ADC setup

	ldi TxByte, $0D				; Send a carriage retun to the terminal,
	call PUT_CHAR

	ldi ZL, low(2*WELC_MSG)
	ldi ZH, high(2*WELC_MSG)
	rcall SER_MSGSEND			; and if everything so far is working, it'll
								; say 'Welcome' on the RS-232 port!

; End RESET

MAIN:

	rjmp LOOP_TEST				; Uncomment this line to build the lookup table
									; Comment out the above line to use the lookup table

	call KEYP					; Then we just loop putting out the keypad results
								; This will probably get pretty dull pretty soon...
								; So build something interesting with it!

rjmp MAIN						; Loop until exasperated enough to pull the plug


LOOP_TEST:
	call KEYP_AVG		; Fetch the value
	call KEYP_RAW		; Puts the raw ADC value out RS-232

rjmp LOOP_TEST			; Loop until exasperated enough to pull the plug


; And included files full of subroutines:

.include "serial.asm"			; Serial port routines (dull)

.include "keypad.asm"			; Keypad routines (the fun part!)
									; Check out the above file.  That's where the fun is.

.include "hexout.asm"			; Make an 8-bit value print to RS-232
									;in two-character hex (dull)

.include "serialnotes.asm"		; What it'll tell you (readily editable!  Make it say rude
									; things!  But not to me, zanks).



