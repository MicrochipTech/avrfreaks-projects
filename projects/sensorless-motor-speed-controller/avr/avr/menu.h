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

#ifndef MENU_H
#define MENU_H

enum { MENU_SEL,MENU_ADD,MENU_BELOW };

// select button.
#define SELECT_DDR DDRA
#define SELECT_PIN PINA
#define SELECT_PORT PORTA
#define SELECT_BIT 2
#define SELECT bit_is_clear(SELECT_PIN,SELECT_BIT)

extern int8_t cur;  // current selection state
extern int8_t xpos;

#endif

