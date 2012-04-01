/*
 * avrutil.h
 * libavrutil
 *
 * Created by Árpád Goretity on 11/02/2012.
 * Licensed under a CreativeCommons Attribution-ShareAlike 3.0 Unported License
 * http://creativecommons.org/licenses/by-sa/3.0/
 */

/*
 * Top-level include file for libavrutil
 *
 * The actual macro defintions, function declarations and variable
 * declarations can be found in the relevant header file.
 * This file is primarily a convenience feature. Developers intending
 * to use only partial functionalities may chhose to include specific
 * header files only regarding the functions they need.
 *
 * Please note that due space and memory constraints on microcontrollers,
 * the use of the high-quality, well-optimized avr-libc is always
 * preferred and encouraged. Only use the routines of this
 * library when it really saves time and provides good abstraction
 * and/or unification. For example, delaying for a constant amount of time
 * should be done using the _delay_ms() or _delay_us() functions insetad of
 * avr_delay_ms() and avr_delay_us().
 */

#ifndef __AVRUTIL_AVRUTIL_H__
#define __AVRUTIL_AVRUTIL_H__

#include <avrutil/analog.h>
#include <avrutil/base.h>
#include <avrutil/binary.h>
#include <avrutil/delay.h>
#include <avrutil/interrupt.h>
#include <avrutil/pulse.h>
#include <avrutil/servo.h>
#include <avrutil/shift.h>
#include <avrutil/sound.h>
#include <avrutil/usart.h>

#endif /* !__AVRUTIL_AVRUTIL_H__ */

