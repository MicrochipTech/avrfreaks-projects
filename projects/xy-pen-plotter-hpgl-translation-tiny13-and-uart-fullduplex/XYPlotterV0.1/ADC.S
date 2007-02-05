#include <avr\io.h>

		.text					; switches to the .text section (code and ROM constants)
		.global	Get_adc
Get_adc:
		ori 	R24,(1<< REFS0)		; Select in Internel 1.
		sts		ADMUX,R24
		ldi		R24,( 1 << ADEN)  | ( 1 << ADSC ) | ( 1 << ADIF ) | 0x07
		sts		ADCSRA,R24
WaitForADC:
		lds		R24,ADCSRA
		andi	R24,( 1 << ADIF )
		breq	WaitForADC
		ret
