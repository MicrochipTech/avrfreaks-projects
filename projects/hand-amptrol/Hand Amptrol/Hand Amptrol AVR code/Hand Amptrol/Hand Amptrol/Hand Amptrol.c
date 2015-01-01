/*
 * Hand_Amptrol.c
 *
 * Created: 8/30/2014 4:37:14 PM
 *  Author: Mike
 */ 


#include <avr/io.h>
#define F_CPU 8000000UL
#include <util/delay.h>
#include "adc.h"
#include "usispi.h"

#define POT_OUTPUT_SCALE 1.5
#define POT_OUTPUT_OFFSET 450

uint8_t POT_OUTPUT;


int main(void)
{
	adc_setup();
	usi_spi_init();
	DDRA |= (1<<PORTA4);
	PORTA |= (1<<PORTA4);
    while(1)
    {
	
		for (uint8_t i = 0; i < 25 ; i++)
	{
		_delay_ms(1);	
		usi_putc(0xfe);
		usi_putc(0x45);
		usi_putc(0x40);
		usi_putv(ADC_VALUE,10,4);
		usi_putv(POT_OUTPUT,10,4);		
		usi_putc(' ');
		usi_putc(' ');
		usi_putc(' ');
		for (uint8_t x = 0; x < 25 ; x++)
		{
		adc();
		_delay_us(500);

		}
		if (((ADC_VALUE+POT_OUTPUT_OFFSET)/POT_OUTPUT_SCALE) >= 255)
		{
			POT_OUTPUT=255;
		}
		else
		{
			POT_OUTPUT = (ADC_VALUE+POT_OUTPUT_OFFSET)/POT_OUTPUT_SCALE;	
		}
		if (((ADC_VALUE+POT_OUTPUT_OFFSET)/POT_OUTPUT_SCALE) <= 0)
		{
			POT_OUTPUT=0;
		}
		usi_spi_out(USI_SPI_CS1,POT_OUTPUT);
		if(POT_OUTPUT == 255)
		{
			PORTA &= ~(1<<PORTA4);
		}
		else
		{
			PORTA |= (1<<PORTA4);
		}
	}
        //TODO:: Please write your application code 
	}
}