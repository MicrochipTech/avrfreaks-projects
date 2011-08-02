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
#include "LcdLowLevel.h"

void LCD_intern_Oval(uint8_t xc, uint8_t yc, uint8_t width, uint8_t height, uint8_t mode,uint8_t filled);
void fillTriangle(uint8_t Ax,uint8_t Ay,uint8_t Bx,uint8_t By,uint8_t Cx,uint8_t Cy,uint8_t mode);

#define drawTriangle(Ax,Ay,Bx,By,Cx,Cy,mode) LCD_drawLine(Ax,Ay,Bx,By,mode); LCD_drawLine(Bx,By,Cx,Cy,mode); LCD_drawLine(Ax,Ay,Cx,Cy,mode); 
#define LCD_drawOval(xc, yc, width, height, mode) LCD_intern_Oval(xc,yc,width,height,mode,0);
#define LCD_fillOval(xc, yc, width, height, mode) LCD_intern_Oval(xc,yc,width,height,mode,1);

#define LCD_drawCircle(cx,cy,size, mode)  LCD_intern_Oval(cx,cy,size,size,mode,0);
#define LCD_drawARCorrectCircle(cx,cy,size, mode)  LCD_intern_Oval(cx,cy,size*(LCD_PIXEL_AR_I)>>6,size,mode,0);
#define LCD_fillCircle(cx,cy,size, mode)  LCD_intern_Oval(cx,cy,size,size,mode,1);
#define LCD_fillARCorrectCircle(cx,cy,size, mode)  LCD_intern_Oval(cx,cy,size*(LCD_PIXEL_AR_I)>>6,size,mode,1);
//void LCD_drawCircle(uint8_t xc, uint8_t yc, uint8_t r, uint8_t mode);





