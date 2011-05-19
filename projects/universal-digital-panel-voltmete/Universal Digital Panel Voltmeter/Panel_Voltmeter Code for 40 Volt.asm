;***********************************************************************************
;*
;* Title		: Universal Panel Voltmeter
;* Version		: 2.0
;* Last updated		: 2010 05 08
;* Target		: ATMEGA48
;*
;* Hardware requirements: see schematic
;*
;* DESCRIPTION
;*
;* This is a Panel voltmeter that can be easily modified for different full scale
;* displays. One only needs to change 1 resistor and change two constants to change
;* the full scale of the voltmeter. Then reassemble using AVR Studio.
;* The present default is for 40 volts range.
;* The ATmega48 uses its internal 8 MHz RC oscillator since timing is not critical.
;*
;***********************************************************************************
;*            Fuse Settings
;*
;* Clear the clock/8 fuse (CLKDIV8). No other changes.
;***********************************************************************************

.include "m48def.inc"

.def		Zero		= r0		; just a register set to $00
.def		Display0	= r1		; 7 segment pattern for digits 0
.def		Display1	= r2		; 7 segment pattern for digits 1
.def		Display2	= r3		; 7 segment pattern for digits 2
.def		Display3	= r4		; 7 segment pattern for digits 3

.def		BCD0		= r13		; BCD value digits 1 and 0
.def		BCD1		= r14		; BCD value digits 3 and 2
.def		BCD2		= r15		; BCD value digit 4

.def		Data0		= r16		; ADC Data manipulation - LSB     multiplicand L
.def		Data1		= r17						; multiplicand H
.def		Data2		= r18						; multiplier L
.def		Data3		= r19		; ADC Data manipulation - MSB     multiplier H
.def		Result0		= r18		; result byte 0 (LSB)
.def		Result1		= r19		; result byte 1
.def		Result2		= r20		; result byte 2
.def		Result3		= r21		; result byte 3 (MSB)
.def		LoopCntr	= r22		; loop counter
.def		tmp		= r23		; general purpose
.def		wt0		= r24
.def		wt1		= r25

.equ		AdBCD0		= 13		; address of BCD0
.equ		AdBCD2		= 15		; address of BCD2

; Change these costants for different voltage ranges as described in spreadsheet
.equ		M2L		= $7D		; 125
.equ		M2H		= $00
.equ		D2L		= $20		; 32
.equ		D2H		= $00

;***********************************************************************************
.CSEG
.ORG		$00      	        	; Program begins
		rjmp	Start			; relative jump to Start

		reti				; IRQ0 Handler
		nop
		reti 				; IRQ1 Handler
		nop
		reti				; Timer2 Compare Handler
		nop
		reti				; Timer2 Overflow Handler
		nop
		reti				; Timer1 Capture Handler
		nop
		reti				; Timer1 CompareA Handler
		nop
		reti				; Timer1 CompareB Handler
		nop
		reti				; Timer1 Overflow Handler
		nop
		reti				; Timer0 Overflow Handler
		nop
		reti				; SPI Transfer Complete Handler
		nop
		reti				; USART RX Complete Handler
		nop
		reti				; UDR Empty Hanlder
		nop
		reti				; USART TX Complete Handler
		nop
		reti				; ADC Conversion Complete Handler
		nop
		reti				; EEPROM Ready Handlder
		nop
		reti				; Analog Comparator Handler
		nop
		reti				; Two-wire Serial Interface Handler
		nop
		reti				; Store Program Memory Ready Handler
		nop

;******************************************************************************
; Start of Program - Initialize Stack Pointer
;******************************************************************************

Start:		ldi	tmp,low(ramend)		; get top of RAM to initialize SP
		out	spl,tmp			; store low byte in stackpointer
		ldi	tmp,high(ramend)	; get top of RAM
		out	sph,tmp			; store high byte in stackpointer

;******************************************************************************
; Initialize Ports B and D to drive LEDs
;******************************************************************************
;*
;*		Port D0 LED Segment a
;*		Port D1 LED Segment b
;*		Port D2 LED Segment c
;*		Port D3 LED Segment d
;*		Port D4 LED Segment e
;*		Port D5 LED Segment f
;*		Port D6 LED Segment g
;*		Port D7 LED Segment dp
;*
;*		Port B1 LED Digit 0 - $02 Least Significant Digit
;*		Port B2 LED Digit 1 - $04
;*		Port B6 LED Digit 2 - $40
;*		Port B7 LED Digit 3 - $80 Most Significant Digit

	ser	tmp			; set tmp register to $FF
	out	DDRD,tmp		; portD DDR for output...Segment driver
	ldi	tmp,$C6			; RS is set to 0 (command)
	out	DDRB,tmp		; Digit Drivers output

	ldi	tmp,$3F			; initialize 7 segment displays to "0"
	mov	Display0,tmp
	mov	Display1,tmp
	mov	Display2,tmp
	mov	Display3,tmp

;******************************************************************************
; A/D Converter Initalization
;******************************************************************************

; PRR defaults to $00 so PRADC is already 0 (power to ADC is on)
;
; Using ADC0 = Pin 23 on 28 Pin DIP
; Using AREF = Pin 21, Reference voltage is 2.5V from TL431
;
; ADMUX defaults to $00 so no changes are necessary.
;	MUX0 = 0	selects ADC0
;	MUX1 = 0
;	MUX2 = 0
;	MUX3 = 0
;	ADLAR = 0	ADC O/P is right adjusted
;	REFS0 = 0	selects AREF input
;	REFS1 = 0

; ADCSRA defaults to $00
;	ADEN = 1  enable ADC (bit 7)
;	Clock prescaler is set to /16 - 125KHz..... so ADPS0 - ADPS2 = $5

	ldi	tmp,$84
	sts	ADCSRA,tmp
;
; ADCSRA defaults to $00...... no changes required for single conversions
;
;******************************************************************************
; A/D Converter Data Aquisition and Scaling
;******************************************************************************
convrt:	lds	tmp,ADCSRA	;start conversion
	ori	tmp,$40
	sts	ADCSRA,tmp	; start conversion by setting ADSC = 1
wait: 	lds	tmp,ADCSRA	; check for end of conversion... ADIF = 1 ?
	andi	tmp,$10		; ADIF is bit 4
	breq	wait		; no data yet

; Getting ready to multiply - See Atmel App Note AVR200 for details
	lds	Data0,ADCL	; mov ADC data to multiplicand
	lds	Data1,ADCH
	ldi	Data2,M2L	; load multiplier constant
	ldi	Data3,M2H

mult16:	clr	Result3		; clear 2 highest bytes of result
	clr	Result2
	ldi	LoopCntr,16	; init loop counter
	lsr	Data3		; multiplier H
	ror	Data2		; multiplier L

mulmor:	brcc	noad8		; if bit 0 of multiplier set
	add	Result2,Data0	; add multiplicand Low to byte 2 of res
	adc	Result3,Data1	; add multiplicand high to byte 3 of res
noad8:	ror	Result3		; shift right result byte 3
	ror	Result2		; rotate right result byte 2
	ror	Result1		; rotate result byte 1 and multiplier High (Data3)
	ror	Result0		; rotate result byte 0 and multiplier Low (Data2)
	dec	LoopCntr	; decrement loop counter
	brne	mulmor		; if not done, loop more

; Divide Result by D2H, D2L
	ldi	Data0,D2L	; loading D2 (divisor) into Data register
	ldi	Data1,D2H

divmore:lsr	Data1		; shift right to divide
	ror	Data0
	brcs	bin2BCD		; a "1" has appeared in carry..... done
	lsr	Result3
	ror	Result2
	ror	Result1
	ror	Result0
	rjmp	divmore

;***********************************************************************************
; convert Result1 and Result0 into packed BCD - See Atmel App Note AVR204 for details

bin2BCD:rcall	digit0		; digit0 .....refresh display first
	ldi	LoopCntr,16	; Init loop counter
	clr	BCD2		; clear result (3 bytes)
	clr	BCD1		
	clr	BCD0		
	clr	ZH		; clear ZH
bBCDx_1:lsl	Result0		; shift input value
	rol	Result1		; through all bytes
	rol	BCD0		;
	rol	BCD1
	rol	BCD2
	dec	LoopCntr	; decrement loop counter
	brne	bBCDx_2		; if counter not zero
	rjmp	Display		; Done

bBCDx_2:ldi	r30,AdBCD2+1	; Z points to result MSB + 1

bBCDx_3:ld	tmp,-Z		; get (Z) with pre-decrement
	subi	tmp,-$03	; add 0x03
	sbrc	tmp,3		; if bit 3 not clear
	st	Z,tmp		;	store back
	ld	tmp,Z		; get (Z)
	subi	tmp,-$30	; add 0x30
	sbrc	tmp,7		; if bit 7 not clear
	st	Z,tmp		;	store back
	cpi	ZL,AdBCD0	; done all three?
	brne	bBCDx_3		; loop again if not
	rjmp	bBCDx_1		

;***********************************************************************************
; The BCD digits are packed in BCD0 - BCD2 but BCD2 is not used....only need 4 digits
; The 7 segment patterns are put into Display0 - Display3
;
Display:rcall	digit0		; digit0 .....refresh display first

	clr	Zero
	ldi	ZL,LOW(SegTbl*2)	; low pointer to display segment table 
	ldi	ZH,HIGH(SegTbl*2)	; high pointer to segment table
	mov	tmp,BCD0	; load digit 0/1 and then mask off digit 1
	andi	tmp,$0F		; tmp now contains digit 0
	add	ZL,tmp		; add index to Z 
	adc	ZH,Zero
	lpm	Display0,Z	; load Display0 with pattern in table

	ldi	ZL,LOW(SegTbl*2)	; low pointer to display segment table 
	ldi	ZH,HIGH(SegTbl*2)	; high pointer to segment table
	mov	tmp,BCD0	; load digit 0/1 and then mask off digit 0
	andi	tmp,$F0		; tmp now contains digit 1
	lsr	tmp		; shift upper nibble down
	lsr	tmp
	lsr	tmp
	lsr	tmp
	add	ZL,tmp		; add index to Z
	adc	ZH,Zero
	lpm	Display1,Z	; load Display1 with pattern in table

	ldi	ZL,LOW(SegTbl*2)	; low pointer to display segment table 
	ldi	ZH,HIGH(SegTbl*2)	; high pointer to segment table
	mov	tmp,BCD1	; load digit 2/3 and then mask off digit 3
	andi	tmp,$0F		; tmp now contains digit 2
	add	ZL,tmp		; add index to Z
	adc	ZH,Zero
	lpm	tmp,Z		; load Data2 with pattern in table
	ori	tmp,$80		; turn on decimal point
	mov	Display2,tmp	; and save in Display2

	ldi	ZL,LOW(SegTbl*2)	; low pointer to display segment table 
	ldi	ZH,HIGH(SegTbl*2)	; high pointer to segment table
	mov	tmp,BCD1	; load digit 2/3 and then mask off digit 2
	andi	tmp,$F0		; tmp now contains digit 3
	lsr	tmp		; shift upper nibble down
	lsr	tmp
	lsr	tmp
	lsr	tmp
	add	ZL,tmp		; add index to Z
	adc	ZH,Zero
	lpm	Display3,Z	; load Display3 with pattern in table
	rcall	digit0		; display result
	rjmp	convrt		; read voltage again

;***********************************************************************************
; Outputing Data to Display
;***********************************************************************************
digit0:	ldi	tmp,$02
	out	PORTB,tmp	; enable digit0
	out	PORTD,Display0	; enable segments
	ldi	wt1,2		; # of 100 us loops ...
	rcall	waitloop

digit1:	ldi	tmp,$04
	out	PORTB,tmp	; enable digit1
	out	PORTD,Display1	; enable segments
	ldi	wt1,2		; # of 100 us loops ...
	rcall	waitloop

digit2:	ldi	tmp,$40
	out	PORTB,tmp	; enable digit2
	out	PORTD,Display2	; enable segments
	ldi	wt1,2		; # of 100 us loops ...
	rcall	waitloop

digit3:	ldi	tmp,$3F		; leading "0" ?
	cp	display3,tmp
	breq	done		; yes... so blank Digit 3
	ldi	tmp,$80
	out	PORTB,tmp	; enable digit3
	out	PORTD,Display3	; enable segments
	ldi	wt1,2		; # of 100 us loops ...
	rcall	waitloop
done:	ret


;***********************************************************************************
; Delay Subroutine
;***********************************************************************************

WaitLoop:	cpi	wt1,$00		; make sure wt1 is not zero, return if it is
		breq	Waitdone		

Wait4:		rjmp	Wait100
Wait2:		dec	wt1		; wt1 is # of 100us loops, set in calling routine
		brne	Wait4
Waitdone:	ret

Wait100:	ldi	wt0,$C8		; delay about 100us @ 8 Mhz
Wait3:		nop
		dec 	wt0
		brne	Wait3
		rjmp	Wait2


;***********************************************************************************
; BCD to 7 Segment Display Table
; 
; Segment "a" is LSB  Segment "dp" is MSB
; The order of segments is dp g f e d c b a
; dp is set high as needed

SegTbl: .db	$3F,$06		; 7 Segment for "0"   0011 1111 and for "1"   0000 0110
	.db	$5B,$4F		; 7 Segment for "2"   0101 1011 and for "3"   0100 1111
	.db	$66,$6D		; 7 Segment for "4"   0110 0110 and for "5"   0110 1101
	.db	$7D,$07		; 7 Segment for "6"   0111 1101 and for "7"   0000 0111
	.db	$7F,$6F		; 7 Segment for "8"   0111 1111 and for "9"   0110 1111
;***********************************************************************************
