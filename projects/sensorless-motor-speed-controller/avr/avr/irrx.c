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
// infra red remote control receiver driver
// uses interrupt pin INT2 to read in transition data
// from the receiver
// there is functionallity available to use these routines as a generic pulse pattern
// recorder should it be enabled.
// can now control monorail with a cannon remote control
// can easly be changed to suit other brands and types
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

#ifdef IRRX_CODE
// infra red receiver unit port pin.
#define IRRX_DDR DDRB
#define IRRX_PORT PORTB
#define IRRX_BIT 2

#define irmax 70
uint8_t rxval[5];
static uint8_t pnt=0;
static uint8_t bit=0;
static uint32_t lasttime=0;

void irrx_print()
{
   uint8_t x;
   puts("irrx data=");
   //  capture_print();
   for(x=0;x<5;x++){
		      put_i16(rxval[x]);
		      putc(',');
		   }
   return;
}

void init_irrx()
{
   IRRX_DDR&=~_BV(IRRX_BIT);
   IRRX_PORT&=~_BV(IRRX_BIT);
   MCUCR|=_BV(ISC2);  // enable interrupt on transition of int1 input
   GICR|=_BV(INT2);  //
}

// infra red data pulse edge received interrupt
SIGNAL (SIG_INTERRUPT2)
{
   uint8_t x,y,z=0;
   uint32_t time=TCNT1+(((uint32_t)mainclock)<<16);
   uint16_t dt=(time-lasttime)/MCLOCK;    // we now have our pulse width in us
   // dt/=100;   // scale down the width
   if(dt>4000){  pnt=0;bit=0;  }  // reset
   lasttime=time;
   GIFR|=_BV(INTF2);  // clear int 2 interrupt
   if(bit_is_set(MCUCSR,ISC2))
   {  // negative going edge received
      // capture(-dt);
      MCUCSR&=~_BV(ISC2);
   }else {
	    //	    capture(dt);
	    MCUCSR|=_BV(ISC2);
	    if( bit<32 && pnt>2 )// decoding for pre pulse >40ms
	    {
	       if(dt<1000)
		  //	       {  rxval&=~(1<<bit));
	       //	       }else{  rxval|=(1<<bit);
	       //		    }
	       {
		  rxval[bit>>3]&=~(1<<(bit&7));
	       }else{
		       rxval[bit>>3]|=(1<<(bit&7));
		    }
	       bit++;
	    }
	 }

   pnt++;
   if( pnt >= irmax ) pnt=irmax;
   // transmission completed now lets controll something
   // setup for cannon wireless remote controller WL-D77 for a handycam
   if( bit >= 31)
   {
      switch(rxval[3])
      {
	 // operate the monorail with a remote controll unit
#ifdef MONORAIL
	 case 229:  // rewind
	    break;
	 case 228:   // ffwd
	    break;
	 case 244:  // stop
	    stop();
	    break;
	 case 255:  // play
	    break;
	 case 253:  // pause
	 default:
	    stop();
#endif
	    break;
      }
   }
}
#endif

