/*

	file:	main.c
	desc:	AVGA demo.

 	note:	best results with WinAVR-20060125
	
	target:	ATMEGA168

	author:	Jaromir Dvorak (md@unicode.cz)

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "AVGA/video/video.h"
#include "AVGA/video/driver.h"

#include "AVGA/utils/misc.h"
#include "AVGA/utils/window.h"
#include "AVGA/utils/overlay.h"
#include "AVGA/utils/background.h"
#include "AVGA/utils/delay.h"


#include "graphics/graphics.h"
#include "graphics/tileset.h"	//generated with tile2h
#include "graphics/map1.h" 		//generated with x2h_rle
#include "graphics/anim1.h"		//generated with x2h


//memory for the driver.
unsigned char screen[DRIVER_SCREEN_SIZE];
const unsigned char pgmmap[] PROGMEM = TILESET;
unsigned char rammap[DRIVER_RAMMAP_SIZE];


//user graphical objects
const unsigned char anim1[] PROGMEM = ANIM1;
const unsigned char map1[] PROGMEM = MAP1;
const unsigned char avrlogo[] PROGMEM = {13,14,15,16,17,18,19,20};


//Some high-level helpful functions.
static unsigned int DrawCenteredText(int y, PGM_P txt);
static unsigned char RenderMap( signed int x, signed int y );
static void DrawCenteredTextWithFrame(int y, PGM_P   txt);
static void DrawFrame( unsigned char x, unsigned char y, unsigned char width, unsigned char height, unsigned char frame );
static void DrawInt16( int x, int y, unsigned int i );
static void DrawPalette( unsigned char x, unsigned char y );


//demonstration
static inline void intro();
static inline void demo1();
static inline void demo2();
static inline void demo3();
static inline void demo4();
static inline void demo5();
static inline void demo6();
static inline void outro();


int main( void )
{
	driver_mmap(screen, pgmmap, rammap);
	video_init(PAL);

	video_enable(); 	//start driver.
	wait_seconds(1); 	//wait for display to sync.

	intro();
	demo1();
	demo2();
	demo3();
	demo4();
	demo5();
	demo6();
	outro();

	__device_reset();

}



static inline void intro()
{
	driver_fill(128);  //clear screen
	wait_seconds(1);


 	driver_set_block(0, 0, 4);
 	driver_set_block(DRIVER_LAST_COLUMN, 0, 4);
 	driver_set_block(0, DRIVER_LAST_ROW, 4);
 	driver_set_block(DRIVER_LAST_COLUMN, DRIVER_LAST_ROW, 4);
	DrawCenteredText(5, PSTR("Please, adjust\nVIDEO_XPOS, VIDEO_YPOS\nin video.h\nto fit your display!"));
	wait_seconds(4);

	driver_fill(128);
	wait_seconds(1);
	DrawCenteredText(3, PSTR("Hello, world!\n\nAVGA - an      based\nCOLOR video game\ndevelopment platform\n\n" __DATE__));
	background_draw(12, 6, avrlogo, 4, 2);
	wait_seconds(5);

	driver_fill(128);
	DrawCenteredText(4, PSTR("192*144*4bpp\ninterrupt driven\n3 MIPS left\nfor user program."));
	DrawPalette(48,112);
	wait_seconds(5);
	overlay_release();   //palette...
}

static inline void demo1()
{
    int x, y;
	driver_fill(128);

	wait_seconds(1);
	DrawCenteredTextWithFrame(7, PSTR("Demo #1\nPGM graphics"));
	wait_seconds(3);

	driver_fill(21);
	for(y=DRIVER_LAST_ROW;y>=0;y--)
	{
	 for(x=0;x<DRIVER_LAST_COLUMN;x++)
	 {
	 	driver_set_block(x,y,23);
		sync();
	 }
	 for(x=DRIVER_LAST_COLUMN;x>=0;x--)
	 {
	 	driver_set_block(x,y,22);
		sync();
	 }
	}
	wait_seconds(3);
}

static inline void demo2()
{
	driver_fill(128);
	wait_seconds(1);

	DrawCenteredTextWithFrame(7, PSTR("Demo #2\nRAM graphics"));
	wait_seconds(3);

	unsigned char* ptr = screen;
	unsigned char b=0;
	unsigned int i;
	for(i=0;i<DRIVER_SCREEN_SIZE;i++)
	{
		*ptr++ = driver_rammap_block(b);
		if(++b >= DRIVER_RAM_BLOCK_COUNT) b=0;
	}
	wait_seconds(2);
	
	memset(rammap, 0, sizeof(rammap));
	wait_seconds(1);

	ptr = screen;

	for(i=0;i<DRIVER_SCREEN_SIZE;i++)
		*ptr++ = driver_rammap_block(rand() % DRIVER_RAM_BLOCK_COUNT);

	for(i=0;i<5000;i++)
		rammap[rand() % sizeof(rammap)] = rand();

	wait_seconds(2);
}

static inline void demo3()
{
    unsigned char x, y, i;
	driver_fill(128);
	wait_seconds(1);
	DrawCenteredTextWithFrame(7, PSTR("Demo #3\nWINDOWS"));
	wait_seconds(3);

	//driver_fill(3);

	WINDOW wnd = window_add_standard(0, 9);
	window_fill(21);
	window_print(5,4,"This is a window");
	wait_seconds(1);

	window_add_clone(wnd);
	wait_seconds(1);

	for(y=0;y<9;y++)
	 for(x=0;x<DRIVER_WIDTH;x++)
	 {
	 	window_set_block(x,y,(rand() % 3)+1);
		sync();
	 }


	wait_seconds(3);

	//transition effects
	window_reset();
	driver_fill(128);
	wait_seconds(1);

	WINDOW wndA = window_add_standard(0, 6);
	WINDOW wndB = window_add_standard(6, 6);
	WINDOW wndC = window_add_standard(12, 30);

	awindow_fill(wndA, 206);
	awindow_fill(wndB, 216);
	awindow_fill(wndC, 22);

	wait_seconds(1);
	//awindow_print(wndA, 5, 2, "wndA");
	//awindow_print(wndB, 5, 2, "wndB");
	//awindow_print(wndC, 5, 2, "wndC");

	//window_select(wndC);
	//DrawFrame(0,0,22,4,24);

	for(i=0;i<48;i++)
	{
		awindow_move(wndB, 1);
		awindow_shrink(wndC, 1);
		sync();
		sync();
	}
	for(i=0;i<47;i++)
	{
		awindow_shrink(wndA, -1);
		awindow_move(wndC, -1);
		sync();
		sync();
	}
	for(i=0;i<95;i++)
	{
		awindow_shrink(wndA, 1);
		awindow_move(wndC, 1);
		sync();
		sync();
	}
	awindow_set_colormask(wndB, 0xAA);

	for(i=0;i<96;i++)
	{
		awindow_shrink(wndA, -1);
		awindow_move(wndB, -1);
		awindow_move(wndC, -1);
		sync();	
		sync();
	}
	awindow_set_colormask(wndB, 0xBB);

	for(i=0;i<150;i++)
	{
		awindow_shrink(wndA, 1);
		awindow_move(wndB, 1);
		awindow_move(wndC, 1);
		sync();
		sync();
	}
	wait_seconds(3);

	window_reset();
}	

static inline void demo4()
{
	unsigned int i;
	int x=0, y=0;

	driver_fill(128);
	wait_seconds(1);

	DrawCenteredTextWithFrame(7, PSTR("Demo #4\nBACKGROUND"));
	wait_seconds(3);

	WINDOW wndA = window_add_standard(0, 1);
	WINDOW wndB = window_add_standard(1, 16);
	WINDOW wndC = window_add_standard(18, 1);
	awindow_fill(wndA, 25);
	awindow_fill(wndC, 29);
	window_select(wndB);

	signed char dx=-1,dy=-1;

	for(i=0;i<1000;i++)
	{
		window_fill_scroll(128);
		window_adjust_for_background(x, y);
		//background_draw(x>>3, x>>3,  avrlogo, 4, 2);
		background_draw((x>>3),(y>>3), (PGM_P)(10000), 100, 100);	//
	
		x+=dx; if(x >= 0 || x <= -200) dx=-dx;
        y+=dy; if(y >= 0 || y <= -100) dy=-dy;
		sync();
	}
	wait_seconds(3);

	window_reset();
	window_set_scroll(0,0);
}

static inline void demo5()
{
	unsigned int i;

	driver_fill(128);
	wait_seconds(1);

	DrawCenteredTextWithFrame(7, PSTR("Demo #5\nOVERLAY"));
	wait_seconds(3);

	//unsigned char n;
	//unsigned char x5[]={50,100,150}, y5[]={20,80,100};
	//signed char dx5[]={1,-1,1}, dy5[]={-1,1,1};

	//beter use registers...
	unsigned char x1=50, x2=100, x3=150, y1=20, y2=80, y3=100;
	signed char dx1=1, dx2=-1, dx3=1, dy1=-1, dy2=1, dy3=1;

	driver_fill(153);
	wait_seconds(1);

	for(i=0;i<1000;i++)
	{
		overlay_clear();

		//3 sprites with size 8*8; so in the worst case, we need just 12 ramblocks.
	   	overlay_draw_sprite(x1, y1, 1);
    	overlay_draw_sprite(x2, y2, 2);
    	overlay_draw_sprite(x3, y3, 3);


		x1+=dx1; if(x1 >= 184 || x1 <= 0) dx1=-dx1;
        y1+=dy1; if(y1 >= 136 || y1 <= 0) dy1=-dy1;

		x2+=dx2; if(x2 >= 184 || x2 <= 0) dx2=-dx2;
        y2+=dy2; if(y2 >= 136 || y2 <= 0) dy2=-dy2;

		x3+=dx3; if(x3 >= 184 || x3 <= 0) dx3=-dx3;
        y3+=dy3; if(y3 >= 136 || y3 <= 0) dy3=-dy3;

		sync();
	}

	wait_seconds(3);



    //try this with OVERLAY_INVERSE_PRIORITY
/*
	dx=1; dy=1;
    driver_fill(128);
	overlay_release();
	for(i=0;i<500;i++)
	{
		x+=dx; if(x >= 192 || x <= 0) dx=-dx;
        y+=dy; if(y >= 144 || y <= 0) dy=-dy;
		overlay_putpixel(x,y,15);
		sync();
	}

	driver_fill(128);
	overlay_release();
	for(i=0;i<500;i++)
	{
		overlay_putpixel(rand() % 192, rand() % 144, rand() % 15);
		sync();
	}
*/

}

static inline void demo6()
{
	driver_fill(128);
	wait_seconds(1);
	DrawCenteredTextWithFrame(7, PSTR("Demo #6\nGame environment"));
	wait_seconds(3);

	driver_fill(128);
	WINDOW wnd1 = window_add_standard(0, 15);
	WINDOW wnd2 = window_add_standard(16, 3); //leave one row for scroll.

	window_select(wnd2);
	DrawFrame(0, 0, 22, 1, 5);
	window_print_P(1, 1, PSTR("Cycles left (*7):"));

	signed char dx=1, dy=0;
	unsigned int x=0, y=0;
	unsigned int i;
	char j=0;

	PGM_P frame = anim1; //animation

	for(i=0;i<1930;i++)
	{
		//render first.
		window_select(wnd1); 

		if(RenderMap(-x, -(y/20))) 
			overlay_release();
		else
			overlay_clear(); //more time left now...

		overlay_draw(x/2, y/20, frame, 16, 16); //render animating sprite.

	
		//ok, now it's time to move the scene.
		if(++j > 2) //animate
		{
			frame += 128;
			if((unsigned int)frame >= (unsigned int)anim1+sizeof(anim1)) frame = anim1;
			j=0;
		}

		//do simple ball physics
		x+=dx;
        if(x > 280 || x < 0) dx=-dx;

        y+=dy;
        if(y > 1780) { dy=-dy; dy+=2; y = 1780; }
		dy++;
			

		//ok, wait for retrace.
		unsigned int cycles = sync();

		window_select(wnd2);
		DrawInt16(19, 1, cycles); //show remaining time

		//wait_frames(1); 		//slow down...
	}
}

static inline void outro()
{
	unsigned char y1=40, y2=40;
	signed char dy1=1;
	signed char dy2=-1;
	unsigned char y=60;
	unsigned int i;

	//enable OVERLAY_SAFE and set y to 100...

	window_select(window_get_default());
	window_set_scroll(0,0);
	//window_set_colormask(3);

	for(i=0;i<500;i++)  	//show "Prehistorik2" - like text
	{
		overlay_clear();

		overlay_draw_sprite_param(48, y+y1, 'T', 8);
    	overlay_draw_sprite_param(64, y+y2, 'h', 9);
    	overlay_draw_sprite_param(80, y+y1, 'E', 10);

		overlay_draw_sprite_param(112, y+y2, 'E', 11);
    	overlay_draw_sprite_param(128, y+y1, 'n', 12);
    	overlay_draw_sprite_param(144, y+y2, 'D', 13);

		y1+=dy1; if(y1 >= 90) { y1=90; dy1=-dy1; } else dy1++;
		y2+=dy2; if(y2 >= 90) { y2=90; dy2=-dy2; } else dy2++;
		
		if(y) y--;
		
		sync();
		sync();
	}
	wait_seconds(3);
}



//
// High-level drawing functions.
//

static void DrawInt16(int x, int y, unsigned int i)
{
	unsigned char str[6];
	itoa(i,str,10);
	window_print(x, y, str);
}

static void DrawFrame(unsigned char x, unsigned char y, unsigned char width, unsigned char height, unsigned char frame)
{
	unsigned char i;

 	window_set_block(x++, y, frame++);
	for(i=width; i;i--) window_set_block(x++, y, frame); frame++;
	window_set_block(x, y++, frame++);
	for(i=height;i;i--) window_set_block(x, y++, frame); frame++;
	window_set_block(x--, y, frame++);
	for(i=width; i;i--) window_set_block(x--, y, frame); frame++;
	window_set_block(x, y--, frame++);
	for(i=height;i;i--) window_set_block(x, y--, frame); frame++;
}

static unsigned int DrawCenteredText(int y, PGM_P  txt)
{
	unsigned char c, width=0, height=0;	   

	do //TODO: optimize
	{
	   	PGM_P txt2 = txt;
		unsigned char i=0;

	    while(1)
		{
		   c = pgm_read_byte(txt++);
		   if(!c || c == '\n') break;
		   i++;
		}

   	    unsigned char* ptr = &driver_get_block((DRIVER_COLUMNS/2) - i/2, y);
	    memcpy_P(ptr, txt2, i);
		y+=2;

		if(i>width) width=i;
		height+=2;
	}
	while(c);

	return (width) | ((height-1)<<8);
}

static void DrawCenteredTextWithFrame(int y, PGM_P txt)
{
	register unsigned int i = DrawCenteredText(y, txt);

	unsigned char width = (i & 0xff); //TODO: optimize
	unsigned char height = (i >> 8);
	unsigned char x = (DRIVER_COLUMNS/2) - (width/2);

	DrawFrame(x-1, y-1, width, height, 24);
}

static unsigned char RenderMap(signed int x, signed int y)
{
	window_adjust_for_background(x, y); 	//adjust window
	x>>=3;
	y>>=3;

	static signed char lastx, lasty;
	if(x == lastx && y == lasty) return 0; //no need to redraw!
	lastx=x;
	lasty=y;

	//now draw game map and everything attached to it.
	//window_clear_scroll(wnd, 0);

	background_draw_RLE( x, y, map1, 60, 30 );
	//background_draw( x+30, y+20, avrlogo, 4, 2);

	return 1;
}

static void DrawPalette( unsigned char x, unsigned char y)
{
    unsigned char xp, yp;

	for(yp=0;yp<8;yp++)
	 for(xp=0;xp<96;xp++) 
	  overlay_putpixel(x+xp, y+yp, xp/6);
}








