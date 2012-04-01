/*
 * servo.c
 * libavrutil
 *
 * Created by Árpád Goretity on 11/02/2012.
 * Licensed under a CreativeCommons Attribution-ShareAlike 3.0 Unported License
 * http://creativecommons.org/licenses/by-sa/3.0/
 */

#include <stdint.h>
#include <avr/io.h>
#include "servo.h"

#if defined(TCCR1A) && defined(TCCR1B)

/* Times are in microseconds */
#define __AVR_SERVO_TIVAL  20000
#define __AVR_SERVO_MIN   750
#define __AVR_SERVO_MAX  2250
#define __AVR_SERVO_FREQ (1000000 / __AVR_SERVO_TIVAL) /* 50 Hertz */

static uint16_t __avr_servo_position_a = AVR_SERVO_CENTER;
static uint16_t __avr_servo_position_b = AVR_SERVO_CENTER;
static uint32_t ticks = F_CPU / __AVR_SERVO_FREQ;

void avr_servo_init()
{
	/* Clear timer 1 control bits, just in case */
	TCCR1A = 0;
	TCCR1B = 0;
	
	/* Set timer 1 to 16-bit fast PWM, TOP = ICR1, clear on OCR1A or OCR1B */
	TCCR1A |= _BV(WGM11);
	TCCR1B |= _BV(WGM13) | _BV(WGM12);

	/*
	 * Select the prescaler value
	 */
	
	if (ticks > 8388607ULL) {
		ticks >>= 10;
		/* Prescale = 1024 */
		TCCR1B |= _BV(CS12) | _BV(CS10);
	} else if (ticks > 2097151ULL) {
		ticks >>= 8;
		/* Prescale = 256 */
		TCCR1B |= _BV(CS12);
	} else if (ticks > 524287ULL) {
		ticks >>= 6;
		/* Prescale = 64 */
		TCCR1B |= _BV(CS11) | _BV(CS10);
	} else if (ticks > 65535ULL) {
		ticks >>= 3;
		/* Prescale = 8 */
		TCCR1B |= _BV(CS11);
	} else {
		/* Prescale = 1 */
		TCCR1B |= _BV(CS10);
	}

	ICR1 = ticks;
}

void avr_servo_attach(uint8_t timer)
{
	if (timer == TIMER1A) {
		TCCR1A |= _BV(COM1A1);
	} else if (timer == TIMER1B) {
		TCCR1A |= _BV(COM1B1);
	} else {
		/* Invalid timer */
		return;
	}

	/* Set servo position to center */
	avr_servo_set_pos(timer, AVR_SERVO_CENTER);
}


void avr_servo_detach(uint8_t timer)
{
	if (timer == TIMER1A) {
		TCCR1A &= ~(_BV(COM1A1) | _BV(COM1A0));
	} else if (timer == TIMER1B) {
		TCCR1A &= ~(_BV(COM1B1) | _BV(COM1B0));
	}
}

void avr_servo_set_pos(uint8_t timer, uint16_t pos)
{
	if (timer == TIMER1A) {
		__avr_servo_position_a = pos;
		
		/* Calculate timer ticks */
		uint32_t ival_high = (__avr_servo_position_a * 1.0 * (__AVR_SERVO_MAX - __AVR_SERVO_MIN)) / (AVR_SERVO_RIGHT - AVR_SERVO_LEFT) + __AVR_SERVO_MIN;
		uint16_t ticks_high = (ticks * ival_high) / __AVR_SERVO_TIVAL;
		
		/* And write the result to the OCR1A register */
		OCR1A = ticks_high - 1;
	} else if (timer == TIMER1B) {
		__avr_servo_position_b = pos;
		
		/* Calculate timer ticks */
		uint32_t ival_high = (__avr_servo_position_b * 1.0 * (__AVR_SERVO_MAX - __AVR_SERVO_MIN)) / (AVR_SERVO_RIGHT - AVR_SERVO_LEFT) + __AVR_SERVO_MIN;
		uint16_t ticks_high = (ticks * ival_high) / __AVR_SERVO_TIVAL;
	
		/* And write the result to the OCR1B register */
		OCR1B = ticks_high - 1;
	}
}

uint16_t avr_servo_get_pos(uint8_t timer)
{
	if (timer == TIMER1A) {
		return __avr_servo_position_a;
	} else if (timer == TIMER1B) {
		return __avr_servo_position_b;
	}
	
	/* If reached, then the timer was invalid */
	return 0;
}

#endif /* TCCR1A && TCCR1B */

