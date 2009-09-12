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
#include "video.h"

// the following define is to restrict the number of columns to display.
// (in can help whilst debugging - reducing the number of columns to display
// means that debug code can then be added without running out of time)
#define COL_COUNT_VISIBLE (COL_COUNT)

render_handler render_handler_nondisplay ;
static render_handler render_handler_font ;
static render_handler render_handler_fontwide ;
static render_handler render_handler_fontblank ;

render_handler *g_render_handler = render_handler_nondisplay ;

static uint8_t s_DisplayRow = 0;
static uint8_t s_RowSlice = 0;			// The slice of the row that is being rendered (1-FONT_CHAR_HEIGHT)
static uint8_t s_FontPtrHi = 0;			// The font-slice that should be used to render this scanline.
static uint8_t s_HighlightColumn = 0;	// Which character should be inverted (unusual ordering here)
static uint8_t s_CurrentRowInfo = 0;	// row attributes for the character row currently being rendered.
static uint8_t s_CursorDelta;			// see render_handler_nondisplay for how this is used.
static uint8_t s_RowsCompleted ;		// the number of characters rows completed in this field.

// variables used to store pre-calculated values
static render_handler *s_precalc_render_handler ; // the next row's render handler
static uint8_t s_precalc_CurrentRowInfo ;	// the next row's RowInfo


static void _setup_next_row( void ) ;

// lookup table for which routine should render which sort of row...
static render_handler * PROGMEM gf_render_lookup[] =
{
// these match with all the potential ROWINFO values...
	render_handler_fontblank, // 0000 - non-visible normal
	render_handler_font,      // 0001 - visible, normal
	render_handler_fontblank, // 0010 - non-visible double height (top)
	render_handler_font,      // 0011 - visible double height (top)
	render_handler_fontblank, // 0100 - non-visible, normal (DOUBLE_HEIGHT_BOTTOM ignored)
	render_handler_font,      // 0101 - visible, normal (DOUBLE_HEIGHT_BOTTOM ignored)
	render_handler_fontblank, // 0110 - non-visible, double height (bottom)
	render_handler_font,      // 0111 - visible, double height (bottom)
	render_handler_fontblank, // 1000 - non-visible double width
	render_handler_fontwide,  // 1001 - visible, double width
	render_handler_fontblank, // 1010 - non-visible double width, double height (top)
	render_handler_fontwide,  // 1011 - visible double width, double height (top)
	render_handler_fontblank, // 1100 - non-visible, double width (DOUBLE_HEIGHT_BOTTOM ignored)
	render_handler_fontwide,  // 1101 - visible, double width (DOUBLE_HEIGHT_BOTTOM ignored)
	render_handler_fontblank, // 1110 - non-visible, double width, double height (bottom)
	render_handler_fontwide   // 1111 - visible, double width, double height (bottom)
};



void render_handler_nondisplay( void )
{
uint16_t scanline = g_current_scanline ;
uint16_t first_textsafe ;
uint16_t last_display ;

	if (g_options & (1<<OPT_PAL))
	{
		first_textsafe = SCANLINE_FIRST_TEXTSAFE_PAL ;
		last_display = SCANLINE_LAST_DISPLAY_PAL ;
	}
	else
	{
		first_textsafe = SCANLINE_FIRST_TEXTSAFE_NTSC ;
		last_display = SCANLINE_LAST_DISPLAY_NTSC ;
	}

	if (scanline == first_textsafe)
	{
		// This line isn't actually rendered (unless, in future, inversed screen/rows are implemented, in which case a white line
		// above the top stripe of font data will be required)
		// it's the line before the first font stripe that's displayed.
		// Some pre-calculations are carried out here, the remainder in _setup_next_row().
		s_DisplayRow = g_row_scroll_offset - 1; // it will be incremented to the first-row in _setup_next_row()
		s_RowsCompleted = 255; // it will be incremented to 0 in _setup_next_row()
		s_RowSlice = (FONT_CHAR_HEIGHT - 1); // triggers _setup_next_row() to re-set stuff for this first row.
		
		// increment the framecount (leaving the top 3 bits alone)
		uint8_t temp = g_framecount &0b00011111 ;
		temp += 1 ;
		temp = temp & 0b00011111 ;
		g_framecount = (g_framecount & 0b11100000) | temp ;
		
		// do the pre-calculations that would normally done in previous scanlines
		s_precalc_CurrentRowInfo = g_rowinfo[ g_row_scroll_offset ] ;
		s_precalc_render_handler = (render_handler *)pgm_read_word( &gf_render_lookup[ s_precalc_CurrentRowInfo & 0b1111 ] );
		
		// cursor delta affects is how the cursor is displayed.
		// the value is calculated here once per screen, when there's plenty of free time.
		// if the '
		// a delta of 0 means 'block cursor'
		// a delta of (COL_COUNT+1) means 'underscore cursor'
		// a delta of 2 * (COL_COUNT+1) means 'no cursor'.
		s_CursorDelta= 0 ;
		if (g_options & (1<<OPT_HIDECURSOR)) s_CursorDelta+= (2 * (COL_COUNT_VISIBLE + 1)) ;
		if (!(g_options & (1<<OPT_BLOCKCURSOR))) s_CursorDelta+= (COL_COUNT_VISIBLE + 1) ;
		
		SPCR = (1<<SPE)|(1<<MSTR)|(1<<CPHA); // enable SPI output (with phase alteration)


		// if HIDEDISPLAY is active, trick the setup next row into thinking it's finished
		// drawing everything (in which case, this routine will be repeatedly called instead).
		if (g_options & (1<<OPT_HIDEDISPLAY)) s_RowsCompleted = 50 ;

		_setup_next_row();
	}
	
	if (scanline == last_display)
	{
		g_sync_handler = sync_handler_field_header ;
	}
}


static inline void _setup_next_row()
{
	CYCLES(4);
	SPSR = (1 << SPI2X); // as fast as possible - F_CPU / 2.
	PIXEL_DISABLE;
	// reset back to the normal speed for SPI.

	if (s_RowSlice != (FONT_CHAR_HEIGHT - 1))
	{
		// this is not the last scanline.
		
		if (s_RowSlice == (FONT_CHAR_HEIGHT - 2))
		{
			// the next font slice is the last in the font - handle the cursor appropriately.
			if (s_HighlightColumn > COL_COUNT_VISIBLE)
			{ // if it's not already being displayed, modify the value to see if it should be...
				s_HighlightColumn += (COL_COUNT_VISIBLE + 1);
			}
		}
		
		uint8_t inc = 1 ;
		if (s_CurrentRowInfo & (1<<ROWINFO_DOUBLE_HEIGHT))
		{
			if (s_RowSlice & 0b1) inc++ ;
			if (!(s_CurrentRowInfo & (1<<ROWINFO_DOUBLE_HEIGHT_BOTTOM))) inc++ ;
			inc = inc & 0b1 ;
		}
		s_RowSlice++ ;
		s_FontPtrHi += inc ; // point to the next block of 256 character bitslices
		
		//pre-calculate the rowInfo value to use for the next character row.
		uint8_t nextrow = s_DisplayRow + 1 ;
		if (nextrow >= ROW_COUNT) nextrow = 0 ;
		s_precalc_CurrentRowInfo = g_rowinfo[ nextrow ] ;
		
		//pre-calculate the render handler to use for the next character row...
		s_precalc_render_handler = (render_handler *)pgm_read_word( &gf_render_lookup[ s_precalc_CurrentRowInfo & 0b1111 ] );
	}
	else
	{
		// we have just drawn the last of FONT_CHAR_HEIGHT rows in this row.
		// set-up the next row...
		s_RowsCompleted += 1 ;
		if (s_RowsCompleted >= ROW_COUNT)
		{
			// We've drawn enough rows - nothing more to display!
			g_render_handler = render_handler_nondisplay ;
			return ;
		}

		// move onto the next character row...
		s_DisplayRow ++;
		if (s_DisplayRow >= ROW_COUNT) s_DisplayRow = 0;

		// now set the render handler and rowinfo for the next row.
		// these were calculated in the previous scanline(s) to save some precious cycles.
		// (or in render_handler_firstline)
		
		g_render_handler = s_precalc_render_handler ;
		s_CurrentRowInfo = s_precalc_CurrentRowInfo;
		
		s_RowSlice = 0;
		s_FontPtrHi = ((uint16_t)f_font >> 8);

#if FONTBANK_COUNT > 1		
		uint8_t fontbank = s_CurrentRowInfo >> 4 ;
		fontbank = fontbank * FONT_CHAR_HEIGHT ;
		s_FontPtrHi += fontbank ;
#endif
		
		if (	(s_CurrentRowInfo & ((1<<ROWINFO_DOUBLE_HEIGHT) | (1<<ROWINFO_DOUBLE_HEIGHT_BOTTOM)))
				== ((1<<ROWINFO_DOUBLE_HEIGHT) | (1<<ROWINFO_DOUBLE_HEIGHT_BOTTOM)))
		{
			s_FontPtrHi += 4 ;
		}
		
		// Set the Highlight column if a cursor is required.
		s_HighlightColumn = 0; // default
		if (
			(s_DisplayRow == g_current_row)
				&&
			(g_framecount & 0b10000)
			)
		{
			s_HighlightColumn = (COL_COUNT_VISIBLE - g_current_col) - (s_CursorDelta) ;
			//if (g_options & (1<<OPT_BLOCKCURSOR)) s_HighlightColumn -= (COL_COUNT + 1);
		}
		
	}
	
}


static void render_handler_fontblank( void )
{ // 221c to here (from scanline start) == 13.8us
uint8_t i = COL_COUNT_VISIBLE ;

	// font display line where no characters are visible
	// Note that the cursor will never be displayed in a non-visible line
	// (as the line will have been normalised beforehand).
	SPCR = 0; //1c
	SPCR = (1<<SPE)|(1<<MSTR)|(1<<CPHA); //2c

	_delay_cycles(7);
	while(i > 0)
	{
		_delay_cycles(11);
		PIXEL_DISABLE ;
		_delay_cycles(1);
		SPDR = 0b00000000;
		PIXEL_ENABLE;
		i--;	
	}
	_setup_next_row();
}

static void render_handler_font( void )
{
uint8_t *char_ptr = &(g_display[ s_DisplayRow ][0]) ;

	asm("\n\t"
			// initialise registers
			// r21: which character position to invert (for cursor)
			// r22: DDRB setting for "enable pixel output"
			// r23: DDRB setting for "disable pixel output"
			// r24: bit-pattern of previous character
			//	    This is stored so that the 9th bit can duplicate the 8th bit.
			// r25: count of characters left to display
			// X  : (r26,r27) address of next character to output
			// r30: (z-lo) lo-byte of font lookup table (e.g. the character to lookup).
			// r31: (z-hi) hi-byte of font lookup table (256-byte aligned - determines which slice)
			"	lds		r21, s_HighlightColumn	\n\t"
			"	ldi		r22, %[enable_pixel]			\n\t"
			"	ldi		r23, %[disable_pixel]			\n\t"
			"	ldi		r24, 0x00						\n\t"
			"	ldi		r25, %[visible_column_count]	\n\t"
			"	lds		r31, s_FontPtrHi			\n\t"
			"loop:										\n\t"
			"	ld		r30, %a[char_ptr]+   ; straight into z-lo\n\t"
			"	lpm		__tmp_reg__,Z					\n\t"
			"	cp		r21, r25						\n\t"
			"	brne	.+2 		         ; invert if this is the current cursor position\n\t"
			"	com		__tmp_reg__						\n\t"
			"	sbrs	r24, 0	             ; skip turning off the pixel output if we want pixel 9 to be white\n\t"
			"	out		%[DDR],	r23						\n\t"
			"	mov		r24, __tmp_reg__				\n\t"
			"	out		%[_SPDR], __tmp_reg__			\n\t"
			"	out		%[DDR], r22	        ; switch MOSI pin to output\n\t"
			"	rjmp	.+0		;2 cycle nop			\n\t"
			"	subi	r25, 0x01						\n\t"
			"	brne	loop							\n\t"
			:
			:
			[char_ptr]		"x" (char_ptr),
			[visible_column_count] "M" (COL_COUNT_VISIBLE),
			[enable_pixel]  "M" ((1<<SIG_PIXEL_PIN)|(1<<SIG_SYNC_PIN)),
			[disable_pixel] "M" ((0<<SIG_PIXEL_PIN)|(1<<SIG_SYNC_PIN)),
			[DDR]			"I" (_SFR_IO_ADDR(DDRB)),
			[_SPDR]			"I" (_SFR_IO_ADDR(SPDR))
			:
			"r21","r22","r23","r24","r25","r30","r31"
		);
		
	CYCLES(2); // wait until the SPI has finished squirting

	_setup_next_row();
}

static void render_handler_fontwide( void )
{
uint8_t byteout;
uint8_t i = (COL_COUNT_VISIBLE >> 1) - 1; // only display half the number of characters
uint8_t *char_ptr = &(g_display[ s_DisplayRow ][0]) ;
uint8_t *font_ptr = (uint8_t*)(s_FontPtrHi << 8) ;
uint8_t lastbyteout = 0;
uint8_t invcount = s_HighlightColumn;
uint8_t c;
uint8_t temp = (1<<SIG_PIXEL_PIN)|(1<<SIG_SYNC_PIN);

	_delay_cycles(1);
	SPSR = 0;// turn off the SPI speed-doubling.
	invcount -= (COL_COUNT_VISIBLE >> 1) + 1;
	
	c = *char_ptr++;
	asm (	"movw r30, %1    \n\t"
			"add  r30, %2    \n\t"
			"lpm  %0 , z     \n\t"
			: "=r" (byteout)
			: "r" (font_ptr),
			  "r" (c)
			: "r30","r31" ) ;
	if (invcount==i) byteout = (uint8_t)~byteout;
	SPDR = byteout ;
	DDRB = temp ;
	asm(""::"r" (temp)); // force a preload of temp.
	_delay_cycles(3);

	while(i > 0)
	{
		lastbyteout = byteout ;
		_delay_cycles(7);
		i--;

		c = *char_ptr++ ;
		// the following code is equivalent to byteout = pgm_read_byte( font_ptr + c ),
		// except that it knows that font_ptr will be 256-byte aligned, hence a single add
		// is required.
		asm (	"movw r30, %1    \n\t"
				"add  r30, %2    \n\t"
				"lpm  %0 , z     \n\t"
				: "=r" (byteout)
				: "r" (font_ptr),
				  "r" (c)
				: "r30","r31" ) ;
		if (invcount==i) byteout = (uint8_t)~byteout;
		if (!(lastbyteout & 0b1)) PIXEL_DISABLE ;
		_delay_cycles(3);
		SPDR = byteout;
		PIXEL_ENABLE;
	}
	_delay_cycles(20);
	_setup_next_row();
}

