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
// under construction
// feedback controlled PWM motor speed controller
// PWM motor driver using  OC2 and one arbitary direction pin
// uses the pulse input for getting speed feedback
// this is a ramp up and time delay stop driver
// which has command&direction flags which allow asynchronous
// control over the next state of the driver
// has a goto position function
#include <avr/io.h>
#include ATMEGA
#include <inttypes.h>
#include <math.h>
#include <avr/timer.h>
#include <avr/interrupt.h>
#include <avr/signal.h>
#include "adc.h"
#include "fail.h"
#include "string.h"
#include "monorail.h"
#include "command.h"
#include "pwm.h"
#include "avr.h"

// enable this module here
//#define PWMPID
#ifdef PWMPID

// pin 20 is PORTD6
#define PWMDIR_PORT PORTD
#define PWMDIR_DDR DDRD
#define PWMDIR_PIN PIND
#define PWMDIR_BIT 6

#define GOFORWARD sbi(PORTD,PWMDIR_BIT)
#define GOREVERSE cbi(PORTD,PWMDIR_BIT)

#define FORWARD bit_is_set(PWMDIR_PIN,PWMDIR_BIT)
#define REVERSE bit_is_clear(PWMDIR_PIN,PWMDIR_BIT)

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

// pin 21 is speed PWM output
#define SPEED OCR2

void init_pwm()
{
   //   port D PWM motor output
   PWMDIR_DDR|=_BV(PWMDIR_BIT);  // direction
   sbi(DDRD,7);   // speed
   // timer2 PWM mode
   // TCCR2=(1<<COM21)+(1<<CS20)+T2_CK1024;
   OCR2=0x7F;  // 50%
   // fast PWM mode, PWM compare toggle, direct clocking no prescaler
   TCCR2= (1<<WGM21) + (1<<WGM20) + (1<<COM21) + T2_CK256;
   sbi(TIMSK,TOIE2);  // enable timer 2 overflow interrupt enable
}

const int TICTIME=((256/MCLOCK)*255);
// tic is the time period of the main loop interupt occurances
// most main loop timing is related to this value
// the main loop runs off a timer 1 overflow interrupt

struct drive mydrive;
// called to initialise control
void init_rail()
{
   struct drive *z=&mydrive;  // setup structure pointer
   //   z->utime=0;
   z->state=STOPPED;
   z->ramp_rate=20;  // in 804c3e0er second seconds
   z->stop_time=2000000;  // us
   z->top_speed=130;  //  =0-255 (pwm ratio)
   z->timeout=10000000; // _timer varable. =0
   z->quad_set=0;  //quadrature
   z->quad_count=1<<16; // halfway
   z->battery_low=11*ADCSCALE;  // level. =11volts
   z->battery_high=14*ADCSCALE;   //9 battery high level. =14 volts
   z->battery=12*ADCSCALE;   //
   FLAGS=0;
   sbi(FLAGS,TESTMODE);   //
   sbi(FLAGS,TIMEOUT);   //
   //sbi(FLAGS,S_FORWARD);   //
};

/* called to startup motor */
void go(){
	    struct drive *z=&mydrive;  // setup structure pointer
	    z->utime=0;
	    z->state=RAMP;
	    sbi(TCCR2,COM21);  // connect PWM signal
	    SPEED=0;
	 };

/* called to effect a stop */
void stop(){
	      struct drive *z=&mydrive;  // setup structure pointer
	      z->utime=.005;
	      SPEED=0;
	      z->state=STOPPING;
	   };

void pwm_print()
{
   switch(mydrive.state){
			   case TRANSIT:
			      puts("transit");
			      break;
			   case STOPPED:
			      puts("stopped");
			      break;
			   case RAMP:
			      puts("ramp");
			      break;
			   case STOPPING:
			      puts("stopping");
			      break;
			   default:
			      puts("undef");
			      break;
			}
   puts(" flags{");
   if(bit_is_set(FLAGS,S_FORWARD))puts("Sforward ");
   if(bit_is_set(FLAGS,S_REVERSE))puts("Sreverse ");
   if(bit_is_set(FLAGS,TESTMODE))puts("testmode ");
   if(bit_is_set(FLAGS,TIMEOUT))puts("timeout ");
   //syncwait();
   puts("} speed=");
   puts(itoa(SPEED,&buf,10));
   put_cr();
   return;
}

//rollover time for 32bit us clock is

uint16_t speed;
// damping
// call this 100 times each second
pid()
{
   float speed;
   // dont adjust speed unless we have a new pulse sample available
   if(pulse_count==last_count)return;
   last_count=pulse_count;

   static uint32_t lasttime=0;
   uint32_t time=TCNT1+(((uint32_t)mainclock)<<16);
   uint16_t dt=(time-lasttime)/MCLOCK;
   lasttime=time;
   // we attempt to control speed to match set to pulse width
   // uses PID
   // set speed operates one cycle
   // only operate routines if a new pulse is received
   // or can timeout
   float velocity=dt/pulse_width;
   speed+=.6*((velocity-lvel)/dt);  // get time-velocity differential
   lvel=velocity;  // remember last velocity

   cvel=(set-velocity);  // find difference between set and actual speed
   intigration+=cvel;  // intigration of difference
   speed+=.8*cvel;
   speed+=dvel*.6;  // differential factor
   speed+=intigration;

}

// this controls the motor drive PWM output and direction relays
SIGNAL (SIG_OVERFLOW2)   //void interrupt_loop()
{
   // static uint32_t utime;
   struct drive *z=&mydrive;  // setup structure pointer

   // static uint16_t utime=z->utime;  increment time
   z->utime+=TICTIME;      // increment system time
   // uint8_t speed;
   switch (z->state)
   {
      case STOPPED:
	 cbi(TCCR2,COM21);  // disconnect PWM signal when off
	 SPEED=0x00;  // off
	 break;
	 // 10ms increments for % rate change
	 // 1s=100
      case RAMP:
	 {
	    //  code below is most efficient only 2 function calls (one divide one multiply )
	    uint16_t fac=(z->ramp_rate * z->top_speed);
	    SPEED=( (z->utime>>8) * fac)/(uint32_t)(100000000>>8);
	    if(SPEED >= z->top_speed){
					z->state=TRANSIT;
				     }
	 }
	 break;

      case STOPPING:
	 {
	    SPEED=0;
	    if(z->utime >= z->stop_time )z->state=STOPPED;
	 }
	 break;

      case TRANSIT:
	 {
	    SPEED=z->top_speed;
	    break;
	 }
   }

   if(z->state == TRANSIT  || z->state== RAMP)
   {
      // special stop conditions
      if( z->battery < z->battery_low ) stop();
      if(FORWARD && TRACK_END) stop();
      if(REVERSE && TRACK_START )stop();

      // optional timout condition
      if (bit_is_set(FLAGS, TIMEOUT))
      {
	 if(z->utime >= z->timeout ) stop();
      }

      // optional distance stop condition
      if (bit_is_set(FLAGS, QCOUNT))
      {
	 if(FORWARD && z->quad_set < z->quad_count) stop();
	 if(REVERSE && z->quad_count < z->quad_set) stop();
      }
   }

   if (z->state== STOPPED )
   {
      // autoreverse test feature
      if(bit_is_set(FLAGS, TESTMODE))
      {
	 // when in test mode we will start moving in opposite direction if stopped for longer than timout
	 if(z->utime >= z->timeout ){
				       if(FORWARD) sbi(FLAGS ,S_REVERSE);
				       if(REVERSE) sbi(FLAGS ,S_FORWARD);
				       z->utime=0;
				    }
	 // when we get to the end we autoreverse
	 if(TRACK_END){
			 GOREVERSE;
			 go();
		      }
	 if(TRACK_START){
			   GOFORWARD;
			   go();
			}
      }

      // check command bits
      if(bit_is_set(FLAGS,S_FORWARD)){
					cbi(FLAGS ,S_FORWARD);
					GOFORWARD;
					go();
				     }
      if(bit_is_set(FLAGS,S_REVERSE)){
					cbi(FLAGS ,S_REVERSE);
					//				 bit_is_set(FLAGS ,= ~(S_FORWARD | S_REVERSE);
					GOREVERSE;
					go();
				     }
   }

   if(ESTOP){
	       //    FLAGS=0;
	       // stop();
	    }
   if(TESTBUTTON){
		    //    sbi(FLAGS,TESTMODE);
		    //sbi(FLAGS,S_REVERSE);
		    //stop();
		 }
}
#endif

