/* copyright
 * (c) 2004 Stephen Humble stephenhumble at telstra.com
 *
 *  This file is part of robotx see http://Robotx.sourceforge.net
 *  dual sensorless motor driver firmware.
 *
 *  This is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
*/
#include <io.h>
#include <avr/io.h>
#include <inttypes.h>
#include <math.h>
#include <avr/timer.h>
#include <avr/interrupt.h>
#include <avr/signal.h>

#ifndef COMMON_H
#define COMMON_H

#define RED_PORT PORTB
#define RED_DDR DDRB
#define RED_PIN PINB
#define RED_BIT 0

#define GREEN_PORT PORTB
#define GREEN_DDR DDRB
#define GREEN_PIN PINB
#define GREEN_BIT 2

// debug leds - may be available on some targets only
#define RED_OFF RED_PORT&=~_BV(RED_BIT)
#define RED_ON RED_PORT|=_BV(RED_BIT)
#define GREEN_OFF GREEN_PORT&=~_BV(GREEN_BIT)
#define GREEN_ON GREEN_PORT|=_BV(GREEN_BIT)

// this combined with timer 1 count provides the 32 bit main clock
extern volatile uint16_t mainclock;

#define BUFMAX 40

// general scratch pad for strings etc
extern char buf[BUFMAX];

#endif

