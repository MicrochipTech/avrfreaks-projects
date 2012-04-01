/*
 * sound.h
 * libavrutil
 *
 * Created by Árpád Goretity on 11/02/2012.
 * Licensed under a CreativeCommons Attribution-ShareAlike 3.0 Unported License
 * http://creativecommons.org/licenses/by-sa/3.0/
 */

#ifndef __AVRUTIL_SOUND_H__
#define __AVRUTIL_SOUND_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <stdint.h>
#include "tones.h"
#include "base.h"

#if defined(TCCR1A) && defined(TCCR1B)

/*
 * Generate an 50% duty cycle PWM signal (square wave)
 * on the timer 1 channel A, with frequency `freq'
 * until avr_sound_stop is called.
 *
 * Note that the tone generator function can only be implemented
 * using a 16 bit timer, because 8 bit ones are
 * not sufficient for generating low frequencies on microcontrollers
 * with a high clock frequency. Refer to the actual AVR's datasheet
 * to find out if your AVR has 16 bit timers, and if so, which they are.
 */
void avr_sound_start(uint16_t freq);

/*
 * Stops generating sound after a call to avr_sound_start()
 */
void avr_sound_stop();

#endif /* TCCR1A && TCCR1B */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* !__AVRUTIL_SOUND_H__ */

