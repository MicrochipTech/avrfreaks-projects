;************************************************************************
;* Psuedo-Random Number Generator tt800
;*
;* tt800 written by Makoto Matsumoto July 8 1996 version
;* Ported to AVR assembler Russell Keegstra June 20 2001
;*
;* tt800 uses 800 bits of twisted Feedback Shift Register (tFSR) and
;* 3 32 bit magic numbers.  It has a good distribution and a very long
;* period ((2 ^ 800) - 1, or better than 6.6 * 10^240).  And it's fast!
;* It's nicely implementable in a small CPU, too.  The Mersenne Twister
;* would require 2496 bytes of storage to implement...
;*
;* The 100 data bytes are loaded into SRAM starting at $0060 by the
;* RESET code.  The magic numbers are hard coded into the routine for
;* efficiency.
;*
;* Technically you can seed tt800 with anything you want (as long as
;* it's not all zeros), the seed data that I use here is from
;* Matsumoto's original C code.
;*
;* rand1-4 are the returned random number (32 bit) from the routine.
;* They can be in the low 16 registers because we don't need to do any
;* immediates on them.
;*
;* scr1-4 are scratch values, but they need to be in the high 16 because
;* immediate operations are done on them.  They should probably be pushed
;* and popped in the routine so they can be used for other purposes
;* elsewhere.
;*
;* YL and YH must be preserved, they are the current index into our
;* random array.
;************************************************************************
.include "8535def.inc"

; Register names

.def	lpmdest = r0	; Destination for the lpm instruction

.def	rand1 = r1	; Byte 1 of our 4 byte return random value
.def	rand2 = r2	; Byte 2
.def	rand3 = r3	; Byte 3
.def	rand4 = r4	; Byte 4

.def	temp = r16	; Generic variable

.def	scr1 = r17	; Scratchpad byte 1
.def	scr2 = r18	; Scratchpad byte 2
.def	scr3 = r19	; Scratchpad byte 3
.def	scr4 = r20	; Scratchpad byte 4

;************************************************************************
; 4434/8535 has a multitude of interrupt/reset vectors:

	rjmp	RESET	; 0  Reset
	rjmp	IGNORE	; 1  External interrupt 0 (PIND2)
	rjmp	IGNORE	; 2  External interrupt 1 (PIND3)
	rjmp	IGNORE	; 3  Timer2 compare match
	rjmp	IGNORE	; 4  Timer2 overflow
	rjmp	IGNORE	; 5  Timer1 input capture
	rjmp	IGNORE	; 6  Timer1 output compare A
	rjmp	IGNORE	; 7  Timer1 output compare B
	rjmp	IGNORE	; 8  Timer1 Overflow
	rjmp	IGNORE	; 9  Timer0 Overflow
	rjmp	IGNORE	; A  SPI
	rjmp	IGNORE	; B  UART receive complete
	rjmp	IGNORE	; C  UART data register empty
	rjmp	IGNORE	; D  UART transmit complete
	rjmp	IGNORE	; E  ADC conversion complete
	rjmp	IGNORE	; F  EEPROM write complete
	rjmp	IGNORE	; 10 Analog comparator

;************************************************************************
; Reset initialization code

RESET:
; Initialize stack pointer to the top of static RAM
	ldi	temp, high(RAMEND)
	out	SPH, temp
	ldi	temp, low(RAMEND)
	out	SPL, temp

; Copy our 100 seed bytes into SRAM.  Z points to flash code,
; Y points to SRAM location.
	ldi	ZH, high(2 * SEED0)	; High Z pointer
	ldi	ZL, low(2 * SEED0)	; and low Z pointer
	ldi	YH, 0		; High Y pointer
	ldi	YL, $60		; Low Y pointer, SRAM base
	ldi	temp, 100	; Loop counter
RESET1:	lpm			; Get byte
	st	Y+, lpmdest	; Store byte in SRAM, inc Y
	adiw	ZL, 1		; Increment Z pointer
	dec	temp		; Decrement loop counter
	brne	RESET1		; and continue

	ldi	YL, $60		; Initialize our array pointer

;************************************************************************
; Main code

MAIN:
	rcall	RANDOM

	rjmp	MAIN

;************************************************************************
;* Subroutines
;************************************************************************
; RANDOM -- Return a random number (which is 4 bytes long) in rand1-4
; Note the original C code preceding each section, x is the array of 25
; 32 bit values, k is the index into that array, y is the returned value.
; Yeah yeah yeah, I treat the 32 bit values as big-endian.  Sue me.
;
; 34 cycles to calculate our return value
; 47 or 54 cycles to calculate new data value
; Total time including call and return: 88/95 cycles

RANDOM:
; y = x[k]
	ld	rand1, Y+	; Read the current 4 byte value
	ld	rand2, Y+
	ld	rand3, Y+
	ld	rand4, Y+
; y ^= (y << 7) & 0x2b5b2500  /* Magic B */
	mov	scr1, rand2	; Make a copy, shift left 7 bits
	mov	scr2, rand3	; We will just load it 8 bits to the
	mov	scr3, rand4	; left and then shift right one bit
	mov	scr4, rand1
	lsr	scr4		; Get bottom bit of first byte
	ror	scr1		; Rotate carry into top
	ror	scr2		; and continue shifting right one bit
	ror	scr3		; scr4 is irrelevant because we will
	andi	scr1, $2B	; AND our shifted copy with Magic B
	andi	scr2, $5B	; and Magic B ends in 0x00
	andi	scr3, $25
	eor	rand1, scr1	; And now ex-or the result back in
	eor	rand2, scr2
	eor	rand3, scr3	; rand4 will be unchanged, scr4 is 00
; y ^= (y << 15) & 0xdb8b0000  /* Magic C */
	mov	scr1, rand3	; Make a copy, shift left 15 bits
	mov	scr2, rand4	; We will just load it 16 bits to the
	mov	scr4, rand2
	lsr	scr4		; Get bottom bit of second byte
	ror	scr1		; Rotate carry into top byte
	ror	scr2		; and continue shifting right one bit
	andi	scr1, $DB	; AND shifted copy with Magic C
	andi	scr2, $8B	; scr3-4 are now irrelevant
	eor	rand1, scr1	; And now ex-or the result back in
	eor	rand2, scr2	; rand3-4 will be unchanged
; y ^= (y >> 16)
	eor	rand3, rand1	; We can bypass the copy and shift
	eor	rand4, rand2	; and do this in place

; Now we have our return value in rand1-4, and we need to calculate
; the replacement for our current data value x[k]

; x[k] = x[k+M] ^ (x[k] >> 1) ^ mag01[x[k] % 2]

; That's a lot of code.  x[k+M] wraps - it's x[k+(M-N)] when k+M > N.
; incidentally, N is 25 and M is 7 (in 32bit unsigned long integers).
; mag01 is a two entry structure, entry 0 is 0 and entry 1 is Magic A
; so if x[k] is even that last term is just 0 (and drops out).  If x[k]
; is odd then the last term is Magic A, or 0x8ebfd028.

	subi	YL, 4		; Reset our pointer back to x[k]
	ld	scr1, Y+	; Load x[k] into scratch area
	ld	scr2, Y+
	ld	scr3, Y+
	ld	scr4, Y+
	bst	scr4, 0		; Save bottom bit for later
	lsr	scr1		; Shift right one bit
	ror	scr2
	ror	scr3
	ror	scr4
	subi	YL, -24		; Because there is no ADDI
	cpi	YL, $C4		; Need we wrap?
	brlo	RANDOM_1
	subi	YL, 100		; Wrap pointer
RANDOM_1:
	ld	temp, Y+	; Get x[k+M] byte by byte
	eor	scr1, temp	; and ex-or it with our scratch value
	ld	temp, Y+
	eor	scr2, temp
	ld	temp, Y+
	eor	scr3, temp
	ld	temp, Y+
	eor	scr4, temp
	brtc	RANDOM_2	; Is x[k] even or odd?
	ldi	temp, $8E	; If it's odd, we ex-or with Magic A
	eor	scr1, temp	; (no eor immediate)
	ldi	temp, $BF	; ...byte by byte
	eor	scr2, temp
	ldi	temp, $D0
	eor	scr3, temp
	ldi	temp, $28
	eor	scr4, temp
RANDOM_2:
	subi	YL, 32		; Reset pointer back to original x[k]
	cpi	YL, $60		; Need we wrap?
	brsh	RANDOM_3
	subi	YL, -100	; Once again, no ADDI
RANDOM_3:
	st	Y+, scr1	; Store our value back in x[k]
	st	Y+, scr2
	st	Y+, scr3
	st	Y+, scr4
; k++ is handled by the auto-increment of Y above
; if (k == N) k = 0
	cpi	YL, $C4		; Is Y at the end of our data?
	brne	RANDOM_4
	ldi	YL, $60		; If so reset to beginning
RANDOM_4:
	ret

;************************************************************************
;* Interrupt routiness
;************************************************************************
; Generic interrupt routine which we should never hit

IGNORE:	reti				; interrupt routine
;************************************************************************
;************************************************************************
; Initial seed of our 100 data bytes

SEED0:	.db	$95, $f2, $4d, $ab
SEED1:	.db	$0b, $68, $52, $15
SEED2:	.db	$e7, $6c, $ca, $e7
SEED3:	.db	$af, $3e, $c2, $39
SEED4:	.db	$71, $5f, $ad, $23
SEED5:	.db	$24, $a5, $90, $ad
SEED6:	.db	$69, $e4, $b5, $ef
SEED7:	.db	$bf, $45, $61, $41
SEED8:	.db	$96, $bc, $1b, $7b
SEED9:	.db	$a7, $bd, $f8, $25
SEED10:	.db	$c1, $de, $75, $b7
SEED11:	.db	$88, $58, $a9, $c9
SEED12:	.db	$2d, $a8, $76, $93
SEED13:	.db	$b6, $57, $f9, $dd
SEED14:	.db	$ff, $dc, $8a, $9f
SEED15:	.db	$81, $21, $da, $71
SEED16:	.db	$8b, $82, $3e, $cb
SEED17:	.db	$88, $5d, $05, $f5
SEED18:	.db	$4e, $20, $cd, $47
SEED19:	.db	$5a, $9a, $d5, $d9
SEED20:	.db	$51, $2c, $0c, $03
SEED21:	.db	$ea, $85, $7c, $cd
SEED22:	.db	$4c, $c1, $d3, $0f
SEED23:	.db	$88, $91, $a8, $a1
SEED24:	.db	$a6, $b7, $aa, $db
