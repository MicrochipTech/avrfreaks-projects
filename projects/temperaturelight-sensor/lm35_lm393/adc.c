/*
Description:
    A library to easily access ADC with AVR series of
    MCUs from Atmel. Use with AVR studio and WinAVR.

Author:
    Panos Mavrogiannopoulos
Web:
    http://nmav.homeip.net
*/

#include <avr/io.h>
#include <inttypes.h>

#ifndef F_CPU
	#define F_CPU 2000000UL
#endif

#include <util/delay.h>

#include "adc.h"



void InitADC()
{
ADMUX=(1<<REFS0);                         // For Aref=AVcc;
ADCSRA=(1<<ADEN)|(1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0); //Rrescalar div factor =128
}






uint16_t ReadADC(uint8_t ch)
{
   InitADC();
   //Select ADC Channel ch must be 0-7
   ch=ch&0b00000111;
   ADMUX|=ch;

   //Start Single conversion
   ADCSRA|=(1<<ADSC);

   //Wait for conversion to complete
   while(!(ADCSRA & (1<<ADIF)));

   //Clear ADIF by writing one to it
   //Note you may be wondering why we have write one to clear it
   //This is standard way of clearing bits in io as said in datasheets.
   //The code writes '1' but it result in setting bit to '0' !!!

   ADCSRA|=(1<<ADIF);

   return(ADC);
}


