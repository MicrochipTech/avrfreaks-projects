//
// ADC control object
//
// ADC runs in interrupt
// 
//   



#include <avr/io.h>
#include <inttypes.h>
#include <avr/signal.h>

#include "counter.h"
#include "adc.h"

void adc_init(void)
{ 
	//channel 0 + AVCC + external cap on AREF pin
	ADMUX = _BV(REFS0); 
	// adc enabled, adc started, set adc clock
	//clock 8 MHZ / 64 = 125kHz
	ADCSRA = _BV(ADEN) | _BV (ADIE) | _BV(ADSC) | _BV(ADPS2) | _BV(ADPS1); 
}

ISR(ADC_vect) //interrupts are disabled
{
	if (ADMUX & 0x1) //PC1
	{
		adc_dir = ADC >> 2;//8 bits are enough 
		ADMUX = _BV(REFS0);
	}
	else
	{
		adc_clock = ADC >> 2;//8 bits are enough
		ADMUX = _BV(REFS0) | 1;
	}

	ADCSRA |= _BV(ADSC); //start next conversion
}


