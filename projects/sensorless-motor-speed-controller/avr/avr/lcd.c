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
// this code was adapted from avr freaks website lcd.c

// note MCLOCK is the speed of the CPU in MHZ

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
#include "lcd.h"
#include <string.h>

#ifdef LCD_CODE

#define byte uint8_t

//these defines are used to describe the AVRs pinout for interface to LCD
// we can use any 7 avr pins we want

// sharp LCD instruction/data register select
#define LCD_RS_DDR DDRD
#define LCD_RS_PIN PIND
#define LCD_RS_PORT PORTD
#define LCD_RS_BIT 7

// sharp LCD read/write 0 is write 1 is read
#define LCD_RW_DDR DDRD
#define LCD_RW_PIN PIND
#define LCD_RW_PORT PORTD
#define LCD_RW_BIT 6

// sharp LCD enable operation clocking signal
#define LCD_E_DDR DDRD
#define LCD_E_PIN PIND
#define LCD_E_PORT PORTD
#define LCD_E_BIT 5

/*
   // sharp LCD data 0-3 pins not used for 4 bit mode
   #define LCD_D0_DDR DDRD
   #define LCD_D0_PIN PIND
   #define LCD_D0_PORT PORTD
   #define LCD_D0_BIT 3

   // sharp LCD data not used for 4 bit mode
   #define LCD_D1_DDR DDRD
   #define LCD_D1_PIN PIND
   #define LCD_D1_PORT PORTD
   #define LCD_D1_BIT 3

   // sharp LCD data not used for 4 bit mode
   #define LCD_D2_DDR DDRD
   #define LCD_D2_PIN PIND
   #define LCD_D2_PORT PORTD
   #define LCD_D2_BIT 3

   // sharp LCD data not used for 4 bit mode
   #define LCD_D3_DDR DDRD
   #define LCD_D3_PIN PIND
   #define LCD_D3_PORT PORTD
   #define LCD_D3_BIT 3
*/

// sharp LCD data bits 4-7 are used 4 bit mode
#define LCD_D4_DDR DDRC
#define LCD_D4_PIN PINC
#define LCD_D4_PORT PORTC
#define LCD_D4_BIT 4

// sharp LCD data bits 4-7 are used 4 bit mode
#define LCD_D5_DDR DDRC
#define LCD_D5_PIN PINC
#define LCD_D5_PORT PORTC
#define LCD_D5_BIT 5

// sharp LCD data bits 4-7 are used 4 bit mode
#define LCD_D6_DDR DDRC
#define LCD_D6_PIN PINC
#define LCD_D6_PORT PORTC
#define LCD_D6_BIT 6

// sharp LCD data bits 4-7 are used 4 bit mode
#define LCD_D7_DDR DDRC
#define LCD_D7_PIN PINC
#define LCD_D7_PORT PORTC
#define LCD_D7_BIT 7

// wait for the given time in cpu clocks 16 bit version
// gets time count by using polling of TCNT1 which runs at clock rate of CPU
void lcd_wait(uint16_t z)
{
   uint16_t now=TCNT1;
   uint16_t tmp=0;
   uint16_t last;
   do{
	z-=tmp;
	last=now;
	now=TCNT1;
	tmp=(now-last);
     }
   //   while(z>tmp);
   while(0);
   return;
}
/*
   // wait for the given time in cpu clocks 32 bit version
   // gets time count by using polling of TCNT1 which runs at clock rate of CPU
   void lcd_wait32(uint32_t z)
   {
   uint16_t now=TCNT1;
   uint16_t tmp=0;
   uint16_t last;
   do{
   z-=tmp;
   last=now;
   now=TCNT1;
   tmp=(now-last);
   }
   while(z>tmp);
   return;
   }
*/

void nibble_out(byte data)
{
   if(data&1) {
		 LCD_D4_PORT|=_BV(LCD_D4_BIT);
	      }else{
		      LCD_D4_PORT&=~_BV(LCD_D4_BIT);
		   }
   if(data&2) {
		 LCD_D5_PORT|=_BV(LCD_D5_BIT);
	      }else{
		      LCD_D5_PORT&=~_BV(LCD_D5_BIT);
		   }
   if(data&4) {
		 LCD_D6_PORT|=_BV(LCD_D6_BIT);
	      }else{
		      LCD_D6_PORT&=~_BV(LCD_D6_BIT);
		   }
   if(data&8) {
		 LCD_D7_PORT|=_BV(LCD_D7_BIT);
	      }else{
		      LCD_D7_PORT&=~_BV(LCD_D7_BIT);
		   }
   LCD_E_PORT|=_BV(LCD_E_BIT);
   lcd_wait(MCLOCK);
   LCD_E_PORT&=~_BV(LCD_E_BIT);
   return;
}

// returns current cursor position
uint8_t lcd_ready ()
{  byte busy;
   uint8_t count=0;
   uint32_t timeout=0;
   uint8_t z=0;
   // Switch data lines to input mode
   LCD_D4_DDR&=~_BV(LCD_D4_BIT);
   LCD_D5_DDR&=~_BV(LCD_D5_BIT);
   LCD_D6_DDR&=~_BV(LCD_D6_BIT);
   LCD_D7_DDR&=~_BV(LCD_D7_BIT);

   LCD_D7_PORT&=~_BV(LCD_D7_BIT);      	// d7 line to low dont want pullup for this

   LCD_RS_PORT&=~_BV(LCD_RS_BIT);  // instruction mode
   LCD_E_PORT&=~_BV(LCD_E_BIT);  // ensure enable is off
   LCD_RW_PORT|=_BV(LCD_RW_BIT);  // set read mode
   do
   {
      lcd_wait(MCLOCK);
      LCD_E_PORT|=_BV(LCD_E_BIT);
      lcd_wait(MCLOCK);
      //  get upper 3 bits of curser position and check busy flag
      busy=LCD_D7_PIN&_BV(LCD_D7_BIT);
      if(bit_is_set(LCD_D4_PIN,LCD_D4_BIT))z|=16;
      if(bit_is_set(LCD_D5_PIN,LCD_D5_BIT))z|=32;
      if(bit_is_set(LCD_D6_PIN,LCD_D6_BIT))z|=64;

      LCD_E_PORT&=~_BV(LCD_E_BIT);

      lcd_wait(MCLOCK);

      LCD_E_PORT|=_BV(LCD_E_BIT);
      lcd_wait(MCLOCK);
      //  get lower 4 bits of curser position
      if(bit_is_set(LCD_D4_PIN,LCD_D4_BIT))z|=1;
      if(bit_is_set(LCD_D5_PIN,LCD_D5_BIT))z|=2;
      if(bit_is_set(LCD_D6_PIN,LCD_D6_BIT))z|=4;
      if(bit_is_set(LCD_D7_PIN,LCD_D7_BIT))z|=8;
      LCD_E_PORT&=~_BV(LCD_E_BIT);
      if( count++>20)break;
   }
   while (busy);
   return z;
}

void lcd_out (byte data)
{  byte tempdat;

   // Switch data lines to output mode
   LCD_D4_DDR|=_BV(LCD_D4_BIT);
   LCD_D5_DDR|=_BV(LCD_D5_BIT);
   LCD_D6_DDR|=_BV(LCD_D6_BIT);
   LCD_D7_DDR|=_BV(LCD_D7_BIT);

   nibble_out(data>>4);
   nibble_out(data);

   // Switch data lines to input mode
   LCD_D4_DDR&=~_BV(LCD_D4_BIT);
   LCD_D5_DDR&=~_BV(LCD_D5_BIT);
   LCD_D6_DDR&=~_BV(LCD_D6_BIT);
   LCD_D7_DDR&=~_BV(LCD_D7_BIT);
}

void lcd_write_command (byte com)
{
   lcd_ready();
   LCD_RW_PORT&=~_BV(LCD_RW_BIT);  // write mode
   LCD_RS_PORT&=~_BV(LCD_RS_BIT);  // instruction mode
   LCD_E_PORT&=~_BV(LCD_E_BIT);
   lcd_out (com);
}

/* width is the size of the area to write 
   if the text string is shorter than width - 
   the additional width is filled with spaces
   if width is <0 the line is cleared and the text is centered
*/
void lcd_write_string(char x,char y, char *text,char width)
{
   char xs;
   lcd_move(x,y);
   while(*text!=0)
   {
      lcd_put(*text++);
      width--;
   }
   // fill non text width with spaces
   while (width-->0)lcd_put(' ');
}

// clear screen and center text on line
void lcd_title(char *text)
{
   unsigned char le=0;
   lcd_cls();
   while(text[le]!=0)le++;
   lcd_move((LCDCOLS-le)>>1,0);
   lcd_puts(text);
}

void lcd_move(char x,char y)
{  byte ramadress;
   ramadress=0x40*y+x;
   lcd_write_command (ramadress|0x80);
   return;
}

void lcd_puts(char *text)
{
   while(*text!=0 )
   {
      lcd_put(*text++);
   }
   return;
}

void lcd_put(char c)
{
   lcd_ready();
   LCD_RW_PORT&=~_BV(LCD_RW_BIT);  // write=0 read=1
   LCD_E_PORT&=~_BV(LCD_E_BIT);
   LCD_RS_PORT|=_BV(LCD_RS_BIT);   // data mode
   lcd_out (c);
   return ;
}

void lcd_cls (void)
{  lcd_write_command (LCD_CLS);
}

void lcd_init (void)
{
   byte counter;
   uint16_t ct;
   for(ct=0;ct<200;ct++)lcd_wait(1000*MCLOCK); // 200ms

   // set all control lines low during LCD initialisation
   LCD_RW_PORT&=~_BV(LCD_RW_BIT);
   LCD_RS_PORT&=~_BV(LCD_RS_BIT);
   LCD_E_PORT&=~_BV(LCD_E_BIT);

   // setup data direction for control pins to LCD
   LCD_RW_DDR|=_BV(LCD_RW_BIT);
   LCD_RS_DDR|=_BV(LCD_RS_BIT);
   LCD_E_DDR|=_BV(LCD_E_BIT);

   // clock D4 and D5 high 3 times with 5ms delay between
   for (counter=0;counter<3;counter++)
   {
      nibble_out(0x03);
      for(ct=0;ct<5;ct++)lcd_wait(1000*MCLOCK);  // 5ms
   }

   //set 4 bit transfer mode
   nibble_out(0x02);
   for(ct=0;ct<50;ct++)lcd_wait(1000*MCLOCK);  // 50ms

   lcd_write_command (0x28);      //2 line mode,5x7 dots,4bit Interface
   lcd_write_command (0x0c);		//Display on, Cursor off,not  blinking
   lcd_write_command (1);		//Clear display
   lcd_write_command (0x06); 	//Entry mode increment, shift off
}
#endif

