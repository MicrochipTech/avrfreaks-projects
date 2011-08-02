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
#include "LcdAdvancedDraw.h"

//  Bresenham used for circles (int only, no sin/cos, fast)

void DrawMirrorPixelsScaled(uint8_t xc, uint8_t yc, uint8_t x, uint8_t y, uint8_t scaleX, uint8_t scaleY,uint8_t mode)
{
	uint8_t sxh=(x*scaleX)>>7;
	uint8_t syh=(y*scaleX)>>7;
	uint8_t sxv=(x*scaleY)>>7;
	uint8_t syv=(y*scaleY)>>7;
	LCD_setPixelXY (xc + sxh, yc + syv, mode);
	LCD_setPixelXY (xc - sxh, yc + syv, mode);
	LCD_setPixelXY (xc + sxh, yc - syv, mode);
	LCD_setPixelXY (xc - sxh, yc - syv, mode);
	LCD_setPixelXY (xc - syh, yc - sxv, mode);
	LCD_setPixelXY (xc - syh, yc + sxv, mode);
	LCD_setPixelXY (xc + syh, yc - sxv, mode);
	LCD_setPixelXY (xc + syh, yc + sxv, mode);
}

void DrawMirrorPixelsFilledScaled(uint8_t xc, uint8_t yc, uint8_t x, uint8_t y, uint8_t scaleX, uint8_t scaleY,uint8_t mode)
{
	uint8_t sxh=(x*scaleX)>>7;
	uint8_t syh=(y*scaleX)>>7;
	uint8_t sxv=(x*scaleY)>>7;
	uint8_t syv=(y*scaleY)>>7;

	LCD_drawLine(xc - sxh, yc - syv,  xc + sxh, yc - syv,mode);
	LCD_drawLine(xc - sxh, yc + syv,  xc + sxh, yc + syv,mode);
	LCD_drawLine(xc - syh, yc - sxv,  xc + syh, yc - sxv,mode);
	LCD_drawLine(xc - syh, yc + sxv,  xc + syh, yc + sxv,mode);
}


void LCD_intern_Oval(uint8_t xc, uint8_t yc, uint8_t width, uint8_t height, uint8_t mode, uint8_t filled)
{
	uint8_t x;
	uint8_t y = 0;
	int8_t g;
	uint8_t scaleX,scaleY,r;	

	if (width>height) { r=width; scaleX=128; scaleY=(128*height)/width; }
	else { r=height; scaleY=128; scaleX=(128*width)/height; }


	x=r;
	g = 2 - 2*r;

	if (filled==1) DrawMirrorPixelsFilledScaled(xc, yc, x, y,scaleX,scaleY,mode);
	else DrawMirrorPixelsScaled(xc,yc,x,y,scaleX,scaleY,mode);
	while(x > y) 
	{
		y = y + 1;
		if(g < 0) g = g + 4*y + 2;
		else 
		{
			x = x - 1;
			g = g + 4*(y - x) + 5;
		}
	if (filled==1) DrawMirrorPixelsFilledScaled(xc, yc, x, y,scaleX,scaleY,mode);
	else DrawMirrorPixelsScaled(xc,yc,x,y,scaleX,scaleY,mode);
	}
}



/*
    *  the coordinates of vertices are (A.x,A.y), (B.x,B.y), (C.x,C.y); we assume that A.y<=B.y<=C.y (you should sort them first)
    * dx1,dx2,dx3 are deltas used in interpolation
    * horizline draws horizontal segment with coordinates (S.x,Y), (E.x,Y)
    * S.x, E.x are left and right x-coordinates of the segment we have to draw
    * S=A means that S.x=A.x; S.y=A.y; 
*/

void swap(uint8_t* a,uint8_t *b)
{
	uint8_t t=*a;
	*a=*b;
	*b=t;
}

void fillTriangle(uint8_t Ax,uint8_t Ay,uint8_t Bx,uint8_t By,uint8_t Cx,uint8_t Cy,uint8_t mode)
{
	int16_t dx1,dx2,dx3;
	int16_t Sx,Ex;
	uint8_t Sy,Ey;

	if (Ay>By) { swap(&Ay,&By); swap(&Ax,&Bx); }
	if (Ay>Cy) { swap(&Ay,&Cy); swap(&Ax,&Cx); }
	if (By>Cy) { swap(&By,&Cy); swap(&Bx,&Cx); }
	if (By-Ay > 0) dx1=((Bx-Ax)<<8)/(By-Ay); else dx1=0;
	if (Cy-Ay > 0) dx2=((Cx-Ax)<<8)/(Cy-Ay); else dx2=0;
	if (Cy-By > 0) dx3=((Cx-Bx)<<8)/(Cy-By); else dx3=0;

	Ex=Sx=Ax<<8; Ey=Sy=Ay;

	if(dx1 > dx2) 
	{
		while(Sy<=By)
		{
			LCD_drawLine(Sx>>8,Sy++,Ex>>8,Ey++,mode);
			Sx+=dx2;
			Ex+=dx1;
		}
		Ex=Bx<<8; Ey=By;
		while(Sy<=Cy)
		{		
			LCD_drawLine(Sx>>8,Sy++,Ex>>8,Ey++,mode);
			Sx+=dx2;
			Ex+=dx3;
		}
	} 
	else 
	{
		while(Sy<=By)
		{			
			LCD_drawLine(Sx>>8,Sy++,Ex>>8,Ey++,mode);
			Sx+=dx1;
			Ex+=dx2;
		}
		Sx=Bx<<8; Sy=By;
		while(Sy<=Cy)
		{		
			LCD_drawLine(Sx>>8,Sy++,Ex>>8,Ey++,mode);
			Sx+=dx3;
			Ex+=dx2;
		}
	}
}

