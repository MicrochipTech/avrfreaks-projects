/*
 * For the STK500. Connect PORTB to LEDs and PORTD to SWITCHes.
 */
#include <stdlib.h>
#include <avr/io.h>
#define F_CPU 100000UL 
#include <util/delay.h>

// Globals
int leds[9] = {255,254,253,251,247,239,223,191,127};
int i = 0;
int comp = 0;

// Prototypes

int main(void) {
	DDRD = 0;    /* set for input. */
	DDRB = 0xff; /* enable as output */

	while(1) {
		if ( i == 0 ) {
			PORTB=leds[0];
		}
		while ( i == 0 ) {
			if ( ( PIND & ( 1 << PD0 ) ) == 0 ) {
				PORTB=leds[1];
				i=1;
			}
			else if ( ( PIND & ( 1 << PD1 ) ) == 0 ) {
				PORTB=leds[2];
				i=2;
			}
			else if ( ( PIND & ( 1 << PD2 ) ) == 0 ) {
				PORTB=leds[3];
				i=3;
			}
			comp = rand()%3+1;
		}

		_delay_ms(5000);
		PORTB=leds[comp+3] & leds[i];
		_delay_ms(3000);
		_delay_ms(5000);
		_delay_ms(3000);
		_delay_ms(5000);
		_delay_ms(3000);
		_delay_ms(5000);
		_delay_ms(3000);
		_delay_ms(5000);

		if ( comp == i ) {
			for (int j=0;j<5;j++) {
				_delay_ms(3000);
				PORTB=leds[1]&leds[2]&leds[3]&leds[4]&leds[5]&leds[6];
				_delay_ms(3000);
				PORTB=0xFF;
				_delay_ms(3000);
				PORTB=leds[comp+3] & leds[i];
			}
		}
		else if ( ( comp == 1 && i == 3 ) || ( comp == 2 && i == 1 ) || (comp == 3 && i == 2 ) ) {
			for (int j=0;j<5;j++) {
				_delay_ms(3000);
				PORTB=leds[4]&leds[5]&leds[6];
				_delay_ms(3000);
				PORTB=0xFF;
				_delay_ms(3000);
				PORTB=leds[comp+3] & leds[i];
			}
		}
		else {
			for (int j=0;j<5;j++) {
				_delay_ms(3000);
				PORTB=leds[1]&leds[2]&leds[3];
				_delay_ms(3000);
				PORTB=0xFF;
				_delay_ms(3000);
				PORTB=leds[comp+3] & leds[i];
			}
		}

		i=0;
	}
	return 1;
}



