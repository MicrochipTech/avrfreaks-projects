/* Interfaceing the LDR, The intensity of light will be shown by the no. of leds on
Author: Hemal M. Chevli
Date: 5th March 2011
Project completed on 8th March 2011 */

#include<avr/io.h>
#include<avr/interrupt.h>
#include "LCD_MEGA.h"

void ADC_init(void);

void ADC_init(void)
{
	ADCSRA |= (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0); // Set ADC prescalar to 128 - 125KHz sample rate @ 16MHz 
	ADMUX |= (1 << REFS0); // Set ADC reference to AVCC 
	ADMUX |= (1 << ADLAR); // Left adjust ADC result to allow easy 8 bit reading 
   // No MUX values needed to be changed to use ADC0 
   ADCSRA |= (1 << ADFR);  // Set ADC to Free-Running Mode 
   ADCSRA |= (1 << ADEN);  // Enable ADC 
   ADCSRA |= (1 << ADSC);  // Start A2D Conversions 
}
int main(void)
{
	int i,j;
	DDRC=0xFF;
	lcd_init();	
	ADC_init();	
   for(;;)  // Loop Forever 
   { 
     
		if(1<<ADCSRA)//if converstion is complete
		{
			if(ADCH<32)
			PORTC=0b11111111;
			else if (ADCH<64)
			PORTC=0b11111110;
			else if (ADCH<95)
			PORTC=0b11111100;
			else if (ADCH<128)
			PORTC=0b11111000;
			else if (ADCH<159)
			PORTC=0b11110000;
			else if (ADCH<191)
			PORTC=0b11100000;
			else if (ADCH<223)
			PORTC=0b11000000;
			else
			PORTC=0b00000000;
		}
		lcd_gotoxy1(5);
		lcd_showvalue(ADCH);
	} 
} 



