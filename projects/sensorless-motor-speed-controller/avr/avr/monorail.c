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
// main entry point is here
#include <avr/io.h>
#include ATMEGA
#include <inttypes.h>
#include <stdlib.h>
#include <math.h>
#include <avr/timer.h>
#include <avr/interrupt.h>
#include <avr/signal.h>
#include "adc.h"
#include "uart.h"
#include "fail.h"
#include "string.h"
#include "monorail.h"
#include "command.h"
#include "common.h"
#include "pwm.h"
#include "step.h"
#include "bridge.h"

#ifdef MONORAIL
/* This is code specific to the miniature UCC monorail
   it uses a single DC motor to traverse along a pipe track
   we can control the motor speed using the voltage feedback method 
   and detect an over current condition as an obstruction on the the track
   also have track end sensors and battery voltage monitoring 
   it should automatically move back to the recharging station
   when the battery is going flat, and have the ability to
   move slowly along the track , or to a specific location
   on the track, we have a low resolution wheel pulse sensor
   communications consists of carrdige return terminated packets
   via RS232  
   and now added an LCD status display function
*/


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

// this is only called after we are sure everything is safe to begin
void startup()
{
   //init_quad();     // setup a quaderature encoder input
   //init_pulse();     // setup a pulse counting input
   //init_switches(); // setup some limit switches
   RED_DDR|=_BV(RED_BIT);
   GREEN_DDR|=_BV(GREEN_BIT);

   //   RED_ON;
   //   GREEN_ON;
   // init_rail();
   // init_pwm();    // PWM motor driver
   init_adc();      // start ADC converter loop
   init_bridge();    // h bridge motor driver
   //init_step();    // stepper motor driver
   //init_irrx();   // infra red receiver
   //if(ESTOP )fail(4);
   //if(TESTBUTTON )fail(3);
   //if(TRACK_START)fail(2);
   //if(TRACK_END)fail(1);
   init_servo();    // Dual RC servo driver
   // init_irdds(); // setup driver for sharp GP2D02
   return;
}

void rail_com( char **z)
{
   char *dummy;
   char y=**z;
   (*z)++;
   switch (y)
   {
      case 'p':
	 rail_print();
      case 'w':
	 sbi(FLAGS,S_FORWARD);
	 puts("qfstop");  //bridge trip reset
	 put_cr();
	 break;
      case 'z':
	 init_bridge();
	 break;
      case 'v':   // set battery low voltage recharge alert level
	 if(isprint(**z)){
			    struct drive *q=&mydrive;  // setup structure pointer
			    q->battery_low=strtod(*z,&dummy);
			 }
	 break;
      case 'i':   // set battery voltage recharge level
	 if(isprint(**z)){
			    struct drive *q=&mydrive;  // setup structure pointer
			    q->battery_high=strtol(*z,0,10);
			 }
	 break;
      case 's':   // set cruise speed
	 if(isprint(**z)){
			    struct drive *q=&mydrive;  // setup structure pointer
			    q->battery_high=strtod(*z,&dummy);
			 }
	 break;
      case 'b':   // read parameters from mime64 data
	 {
/*
	       mimetoraw(&z,buf,12);
	       uint8_t *data=buf;
	       struct drive *z=&mydrive;  // setup structure pointer
	       z->battery_low=getw(&data);
	       z->battery_high=getw(&data);
	       z->ramp_rate=getw(&data);
	       z->stop_time=getw(&data);
*/
	 }
	 break;
	 (*z)++;
	 return;
   }
}

void rail_print()
{
   puts(" stoptime=");
   puts(ultoa(mydrive.stop_time,&buf,10));
   puts(" lasttime=");
   puts(itoa(mydrive.dir,&buf,10));
   puts(" hi=");
   puts(dtostrf(mydrive.battery_high,5,4,&buf));
   puts(" lo=");
   puts(dtostrf(mydrive.battery_low,5,4,&buf));
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
   putc('}');   put_cr();
   //syncwait();
}
const int TICTIME=((256/MCLOCK)*255);
// tic is the time period of the main loop interupt occurances
// most main loop timing is related to this value
// the main loop runs off a timer 1 overflow interrupt

struct drive mydrive;
// called to initialise rail control
void init_rail()
{
   struct drive *z=&mydrive;  // setup structure pointer
   //utime=0;
   z->state=STOPPED;
   z->ramp_rate=20;  // in 804c3e0er second seconds
   z->stop_time=8000000;  // us
   z->top_speed=200;  //  =0-255 (pwm ratio)
   z->top_rate=47000;   // minimum stepper motor pulse width
   z->dir=1;   // direction
   z->timeout=10000000; // _timer varable. =0
   z->quad_set=0;  //quadrature
   z->quad_count=1<<16; // halfway
   z->battery_low=11*ADCSCALE;  // level. =11volts
   z->battery_high=14*ADCSCALE;   //9 battery high level. =14 volts
   //  z->battery=12*ADCSCALE;   //
   FLAGS=0;
   sbi(FLAGS,TESTMODE);   //
   //sbi(FLAGS,TIMEOUT);   //
   //sbi(FLAGS,S_FORWARD);   //
};

/* called to effect a stop */
void stop(){
	      struct drive *z=&mydrive;  // setup structure pointer
	      z->utime=0;
	      //pwm_set(0);
	      //step_set(0,0);
	      z->state=STOPPING;
	   };

// this is the rail carridge main control loop
// handles stopping acelleration , end stops
// test operation modes etc.
// has a ramp up and time delay stop driver
// which has command&direction flags which allow asynchronous
// control over the next state of the driver
// must call it preferabley 10 times each second or more-periodically
// so you can run it off any convenient interrupt
void rail_interrupt()
{
   struct drive *z=&mydrive;  // setup structure pointer
   static uint32_t lasttime=0;
   uint32_t time=TCNT1+(((uint32_t)mainclock)<<16);
   uint32_t dt=(time-lasttime)/MCLOCK;
   lasttime=time;
   // static uint16_t utime=utime;  increment time
   z->utime+=dt;   //TICTIME;      // increment system time
   // uint8_t speed;
   switch (z->state)
   {
      case STOPPED:
	 z->dir=0;
	 //pwm_dir(0);
	 //pwm_set(0x00);  // off
	 //step_set(0,0);
	 break;
      case RAMP:
	 {
	    //  code below is most efficient only 2 function calls (one divide one multiply )
	    uint16_t fac=(z->ramp_rate * z->top_speed);
	    //pwm_set(( (utime>>8) * fac)/(uint32_t)(100000000>>8));
	    uint8_t speed=(( (z->utime>>8) * fac)/(uint32_t)(100000000>>8));
	    bridge_set(&hbq,speed,z->dir);
	    if(speed >= z->top_speed){
					z->state=TRANSIT;
					bridge_set(&hbq,z->top_speed,z->dir);
				     }
/*
	       if(utime>800000){
	       step_set(z->top_rate+(z->top_rate>>1),z->dir);  // 3/4 speed
	       }else{
	       step_set(z->top_rate*2,z->dir);   // 1/2 speed
	       }
	       if(utime>1600000)z->state=TRANSIT;
*/
	 }
	 break;

      case STOPPING:
	 {
	    //pwm_set(0);
	    bridge_set(&hbq,0,z->dir+z->dir);
	    //qbridge_set(0,0);
	    //step_set(0,0);
	    if(z->utime >= z->stop_time ){
					    z->state=STOPPED;
					    z->utime=0;
					    bridge_set(&hbq,0,0);
					 }
	 }
	 break;

      case TRANSIT:
	 {
	    //pwm_set(z->top_speed);
	    //step_set(z->top_rate,z->dir);
	    break;
	 }
   }

   if(z->state == TRANSIT  || z->state== RAMP)
   {
      // special stop conditions
      //if( adc_battery < z->battery_low ) stop();
      if(z->dir>0 && TRACK_END)stop();
      if(z->dir<0 && TRACK_START )stop();

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
				       if(z->dir>=0) sbi(FLAGS ,S_REVERSE);
				       if(z->dir<0) sbi(FLAGS ,S_FORWARD);
				       z->utime=0;
				    }
	 // when we get to the end we autoreverse
	 if(TRACK_END){
			 z->utime=0;
			 z->state=RAMP;
			 z->dir=-1;
		      }
	 if(TRACK_START){
			   z->utime=0;
			   z->state=RAMP;
			   z->dir=1;
			}
      }

      // check command bits
      if(bit_is_set(FLAGS,S_FORWARD)){
					cbi(FLAGS ,S_FORWARD);
					GOFORWARD;
					z->utime=0;
					z->state=RAMP;
					z->dir=1;
					// pwm_dir(z->dir);
					// step_set(z->top_rate,z->dir);
				     }
      if(bit_is_set(FLAGS,S_REVERSE)){
					cbi(FLAGS ,S_REVERSE);
					z->utime=0;
					z->state=RAMP;
					z->dir=-1;
					// pwm_dir(z->dir);
					// step_set(z->top_rate,z->dir);
				     }
   }

   if(ESTOP){
	    }
   if(TESTBUTTON){
		 }
}


SIGNAL (SIG_OVERFLOW2)
{
   rail_interrupt();
}
#endif

