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
/*
   RC servo high resolution driver
   uses full rate avr clock resolution pulses
   to control 2 RC servos using the 16 bit counter timer
   ouptuts OC1A and OC1B
   includes slew rate controlled positioning.

   as many as 16 servos could be driven by using output multiplexers 
   to drive each servo in turn, with full clock resolution.

   16 bit counter Timer 1  also provides the Master Clock update
   so this extra function is part of the servo driver functionality,
   other functions which use the master clock will not work properley
   if this update is disabled.
*/
#include <avr/io.h>
#include ATMEGA
#include <inttypes.h>
#include <math.h>
#include <avr/timer.h>
#include <avr/interrupt.h>
#include <avr/signal.h>
#include "adc.h"
#include "avr.h"

#include "servo.h"
#include "monorail.h"
#include "command.h"

#ifndef TIMER
#ifdef SERVO_CODE
// macro to setup clear output pins to low ona compare match
#define CLEAR {  \
		    TCCR1A&=~_BV(COM1A0); \
		    TCCR1A&=~_BV(COM1B0); \
		    TCCR1A|=_BV(COM1A1); \
		    TCCR1A|=_BV(COM1B1); \
}

// macro to setup clear output pins to low ona compare match
#define OFF {  \
		    TCCR1A&=~_BV(COM1A0); \
		    TCCR1A&=~_BV(COM1B0); \
		    TCCR1A&=~_BV(COM1A1); \
		    TCCR1A&=~_BV(COM1B1); \
}

// macro to setup Set output pins to low on a compare match
#define SET  { \
   TCCR1A|=_BV(COM1A0); \
   TCCR1A|=_BV(COM1B0); \
   TCCR1A|=_BV(COM1A1); \
   TCCR1A|=_BV(COM1B1); \
}

#else
#define CLEAR {}
#define OFF { }
#define SET  { }
#endif

// resolution is the number of bits of precision for the
// servo pulse width value can be from 8 to 12 bits
// this will affect the positioning information to the servos
// as positions must be given in this resolution
#define RESOLUTION 11

uint16_t adctime;
const uint16_t SERVOTOP=65535;
// routine to provide a 1-2ms length pulse duration for servo control
uint16_t get(struct servo_ *z)
{
   return((z->pos*16000.0)+6000);
}

uint32_t tmp;
// routine to provide a 1-2ms length pulse duration for servo control
// return unit is pulse width in clock cycles (of whatever CLOCK rate is)
// dt is to be in micro seconds
uint16_t step(struct servo_ *z,uint16_t dt)
{
   if(z->rate>0 && z->pos<z->endpos) z->pos+=z->rate;
   if(z->rate>0 && z->pos>z->endpos) z->pos-=z->rate;
   if(z->pos>=_BV(RESOLUTION)){
				 z->pos=_BV(RESOLUTION)-1;
			      }
   if(z->pos<0){
		  z->pos=0;
	       }
   return((  ( z->pos * (int32_t)(CLOCK*.0018) )>>RESOLUTION )+(CLOCK*.0008));
}

struct servo_ servo[2];
const uint16_t TMR1US=4096;
volatile uint16_t servoutime=0;
volatile char pulse_a=1;
volatile char pulse_b=1;

servo_print2(struct servo_ *z)
{
/*
      puts("pos=");
      put_i16(z->pos);
      puts(" rate=");
      put_i16(z->rate);
      puts(" endp=");
      put_i16(z->endpos);
      put_cr();
*/
}

servo_print()
{
   puts("srvo=");
   servo_print2(&servo[0]);
   servo_print2(&servo[1]);
   put_i16(OCR1A);
   equal("adc");
   put_i16(adctime);
   put_cr();
   return;
}

// when match occurs we set inverted mode generation and set compare for top
SIGNAL(SIG_OUTPUT_COMPARE1A)
{
   if(pulse_a>0){
		   pulse_a--;
		   return;
		}
#ifdef SERVOS
   TCCR1A&=~_BV(COM1A1);  // disconnect outout now pulse is finished
#endif
   cbi(TIMSK,OCIE1A);
}

SIGNAL(SIG_OUTPUT_COMPARE1B)
   //SIGNAL(SIG_TIMER1COMPB)
{
   adctime=TCNT1;
   if(pulse_b>0){
		   pulse_b--;
		   return;
		}
   // TCCR1A&=~_BV(COM1B1);  // disable output
   // cbi(TIMSK,OCIE1B);
}


// provide the periodic update to the servo positions called every x ms
SIGNAL (SIG_OVERFLOW1)
{
   TIMSK|=_BV(TOIE0);  // timer 0 overflow interrupts  enable irdds to run
   //adc_task();
   //irdds_go();
   mainclock++;  // update main cpu clock counter

   servoutime+=TMR1US;
   if(servoutime<17000)return;
   // it's time to prepare generation of a pulse
   OCR1A=step(&servo[0],servoutime); //pwm;
  // OCR1B=step(&servo[1],servoutime); //pwm;
#ifdef SERVOS
   sbi(DDRD,4);  // servo pulse out
   sbi(DDRD,5);  // servo pulse out
   TCCR1A|=_BV(COM1A1);
   TCCR1A|=_BV(COM1B1);  // enable non inverted PWM
#endif
   // enable OC1A OC1B pins
   // it's to late to have any effect on this cycle but it will
   // be loaded via the double buffer ready for next time
   servoutime=0;
   pulse_a=2;
   pulse_b=2;
   //TIFR=_BV(OCF1B)|_BV(OCF1B);  // clear pending interrupts
   //TIFR=_BV(OCF1A)|_BV(OCF1A);  // clear pending interrupts
   sbi(TIMSK,OCIE1A);  // enable overflow interrups  timer_enable_int (_BV (TOIE1));
   // sbi(TIMSK,OCIE1B);  // enable overflow interrups  timer_enable_int (_BV (TOIE1));
   return;
}

void   init_servo()
{
#ifdef SERVOS
   sbi(DDRD,4);  // servo pulse out
   sbi(DDRD,5);  // servo pulse out
   cbi(PORTD,4);
   cbi(PORTD,5);
   //sbi(TCCR1A,COM1A0);  // enable OC1A pin
   //sbi(TCCR1A,COM1A1);  // enable OC1A pin
   //sbi(TCCR1A,COM1B0);  // enable OC1B pin
   //sbi(TCCR1A,COM1B1);  // enable OC1B pin
#endif
   // 16bit normal mode operation
   TCCR1A &=~_BV(WGM11);   // | _BV(WGM10);
   TCCR1A &=~_BV(WGM10);
   TCCR1B =  T1_CK1;
   TCCR1B &=~_BV(WGM12);
   TCCR1B &=~_BV(WGM13);
   TIMSK|=_BV(TOIE1)|_BV(OCIE1A);  // enable overflow interrups  timer_enable_int (_BV (TOIE1));
   //TIMSK|=_BV(TOIE1);  // enable overflow interrups  timer_enable_int (_BV (TOIE1));
   TIMSK|=_BV(OCIE1A);
   TIMSK|=_BV(OCIE1B);  // enable compare interrupt
   cbi(TIMSK,TICIE1);  // dis able input capture
   OCR1A = 13000;
   OCR1B = 12000;
   SET;  // Set compare bits on compare match
   ICR1=~0;
   servo[0].pos=1<<(RESOLUTION-1);
   servo[0].rate=1;
   servo[0].endpos=1<<(RESOLUTION-1);
   servo[1].pos=1<<(RESOLUTION-1);
   servo[1].rate=1;
   servo[1].endpos=1<<(RESOLUTION-1);
/* enable interrupts */
}
#endif

