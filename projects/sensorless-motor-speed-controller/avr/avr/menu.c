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

/* routines to help implement a user interface with a quadrature encoder
   wheel, one button and an LCD to
   allow user to scroll horizontally through items 
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
#include "avr.h"

// select button.
#define SELECT_DDR DDRA
#define SELECT_PIN PINA
#define SELECT_PORT PORTA
#define SELECT_BIT 2

#define SELECT bit_is_clear(SELECT_PIN,SELECT_BIT)

#ifdef LCD_CODE
/* implement a user interface with a quadrature encoder wheel and one button
   and LCD to
   allow user to change minimum & maximum weld time , and hold time.
   movement trip distance,
   calibration distance and start calibration function
   save current settings to eeprom , clear eeprom
*/

// returns a 1 if the button has been press since the last call
char button_get()
{
   static char now;
   if(now==0 && SELECT){
			  now=1;
			  return 1;
		       }
   if(now==1 && !SELECT) now=0;
   return 0;
}

int8_t xpos=0;

// handy horizontal scrolling menu function start menue line function
int8_t menu_start(unsigned char p)
{
   char x;
   xpos=(LCDCOLS>>1)-1;
   if(p<xpos)lcd_move(0,0);
   for(x=0;x<xpos-p;x++)lcd_put(' ');
}

char data_pos;

// handy horizontal scrolling menu function
// if the menu item is presently centered for selection
// this will return a 2 for that item
// pos is the present cursor position
// allows a scrolling menu upto 120 characters wide
// if menu item is visible will return 1
// if sel is 1 then the menue will highlight the item
// as selected.
// if sel is 2 no border is added just the char *fun
// the item is added in the same columns as the last item
//
// the lcd line to write to is line
char menu_add(char p,char *fun,char sel,int8_t size2,char line)
{
   char x,q=0;
   char ret=0;
   char ind=' ';
   char size;
   char ind2=ind;
   for(size=0;fun[size]!=0;size++);
   size++;  // leave room for space after item
   int8_t where;
   if(sel==2){
		size=size2;
		where=data_pos;
	     }else{
		     where=(xpos-p);
		     data_pos=where;
		     xpos+=size+1;  // add menu item width plus seperator space
		  }
   // draw item seperator
   // if item is off screen return 0
   if(where>LCDCOLS || where<(-size-2)){
					  return 0;
				       }

   // check if item is in selection zone
   if(where<8 && (xpos-p)>=8){
				ret=1;
				if(sel==1){
					     ind2=ind='*';
					  } else
				   if( sel==0)
				   {  ind='<';
				      ind2='>';  }
			     } // Item is at selection point

   for( x=0;x<=size;x++)
   {
      if(where>=0 && where <LCDCOLS){  // if it's visible then draw it
				       if(!q){  lcd_move(where,line);
						q=1;  }
				       if(x==0 ){
						   lcd_put(ind);
						}else
				       {

					  if(*fun!=0){
							lcd_put(*fun);
						     }else{
							     lcd_put(ind2);
							  }
				       }
				    }
      if(*fun!=0 && x!=0)fun++;  // dont increment if end of string or first character
      where++;
   }
   return(q+ret);
}
// handy advisary horizontal scrolling menu function
// allocates the given len of menu if the len is visible at all
// it returns a 1 otherwise it allocates the space and returns zero
char data_visible(char p,char len)
{
   int8_t where;
   where=(xpos-p);
   data_pos=where;
   xpos+=len+1;  // add menu item width plus seperator space
   // if item is off screen return 0
   if(where>LCDCOLS || where<(-len-2)){
					 return 0;
				      }
   return(1);
}

/*
   // will add the fun string to the line below the position of the menu item last drawn
   // will write size characters
   void data_add(char *fun,int8_t size,char line)
   {
   char x,q=0;
   int8_t where=data_pos;
   for( x=0;x<=(size);x++)
   {
   if(where>=0 && where <LCDCOLS){  // if it's visible then draw it
   if(!q){  lcd_move(where,line);
   q=1;  }
   if(x==0){   // draw item seperator
   lcd_put(' ');
   }else
   {
   if(*fun!=0){
   lcd_put(*fun);
   }else{
   lcd_put(' ');
   }
   }
   }
   if(*fun!=0 && x!=0)fun++;  // dont increment if end of string or first character
   where++;
   }
   return;
   }
*/

// will clear the size characers on the line below the position of the menu item last drawn

void data_clr(int8_t size)
{
   menu_add(0,"",2,size,1);
}
#endif

