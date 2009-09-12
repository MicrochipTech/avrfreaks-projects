/*
Copyright (c) 2009, Nigel Batten.
Contactable at <firstname>.<lastname>@mail.com

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

	1.	Redistributions of source code must retain the above copyright
		notice, this list of conditions and the following disclaimer.
	2.	Redistributions in binary form must reproduce the above copyright
		notice, this list of conditions and the following disclaimer in the
		documentation and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES,
INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
COPYRIGHT HOLDER BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
OF SUCH DAMAGE.

===============================================
*/

#include <avr/io.h>
#include <avr/sleep.h>
#include "video.h"

sync_handler *g_sync_handler = sync_handler_field_header;

inline void sync_handler_display( void )
{
/*
	SYNC_ON has already been called.
	4us Sync (during which the char_rx is called)
	8us pause (during which the char_handler is called)
*/
	g_char_poll();
	CYCLES( CYCLES_HSYNC - (CYCLES_CHAR_POLL + CYCLES_SYNC_HANDLER_ENTRY ) ) ;
	SYNC_OFF ;
	g_char_process();
	sleep_cpu();
	
	// There was a dalliance with a different method of timing the start of drawing:
	//
	// 	Go into a tight loop to wait for the timer flag to be set.
	// 	Adjust the jitter (caused by the 0-3cycle timing jitter from a 4cycle loop)
	//
	// The loop + adjustment time overhead ended up being the same (or slightly longer)
	// that going to sleep and waking up again.
	//
	// Because the sleep method is far simpler, it's been put back in.
}

inline void sync_handler_field_header( void )
{
/*
	SYNC_ON has already been called.
	
	PAL:
	This routine will run for 8 scanlines, with the following half-scanline pattern:
	6 short pulses
	5 broad pulses
	5 short pulses
	
	NTSC:
	This routine will run for 9 scanlines, with the following half-scanline pattern:
	6 short pulses
	6 broad pulses
	6 short pulses
*/
	// 2 PUSH instructions on entry, hence 4c fewer delay cycles required.
	CYCLES( CYCLES_SHORTSYNC - (CYCLES_SYNC_HANDLER_ENTRY + 4) ) ;
	SYNC_OFF ;
	
	// Sneak in a loop if the output is to be suppressed...
	if (g_options & (1<<OPT_SUPPRESSOUTPUT))
	{
		SYNC_ON ; // this pulls down the output to 0v.
		while ( g_options & (1<<OPT_SUPPRESSOUTPUT))
		{
			g_char_poll();
			g_char_process();
			sleep_cpu();
		}
		SYNC_OFF ; // this returns sync to a normal level.
	}


	if (g_options & (1<<OPT_PAL))
	{
		OCR1A = CLOCK_HALFSCANLINE_PAL ;
		OCR1B = CLOCK_LONGSYNC_PAL ;
	}
	else
	{
		OCR1A = CLOCK_HALFSCANLINE_NTSC ;
		OCR1B = CLOCK_LONGSYNC_NTSC ;
	}


	g_char_poll();
	g_char_process();

	
//	TIMER1INTERRUPTS |= (1<<OCIE1B) ; // enable the second interrupt.
//	TIMER1INTERRUPTS |= (1<<OCIE1A) ; // enable the scanline interrupt.
	
	uint8_t i = 5 ; // 6 shorts in total (one has already been done).
	while( i-- )
	{
		sleep_cpu(); // will wake up at the longsync point. (not used in shortsync halflines)
		sleep_cpu(); // will wake up at the next half/whole scanline boundary.
		SYNC_ON ;
		CYCLES( CYCLES_SHORTSYNC - ( 2 ) ) ;
		SYNC_OFF ;
		g_char_poll() ;
		g_char_process();
	}
	
	sleep_cpu(); // will wake up at the longsync point (at the end of the last shortsync halfline)
	
	if (g_options & (1<<OPT_PAL))
	{
		i = 5 ;
	}
	else
	{
		i = 6 ;
	}
	while( i-- )
	{
		sleep_cpu(); // will wake up at the start of a whole/half scanline boundary
		SYNC_ON ;
		g_char_poll();
		g_char_process();
		sleep_cpu(); // will wake up at the longsync point
		SYNC_OFF ;
	}

	if (g_options & (1<<OPT_PAL))
	{
		i = 4 ; // only 4 in the loop. The extra one is done afterwards whilst changing back to full-scanline timing.
	}
	else
	{
		i = 5 ; // only 5 in the loop. The extra one is done afterwards whilst changing back to full-scanline timing.
	}
	
	while( i-- )
	{
		sleep_cpu(); // will wake up at the whole/half scanline boundary
		SYNC_ON ;
		CYCLES( CYCLES_SHORTSYNC - (2)) ;
		SYNC_OFF ;
		g_char_poll();
		g_char_process();
		sleep_cpu(); // will wake up at the longsync point (not used in shortsync halflines)
	}

	// Reset the interrupt back to the normal _whole_ scanline.
	if (g_options & (1<<OPT_PAL))
	{
		OCR1A = CLOCK_SCANLINE_PAL ;
		// 12c this route.
	}
	else
	{
		OCR1A = CLOCK_SCANLINE_NTSC ;
		CYCLES(1);
		// 12c this route.
	}
	// we are now just before the start of the last shortsync halfline...
	// there are 12 clock cycles since we last woke up.
	CYCLES(CYCLES_HSYNC - (12)); // we're now at the start of the last half-line.
	SYNC_ON;
	CYCLES(CYCLES_SHORTSYNC-(2));
	SYNC_OFF;
	
	// restore the normal display interrupt.
	if (g_options & (1<<OPT_PAL))
	{
		OCR1B = CLOCK_DISPLAYSTART_PAL ;
	}
	else
	{
		OCR1B = CLOCK_DISPLAYSTART_NTSC ;
	}
//	TIMER1INTERRUPTS &= ~(1<<OCIE1B) ; // disable the second interrupt.
//	TIMER1INTERRUPTS &= ~(1<<OCIE1A) ; // disable the scanline interrupt.
	
	// no need to call the f_char_rx or g_char_handler routines in this last half-scanline.
	// - they only need calling once per whole scanline.

	g_sync_handler = sync_handler_display ;
	g_render_handler = render_handler_nondisplay ;
	if (g_options & (1<<OPT_PAL))
	{
		g_current_scanline = SCANLINE_FIRST_DISPLAY_PAL - 1 ;
	}
	else
	{
		g_current_scanline = SCANLINE_FIRST_DISPLAY_NTSC - 1 ;
	}
}
