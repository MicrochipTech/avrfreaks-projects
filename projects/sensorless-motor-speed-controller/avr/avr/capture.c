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
// digital waveform capture routines
// can records times between calls
// and also for data capture from adc or other sources
// usefull for debugging time critical stuff
#include <avr/io.h>
#include ATMEGA
#include <inttypes.h>
#include <math.h>
#include <avr/timer.h>
#include <avr/interrupt.h>
#include <avr/signal.h>
#include "common.h"
#include "capture.h"

#define capmax 1
#ifdef CAP_CODE

#define CAP_BUF_SIZE             8
#define CAP_BUF_MASK             ( CAP_BUF_SIZE - 1 )
struct xy{
      uint16_t v1,v2;
};
struct xy cap_buf[CAP_BUF_SIZE];
//uint16_t
static uint8_t cpnt=0;
uint8_t capmode;
volatile uint8_t cap_head;
volatile uint8_t cap_tail;

void capture(uint16_t val,uint16_t v2)
{
   uint8_t tmp_head;
   tmp_head = ( cap_head + 1 ) & CAP_BUF_MASK;
   if( tmp_head == cap_tail )
      return;
   cap_buf[ cap_head = tmp_head ].v1 = val;
   cap_buf[ cap_head = tmp_head ].v2 = v2;
}

// mode 0 fill and stop
// mode 1 wrap around
void capture_mode(uint8_t mode)
{
   capmode=mode;
}

void caps( void )
{
   uint8_t                 tmp_tail;
   if( cap_head == cap_tail )
   {
      return;
   }
   cap_tail = tmp_tail = ( cap_tail + 1 ) & CAP_BUF_MASK;
   syncwait();
   put_i16(cap_buf[tmp_tail].v1);
   putc(',');
   put_i16(cap_buf[tmp_tail].v2);
   put_cr();
   syncwait();
}

void capture_reset()
{
   cap_head = cap_tail = 0;
   cpnt=0;
   return;
}

void capture_print()
{

/*
      uint8_t x,y,w;

      switch( capmode)
      {
      case 1:
      puts("rol=");
      x=cpnt;
      w=0;
      {
      for(y=0;y<capmax;y++){
      if(++w>32){
      w=0;
      put_cr();
      }
      put_i16(cap_buf[x]);
      cap_buf[x]=0;  // clear
      if(++x>=capmax)x=0;
      putc(',');
      }
      put_cr();
      }
      break;

      case 0:
      puts("cap=");
      {
      for(x=0;x<cpnt;x++){
      if(x!=0)putc(',');
      put_i16(cap_buf[x]);
      }
      putc(' ');
      puts(utoa(cpnt,&buf,10));
      puts("vs");
      put_cr();
      cpnt=0;
      }
      break;
      default:
      break;
      }
*/
   return;
}


// digital waveform capture this records time between calls in us
static uint32_t caplasttime=0;
inline int16_t capture_time()
{
   uint32_t time=TCNT1+(((uint32_t)mainclock)<<16);
   uint16_t dt=(time-caplasttime)/MCLOCK;
   //uint16_t dt=((time>>4)-(caplasttime>>4));
   caplasttime=time;
   return(dt);    // we now have our pulse width in us
}
/*
   void capture(int16_t val)
   {
   cap_buf[cpnt]=val;
   switch (capmode){
   case 0:
   if( cpnt < capmax) cpnt++;
   break;
   case 1:
   cpnt++;
   if( cpnt >= capmax) cpnt=0;
   break;
   default:
   break;
   }
   return;
   }
*/
inline void capture1()
{
//   capture(capture_time());
}

inline void capture0()
{
//   capture(-capture_time());
}
#else
void capture_print()
{};
#endif

