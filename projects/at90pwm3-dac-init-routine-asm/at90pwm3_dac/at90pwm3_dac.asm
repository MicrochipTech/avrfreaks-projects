	ldi	temp, (1<<REFS0)		;Vref = AVcc (DAC same Vref as ADC)
	sts	ADMUX, temp
	ldi	temp, (1<<DAEN)|(1<<DAOE)	;enable DAC, enable output (D2A)
	sts	DACON, temp
	ldi	XH, high(511)			;Vout=DAC*Vref/1023
	ldi	XL, low(511)
	sts	DACL, XL			;Vout=511*5.06V/1023 = 2.528V
	sts	DACH, XH
