
; Hexadecimal 7-segment display decoder/driver/latch for ATTiny26
; by Mark Graybill
; 20 Dec 2006
; Tested and functioning, 29 Dec 2006 mag

; Accepts a 4-bit value on PB0-3 when LATCH (PB4) is low,
; decodes and displays on a 7-segment LED. Drives all segments
; plus the decimal point, which is used to signify an alpha
; digit (A-F) if desired.

; Display will drive either a common cathode or common anode display
; depending on the state of CC/CA (PB5) at startup. If CC/CA is high,
; outputs to LEDs will be active high, if CC/CA is low, outputs to LEDs
; will be active low. Once set, CC/CA is no longer sampled, so it cannot
; be changed without a /RESET or power cycle.

; Pinout:
;
;	 			    ___ ___
;	    D0 (PB0) --|   U   |-- f (PA0)
;	    D1 (PB1) --|       |-- g (PA1)
;	    D2 (PB2) --|       |-- a (PA2)
;	    D3 (PB3) --|       |-- b (PA3)
;	         Vcc --|       |-- GND
;	         GND --|       |-- AVcc
;	 LATCH (PB4) --|       |-- c (PA4)
;	 CC/CA (PB5) --|       |-- d (PA5)
;	Unused (PB6) --|       |-- e (PA6)
;	/RESET (PB7) --|_______|-- d.p. (PA7)
;
;

; Patterns:
;
;	Display	Segments
;	Digit	a b c d e f g d.p.
;	0		X X X X X X - -
;	1		X X - - - - - -
;	2		X X - X - X X -
;	3		X X X X - - X -
;	4		- X X - - X X -
;	5		X - X X - X X -
;	6		X - X X X X X -
;	7		X X X - - - - -
;	8		X X X X X X X -
;	9		X X X - - X X -
;	A		X X X - X X X X
;	b		- - X X X X X X
;	C		X - - X X X - X
;	d		- X X X X - X X
;	E		X - - X X X X X
;	F		X - - - X X X X
;

; Basic Operation:
;
;	Upon initialization, the program stores either the CC or CA
; 	offset into the index register.
;
;	It then enters a loop, where it tests LATCH. If LATCH is high, it loops.
; If LATCH goes low, it decodes the data nibble and displays the digit, then 
; returns to the LATCH test loop.
;

; Programming:
;
; This program assumes use of the internal oscillator as system clock. If a 
; clock calibration byte is to be used, it should be programmed into the 
; system FLASH at the last word ($03FF.) 
;
; Nominal configuration is to set the internal oscillator for 8MHz to
; provide the best response time, lower speeds can be programmed where
; acceptable to save power. Brown out detection is normally disabled to
; reduce power utilization as well, it can be enabled in applications
; it would be useful.
; For a faster clock, the PLL clock can be used to obtain 16MHz. In this
; case it is essential that the clock calibration byte be used with the
; appropriate value.
;
; The binary program file should first be programmed into FLASH, then the
; clock calibration byte (if used) should be programmed into FLASH at $03FF. Then
; all fuses should be set properly, with the clock fuses set for the internal
; oscillator at the selected speed (nominally 8MHz.)
;
; Fuse Bits:
;	RSTDISBL is unprogrammed
;	SPIEN		don't care
;	EESAVE		don't care
;	BODLEVEL	don't care
;	BODEN		1 (Brown Out Detector disabled.)
;
;	PLLCLK		1 (Do not use PLL for clock)
;	CKOPT		1 (not programmed)
;	SUT1		0 (CK+4ms rise time.)
;	SUT0		1
;	CKSEL3		0 (8MHz clock)
;	CKSEL2		1
;	CKSEL1		0
;	CKSEL0		0
;

.include	"tn26def.inc"

; equates--LED patterns defined here
; Patterns:
;
;	Display	Segments in bit order PA7->PA0
;	Digit	dp e d c b a g f $common cathode value
;	0		 - X X X X X - X $7D
;	1		 - - - X X - - - $18
;	2		 - X X - X X X - $6E
;	3		 - - X X X X X - $3E
;	4		 - - - X X - X X $1B
;	5		 - - X X - X X X $37
;	6		 - X X X - X X X $77
;	7		 - - - X X X - - $1C
;	8		 - X X X X X X X $7F
;	9		 - - - X X X X X $1F
;	A		 X X - X X X X X $DF
;	b		 X X X X - - X X $F3
;	C		 X X X - - X - X $E5
;	d		 X X X X X - X - $FA
;	E		 X X X - - X X X $E7
;	F		 X X - - - X X X $C7

.equ seg0 = $7D
.equ seg1 = $18
.equ seg2 = $6E
.equ seg3 = $3E
.equ seg4 = $1B
.equ seg5 = $37
.equ seg6 = $77
.equ seg7 = $1C
.equ seg8 = $7F
.equ seg9 = $1F
.equ sega = $DF
.equ segb = $F3
.equ segc = $E5
.equ segd = $FA
.equ sege = $E7
.equ segf = $C7

; disable interrupts
cli

; set oscillator calibration byte
ldi	ZH, high(RAMEND<<1)	; set z address for OSCCAL byte
ldi	ZL, low(RAMEND<<1)	; =flash word $1FF, low byte (LSB=0)
lpm						; OSCCAL value is now in R0
out	OSCCAL, R0			; write it to the OSCCAL register.

; set constants in registers
clr XH					; clear high byte of X for index into patterns
						; in R0-R15 (data space addr $00-$0F.)

; set up data direction registers
clr r16					; $00 for all inputs on PORTB except PB7:/RESET
out DDRB, R16			; which is disabled as a digital input by RSTDISBL fuse 
						; being unprogrammed (at value '1'.)
ser R16					; $FF for all outputs on PORTA
out DDRA, R16

; write appropriate display patterns to R0-R15

ldi r16, seg0
mov	r0, r16
ldi	r16, seg1
mov	r1, r16
ldi	r16, seg2
mov	r2, r16
ldi	r16, seg3
mov r3, r16
ldi r16, seg4
mov r4, r16
ldi r16, seg5
mov r5, r16
ldi r16, seg6
mov r6, r16
ldi r16, seg7
mov r7, r16
ldi r16, seg8
mov r8, r16
ldi r16, seg9
mov r9, r16
ldi r16, sega
mov	r10, r16
ldi r16, segb
mov	r11, r16
ldi r16, segc
mov r12, r16
ldi r16, segd
mov r13, r16
ldi	r16, sege
mov r14, r16
ldi r16, segf
mov r15, r16

; if CC/CA is low, outputs should be active low.
;
	in		R16, PINB			; read the input port
	andi	R16, $20			; see if CC/CA is set
	brne	latch				; if so, skip ahead to main prog.
	com		r0					; otherwise, complement all the bits
	com 	r1
	com 	r2
	com 	r3
	com		r4
	com 	r5
	com 	r6
	com 	r7
	com		r8
	com 	r9
	com 	r10
	com 	r11
	com		r12
	com 	r13
	com 	r14
	com 	r15


; main program
latch:
	in 		R16, PINB			; read the input port
	mov 	XL, R16				; preserve value for later data lookup
	andi 	R16, $10			; see if LATCH bit low
	brne 	latch				; if not, keep watching
								; otherwise, display the received value
	andi 	XL, $0F				; mask off the top nibble
	ld		R19, X				; get the LED pattern into R19
	out		PORTA, R19			; and send it out
	rjmp	latch				; go back for more


.org $03FF
.dw $FF			; You can put your OSCCAL value in here for your part, or use
				; your programmer to put it in this location after FLASH programming.
				; If you put a hard-coded value in, remember to change it for other
				; parts, or put this back to $FF so that a value can be programmed in
				; after being read from the part.
