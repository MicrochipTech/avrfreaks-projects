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

#ifdef TIMER
/* this file contains initialisation and procedures specific to a
   process controller and timer my main objective was to provide a user friendly
   multifeatured process timing and temperature controller with an LCD display.
   can be used as an alarm clock , yoghurt culture timer,
   brewing timer and thermal controller, or a simple termerature controller
   countdown timer to trigger an event at some point in the future
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
#define HEATER_DDR DDRC
#define HEATER_PIN PINC
#define HEATER_PORT PORTC
#define HEATER_BIT 0

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
#define HEATER_DDR DDRC
#define HEATER_PIN PINC
#define HEATER_PORT PORTC
#define HEATER_BIT 0

#define SWON bit_is_set(HEATER_PIN,HEATER_BIT)
#define SWOFF bit_is_clear(HEATER_PIN,HEATER_BIT)
#define BUTTON bit_is_set(BUTTON_PIN,BUTTON_BIT)

#define HEATER_ON HEATER_PORT|=_BV(HEATER_BIT)
#define HEATER_OFF HEATER_PORT&=~_BV(HEATER_BIT)

char tmp[12];
int32_t sectime;  // accumulates cpu clocks untill 1 second worth have expired
int32_t seconds;  // second clock updated very timer 1 interrupt (~200hz)

int8_t cur;  // current selection state

static uint8_t bit=0;
uint16_t max_temp;
uint16_t min_temp;
int32_t hold_time;
int32_t off_time;
int32_t alarm_time;

enum{  PSET,PON,POFF  }state;
char title[]={  "Thermo Timer"  };


// update the second and cpu clock counters
SIGNAL (SIG_OVERFLOW1)
{
   mainclock++;  // update main cpu clock counter
   sectime+=(65536); // the value added is the cpu clocks for each time this is called
   if(sectime>CLOCK){
		       seconds++;
		       sectime-=CLOCK;  // keep the remainder of the count
		    }
}

// a unix style time function
uint32_t time()
{
   return seconds;
}

set_time(uint32_t n)
{
   seconds = n;
}

char *print2(short z,char* y)
{
   itoa(z,y,10);
   if(y[1]==0){
		 y[1]=y[0];
		 y[0]='_';
	      }
   y+=2;
   return y;
}

// simplified and reduced size version of strtol
int32_t strtol2(char *z)
{
   int32_t q=0;
   char neg=0;
   while(*z!=0){
		  if(*z>='0' && *z<='9'){
					   q*=(int32_t)10;
					   //q=(uint32_t)q<<1+(uint32_t)q<<3;
					   q+=*z-'0';
					}
		  if(*z=='-')neg=1;
		  z++;
	       }
   if(neg)q=~q;
   return q;
}

//show the time return a string with the time in days hours minutes and seconds
char *ctime(uint32_t z)
{
   char *y=buf;
   short days,hours,minutes,seconds;
   seconds=z%60;
   z/=60;
   minutes=z%60;
   z/=60;
   hours=z%24;
   z/=24;
   days=z;
   itoa(days,y,10);
   while(*y!=0)y++;
   *y++='d';
   y=print2(hours,y);
   *y++='h';
   y=print2(minutes,y);
   *y++='m';
   y=print2(seconds,y);
   *y++='s';
   return (buf);
}


// this is the timer initialisation
void startup()
{
   puts(title);
   put_cr();
   syncwait();
   init_quad();    // my control quaderature endoder control dial
   init_timer();
   init_adc();
#ifdef LCD_CODE
   lcd_init();
   lcd_title(title);
#endif
   return;
}

// serial port and eeprom boot commands processor
void timer_com( char **z)
{
   char *dummy;
   char y=**z;
   (*z)++;
   switch (y)
   {
      case 'p':
	 timer_print();
	 break;
      case 'z':
	 init_timer();
	 break;
      case 's':   // set movement detection alarm_time factor
	 if('='==(**z)){
			  alarm_time=strtol2((*z)+1);
			  //alarm_time=strtol((*z)+1,0,10);
		       }
      case 'h':   // set movement detection alarm_time factor
	 if('='==(**z)){
			  hold_time=strtol2((*z)+1);
		       }
	 break;
      case '>':   // set maximum temperature
	 if('='==(**z)){
			  min_temp=strtol2((*z)+1);
		       }
	 break;
      case '<':   // set minimum temperature
	 if('='==(**z)){
			  max_temp=strtol2((*z)+1);
			  //max_temp=strtol2((*z)+1,0,10);
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
	  ME_CLOCK,
	  ME_ALARM,
	  ME_TIMER,
	  ME_SAVE,
	  ME_SHOW,
	  ME_LOAD,
    };

enum{
       GET_ALARM,
	  GET_HOLD,
	  GET_MAX,
	  GET_MIN,
	  GET_END
    };

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
   the time adjustment functions have a special feature when selected again they adjust thhe next unit in  the serie       seconds,minutes,hours  and days
*/
char unit=0;
void timer_menu()
{
#ifdef LCD_CODE
   static uint16_t pos=8;  // remember position in top level menu
   int8_t y,z,v=0;
   int32_t x;
   uint16_t pos2;
   x=quad_get();
   y=button_get();

   if(cur==ME_SHOW){
		      // weld_show();
		      if(y)cur=ME_TOP;  //return
		      return;
		   }

   if(cur==ME_TOP)
   {
      unit=0;
      if(x>=0){
		 pos+=x;
	      }else{
		      if(pos>(-x)){  pos+=x;
				  }else{  pos=0;
				       }
		   }
      if((pos>>2)>(xpos-8-(LCDCOLS>>1)))pos=(xpos-8-(LCDCOLS>>1))<<2;    // prevent user scrolling past end of menu
   }
   //  allow hours minutes seconds and days adjustment rates
   if(unit==1)x*=60;
   if(unit==2)x*=3600;
   if(unit==3)x*=86400;
   pos2=pos>>2;

   menu_start(pos2);
   if((menu_add(pos2,"start",0,0,0)==2) && y)
   {
      state=POFF;
      RED_ON;
      GREEN_ON;
   }
   data_clr(5);


   if((menu_add(pos2,"stop",0,0,0)==2) && y)
   {
      RED_OFF;
      off_time=hold_time+time();
      state=POFF;
      GREEN_OFF;
   }
   data_clr(5);


   z=menu_add(pos2,"load",0,0,0);
   if(z==2 && y )efile_exec();


   if((2==menu_add(pos2,"save",0,0,0)) && y )timer_save();
   data_clr(5);


   if(cur==ME_CLOCK)   {  v=1;    seconds+=x;    x=0;
			  if(seconds<0)seconds=0;
		       }else {  v=0;  }
   z=menu_add(pos2,"clock_setti",v,0,0);
   if(z!=0)menu_add(0,ctime(seconds),2,12,1);
   if(z==2 && y) cur=ME_CLOCK;
   if(y && v){
		unit++;
		if(unit>3)cur=ME_TOP;  // exit up
	     }

   if(cur==ME_TIMER)   {  v=1;    hold_time+=x;    x=0;
			  if(hold_time<0)hold_time=0;
		       }else {  v=0;  }
   z=menu_add(pos2,"timer_set",v,0,0);
   if(z!=0)menu_add(0,ctime(hold_time),2,10,1);
   if(z==2 && y) cur=ME_TIMER;
   if(y && v){
		unit++;
		if(unit>3)cur=ME_TOP;  // exit up
	     }


   if(cur==ME_ALARM)   {  v=1;    alarm_time+=x;    x=0;
			  if(alarm_time<0)alarm_time=0;
		       }else {  v=0;  }
   z=menu_add(pos2,"alarm_set",v,0,0);
   if(z!=0)menu_add(0,ctime(alarm_time),2,10,1);
   if(z==2 && y) cur=ME_ALARM;
   if(y && v){
		unit++;
		if(unit>3)cur=ME_TOP;  // exit up
	     }


   //MIN
   if(cur==ME_MIN)   {  v=1;    min_temp+=x;    x=0;
		     }else {  v=0;  }
   z=menu_add(pos2,"min_tp",v,0,0);
   if(z!=0)menu_add(0,itoa(min_temp,&buf,10),2,5,1);   // draw min_temp if visible
   if(z==2 && y) cur=ME_MIN;
   if(y && v)cur=ME_TOP;  // exit up


   //MAX
   if(cur==ME_MAX){  v=1;   max_temp+=x;     x=0;
		  }else {  v=0;  }
   z=menu_add(pos2,"max_tp",v,0,0);
   if(z)menu_add(0,itoa(max_temp,&buf,10),2,5,1);   // draw max_temp if visible
   if(z==2 && y) cur=ME_MAX;
   if(y && v)cur=ME_TOP;   //exit up

   menu_add(pos2,"the end",0,0,0);

/*
      // on the end of the menu add the timer print function it's a real cpu chewer
      char h;
      for(h=0;h<GET_END;h+=2){
      if(data_visible(pos2,10)){
      data_add(timerget(h),10,0);
      data_add(timerget(h+1),10,1);
      }
      }
*/
#endif
}

void timer_print()
{
   equal("ti");
   put_i32(seconds);
   syncwait();
   char x;
   for(x=0;x<GET_END;x++){
			    puts(timerget(x));
			    putc(' ');
			 }
   put_cr();
   return;
}

// this function returns strings describing various weld parameters
// the 3 preceding characters identify the variable ie w>=
char *timerget(char val)
{
   uint32_t tmp;
   buf[0]='t';
   buf[2]='=';
   buf[3]=0;
   switch(val){
		 case GET_MIN:
		    buf[1]='>';
		    ltoa(min_temp,&buf[3],10);
		    break;
		 case GET_MAX:
		    buf[1]='<';
		    ltoa(max_temp,&buf[3],10);
		    break;
		 case GET_HOLD:
		    buf[1]='h';
		    ltoa(hold_time,&buf[3],10);
		    break;
		 case GET_ALARM:
		    buf[1]='s';
		    ltoa(alarm_time,&buf[3],10);
		    break;
		 default:
	      }
   return &buf;
}


timer_save()
{
   char x;
   for(x=0;x<=GET_END;x++){
			     efile_write(timerget(x),1);
			  }
}

float maxi,maxv;
void timer_show()
{
#ifdef LCD_CODE
   buf[0]=0;
   strcat(buf,ltoa(seconds,&tmp,10));
   lcd_write_string (0,0,buf,8);
   buf[0]=0;
   strcat(buf,ltoa(mainclock,&tmp,10));
   lcd_write_string (8,1,buf,8);
#endif
}

float sr;
#define PERIOD ((64*256)/MCLOCK)
void   init_timer()
{
   cur=ME_TOP;  // menu level
   state=POFF;
   seconds=0;
   max_temp=46;
   min_temp=43;
   hold_time=900;

   SELECT_DDR&=~_BV(SELECT_BIT);
   SELECT_PORT|=_BV(SELECT_BIT);
   HEATER_DDR|=_BV(HEATER_BIT);
   HEATER_OFF;

   // setup timer 1
   // 16bit normal mode operation
   TCCR1A &=~_BV(WGM11);
   TCCR1A &=~_BV(WGM10);
   TCCR1B =  T1_CK1;
   TCCR1B &=~_BV(WGM12);
   TCCR1B &=~_BV(WGM13);
   TIMSK|=_BV(TOIE1);
   //TIMSK|=_BV(OCIE1A);
   //TIMSK|=_BV(OCIE1B);  // enable compare interrupt
   cbi(TIMSK,TICIE1);  // dis able input capture
   ICR1=~0;
/* enable interrupts */
}

/* this is the process timing control loop
   it uses the inimum and maximum temperature to set the pulse width of the 
   relay output  analogue sample rate needs to be at ~3000hz to
   * operate correctly
*/

void timer_loop(uint16_t temperature)
{
   static int16_t count=0;
   static uint32_t this_time;
   static width=0;
   count++;
   if(count>=100)count=0;

   if(state==POFF)
   {
      HEATER_OFF;
      RED_OFF;
   }
   if(state==PON )
   {
      // work out the pulse width in part per hundred
      if( temperature< min_temp)width=100;
      else if( temperature> max_temp)width=0;
      else {
	      width=((temperature-min_temp)*100)/(min_temp-max_temp);
	   }

      if(count<width){
			HEATER_ON;
			RED_ON;
		     }else{
			     HEATER_OFF;
			     RED_OFF;
			  }

   }
   if(state==PON)      if(time()>off_time)state=POFF;

}
#endif

