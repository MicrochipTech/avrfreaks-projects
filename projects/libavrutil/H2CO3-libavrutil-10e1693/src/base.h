/*
 * base.h
 * libavrutil
 *
 * Created by Árpád Goretity on 11/02/2012.
 * Licensed under a CreativeCommons Attribution-ShareAlike 3.0 Unported License
 * http://creativecommons.org/licenses/by-sa/3.0/
 */

/*
 * Fundamental microcontroller-friendly definitions,
 * various useful utility functions and macros
 */

#ifndef __AVRUTIL_BASE_H__
#define __AVRUTIL_BASE_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <avr/io.h>
#include <avr/wdt.h>
#include "binary.h"

#define __avr_inline __attribute__((always_inline)) static inline

/*
 * Useful math macros used for i. e. mapping different sensor
 * levels to each other.
 */

#define interpolate(x, xmin, xmax, ymin, ymax) (((x) - (xmin)) / ((xmax) - (xmin)) * ((ymax) - (ymin)) + (ymin))
#define limit(val, min, max) ((val) < (min) ? (min) : (val) > (max) ? (max) : (val))
#define nop() __asm__ __volatile__("nop")
#define avr_soft_reset() ((void (*)())0)()

/*
 * Pin mode and pin state definitions
 */

#define AVR_OUT 1
#define AVR_IN 0
#define AVR_HIGH 1
#define AVR_LOW 0

/*
 * Timer definitions
 */

#define TIMER0A 0x00
#define TIMER0B 0x01
#define TIMER1A 0x02
#define TIMER1B 0x03
#define TIMER2  0x04
#define TIMER2A 0x05
#define TIMER2B 0x06
#define TIMER3A 0x07
#define TIMER3B 0x08
#define TIMER3C 0x09
#define TIMER4A 0x0a
#define TIMER4B 0x0b
#define TIMER4C 0x0c
#define TIMER5A 0x0d
#define TIMER5B 0x0e
#define TIMER5C 0x0f

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* !__AVRUTIL_BASE_H__ */

