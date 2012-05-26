#ifdef __GNUC__ 
	#include <avr/io.h>
#else
	#include <avr/iom16.h>
#endif
#include "ADC.h"
#include "display.h"
#include "thermistor.h"
#include <avr\pgmspace.h>

void InitADC(void)
{
	ADMUX = 2;                           /* SELECT CHANNEL 1*/
	ADCSRA = (1>>ADPS0) | (1>>ADPS1)     /*(CLK/8)*/
		| (1<<ADEN)                   /* ENABLE THE ADC*/
		| (1<<ADSC);                  /*START FIRST DUMMY CONVERTON*/
	while (!(ADCSRA & (1<<ADIF))) ;             /*WAIT FOR CONVERTION TO COMPLETE*/
	ADCSRA |=(1<<ADIF); /*CLEAR ADC CONVERSION COMPLETE INTERRUPT FLAG*/
}

unsigned int ReadADC(unsigned char Channel)
{
	int timeout = 10000;
	ADMUX =Channel; /* SELECT CHANNEL*/
	ADCSRA |= (1<<ADSC);   /*START CONVERTON*/
	while (!(ADCSRA & (1<<ADIF)) && (--timeout )) ;             /*WAIT FOR CONVERTION TO COMPLETE*/
	return ADC ;  /*ADCH:ADCL*/
}

void ReadTemperature(void)
{
	unsigned int t=ReadADC(2);
	if(t<66)
	{
		write_temp(0);
		return;
	}
	if(t>784)
	{
		write_temp(999); // 99.9 
		return;
	}
	write_temp(pgm_read_word (&TempTable[t]));
}
