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

// this is called on completion of motor ADC sampling
// it provides current limiting for both motors
// during regenerative breaking and accelleration
// allows voltage feedback based speed control
// this routine is for 2 independent motors

// suggest motor speed be treated as a single variable which
// mimics the motor voltage for this application
// an alternative may be to use pulse frequency instead
// but voltage is good for now - there is no differential
// or integrational component just a loop gain in the feedback

#include <avr/io.h>
#include ATMEGA
#include <inttypes.h>
#include "bridge.h"

#ifdef MONORAIL

int16_t aspeed;
int16_t bspeed;

motor_fb()
{
}

char hbcon(struct hb *z)
{
   int16_t volt,amps;
   // collect and interpret the motor speed using voltage feedback
   // and torque using current feedback
   // possibly also do an averaging filter on the speed and current
   char pdir=z->dir;
   uint8_t setti=z->dir;
   switch(pdir)
   {
      case BR_FWD:
	 volt=z->voltage1;
	 amps=z->current1;
	 break;
      case BR_FWDSTOP:
	 volt=z->voltage1;
	 amps=z->current1;
	 break;
      case BR_REV:
	 volt=-z->voltage2;
	 amps=z->current2;
	 break;
      case BR_REVSTOP:
	 volt=-z->voltage2;
	 amps=z->current2;
	 break;
      case BR_HIGH:
	 break;
      case BR_LOW:
	 break;
      case BR_OFF:
	 break;
      case BR_WAIT:
	 break;
      default:
	 break;
   }


   // ie we can have several methods of controlling a motors speed
   // to accellerate if we are alloready going forward just apply more forward power
   // if we are presently spinning backwards use reverse regen breaking to stop reverse motion
   // first.
   // to slow down we can reduce drive and coast down to a lower speed, this would work
   // when the friction and therefore drive current is quite high.
   // use regenerative breaking if drive current is low
   // such as could detect a freewheeling situation by the current.
   // flow level ie if we detect freewheeling or negtive current then regenerative breaking is
   // necessary
   // or fastest method is to use reverse drive may be necessary if drive current
   // is very low or negative.
   // This intelligent high level control can decide
   // which method to use based on feedback information

   if(z->speed>0 )  // forward motion options
   {
      // we are suposed to be going forward but should I be breaking
      // to reduce speed or powering on to increase speed
      if(volt < z->speed)
      {
	 // i am going too slowly in foreward direction

	 // possibilitys i am at least spinning foreward as i should be
	 // or i am actually going backwards and can use reverse breaking to
	 // stop this reverse direction problem.
	 if(pdir == BR_FWD){
			      {
				 setti+=16;  // continue to build up power
				 if(setti>200)setti=200;
			      }
			      goto doneit;
			   }
	 if(pdir == BR_REVSTOP ){
				   if(amps>-2){
						 // breaking process is completed
						 pdir= BR_FWD;
						 setti=16;
					      }
				   if(amps < -2 ){
						    setti+=16;
						    if(setti>200) setti=200;
						 }
				   goto doneit;
				}

	 //require forward drive accelleration
	 if(!(pdir == BR_FWD  || pdir == BR_REVSTOP) ){
							 if(volt>0){
								      setti=16;  // start at minimum forward speed
								      pdir=BR_FWD;
								   }
							 else{
								setti=16;  // start at minimum forward speed
								pdir=BR_REVSTOP;
							     }
							 goto doneit;
						      }
      }
      if(volt > z->speed)
      {
	 // slow down somehow
	 // if drive friction and curent is high we just reduce drive
	 if(pdir == BR_FWD){
			      if(amps > 2){
					     setti-=16;  // reduce drive power
					     if(setti<8)setti=8;
					  }
			      if(amps<2){
					   // if drive current is low already
					   // use breaking to stop instead.
					   pdir=BR_FWDSTOP;
					   setti=128;  // regen breaking is constant 50% duty cycle
					}
			      goto doneit;
			   }
	 if(pdir == BR_FWDSTOP){
				  if(amps < 1 ){
						  // we have no stopping power
						  // use reverse drive to slow down
						  pdir=BR_REV;
						  setti=32;
					       }
				  if(amps > 1 ){
						  setti+=16;
						  if(setti>200) setti=200;
					       }
				  goto doneit;
			       }
	 if(pdir == BR_REV){
			      // increase reverse drive untill reverse current saturation
			      if(amps < -10 )setti-=16;
			      if(amps > -10 )setti+=16;
			      goto doneit;
			      return;
			   }
	 if(!(pdir == BR_REV  || pdir == BR_FWDSTOP || pdir == BR_FWD)){
									 // this state is no use here
									 pdir= BR_FWD;
                                                                         setti=16;
									 goto doneit;
								      }
      }
      // calling qset or uset as the case may beto correct motor motion
   }
   doneit:
      bridge_set(z,setti,pdir);
   return;

}
set_motor()
{
}

// routine to control a motor using only sensorless feedback
void init_motor()
{
}

#endif

