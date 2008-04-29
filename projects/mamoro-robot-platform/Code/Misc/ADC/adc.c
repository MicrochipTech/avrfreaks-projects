/*****************************************************************************
*Author: 	Jidan Al-Eryani,	jidan@gmx.net 
*Date: 	16.4.2007
*
*Read ADC
*
****************************************************************************/

#include <avr/io.h>
#include <inttypes.h>
#include "adc.h"

//read from the ADC
unsigned short readADC(unsigned char channel) {
	unsigned char c;
	unsigned short result = 0;
	
	// Activate ADC and set prescalar to 64
	ADCSRA = (1<<ADEN) | (1<<ADPS2) | (1<<ADPS1);

	// set the channel which the the ADC will read
	ADMUX = channel;
	
	// Referece voltage: AVCC (3.3V)
	ADMUX |= (1<<REFS0);
	
	// Initilize the ADC and do a  Dummy
	ADCSRA |= (1<<ADSC);
	while(ADCSRA & (1<<ADSC));
	
	//Convert the analgue signal 3 times and calculte the average value
	for(c=0; c<3; c++) {
		// start the conversion
		ADCSRA |= (1<<ADSC);
		// wait for the ADC result
		while(ADCSRA & (1<<ADSC));
		
		result += ADCW;
	}
	
	// deactivate the ADC
	ADCSRA &= ~(1<<ADEN);
	
	result /= 3;
	
	return result;
}
