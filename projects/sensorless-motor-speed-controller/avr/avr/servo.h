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
#include <stdlib.h>
#include <math.h>
#include <avr/timer.h>
#include <avr/interrupt.h>
#include <avr/signal.h>

#ifndef SERVO
#define SERVO

/*
typedef struct servo_{
      float pos;
      float rate;
      float endpos;
};
*/

typedef struct servo_{
      uint16_t pos;   // pulse width servo position bits
      int16_t rate;      // rate is in RESOLUTION units per servo pulse generated
      uint16_t endpos;
};
/*
   typedef struct servo_{
   uint16_t pos;  // from 0 to 1023 ( 10 bit resolution )
   int16_t rate;  // in pos per ms
   uint16_t endpos;
   };
*/
uint16_t step(struct servo_ *z,uint16_t dt);
extern struct servo_ servo[2];

#endif

