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
// pulse count and period - frequency measurment driver
// uses one external interrupt INT0
// provides accurate frequency over the range from 5hz to 65khz
//
// the motor system needs feedback every 20th to 50th of a second
// when the pulse feedback count is under 5 counts then pulse width is
// switched on to provide accuracy.
#include <avr/io.h>
#include ATMEGA
#include <inttypes.h>
#include <math.h>
#include <avr/timer.h>
#include <avr/interrupt.h>
#include <avr/signal.h>
#include "adc.h"
#include "uart.h"
#include "fail.h"
#include "string.h"

#include "monorail.h"
#include "capture.h"
#include "servo.h"

#ifdef PULSE_CODE

// we use 1 input for pulse counter
// one is an external interrupt
// external interrupt 0
#define PC_BIT 2
#define PC_DDR DDRD
#define PC_PIN PIND
#define PC_PORT PORTD
// which interrupt is for the above pin only 0 or 1 will work presently
#define PC_INT 1

uint16_t pulse_count;

unsigned short quad;
void pulse_print()
{
   equal("pulse");
   put_i16(pulse_count);
   put_cr();
   return;
}

void init_pulse()
{
   pulse_count=0;
   PC_DDR&=~_BV(PC_BIT);
   PC_PORT=_BV(PC_BIT);  // enable internal pullup resistor
#if PC_INT==0
   MCUCR|=_BV(ISC00);  // enable interrupt on transition of int0 input
   GICR|=_BV(INT0);
}
#elif PC_INT==1
   MCUCR|=_BV(ISC10);  // enable interrupt on transition of int1 input
   GICR|=_BV(INT1);  
#endif
}

#define MAXBITS 9
// increment number of counts and the time of the last count
// this keeps processing and registers to a minimum
static uint16_t pulse_time=0;
static uint16_t pulse_time1=0;
static uint16_t pulse_time2=0;
static inline void pulse_int()
{
   pulse_time1=mainclock;   // just record the clock value we calculate the actual time value later on
   pulse_time2=TCNT1;
   //pulse_time=TCNT1+(((uint32_t)mainclock)<<16);  // record the time of last pulse
   pulse_count++;   // increment count
   return;
}

#if PC_INT==0
SIGNAL (SIG_INTERRUPT0)
{
   pulse_int();
}
#elif  PC_INT==1
SIGNAL (SIG_INTERRUPT1)
{
   pulse_int();
}
#endif

/*
   static uint16_t pulse_time=0;
   oldver (SIG_INTERRUPT0)
   {
   pulse_time=(mainclock<<8)+(TCNT1>>8);  // record the time of last pulse to an accuracy of 16us
   //pulse_time=TCNT1+(((uint32_t)mainclock)<<16);  // record the time of last pulse
   pulse_count++;   // increment count
   return;
   }
*/
// routine to interpret and read back the average pulses per second since
// it was last called
// will not work for pulse rates below a set limit of 5Hz.
// the routine utilises pulse width and pulse count to provide a considerabley
// more accurate reading that could be obtained with a simple pulse count.
// so accuracy is maintained from for high frequency pulses
// or for very slow pulses
// at low pulse rates where the readback speed is higher than the pulse rate
// the previous sample time is retained so that an
// accurate
// pulse rate is still obtained for lower than  readback speed pulse frequencys
uint32_t pulse_read()
{
   static uint16_t pulse_ltime=0;
   uint16_t dt,idt=0;
   // one or more pulses recorded is enough to provide a completely accurate sample
   if(pulse_count>=1)
   {
      dt=(pulse_time-pulse_ltime);  // get total measurment period in 16us units since last called
      idt=(1000000/16)/dt;   // get 1/(sample period)  ie for 100ms = 10
      idt*=pulse_count;
      pulse_count=0;
      if(pulse_ltime==0){  // check for an unwanted time overflow condition
			   pulse_ltime=pulse_time;  // in case of a sudden slowdown remember last transition time
			   return(0);  // special condition for when
			}
      pulse_ltime=pulse_time;  // in case of a sudden slowdown remember last transition time
      return(idt);
   }

   // very slow or stopped
   // can see when last pulse was recorded and assume the next pulse is now
   // if last pulse was to long ago our frequency will be zero
   if(pulse_ltime==0) return(0);  // special condition for when
   if(pulse_count=0)
   {
      dt=(mainclock<<8)+(TCNT1>>8)-pulse_ltime; // get total measurment period in 16us units since last called
      if(dt>20000) {
		      pulse_ltime=0;
		      return(0);
		   }
      //     get total time since last real pulse
      dt=(pulse_time-pulse_ltime);  // use previous sample times
      idt=(1000000/16)/dt;   // get 1/(sample period)  ie for 100ms = 10
      if(idt<5) idt=0;  // we really must have stopped
      return(idt);
   }
   return 0;
}
#endif

