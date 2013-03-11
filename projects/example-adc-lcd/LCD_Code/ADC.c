




#include <avr/io.h>
#include <inttypes.h>

#ifndef F_CPU
	#define F_CPU 12000000UL
#endif

#include <util/delay.h>

#include "ADC.h"




void init_adc(void)
{
	DDRC |= 0x00; // set port for input
	ADCSRA = _BV(ADEN) | _BV(ADPS2);    // Activate ADC with Prescaler 16 --> 1Mhz/16 = 62.5kHz
	
}

unsigned int Read_ADC(unsigned char channel)
{
	ADCSRA = _BV(ADEN) | _BV(ADPS2);
	ADMUX = channel;						// Select pin ADC0 using MUX		
	ADCSRA |= _BV(ADSC);			// Start conversion 		
	while (ADCSRA & _BV(ADSC)) 	// wait until converstion completed
	{
	
	}
	return ADCW;

}

