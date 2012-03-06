/***********************************************

		ADC.C
					
	ADC  routunes for the AVR device
	
************************************************/

#include<avr/io.h>
#include"adc.h"


/*	Initialise ADC module with REF reference, channel-0 and with presclar 8,
	and do a sample conversion*/
void adc_init(uint8_t REF)
{
	ADMUX = REF;
	ADCSRA = ( (1<<ADEN)|(1<<ADSC)| PRESCALAR_8 ); //Enables and start a dummy converion
	while ( !(ADCSRA & (1<<ADIF)) ) 
		; // wait for dummy conversion to finish
	ADCSRA |= (1<<ADIF);
}


/*  change reference source and channel	*/
void adc_configure(uint8_t admux)
{
	ADMUX = admux;
	adc_read_10bit(); //start couple of dummy conversions and discard them 
	adc_read_10bit(); 
}	
 	
/*  Do a single converion and returns result in 8-bits */
uint8_t adc_read_8bit(void)
{
	ADMUX |= (1<<ADLAR);//enablele ADLAR (for 8-bit result)
	ADCSRA |= (1<<ADSC); // start conversion
	while ( !(ADCSRA & (1<<ADIF)) ) 
		; // wait for the conversion to finish

	ADCSRA |= (1<<ADIF); // clear ADIF flag by writing  1 to it
	return ADCH; // return 8-bit result
}


/*  Do a single converion and returns result in 16-bits */
uint16_t adc_read_10bit(void)
{
	ADMUX &= ~(1<<ADLAR); // clear ADLAR bit for full 10-bit conversion
	ADCSRA |= (1<<ADSC); // start conversion
	while ( !(ADCSRA & (1<<ADIF)) ) 
		; // wait for the conversion to finish
		
	ADCSRA |= (1<<ADIF); // clear ADIF flag by writing  1 to it
	return ADC;
	
}