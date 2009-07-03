; An ATMega8-based dual digit hexadecimal display driver.
; Drives either a dual digit 14-segment LED display
; or a dual-digit 7-segment LED display.
;
; by Mark Graybill, 30Mar2006, 08Apr2009.


; Device Programming Overview
;
; Clock: Internal oscillator, 4MHz or 8MHz. See CKSEL3..0 fuse bits, below.
; Clock calibration byte: Not required, may be placed in FLASH at $13 if desired.
; RSTDISBL is unprogrammed. (Factory default.)
; BOD: Not used. (Factory defaults.)
; Watchdog: Not used. (Factory defaults.)

; Fuse Bit Programming
;
; High Byte:
; RSTDISBL	=1 DEFAULT (Unprogrammed, PC6 is /RESET)
; WDTON		=1 DEFAULT (Unprogrammed, WDT Enabled by WDTCR)
; SPIEN		=0 DEFAULT (Programmed, Serial Programming Enabled)
; CKOPT		=1 DEFAULT (Unprogrammed, using internal osc.)
; EESAVE	=1 DEFAULT (Unprogrammed, EEPROM cleared on Chip Erase.)
; BOOTSZ1	=0 DEFAULT (Programmed, do not care.)
; BOOTSZ0	=0 DEFAULT (Programmed, do not care.)
; BOOTRST	=1 DEFAULT (Unprogrammed, use normal reset vector.)
;
; Low Byte:
; BODLEVEL	=1 DEFAULT (Unprogrammed, BOD is not used.)
; BODEN		=1 DEFAULT (Unprogrammed, BOD disabled.)
; SUT1		=0 MODIFIED (Programmed for fast rising power.)
; SUT0		=1 MODIFIED ( SUT1..0 = 01 For fast rising power.)
; CKSEL3	=0 DEFAULT	(CKSEL3..0 = 0011 for 4MHz operation.)
; CKSEL2	=0 DEFAULT	(CKSEL3..0 = 0100 for 8MHz operation.)
; CKSEL1	=1 MODIFIED
; CKSEL0	=1 DEFAULT
;

; Basic operation:
;
; 8 bits of data are received on Port D from host.
; If Latch Enable (Port B pin 7) is low, the outputs
; on Port B and Port C will drive a dual digit display
; to provide a hexadecimal representation of the binary
; value read on Port D on the displays. When PB7 is high, 
; the data will be latched.
; The outputs provided in this program will drive common 
; cathode displays.

; At start-up, the display is blank and is held in a blank state
; until the LATCH line is pulled low, at which time the display
; will follow the value on the data lines until the LATCH goes
; high again.

;
; At 8MHz, sample rate is nominally 400,000 samples/second.
; So, allowing for variations in clock frequency
; of the internal oscillator:
; 200K<= samples/Second <=800K
; Tw = 500uS (Minimum pulse width on LATCH)
; Th = 250nS (Minimum data hold time after LATCH returns high.)

; At 4MHz the sample rate is nominally 200Ks/S, 
; 100K <= samples/Second <= 400K
; Tw = 1mS
; Th = 500nS
 
; This means that this circuit is relatively slow compared to,
; e.g. a TIL311 (~7.3Msamples/S. If more responsiveness is required,
; an octal latch (e.g. 74XX373) should be used on the data input lines
;  with the LATCH line of this circuit held low to allow the display
; to follow the value in the octal latch at all times.
; This will provide adequate response for any human-readable display
; while allowing for far shorter Tw/Th times.
;
; Inverting the outputs (either in software or in external
; hardware) will allow this circuit to drive common anode
; displays.
;
; Port B pin 6 is an input to select the use of either
; a dual 7-segment display or a dual 14-segment display.
; High = 7 segment, Low = 14 segment.
; This input is read once at the start of the program.
; To change the type of display to be driven, a /RESET
; or power cycle is required.
;
; Port B pins 1 through 5 drive display segments A through E.
; Port C pins 0 through 4 drive the following display segments:
; Pin	Segment
; 0		F
; 1		G1 (G on 7-segment displays, Middle left horizontal seg on 10/14/15 seg disp.)
; 2		G2 (Middle right horizontal segment on 10/14/15 segment displays.)
; 3		H	" (Top center vertical on 10/14/15 segment displays.)
; 4		J	" (Bottom center vertical on 10/14/15 segment displays.)
;
; Segments K,L,M, and N (diagonals) are unused on 14/15-segment displays.
;
; Port C pin 4 selects the high-order digit. (Left digit.)
; Port C pin 5 selects the low-order digit. (Right digit.)
;
; Port C pin 6 is reserved as an MCU reset to facilitate subsequent
; programming of the MCU. This line should be tied high in normal use,
; or it may be used for display blanking where MCU start-up times are
; not a problem for the application. To blank the display simply pull
; this line low.

; The display will remain blanked until the LATCH line is pulled low.

;
; Pinout as used:
;
;                1 -------U------- 28
;        /RESET---|PC6         PC5|---CC2
;                2|               |27
;            D0---|PD0         PC4|---CC1
;                3|               |26
;            D1---|PD1         PC3|---J
;                4|               |25
;            D2---|PD2         PC2|---H
;                5|               |24
;            D3---|PD3         PC1|---G2
;                6|               |23
;            D4---|PD4         PC0|---G1
;                7|               |22
;           Vcc---|Vcc         GND|---GND
;                8|               |21
;           GND---|GND        AREF|---N/C
;                9|               |20
;  7/14 Seg Sel---|PB6        AVcc|---Vcc
;               10|               |19
;  Latch Enable---|PB7         PB5|---F
;               11|               |18
;            D5---|PD5         PB4|---E
;               12|               |17
;            D6---|PD6         PB3|---D
;               13|               |16
;            D7---|PD7         PB2|---C
;               14|               |15
;             A---|PB0         PB1|---B
;                  ---------------
;
; Display Output
;
; Data Nibble:	Display	Segments Illuminated 7/14 segment:
;				A B C D E F G / A B C D E F G1 G2 H J
;		0		x x x x x x     x x x x x x
;		1		  x x             x x
;		2		x x   x x   x   x x   x x   x  x
;		3		x x x x     x   x x x x     x  x
;		4         x x     x x     x x     x x  x
;       5       x   x x   x x   x   x x   x x  x
;       6       x   x x x x x   x   x x x x x  x
;		7		x x x           x x x
;       8       x x x x x x x   x x x x x x x  x
;		9       x x x x   x x   x x x x   x x  x
;		A		x x x   x x x   x x x   x x x  x
;		B		    x x x x x   x x x x        x  x x
;		C		x     x x x     x     x x x
;		D		  x x x x   x   x x x x           x x
;		E		x     x x x x   x     x x x x  x
;		F       x       x x x   x       x x x  x
;

; Some Possible Improvements:
; Drive both H and J off the same line to free up an I/O line.
; Use one of the freed lines to provide an external select for CA or CC
; displays.
; Add the ability to scan a 4x4 keypad using the same lines as the segment
; drivers, and use a freed line as a keyboard data output enable with data
; placed on the same Port D lines as the display data.
;
; Add loops during which data is sampled, but the display is blanked
; to reduce power usage. See Tiny26HexDispDriverRP3.asm for an example.
;
; Use a line to acknowledge receipt of data for high speed applications, if
; simply changing the frequency to 8MHz still isn't fast enough. E.g. get rid
; of the check for display type, hardcode in which data set to use. Then add
; code to pulse PB6 once data has been read in after the latch line has been
; released. That way the host can present the data and hold the latch low long
; enough for the Mega8 to get around to reading it where the normal timing
; isn't fast enough.
;

; ////////////////////////////////
; ///         Includes         ///
; ////////////////////////////////

; Register names, etc for ATMega8
.include	"m8def.inc"

; Defines
.def 	blank = r15	// Used to blank outputs, set to $00 in program.


; \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
; \\\    Handler Jump Table    \\\
; \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
.cseg

.org	$000

	rjmp RESET		; RESET handler


; *******************************
; ******* Initialization ********
; *******************************
.org $013

OSCALBYTE:	.db	$FF, $FF	; Place OSCCAL Byte here during programming if desired.

.org $014			; leave room for the full set of ATMega8 interrupts
					; plus the calibration byte at address $013

RESET:
	cli						; turn off interrupts

; Set oscillator calibration byte (if used.)
Set_OSCCAL:
	ldi	ZH,high(OSCALBYTE<<1)		; set z address for OSCCAL byte
	ldi	ZL,low(OSCALBYTE<<1)		;  low byte (LSB=0)
	lpm				; OSCCAL value is now in R0
	out	OSCCAL,r0	; write it to the OSCCAL register.


	ldi	r16,high(RAMEND) 	;Initialize Stack Pointer 
	out	SPH,r16	         
	ldi	r16,low(RAMEND)	 
	out	SPL,r16

; Configure I/Os

; PortD, data bus, all inputs
	clr	r16
	out	PORTD,r16	; all 0's, no pullups.
	out	DDRD, r16	; all inputs.

; PortC, /RESET on PC6, all others outputs
	clr	r16
	out	PORTC, r16	; all 0's for now
	ldi	r16, (1<<PC5)|(1<<PC4)|(1<<PC3)|(1<<PC2)|(1<<PC1)|(1<<PC0)
	out	DDRC, r16	; Set PC0..5 as outputs.

; PortB: 0-5 are outputs, 6 and 7 are inputs
	out	DDRB, r16	; Set PB0..5 as outputs. PB6..7 as inputs.
	clr	R16			; Put 0's out on all outputs of PB.
	out	PORTB, r16

; Determine whether we're using 14 segment or 7 segment displays.
; If PB6 is high, we're using 7-segment, otherwise 14 segment.
	in r16, PINB
	andi	r16, $40
	breq	FourteenSeg	; If it's low, we're using 14seg characters.

SevenSeg:
	; Load the appropriate character definitions into SRAM for each nibble value.
	ldi	r17, $10			; Set counter to 16 for # of digits.
	clr	YH					; Set Y to point to bottom of SRAM.
	ldi	YL, $60
	ldi	ZH, high(Seven<<1)	; Set Z to point to first char byte
	ldi	ZL, low(Seven<<1)
SS:	lpm	r16, Z+				; Get char pattern low byte,
	st	Y+, r16				; and store it in RAM.
	lpm	r16, Z+				; Get pattern high byte,
	st	Y+, r16				; and store it as well.
	dec	r17
	brne	SS				; Keep going till done.
	rjmp	Clear			; Then go to Main.

FourteenSeg:
	; Load the appropriate character definitions into SRAM for each nibble value.
	ldi	r17, $10			; Set counter to 16 for # of digits.
	clr	YH					; Set Y to point to bottom of SRAM.
	ldi	YL, $60
	ldi	ZH, high(Fourteen<<1)	; Set Z to point to first char byte
	ldi	ZL, low(Fourteen<<1)
FS:	lpm	r16, Z+				; Get char pattern low byte,
	st	Y+, r16				; and store it in RAM.
	lpm	r16, Z+				; Get pattern high byte,
	st	Y+, r16				; and store it as well.
	dec	r17
	brne	FS				; Keep going till done.

Clear:
; Clear the digit data register
	clr	r16
; Set the "Blank" register to $00
	mov blank, r16
; Output blank characters until we get data
	out PORTB, blank
	out PORTC, blank
WaitData:
	in	r17, PINB	; Get the latch
	in	r22, PIND	; Get the data
	rol	r17			; See if the latch has been dropped.
	brcs WaitData	; If not, keep waiting.
	mov	r16, r22	; If so, put the received data into our digit data register.

; **********************************
; ***     End Initialization     ***
; **********************************
;

; [[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
; [[[            Main            [[[
; [[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
;

Main:
; Check the status of the latch bit, and get the data value.
; If the latch is low, replace our digit value with the new data value.
	in	r17, PINB	; Get the latch
	in	r22, PIND	; Get the data bus value.
	rol	r17
	brcs	ShowHigh	; PB7 is high, ignore new data from PIND

	; Otherwise use the new value off the data bus.
	mov	r16, r22

ShowHigh:
	; Display the values on the display.
	; High digit, then low digit.
	mov	r18, r16
	andi r18, $F0	; Get the high nibble
	swap r18		; Put it on the low end.
	add	r18, r18	; Double it to get offset into SRAM(for two bytes per char.)
	ldi	YH, $0		; Set the SRAM base high address
	ldi	YL, $60		; Get the SRAM base low address
	add	YL, r18		; Add the char address offset to Y.
	ld	r20,Y+		; Get the character patterns.
	ld	r19,Y
	ori r20,$10		; Set the character select bit for CC1
	out	PORTB, r19	; display the character
	out	PORTC, r20
	nop				; Kill a cycle
	

; Check the status of the latch bit, and get the data value if appropriate.
	in	r17, PINB
	in	r22, PIND
	rol	r17
	brcs ShowLow	; PB7 is high, ignore new data from PIND

	; Otherwise use the new value off the data bus.
	mov	r16, r22

ShowLow:
	; Now the show low digit.
	mov	r18, r16	; Put the data value in a place we can fiddle with it.
	andi r18, $0F	; Get the low nibble
	add	r18, r18	; Double it to get offset into SRAM(for two bytes per char.)
	ldi	YH, $0		; Set the SRAM base high address
	ldi	YL, $60		; Get the SRAM base low address
	add	YL, r18		; Add the char address offset to Y.
	ld	r20,Y+		; Get the character patterns.
	ld	r19,Y
	ori r20,$20		; Set the character select bit for CC2
	out	PORTB, r19	; display the character
	out	PORTC, r20

rjmp	Main		; Do it all over again, ad infinitum.

; ]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
; ]]]         End of Main        ]]]
; ]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
;

; Flash Data
;
; Seven Segment Definitions, bit order by segment is:
;             00FEDCBA000000XG -- zeroes are left zero. X is a ghost "G2"
;								for use with 10-seg disp treated as 7 seg.
Seven:	.dw	0b0011111100000000	; zero
		.dw	0b0000011000000000	; one
		.dw	0b0001101100000011	; two
		.dw	0b0000111100000011	; three
		.dw	0b0010011000000011	; four
		.dw	0b0010110100000011	; five
		.dw	0b0011110100000011	; six
		.dw	0b0000011100000000	; seven
		.dw	0b0011111100000011	; eight
		.dw	0b0010111100000011	; nine
		.dw	0b0011011100000011	; A
		.dw	0b0011110000000011	; b
		.dw	0b0011100100000000	; C
		.dw	0b0001111000000011	; d
		.dw	0b0011100100000011	; E
		.dw	0b0011000100000011	; F

Fourteen:
; Fourteen Segment Definitions, bit order by segment is:
;             00FEDCBA0000JHG2G1 -- zeroes are left zero.
		.dw	0b0011111100000000	; zero
		.dw	0b0000011000000000	; one
		.dw	0b0001101100000011	; two
		.dw	0b0000111100000011	; three
		.dw	0b0010011000000011	; four
		.dw	0b0010110100000011	; five
		.dw	0b0011110100000011	; six
		.dw	0b0000011100000000	; seven
		.dw	0b0011111100000011	; eight
		.dw	0b0010111100000011	; nine
		.dw	0b0011011100000011	; A
		.dw	0b0000111100001110	; B
		.dw	0b0011100100000000	; C
		.dw	0b0000111100001100	; D
		.dw	0b0011100100000011	; E
		.dw	0b0011000100000011	; F
