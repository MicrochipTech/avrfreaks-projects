/******************************************************************************

                          eXtreme Electronics ProGFX Project(TM)
						  --------------------------------------
						        www.eXtremeElectronics.co.in
								       www.ProGFX.org

ProGFX is a Powerful but easy to use C library to program Common Graphical LCDs.

*Line Drawing
*Rectangle Drawing
*Filled Rectangle Drwaing
*Circle Drawing
*Image/Icon Drawing
*Text Drawing
*Number (Integer Drawing)
*Varriable Width Font Support.
*Supports KS0108B Based 128x64 Graphic LCD Modules.
*Runs On AVRs with more than 2K of RAM
*Double Buffering for flicker free animations.
*Free for any use provided you give credit to www.ProGFX.org

The APIs are highly documented and easy to use even by a beginner.

For More Info Log On to 
www.eXtremeElectronics.co.in

or

www.ProGFX,org

Copyright 2008-2010 eXtreme Electronics India

                                 

Please give credit to www.eXtremeElectronics.co.in if you use
it in your projects and find it useful

                                     NOTICE
									--------
NO PART OF THIS WORK CAN BE COPIED, DISTRIBUTED OR PUBLISHED WITHOUT A
WRITTEN PERMISSION FROM EXTREME ELECTRONICS INDIA. THE LIBRARY, NOR ANY PART
OF IT CAN BE USED IN COMMERCIAL APPLICATIONS. IT IS INTENDED TO BE USED FOR
HOBBY, LEARNING AND EDUCATIONAL PURPOSE ONLY. IF YOU WANT TO USE THEM IN 
COMMERCIAL APPLICATION PLEASE WRITE TO THE AUTHOR.

THE PROGRAM IS PROVIDED "AS IS" WITHOUT WARRANTY
OF ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING, BUT NOT LIMITED TO,
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
PURPOSE.  THE ENTIRE RISK AS TO THE QUALITY AND PERFORMANCE OF THE PROGRAM
IS WITH YOU.  SHOULD THE PROGRAM PROVE DEFECTIVE, YOU ASSUME THE COST OF
ALL NECESSARY SERVICING, REPAIR OR CORRECTION.



WRITTEN BY:
AVINASH GUPTA
me@avinashgupta.com


*******************************************************************************/

#ifndef _GFX_H_
#define _GFX_H_

#include <avr/io.h>
#include <avr/pgmspace.h>

#include "types.h"


//#define GFX_OPTION_NOCHECK

#define GFX_SCREEN_WIDTH 128
#define GFX_SCREEN_HEIGHT 64

extern UINT8 video_mem[1024];	//Main GFX Frame Buffer

#define GFX_COLOR_BLACK 1
#define GFX_COLOR_WHITE 0

/**********************************************************

void GFXInit()
	This function Initialize the ProGFX Graphics Engine.
	Must be called before using any other graphic
	functions.

Argument:
	NONE

Return Value
	NONE

**********************************************************/
void GFXInit();



/**********************************************************

void GFXUpdate()
	This function transfers the internal graphic buffer
	to the LCD Module. After your drawing is complete you
	must call this to make it visible on screen.

Argument:
	NONE

Return Value
	NONE

**********************************************************/
void GFXUpdate();



/**********************************************************

void GFXRawPutPixel(void (UINT8 x,UINT8 y,UINT8 color)
	This function Draws a single pixel on screen.

Argument:
	x= x co-ordinate
	y= y co-ordinate
	color = GFX_COLOR_BLACK or GFX_COLOR_WHITE

Return Value
	NONE

**********************************************************/
void GFXRawPutPixel(UINT8 x,UINT8 y,UINT8 color);



/**********************************************************

void GFXHLine(UINT8 x1,UINT8 y1,UINT8 len,UINT8 color)
	This function Draws a Horizontal Line.

Argument:
	x1 = starting x co-ordinate.
	y1 = starting y co-ordinate.

	len = length of line

	colour = either GFX_COLOR_BLACK or GFX_COLOR_WHITE

Return Value
	NONE

**********************************************************/
void GFXHLine(UINT8 x1,UINT8 y1,UINT8 len,UINT8 color);



/**********************************************************

void GFXVLine(UINT8 x1,UINT8 y1,UINT8 len,UINT8 color)
	This function Draws a Vertical Line.

Argument:
	x1 = starting x co-ordinate.
	y1 = starting y co-ordinate.

	len = length of line

	colour = either GFX_COLOR_BLACK or GFX_COLOR_WHITE

Return Value
	NONE

**********************************************************/
void GFXVLine(UINT8 x1,UINT8 y1,UINT8 len,UINT8 color);




/**********************************************************

void GFXRect(UINT8 x1,UINT8 y1,UINT8 x2, UINT8 y2,UINT8 color)

	This function Draws a Rectangle.

Argument:
	x1 = top-left x co-ordinate.
	y1 = top-left y co-ordinate.

	x2 = bottom-right x co-ordinate.
	y2 = bottom-right y co-ordinate.

	colour = either GFX_COLOR_BLACK or GFX_COLOR_WHITE

Return Value
	NONE

**********************************************************/
void GFXRect(UINT8 x1,UINT8 y1,UINT8 x2, UINT8 y2,UINT8 color);




/**********************************************************

void GFXFillRect(UINT8 x1,UINT8 y1,UINT8 x2, UINT8 y2,UINT8 color)

	This function Draws a Filled Rectangle.

Argument:
	x1 = top-left x co-ordinate.
	y1 = top-left y co-ordinate.

	x2 = bottom-right x co-ordinate.
	y2 = bottom-right y co-ordinate.

	colour = either GFX_COLOR_BLACK or GFX_COLOR_WHITE

Return Value
	NONE

**********************************************************/
void GFXFillRect(UINT8 x1,UINT8 y1,UINT8 x2, UINT8 y2,UINT8 color);




/**********************************************************

void GFXClear();

	This function Clears the screen.

Argument:
	NONE

Return Value
	NONE

**********************************************************/
void GFXClear();




/**********************************************************

INT8 GFXPutCharXY(UINT8 x, UINT8 y,char c,UINT8 color)
	
	This function Draws the given char.

Argument:
	x = x co-ordinate.
	y = y co-ordinate.

	char = the char user wants to draw.

	colour = either GFX_COLOR_BLACK or GFX_COLOR_WHITE

Return Value
	1 = Success.
	-1 = Fail!

**********************************************************/
INT8 GFXPutCharXY(UINT8 x, UINT8 y,char c,UINT8 color);




/**********************************************************

INT8 GFXGetCharWidth(char c)
	
	Returns the width of given char c in the current
	selected font.

Argument:
	c= any character eg. 'a'.

Return Value
	The width in pixel of the above char.

**********************************************************/
INT8 GFXGetCharWidth(char c);




/**********************************************************

INT8 GFXGetCharHeight()
	
	This function returns the height of charaters in
	current selected font.

Argument:
	x1 = top-left x co-ordinate.
	y1 = top-left y co-ordinate.

	x2 = bottom-right x co-ordinate.
	y2 = bottom-right y co-ordinate.

	colour = either GFX_COLOR_BLACK or GFX_COLOR_WHITE

Return Value
	NONE

**********************************************************/
INT8 GFXGetCharHeight();




/**********************************************************

INT8 GFXGetStringWidth(char *string)

	This function returns the width of given string in the
	current selected font.

Argument:
	string = any C string.

Return Value
	Width of the string in pixel.

**********************************************************/
INT8 GFXGetStringWidth(const char *string);




/**********************************************************

INT8 GFXWriteStringXY(UINT8 x,UINT8 y,const char *string,UINT8 color)

	This function Draws a given string on screen.

Argument:
	x = x co-ordinate.
	y = y co-ordinate.
	
	string = any C String like "Hello World !"

	colour = either GFX_COLOR_BLACK or GFX_COLOR_WHITE

Return Value
	1 on success.
	-1 on failure.

**********************************************************/
INT8 GFXWriteStringXY(UINT8 x,UINT8 y,const char *string,UINT8 color);




/**********************************************************

INT8 GFXCenterText(const char *string,UINT8 color)

	This function Draws a string in exact centre of screen.

Argument:
	string = any C String like "Hello World !"


	colour = either GFX_COLOR_BLACK or GFX_COLOR_WHITE

Return Value
	1 on success.
	-1 on failure.

**********************************************************/
INT8 GFXCenterText(const char *string,UINT8 color);




/**********************************************************

void GFXLine(UINT8 x1,UINT8 y1,UINT8 x2,UINT8 y2,UINT8 color)
	
	This function Draws a line between two given points.

Argument:
	x1 = start x co-ordinate.
	y1 = start y co-ordinate.

	x2 = end x co-ordinate.
	y2 = end y co-ordinate.

	colour = either GFX_COLOR_BLACK or GFX_COLOR_WHITE

Return Value
	NONE

**********************************************************/
void GFXLine(UINT8 x1,UINT8 y1,UINT8 x2,UINT8 y2,UINT8 color);




/**********************************************************

void GFXCircle(UINT8 cx,UINT8 cy,UINT8 rad,UINT8 color)

	This function Draws a Circle with given center and radius.

Argument:
	cx = x co-ordinate of center.
	cy = y co-ordinate of center.

	rad = radius.

	colour = either GFX_COLOR_BLACK or GFX_COLOR_WHITE

Return Value
	NONE

**********************************************************/
void GFXCircle(UINT8 cx,UINT8 cy,UINT8 rad,UINT8 color);




/**********************************************************

void GFXDrawImage(UINT8 x,UINT8 y,prog_uint8_t *img)

	This function Draws an Image.

Argument:
	x = x co-ordinate.
	y = y co-ordinate.

	img = image to draw

Return Value
	NONE

**********************************************************/
void GFXDrawImage(UINT8 x,UINT8 y,prog_uint8_t *img);

/**********************************************************

void GFXWriteIntXY(UINT8 x,UINT8 y,INT16 val,INT8 feild_length,UINT8 color)
	
	This function Draws a number (integer) at given x,y location.

Argument:
	x = x co-ordinate.
	y = y co-ordinate.

	val = integer number to draw

	colour = either GFX_COLOR_BLACK or GFX_COLOR_WHITE

Return Value
	NONE

**********************************************************/
void GFXWriteIntXY(UINT8 x,UINT8 y,INT16 val,INT8 feild_length,UINT8 color);
			
#endif

