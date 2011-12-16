// binclock.c
// jrlf 2011-12-14

// This program implements a simple binary clock.


#define F_CPU 4000000UL // 4Mhz

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <avr/sleep.h>
#include <stdlib.h>


// Yes, I should have used enums here. However, I was seeming to have
// trouble making them work with the compiler (it's been a long time
// since I've done C, much) so I just converted them to numbers. It's
// only four states. Sheesh.

// | Number | State   | 
// | 0      | INIT    |
// | 1      | DISPLAY |
// | 2      | BLINK   |
// | 3      | WAIT    |

unsigned char state = 0;
unsigned char hour, minute, second;
unsigned char dwait = 0;


// This interrupt service routine gets triggered every second by T/C1. It
// updates the hour, minute and second counters and sets the next display
// state.

ISR(TIMER1_COMP1_vect) {
	second++;
	if(state != 1) { state = 2; } // BLINK

	if(second > 59) {
		state = 1; // DISPLAY
		second = 0;
		minute++;
		if(minute > 59) {
			minute = 0;
			hour++;
			if(hour > 12) {hour = 1;}
		}
	}
}


int main(void) {
	DDRB  = 0xff;
	PORTB = 0xc0;

	TCCR1A = 0x00; 
	TCCR1B = 0x0b; // Clear T/C1 on match, prescale by 64.
	OCR1H  = 0xf4; // 4000000 cycles / 64 = 62500, or 0xf424.
	OCR1L  = 0x24; 
	// OCR1H = 0x00; OCR1L = 0x08; // Test count.

	TIMSK  = 0x40; // Turn on T/C1 Output Compare Match Interrupt.
	sei();

	dwait  = 0;
	hour   = 8;
	minute = 0;
	second = 0;

	while(1) {
		if(0 == state) {         // INIT
			state = 1;
		} else if(1 == state) {  // DISPLAY
			PORTB = 0x40 | hour;
			_delay_ms(10);
			PORTB = 0x80 | minute;
			_delay_ms(10);

			dwait++;
			if(dwait >= 250) { // Show display for 5 seconds (250 * 20ms).
				dwait = 0;
				state = 3;
			}
		} else if(2 == state) {  // BLINK
			PORTB = 0xa0;
			_delay_ms(200);
			state = 3;
		} else if(3 == state) {  // WAIT
			PORTB = 0xc0; // Turn off all blinkenlights.
			set_sleep_mode(SLEEP_MODE_IDLE);
			sleep_mode();
		}
	}
}
