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
// infra red remote control signal sender
// used for transmission remote communications with other ir devices
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
#include "common.h"

#ifdef IRTX_CODE
// infra red tx unit timer port pin.
#define IRTX_DDR DDRB
#define IRTX_PORT PORTB
#define IRTX_BIT 0

#define CNT (CLOCK/(8*38000)

static uint8_t pnt=0;
static uint8_t bit=0;
static uint32_t lasttime=0;
static uint32_t data=0;

/* just show the timeing count we are using */
void irtx_print()
{
   puts(" cnt=");
   put_i16(CNT);
   put_cr();
   return;
}

void irtx_cmd(char **z)
{
   char *dummy;
   char y=**z;
   (*z)++;
   switch (y)
   {
      case 'p':
	 puts(" cnt=");
	 put_i16(CNT);
	 put_cr();
	 break;
      case 's':   // set final heating temperature
	 if('='==(**z)){
			  irtx_send(strtol((*z)+1,0,10));
			  break;
		       }
   }
}

void init_irtx()
{
   IRTX_DDR&=~_BV(IRTX_BIT);
   IRTX_PORT&=~_BV(IRTX_BIT);

   TCCR0=T0_CTC0+T0_CK1;
   //TIMSK|=_BV(TOIE0);  // timer 0 overflow interrupts  enable
   TIMSK|=_BV(OCIE0);  // timer 0 output compare interrupt  enable
}

/* send a signal cannon brand equipment style
   data usually is 32 bits first 16 bits are the 
   equipment identifier */
irtx_send(uint32 data)
{

}

// send clocking and read pulses to the irdds and get back data
// expect need ~30 cycles to complete this routine
SIGNAL (SIG_OVERFLOW0)   //void interrupt_loop()
{
   return;
}

const uint8_t us10000=(10000/1024)*MCLOCK;
SIGNAL (SIG_OUTPUT_COMPARE0)
{
   static uint8_t count=0;
   static uint8_t num=0;  // count number of 38khz transitions
   irstate++;
   if(irstate>num){
/*   get next bit of transmission*/
		     if(data & 1){
				    TCCR0=T0_CTCT+T0_CK8;
				    OCR0=60;

				 }else
		     {
			TCCR0=T0_CTC0+T0_CK8;
		     }

		     data>>=1;

		     return;
		  }
}
#endif

