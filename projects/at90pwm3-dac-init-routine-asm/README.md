# AT90PWM3 DAC init routine (ASM)

Uploaded by RES on 2011-12-16 15:26:00 (rating 0 out of 5)

## Summary

 Setting up the DAC of an AT90PWM3.


 



```

	ldi	temp, (1<<REFS0)		;Vref = AVcc (DAC same Vref as ADC)
	sts	ADMUX, temp
	ldi	temp, (1<<DAEN)|(1<<DAOE)	;enable DAC, enable output (D2A)
	sts	DACON, temp
	ldi	XH, high(511)			;Vout=DAC*Vref/1023
	ldi	XL, low(511)
	sts	DACL, XL			;Vout=511*5.06V/1023 = 2.528V
	sts	DACH, XH
```

## Compilers

- wavrasm

## Tags

- Complete code
- wavrasm
- AT90PWM3
- dac setup
