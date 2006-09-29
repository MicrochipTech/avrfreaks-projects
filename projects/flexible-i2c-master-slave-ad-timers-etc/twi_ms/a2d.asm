


;A/D defs (single ended, 8MHz clk, external AREF, left justified)
.equ admux_val = 0xE0	;left justified, internal 2.56V ref, PA0
.equ adcsra_val = 0x8F	;enable things including irqs, clk/128 (62.5KHz),
						; so 208usec per conversion, updating each of the
						; 5 voltage readings about 962 times per second
.equ sfior_mask = 0xF7	;disable the comparitor input stuff (ACME)
.equ AD_start = 0x40	;the missing bit from ADCSRA
.equ ADMux_mask = 0x1F	;mask of the mux bits of ADMUX
.equ AD_Tbl_Max = 3

InitA2D:
			;Initialize and configure the A/D converter
			;
			;The setup is single ended, left justified, and continuous
			; auto sampling, so the high byte contains all of the data that
			; we care about for the 8 bit displays.
			;
			;Default to PA0 for the input mux
			;
			;go ahead and start conversions at the end

	ldi Temp, admux_val
	out ADMUX, Temp

	in Temp, SFIOR
	andi Temp, sfior_mask	;not really necessary since this is 0, but ok form.
	out SFIOR, Temp			; note that SFIOR is being overwritten here

	ldi Temp, adcsra_val
	out ADCSRA, Temp

	sbi ADCSRA, ADSC	;do a first conversion

	ret

;
;	So the A2D interrupt service routing just reads the result of the completed conversion,
;		puts it into the proper I2C register location, goes to the next input looping
;		around as needed, and starts the next conversion.  This means that the I2C A/D
;		values should always be very up to date and can be used by any routine that
;		wants to know.
;
;	If needed, since this is much faster than required, we could take multiple readings and
;	average them (FIR filter) of 4 or 8 samples, but this is probably not needed due to the
;	filtering cap on the signals.  Might be fun to do sometime though.
;
A2D_ISR:
	st	-Y, Temp
	in	Temp, SREG
	st	-Y, Temp
	st	-Y, T2
	st	-Y, XL
	st	-Y, XH

	ldi	XL, low(I2C_AD0)
	ldi	XH,	high(I2C_AD0)	;this is the base of the A/D registers

	in	Temp, ADMUX
	andi	Temp, ADMux_mask	;get the offset for the conversion that just finished from the mux
	clr	T2

	add	XL, Temp
	adc	XH, T2					;now we have the pointer

	in	T2, ADCH
	st	X, T2					;write the new value

	inc Temp					;go to the next A/D input
	cpi	Temp, AD_Tbl_Max
	brlo	A2D_ISR_1
	clr	Temp					;wrap back around if it's time

A2D_ISR_1:
	ori	Temp, admux_val
	out	ADMUX, Temp				;output the new mux value

	sbi	ADCSRA, ADSC			; and start the next conversion

	ld	XH, Y+
	ld	XL, Y+
	ld	T2, Y+
	ld	Temp, Y+
	out	SREG, Temp
	ld	Temp, Y+
	reti

	
	
