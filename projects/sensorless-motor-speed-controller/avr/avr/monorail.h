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

#ifndef MONORAIL_H
#define MONORAIL_H

#define RED_PORT PORTB
#define RED_DDR DDRB
#define RED_PIN PINB
#define RED_BIT 0

#define GREEN_PORT PORTB
#define GREEN_DDR DDRB
#define GREEN_PIN PINB
#define GREEN_BIT 2

#define RED_OFF RED_PORT&=~_BV(RED_BIT)
#define RED_ON RED_PORT|=_BV(RED_BIT)

#define GREEN_OFF GREEN_PORT&=~_BV(GREEN_BIT)
#define GREEN_ON GREEN_PORT|=_BV(GREEN_BIT)

// this combined with timer 1 count provides the 32 bit main clock
extern volatile uint16_t mainclock;

// #define BUFMAX 20
// general scratch pad for strings etc
// extern char buf[BUFMAX];

// pin 36 is portA4
#define TRACK_START bit_is_clear(PINA,7)
// pin 35 is portA5
#define TRACK_END bit_is_clear(PINA,6)
//// pin 17 is portD3
//#define ESTOP bit_is_set(PIND,3)
// pin 34 is portA6
#define ESTOP bit_is_clear(PINB,0)
// pin 33 is portA7
#define TESTBUTTON bit_is_clear(PINB,1)


typedef struct drive{
      volatile uint32_t stop_time;  // in us
      volatile uint32_t utime;    // in us (basically the overflow of the u clock
      volatile uint8_t ramp_rate;  // 804c3e0er second
      volatile uint32_t timeout; // in us _timer varable. =0
      volatile long quad_set;  //quadrature
      volatile uint32_t quad_count;   // =0
      volatile uint32_t top_rate;   // =0
      volatile uint16_t velocity;  // main clock counts between rising interrupt events

      volatile float battery_low;  // level. =11volts
      volatile float battery_high;   //9 battery high level. =14 volts
/*
      volatile uint16_t battery_low;  // level. =11volts
      volatile uint16_t battery;  // level. =11volts
      volatile uint16_t battery_high;   //9 battery high level. =14 volts
*/
      volatile uint8_t top_speed;  //  =100% (pwm ratio
      volatile int8_t dir;
      // volatile uint8_t state;
      volatile enum {  TRANSIT,STOPPED,RAMP,STOPPING  }state;
#define FLAGS EEDR
#define TIMEOUT 1
#define QCOUNT 2
#define TESTMODE 3
#define S_FORWARD 4
#define S_REVERSE 5
      //      uint8_t flags;
      //	enum flags{ TIMEOUT, QCOUNT, TESTMODE, S_FORWARD,S_REVERSE};

};

void rail_print();
void rail_interrupt();

void startup();

#ifndef MYDRIVE
extern struct drive mydrive;
#define MYDRIVE
#endif

#endif

