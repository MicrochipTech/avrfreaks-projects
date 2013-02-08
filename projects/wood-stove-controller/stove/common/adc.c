/*
 * adc.c
 *
 * Created: 12/28/2012 12:47:34 PM
 *  Author: sasquach
 */ 
#include <avr/io.h>
#include <adc.h>
//#include "config.h"
uint8_t ADC_BUFFER[64];
uint16_t ADC_BUFFER_SUM = 0x3fc0;
uint8_t ADC_BUFFER_POS = 0;
uint8_t ADC_VALUE1;
uint8_t ADC_VALUE2;
uint8_t ADC_VALUE3;


void adc_init(void)
{
	ADMUX = _BV(REFS0) | _BV(REFS1) | _BV(ADLAR);
	ADCSRA |= (ADC_CLOCK_SCALER | _BV(ADEN));
	ADCSRA |= _BV(ADSC);
	for (uint8_t x = 0; x < 64 ; x++ )
	{
		ADC_BUFFER[x] = 0xff;
	}
}

void adc(void)
{
	for( ;; )
	{
		ADMUX &= ~(_BV(MUX0));
		
		ADCSRA |= _BV(ADSC);
		
		while (bit_is_clear(ADCSRA,ADIF)) 
		{
				vTaskDelay(1);
		}
		
		ADC_BUFFER_SUM -= ADC_BUFFER[ADC_BUFFER_POS];
		ADC_BUFFER[ADC_BUFFER_POS] = ADCH;
		ADC_BUFFER_SUM += ADC_BUFFER[ADC_BUFFER_POS];
		ADC_VALUE1 = (ADC_BUFFER_SUM >> 6);
		ADC_BUFFER_POS++;
		ADC_BUFFER_POS &= 0x3f;
		
		vTaskDelay(ADC_DELAY);
		
		ADMUX |= _BV(MUX0);
		
		ADCSRA |= _BV(ADSC);
		
		while (bit_is_clear(ADCSRA,ADIF))
		{
			vTaskDelay(1);
		}
		
		ADC_VALUE2 = ADCH;
		
		vTaskDelay(ADC_DELAY);
		ADMUX &= ~(_BV(MUX0));
		#ifndef DOWN_STAIRS_
		
		ADMUX |= _BV(MUX2);
		
		ADCSRA |= _BV(ADSC);
		
		while (bit_is_clear(ADCSRA,ADIF))
		{
			vTaskDelay(1);
		}
		
		ADC_VALUE3 = ADCH;
		vTaskDelay(ADC_DELAY);
		ADMUX &= ~(_BV(MUX2));
		#endif	
	}
}