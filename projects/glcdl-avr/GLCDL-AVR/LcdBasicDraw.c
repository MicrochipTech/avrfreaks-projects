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
#include "LcdLowLevel.h"
#include "LcdBasicDraw.h"


void LCD_drawLine ( uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t mode )
{
    int dx, dy, stepx, stepy, fraction;

    dy = y2 - y1;
    dx = x2 - x1;

    if ( dy < 0 )
    {
        dy    = -dy;
        stepy = -1;
    }
    else
    {
        stepy = 1;
    }

    if ( dx < 0 )
    {
        dx    = -dx;
        stepx = -1;
    }
    else
    {
        stepx = 1;
    }

    dx <<= 1;
    dy <<= 1;

    LCD_setPixelXY( x1, y1, mode );

    if ( dx > dy )
    {
        fraction = dy - (dx >> 1);
        while ( x1 != x2 )
        {
            if ( fraction >= 0 )
            {
                y1 += stepy;
                fraction -= dx;
            }
            x1 += stepx;
            fraction += dy;
            LCD_setPixelXY( x1, y1, mode );
        }
    }
    else
    {
        fraction = dx - (dy >> 1);
        while ( y1 != y2 )
        {
            if ( fraction >= 0 )
            {
                x1 += stepx;
                fraction -= dy;
            }
            y1 += stepy;
            fraction += dx;
            LCD_setPixelXY( x1, y1, mode );
        }
    }
}

uint8_t clampX(uint8_t x)
{
	//if (x<0) return 0; else 
	if (x>=BUFFER_WIDTH) return BUFFER_WIDTH-1;
	return x;
}

uint8_t clampY(uint8_t y)
{
	//if (y<0) return 0; else 
	if (y>=BUFFER_HEIGHT) return BUFFER_HEIGHT-1;
	return y;
}

void LCD_fillRect(uint8_t x1,uint8_t y1,uint8_t x2,uint8_t y2,uint8_t mode)
{
	uint8_t t;
	x1=clampX(x1);
	x2=clampX(x2);
	y1=clampY(y1);
	y2=clampY(y2);

	if (x2<x1) { t=x1; x1=x2; x2=t; } 
	if (y2<y1) { t=y1; y1=y2; y2=t; } 

	while(y1<=y2)
	{
		LCD_drawLine(x1,y1,x2,y1,mode);
		y1++;
	}	
}
