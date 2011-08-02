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
#include <util\delay.h>
#include "LcdLowLevel.h"
#include "LcdFont.h"

/*** lcdCh *******************************************************************
 *	
 *	Write predefined fonts to screen at set location
 *	
 ***************************************************************************/
void LCD_char(uint8_t* fontSet, uint8_t ch,uint8_t scale,uint8_t x,uint8_t y,uint8_t mode) 
{
	uint8_t i, j;
	uint8_t k  = pgm_read_byte(fontSet);
	uint8_t xx = pgm_read_byte(fontSet+1);
	uint8_t yy = pgm_read_byte(fontSet+2);

	uint8_t px=xx*scale;
	uint8_t py=yy*scale;


	if(ch < k) 
	{
		for(i=0; i<px; i++) 
		{
			for(j=0; j<py; j++) 
			{
				if(pgm_read_byte(fontSet+(xx*ch+3)+(i/scale)) & _BV((j/scale))) 
					LCD_setPixelXY(x,y+j,mode);
			}
			x++;
		}
	}
	else 
	{
		x+=scale;
	}
}


/*** lcdWrite *****************************************************************
 *	
 *	Translate input to selected font, Write to display at set location
 *	
 **************************************************************************/
void LCD_drawString(uint8_t fontSet, uint8_t* buf, uint8_t is_pstr,uint8_t scale,uint8_t x,uint8_t y,uint8_t mode) 
{
	uint8_t i=0, offset, ch;
	uint8_t xx = pgm_read_byte(fontSet+1);

	// check if string is in flash/sram and read appropriately
	if(is_pstr) ch = pgm_read_byte(buf+i);
	else ch = buf[i];


	do 
	{
		if(ch > 32) 
		{
			offset = 33;
			
			if( (ch>96) && (ch<123) ) offset = 65;
			else if(ch>122) offset = 60;
	
			LCD_char(fontSet, ch-offset,scale,x,y,mode);
		}
		else 
		{
			LCD_char(fontSet, 255,scale,x,y,mode);	// empty (not recognized)
		}
		x+=xx*scale+1*scale; // char spacing
		
		i++;
		
		// check if string is in flash/sram and read appropriately
		if(is_pstr) ch = pgm_read_byte(buf+i);
		else  ch = buf[i];


	} while(ch != '\0');
}
