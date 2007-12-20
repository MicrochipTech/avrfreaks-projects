/*

	file:	background.c
	desc:	implementation of background tools.

	author: Jaromir Dvorak (md@unicode.cz)

*/

#include <avr/pgmspace.h>
#include <avr/eeprom.h>
#include <string.h>
#include "../video/driver.h"
#include "window.h"
#include "background.h"
                


#ifdef BACKGROUND_DRAW_SIMPLE
void background_draw_simple( unsigned char x, unsigned char y, PGM_P img, unsigned char width, unsigned char height )
{
    register unsigned char *scraddr = &window_get_block(x, y);

	do
    {
		register unsigned char *scrptr = scraddr;

        register unsigned char tmp1;
		for(tmp1=width;tmp1;tmp1--)
				*scrptr++ = pgm_read_byte(img++);
	
		scraddr += DRIVER_COLUMNS;
	}
	while(--height);
}
#endif



#ifdef BACKGROUND_DRAW
unsigned char background_draw(signed char x, signed char y, PGM_P img, uint8_t width, uint8_t height)
{
	register unsigned char endrow = window_get_height()+1;
    register unsigned char *scraddr = window_get_ptr();


	//do all the calculations, to make the loop as fast as possible
	signed int  wx = x + width;
	if(wx > DRIVER_COLUMNS) wx = DRIVER_COLUMNS;
	if(x >= 0) { scraddr += x; wx -= x; }
	else img -= x;
	if(wx <= 0) return 2;

	signed int  tmp = y + height;
	if(tmp > endrow) tmp=endrow;
	if(y >= 0) { scraddr += y*DRIVER_COLUMNS; tmp -= y; }
	else img -= y*width;
	if(tmp <= 0) return 3;


	register unsigned char wy = tmp;
	do
    {
		PGM_P imgptr = img;
		register unsigned char *scrptr = scraddr;
        register unsigned char tmp1;
		for(tmp1=wx;tmp1;tmp1--)
				*scrptr++ = pgm_read_byte(imgptr++);  
		
		scraddr += DRIVER_COLUMNS;
		img += width;
	}
	while(--wy);

	return 0;
}
#endif


////
//RLE decompressing macro.
//upper 128 blocks can be addressed with this method.
////
#define RLE_get_next(img)                              				 		 \
{                                                              				 \
        if(cnt)  cnt--;                                        				 \
        else                                                   				 \
        {                                                      				 \
         byte = pgm_read_byte(img++);                                        \
         if(byte & 0x80)  cnt = pgm_read_byte(img++);      				     \
		 byte |= 0x80;													 	 \
        }                                                      				 \
}       



////
//note: disadvantage of the RLE is that we cannot address the image directly.
//		we always have to start from the beginnig, which takes a lot of expensive time.
//		execution time depends on distance between the coordinates and window start block, 
//		this could be partly solved by indexing the RLE stream somehow......
////
#ifdef BACKGROUND_DRAW_RLE

void background_draw_RLE( signed char x, signed char y, PGM_P img, uint8_t width, uint8_t  height)
{
	register unsigned char byte=0, cnt=0;
	register unsigned char endrow = window_get_height();
	register unsigned char *scraddr = window_get_ptr();
	if(x > 0) scraddr += x;
	if(y > 0) scraddr += y*DRIVER_COLUMNS;

    asm("clt");

	do
    {
		 if(y > endrow) break;

		 asm volatile("brts nxt"); //if(!T)
       	 if(y >= 0) asm("set"); // v ramci okna
		 asm volatile("rjmp nxt2"); 
		 asm volatile("nxt:");	  //else
		 scraddr += DRIVER_COLUMNS;
		 asm volatile("nxt2:");
		
       	 register unsigned char *scrptr = scraddr;

         register unsigned char tmp1 = width;
		 register signed char tmp2 = x;
         
		 do
	 	 {
	 			RLE_get_next(img);
				asm("brtc nextblk");
				//the zero test is fast so there's no need for another flag.
				if(tmp2>=0 && tmp2<DRIVER_COLUMNS) *scrptr++ = byte;
      	 		asm("nextblk:");
				tmp2++;
		 }
		 while(--tmp1);
		 
         y++;
	}
	while(--height);
}
#endif





