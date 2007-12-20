/*

	file:	overlay.c
	desc:	OVRELAY module implementation.

	author: Jaromir Dvorak (md@unicode.cz)

*/

#include <stdlib.h>
#include <avr/pgmspace.h>
#include "../video/driver.h"
#include "window.h"
#include "overlay.h"

#define DIV10(x)  ((x)/10)
#define DIV8(x)   ((x)>>3)
#define MOD10(x)  ((x)%10)
#define MOD2(x)   ((x)&1)
#define MOD4(x)   ((x)&3)
#define MOD8(x)   ((x)&7)
#define ODD(x)    MOD2(x)


unsigned char overlay_block_id = 0;
#ifdef OVERLAY_REMEMBER_BLOCKS
unsigned char s_blk[OVERLAY_BLOCK_COUNT];
unsigned char *s_addr[OVERLAY_BLOCK_COUNT];
#endif


/*
 * 
 * Clears all drawn overlay objects.
 *
 */

#ifdef OVERLAY_CLEARABLE 
void overlay_clear()
{
     goto a1;     //optimized
     while(1)
     {
         register unsigned char blk = s_blk[overlay_block_id];
         if(!driver_is_rammap(blk)) *s_addr[overlay_block_id] = blk;

#ifdef OVERLAY_INVERSE_PRIORITY
		 s_addr[overlay_block_id]=0;
#endif

a1:	 	 if(!overlay_block_id) return;
		 overlay_block_id--;
     }
}
#endif




/*
 *
 * overlay_draw(...)
 *
 * Note: This functions is pretty complex and should be as fast as possible, it's well optimized for speed.
 *		 Some compromises between spped / size, of course.
 *		 However, it could be rewritten to asm completely to save some more time. 
 *		 Special versions could be added to handle some special cases faster (no transparency, etc).
 * 
 *
 * Note: This is extended configurable version, so it may seem difficult to understand.  
 *		 See original version is in overlay.txt.
 *
 * 		 All configurations are designed to allow the optimizer to always do the best job, use -o2.
 *		 Every saved instructon is good in such time-critical system. 
 *		 For example, when configured, this version will compile with exactly the same size as that one in overlay.txt.
 *
 */

#ifdef OVERLAY_DRAW
#ifdef OVERLAY_DRAW_PARAM
unsigned char overlay_draw_param
(
#ifdef OVERLAY_DRAW_SAFE
	signed int x, signed int y, 
#else
	unsigned char x, unsigned char y,
#endif
	PGM_P img, 
	unsigned char width, unsigned char height,
	unsigned char param
)
#else //!OVERLAY_DRAW_PARAM
unsigned char overlay_draw
(
#ifdef OVERLAY_DRAW_SAFE
	signed int x, signed int y, 
#else
	unsigned char x, unsigned char y,
#endif
	PGM_P img, 
	unsigned char width, unsigned char height
)
#endif //OVERLAY_DRAW_PARAM



{
    	register unsigned char *scraddr = &driver_get_block(0,0);
		register unsigned char prex;
		register unsigned char prey;
		register unsigned char wndx = 0, wndy = DRIVER_MAXY; //removed by optimizer if not needed....
		
		//At first, do all the selected calculations, to make the loop as fast as possible.

#ifdef OVERLAY_DRAW_TRANSFORMATIONS		
		register signed char lp = width >> 1;
		register signed char pp = 1;
		if(param & 0x10) { img += lp-1;  pp=-pp; }
		if(param & 0x20) { img += (height-1) * lp; lp=-lp; }
#else	//!OVERLAY_DRAW_TRANSFORMATIONS	
		register unsigned char lp = width >> 1;
		register unsigned char pp = 1;
#endif
#ifdef OVERLAY_DRAW_COLORMODIFY			//XORing image?
		param &= 0x0F;
#endif


#ifdef OVERLAY_DRAW_WINDOW				//align to window?
		wndx = window_get_scrollX();
		x+=wndx;
		wndy = window_get_startline();
		y+=wndy;
		wndy += window_get_num_lines();
#endif

			
#ifdef OVERLAY_DRAW_SAFE				//cut image when it's exceeding, or return if it lies outside drawing area
		asm volatile("clt");
		wndx += DRIVER_RESX;
		register signed int tmp = x + width;
		if(tmp>wndx) tmp = wndx;
		if(x >= 0)
		{
			tmp -= x; 
			scraddr += DIV8(x);
			prex = MOD8(x);		   
		}
		else
		{
			prex = 0;
			x = abs(x) >> 1;

			asm volatile("brcc b0"); 
			tmp++;						//increase width
			asm volatile("set");		//flag for odd shift 
			asm volatile("b0:");

			img += x * pp;

		}
		if(tmp <= 0) return 2; 			//x coord out of drawing area
		width = (unsigned char)tmp;

	    tmp = y + height;
		if(tmp > wndy) tmp = wndy;
		if(y >= 0)
		{ 
			scraddr += DIV8(y) * DRIVER_COLUMNS;
			prey = MOD8(y) * 4;
			tmp -= y;				
		}
		else
		{ 
			img += abs(y) * lp;
			prey=0;
		}
		if(tmp <= 0) return 3;			//y coord out of draving area.
		height = (unsigned char)tmp;


#else	//!OVERLAY_DRAW_SAFE
		scraddr += DIV8(x);
		scraddr += DIV8(y) * DRIVER_COLUMNS;
 		prex = MOD8(x);
        prey = MOD8(y);
        prey*=4;
#endif


#ifndef OVERLAY_DRAW_IMGADDR 	//are we addresing the image?
		PGM_P imgptr = img;
#endif


		// now ready for drawing!


        do //source line loop
        {
			 register unsigned char x1 = 0;    //x1 for img (PGM).
			 register unsigned char inpix;


#ifdef OVERLAY_DRAW_IMGADDR
			PGM_P imgptr = img; //addresing or continuous bitstream?
#endif


#ifdef OVERLAY_DRAW_SAFE					//odd shift, optimized for faster inner loop
 			 //asm volatile("brtc b2"); 
			 if(SREG & 0x40)  //odd start
			 {
			  inpix=pgm_read_byte(imgptr+=pp);		
			  x1=1;
			 }
			 //asm volatile("b2:"); 
#endif

             register unsigned char x2 = prex;
             register unsigned char* scrptr = scraddr;  //block pointer

             while(1)
             {
			   unsigned char* sp;// = _overlay_realloc_block( scrptr );
			   //if(!sp) return 1;

			   //hand-inlined _overlay_realloc_block; saves few instructions when returning.
	           register unsigned char i = *scrptr; 
               if(driver_is_rammap(i))
               {
               	       i-=DRIVER_PGM_BLOCK_COUNT;
                       sp = (&_ramp[i * DRIVER_BLOCK_SIZE]);
               }
               else
               {
					   if(overlay_block_id >= OVERLAY_BLOCK_COUNT)
#ifdef OVERLAY_INVERSE_PRIORITY
					   overlay_block_id = 0;
					   unsigned char *lastptr = s_addr[overlay_block_id];
					   if(lastptr) *lastptr = s_blk[overlay_block_id];
#else
					   return 1;
#endif
#ifdef OVERLAY_REMEMBER_BLOCKS
   	    			   s_blk[overlay_block_id] = i;
      				   s_addr[overlay_block_id] = scrptr;
#endif
        	       	   sp = &_ramp[overlay_block_id * DRIVER_BLOCK_SIZE];

                       memcpy_P(sp, &_pgmp[i * DRIVER_BLOCK_SIZE], DRIVER_BLOCK_SIZE);
               	       *scrptr = driver_rammap_block(overlay_block_id++);
               }


               sp += prey;    //line in block
               sp += x2 >> 1; //column in block

               register unsigned char pix;
               register unsigned char outpix = *sp;

			   
			   for(;x2<8;x2++) //inner loop. should be as fast as possible.
       	       {
               	 if(!(x1&1)) 
				 {
					if(pp<0) { inpix = pgm_read_byte(imgptr--); goto newpixel; }  //removed as deadcode when not using transformations,
					inpix = pgm_read_byte(imgptr++); //lpm Z+
				 }

				 asm volatile ("swap %0" : "=r" (inpix) : "0" (inpix));

newpixel:		 pix = inpix & 0x0F;  //source pixel.


#ifdef OVERLAY_DRAW_ALPHA
                 if(pix != OVERLAY_DRAW_ALPHA)	//transparent?
#endif
                 {

#ifdef OVERLAY_DRAW_COLORMODIFY
				  pix ^= param;
#endif
                  if(x2 & 1) outpix &= 0xf0; 
				  else { outpix &= 0x0f;  asm volatile ("swap %0" : "=r" (pix) : "0" (pix)); }
		
                  outpix |= pix;
                 }

                 if(++x1 >= width) { *sp = outpix;  goto newline; } //last pixel in source line
                 if(x2 & 1) { *sp++ = outpix; outpix = *sp; }
               }

		       x2=0;
               scrptr++; //next block column
             }

newline:   	 img += lp; //removed by optimizer when not addressing.

			 if((prey+=4) >= (DRIVER_BLOCK_HEIGHT*4))
			 {
			 	prey=0; 
				scraddr+=DRIVER_COLUMNS; //next block line
				//if(!--row) return 0; 
			 }
       }
       while(--height);

       return 0;
}

#endif //OVERLAY_DRAW


/*
 *
 * Overlay primitives.
 *
 * Note: this simple API - _overlay_write_pixel_to_block and _overlay_realloc_block
 *		 can be used to implement all basic primitives.
 *
 * TODO: Implement primitives...
 * 
 */

#ifdef OVERLAY_PRIMITIVES

static inline unsigned char* _overlay_realloc_block(unsigned char* scraddr)										
{
        register unsigned char i = *scraddr;

        if(driver_is_rammap(i))
        {
       	       i-=DRIVER_PGM_BLOCK_COUNT;
               return (&_ramp[i * DRIVER_BLOCK_SIZE]);
        }

		if(overlay_block_id >= OVERLAY_BLOCK_COUNT)
#ifdef OVERLAY_INVERSE_PRIORITY
		overlay_block_id = 0;
		unsigned char *lastptr = s_addr[overlay_block_id];
		if(lastptr) *lastptr = s_blk[overlay_block_id];
#else
		return 0;
#endif

#ifdef OVERLAY_REMEMBER_BLOCKS
   	    s_blk[overlay_block_id] = i;
      	s_addr[overlay_block_id] = scraddr;
#endif

        unsigned char* sp = &_ramp[overlay_block_id * DRIVER_BLOCK_SIZE];

        memcpy_P(sp, &_pgmp[i * DRIVER_BLOCK_SIZE], DRIVER_BLOCK_SIZE);
        *scraddr = driver_rammap_block(overlay_block_id++);

		return sp;
}
	 
static inline void _overlay_write_pixel_to_block(unsigned char* ptr, unsigned char x, unsigned char y,  unsigned char color)
{
        ptr += y * 4;    
        ptr += x >> 1; 
		color &= 0x0f;
		register unsigned char outpix = *ptr;
		if(x & 1)  outpix &= 0xf0; 
		else { asm volatile ("swap %0" : "=r" (color) : "0" (color)); outpix &= 0x0f; }
        outpix |= color;
		*ptr = outpix;
}


//
// classic pupixel. Behaves just like in full graphic.
//
unsigned char overlay_putpixel (unsigned char x, unsigned char y, unsigned char color)
{
        unsigned char* scrptr = &window_get_block(DIV8(x), DIV8(y));
		unsigned char* blockptr = _overlay_realloc_block(scrptr);
		if(!blockptr) return 1;
		_overlay_write_pixel_to_block(blockptr, MOD8(x), MOD8(y), color);
		return 0;
}


#endif //OVERLAY_PRIMITIVES








