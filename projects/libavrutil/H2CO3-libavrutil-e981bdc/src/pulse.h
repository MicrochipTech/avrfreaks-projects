/*
 * pulse.h
 * libavrutil
 *
 * Created by Árpád Goretity on 26/02/2012.
 * Licensed under a CreativeCommons Attribution-ShareAlike 3.0 Unported License
 * http://creativecommons.org/licenses/by-sa/3.0/
 */

#ifndef __AVRUTIL_PULSE_H__
#define __AVRUTIL_PULSE_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <stdint.h>
#include <stdlib.h>
#include "base.h"

/*
 * These functions generate a pulse for the specified time interval
 * (given in microseconds or milliseconds, respectively) on the port
 * pinted to by `port' and pin `pin'. (Note that you have to pass in a
 * _pointer_ to the port register, e. g. avr_pulse_generate_us(&PORTB, PIN0, 100);
 * Functions with `inv' in their name generate an inversed-logic pulse,
 * that is, they pull the output to LOW during the specified time. Note that
 * in this case, you'll need to initialize your port and pin to HIGH beforehand,
 * e. g. avr_bit_set(PORTB, PIN0).
 */

void avr_pulse_generate_us(uint8_t *port, uint8_t pin, uint16_t us);

void avr_pulse_generate_ms(uint8_t *port, uint8_t pin, uint32_t ms);

void avr_pulse_generate_inv_us(uint8_t *port, uint8_t pin, uint16_t us);

void avr_pulse_generate_inv_ms(uint8_t *port, uint8_t pin, uint32_t ms);

/*
 * These functions measure the length of a pulse in microseconds or
 * milliseconds, respectively. The microsecond-precision functions
 * may not be precise for all clock frequencies. Note that you need
 * to pass a _pointer_ to the port register to be observed, e. g.:
 * avr_pulse_measure_us(&PINB, PIN0);
 * The `inv' functions behave according to an inverse logic. (See the
 * above explanation at the pulse generation functions.)
 * The measurement functions need to be called a few microseconds
 * earlier than the start of the actual pulse to be measured.
 */

uint32_t avr_pulse_measure_us(uint8_t *port, uint8_t pin);

uint32_t avr_pulse_measure_ms(uint8_t *port, uint8_t pin);

uint32_t avr_pulse_measure_inv_us(uint8_t *port, uint8_t pin);

uint32_t avr_pulse_measure_inv_ms(uint8_t *port, uint8_t pin);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __AVRUTIL_PULSE_H__ */

