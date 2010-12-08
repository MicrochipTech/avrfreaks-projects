/**
 * adc.c
 *
 * adc conversions
 */

#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/delay.h>

#include "common.h"
#include "adc.h"

uint16_t v_battery=0;
uint16_t v_current=0;
uint16_t v_temperature=0;

#define CORRECTION 		-13

#define CHAN_VOLTAGE		0
#define CHAN_CURRENT		1
#define CHAN_TEMPSENS	8

uint8_t selector=0;

uint16_t counter=0;
uint16_t average=0;

/** Make single conversion **/

uint16_t SingleConversion(void)
{
	/** start conversion **/
	ADCSRA |= (1<<ADSC);
	/** wait finish **/
	while( (ADCSRA & (1<<ADSC)) !=0 );

	return ADC;
}

#define TEMP_CORR  82  // calib, error
static uint16_t skip_cicle=0;

ISR(ADC_vect)
{
	static uint16_t sample;

	if (skip_cicle && (skip_cicle++ < 6250)) return; else skip_cicle = 0;

	/* result in ADCL & ADCH */
	sample = ADC;

	// discard first reading
	if (counter) average+=sample;
	counter++;

	if (counter==16)
	{
		counter=0;
		average/=15;

		switch (selector)
		{
			case 0:
				average  += CORRECTION;
				v_battery = (average*50)>>8;
				ADMUX = 0x40 | CHAN_CURRENT;
			break;
			case 1:
				/** 410 divisioni = 20A, 410:20=(read-512):x, x=20*read/410 **/
				if (average<512) average=512;
				average-=512;
				// no negative values visible
				v_current = (average*200)/410;
				/** now got with temp reading, 1.1V REF **/
				ADMUX = (1<<REFS1) | (1<<REFS0) | CHAN_TEMPSENS;
				/** wait to 100msec stabilize to 1,1V **/
				skip_cicle++;
			break;
			case 2:
				// conversion to mV based on 1.1 VREF
				sample = (uint32_t)average*1100/1024;
				// add fixed error
				sample -= TEMP_CORR;
				// transform to temperature, remove 0*C as per datasheet 23.8
				v_temperature = (sample-289)*10;
				/** now got with 5V REF **/
				ADMUX = 0x40 | CHAN_VOLTAGE;
				/** wait to stabilize back to 5V **/
				skip_cicle++;
			break;
		}
		average=0;
		selector++;
		if (selector==3) selector=0;
	}
	/* start the next conversion */
	ADCSRA |= (1<<ADSC);
}

void InitADC (void)
{
	DDRC   = GLOBAL_DIRC;

	/** prescaler a 128, 8Mhz/128 **/

	ADCSRA = (1<<ADEN) | (1<<ADATE) | (1<<ADIE) | (1<<ADPS2) | (1<<ADPS1) | (1<<ADPS0);


	ADCSRB = 0; /* free run */
	ADMUX  = 0x40 | CHAN_VOLTAGE; /* PORTC 0 */

	/* start conversion */
	ADCSRA |= (1<<ADSC);
}

