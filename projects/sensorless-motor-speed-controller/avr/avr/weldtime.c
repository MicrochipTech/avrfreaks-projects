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

#ifdef WELDING
/* this file contains initialisation and procedures specific to a
   spot welding system designed specifically to weld 1.6mm diameter 
   stainless 19%cr,10%ni,2%mo wire chain links together.
   links vary in size from ~8mm inner diameter, upto ~16.3mm 
   It uses an approximatley 150amp rectified 50hz sine wave (mig welder) 
   with an appropriate welding jig
   inputs/ outputs follow
   A toggle input from a foot switch to controls the start of the weld.
   the logic system will not start a new weld for 5 seconds after the last one
   the logic systemm will not allow a weld to begin unless the IRRX-IRTX
   signal is sufficientley coupled so the electrodes must be close to 
   the ready position.

   an digitally controlled IR Source and Analogue photodiode input RX 
   these are mounted on the welding jig arms and measure the movement
   of the electrodes as the wire heats melts and collapses it detects the 
   movement of the colapse and switches off the mig welder, after the 
   melt_time delay.

   the IR distance sensor system should be setup to ensure minimum noise 
   for the weld jig setup ie an optimal minimum distance of around 16mm ,
   so that ir sensor is close to 
   full saturation at this range.
   this ensures a good signal to nose ratio, I run about 
   16ma through the photo emitter source- this should give upto 5ma
   in the photo detctor (50% coupling at 16mm).

   Also an ADC input which measures voltage drop across a current shunt
   to check the welding current.
   an adc input which measures voltage across the chain link being welded.
   This is used only for an safety cuttoff if an open circuit is detected. 

   One output which controls a relay which switches the current on and off.
   to control the weld heating pulse time.

   serial protocol commands are all decimal format as this is intended as a 
   low speed human readable and configurable standalone unit

   RED and GREEN diagnostic lights are also provided their function varies
   with the operational state

   waiting for foot trigger to weld 
   green on = movement sensor ir is coupled correctley we are ready to weld
   red on = is shown for IR RX overload or overcoupling

   distance sensor calibration is performed by pressing the cal button
   the output is calibrated for 15 mm gap during calibration

   during weld or calibrate
   red and green are both off

   For 5 seconds after weld is finnished
   red on=  abnormal weld termination due to safety interlock
   green on = normal weld termination
   red+green means weld went into overtime
*/
/*
   the welding menu function provides a user interface to 
   adjust minimum & maximum weld time , and hold time.
   movement trip distance ,
   calibration distance and start calibration function
   save current settings to eeprom , clear eeprom
*/

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
#include "adc.h"
#include "quad.h"
#include "lcd.h"
#include "menu.h"
#include "avr.h"

// avr microcontroller software to pins definitions below
// foot switch (toggle)
#define SWITCH_DDR DDRD
#define SWITCH_PIN PIND
#define SWITCH_PORT PORTD
#define SWITCH_BIT 3

// en extra button to provide a distance sensor calibration function.
#define BUTTON_DDR DDRB
#define BUTTON_PIN PINB
#define BUTTON_PORT PORTB
#define BUTTON_BIT 3

// used for a photodiode which provides mechanical movement sensor
#define IRRX_DDR DDRA
#define IRRX_PIN PINA
#define IRRX_PORT PORTA
#define IRRX_BIT 1

// used for a IR emitter diode which provides mechanical movement sensor
#define IRTX_DDR DDRC
#define IRTX_PIN PINC
#define IRTX_PORT PORTC
#define IRTX_BIT 3

// main mig control output
#define RELAY_DDR DDRC
#define RELAY_PIN PINC
#define RELAY_PORT PORTC
#define RELAY_BIT 0

#define SWON bit_is_set(SWITCH_PIN,SWITCH_BIT)
#define SWOFF bit_is_clear(SWITCH_PIN,SWITCH_BIT)
#define BUTTON bit_is_set(BUTTON_PIN,BUTTON_BIT)

#define WELDOFF RELAY_PORT|=_BV(RELAY_BIT)
#define WELDON RELAY_PORT&=~_BV(RELAY_BIT)

// the resistance per meter of the 1.6mm diameter ss wire is .34 ohms

float rShunt;

// ir rx -tx coupling minimum value safety interlock setting 0-1024
#define IRSIG 220
// maximum ir RX signal level allowed from adc 0-1024
#define IRMAX 950
#define VFAC .00488
// IFAC is rshunt/.00488
#define IFAC .8714

// ir distance avaraging
#define LOOPS 2

#define Power
#define LinkLength
#define LinkOhmPermm

int8_t cur;  // current selection state
static uint8_t bit=0;
uint32_t max_time;
uint32_t hold_time;
uint32_t weld_time;
uint16_t weld_count;
uint32_t melt_time;
uint32_t min_time;
char er;

// sensitivity of melt movement trigger threshold in 1/100th of a milimeter
int16_t distance;

// used for ascii string conveersions to eeprom
char tmp[12];
uint8_t posi;  // position of our toggle foot switch

float ir_k;  // ir distance calculation value

int16_t gap;
uint16_t CALIBRATE_GAP;
int16_t move_diff;


enum{  WWAIT,WON,WMELT,WOFF,IRCAL  }state; //=WON;
char title[]={  "Spot Weld"  };

//char wc[]={  "wc"  };
//char wmax[]={  "w>"  };
//char wmin[]={  "w<"  };

// this is only called after we are sure everything is safe to begin
// weld timer routines
void startup()
{
   puts(title);
   put_cr();
   syncwait();
   init_servo();    // Dual RC servo driver also sets up timer 1 clock
   init_quad();    // my control quaderature endoder control dial
   init_weld();
   //efile_exec();
   init_adc();
#ifdef LCD_CODE
   lcd_init();
   lcd_title(title);
#endif
   return;
}

// serial port and eeprom boot commands processor
void weld_com( char **z)
{
   char *dummy;
   char y=**z;
   (*z)++;
   switch (y)
   {
      case 'p':
	 weld_print();
	 break;
      case 'z':
	 init_weld();
	 break;
      case 'w':
	 posi=99;
	 //	 weld_count++;  // remember number of welds we have done
	 //	 posi=SWON;
	 WELDON;
	 //	 RED_OFF;
	 //	 GREEN_OFF;
	 state=WON;
	 //	 setzero();
	 break;
      case 's':   // set movement detection distance factor
	 if('='==(**z)){
			  distance=strtol((*z)+1,0,10);
		       }
	 break;
	 //     case 'r':
	 //	 if('='==(**z)){
	 // 			  RShunt=strtod((*z)+1,&dummy);
	 //		       }
	 //	 break;
      case 'i':
	 if('='==(**z)){
			  ir_k=strtod((*z)+1,&dummy);
		       }
	 break;
      case 'c':
	 if(state==WWAIT){
			    state=IRCAL;
			    setzero();
			    RED_OFF;
			    GREEN_OFF;
			 }
	 break;
      case '>':   // set maximum weld time
	 if('='==(**z)){
			  min_time=strtol((*z)+1,0,10);
		       }
	 break;
      case '<':   // set minimum weld time
	 if('='==(**z)){
			  max_time=strtol((*z)+1,0,10);
		       }
	 break;
      case 'R':
	 if('='==(**z)){
			  rShunt=strtod((*z)+1,&dummy);
		       }
	 break;
	 (*z)++;
	 return;
   }
}

// enumerated menue functions
enum{
       ME_TOP,
	  ME_MIN,
	  ME_MAX,
	  ME_DIS,
	  ME_CAL,
	  ME_SAVE,
	  ME_SHUNT,
	  ME_SHOW,
	  ME_LOAD,
    };

enum{
       GET_I,
	  GET_MIN,
	  GET_MAX,
	  GET_HOLD,
	  GET_S,
	  GET_CALGAP,
	  GET_RSHUNT,
	  GET_COUNT,
	  GET_END
    };


/*
   // add the printout of the welding variables to the scrolling menu
   weld_vars()
   {
   char *zz;
   char z;
   char h;
   for(h=0;h<GET_END;h++){

   zz=weldget(h);
   tmp[0]='i';
   tmp[1]='n';
   tmp[2]='f';
   tmp[3]='o';
   tmp[4]=zz[0];
   tmp[5]=zz[1];
   tmp[6]=zz[2];
   z=menu_add(pos2,tmp,0);
   if(z)data_add(&zz[3],8);  // draw max_time if visible
   }
   }
*/

// get change in dial control since last call
int8_t quad_get()
{
   static uint8_t pos;
   int8_t x;
   x=quad-pos;
   pos=quad;
   return x;
}
/* main call in to update and re draw the LCD menu system
   and process any selected menu items 
*/
void weld_menu()
{
#ifdef LCD_CODE
   static uint16_t pos=8;  // remember position in top level menu
   int8_t x,y,z,v=0;
   uint16_t pos2;
   x=quad_get();
   y=button_get();

   if(cur==ME_SHOW){
		      weld_show();
		      if(y)cur=ME_TOP;  //return
		      return;
		   }

   if(cur==ME_TOP)
   {
      if(x>=0){
		 pos+=x;
	      }else{
		      if(pos>(-x)){  pos+=x;
				  }else{  pos=0;
				       }
		   }
      if((pos>>2)>(xpos-8-(LCDCOLS>>1)))pos=(xpos-8-(LCDCOLS>>1))<<2;    // prevent user scrolling past end of menu
   }
   pos2=pos>>2;

   menu_start(pos2);
   if((menu_add(pos2,"test",0,0,0)==2) && y){
					   cur=ME_SHOW;  // switch to weld show during test
					   WELDON;
					   //	 RED_OFF;
					   //	 GREEN_OFF;
					   state=WON;
					}
   data_clr(4);
   z=menu_add(pos2,"load",0,0,0);
   if(z==2 && y )efile_exec();
   // if(z)menu_add(0,weldget(GET_RSHUNT),2,6,1);  // draw max_time if visible

   if((2==menu_add(pos2,"save",0,0,0)) && y )weld_save();
   data_clr(5);

   z=menu_add(pos2,"show",0,0,0);
   if(z==2 && y )cur=ME_SHOW;
   if(z)menu_add(0,"info",2,5,1);

   //MIN
   if(cur==ME_MIN)   {  v=1;    min_time+=x*1000;    x=0;
		     }else {  v=0;  }
   z=menu_add(pos2,"min",v,0,0);
   if(z!=0)menu_add(0,ltoa(min_time/1000,&buf[3],10),2,5,1);   // draw min_time if visible
   if(z==2 && y) cur=ME_MIN;
   if(y && v)cur=ME_TOP;  // exit up

   //MAX
   if(cur==ME_MAX){  v=1;   max_time+=x*1000;     x=0;
		  }else {  v=0;  }
   z=menu_add(pos2,"max",v,0,0);
   if(z)menu_add(0,ltoa(max_time/1000,buf,10),2,5,1);   // draw min_time if visible
   if(z==2 && y) cur=ME_MAX;
   if(y && v)cur=ME_TOP;   //exit up

   //TRIGGER DISTANCE
   v=0;
   if(cur==ME_DIS){  v=1;   	 distance+=x;	 x=0;
		  }
   z=menu_add(pos2,"dis",v,0,0);
   if(z)menu_add(0,ltoa(distance,&buf[3],10),2,5,1);   // draw distance if visible
   if(z==2 &&y)cur=ME_DIS;
   if(y && v)cur=ME_TOP;   //exit up

   //CALIBRATE GAP
   v=0;
   if(cur==ME_CAL){  CALIBRATE_GAP+=x;   // change calibration gap and perform a calibration to match that gap
		     if(state==WWAIT){
					ir_cal();	RED_OFF;	GREEN_OFF;
				     }
		     v=1;
		  }
   z=menu_add(pos2,"cal",v,0,0);
   if(z)menu_add(0,ltoa(CALIBRATE_GAP,buf,10),2,5,1);   // draw distance if visible
   if(z==2 &&y )cur=ME_CAL;
   if(y && v)cur=ME_TOP;   //exit up when button pressed again
   // if(!SELECT && v)cur=ME_TOP;   // exit up when button released

   // on the end of the menu add the weld print function it's a real cpu chewer

   char h;
   for(h=0;h<GET_END;h+=2){
			     if(data_visible(pos2,10)){
                             menu_add(0,weldget(h),2,10,0);
                             menu_add(0,weldget(h+1),2,10,1);
						      }
			  }
#endif
}


uint32_t timer;
void setzero()
{
   timer=TCNT1+(((uint32_t)mainclock)<<16);
   // mainclock;
   return;
}

uint32_t getus()
{
   // TIFR|=_BV(TOV1);   // clear the interrupt
   uint32_t now;
   now=bit_is_set(TIFR,TOV1)?65536:0;
   now+=TCNT1+(((uint32_t)mainclock)<<16);
   return ((now-timer)/MCLOCK);
}

void weld_print()
{
   equal("wt");
   put_i32(weld_time);
   equal("t");
   put_i32(getus());
   syncwait();
   char x;
   for(x=0;x<GET_END;x++){
			    puts(weldget(x));
			    putc(' ');
			 }
   put_cr();
   return;
}

// this function returns strings describing various weld parameters
// the 3 preceding characters identify the variable ie w>=
char *weldget(char val)
{
   uint32_t tmp;
   buf[0]='w';
   buf[2]='=';
   buf[3]=0;
   switch(val){
		 case GET_I:
		    buf[1]='i';
		    dtostrf(ir_k,6,0,&buf[3]);
		    break;
		 case GET_MIN:
		    buf[1]='>';
		    ltoa(min_time,&buf[3],10);
		    break;
		 case GET_MAX:
		    buf[1]='<';
		    ltoa(max_time,&buf[3],10);
		    break;
		 case GET_HOLD:
		    buf[1]='h';
		    ltoa(hold_time,&buf[3],10);
		    break;
		 case GET_S:
		    buf[1]='s';
		    ltoa(distance,&buf[3],10);
		    break;
		 case GET_CALGAP:
		    buf[1]='c';
		    ltoa(CALIBRATE_GAP,&buf[3],10);
		    break;
		 case GET_RSHUNT:
		    buf[1]='r';
		    dtostrf(rShunt,5,4,&buf[3]);
		    break;
		 case GET_COUNT:
		    buf[1]='C';
		    itoa(weld_count,&buf[3],10);
		    break;
		 default:
	      }
   return &buf;
}


weld_save()
{
   char x;
   for(x=1;x<=5;x++){
		       efile_write(weldget(x),1);

		    }
}
/*
   record the weldtime in ms to the eeprom for later examination
   show results on lcd display
*/
float maxi,maxv;
void weld_stat()
{
   buf[0]=er;
   buf[1]=0;
   //ltoa(weld_time/1000,&buf[2],10);
   // efile_write(buf,1);
   return;
}

float maxi,maxv;
void weld_show()
{
#ifdef LCD_CODE
   buf[0]=er;
   buf[1]=0;
   strcat(buf,ltoa(weld_time/1000,&tmp,10));
   lcd_write_string (0,0,buf,8);

   buf[0]='i';
   buf[1]=0;
   strcat(buf,dtostrf(maxi,5,1,&tmp));
   lcd_write_string (8,0,buf,8);

   buf[0]='v';
   buf[1]=0;
   strcat(buf,dtostrf(maxv,5,3,&tmp));
   lcd_write_string (0,1,buf,8);

   buf[0]=er;
   buf[1]=0;
   strcat(buf,ltoa(gap,&tmp,10));
   lcd_write_string (8,1,buf,8);
#endif
}


/*
   as the distance sensor is a linear device and light attenuation
   is the square of the distance the ir readback value from the IR sensor
   must be processed to produce a correct distance reading.
   the equation of this distance to ir level is 
   IR=k/(D^2)   and therefor   D=sqrt(k/IR)
   to find k we use k=IR*D^2  ( need to know D - should use 10mm as it's a good value)
   all measurments are in mili meters
   return value is calibrated as in 1/100ths of mm 
*/

int16_t ir2d(uint16_t ir)
{
   return(sqrt(ir_k/ir));
}

/* calibrate distance sensor arrangement we must determine the constant k
   must be performed with the distance held at CALIBRATE_GAP is
   distance in 100ths of millimeters
*/
ir_cal()
{
   // this is a test
   ir_k=200*CALIBRATE_GAP*CALIBRATE_GAP;
   return;
   if(move_diff>50)
      ir_k=move_diff*CALIBRATE_GAP*CALIBRATE_GAP;
   return;
}

float sr;
#define PERIOD ((64*256)/MCLOCK)
void init_weld()
{
   cur=ME_TOP;  // menu level
   //   RShunt= .0056;
   ir_k=4100000000.0;
   rShunt=0.34;
   state=WWAIT;
   CALIBRATE_GAP=4000;
   setzero();
   distance=35;
   weld_count=0;
   max_time=1200000;
   min_time=400000;
   melt_time=10000;
   SWITCH_DDR&=~_BV(SWITCH_BIT);
   SWITCH_PORT|=_BV(SWITCH_BIT);
   BUTTON_DDR&=~_BV(BUTTON_BIT);
   BUTTON_PORT|=_BV(BUTTON_BIT);
   SELECT_DDR&=~_BV(SELECT_BIT);
   SELECT_PORT|=_BV(SELECT_BIT);
   RELAY_DDR|=_BV(RELAY_BIT);
   WELDOFF;
   IRTX_DDR|=_BV( IRTX_BIT);
   IRRX_DDR&=~_BV( IRRX_BIT);
   //TCCR2= T2_CK64;  // normal mode no output
   //TIMSK|=_BV(TOIE2);  // enable timer 2 overflow interrupt enable
   return;
}

/* this is the movement measuring welding loop
   * the inputs are the 3 adc samples 
   * the v and i circuits are equal and reasonably linear in responce
   * it measures the resistance of the link about to be welded and
   * triggers the cutoff when metal collapse is detected by the movement sensor 
   * analogue sample rate needs to be at ~3000hz to 
   * operate correctly 
*/

void weldlink(uint16_t vs,uint16_t is,uint16_t move)
{
   static int16_t lcd=0;
   static int16_t move_start;
   static int16_t move_on;
   static int16_t move_off;
   static uint32_t this_time;
   er='T';
   uint32_t mytime=getus();
   // do the distance measurment
   static uint16_t ircnt=0;
   if(bit_is_set(IRTX_PIN,IRTX_BIT)){
				       move_on+=move;
				    }
   else
   {
      move_off+=move;
   }
   ircnt++;
   if(ircnt>=LOOPS){
		      ircnt=0;
		      move_diff=move_on-move_off;
		      gap=ir2d(move_diff);
		      //capture(distance,is);
		      if(state==IRCAL){   // distance sensor calibration request received
					 if(mytime>1000000){
							      ir_cal();
							      state=WWAIT;
							      setzero();
							   }
				      }
		      if(bit_is_set(IRTX_PIN,IRTX_BIT)){
							  move_off=0;
							  IRTX_PORT&=~_BV(IRTX_BIT);
						       }
		      else
		      {
			 move_on=0;
			 IRTX_PORT|=_BV(IRTX_BIT);
		      }
		   }
   else {
	   return;
	}
   // process the voltage samples to determine the current and voltage
   //vs-=is;  // voltage is in series with current so must subtract them
   vs=(is>vs)?0:vs-is;
   float v=vs*VFAC;
   float i=is*IFAC;
   if(i>maxi) maxi=i;
   if(v>maxv) maxv=v;
   if(state==WON || state==WMELT)
   {
      capture(vs,is);
      if(v>2.5){   //  safety cuttoff - open circuit is detected.
		  state=WOFF;
		  RED_ON;
		  er='V';
	       }
      if(move_diff<IRSIG*LOOPS){   //  safety cuttoff - ir rx-tx not coupled.
				  state=WOFF;
				  RED_ON;
				  er='C';
			       }
      if(move>IRMAX){   //  safety cuttoff - IR RX overloaded posible arc flash
		       state=WOFF;
		       RED_ON;
		       er='A';
		    }
      if((mytime>this_time)){   // correct weld completion GREEN indication
			       er='T';
			       state=WOFF;
			       GREEN_ON;
			    }
      if((mytime>max_time)){   // if  weld goes over max_time stop RED&GREEN indication
			      state=WOFF;
			      RED_ON;
			      GREEN_ON;
			      er='>';
			   }
   }

   if(state==WON)
   {
      // determine the initial resistance configuration of the link being welded
      // measure the current and voltage across the link - wait for the
      // current to start flowing.
      if(mytime<min_time-40000){
				  move_start=gap; // reset min - max points
			       }
/* during the premelt period the welding jig should not be moving much yet
	 but the arms are in the operational position. 
	 with our IR distance sensor to help us 
	 set a consistant trigger point, just before the metal begins to melt
	 we record the electrode distance 
*/
      else
	 if(mytime<min_time){
			       move_start=gap;
			    }
      else
      {
	 if(gap+distance<move_start){   // movement detected melting point is reached
				       this_time=mytime+melt_time;
				       state=WMELT;
				    }
      }
   }
   if(state==WOFF){  // welding has just been turned off
		     move_start=0;
		     WELDOFF;
		     weld_time=mytime;
		     setzero();
		     weld_stat();
		     weld_show();
		     state=WWAIT;
		     return;
		  }
   if(state==WWAIT)
   {
      if(i>10)hold_time=mytime;  // measure time for current to stop

      // when measure noise function is activated measures noise on the IR detector.
      // prevent another weld for 5 seconds after the last one
      // this allows a safety factor for user accidentley restarting welding
      // and time for user to see if last weld indication was green or red
      if(mytime>5000000)
      {
	 if(move>IRMAX){  // see if irrx is saturated indicate red if so
			  RED_ON;
		       }
	 else {
		 RED_OFF;
	      }
	 // make sure we have a sufficiently high IR coupling before proceding
	 // indicate green if we are ok to go
	 if(move_diff<IRSIG*LOOPS)
	 {  GREEN_OFF;
	 }else
	 {
	    GREEN_ON;
	    if((posi!=SWON))   // foot toggle has changed
	    {
	       move_start=move_diff;  // record electrode starting position
	       weld_count++;  // remember number of welds we have done
	       posi=SWON;
	       maxi=0;
	       maxv=0;
	       WELDON;
	       RED_OFF;
	       GREEN_OFF;
	       state=WON;
	       this_time=max_time;
	       setzero();
	    }
	    if(BUTTON){
			 state=IRCAL;
			 setzero();
			 // distance sensor calibration request in progress
			 // both lights off to reduce current fluctuation
			 RED_OFF;
			 GREEN_OFF;
		      }
	 }
      }
   }
}
#endif

