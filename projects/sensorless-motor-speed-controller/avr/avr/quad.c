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
// quaderature encoder driver
// uses one external interrupt INT1
// and one other arbitary input pin
// for the A and B inputs
// to attach several quaderature encoders make modified versions of this file
// by using the INT0 and INT2 and analogue comparator inputs
// if it were required
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

#ifdef QUAD_CODE

// we use 2 inputs for each quaderature encoder
// one is an external interrupt
// external interrupt 1

//port pin defines for the quadrature encoder used for welding control system
#ifdef WELDING 
#define QI_BIT 2
#define QI_DDR DDRD
#define QI_PIN PIND
#define QI_PORT PORTD

#define QR_BIT 4
#define QR_DDR DDRD
#define QR_PIN PIND
#define QR_PORT PORTD

#define QI_INT 0
#endif

//port pin defines for the quadrature encoder used for timer - thermostat 
#ifdef TIMER 
#define QI_BIT 2
#define QI_DDR DDRD
#define QI_PIN PIND
#define QI_PORT PORTD

#define QR_BIT 4
#define QR_DDR DDRD
#define QR_PIN PIND
#define QR_PORT PORTD

#define QI_INT 0
#endif

#ifdef OTHERS
#define QI_BIT 3
#define QI_DDR DDRD
#define QI_PIN PIND
#define QI_PORT PORTD

#define QR_BIT 4
#define QR_DDR DDRD
#define QR_PIN PIND
#define QR_PORT PORTD

#define QI_INT 1
#endif

unsigned short quad;
void quad_print()
{
/*
      equal("quad");
      put_i16(quad);
      put_cr();
      return;
*/
}

void init_quad()
{
   QI_DDR&=~_BV(QI_BIT);
   QI_PORT&=~_BV(QI_BIT);
   QR_DDR&=~_BV(QR_BIT);
   QR_PORT&=~_BV(QR_BIT);
#if QI_INT==0
   MCUCR|=_BV(ISC00);  // enable interrupt on transition of int1 input
   GICR|=_BV(INT0);  //
#elif  QI_INT==1
   MCUCR|=_BV(ISC10);  // enable interrupt on transition of int1 input
   GICR|=_BV(INT1);  //
#endif
   quad=0;
}

// quaderature input device up/down count interrupt
// we need one input to be an interrupt thewe other
// just needs to be an ordinary input pin
static inline void quadct()
{
   if((bit_is_set(QI_PIN,QI_BIT)==0)^ ( bit_is_set(QR_PIN,QR_BIT)==0))
   {
      quad++;
   } else {
	     quad--;
	  }
   //   if(quad>(1<<15))quad=1<<15;
   //   if(quad<0)quad=0;
}

#if QI_INT==0
SIGNAL (SIG_INTERRUPT0)
{
   quadct();
}
#elif  QI_INT==1
SIGNAL (SIG_INTERRUPT1)
{
   quadct();
}
#endif

#endif

