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
// infra red digital distance sensor driver
// routines to interface to a sharp GP2D02
// this is the output compare verison it uses either timer 0 or timer 2 output compare
// to produce the irdds control-clock signal this method eliminates clock jitter and
// reduces the number of interrupts required.
// using 1 control pin OC0 hich is attached to an open drain driver fet arrangement
// any number of arbitary input pins can be used allowing any number of irdds
// devices
// written by steve humble 10/11/2003

#include <avr/io.h>
#include ATMEGA
#include <inttypes.h>
#include <stdlib.h>
#include <stdarg.h>
#include <math.h>
#include <avr/timer.h>
#include <avr/interrupt.h>
#include <avr/signal.h>
#include "fail.h"
#include "string.h"
#include "monorail.h"

#include "irdds.h"
#include "command.h"
#include "servo.h"
#include "avr.h"

#ifdef IRDDS_CODE
// pin 4 OC0
#define IRDDSVINPORT PORTB
#define IRDDSVINPIN PINB
#define IRDDSVINDDR DDRB
#define IRDDSVINBIT 3

// pin 1
#define IRDDS1PORT PORTB
#define IRDDS1PIN PINB
#define IRDDS1DDR DDRB
#define IRDDS1BIT 0

uint8_t irddsv[2];

char irstate=-2;

// send clocking and read pulses to the irdds and get back data
// expect need ~30 cycles to complete this routine
SIGNAL (SIG_OVERFLOW0)   //void interrupt_loop()
{
   return;
}
const uint8_t us10000=(10000/1024)*MCLOCK;
SIGNAL (SIG_OUTPUT_COMPARE0)
{
   //capture(irstate);
   static uint8_t count=0;
   switch(irstate)
   {
      case -2:   // setup to produce a 70ms wide negative pulse
#ifdef MONORAIL
	 rail_interrupt();
#endif
	 // GONEG;
	 TCCR0=T0_CTC1+T0_CK1024;
	 count=8;
	 OCR0=us10000;
	 irstate++;
      case -1:
	 count--;
	 // in this version we use the 1024 clock divider timer set to overflow at with the 256000
	 if(count==0){
			// setup to produce 130us pulse width
			irstate++;
			OCR0=20;
			TCCR0=T0_CTCT+T0_CK64;
		     }
	 break;
	 //				  irstate++;
	 //				  GOPLUS;
	 //	 break;
      case 0:   // wait for data line to go high
      case 2:
      case 4:
      case 6:
      case 8:
      case 10:
      case 12:
      case 14:
	 irstate++;
	 //GOPLUS;
	 break;
      case 1:
      case 3:
      case 5:
      case 7:
      case 9:
      case 11:
      case 13:
      case 15:
	 irstate++;  // just count which pulse we are up to
	 irddsv[0]=(irddsv[0]<<1);  // we loose the first of the 9 samples
	 if(bit_is_set(IRDDS1PIN,IRDDS1BIT))irddsv[0]|=1;
	 break;
      case 16:
	 TCCR0=T0_CTCT+T0_CK256;
	 OCR0=1600*MCLOCK/256;
	 irddsv[1]=irddsv[0];
	 irstate=-2;
	 break;
      case 18:
	 irstate++;
	 //TIMSK&=~_BV(TOIE0);  // timer 0 overflow interrupts  disable
	 break;  // at end of conversion we just stop
      default:
	 irstate=-2;
	 break;
   }
   return;
}

void irdds_print()
{
   puts("ird=");
   put_i16(irddsv[1]);
   puts("irsta=");
   put_i16(irstate);
   put_cr();
   return;
}

void   init_irdds()
{
   IRDDS1PORT&=~_BV(IRDDS1BIT);
   IRDDSVINPORT&=~_BV(IRDDSVINBIT);  // use no pullup resistor
   IRDDSVINDDR |= _BV(IRDDSVINBIT);
   IRDDS1DDR&=~_BV(IRDDS1BIT);
   TCCR0=T0_CTC0+T0_CK1;
   //TIMSK|=_BV(TOIE0);  // timer 0 overflow interrupts  enable
   TIMSK|=_BV(OCIE0);  // timer 0 output compare interrupt  enable
}
#endif

