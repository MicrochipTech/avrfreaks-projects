/* -----------------------------------------------------------------------
 * Title:    flick led
 * Author:   Alexander Weber <alexander.weber.0 at gmail.com>
 * Date:     21.02.2007
 * Hardware: ATtiny13v
 * Software: WinAVR 20060421
 */

// define as -D switch
//#define F_CPU 1200000	// Taktfrequenz: 1.2MHz, internal oscillator


#include <avr/io.h>
#include <util/delay.h>

#define TRUE 1
#define FALSE 0 
#define ON 1
#define OFF 0

// use PB2 for led, pin 7
#define LED_BIT 2
// use PB3 to enable ldr, pin 2
#define ADC_ENA_BIT 3
// pause
#define PAUSE 25
// select ADC2, PB4, pin 3
#define CHANNEL 2
// values over threshold are recorded as "on"
#define THRESHOLD 800
// storage for recorded values
#define MAX 400
static uint8_t values[MAX / 8];


/*
 * get_adc
 * Return the 10bit value of the selected adc channel.
 */
uint16_t get_adc(uint8_t channel) {

	// enable voltage for adc 
	PORTB |= (1 << ADC_ENA_BIT);	
	
	// ADC setup
	ADCSRA = 
		(1 << ADEN) |					// enable ADC
		(1 << ADPS1) | (1 << ADPS0);	// set prescaler to 8	
		
	// select channel
	ADMUX = channel;
	
	// select reference voltage
	// ADMUX |= (1 << REFS0);	// use internal reference

	// warm up the ADC, discard the first conversion
	ADCSRA |= (1 << ADSC);
	while (ADCSRA & (1 << ADSC)); 
	
	ADCSRA |= (1 << ADSC);				// start single conversion
	while (ADCSRA & (1 << ADSC)); 		// wait until conversion is done
		
	PORTB &= ~(1 << ADC_ENA_BIT);		// disable voltage for adc
		
	return ADCW;
}



int main(void) {
	
	uint16_t i = 0;
	uint16_t count = 0;
	uint8_t light = OFF;
	uint8_t last_light = OFF;
	uint16_t last_time = 0;
	uint8_t programming = TRUE;
	
	// define LED and ADC enable as outputs
	DDRB |= (1 << LED_BIT) |
			(1 <<ADC_ENA_BIT);	
	
	// intro
	for (i = 0; i < 5; i++) {
		PORTB |= (1 << LED_BIT);	
		_delay_ms(200);
		PORTB &= ~(1 << LED_BIT);
		_delay_ms(200);
	}
	for (i = 0; i < 5; i++) {
		_delay_ms(100);
	}

	while (1) {	
			
		if (programming) {
			// signal that we are ready to program
			for (i = 0; i < 5; i++) {
				PORTB |= (1 << LED_BIT);	
				_delay_ms(40);
				PORTB &= ~(1 << LED_BIT);
				_delay_ms(40);
			}
			// now read the ldr and store it
			for (i = 0; i < MAX; i++) {
				if (get_adc(CHANNEL) > THRESHOLD) {
					values[i / 8] |= (1 << (i % 8));
					PORTB |= (1 << LED_BIT);	
				}
				else {
					values[i / 8] &= ~(1 << (i % 8));
					PORTB &= ~(1 << LED_BIT);
				}							
				_delay_ms(PAUSE);
			}
			// signal that we are finished with programming
			for (i = 0; i < 5; i++) {
				PORTB |= (1 << LED_BIT);	
				_delay_ms(40);
				PORTB &= ~(1 << LED_BIT);
				_delay_ms(40);
			}
			// switch to playback
			programming = FALSE;
			count = 0;				
		}		
		else {	
			// have we detected a change?
			light = (get_adc(CHANNEL) > THRESHOLD) ? ON : OFF;			
			if (light != last_light) {
				// when was the last change?
				if ((count - last_time) == 10) {
					programming = TRUE;			// switch to programming mode
				}
				else {
					last_time = count;
				}
			}
			last_light = light;
			
			// replay recorded lights
			if (values[count / 8] & (1 << (count % 8))) {
				PORTB |= (1 << LED_BIT);	
			}
			else {
				PORTB &= ~(1 << LED_BIT);
			}
			_delay_ms(PAUSE);
			count++;
			if (count == MAX) {
				count = 0;
			}		
		}
		
	}

	return 0;
	
}

