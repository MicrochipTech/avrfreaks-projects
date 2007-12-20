/*

	file:	window.c
	desc:	window tools implementation.

	author: Jaromir Dvorak (md@unicode.cz)

*/

#include <stdlib.h>
#include "window.h"
#include "../video/driver.h"
#include "misc.h"


unsigned char lastwindow = 0;
struct window windows[MAX_WINDOWS];

////
//Creates a window.
////
WINDOW window_add(unsigned char line, unsigned char height, unsigned char params)
{
 struct window* wnd = &windows[lastwindow];

 if(line > DRIVER_MAXY) return 0;
 if(line+height > DRIVER_MAXY) height = DRIVER_MAXY - line;

 wnd->line = line;
 wnd->height = height;
 wnd->params = params;
 
 if(++lastwindow >= MAX_WINDOWS) lastwindow=0;
 else
 {
  windows[lastwindow].height = 255;
  windows[lastwindow].params = 0;
 }

 return wnd;
}

////
//Resets to original state (maps whole memory to whole screen)
////
void window_reset()
{
	lastwindow = 0;
	(*windows).height = DRIVER_RESY;
	(*windows).params = 0xF8;
	window_select(windows);
}


////
//Color masks.
////
void awindow_set_colormask(WINDOW wnd, unsigned char mask)
{
	register unsigned char tmp = wnd->params;
    tmp&=~0xF0;
    mask&=0xF0;
    tmp|=mask;
    wnd->params = tmp;
}


////
//Fast scrolling macros.
////
static inline void awindow_set_scroll_x(WINDOW wnd, unsigned char x)
{
    register unsigned char tmp = wnd->params;
    tmp&=~7;
    x&=7;
    tmp|=x;
    wnd->params = tmp;
}
static inline signed char awindow_rel_scroll_x(WINDOW wnd, signed char x)
{
    register unsigned char xs = wnd->params;
    x+=xs&7;
	xs&=~7;
	xs|=x&7;
	wnd->params = xs;
	return x;
}	
static inline void awindow_set_scroll_y(WINDOW wnd, unsigned char y)
{
    register unsigned char tmp = wnd->line;
    tmp&=~7;
    y&=7;
    tmp|=y;
    wnd->line = tmp;
}
static inline signed char awindow_rel_scroll_y(WINDOW wnd, signed char y)
{
    register unsigned char ys = wnd->line;
	y+=ys&7;
	ys&=~7;
	ys|=y&7;
	wnd->line = ys;
	return y;
}



////
//Classic absolute scrolling (max. 7px vertically and 7px horizontally)
////
#ifdef WINDOW_ABS_SCROLL
void awindow_set_scroll(WINDOW wnd, unsigned char x, unsigned char y)
{
    awindow_set_scroll_x(wnd, x);
    awindow_set_scroll_y(wnd, y);
}
#endif


////
//Relative scrolling.
////
#ifdef WINDOW_SCROLL
void awindow_scroll(WINDOW wnd, signed char x, signed char y)
{
	awindow_rel_scroll_x(wnd, x);
	awindow_rel_scroll_y(wnd, y);
}	
#endif


////
//Relative scrolling with data transfer.
////
#ifdef WINDOW_SCROLL_FULL
void awindow_scroll_full(WINDOW wnd, signed char x, signed char y, unsigned char default_block)
{
	x = awindow_rel_scroll_x(wnd, x) >> 3;
	y = awindow_rel_scroll_y(wnd, y) >> 3;
	
	signed char dir;
	if(y) dir = sgn(y);
	else if(x) dir = sgn(x);
	else return;


	register signed char xp = DRIVER_COLUMNS; 
	register signed char yp = window_get_height()+1;

	register unsigned char *scraddr1 = window_get_ptr();
    if(dir<0)  scraddr1 += (yp*DRIVER_COLUMNS-1);

	register unsigned char *scraddr2 = scraddr1;
	scraddr2+=x;
	scraddr2+=y*DRIVER_COLUMNS;

	x=abs(x);
	y=abs(y);

	while(1)
	{
		if(xp>x && yp>y) *scraddr1 = *scraddr2;
		else   	         *scraddr1 = default_block;

        scraddr1+=dir;
		scraddr2+=dir;

		if(!--xp) 
        {
           if(!--yp) return;
           xp=DRIVER_COLUMNS;
        }
	}
}
#endif










