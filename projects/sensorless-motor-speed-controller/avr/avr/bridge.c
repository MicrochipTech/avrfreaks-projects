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
// avr microcontroller code
// bridge code using timer2 interrupts to control
// the bridge switch timing.
// we use timer counter 2 as the switching clock and set the divider
// to 8 to provide 7.2khz with a 14.7856Mhz clock, 7.8khz for a 16Mhz clock
// 9 khz for a 18.4Mhz clock ,or 10 khz for a 22.118400 Mhz clock
//
// the adc operations are handled by the ADC complete interrupt.
// to each bridge in turn to provide either current and voltage samples
// once a bridge has obtained valid currant and voltages this data can
// provide a feedback operation to increase or decrease the PWM ratio
// the motor voltage samples are not required if alternative feedback is used instead.
//
// current and voltage feedback sensing pulse width bridge drive routines with
// overcurrent cutback
// can control 2 bridge drivers with either side prefered switching.
// switching includes wait states to allow for a slower high side switches
// should be easy to adapt to various bridge or step driver ics like 297
// warning full software controlled bridge -any bug may result in a circuitry overload and
// possible failure or damage to components
//
// drive patterns - voltage feedback model - user selects motor generator action voltage level
// which should be linear and directly proportional to our speed -
//  small DC motor from spindle mechanisim  I=3.4mh R=12.0ohms
//  small DC motor from eject mechanisim  I=19.5mh R=16.4ohms
//  grauphner SPEED600 - small high power model RC motor  I=0.04mh R=0.3ohms
//
// time constant calculation for grauphner is t=r/I = .340*.00004= t=cr
// for small motor t =
// ok so on startup we do the following run gradually increasing period pulses
// to plot the dI to profile our motor characteristics and impedance.
// Thus we can use this to provide pulse width current limiting information for
// our motors later on either manually or automatically.

#ifdef MONORAIL

#include <sig-avr.h>
#include <avr/io.h>
#include ATMEGA
#include <interrupt.h>
#include <inttypes.h>
#include "bridge.h"
#include "monorail.h"

#define IVMAX	220
// this is the overcurrent cutout setting
// very important when running low Z motors
// the adc samples are using 2.5v reference
// the 10amp .077ohms setting is 300

#define SENSORLESS
//include sensorless voltage-current based speed control

#include <avr/io.h>
#include ATMEGA
#include <inttypes.h>
#include <math.h>
#include <avr/timer.h>
#include <avr/interrupt.h>
#include <avr/signal.h>
#include "fail.h"
#include "string.h"
#include "monorail.h"
#include "avr.h"
#include "adc.h"

// enable this module here
//#define BRIDGEC

// pin 5 is high side voltage pump
#define BRVP_PORT PORTB
#define BRVP_DDR DDRB
#define BRVP_PIN PINB
#define BRVP_BIT 4

// forward direction is defined as the direction where pin 1 of the motor is
// ground and pin 2 is positive

// BRIDGE Q CONSISTS OF
// pin 22
#define BRQHI1_PORT PORTC
#define BRQHI1_DDR DDRC
#define BRQHI1_PIN PINC
#define BRQHI1_BIT 0

#define BRQLO1_PORT PORTC
#define BRQLO1_DDR DDRC
#define BRQLO1_PIN PINC
#define BRQLO1_BIT 1

#define BRQHI2_PORT PORTC
#define BRQHI2_DDR DDRC
#define BRQHI2_PIN PINC
#define BRQHI2_BIT 3

#define BRQLO2_PORT PORTC
#define BRQLO2_DDR DDRC
#define BRQLO2_PIN PINC
#define BRQLO2_BIT 2

#define BRQ_PORT PORTC

// Q bridge operation set and clear patterns
// Q bridge macros for controlling indivdual bridge elements
#define BRQHI1_OFF_or  (_BV(BRQHI1_BIT))
#define BRQHI1_OFF_and 255
#define BRQHI1_ON_or   0
#define BRQHI1_ON_and  (~_BV(BRQHI1_BIT))

#define BRQHI2_OFF_or  (_BV(BRQHI2_BIT))
#define BRQHI2_OFF_and 255
#define BRQHI2_ON_or   0
#define BRQHI2_ON_and  (~_BV(BRQHI2_BIT))

#define BRQLO1_OFF_or  (_BV(BRQLO1_BIT))
#define BRQLO1_OFF_and 255
#define BRQLO1_ON_or   0
#define BRQLO1_ON_and  (~_BV(BRQLO1_BIT))

#define BRQLO2_OFF_or  (_BV(BRQLO2_BIT))
#define BRQLO2_OFF_and 255
#define BRQLO2_ON_or   0
#define BRQLO2_ON_and  (~_BV(BRQLO2_BIT))

// Q bridge macros for controlling each half bridge hi-low pair
#define BRQ1_OPEN_or (BRQLO1_OFF_or | BRQHI1_OFF_or)
#define BRQ1_OPEN_and (BRQLO1_OFF_and & BRQHI1_OFF_and)

#define BRQ1_GND_or (BRQLO1_ON_or | BRQHI1_OFF_or)
#define BRQ1_GND_and (BRQLO1_ON_and & BRQHI1_OFF_and)

#define BRQ1_VSS_or (BRQLO1_OFF_or | BRQHI1_ON_or)
#define BRQ1_VSS_and (BRQLO1_OFF_and & BRQHI1_ON_and)

#define BRQ2_OPEN_or (BRQLO2_OFF_or | BRQHI2_OFF_or)
#define BRQ2_OPEN_and (BRQLO2_OFF_and & BRQHI2_OFF_and)

#define BRQ2_GND_or (BRQLO2_ON_or | BRQHI2_OFF_or)
#define BRQ2_GND_and (BRQLO2_ON_and & BRQHI2_OFF_and)

#define BRQ2_VSS_or (BRQLO2_OFF_or | BRQHI2_ON_or)
#define BRQ2_VSS_and (BRQLO2_OFF_and & BRQHI2_ON_and)

// Q bridge macros for controlling all bridge elements
// theses are logically organised into the motor operations required

// REVERSE
#define BRQ_REV1_or (BRQ1_VSS_or |  BRQ2_GND_or)
#define BRQ_REV1_and (BRQ1_VSS_and &  BRQ2_GND_and)

#define BRQ_REV0_or (BRQ1_VSS_or |  BRQ2_OPEN_or)
#define BRQ_REV0_and (BRQ1_VSS_and &  BRQ2_OPEN_and)

// breaking
#define BRQ_REVSTOP1_or (BRQ1_GND_or |  BRQ2_GND_or)
#define BRQ_REVSTOP1_and (BRQ1_GND_and &  BRQ2_GND_and)

#define BRQ_REVSTOP0_or (BRQ1_GND_or |  BRQ2_OPEN_or)
#define BRQ_REVSTOP0_and (BRQ1_GND_and &  BRQ2_OPEN_and)

// stopped, coasting or transitional state
#define BRQ_OFF_or (BRQ1_OPEN_or |  BRQ2_OPEN_or)
#define BRQ_OFF_and (BRQ1_OPEN_and &  BRQ2_OPEN_and)

// high voltage check (used to check and calibrate ADC ports )

#define BRQ_HIGH_or (BRQ1_VSS_or |  BRQ2_VSS_or)
#define BRQ_HIGH_and (BRQ1_VSS_and &  BRQ2_VSS_and)

#define BRQ_LOW_or (BRQ1_GND_or |  BRQ2_GND_or)
#define BRQ_LOW_and (BRQ1_GND_and &  BRQ2_GND_and)

// FORWARD
#define BRQ_FWD1_or (BRQ2_VSS_or |  BRQ1_GND_or)
#define BRQ_FWD1_and (BRQ2_VSS_and &  BRQ1_GND_and)

#define BRQ_FWD0_or (BRQ2_VSS_or |  BRQ1_OPEN_or)
#define BRQ_FWD0_and (BRQ2_VSS_and &  BRQ1_OPEN_and)

// breaking
#define BRQ_FWDSTOP1_or (BRQ2_GND_or |  BRQ1_GND_or)
#define BRQ_FWDSTOP1_and (BRQ2_GND_and &  BRQ1_GND_and)

#define BRQ_FWDSTOP0_or (BRQ2_GND_or |  BRQ1_OPEN_or)
#define BRQ_FWDSTOP0_and (BRQ2_GND_and &  BRQ1_OPEN_and)

// go forward
#define BRQ_FWD1 	{    \
   BRQ1_VSS; \
   BRQ2_GND;\
   }
#define BRQ_FWD0 	{    \
   BRQ1_VSS; \
   BRQ2_OPEN;\
   }

// forward breaking
#define BRQ_FWDSTOP1 	{	 \
   BRQ1_GND; \
   BRQ2_GND;\
   }
#define BRQ_FWDSTOP0 	{	 \
   BRQ1_GND; \
   BRQ2_OPEN;\
   }

// stopped, coasting or transitional state
#define BRQ_OFF 	{ \
   BRQ1_OPEN; \
   BRQ2_OPEN;\
   }

// high voltage check (used to check and calibrate ADC ports )
#define BRQ_HIGH 	{	 \
   BRQ1_VSS;\
   BRQ2_VSS;\
   }

// go reverse
#define BRQ_REV1 	{ \
   BRQ1_GND; \
   BRQ2_VSS;\
   }
#define BRQ_REV0 	{ \
   BRQ1_OPEN; \
   BRQ2_VSS;\
   }

// reverse breaking
#define BRQ_REVSTOP0 	{	 \
   BRQ1_GND; \
   BRQ2_VSS; \
   }
#define BRQ_REVSTOP1 	{	 \
   BRQ1_GND; \
   BRQ2_GND; \
   }


// Q bridge macros for controlling indivdual bridge elements
#define BRQHI1_OFF  BRQ_PORT|=_BV(BRQHI1_BIT)
#define BRQHI1_ON  BRQ_PORT&=~_BV(BRQHI1_BIT)

#define BRQHI2_OFF  BRQ_PORT|=_BV(BRQHI2_BIT)
#define BRQHI2_ON  BRQ_PORT&=~_BV(BRQHI2_BIT)

#define BRQLO1_ON  BRQ_PORT&=~_BV(BRQLO1_BIT)
#define BRQLO1_OFF  BRQ_PORT|=_BV(BRQLO1_BIT)

#define BRQLO2_ON  BRQ_PORT&=~_BV(BRQLO2_BIT)
#define BRQLO2_OFF  BRQ_PORT=_BV(BRQLO2_BIT)

// Q bridge macros for controlling each half bridge hi-low pair
#define BRQ1_OPEN 	{ \
   BRQLO1_OFF; \
   BRQHI1_OFF;\
   }

#define BRQ1_GND 	{    \
   BRQLO1_ON; \
   BRQHI1_OFF;\
   }

#define BRQ1_VSS 	{    \
   BRQLO1_OFF; \
   BRQHI1_ON;\
   }

#define BRQ2_OPEN 	{ \
   BRQLO2_OFF; \
   BRQHI2_OFF;\
   }

#define BRQ2_GND 	{    \
   BRQLO2_ON; \
   BRQHI2_OFF;\
   }

#define BRQ2_VSS 	{    \
   BRQLO2_ON; \
   BRQHI2_OFF;\
   }


// BRIDGE U CONSISTS OF the following ports
#define BRUHI1_PORT PORTC
#define BRUHI1_DDR DDRC
#define BRUHI1_PIN PINC
#define BRUHI1_BIT 4

#define BRULO1_PORT PORTC
#define BRULO1_DDR DDRC
#define BRULO1_PIN PINC
#define BRULO1_BIT 5

#define BRUHI2_PORT PORTC
#define BRUHI2_DDR DDRC
#define BRUHI2_PIN PINC
#define BRUHI2_BIT 7

#define BRULO2_PORT PORTC
#define BRULO2_DDR DDRC
#define BRULO2_PIN PINC
#define BRULO2_BIT 6

// U bridge macros for controlling indivdual bridge elements
#define BRUHI2_ON   BRUHI2_PORT&=~_BV(BRUHI2_BIT)
#define BRUHI2_OFF   BRUHI2_PORT|=_BV(BRUHI2_BIT)
#define BRUHI1_ON   BRUHI1_PORT&=~_BV(BRUHI1_BIT)
#define BRUHI1_OFF   BRUHI1_PORT|=_BV(BRUHI1_BIT)

#define BRULO2_ON   BRULO2_PORT&=~_BV(BRULO2_BIT)
#define BRULO2_OFF   BRULO2_PORT|=_BV(BRULO2_BIT)
#define BRULO1_ON   BRULO1_PORT&=~_BV(BRULO1_BIT)
#define BRULO1_OFF   BRULO1_PORT|=_BV(BRULO1_BIT)

// U bridge macros for controlling each half bridge hi-low pair
#define BRU1_OPEN 	{ \
   BRULO1_OFF; \
   BRUHI1_OFF;\
   }

#define BRU1_GND 	{    \
   BRULO1_ON; \
   BRUHI1_OFF;\
   }

#define BRU1_VSS 	{    \
   BRULO1_OFF; \
   BRUHI1_ON;\
   }

#define BRU2_OPEN 	{ \
   BRULO2_OFF; \
   BRUHI2_OFF;\
   }

#define BRU2_GND 	{    \
   BRULO2_ON; \
   BRUHI2_OFF;\
   }

#define BRU2_VSS 	{    \
   BRULO2_ON; \
   BRUHI2_OFF;\
   }

// U bridge macros for controlling all bridge elements
// theses are logically organised into the motor operations required
// they all operate in 4 cycles on the avr cpu

// go forward
#define BRUFWD1 	{    \
   BRU1_VSS; \
   BRU2_GND;\
   }
#define BRUFWD0 	{    \
   BRU1_VSS; \
   BRU2_OPEN;\
   }

// forward breaking
#define BRUFWDSTOP1 	{	 \
   BRU1_GND; \
   BRU2_GND;\
   }
#define BRUFWDSTOP0 	{	 \
   BRU1_GND; \
   BRU2_OPEN;\
   }

// stopped, coasting or transitional state
#define BRU_OFF 	{ \
   BRU1_OPEN; \
   BRU2_OPEN;\
   }

// high voltage check (used to check and calibrate ADC ports )
#define BRU_HIGH 	{	 \
   BRU1_VSS;\
   BRU2_VSS;\
   }

// go reverse
#define BRUREV1 	{ \
   BRU1_GND; \
   BRU2_VSS;\
   }
#define BRUREV0 	{ \
   BRU1_OPEN; \
   BRU2_VSS;\
   }

// reverse breaking
#define BRUREVSTOP0 	{	 \
   BRU1_GND; \
   BRU2_VSS; \
   }
#define BRUREVSTOP1 	{	 \
   BRU1_GND; \
   BRU2_GND; \
   }

// U bridge operation set and clear patterns
// U bridge macros for controlling indivdual bridge elements
#define BRUHI1_OFF_or  (_BV(BRUHI1_BIT))
#define BRUHI1_OFF_and 255
#define BRUHI1_ON_or   0
#define BRUHI1_ON_and  (~_BV(BRUHI1_BIT))

#define BRUHI2_OFF_or  (_BV(BRUHI2_BIT))
#define BRUHI2_OFF_and 255
#define BRUHI2_ON_or   0
#define BRUHI2_ON_and  (~_BV(BRUHI2_BIT))

#define BRULO1_OFF_or  (_BV(BRULO1_BIT))
#define BRULO1_OFF_and 255
#define BRULO1_ON_or   0
#define BRULO1_ON_and  (~_BV(BRULO1_BIT))

#define BRULO2_OFF_or  (_BV(BRULO2_BIT))
#define BRULO2_OFF_and 255
#define BRULO2_ON_or   0
#define BRULO2_ON_and  (~_BV(BRULO2_BIT))

// U bridge macros for controlling each half bridge hi-low pair
#define BRU1_OPEN_or (BRULO1_OFF_or | BRUHI1_OFF_or)
#define BRU1_OPEN_and (BRULO1_OFF_and & BRUHI1_OFF_and)

#define BRU1_GND_or (BRULO1_ON_or | BRUHI1_OFF_or)
#define BRU1_GND_and (BRULO1_ON_and & BRUHI1_OFF_and)

#define BRU1_VSS_or (BRULO1_OFF_or | BRUHI1_ON_or)
#define BRU1_VSS_and (BRULO1_OFF_and & BRUHI1_ON_and)

#define BRU2_OPEN_or (BRULO2_OFF_or | BRUHI2_OFF_or)
#define BRU2_OPEN_and (BRULO2_OFF_and & BRUHI2_OFF_and)

#define BRU2_GND_or (BRULO2_ON_or | BRUHI2_OFF_or)
#define BRU2_GND_and (BRULO2_ON_and & BRUHI2_OFF_and)

#define BRU2_VSS_or (BRULO2_OFF_or | BRUHI2_ON_or)
#define BRU2_VSS_and (BRULO2_OFF_and & BRUHI2_ON_and)

// U bridge macros for controlling all bridge elements
// theses are logically organised into the motor operations required

#define BRU_FWD1_or (BRU1_VSS_or |  BRU2_GND_or)
#define BRU_FWD1_and (BRU1_VSS_and &  BRU2_GND_and)

#define BRU_FWD0_or (BRU1_VSS_or |  BRU2_OPEN_or)
#define BRU_FWD0_and (BRU1_VSS_and &  BRU2_OPEN_and)

// forward breaking
#define BRU_FWDSTOP1_or (BRU1_GND_or |  BRU2_GND_or)
#define BRU_FWDSTOP1_and (BRU1_GND_and &  BRU2_GND_and)

#define BRU_FWDSTOP0_or (BRU1_GND_or |  BRU2_OPEN_or)
#define BRU_FWDSTOP0_and (BRU1_GND_and &  BRU2_OPEN_and)

// stopped, coasting or transitional state
#define BRU_OFF_or (BRU1_OPEN_or |  BRU2_OPEN_or)
#define BRU_OFF_and (BRU1_OPEN_and &  BRU2_OPEN_and)

// high voltage check (used to check and calibrate ADC ports )

#define BRU_HIGH_or (BRU1_VSS_or |  BRU2_VSS_or)
#define BRU_HIGH_and (BRU1_VSS_and &  BRU2_VSS_and)

#define BRU_LOW_or (BRU1_GND_or |  BRU2_GND_or)
#define BRU_LOW_and (BRU1_GND_and &  BRU2_GND_and)

// reverse
#define BRU_REV1_or (BRU2_VSS_or |  BRU1_GND_or)
#define BRU_REV1_and (BRU2_VSS_and &  BRU1_GND_and)

#define BRU_REV0_or (BRU2_VSS_or |  BRU1_OPEN_or)
#define BRU_REV0_and (BRU2_VSS_and &  BRU1_OPEN_and)

// reverse breaking
#define BRU_REVSTOP1_or (BRU2_GND_or |  BRU1_GND_or)
#define BRU_REVSTOP1_and (BRU2_GND_and &  BRU1_GND_and)

#define BRU_REVSTOP0_or (BRU2_GND_or |  BRU1_OPEN_or)
#define BRU_REVSTOP0_and (BRU2_GND_and &  BRU1_OPEN_and)

struct hb hbq;
struct hb hbu;


/* sw_tab switch table 
   this is a single linked table of bridge switching and
   its gets generated by bridge routines
   and the timer2 interrupt routine iterates through each
   entry when the timer2 matches the count.

   we allocate enough members to hold 3 sets (tripple buffering)
   of with upto 4 operations each.
   this allows for sequenced operations for queuing
   up wait stated type sequences which are performed
   only once, while normal operation sequences would
   loopback on themesleves in an endless loop
*/
struct sw_op{
/* link list next table entry
      last next will be a loopback to the start usually */
      struct sw_op *next;
      uint8_t count;  // timer compare interrupt count
      uint8_t or_bits;
      uint8_t and_bits;
      uint8_t bits;
};

#define SW_MAX 12
volatile struct sw_op sw_tab[SW_MAX];
struct sw_op *sw_pos;
char hookpoint;
char buffer;
// #define tpnt TWBR
volatile uint16_t lasttime;   // debugging only
volatile uint16_t difftime;   // debugging only
volatile short intct;

void init_bridge()
{
   struct hb *q=&hbq;
   struct hb *u=&hbu;
   q->dir=BR_OFF;
   u->dir=BR_OFF;
   q->set=0;
   u->set=0;
   buffer=0;
   hookpoint=0;
/* setup for first member in the switching table 
      is setup for all switches off
*/
   sw_pos=&sw_tab[0];
   sw_tab[0].next=&sw_tab[0];
   sw_tab[0].bits=0xff;
   sw_tab[0].count=33;
   sw_tab[0].or_bits=0xff;
   sw_tab[0].and_bits=0xff;
   BRQ_OFF(PORTC);
   BRVP_DDR|=_BV(BRVP_BIT);
   // initalise bit pattern
   BRQ_OFF;
   BRQHI1_DDR|=_BV(BRQHI1_BIT);
   BRQLO1_DDR|=_BV(BRQLO1_BIT);
   BRQHI2_DDR|=_BV(BRQHI2_BIT);
   BRQLO2_DDR|=_BV(BRQLO2_BIT);
   BRU_OFF;
   BRUHI1_DDR|=_BV(BRUHI1_BIT);
   BRULO1_DDR|=_BV(BRULO1_BIT);
   BRUHI2_DDR|=_BV(BRUHI2_BIT);
   BRULO2_DDR|=_BV(BRULO2_BIT);

   bridge_set(&hbq,0,BR_WAIT);
   bridge_set(&hbu,0,BR_WAIT);
   bridge_print();
   TCCR2= T2_CK8;  // normal mode no output
   // TCCR2= T2_CTC | T2_CK8 | _BV(COM20);
   TIMSK|=_BV(OCIE2);  // enable timer 2 compare interrupt enable
   // TIMSK|=_BV(TOIE2);  // enable timer 2 overflow interrupt enable
}

void bridge_print()
{
   puts(" dif=");   put_u32(difftime-lasttime);
   puts(" lst=");   put_u32(difftime);
   puts(" ocr2=");   put_i32(OCR2);
   puts(" cnt=");   put_i32(TCNT2);
   puts(" i_cnt=");   put_i32(intct);  put_cr();
   put_cr();

   putc('Q');
   bridge_show(&hbq);
   putc('U');
   bridge_show(&hbu);
   puts("icap ");
   put_cr();
   show_tab();
/*
      puts("interrupt types ");
      for(ct=0;ct<8;ct++){
      put_i16(int_list_mode[ct]);
      puts("\t");
      }
*/
   return;
}

void bridge_show(struct hb *q)
{
   puts(" sp=");   put_i32(q->set);
   puts(" dr=");   put_i32(q->dir);
   return;
}

void show_tab()
{
   puts("end=");
   put_i16(hookpoint);
   puts(" buf=");
   put_i16(buffer);
   puts(" now=");
   put_i16(sw_pos);
   put_cr();
   char check=0;
   char ct;
   for(ct=0;ct<SW_MAX;ct++)
   {
      if(check++>100)return;
      put_i16(ct);
      putc('\t');
      put_i16(&sw_tab[ct]);
      putc('[');
      put_i16((uint16_t)sw_tab[ct].count);
      putc('\t');
      put_bin(sw_tab[ct].or_bits);
      putc(',');
      put_bin(sw_tab[ct].and_bits);
      putc(',');
      put_bin(sw_tab[ct].bits);
      //   }
      puts("]\t>");
      put_i16(sw_tab[ct].next);
      put_cr();
      syncwait();
   }
   puts("order");
   put_cr();
   struct sw_op *pnt=&sw_tab[hookpoint];
   pnt=pnt->next;  // start from the beginning
   struct sw_op *start=pnt;  // start from the beginning
   do{
	put_i16(pnt);
	putc('[');
	putc('\t');
	put_i16(pnt->count);
	putc('\t');
	put_bin(pnt->or_bits);
	putc(',');
	put_bin(pnt->and_bits);
	putc(',');
	put_bin(pnt->bits);
	putc(']');
	if(sw_pos==pnt)putc('*');
	put_cr();
	pnt=pnt->next;
      syncwait();
	if(check++>100)return;
     } while(pnt!=start);
}

/* set adc time equivlant to the number of timer 2 ticks 
   which is equal to adc clock ticks as this is the
   sample period for the adc from the start of the trigger pulse 
   adc time is the number of counter ? cycles for setting up an adc 
   sample to the point in time where the sample and hold is completed
   a faster adc will allow a longer duty cycle to be done 
*/
#define ADC_TIME 16
#define START 0
#define MAXSPEED (250-16)

/* called to set motor speed & direction and coordinate state transitions
   the routine is synchronised with the motor PWM
   all changes from one major state to another result in
   insertion of a transitional wait state
*/
void bridge_set(struct hb *q,uint8_t speed,uint8_t dir)
{
   if(q->dir!=dir){
		     // a change of state is requested
		     q->dir=BR_WAIT;
		     q->dirnext=dir;
		  }
   if(speed>=MAXSPEED)speed=MAXSPEED;
   q->set=speed&0xf8;
   list_gen(); // generate a new switch pattern
}

enum {  Q_FWD1=1,Q_FWD0,Q_FWD_STOP0,Q_FWD_STOP1,
	   Q_REV1,Q_REV0,Q_REV_STOP1,Q_REV_STOP0,Q_OFF,
	   U_FWD1,U_FWD0,U_FWD_STOP0,U_FWD_STOP1,
	   U_REV1,U_REV0,U_REV_STOP1,U_REV_STOP0,U_OFF
     };

/*
   this routine prepares a series of q bridge operations to the sw_tab[]
   this table is used by the timer 2 interrupt to control q bridge switches.
   ADC conversions to support feedback are written to the adc_tab[] 
   and trigger adc samples.
   whenever there is a change in q or u drives parameters 
   this routine is called to generate the new switching pattern 
   adc operations are placed into a seperate adc request list
   return value will be 1 if there is a change in switching mode the 
   this indicates this routine must be called again as the switching pattern
   will change again.
*/
char gen(struct hb *q)
{
   char trans=0;
   struct sw_op *z;
   q->tail=q->head;
   struct adc_op *y;
   y=&adc_tab[adc_head];
   adc_tail=adc_head;
   z=&sw_tab[q->head];
   switch(q->dir)
   {
      case BR_FWD:
	 y->count=START; 	 //  place an adc request
	 y->opera=ADC_BRQ_1V;
	 y++;
	 adc_tail++;
	 if(q->set>0){
			z->count=ADC_TIME;
			z->or_bits=BRQ_FWD1_or;
			z->and_bits=BRQ_FWD1_and;
			z++;
			q->tail++;

			y->count=q->set;
			y->opera=ADC_BRQ_1I;
			y++;
			adc_tail++;
		     }else{
			     //qcurrent=0;
			  }
	 z->count=q->set+ADC_TIME;
	 z->or_bits=BRQ_FWD0_or;
	 z->and_bits=BRQ_FWD0_and;
	 z++;
	 q->tail++;
	 break;
      case BR_FWDSTOP:
	 //  place an adc request
	 y->count=START;
	 y->opera=ADC_BRQ_1V;
	 y++;
	 adc_tail++;
	 if(q->set>ADC_TIME){
			       z->count=ADC_TIME;
			       z->or_bits=BRQ_FWDSTOP1_or;
			       z->and_bits=BRQ_FWDSTOP1_and;
			       z++;
			       q->tail++;

			       y->count=q->set;
			       y->opera=ADC_BRQ_1I;
			       y++;
			       adc_tail++;
			    }else{
				    //qcurrent=0;
				 }
	 z->count=q->set+ADC_TIME;
	 z->or_bits=BRQ_FWDSTOP0_or;
	 z->and_bits=BRQ_FWDSTOP0_and;
	 z++; q->tail++;
	 break;

      case BR_REV:
	 //  place an adc request
	 y->count=START;
	 // allocate this ADC request a  number
	 y->opera=ADC_BRQ_2V;
	 y++; adc_tail++;
	 if(q->set>0){
			z->count=ADC_TIME;
			z->or_bits=BRQ_REV1_or;
			z->and_bits=BRQ_REV1_and;
			z++; q->tail++;

			y->count=q->set;
			y->opera=ADC_BRQ_2I;
			y++; adc_tail++;

		     }else{
			     //qcurrent=0;
			  }
	 z->count=q->set+ADC_TIME;
	 z->or_bits=BRQ_REV0_or;
	 z->and_bits=BRQ_REV0_and;
	 z++; q->tail++;
	 break;

      case BR_REVSTOP:
	 //  place an adc request
	 y->count=START;
	 // allocate this ADC request a  number
	 y->opera=ADC_BRQ_2V;
	 y++; adc_tail++;
	 if(q->set>ADC_TIME){
			       z->count=ADC_TIME;
			       z->or_bits=BRQ_REVSTOP1_or;
			       z->and_bits=BRQ_REVSTOP1_and;
			       z++; q->tail++;

			       y->count=q->set;

			       y->opera=ADC_BRQ_2I;
			       y++; adc_tail++;
			    }else{
				    //qcurrent=0;
				 }
	 z->count=q->set+ADC_TIME;
	 z->or_bits=BRQ_REVSTOP0_or;
	 z->and_bits=BRQ_REVSTOP0_and;
	 z++; q->tail++;
	 break;
      case BR_HIGH:
	 z->count=START;
	 z->or_bits=BRQ_HIGH_or;
	 z->and_bits=BRQ_HIGH_and;
	 q->tail++;	 z++;

	 y->count=ADC_TIME;
	 y->opera=ADC_BRQ_1V;
	 y++; adc_tail++;

	 y->count=ADC_TIME;
	 y->opera=ADC_BRQ_2V;
	 y++; adc_tail++;
	 break;
      case BR_LOW:
	 z->count=START;
	 z->or_bits=BRQ_LOW_or;
	 z->and_bits=BRQ_LOW_and;
	 q->tail++;	 z++;
	 y->count=ADC_TIME;

	 y->opera=ADC_BRQ_1V;
	 y++;
	 adc_tail++;
	 y->count=ADC_TIME;
	 y->opera=ADC_BRQ_2V;
	 y++;
	 adc_tail++;
	 break;
      case BR_OFF:
	 z->count=ADC_TIME;
	 z->or_bits=BRQ_OFF_or;
	 z->and_bits=BRQ_OFF_and;
	 q->tail++;
	 z++;
	 break;
      case BR_WAIT:
	 z->count=ADC_TIME;  // begin waiting from close to start of cycle
	 z->or_bits=BRQ_OFF_or;
	 z->and_bits=BRQ_OFF_and;
	 q->tail++;
	 z++;
	 z->count=128;  // wait to end of cycle
	 z->or_bits=BRQ_OFF_or;
	 z->and_bits=BRQ_OFF_and;
	 z++;	 q->tail++;

	 y->count=128;
	 y->opera=ADC_BRQ_1V;
	 y++;
	 adc_tail++;

	 y->count=128;
	 y->opera=ADC_BRQ_2V;
	 y++;
	 adc_tail++;
	 q->dir=q->dirnext;  // transfer to next state
	 if(q->dir!=BR_WAIT)trans=1;
	 break;
      default:
	 //	 puts("error");
	 break;
   }

   y=&adc_tab[adc_head];
   adc_tail=adc_head;

   y->count=0;
   y->opera=ADC_BRQ_1V;
   y++;
   adc_tail++;
   y->count=0;
   y->opera=ADC_BRQ_2V;
   y++;
   adc_tail++;

   y->count=q->set;
   y->opera=ADC_BRQ_1I;
   y++;
   adc_tail++;
   y->count=q->set;
   y->opera=ADC_BRQ_2I;
   y++;
   adc_tail++;
   return trans;
}

// convert Q bridge pins into u bridge pins
uint8_t q2u(uint8_t q)
{
   uint8_t u=0;
   if(q & _BV(BRQHI1_BIT) ) u |= _BV(BRUHI1_BIT);
   if(q & _BV(BRQHI2_BIT) ) u |= _BV(BRUHI2_BIT);
   if(q & _BV(BRQLO1_BIT) ) u |= _BV(BRULO1_BIT);
   if(q & _BV(BRQLO2_BIT) ) u |= _BV(BRULO2_BIT);
   if(q & _BV(BRUHI1_BIT) ) u |= _BV(BRQHI1_BIT);
   if(q & _BV(BRUHI2_BIT) ) u |= _BV(BRQHI2_BIT);
   if(q & _BV(BRULO1_BIT) ) u |= _BV(BRQLO1_BIT);
   if(q & _BV(BRULO2_BIT) ) u |= _BV(BRQLO2_BIT);
   return u;
}

// switch bridge identity was this modifys be list generated to
// the bit patterns and adc convrsions to suit bridge u instead
void q2u_list(struct hb *q)
{
   struct sw_op *z;
   adc_tail=adc_head;
   z=&sw_tab[q->head];
   z--;
   do{
	z++;
	z->or_bits=q2u(z->or_bits);
	z->and_bits=q2u(z->and_bits);
     }
   while(z!=&sw_tab[q->tail]);

   //	CONVERT ADC conversions to u channel as well
   struct adc_op *y;
   y=&adc_tab[adc_head];
   y--;
   do{
	y++;
	y->opera+=ADC_BRU;
     }
   while(y!=&adc_tab[adc_tail]);
}

/*
   this routine prepares a series of q bridge operations to the sw_tab[]
   this table is used by the timer 2 interrupt to control q bridge switches.
   ADC conversions to support feedback are written to the adc_tab[] 
   and trigger adc samples.
   whenever there is a change in q or u drives parameters 
   this routine is called to generate the new switching pattern 
   adc operations are placed into a seperate adc request list
*/

#define USTART 0

// this routine modifies the switching linked list (sw_tab[])
// which is used by the timer2 interrupt to control q&u bridge elements
// it also rewrites adc_tab[], which is used to do the ADC samples
// It must be called whenever there is a change to either u or q's
// parameters in order for them to take effect
list_gen()
{
   struct hb *q=&hbq;
   struct hb *u=&hbu;

   char trans=0;  // flag which signals a transitional state
   uint8_t     head;
   do{
	trans+=trans;  // keep track of iterations
	if(++buffer==3)buffer=0;  // tripple buffer switching

	lasttime=TCNT1;
	q->head=(buffer*(SW_MAX/3));   // depending on buffer
	adc_head=(buffer*(ADC_MAX/3));   // or 8 or 16 depending on buffer

	// test to prevent over loading executing switch statements - should not need this
	char test;
	for(test=q->head;test<q->head+(SW_MAX/3);test++)
	{
	   if(sw_pos==&sw_tab[test]){
				       test=q->head-1;
				    }
	}

	char adc_start=adc_head;
	trans|=gen(q);  //  generate q bridge list
	adc_head=adc_tail;  // join the adc lists together
	u->head=q->tail;
	trans|=gen(u);  // generate a bridge list
	q2u_list(u);    // convert this list to bridge u
	// generate an ordered combined join the 2 lists into buffer a or b
	char up=u->head;
	char qp=q->head; // sub list pointers
	char last=0;  // this is the previous list entry

	head=-1;
	// select the first element -list entry point
	if(up<u->tail && qp<q->tail)
	{
	   // with u first seemed to fail ?
	   head=(sw_tab[qp].count<= sw_tab[up].count )?qp:up;
	} else {
		  if( qp<q->tail) head=qp;
		  if( up<u->tail) head=up;
	       }
	last=head;
	// join the q and u lists together
	while(up<u->tail && qp<q->tail)
	{
	   if(sw_tab[qp].count< sw_tab[up].count )
	   {
	      sw_tab[last].next=&sw_tab[qp];
	      last=qp;
	      qp++;
	      continue;
	   }
	   if(sw_tab[up].count< sw_tab[qp].count ){
						     sw_tab[last].next=&sw_tab[up];
						     last=up;
						     up++;
						     continue;
						  }

	   // only posibility left is that the 2 operations are at the same time
	   // they are both switching operations so we logically combine them to improve
	   // the list processing efficiency
	   sw_tab[last].next=&sw_tab[qp];
	   sw_tab[qp].or_bits|=sw_tab[up].or_bits;
	   sw_tab[qp].and_bits&=sw_tab[up].and_bits;
	   last=qp;
	   up++;
	   qp++;
	}

	while( qp<q->tail) {
			      sw_tab[last].next=&sw_tab[qp];
			      last=qp;
			      qp++;
			   }
	while( up<u->tail) {
			      sw_tab[last].next=&sw_tab[up];
			      last=up;
			      up++;
			   }
	sw_tab[last].next=&sw_tab[head];   // loop back to start point

/* logically merge the and and or operations to make up a single byte pattern
	   this is faster to write */
	unsigned char patt=sw_tab[hookpoint].bits; //this is the combined bit pattern start off safe
	struct sw_op *pnt;
	struct sw_op *start=&sw_tab[head];  // start from the beginning
	pnt=start;  // start from the tail
	char x;
	do{
	     patt|=pnt->or_bits;   // send out a bit pattern to port C
	     patt&=pnt->and_bits;   // send out a bit pattern to port C
	     pnt->bits=patt;  // save pattern
	     pnt=pnt->next;
	  }while(start!=pnt);

/* ok now change-swap lists by connecting the new sequence to the old
	   join new switch sequence to the last point of the
	   previous one , and remember this sequences last point
	   so we can use it to join to the next 
*/
	// TCCR2= T2_CKSTOP;  // stop the clock during the interrupt
	// TIMSK&=~_BV(OCIE2);  // disenable timer 2 compare interrupt enable
	sw_tab[hookpoint].next=&sw_tab[head];
	hookpoint=last;

/* adc conversion linked list handling is here */
	last=adc_start;
	// join the adc list elements together
	char y;
	for(y=adc_start;y<adc_tail;y++){
					  adc_tab[last].next=&adc_tab[y];
					  last=y;
				       }
	adc_tab[last].next=&adc_tab[adc_start];   // loop back to start point
	// ok now change-swap lists by splicing the new sequence to the last
	// element of the previous sequence ,
	// and remember this sequences last point
	// so we can use it to splice into the next
	adc_tab[adc_hook].next=&adc_tab[adc_start];
	adc_hook=last;
	difftime=TCNT1;   // code performance cycle counter
	// char end=x;
	//	request=adc_req-1;
	//	token=adc_req-1;
     }while(trans==1);  // was there a transitional state ?

   // tag last element in the ADC list as a trigger for the feedback
   // ADC data capture
   adc_tab[adc_hook].opera|=_BV(7);
   return;
}

/*
   SIGNAL (SIG_OVERFLOW2)
   {
   return;
   }
*/

/* this interrupt iterates through a  pre generated linked list
   which contains the bit patterns and timing information to operate the
   two H bridges elements at a frequency of around 9Khz.
   This interrupt is normally called between 9000 and 36000 times each
   second, at 36000 calls per second uses 2MIPS or 12% cpu time on the avr.
*/
SIGNAL (SIG_OUTPUT_COMPARE2)
{
/* small hack to make sure we dont miss step
      due to other interrupt processing pushing the count time past
      the next interrupt */
   TCNT2=sw_pos->count+2;  // set the clock in case our interrupt was delayed
   PORTC=sw_pos->bits;
   sw_pos=sw_pos->next;
   OCR2=sw_pos->count;
   PORTB^=_BV(4);  // drive charge pump
}

#endif
//#endif
