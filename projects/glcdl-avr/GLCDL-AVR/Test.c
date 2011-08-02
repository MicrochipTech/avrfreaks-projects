/*
    This file is part of GLCDL-AVR - Copyright 2004 Lorenz Christophe
    christophelorenz magicathere gmail.com
    https://sites.google.com/site/donutscience

    GLCDL-AVR is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    any later version.

    GLCDL-AVR is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
    
    This project is under LPGL, and you are free to donate if you find it usefull.
    https://www.paypal.com/cgi-bin/webscr?cmd=_donations&business=W6HV9YP7GK4S8&lc=BE&item_name=Donut%20Science&currency_code=EUR&bn=PP%2dDonationsBF%3abtn_donateCC_LG%2egif%3aNonHosted
    
*/
#include <stdlib.h>
#include <avr\io.h>
#include <avr\interrupt.h>
#include <avr\eeprom.h>
#include <avr/pgmspace.h>
#include <util\delay.h>

#include "i2cmaster.h"
#include "LcdLowLevel.h"
#include "LcdBasicDraw.h"
#include "LcdAdvancedDraw.h"


#include "font_ttuf0.h"
#include "font_ttuf1.h"
#include "font_ttuf2.h"
#include "LcdFont.h"

void long_delay_ms(uint16_t t);

void testFont()
{
	uint8_t i;
	// load a pointer to the font
	uint8_t* font = (uint8_t*)fontSet1;
//	uint8_t* font = (uint8_t*)fontSet2;

	uint8_t fontSize = pgm_read_byte(font+0);	// chars in this font
	uint8_t fontX	 = pgm_read_byte(font+1);	// font width
	uint8_t fontY	 = pgm_read_byte(font+2);	// font hieght

	uint8_t x,y,scale;
	x=2; y=2;
	scale=1;

	for(i=0; i<fontSize; i++) 
	{
		LCD_char(font, i,scale,x,y,LCD_SET);	// write a char from the fonSet to the frameBuffer.
							//	not ascii, use lcdWrite to print ascii
		
		x+=fontX*scale+1*scale;		// give 1 pixel space b/w letters
		
		// if next char will go over the edge, go to start of next line
		if((x+fontX) > BUFFER_WIDTH) {
			x = 0;
			y+= fontY*scale + 1*scale;	// give one pixel space b/w lines
			if (y>BUFFER_HEIGHT) break;
		}
	}

	x=2;
	y=20;	

// write null-terminated text to screen buffer
	LCD_drawString(font, (uint8_t*)PSTR("Hello \0"), 1,1,x,y,LCD_SET);		// 1 - string read directly from flash
	x=2;
	y=30;	

	LCD_drawString(font, (uint8_t*)"World!\0", 0,1,x,y,LCD_SET);				// 0 - string loaded to ram
	

}



void test()
{
	uint8_t i;
	LCD_clearFrameBuffer();
	LCD_displayFrameBuffer();

//	long_delay_ms(1000);

	i=0;
	while(i<96)
	{
		LCD_setPixelXY(i,(i*39)/95,LCD_SET);
		i++;
	}

	LCD_drawLine(0,39,95,0,LCD_SET);

	LCD_drawCircle(96/2,40/2,15,LCD_SET);

	LCD_drawOval(96/2,40/2,96/2,40/2,LCD_SET);
	LCD_drawARCorrectCircle(96/2,40/2,20,LCD_SET);

	LCD_drawRect(0,39,95,0,LCD_XOR);
	LCD_drawRect(2,2,10,10,LCD_SET);

	LCD_fillRect(40,40,20,20,LCD_SET);

	LCD_fillARCorrectCircle(60,15,12,LCD_SET);
	
	fillTriangle(20,3,40,10,30,20,LCD_SET);


	LCD_clearFrameBuffer();
	testFont();

	LCD_displayFrameBuffer();
}


int main(void) 
{
	DDRC=0xFF;

	DDRB=0xFF;
	PORTB=~0xF;

    sei();

    i2c_init();                             // initialize I2C library
	LCD_Init();

	test();

	while(1) _delay_ms(1);
}

