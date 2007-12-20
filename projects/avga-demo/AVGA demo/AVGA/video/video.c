/*

	file:	video.c
	desc:	video signal generator implementation.

	note:	the algorthim is well optimized. I spent a lot of time searching the best way.

	author: Jaromir Dvorak (md@unicode.cz)

*/


#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include "video.h"
#include "driver.h"


unsigned int c0;
unsigned char line, scroll;
unsigned char *sp, *ss;
volatile unsigned char wsync;


#ifdef VIDEO_USE_WINDOW
#include "../utils/window.h"

unsigned char *aw;
unsigned char height;

#else

unsigned char video_startline;
unsigned char video_scroll;

void video_set_colormask(unsigned char mask)
{
	register unsigned char tmp = video_scroll;
    tmp&=~0xF0;
    mask&=0xF0;
    tmp|=mask;
    video_scroll = tmp;
}

void video_set_scroll(unsigned char x, unsigned char y)
{
    register unsigned char tmp = video_scroll;
    tmp&=~7;
    x&=7;
    tmp|=x;
    video_scroll = tmp;
    tmp = video_startline;
    tmp&=~7;
    y&=7;
    tmp|=y;
    video_startline = tmp;
}

#endif



void video_init(sync_t *s)
{
	sp = ss = (unsigned char*)s;
	c0 = 0;
	__video_timer_setup_mode14();
	__video_timer_select_IRQ_OVF();

	VIDEO_TIMER_OCRA = VIDEO_XPOS;
	VIDEO_TIMER_OCRB_DDR |= 1 << VIDEO_TIMER_OCRB_PIN;

#ifdef VIDEO_USE_WINDOW
	window_reset();
#endif
}


inline unsigned int sync(void)
{
	unsigned int tick = 0;
	wsync = 0;
	while(!wsync) tick++;

	return tick;
}


ISR (VIDEO_TIMER_IRQ_CMP) //time to do a scanline, optimized
{
        if(scroll&0x08) 
			driver_throw_scanline(line, scroll);

        if(!--c0) //frame complete!
        {
		    wsync = 1;
        	__video_timer_select_IRQ_OVF();
        	__video_timer_clear_IRQ_OVF();
			return;
        }

#ifdef VIDEO_DOUBLE_SCANLINE
		if(c0&1) return; //only even lines
#endif

		line++;

#ifdef VIDEO_USE_WINDOW

		if(!--height) //window complete!
		{
			register unsigned char* ptr = aw;

			line = *ptr++;
			height = *ptr++;
			scroll = *ptr++;

			VIDEO_DDR = scroll & 0xF0;

 			aw = ptr;
		}
#endif
}


ISR (VIDEO_TIMER_IRQ_OVF) //time to do next sync pulse, optimized
{
		static unsigned int top;

        VIDEO_TIMER_ICR = top;

       	if(!c0)
        {
			  register unsigned int tmp;
              register uint8_t* ptr = sp;

first_pulse:  asm volatile("lpm %A0, %a1+ \r\n lpm %B0, %a1+" : "=r" (tmp) : "e" (ptr) );
			  if(!tmp) { ptr = ss; goto first_pulse; }
              top = tmp;
			  asm volatile("lpm %A0, %a1+ \r\n lpm %B0, %a1+" : "=r" (tmp) : "e" (ptr) );
              VIDEO_TIMER_OCRB = tmp;
			  asm volatile("lpm %A0, %a1+ \r\n lpm %B0, %a1+" : "=r" (tmp) : "e" (ptr) );
              c0 = tmp;

			  sp = ptr;
		}

        if(--c0 == VIDEO_YPOS)
        {

#ifdef VIDEO_USE_WINDOW
			register unsigned char* ptr = (unsigned char*)window_get_default();
			line = *ptr++;
			height = *ptr++;
			scroll = *ptr++;
			aw = ptr;
#else
			line = video_startline;
			scroll = video_scroll;
#endif

			VIDEO_DDR = scroll & 0xF0;

			__video_timer_select_IRQ_CMP();
			__video_timer_clear_IRQ_CMP();

        }
}












