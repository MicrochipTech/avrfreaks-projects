/*
 * servo.h
 * libavrutil
 *
 * Created by Árpád Goretity on 11/02/2012.
 * Licensed under a CreativeCommons Attribution-ShareAlike 3.0 Unported License
 * http://creativecommons.org/licenses/by-sa/3.0/
 */

/*
 * Functions for controlling a servo motor. For this task, we use
 * a high precision, a. k. a. 16-bit timer: timer 1 channel A, so
 * that we can control the servo position (angle) almost continuously.
 * (anyways, the 65536 timer step is needed to implement the 1000
 * microsecond high and 20 millisecond low state...)
 */

#ifndef __AVRUTIL_SERVO_H__
#define __AVRUTIL_SERVO_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <stdint.h>
#include <avr/io.h>
#include "base.h"

#if defined(TCCR1A) && defined(TCCR1B)

#define AVR_SERVO_LEFT 0
#define AVR_SERVO_CENTER 511
#define AVR_SERVO_RIGHT 1023

/*
 * Initialize the servo driver system.
 * This function MUST be called before any other avr_servo_*() function.
 */
void avr_servo_init();

/*
 * Start servo control on timer 1, either channel A or B
 */
void avr_servo_attach(uint8_t timer);

/*
 * Stop servo control on the specified channel of timer 1
 */
void avr_servo_detach(uint8_t timer);

/*
 * Set servo position: 0 = leftmost, 1023 = rightmost position,
 * 511 = center. The position value is not bound-checked in order
 * not to overdrive the servos.
 */
void avr_servo_set_pos(uint8_t timer, uint16_t pos);

/*
 * Get servo position
 */
uint16_t avr_servo_get_pos(uint8_t timer);

#endif /* TCCR1A && TCCR1B */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* !__AVRUTIL_SERVO_H__ */

