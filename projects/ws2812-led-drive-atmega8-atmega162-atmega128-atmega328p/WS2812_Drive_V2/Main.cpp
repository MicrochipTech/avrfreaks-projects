/*
 //////////////////////////////////////////////////////////////////////////////////////////
         _    _ _____  _____  _____  __   _____  ______      _
        | |  | /  ___|/ __  \|  _  |/  | / __  \ |  _  \    (_)
        | |  | \ `--. `' / /' \ V / `| | `' / /' | | | |_ __ ___   _____
        | |/\| |`--. \  / /   / _ \  | |   / /   | | | | '__| \ \ / / _ \
        \  /\  /\__/ /./ /___| |_| |_| |_./ /___ | |/ /| |  | |\ V /  __/
         \/  \/\____/ \_____/\_____/\___/\_____/ |___/ |_|  |_| \_/ \___|
		
 //////////////////////////////////////////////////////////////////////////////////////////
 --------------------------------- Main Program (tab 4) C++ ------------------------------ 

==========================================================================================
 @AUTHOR  : João D´Artagnan A. Oliveira
 @DATE    : September,28 of 2015
 @TIME    : 4pm22
 @VERSION : 1.0
==========================================================================================
 @PURPOSE....: Drive for ws2812 LED for matrix panel with Rows(n)xColums(m) with graphic 
			   engine to drawing 
			
 @HARDWARE...: Atmega328p running at 16Mhz(not alterable clock frequency)
 @OBSERVATION: I using to develop this project a CJMCU 64 Bit WS2812 5050 RGB LED Driver Development Board
			   8x8 LED Matrix
==========================================================================================
COMPILE CONFIGURATIONS

@SYMBOL -DF_CPU=16000000L -D_TIME_TIMER_NUMBER_=0
@OPTIMIZATION -OS
@MISCELLANEOUS -std=gnu++11

In _GraphBasic.H define _GRAPH_BIG_CHAR_SIZE_ To use character font 5x8 or _GRAPH_NORMAL_CHAR_SIZE To use character font 3x5
==========================================================================================
         ______                                      _                 
         | ___ \                                    | |                
         | |_/ / __ ___   __ _ _ __ __ _ _ __ ___   | |     ___   __ _ 
         |  __/ '__/ _ \ / _` | '__/ _` | '_ ` _ \  | |    / _ \ / _` |
         | |  | | | (_) | (_| | | | (_| | | | | | | | |___| (_) | (_| |
         \_|  |_|  \___/ \__, |_|  \__,_|_| |_| |_| \_____/\___/ \__, |
                          __/ |                                   __/ |
                         |___/                                   |___/ 
==========================================================================================
DATA		HORA		DESCRIÇÃO
__________	_____		__________________________________________________________________________
28/10/2015  16:22		Hardware Made, Starting Coding Drive
28/10/2015	03:00		Code complete, make some adjust tomorrow
29/10/2015	18:04		Sample function created, creating BoxFull, RotateY function
29/10/2015	23:47		Function BoxFull,RotateY created, Normal characters added a to z symbols
					    changed VERSION to VERSION_MAJOR & VERSION_MINOR definitions now 1.02
20/10/2015  19:15       Release Version 1.02 to Atmel Spaces
*/ 

/*
 +--------------------------------------+
 |        HARDWARE DEPENDENT INCLUDES	|
 +--------------------------------------+
*/
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include <avr/sleep.h>
#include <avr/pgmspace.h>
/*
 +--------------------------------------+
 |        C LIBRARY INCLUDES		    |
 +--------------------------------------+
*/
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
/*
 +--------------------------------------+
 |        LIBRARY INCLUDES	            |
 +--------------------------------------+
*/
#include "_Time.h"
#include "_WS2812_Drive.h"

/*
 +--------------------------------------+
 |		  GLOBAL DEFINITIONS		    |
 +--------------------------------------+
*/
#define VERSION_MAJOR	1
#define VERSION_MINOR	2
/*
 +--------------------------------------+
 |		  GLOBAL VARIABLES			    |
 +--------------------------------------+
*/
const Color	BLACK(0,0,0);
const Color	RED(255,0,0);
const Color	GREEN(0,255,0);
const Color	YELLOW(255,255,0);
const Color	BLUE(0,0,255);
const Color	PURPLE(255,0,255);
const Color	CYAN(0,255,255);
const Color	WHITE(255,255,255);
/*
 +--------------------------------------+
 |        LOCAL LIBRARY INCLUDES	    |
 +--------------------------------------+
*/

	class_Time			time;
	class_WS2812_Drive	ws(8,8);

/*
 +--------------------------------------+
 |        GLOBAL FUNCTIONS			    |
 +--------------------------------------+
*/

///////////////////////////
/// Draw Vertical Lines
///////////////////////////
void	DrawVertLines()
{
	//clear screen
	ws.Cls();
	//wait a second
	time.DelayMsI(1000);
	//draw Vertical Lines
	for(uint8_t x=0;x<8;x++)
	{
		ws.VertLine(x,0,7,(x&4)?100:0,(x&2)?100:0,(x&1)?100:0);
		ws.Show();
		//time.DelayMsI(250);
	}
}
///////////////////////////
/// Draw Horizontal Lines
///////////////////////////
void	DrawHorzLines()
{
	//clear screen
	ws.Cls();
	//wait a second
	time.DelayMsI(1000);
	//draw Horizontal lines
	for(uint8_t y=0;y<8;y++)
	{
		ws.HorzLine(0,7,y,(y&4)?100:0,(y&2)?100:0,(y&1)?100:0);
		ws.Show();
		//time.DelayMsI(250);
	}
}
///////////////////////////
/// Draw Lines
///////////////////////////

void	DrawLines()
{
	//clear screen
	ws.Cls();
	//wait a second
	time.DelayMsI(1000);
	//draw lines
	Color tmp=ws.GetBackColor();
	ws.SetBackColor(10,10,10);
	//draw animation lines
	for(uint8_t c=1;c<8;c++)
	{
		for(int x=0;x<8;x++)
		{
			ws.Cls();
			ws.Line(x,0,ws.MaxX()-x,ws.MaxY(),(c&4)?100:0,(c&2)?100:0,(c&1)?100:0);
			ws.Show();
			time.DelayMsI(50);
		}
		for(int x=8;x>=0;x--)
		{
			ws.Cls();
			ws.Line(x,0,ws.MaxX()-x,ws.MaxY(),(c&4)?100:0,(c&2)?100:0,(c&1)?100:0);
			ws.Show();
			time.DelayMsI(50);
		}		
	}
	ws.SetBackColor(tmp);
}
///////////////////////////
/// Draw Boxes
///////////////////////////

void	DrawBoxes()
{
	//clear screen
	ws.Cls();
	//wait a second
	time.DelayMsI(1000);
	//draw concentric boxes
	for(uint8_t i=0;i<5;i++)
	{
		ws.Box(i,i,ws.MaxX()-i,ws.MaxY()-i,((i+1)&4)?100:0,((i+1)&2)?100:0,((i+1)&1)?100:0);
		ws.Show();
		//time.DelayMsI(250);
	}	
}
///////////////////////////
/// Draw Circles
///////////////////////////

void	DrawCircles()
{
	Color Tmp;
	//clear screen
	ws.Cls();
	//wait a second
	time.DelayMsI(1000);
	//draw lines
	Tmp=ws.GetBackColor();
	//draw randomic circles at randomic position at randomic colors
	for(uint8_t i=0;i<20;i++)
	{
		ws.SetBackColor(rand()&15,rand()&15,rand()&15);
		ws.Cls();
		ws.Circle(rand()&7,rand()&7,rand()&3,rand()&255,rand()&255,rand()&255);
		ws.Show();
		time.DelayMsI(250);
	}	
}
///////////////////////////
/// Draw Text
///////////////////////////

void	DrawText()
{
	Color Tmp;
	int x=0;
	char buf[20];
	
	//clear screen
	ws.Cls();
	//wait a second
	time.DelayMsI(1000);
	//draw scrolling text
	Tmp=ws.GetBackColor();
	ws.SetBackColor(0,0,50);
	ws.SetForeColor(100,100,0);
	//formatting test including version 
	sprintf(buf,"  Drive Version %d.%02d",VERSION_MAJOR,VERSION_MINOR);
	//scrolling a text decrementing your x position and draw it on screen
	for(;;)
	{
		ws.Cls();
		ws.Locate(x,0);
		ws.PutS(buf);
		x--;
		if (x<(-ws.GetTextWidth()*23)) break;
		ws.Show();
		time.DelayMsI(100);
	}
	ws.SetBackColor(Tmp);
}
///////////////////////////
/// Rotate X pixels
///////////////////////////

void	RotateX()
{
	uint32_t	tl1;
	
	//draw 4 sinusoidal bars
	for(int i=0;i<8;i++)
	{
		uint8_t y=128+100*sin((i*6.28)/8);

		ws.VertLine(i,0,1,y,0,0);
		ws.VertLine(i,2,3,0,y,0);
		ws.VertLine(i,4,5,0,0,y);
		ws.VertLine(i,6,7,y,y,0);
	}
	//mark a time
	time.SetElapsed(&tl1);
	uint8_t c=0;
	for(;;)
	{
		//rotate screen regions at 40ms a time
		if (time.GetElapsed(&tl1)>40)
		{
			time.SetElapsed(&tl1);
			ws.RotateX(0,7,0,1);
			ws.RotateX(7,0,2,3);
			ws.RotateX(0,7,4,5);
			ws.RotateX(7,0,6,7);
			c++;
		}
		if (c>64) break;
		ws.Show();
		//time.DelayMsI(20);
	}
}
///////////////////////////
/// Fades All Board width colors
///////////////////////////

void Fades()
{
	//clear screen
	ws.Cls();
	//wait a second
	time.DelayMsI(1000);
	//Repeat 2 times
	for(uint8_t j=0;j<2;j++)
	{
		//Draw full screen with 7 colors
		for(uint8_t i=1;i<8;i++)
		{
			//increment color
			for(int v=0;v<=240;v+=16)
			{
				ws.SetBackColor((i&4)?v:0,(i&2)?v:0,(i&1)?v:0);
				ws.Cls();
				ws.Show();
				time.DelayMsI(25);
			}
			//decrement color
			for(int v=240;v>=0;v-=16)
			{
				ws.SetBackColor((i&4)?v:0,(i&2)?v:0,(i&1)?v:0);
				ws.Cls();
				ws.Show();
				time.DelayMsI(25);
			}
		}	
	}
}
///////////////////////////
/// Draw Boxes Full
///////////////////////////

void	DrawBoxesFull()
{
	int x=0;
	uint8_t c=1;
	
	//clear screen
	ws.Cls();
	//wait a second
	time.DelayMsI(1000);
	for(uint8_t i=0;i<64;i++)
	{
		ws.Cls();
		ws.BoxFull(x,x,7-x,7-x,(c&4)?100:0,(c&2)?100:0,(c&1)?100:0);
		c++;
		x=(x+1)&7;
		ws.Show();
		time.DelayMsI(100);
	}	
}
///////////////////////////
/// Rotates X axis and Y axis
///////////////////////////

void	RotatesXY()
{
	//   0 1 2 3 4 5 6 7
	// 0 O O O O O O O O
	// 1 O O O O O O O O
	// 2 O O O O O O O O
	// 3 O O O O O O O O
	// 4 O O O O O O O O
	// 5 O O O O O O O O
	// 6 O O O O O O O O
	// 7 O O O O O O O O
	int8_t	c=0,k=0,b=0;

	for(;;)
	{
		//b>16 draw randomic color pixels else draw ladder color pixels
		if (b>16)
		{
			ws.SetPixel(0,0,rand()&127,rand()&127,rand()&127);
		}
		else
		{
			ws.SetPixel(0,0,(b&4)?c:0,(b&2)?c:0,(b&1)?c:0);	
		}
		//process ladder
		k++;
		if (k>8)
		{
			k=0;
			c+=8;
			if (c>100)
			{
				c=0;
				b++;
			}
		}
		
		//rotate in spiral form
		
		ws.RotateX(7,0,0,0,true);
		ws.RotateY(7,7,7,0,true);
		ws.RotateX(0,7,7,7,true);
		ws.RotateY(0,0,1,7,true);
		
		ws.RotateX(6,0,1,1,true);
		ws.RotateY(6,6,6,1,true);
		ws.RotateX(1,6,6,6,true);
		ws.RotateY(1,1,2,6,true);
		
		ws.RotateX(5,1,2,2,true);
		ws.RotateY(5,5,5,2,true);
		ws.RotateX(2,5,5,5,true);
		ws.RotateY(2,2,3,5,true);
		
		ws.RotateX(4,2,3,3,true);
		ws.RotateY(4,4,4,3,true);
		ws.RotateX(3,4,4,4,true);
		
		//show rotation
		ws.Show();
		//change time if b>16
		if (b>16) time.DelayMsI(150); else time.DelayMsI(10);
		//exit if b>17
		if (b>17) break;
	}
}
/*
       ==================================================================================
       =        ___  ___      _        ______                                           =
       =        |  \/  |     (_)       | ___ \                                          =
       =        | .  . | __ _ _ _ __   | |_/ / __ ___   __ _ _ __ __ _ _ __ ___         =
       =        | |\/| |/ _` | | '_ \  |  __/ '__/ _ \ / _` | '__/ _` | '_ ` _ \        =
       =        | |  | | (_| | | | | | | |  | | | (_) | (_| | | | (_| | | | | | |       =
       =        \_|  |_/\__,_|_|_| |_| \_|  |_|  \___/ \__, |_|  \__,_|_| |_| |_|       =
       =                                                __/ |                           =
       =                                               |___/                            =
       ==================================================================================
*/

int main()
{
	 
	//Enable Global Interrupts
        
        sei();
	
	//Main Demo Loop

	for(;;)
	{
		DrawText();
		DrawVertLines();
		DrawHorzLines();
		DrawBoxes();
		DrawLines();
		DrawCircles();
		RotateX();
		Fades();
		DrawBoxesFull();
		RotatesXY();
	}	
}
