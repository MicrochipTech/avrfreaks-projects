/*
 * sound.c
 * libavrutil
 *
 * Created by Árpád Goretity on 11/02/2012.
 * Licensed under a CreativeCommons Attribution-ShareAlike 3.0 Unported License
 * http://creativecommons.org/licenses/by-sa/3.0/
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include "sound.h"

#if defined(TCCR1A) && defined(TCCR1B)

void avr_sound_start(uint16_t freq)
{
	uint32_t timer_ticks_per_cycle = F_CPU / freq / 2;
	/*
	 * Divide it by 2, because x Hz would ideally mean a sinusoid wave
	 * with a period of 1 / x seconds, and in one period, there's a minimum
	 * and a maximum of the wave. Thus, the square wave has to change
	 * twice within one single period.
	 */
		
	/* Clear timer 1 bits, in case it was used before by avr_pwm_*() */
	TCCR1A = 0;
	TCCR1B = 0;
	
	/* Put timer 1 into CTC mode */
	TCCR1B |= _BV(WGM12);
	/* Toggle OC1A on compare match */
	TCCR1A |= _BV(COM1A0);
	
	/*
	 * Select the prescaler value
	 * First, clear all prescaler bits
	 */
	TCCR1B &= ~(_BV(CS12) | _BV(CS11) | _BV(CS10));
	
	if (timer_ticks_per_cycle > 8388607ULL) {
		timer_ticks_per_cycle >>= 10;
		/* Prescale = 1024 */
		TCCR1B |= _BV(CS12) | _BV(CS10);
	} else if (timer_ticks_per_cycle > 2097151ULL) {
		timer_ticks_per_cycle >>= 8;
		/* Prescale = 256 */
		TCCR1B |= _BV(CS12);
	} else if (timer_ticks_per_cycle > 524287ULL) {
		timer_ticks_per_cycle >>= 6;
		/* Prescale = 64 */
		TCCR1B |= _BV(CS11) | _BV(CS10);
	} else if (timer_ticks_per_cycle > 65535ULL) {
		timer_ticks_per_cycle >>= 3;
		/* Prescale = 8 */
		TCCR1B |= _BV(CS11);
	} else {
		/* Prescale = 1 */
		TCCR1B |= _BV(CS10);
	}
	
	/*
	 * Set the number of timer ticks per toggle - taking into account
	 * the first being 0, thus we have to substract 1 from the actual tick count
	 */
	OCR1A = timer_ticks_per_cycle - 1;
}

void avr_sound_stop()
{
	/* Disconnect pin from timer 1 channel A */
	TCCR1A &= ~(_BV(COM1A1) | _BV(COM1A0));
	/* Stop timer: clear all prescaler bits */
	TCCR1B &= ~(_BV(CS12) | _BV(CS11) | _BV(CS10));
}

#endif /* TCCR1A && TCCR1B */

