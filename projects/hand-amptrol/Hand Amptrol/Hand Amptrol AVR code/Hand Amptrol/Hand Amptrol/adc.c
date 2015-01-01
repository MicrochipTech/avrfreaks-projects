/*
 * adc.c
 *
 * Created: 8/30/2014 4:43:01 PM
 *  Author: sasquach
 */ 
#include "avr/io.h"
#include "adc.h"
#include <avr/interrupt.h>
#include "avr/sleep.h"

int16_t ADC_BUFFER[ADC_BUFFER_POS_len];
int32_t ADC_BUFFER_SUM;
uint8_t ADC_BUFFER_POS = 0;
int16_t ADC_TEMP;
int16_t ADC_VALUE;
uint8_t SET_SWITCH_COUNT = 0;
uint8_t ADC_LOCK=0;

EMPTY_INTERRUPT(ADC_vect);


void adc_setup(void)
{
	ADMUX &= ~(0x1f);
	ADMUX |= (((ADC_MUX & 0x07)<<2) | (1<<ADC_MUX_POL) | (ADC_GAIN_HILOW & 0x01));
	ADMUX &= ~((1<<REFS0) | (1<<REFS1));
	ADMUX |= (1<<REFS1);
	ADCSRA &= ~((1<<ADPS0) | (1<<ADPS1) | (1<<ADPS2));
	ADCSRA |= ((1<<ADPS0) | (1<<ADPS2)) | (1<<ADEN) | (1<<ADIE);
	ADCSRB &= ~((1<<GSEL) | (1<<MUX5));
	ADCSRB |= ((((ADC_MUX & 1<<3)>>3)<<MUX5) | (((ADC_GAIN_HILOW & 1<<1)>>1)<<GSEL)) | (1<<BIN) | (1<<REFS2);
	DIDR0 |= (1<<ADC5D) | (1<<ADC6D);

	
	
}

void adc_read(void)
{
	set_sleep_mode(SLEEP_MODE_ADC);
	sleep_enable();
	sei();
	sleep_cpu();
	sleep_disable();

}

void adc(void)
{
	ADMUX &= ~(1<<ADC_MUX_POL);
	ADCSRB |= (1<<BIN);
	adc_read();
	ADC_TEMP = ADCW;
/* this is for 11bit bipolar sampling not working right at low voltage so will leave be	
*/
	if ((ADC_TEMP>>9)== 1) 
	{
		ADMUX |= (1<<ADC_MUX_POL);
		ADCSRB &= ~(1<<BIN);
		adc_read();
		adc_read();
		ADC_TEMP=-ADCW;		

	}
	else
	{
		ADMUX &= ~(1<<ADC_MUX_POL);
		ADCSRB &= ~(1<<BIN);
		adc_read();
		adc_read();
		ADC_TEMP=ADCW;
		
	}
/**/
	if (ADC_TEMP >= SET_SWITCH_LVL)
	{
		if (SET_SWITCH_COUNT <= SET_SWITCH_COUNT_LEN)
		{
			SET_SWITCH_COUNT++;			
		} 
		else
		{
			if (SET_SWITCH_COUNT == (SET_SWITCH_COUNT_LEN+1))
			{
				ADC_LOCK ^= 1;
			}
			if (SET_SWITCH_COUNT <= SET_SWITCH_COUNT_LEN_2)
			{
				SET_SWITCH_COUNT++;
			}
			else
			{
				SET_SWITCH_COUNT=0;
			}
		}
	}
	else
	{
		SET_SWITCH_COUNT=0;
		if (ADC_LOCK == 0)
		{
			ADC_BUFFER_SUM -= ADC_BUFFER[ADC_BUFFER_POS];
			ADC_BUFFER[ADC_BUFFER_POS] = ADC_TEMP;
			ADC_BUFFER_SUM += ADC_BUFFER[ADC_BUFFER_POS];
			if (ADC_BUFFER_POS == (ADC_BUFFER_POS_len-1) )
			{
				ADC_BUFFER_POS=0;
			}
			else
			{
				ADC_BUFFER_POS ++;
			}
			ADC_VALUE = (ADC_BUFFER_SUM / ADC_BUFFER_POS_len);
		}
	}
}