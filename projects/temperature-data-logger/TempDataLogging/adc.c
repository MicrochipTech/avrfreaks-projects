//**********************************************************************//
//FILE: adc.c 
//AUTHOR: Adam Kadolph
//DATE:	01-7-09
//DESCRIPTION: C file to handle A/D Converter initialization and run routines
//**********************************************************************//

#include <avr/io.h>
#include "adc.h"

void ADCinit(void)
{
	ADMUX = (1<<MUX0);	//setup A/D to use channel 1

	ADCSRA = (1<<ADEN)|(1<<ADPS2)|(1<<ADPS1);	//setup ADC with division factor of 64 giving ~125kHz ADC clk
}

void ADCstart(void)
{
	ADCSRA |= (1<<ADSC);	//start conversaion

	while(!(ADCSRA & (1<<ADIF)));	//wait for conversion to complete
}
	
void TempConvert(unsigned char ADC_DATA, unsigned char i)
{
	unsigned char temp,tens,ones,tenth;

	//ADC_DATA = ADC_DATA - (ADC_DATA / 0x32);	//error correction

	tens = (ADC_DATA / 0x02) / 0x0A;

	temp = (((ADC_DATA / 0x02) - (tens * 0x0A)) * 0x15) - 0x64;	//calculation for ones and tenths digits
	
	ones = temp / 0x0A;
	
	tenth = temp - (ones * 0x0A);

	temperature[i] = tens | 0x30;	//store data in ascii form for display
	temperature[i+1] = ones| 0x30;
	temperature[i+2] = tenth| 0x30;
		
}
