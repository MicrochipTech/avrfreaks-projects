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
// several handy debuging type routines
#ifdef DEBUG

#include <avr/io.h>
#include <inttypes.h>
#include <math.h>
#include <avr/timer.h>
#include <avr/interrupt.h>
#include <avr/signal.h>
#include "fail.h"
#include "uart.h"
#include "common.h"

#define DELAY 8

void  fail (uint8_t code)
{
   //  puts("this is the system error message");
/*
      cli();  // disable interrupts
      uint32_t x,c;
      failure:
      PORTC=0;
      PORTD=0;
      for(x=0;x<270*DELAY;x++)
      {
      for(c=0;c<270*DELAY;c+=DELAY){
      if(c>x){  PORTC=255; PORTD=255;  }
      if(c<x){  PORTC=0;  PORTD=0;  }
      }
      }
      for(c=0;c<8;c++)
      {
      if(((1<<c) & code) == (1<<c)){
      for(x=0;x<20000*DELAY;x++){  PORTC=255; PORTD=255;  }
      } else {
      for(x=0;x<10000*DELAY;x++){  PORTC=255; PORTC=0; PORTD=255; PORTD=0;  }  //~PINC; PORTD=~PIND;  }
      }
      for(x=0;x<20000*DELAY;x++){  PORTC=0; PORTD=0;  }
      }
      for(x=0;x<30000*DELAY;x++){  PORTC=0; PORTD =0;  }
      goto failure;
*/
}


// prints out contents of the registers
void dump()
{
   uint8_t x;
   uint8_t *pt=&R0;
   //for(x=0;x<32;x++){
   equal("r0-26");
   for(x=0;x<26;x++){
		       put_i16(*pt);
		       pt++;
		       putc(',');
		    }
   put_cr();
   equal("sp");
   put_i16(SP);
   equal("x");
   put_i16(RX);
   equal("y");
   put_i16(RY);
   equal("z");
   put_i16(RZ);
   put_cr();
}

// prints out contents of the registers
void dump2()
{
   uint8_t x;
   uint8_t *pt=32;
   //for(x=0;x<32;x++){
   equal("r0-$3F");
   for(x=0;x<64;x++){
		       putc('$');
		       puts(itoa(x,&buf,16));
		       putc('=');
		       putc('\t');
		       putc('$');
		       puts(itoa(0xff&(*pt),&buf,16));
		       putc('\t');
		       putc('%');
		       put_bin(*pt);
		       pt++;
		       put_cr();
		    }
}
/*
   void mem()
   {
   char *pt;
   puts("mem=");
   for(pt=96;pt<96+1023;pt++){
   if(*pt>='a' && *pt<='Z' )
   putc(*pt);
   syncwait();
   }
   }
   }

*/
#endif

