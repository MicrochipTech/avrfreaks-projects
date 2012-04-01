/*
 * delay.h
 * libavrutil
 *
 * Created by Árpád Goretity on 11/02/2012.
 * Licensed under a CreativeCommons Attribution-ShareAlike 3.0 Unported License
 * http://creativecommons.org/licenses/by-sa/3.0/
 */

/*
 * Microsecond- and millisecond-resolution delay functions
 * (they all work approximately, giving a minimum amount of delay)
 */

#ifndef __AVRUTIL_DELAY_H__
#define __AVRUTIL_DELAY_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <stdlib.h>
#include <stdint.h>
#include <avr/io.h>
#include <util/delay.h>
#include "base.h"

#if F_CPU % 4000000ULL
#warning CPU frequency is not divisible by 4 MHz,
#warning microsecond-precision delay functions may malfunction
#warning (you can still use _delay_us() or _delay_loop_1() from the avr-libc
#warning <util/delay.h> and <util/delay_basic.h>, respectively).
#endif

/*
 * The maximum possible delay which can be realized using avr_delay_us().
 * Specifying greater values is undefined behaviour.
 */
#define AVR_MAX_DELAY_US ((65536 * 4 * 1000000) / F_CPU)

/*
 * Delay for `us' microseconds using busy wait. This function is not
 * very accurate for small delays (e. g., < 10 microseconds, depending
 * on the MCU clock frequency). Relies on F_CPU being properly defined.
 * This function MUST be supplied a value less than or equal to
 * AVR_MAX_DELAY_US. Specifying greater values is undefined behaviour.
 */
__avr_inline void avr_delay_us(uint16_t us);

void avr_delay_us(uint16_t us)
{
	uint16_t loops = ((us * F_CPU) / 1000000.0 / 4);
	
	if (!loops)
	{
		loops = 1;
	}

	_delay_loop_2(loops);
}

#if F_CPU % 4000ULL
#warning CPU frequency is not divisible by 4 kHz,
#warning millisecond-precision delay functions may malfunction
#warning (you can still use _delay_ms() or _delay_loop_2() from the avr-libc
#warning <util/delay.h> and <util/delay_basic.h>, respectively).
#endif

/*
 * Delay `ms' milliseconds by busy waiting. This function is fairly
 * accurate even for a 1 ms delay. Also relies on F_CPU to be defined properly.
 * You can specify any value as an argument for this function (within the
 * boundaries of uint32_t), it will wait for any requested timer interval.
 */
void avr_delay_ms(uint32_t ms);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* !__AVRUTIL_DELAY_H__ */

