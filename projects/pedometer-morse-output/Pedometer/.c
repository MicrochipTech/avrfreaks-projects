/*
 * ATtiny13A Pedometer
 *
 * Author:  Alan Yates <alany@ay.com.au>
 * 			Ondrej Kolonicny <ondra@ok1cdj.com>
 *
 * Sensor is Tilt ball switch from Adafruit.com
 * http://www.adafruit.com/index.php?main_page=product_info&cPath=35&products_id=173
 * Sensor - PB3
 * PIEZO  - PB0
 * BUTTON - PB4
 * 
 *
 */
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>


#include "data.h"

#define BEEP_TONE 800				// Hz
#define BEEP_HALF_PERIOD 1000000/BEEP_TONE	// us
#define SHORT_BEEP 100000			// us
#define LONG_BEEP 1000000			// us

#define DEBOUNCE_WAIT 20	// ms
#define HOLD_POLL_INTERVAL 50	// ms
#define HOLD_TIME 2000		// ms

#define WPM 15			// morse speed in words-per-minute
#define DIT_TIME (1200/WPM)	// dit interval in milliseconds

#define PIEZO _BV(PB0)		// piezo pin
#define BUTTON _BV(PB4)		// button pin
#define SENSOR _BV(PB3)		// footstep switch pin

// need to be set to another values if different sensor is used
#define SENSOR_DEBOUNCE 50	// ms
#define SENSOR_LOCKOUT 300	// ms ~3Hz faster than most human gaits cadences

// average step - man 0.7m-0.75m , woman 0.6m, child 0.54m
// average step 0.63m
// insert step*100

#define AVG_STEP 63

// if DISP_M = 1 play distance in m else count of steps
#define DISP_M 1

volatile unsigned count;	// footstep count

void init() {
	cli();

	// piezo output
	DDRB = PIEZO;
	
	// button input
	PORTB = BUTTON | SENSOR;
	PCMSK = BUTTON | SENSOR;
	GIMSK = _BV(PCIF);

	// power config
	set_sleep_mode(SLEEP_MODE_PWR_DOWN);
	count = 0;
	sei();
}

void tick() {
	PORTB ^= PIEZO;
}

void beep(unsigned long us) {
	for(; us > 0; us -= BEEP_HALF_PERIOD) {
		PORTB ^= PIEZO;
		_delay_us(BEEP_HALF_PERIOD);
	}
}

unsigned char debounce_button() {
	if(!(PINB & BUTTON)) {
		_delay_ms(DEBOUNCE_WAIT);
		return (!(PINB & BUTTON)?(1):(0));
	}
	return 0;
}

// 0 = not pressed, 1 = a press, 2 = a hold
unsigned char button_press() {
	unsigned heldTicks = 0;
	if(!debounce_button()) return 0;
	while(heldTicks < HOLD_TIME/HOLD_POLL_INTERVAL) {
		if(PINB & BUTTON) return 1;
		_delay_ms(HOLD_POLL_INTERVAL);
		heldTicks++;
	}
	return 2;
}

unsigned char footstep() {
	if(!(PINB & SENSOR)) {
		_delay_ms(SENSOR_DEBOUNCE);
		if(PINB & SENSOR) return 0;
		_delay_ms(SENSOR_LOCKOUT);
		return 1;
	}
	return 0;
}

void sendMorse(unsigned char num) {
	unsigned char i;
	unsigned data;

	if(num > 9) return;

	data = pgm_read_byte(&morseData[2*num]) << 8 | pgm_read_byte(&morseData[(2*num) + 1]);
	for(i = 0; i < NUM_LENGTH*2; i += 2) {
		switch(data >> (14 - i) & 0x03) {
			case 0:
				_delay_ms(DIT_TIME * 7);
				break;
			case 1:
				_delay_ms(DIT_TIME * 3);
				break;
			case 2:
				beep(DIT_TIME * 1000UL);
				_delay_ms(DIT_TIME);
				break;
			case 3:
				beep(DIT_TIME * 3000UL);
				_delay_ms(DIT_TIME);
				break;
		}
	}
}

void display() {
	unsigned char i, n, first = 1;
	unsigned m, c;
	
	if(DISP_M){
	// convert foot steps to meters
	m=count*AVG_STEP/100;
	} else m=count;
	c=m;

	for(i = 0; i < 5; i++) {
		m = pgm_read_word(&mags[(unsigned)i]);
		for(n = 0; c >= m; c -= m) n++;
		if(first && n < 1) continue;
		first = 0;
		sendMorse(n);
		_delay_ms(DIT_TIME * 3);
	}

	// zero
	if(first) {
		sendMorse(0);
	}
}

ISR(PCINT0_vect) {
	switch(button_press()) {
		case 1:
			display();
			break;
		case 2:
			count = 0;
			beep(LONG_BEEP);
			break;
		default:
			if(footstep()) {
				count++;
				tick();
			}
	}

	// We clear the interrupt flag here to prevent
	// button interrupts from causing a false count
	// from the sensor if it was held low during the
	// button press.
	GIFR |= _BV(PCIF);
}

int main() {
	init();
	while(1) sleep_mode();
	return 0; // unreached
}
