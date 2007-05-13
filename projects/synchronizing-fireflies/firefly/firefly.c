/* -----------------------------------------------------------------------
 * Title:    firefly
 * Author:   Alexander Weber <alexander.weber.0 at gmail.com>
 * Date:     18.03.2007
 * Hardware: ATtiny13v
 * Software: WinAVR 20060421
 */

// define as -D switch
//#define F_CPU 1200000	// clock 1.2MHz, internal oscillator

#include <avr/io.h>
#include <avr/eeprom.h>
#include <util/delay.h>

// use PB2 for led, pin 7
#define LED_BIT 2
// use PB3 to enable ldr, pin 2
#define ADC_ENA_BIT 3
// use ADC2, PB4, pin 3
#define CHANNEL 2
// power level
#define FLASH_POWER 8000
// daylight level
#define DAY_LIGHT 500

/*
 * get_adc
 * Return the 10bit value of the selected adc channel.
 */
uint16_t get_adc(uint8_t channel) {

	uint8_t i = 0;
	uint16_t value = 0;
	
	// enable voltage for LDR
	PORTB |= (1 << ADC_ENA_BIT);	
	
	// ADC setup
	ADCSRA = 
		(1 << ADEN) |						// enable ADC
		(1 << ADPS1) | (1 << ADPS0);		// set prescaler to 8	
		
	// select channel
	ADMUX = channel;
	
	// uncomment this to select reference voltage
	// ADMUX |= (1 << REFS0);	// use internal reference

	// warm up the ADC, discard the first conversion
	ADCSRA |= (1 << ADSC);
	while (ADCSRA & (1 << ADSC)); 
	
	// do 4 conversions and take the average
	for (i = 0; i < 4; i++) {				
		ADCSRA |= (1 << ADSC);				// start single conversion
		while (ADCSRA & (1 << ADSC)); 		// wait until conversion is done
		value += ADCW;
	}
	value = value >> 2;
		
	PORTB &= ~(1 << ADC_ENA_BIT);			// disable voltage for LDR
	ADCSRA &= ~(1<<ADEN); 					// shut down the ADC
		
	return value;
}



int main(void) {	
	
	uint8_t i = 0;
	uint16_t light = 0;
	uint16_t threshold = 0;
	uint16_t power = 0;
	uint8_t blind = 0;
	
	// define LED and ADC enable as outputs
	DDRB |= (1 << LED_BIT) |
			(1 << PB0) |
			(1 <<ADC_ENA_BIT);	
			
	// set pin 0 to low
	PORTB &= ~(1 << PB0);
	
	// intro
	for (i = 0; i < 5; i++) {
		PORTB |= (1 << LED_BIT);	
		_delay_ms(200);
		PORTB &= ~(1 << LED_BIT);
		_delay_ms(200);
	}
	
	// compute threshold
	for (i = 0; i < 4; i++) {
		threshold += get_adc(CHANNEL);
		_delay_ms(200);
		_delay_ms(200);
		_delay_ms(200);
		_delay_ms(200);
		_delay_ms(200);
	}	
	threshold = threshold >> 2;
	threshold += 80;

	while (1) {	
		
		_delay_loop_1(255);
		if (power > 6000) {
			power += 1;
		}
		else if (power > 4000) {
			power += 2;
		}
		else if (power > 3000) {
			power += 4;
		}
		else if (power > 2000) {
			power += 8;
		}
		else {
			power += 16;
		}

		if (!blind) {
		    light = get_adc(CHANNEL);
		    if (light > DAY_LIGHT) {				// if there is too much light
		    	for (i = 0; i < 20; i++) {			// then sleep some seconds
		    		_delay_ms(250);
		    	}
		    }
			else if (light > threshold) {			// if there was another flash then
				power += 200;						// increase power
				blind = 100;						// and we are blind for 100 cycles
			}
		}
		else if (blind > 0) {
			blind--;							
		}
						
		if (power > FLASH_POWER) {					// if there is enough power
			PORTB |= (1 << LED_BIT);				// flash on
			_delay_ms(200);							// for 0.2 seconds
			PORTB &= ~(1 << LED_BIT);				// flash off
			power = 0;
		}
		
	}

	return 0;
	
}
