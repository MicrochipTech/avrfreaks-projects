#include "lcd.h"
#include "tools.h"
#include "progmem.h"
#include "inttypes.h"

#define	RS		16		// RS Signal 
#define	RW		32		// RW Signal 
#define	ENABLE		64		// ENABLE Signal 

#define LCD_PORT PORTD			// LCD data port
#define LCD_DDR DDRD			// LCD port data direction register
#define LCD_PIN PIND			// LCD port input pins



//	Display connector description:
//
//
//  ============================================================================================
//  |  1  |  2  |  3  |  4  |  5  |  6  |  7  |  8  |  9  | 10 | 11 | 12 | 13 | 14 | 15  | 16  |
//  |===========================================================================================
//  | GND | VCC | Vo  | RS  | R/W |  EN | D0  | D1  | D2  | D3 | D4 | D5 | D6 | D7 | Vl+ | Vl- |
//  ============================================================================================
//
//  If you want to use the unit without changing anything you mus connect your
//  display exactly like this:
//
//  Display  	Atmel
//  connector:	controller:
//
//  1 GND	GND		
//  2 VCC	VTG or any 5V supply	
//  3 Vo	see note (*1)
//  4 RS	PD4
//  5 R/W	PD5
//  6 ENABLE	PD6
//  11 D4	PD0
//  12 D5	PD1
//  13 D6	PD2	  
//  14 D7	PD3
//
//
//  Description:
//
//  GND   =  signal ground ( GND of Atmel board )
//  VCC   =  +5V supply voltage ( VTG on Atmel board )
//  Vo    =  voltage input for display contrast  (*1)
//  RS    =  register select input 
//  R/W   =  read/write signal input
//  EN    =  enable signal input
//  D0-D7 =  data bits from D0 to D7 ( I used 4 bit data bus, so only D7 - D4 are used !!)
//  Vl+   =  positive supply for the background light (if your display has one !)
//  Vl-   =  negative supply for the background light (if your display has one !)

//  (*1)  to create the Vo voltage use a variable resistor and connect like this:
//
//         VCC  --------+
//			| _.--------- Vo  
//		       | |/|
//	               |/|
//                    /|_|  R = 10k..20k
//                      |
//         GND  --------+
//
//
//
//
//  Have a lot of fun !
//
//  Florian Hirschberg (fhirschberg@fhelectronic.de)
//
//  www.fhelectronic.de
//


static char screenpos[4][20] = {
                                {0x80,0x81,0x82,0x83,0x84,0x85,0x86,0x87,0x88,0x89,
                                 0x8A,0x8B,0x8C,0x8D,0x8E,0x8F,0x90,0x91,0x92,0x93},
                                {0xC0,0xC1,0xC2,0xC3,0xC4,0xC5,0xC6,0xC7,0xC8,0xC9,
                                 0xCA,0xCB,0xCC,0xCD,0xCE,0xCF,0xD0,0xD1,0xD2,0xD3},
                                {0x94,0x95,0x96,0x97,0x98,0x99,0x9A,0x9B,0x9C,0x9D,
                                 0x9E,0x9F,0xA0,0xA1,0xA2,0xA3,0xA4,0xA5,0xA6,0xA7},
                                {0xD4,0xD5,0xD6,0xD7,0xD8,0xD9,0xDA,0xDB,0xDC,0xDD,
                                 0xDE,0xDF,0xE0,0xE1,0xE2,0xE3,0xE4,0xE5,0xE6,0xE7}
                               };
                                 

void LCD_sendcommand (const char command) {
 while (LCD_busy());
 outp (((command >> 4) & 0x0F) | ENABLE,LCD_PORT);	// send twice because atmel is too fast 
 outp (((command >> 4) & 0x0F) | ENABLE,LCD_PORT); 	// for the LCD display !
 outp ((command >> 4) & 0x0F,LCD_PORT);
 outp ((command >> 4) & 0x0F,LCD_PORT); 
 outp ((command & 0x0F)  | ENABLE,LCD_PORT);		// send twice because atmel is too fast 
 outp ((command & 0x0F)  | ENABLE,LCD_PORT); 		// for the LCD display !
 outp ((command & 0x0F),LCD_PORT); 
 outp ((command & 0x0F),LCD_PORT);  
 outp (0x00,LCD_PORT);
}


void LCD_sendchar (const char command) {		// same as sendcommand but RS is set, too
 while (LCD_busy());
 outp ((command >> 4) | RS ,LCD_PORT);			// RS must set BEFORE enable !!!
 outp ((command >> 4) | RS  | ENABLE,LCD_PORT);		
 outp ((command >> 4) | RS  | ENABLE,LCD_PORT);
 outp ((command >> 4) | RS,LCD_PORT);
 outp ((command >> 4),LCD_PORT); 
 outp ((command & 0x0F) | RS,LCD_PORT); 
 outp ((command & 0x0F) | RS | ENABLE,LCD_PORT);
 outp ((command & 0x0F) | RS | ENABLE,LCD_PORT);
 outp ((command & 0x0F) | RS,LCD_PORT);  
 outp ((command & 0x0F),LCD_PORT); 
 outp (0x00,LCD_PORT); 
}

void LCD_sendstring (char *str) {			// sends the whole string to LCD
 while ((*str != 0) && (*str != 10)) LCD_sendchar (*str++);
}


void LCD_sendconststring (char *str) {			// sends the whole string to LCD
 char value;
 char nr = 0;
 	
 value = PRG_RDB(&str[nr]); 
 while ((value != 0) && (value != 10)) {
  LCD_sendchar (value);
  nr++;
  value = PRG_RDB(&str[nr]);
 };
}


int LCD_busy (void) {					// tests the busy bit of the LCD Display
 char input;
 input=0;
 outp (0xF0,LCD_DDR);		// unteren 4 bit eingang !!
 outp (RW,LCD_PORT);
 outp (RW | ENABLE,LCD_PORT);
 outp (RW | ENABLE,LCD_PORT);
 input = inp (LCD_PIN) << 4;
 outp (RW,LCD_PORT);
 outp (RW,LCD_PORT);
 outp (RW,LCD_PORT);
 outp (RW | ENABLE,LCD_PORT);
 outp (RW | ENABLE,LCD_PORT);
 input = input | inp (LCD_PIN);
 outp (RW,LCD_PORT);
 outp (RW,LCD_PORT);
 outp (0xff,LCD_DDR);
 outp (0x00,LCD_PORT);  
 return (input & 128)==128;				// true if busy set !
}

void LCD_init (void) {
 outp (0xFF,LCD_DDR); 
 
 pause (15);						// Wait for display power on

 outp (0x03+ENABLE,LCD_PORT);				// Send display software reset 
 outp (0x03+ENABLE,LCD_PORT);
 outp (0x03,LCD_PORT);
 outp (0x03,LCD_PORT);

 pause (5);
 
 outp (0x03+ENABLE,LCD_PORT);				// Send display software reset 
 outp (0x03+ENABLE,LCD_PORT);
 outp (0x03,LCD_PORT);
 outp (0x03,LCD_PORT);

 pause (5);

 outp (0x03+ENABLE,LCD_PORT);				// Send display software reset 
 outp (0x03+ENABLE,LCD_PORT);
 outp (0x03,LCD_PORT);
 outp (0x03,LCD_PORT);

 pause (5);
   
 outp (0x02+ENABLE,LCD_PORT);				// Set display 4 bit interface
 outp (0x02+ENABLE,LCD_PORT);
 outp (0x02,LCD_PORT);
 outp (0x02,LCD_PORT);

 pause (5);

 LCD_sendcommand (0x28);				// 2 line display, 5x8 matrix

 LCD_sendcommand (8);					// turn display off

 LCD_sendcommand (0x01);				// clear display

 LCD_sendcommand (0x06);				// set entry mode 

 LCD_sendcommand (12);					// turn display on
 							// cursor on cursor blink off
 
 pause (5);
};

void LCD_clrscr (void) {
 LCD_sendcommand (0x01);
};


void LCD_home (void) {
 LCD_sendcommand (0x10);
};

void LCD_setpos (char x,char y) {
 LCD_sendcommand (screenpos[x-1][y-1]);
};

