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
// #include <inttypes.h>
#include <avr/timer.h>
#include <avr/interrupt.h>
#include <avr/signal.h>

#ifndef CAPTURE
#define CAPTURE

void capture_reset();
void capture_print();
void capture_mode(uint8_t mode);
void capture(uint16_t val,uint16_t v2);
//void capture(uint16_t val);
inline void capture1();
inline void capture0();

#endif

