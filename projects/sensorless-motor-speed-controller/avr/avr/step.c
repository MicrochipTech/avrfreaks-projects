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
// STEPPER motor IC driver using OC2 and one arbitary direction pin
// this is a ramp up and time delay stop driver
// which has command&direction flags which allow asynchronous
// using a 5804 stepper motor driver IC for testing
// control over the next state of the driver
// could be easily modified to use any 8 bit timer
#include <avr/io.h>
#include ATMEGA
#include <inttypes.h>
#include <avr/timer.h>
#include <avr/interrupt.h>
#include <avr/signal.h>
#include "adc.h"
#include "fail.h"
#include "string.h"
#include "monorail.h"
#include "command.h"
#include "step.h"
#include "pwm.h"
#include "avr.h"

#ifdef STEPPER

// pin is PORT C0 pin avr-22
#define STEPDIR_DDR DDRC
#define STEPDIR_PIN PINC
#define STEPDIR_PORT PORTC
#define STEPDIR_BIT 0

// 5804 pin avr-21 is PORTD7 and OC2 pin
#define STEPIN_DDR DDRD
#define STEPIN_PIN PIND
#define STEPIN_PORT PORTD
#define STEPIN_BIT 7

// 5804 stepper IC enable pin to avr-27
#define STEPEN_DDR DDRC
#define STEPEN_PIN PINC
#define STEPEN_PORT PORTC
#define STEPEN_BIT 5

// 5804 stepper IC one phase input  avr
// #define STEPONE_DDR DDRC
// #define STEPONE_PIN PINC
// #define STEPONE_PORT PORTC
// #define STEPONE_BIT 3

// 5804 stepper IC one phase input from pin 28 PC6 on avr
#define STEPHALF_DDR DDRC
#define STEPHALF_PIN PINC
#define STEPHALF_PORT PORTC
#define STEPHALF_BIT 6

#define GOFORWARD sbi(PORTD,STEPDIR_BIT)
#define GOREVERSE cbi(PORTD,STEPDIR_BIT)

#define FORWARD bit_is_set(STEPDIR_PIN,STEPDIR_BIT)
#define REVERSE bit_is_clear(STEPDIR_PIN,STEPDIR_BIT)

// pin 21 is speed STEP output
#define SPEED OCR2


uint16_t modulo=0;
uint16_t cnt=0;
uint32_t speed=53333;

void init_step()
{
   STEPEN_DDR|=_BV(STEPEN_BIT);
   STEPHALF_DDR|=_BV(STEPHALF_BIT);
   STEPIN_DDR|=_BV(STEPIN_BIT);
   STEPDIR_DDR|=_BV(STEPDIR_BIT);

   STEPHALF_PORT|=_BV(STEPHALF_BIT);

   STEPDIR_PORT|=_BV(STEPDIR_BIT);

   STEPEN_PORT&=~_BV(STEPEN_BIT);

   // fast STEP mode, STEP compare toggle, direct clocking no prescaler
   //TCCR2= (1<<WGM21) + (1<<COM20) + T2_CK1024;  // CTC mode with toggle pin
   TCCR2= (1<<WGM21) + (1<<COM20) + T2_CK1024;  // CTC mode with toggle pin
   TCCR2= (1<<WGM21) + T2_CK8;  // CTC mode no oputput
   // sbi(TIMSK,OCIE2);  // enable timer 2 compare interrupt enable
   step_set(100000,1);
}
/* called to startup motor */
void step_go(){
		 struct drive *z=&mydrive;  // setup structure pointer
#ifdef MONORAIL
		 z->utime=0;
		 z->state=RAMP;
#endif
		 sbi(TCCR2,COM21);  // connect STEP signal
		 SPEED=0;
	      };


/* called to effect a stop */
void step_print(){
		    equal("ocr2");
		    put_i16(OCR2);
		    equal("tccr2");
		    put_i16(TCCR2);
		    equal("mod");
		    put_i16(modulo);
		    put_cr();
		    return;
		 };

// can set any period from 1/8Meg upto several minutes
// these routines will only use the upper 8 significant bits of
// precission
#define T2_CTC (_BV(WGM21))
void step_set(uint32_t width,int8_t dir)
{
   if(dir==0){
		TIMSK&=~_BV(OCIE2);  // disable timer 2 compare interrupt
		STEPEN_PORT|=_BV(STEPEN_BIT);
		return;
	     }else{
		     STEPEN_PORT&=~_BV(STEPEN_BIT);
		  }
   if(dir>0)   STEPDIR_DDR|=_BV(STEPDIR_BIT);
   if(dir<0)   STEPDIR_DDR&=~_BV(STEPDIR_BIT);

   modulo=0;
   if(width<256){
		   TIMSK&=~_BV(OCIE2);  // disable timer 2 compare interrupt
		   TCCR2= T2_CTC | T2_CK1 | _BV(COM20);
		   OCR2=width;
		   return;
		}
   if(width<256*8){
		     TIMSK&=~_BV(OCIE2);  // disable timer 2 compare interrupt
		     TCCR2= T2_CTC | T2_CK8 | _BV(COM20);
		     OCR2=width>>3;
		     return;
		  }
   if(width<256*32){
		      TIMSK&=~_BV(OCIE2);  // disable timer 2 compare interrupt
		      TCCR2= T2_CTC | T2_CK32 | _BV(COM20);
		      OCR2=width>>5;
		      return;
		   }
   if(width<(uint32_t)(256*64)){
				  TIMSK&=~_BV(OCIE2);  // disable timer 2 compare interrupt enable
				  TCCR2= T2_CTC | T2_CK64 | _BV(COM20);
				  //TIMSK|=_BV(TOIE2);  // enable timer 2 overflow interrupt enable
				  OCR2=width>>6;
				  return;
			       }
   if(width<(uint32_t)1<<15){
			       TCCR2= T2_CTC | T2_CK128 | _BV(COM20);
			       //TIMSK|=_BV(TOIE2);  // enable timer 2 overflow interrupt
			       TIMSK&=~_BV(OCIE2);  // disable timer 2 compare interrupt
			       OCR2=width>>7;
			       return;
			    }
   if(width<(uint32_t)1<<16){
			       TCCR2= T2_CTC | T2_CK256 | _BV(COM20);
			       OCR2=width>>8;
			       //  TIMSK|=_BV(TOIE2);  // enable timer 2 overflow interrupt
			       TIMSK&=~_BV(OCIE2);  // disable timer 2 compare interrupt
			       return;
			    }
   if(width<(uint32_t)1<<18){
			       TCCR2= T2_CTC | T2_CK1024 | _BV(COM20);
			       //    TIMSK|=_BV(TOIE2);  // enable timer 2 overflow interrupt
			       TIMSK&=~_BV(OCIE2);  // disable timer 2 compare interrupt
			       OCR2=width>>10;
			       return;
			    }
   // for longer pulse widths we have no choice but to use multiple interrupt occurances
#define VER1 yes
#ifdef VER1
   TCCR2= T2_CTC | T2_CK1024;
   width=width>>10;
   while(width>(uint32_t)255){
				modulo+=modulo;
				if(modulo==0)modulo=1;
				width=width>>1;
			     }
   OCR2=width;
   TIMSK|=_BV(OCIE2);  // enable timer 2 compare interrupt enable
#endif
#ifdef VER2
   // in version 2 use another trick to improve the resolution on the final
   // count can use an overflow interrupt to trip the timeout to keep the resolution
   // accurate to 22 bits.
   TCCR2= T2_CTC | T2_CK1024;
   width=width>>10;
   while(width>(uint32_t)255){
				modulo+=modulo;
				if(modulo==0)modulo=1;
				width=width>>1;
			     }
   OCR2=width;
   TIMSK|=_BV(OCIE2);  // enable timer 2 compare interrupt enable
#endif


   //TIMSK&=~_BV(TOIE2);  // disable timer 2 overflow interrupt enable
   return;
}

// this controls the motor drive STEP output
SIGNAL (SIG_OVERFLOW2)
{
   // in version 2 use the overflow to count the MSB's and on the last one enable the output compare enable interrupt
}

// this controls the motor drive STEP output
SIGNAL (SIG_OUTPUT_COMPARE2)
{
   if(cnt>=modulo){
		     STEPIN_PORT^=_BV(STEPIN_BIT);
		     cnt=0;
		  }
   cnt++;
}
#endif

