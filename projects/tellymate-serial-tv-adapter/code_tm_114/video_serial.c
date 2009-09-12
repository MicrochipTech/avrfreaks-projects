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

#include "video.h"
#include "delay_x.h"
#include <avr/pgmspace.h>
#include <avr/wdt.h>
#include <avr/interrupt.h>
#include <avr/boot.h>

#define CHAR_NUL	0x00
#define CHAR_BEL	0x07
#define CHAR_BS		0x08
#define CHAR_TAB	0x09
#define CHAR_LF		0x0A
#define CHAR_FF		0x0C
#define CHAR_CR		0x0D
#define CHAR_DLE	0x10
#define CHAR_CAN	0x18
#define CHAR_ESC	0x1B
#define CHAR_SPACE	0x20
#define CHAR_DEL	0x7F

// Character polling routines.
// One of these routines is allways called once-per-scanline to determine what to do with any received character.
// There is one per 'state' of the state machine
// they must run in exactly CYCLES_CHAR_POLL cycles (including the 7c in and 4c out overheads)
// because they may be called during/between sync-pulses which need accurate timing.
char_poll poll_ground ; // normal, default state.
char_poll poll_esc ;	// waiting for the command specifier
char_poll poll_param ;	// waiting for a parameter
char_poll poll_dle ;	// waiting for a character to output directly to the screen.
char_poll poll_split ;	// temporary state to process a command that is split across 2 scanlines

// pointer to the current character polling routine:
char_poll *g_char_poll = poll_ground;



// Character processing routines
// One of these routines is allways called once-per-scanline.
// *Which* routine is usually selected by the (called-earlier-in-the-scanline) character polling routine.
// They perform all manner of functions, ranging from "housekeeping" when no characters are received, through
// outputting normal characters, all the way up to resetting the device when a 'reset' code is received.
char_process nul_process ;		// Does row normalisation. Importantly, this will be called even if *no* character has been called.
char_process output_char ;	// generic 'output this character' handler.
char_process control_default ; // handler for not-understood control characters - does nothing.
char_process control_BEL ;
char_process control_TAB ;
char_process control_BS ;
char_process control_LF ;
char_process control_FF ;
char_process control_CR ;
char_process control_ESC ;
char_process control_DLE ;
char_process control_CAN ;

// special character handlers for whilst in DLE state.
// they are functionally identical to each other except for the state that they return to after a (non-null) character is received.
char_process dle_ground ;
char_process dle_esc ;
char_process dle_param ;

// escape code specifiers
// These routines are called according to which character was received after an escape code.
// Any escape code that requires parameters are handled by the generic parameterised command
char_process esc_default ; // do nothing
char_process cursor_up ;
char_process cursor_up_scroll ;
char_process cursor_down ;
char_process cursor_right ;
char_process cursor_left ;
char_process cursor_home ;
char_process cursor_show ;
char_process cursor_hide ;
char_process cursor_save ;
char_process cursor_restore ;
char_process clear_start_of_screen ;
char_process clear_line_from_start ;
char_process clear_end_of_screen ;
char_process clear_end_of_line ;
char_process clear_screen ;
char_process clear_line ;
char_process set_overflow_on ;
char_process set_overflow_off ;
char_process reset __attribute__((noreturn)) __attribute((noinline));
//char_process FILL_SCREEN ;
char_process show_state ;
char_process cursor_send ;

// generic escape-code parameter handlers.
char_process esc_command_with_param ; // handles the start of a parameterised command (currently Y,y,x,_,^,`,~ or R)
char_process generic_param ; // handles receipt of a parameter
#define CMD_CURSOR_DIRECT_ADDRESS 'Y'
#define CMD_SET_ROW_ATTRIBUTE '_'
#define CMD_SET_ENABLE 'x'
#define CMD_SET_DISABLE 'y'
#define CMD_SEND_DATA '^'
#define CMD_READ_CHAR_RC '`'
#define CMD_TRANSMIT_ENABLE '~'
#define CMD_PROGRAM_FONTBANK_PAGE 'R'

// which character handler should be called?
char_process *g_char_process = nul_process;

// array to lookup which character handler should be called for which control codes.
char_process * PROGMEM f_control_lookup[ 32 ] =
{
	nul_process,				// 0  [does housekeeping]
	control_default ,// 1
	control_default ,// 2
	control_default ,// 3
	control_default ,// 4
	control_default ,// 5
	control_default ,// 6
	control_BEL ,	// 7
	control_BS ,		// 8
	control_TAB ,	// 9
	control_LF ,		// 10
	control_default ,// 11
	control_FF ,		// 12
	control_CR ,		// 13
	control_default ,// 14
	control_default ,// 15
	control_DLE ,	// 16
	control_default ,// 17
	control_default ,// 18
	control_default ,// 19
	control_default ,// 20
	control_default ,// 21
	control_default ,// 22
	control_default ,// 23
	control_CAN ,	// 24
	control_default ,// 25
	control_default ,// 26
	control_ESC ,	// 27
	control_default ,// 28
	control_default ,// 29
	control_default ,// 30
	control_default ,// 31
};

// array to lookup which character handler should be called for which escaped characters... (e.g. immediately following an <ESC>)
char_process * PROGMEM f_esc_lookup[] =
{
	cursor_up ,			// A	cursor up (non-scrolling/wrapping)
	cursor_down ,		// B	cursor down (non-scrolling/wrapping)
	cursor_right ,		// C	cursor right (non-scrolling/wrapping)
	cursor_left ,		// D	cursor left (non-scrolling/wrapping)
	clear_screen , 		// E	clear screen and home cursor [to (0,0)]
	esc_default ,		// F
	esc_default ,		// G
	cursor_home ,		// H	home cursor [to (0,0)]
	cursor_up_scroll , 	// I	cursor up (scrolling if already at top of screen)
	clear_end_of_screen ,//J	clear to end of screen (inclusive)
	clear_end_of_line ,	// K	clear to end-of-line
	esc_default ,		// L
	esc_default ,		// M
	esc_default ,		// N
	esc_default ,		// O
	esc_default ,		// P
	show_state ,		// Q
#ifdef PROGRAMMABLE_FONTS
	esc_command_with_param ,// R	Program fontbank page (f,p follows)
#else
	esc_default ,			// R
#endif
	esc_default ,		// S
	esc_default ,		// T
	esc_default ,		// U
	esc_default ,		// V
	esc_default ,		// W
	esc_default ,		// X
	esc_command_with_param ,// Y	Direct Cursor Address (DCA). 2 parameters follow.
	esc_default ,		// Z
	esc_default ,		// [
	esc_default ,		// backslash
	esc_default ,		// ]
	esc_command_with_param ,// ^	Send Data (1 parameter follows)
	esc_command_with_param ,// _	set row attributes
	esc_command_with_param ,// `	Send Character At (r,c follows)
	esc_default ,		// a
	clear_start_of_screen ,	// b	clear to start-of-screen (inclusive)
	esc_default ,		// c
	esc_default ,		// d
	cursor_show ,		// cursor on
	cursor_hide ,		// cursor off
	esc_default ,		// g
	esc_default ,		// h
	esc_default ,		// i
	cursor_save ,		// j	save cursor position
	cursor_restore ,	// k	restore cursor position
	clear_line ,		// l	clear current line and move to start
	esc_default ,		// m
	esc_default ,		// n
	clear_line_from_start ,// o	clear from start of line (inclusive)
	esc_default ,		// p
	esc_default ,		// q
	esc_default ,		// r
	esc_default ,		// s
	esc_default ,		// t
	esc_default ,		// u
	set_overflow_on ,	// v	line overflow on
	set_overflow_off ,	// w	line overflow off
	esc_command_with_param ,// x	enable setting
	esc_command_with_param ,// y	disable setting
	reset ,				// z	reset to power-on state
	esc_default	,		// {
	cursor_send ,		// |	Send Character (returns the character under the cursor)
	esc_default	,		// }
	esc_command_with_param ,// ~ transmit-enable 3 parameters follow.
	esc_default			// DEL
} ;

// routines called when parameters are received [a routine is called once-per parameter received]
static void param_cursor_direct_address( void ) ;
static void param_set_row_attribute( void ) ;
static void param_set_enable( void ) ;
static void param_set_disable( void ) ;
static void param_transmit_enable( void ) ;
static void param_send_data( void ) ;
static void param_send_char_rc( void ) ;
#ifdef PROGRAMMABLE_FONTS
static void param_program_fontbank_page( void ) ;
static void program_fontbank_chunk( uint8_t fontbank, uint8_t fontchunk ) __attribute__ ((noinline)) __attribute__((section (".boot"))) __attribute__((noreturn));
#endif

static void _output_char( void );
static void _cursor_up( uint8_t ) ;
static void _cursor_down( uint8_t );
static void _clear_screen( void ) ;
static void _normalise_current_row( void ) ;

static void _mem_clear( uint8_t *, uint8_t ) ;

static __attribute__((noinline)) void _mem_clear( uint8_t *p , uint8_t len )
{
// Clear len bytes of memory from p onwards.
// len may be zero
// len MUST NOT BE GREATER THAN 40.
// If len is greater than 40, everything will collapse in a heap. Don't do it.
//
// Why is this written as an inline assembler statement, rather than a .S external file?
// 2 reasons
// 1. Register clobbering.
// When compiling a call to a .S function, the compiler has to assume that all the
// "call-saved" registers have been clobbered by the remote function. That essentially
// wipes out half of its working registers around that call.
// If inline asm is used, C knows exactly which registers have been clobbered.
// 2. parameter passing
// If calling a .S function, the compiler uses the convention of passing parameters
// in r24 downwards. These parameters then need shuffling to the correct register.
// If calling an inline asm function, the compiler can see exactly which registers
// are required to be populated (so only does it once)
//
// These reasons only save a few cycles, but when things are really tight, inline asm
// just has the edge.
	asm (
			"	ldi	r30, lo8(pm(_mem_clear_end_));load pointer to the end of function into Z\n\t"
			"	ldi r31, hi8(pm(_mem_clear_end_));ditto\n\t"
			"	sub	r30, %1;move backwards however many instructions needed\n\t"
			"	sbc	r31, __zero_reg__\n\t"
			"	ijmp;\n\t"
			"	st		%a0+,__zero_reg__ ;40\n\t"
			"	st		%a0+,__zero_reg__ ;39\n\t"
			"	st		%a0+,__zero_reg__ ;38\n\t"
			"	st		%a0+,__zero_reg__ ;37\n\t"
			"	st		%a0+,__zero_reg__ ;36\n\t"
			"	st		%a0+,__zero_reg__ ;35\n\t"
			"	st		%a0+,__zero_reg__ ;34\n\t"
			"	st		%a0+,__zero_reg__ ;33\n\t"
			"	st		%a0+,__zero_reg__ ;32\n\t"
			"	st		%a0+,__zero_reg__ ;31\n\t"
			"	st		%a0+,__zero_reg__ ;30\n\t"
			"	st		%a0+,__zero_reg__ ;29\n\t"
			"	st		%a0+,__zero_reg__ ;28\n\t"
			"	st		%a0+,__zero_reg__ ;27\n\t"
			"	st		%a0+,__zero_reg__ ;26\n\t"
			"	st		%a0+,__zero_reg__ ;25\n\t"
			"	st		%a0+,__zero_reg__ ;24\n\t"
			"	st		%a0+,__zero_reg__ ;23\n\t"
			"	st		%a0+,__zero_reg__ ;22\n\t"
			"	st		%a0+,__zero_reg__ ;21\n\t"
			"	st		%a0+,__zero_reg__ ;20\n\t"
			"	st		%a0+,__zero_reg__ ;19\n\t"
			"	st		%a0+,__zero_reg__ ;18\n\t"
			"	st		%a0+,__zero_reg__ ;17\n\t"
			"	st		%a0+,__zero_reg__ ;16\n\t"
			"	st		%a0+,__zero_reg__ ;15\n\t"
			"	st		%a0+,__zero_reg__ ;14\n\t"
			"	st		%a0+,__zero_reg__ ;13\n\t"
			"	st		%a0+,__zero_reg__ ;12\n\t"
			"	st		%a0+,__zero_reg__ ;11\n\t"
			"	st		%a0+,__zero_reg__ ;10\n\t"
			"	st		%a0+,__zero_reg__ ;9\n\t"
			"	st		%a0+,__zero_reg__ ;8\n\t"
			"	st		%a0+,__zero_reg__ ;7\n\t"
			"	st		%a0+,__zero_reg__ ;6\n\t"
			"	st		%a0+,__zero_reg__ ;5\n\t"
			"	st		%a0+,__zero_reg__ ;4\n\t"
			"	st		%a0+,__zero_reg__ ;3\n\t"
			"	st		%a0+,__zero_reg__ ;2\n\t"
			"	st		%a0+,__zero_reg__ ;1\n\t"
			"_mem_clear_end_:\n\t"
			:: "x" (p),"r" (len) : "r30","r31" ) ; // %0 is p (put into X), %1 is len (put anywhere). R30 and r31 are clobbered.
}

// ===================================================================
// Character Receive routines
// Each of these must fit into 45 cycles
// (including entry (7c) and exit (4c) overheads)
//
// ===================================================================
// Ground State
//	Control characters obeyed
//	All other characters output
void poll_ground( void )
{
	if (USART0_REG_A & (1<<USART0_BIT_RXC))
	{ // +2c
        uint8_t c = USART0_DATA ; // 1c
		if (c)
		{ // +2c
			g_char = c ; // 2c
			
			// NULs have already been dropped!
			if (g_char < CHAR_SPACE)
			{ // +2c into here
				// control handler...
                CYCLES( CYCLES_CHAR_POLL - ((11) + (2 + 1 + 2 + 2 + 2 + 16 + 2))) ; // 7c free
				g_char_process = (char_process *) pgm_read_word( &f_control_lookup[ g_char ] ) ; // 16c
                // 2c jump to end.
			}
			else
			{ // +3c
                CYCLES( CYCLES_CHAR_POLL - ((11) + (2 + 1 + 2 + 2 + 3 + 6 + 2))) ;
				g_char_process = output_char ; // 6c
                // 2c jump to end
			}
		}
		else
		{ // +3c
			// there are no characters to process.
            CYCLES( CYCLES_CHAR_POLL - ((11) + (2 + 1 + 3 + 6 + 2))) ;
			g_char_process = nul_process ; // 6c
            // 2c jump to end
		}
	}
	else
	{ // 3c to here.
		CYCLES( CYCLES_CHAR_POLL - ((11)+(3 + 6)) ) ;
		g_char_process = nul_process ; // 6c
        // no 2c jump to end.
	}
}

// Escape State
//	Control characters obeyed
//	Known specifiers processed
//	Unknown specifiers ignored (and drop back to ground state)
void poll_esc( void )
{
	if (USART0_REG_A & (1<<USART0_BIT_RXC))
	{ // +2c
        uint8_t c = USART0_DATA ; // 1c

		g_char = c ; // 1c
		// Note: NULs are (now) handled by the control-code function look-up.
		// Note: No control routines change the state
		if (c < CHAR_SPACE)
		{ // 2c
			// this is a C0 character... execute it as normal.
			CYCLES( CYCLES_CHAR_POLL - ((11) + (2 + 1 + 1 + 2 + 13 + 2 + 4))) ;
			g_char_process = (char_process *) pgm_read_word( &f_control_lookup[ g_char ] ) ; // 13c
			// 2c jump then 4c store.
		}
		else
		{ //  3c
			// this a GL character... Is it one that I understand as an escape-sequence.
			g_char_poll = poll_ground ; // default is to return to non-escaped state. 6c
			c -= 'A' ; // 2c   Note: since c is unsigned, any characters less than 'A' are now greater than 'DEL'
			if (c <= (CHAR_DEL - 'A'))
			{ // +2c
				// This is the most expensive (cycle-wise) path.
				g_char_process = (char_process *) pgm_read_word( &f_esc_lookup[ c ] ) ; // 16c
				CYCLES( CYCLES_CHAR_POLL - ((11) + (2 + 1 + 1 + 3 + 6 + 2 + 2 + 16))) ; // 44c total. (1c spare)
			}
			else
			{ // +3c
				g_char_process = esc_default ;
				CYCLES( CYCLES_CHAR_POLL - ((11) + (2 + 1 + 1 + 3 + 6 + 2 + 3)) ) ;
			}
		}
	}
	else
	{ // 3c to here.
		CYCLES( CYCLES_CHAR_POLL - ((11)+(3 + 6)) ) ;
		g_char_process = nul_process ; // 6c
        // no 2c jump to end.
	}
}

// Param State
//	Control characters obeyed
//	Other characters simply passed to the generic parameter character handler
void poll_param( void )
{
	if (USART0_REG_A & (1<<USART0_BIT_RXC))
	{ // +2c
        uint8_t c = USART0_DATA ; // 1c
		g_char = c ; // 1c
		
		// Note: NULs are (now) handled by the control-code function look-up.
		// Note: No control routines change the state
		if (c < CHAR_SPACE)
		{ // +2c
			// this is a C0 character... execute it as normal.
			CYCLES( CYCLES_CHAR_POLL - ((11) + (2 + 1 + 1 + 2 + 13 + 2 + 4))) ;
			g_char_process = (char_process *) pgm_read_word( &f_control_lookup[ g_char ] ) ; // 13c
			// 2c jump then 4c store.
		}
		else
		{ // +3c
			// no matter what the character that's received, I'll just pass it to 
			// the 'character processing handler'
			CYCLES( CYCLES_CHAR_POLL - (11 + 2 + 1 + 1 + 3 + 2 + 2 + 4) ) ;
			g_char_process = generic_param ; // 2c 
			// 2c jump then 4c store
		}
	}
	else
	{ // 3c to here.
		CYCLES( CYCLES_CHAR_POLL - ((11)+(3 + 6)) ) ;
		g_char_process = nul_process ; // 6c
        // no 2c jump to end.
	}
}

// DLE State.
// Control Characters NOT obeyed.
// Any non-null character will be output to the screen *as a character*
// NULL is ignored.
void poll_dle( void )
{
	if (USART0_REG_A & (1<<USART0_BIT_RXC))
	{ // 2c
		CYCLES( CYCLES_CHAR_POLL - (11 + 2 + 1 + 1) ) ;
		uint8_t c = USART0_DATA ; // 1c
		g_char = c ; // 1c
		// don't change g_char_process
	}
	else
	{ // 3c
		// clearing the g_char value is important if no character is received.
		g_char = CHAR_NUL ; // 1c
		// don't change g_char_process
		CYCLES( CYCLES_CHAR_POLL - (11 + 3 + 1) ) ;
	}
}

// Split state
// NO CHARACTERS ARE RECEIVED
// This state is entered for just one scanline. It allows a second char_process function to be called
// on a second scanline for commands where there is just too much to do.
// An example is clear_start_of_screen, which clears all the rows above the cursor on scanline A and
// clears to the start of the line on scanline B.
// This state always switches back to poll_ground
//
// Any work is actually done by whatever function g_char_process is previously left pointing at.
void poll_split( void )
{
	g_char_poll = poll_ground ; // simply return to ground state. 6c
	CYCLES( CYCLES_CHAR_POLL - (11 + 6) ) ;
}

// ===================================================================
// Character Handler routines
// Each of these must run in under about 130 cycles, otherwise glitches
// will be seen on the video output.
// ===================================================================
void nul_process( void )
{
	// no character received. simply normalise the current line.
	// 'normalise' means "convert a 'don't display' line into all-blanks and mark it as 'do display'"
	_normalise_current_row();
}
	

void output_char( void )
{	// a normal character was received. Simply output it.
	_output_char();
}

static char g_param_command = 0; // the character of the original command
static uint8_t g_param_count = 0 ; // the current parameter number (0 for the first)
void generic_param( void )
{
	// a parameter has been received for an escape sequence.
	// e.g. <esc><command><param>[...<param>]
	// it is up to the individual parameter handling routines what they do with the parameter.
	
	// a simple switch statement will do here, since accurate timing is now handled
	// by a second interrupt and none of the routines are tight on time,
	switch (g_param_command)
	{ // call the appropriate parameter processing routine.
		case CMD_CURSOR_DIRECT_ADDRESS:
			param_cursor_direct_address();
			break;
		case CMD_SET_ROW_ATTRIBUTE:
			param_set_row_attribute();
			break;
		case CMD_SET_ENABLE:
			param_set_enable();
			break;
		case CMD_SET_DISABLE:
			param_set_disable();
			break;
		case CMD_SEND_DATA:
			param_send_data();
			break;
		case CMD_TRANSMIT_ENABLE:
			param_transmit_enable();
			break;
		case CMD_READ_CHAR_RC:
			param_send_char_rc();
			break;
#ifdef PROGRAMMABLE_FONTS
		case CMD_PROGRAM_FONTBANK_PAGE:
			param_program_fontbank_page();
			break;
#endif
	}
}

#ifdef PROGRAMMABLE_FONTS
void param_program_fontbank_page( void ) 
{
// processes a parameter for the <ESC>Rfp sequence
	if (g_param_count ==0)
	{ // this is the first parameter (which fontbank to program [0-10])
		g_param_count = g_char ; // store f in the g_param_count. This will work, as it's never 0 (NULLs are ignored).
	}
	else
	{ // this is the second parameter (which chunk within the fontbank to program [0-17])
		// the fontbank parameter is stored in g_param_count
		// the fontchunk parameter is stored in g_char
		
		uint8_t fontbank = g_param_count - CHAR_SPACE ;
		uint8_t fontchunk = g_char - CHAR_SPACE ;
		
		// check that these parameters are within range...
		if (fontbank >= FONTBANK_COUNT) return ; // this fontbank isn't defined.
		if (fontchunk >= 6) return ; // each fontbank consists of 6 chunks... 0 - 5
		
		program_fontbank_chunk( fontbank, fontchunk );
	}
}

uint8_t decodehex(uint8_t) __attribute__((section(".boot")));
uint8_t decodehex( uint8_t c )
{
	if ((c >='0') && (c <= '9')) return (c - '0') ;
	if ((c >= 'a') && (c <= 'f')) return ((c - 'a') + 10) ;
	// force a reset
	wdt_enable( WDTO_15MS );
	while(1); // loop forever.
}

// NOTE: This function cannot be inline - it has to exist in the .boot section.
void program_fontbank_chunk( uint8_t fontbank, uint8_t fontchunk )
{
	// NOTE:
	// Any code that uses the SPM function (e.g. write/erase flash) MUST exist in the
	// bootloader section of the flash memory. If not, the SPM instruction is ignored.
	// A special code section called .boot has been created within this bootloader section.
	// Any function that uses SPM should be given this section as an attribute.
	// 				__attribute__((section (".boot")))
	
	// This code programs a single fontchunk.
	// Each fontchunk is 384 bytes. (There are 6 fontchunks per fontbank)
	// The 384 bytes to program are held in the screen memory.
	// 16 bytes per row, encoded as lower-case hex values in columns 0-31
	// 24 rows (0-23)
	// 24 * 16 = 384 bytes
	
	
	cli(); // stop interrupts running.

	uint8_t *p_display = &g_display[0][0] ;
	uint8_t *p_flashpage = &f_font[ FONT_CHAR_HEIGHT * FONT_CHAR_COUNT * fontbank ] ;
	p_flashpage += (fontchunk * 384); // each fontchunk is 384 bytes long.
		
	
	// before starting, make sure any other writing/erasing etc. is finished.
	boot_spm_busy_wait();
	
	for( uint8_t row = 0 ; row < 24 ; )
	{
		for ( uint8_t flashpageindex = 0 ; flashpageindex < SPM_PAGESIZE ; )
		{
			for ( uint8_t col = 0 ; col < 32 ; col+= 4 ) // each word is 4 characters
			{	// loop around each of the 8 words for this line...
				// find the word to store...
				uint8_t c0 ;
				uint8_t c1 ;
				uint8_t c2 ;
				uint8_t c3 ;
				c0 = decodehex(*p_display++);
				c1 = decodehex(*p_display++);
				c2 = decodehex(*p_display++);
				c3 = decodehex(*p_display++);
				
				uint16_t word = (((c2<<4)|c3)<<8)|((c0<<4)|c1) ;
				
				boot_page_fill (p_flashpage + flashpageindex , word);
				flashpageindex += 2 ; // 2 bytes per word
			}
			p_display += 6 ; // move to the start of the next row...
			row++;
		}
		// that's a page of flash filled...
		boot_page_erase (p_flashpage);
		boot_spm_busy_wait ();				// Wait until the memory is erased.
		boot_page_write (p_flashpage);		// Store buffer in flash page.
		boot_spm_busy_wait();       		// Wait until the memory is written.
		p_flashpage += SPM_PAGESIZE ;// move to the next flash page
	}
	
	// force a reset
	wdt_enable( WDTO_15MS );
	while(1); // loop forever.
}
#endif

void param_cursor_direct_address( void )
{
// processes a parameter for the <ESC>Yrc sequence
	if (g_param_count == 0)
	{ // this is the first parameter, [row]
		g_param_count++; // there should be a second parameter.
		
		// turn the parameter into the correct row number
		uint8_t newrow = g_char - CHAR_SPACE ;
		
		// if it's off the bottom of the screen, don't move the current_column.
		// (see page 21 of the VT52 user manual)
		if (newrow >= ROW_COUNT)
		{
			return ;
		}
		
		// now account for the current scroll position...
		newrow += g_row_scroll_offset ;
		if (newrow >= ROW_COUNT)
		{
			newrow -= ROW_COUNT ;
		}
		
		// finally set the current_row.
		g_current_row = newrow ;
	}
	else
	{ // this is the second parameter, [column]
		g_char_poll = poll_ground ;// there are no more parameters.
		
		//turn the parameter into the correct column number.
		uint8_t newcol = g_char - CHAR_SPACE ;
		if (newcol >= COL_COUNT)
		{
			newcol = COL_COUNT - 1 ;
		}
		
		// finally set the current column.
		g_current_col = newcol ;
	}
}

void param_send_data( void )
{
uint8_t r ;

	g_char_poll = poll_ground ; // there is only ever one parameter to Send Data
	if (g_param_count == 0)
	switch( g_char)
	{
		case '0': // send cursor row
		
			// adjust to allow for the vertical scroll position.
			r = g_current_row ;
			r += ROW_COUNT ;
			r -= g_row_scroll_offset ;
			if ( r >= ROW_COUNT ) r -= ROW_COUNT ;
			USART0_DATA = r ;
			break ;
		case '1': // send cursor column
			USART0_DATA = g_current_col ;
			break ;
		case '2': // send firmware major version
			USART0_DATA = VERSION_MAJOR ;
			break ;
		case '3': // send firmware minor version
			USART0_DATA = VERSION_MINOR ;
			break ;
		case '4': // send firmware revision version
			USART0_DATA = VERSION_REVISION ;
			break ;
		case '5': // send signal format (0 = NTSC, 1 = PAL)
			USART0_DATA = (g_options & (1<<OPT_PAL))?1:0 ;
			break ;
		case '6' : // send row, column (two bytes)
			// Normally you'd need to check to see if the TX buffer is clear
			// before shovelling data to the USART.
			// We don't need to here, because we know that no data has been
			// sent recently - at least 3 characters ago (<ESC>{n)
			//
			// As soon as the first character has been put into the data register,
			// it is copied into the USART's shift register, (freeing up the data
			// register straight away) - meaning we can immediately send the second character.
			//
			// Very useful.
			
			// adjust to allow for the vertical scroll position.
			r = g_current_row ;
			r += ROW_COUNT ;
			r -= g_row_scroll_offset ;
			if ( r >= ROW_COUNT ) r -= ROW_COUNT ;
			USART0_DATA = r ;
			USART0_DATA = g_current_col ;
			break ;
	}
}

void param_send_char_rc( void )
{ // <ESC>}rc  where r = 32+row , c = 32+col
	if (g_param_count == 0)
	{ // this is the first parameter.
		// This first parameter will be stored in g_param_count (we're mighty short on memory)
		// It works because the first parameter can never be 0.
		// (hence the receipt of the second character causes the 'else' statement [below] to be entered)
		g_param_count = g_char ;
	}
	else
	{ // this is the second parameter.
		g_char_poll = poll_ground ;// there are no more parameters - return to the ground state from here.
		
		// sort out the row parameter (carried in g_param_count)...
		uint8_t row = g_param_count - CHAR_SPACE ;

		// now account for the current scroll position...
		row += g_row_scroll_offset ;
		if (row >= ROW_COUNT)
		{
			row -= ROW_COUNT ;
		}
	
		// sort out the col parameter (carried in g_char)...
		uint8_t col = g_char - CHAR_SPACE ;
		
		// now send back a result (depending if the row,column are in-bounds).
		if ((row < ROW_COUNT) && (col < COL_COUNT))
		{
			USART0_DATA = g_display[ row ][ col ] ;
		}
		else
		{
			USART0_DATA = 0 ; // return a <NUL> for any out-of-bounds request.
		}
	}
}

void param_transmit_enable( void )
{
// process a parameter for the <ESC>~~~~ sequence
// The first ~ is not a 'parameter', so there are 3 parameters expected...

	if (g_char == '~')
	{ // this is the expected parameter.
		g_param_count++ ;
		if (g_param_count >= 3)
		{
			// all 3 ~'s have been received, so enable the transmit functionality
			USART0_REG_B |= (1<<USART0_BIT_TXEN) ; // enable transmit
			
			// and drop back to the ground state...
			g_char_poll = poll_ground ;
		}
	}
	else
	{ // this is not an expected parameter. Disable transmit functionality.
		USART0_REG_B &= ~(1<<USART0_BIT_TXEN) ; // disable transmit
		
		// and drop back to the ground state...
		g_char_poll = poll_ground ;
	}
}

static uint8_t PROGMEM f_options_bit_lookup[] = // maps ESC [x|y] parameters to option byte bits.
{
	0, // line_25 setting     (not supported)   1
	0, // keyclick            (not supported)   2
	0, // hold_screen setting (not supported)   3
	(1<<OPT_BLOCKCURSOR), //                    4
	(1<<OPT_HIDECURSOR),//                      5
	0, // shift keypad (not supported)          6
	0, // alt keypad (not supported)            7
	(1<<OPT_AUTO_LF),//                         8
	(1<<OPT_AUTO_CR),//                         9
	(1<<OPT_HIDEDISPLAY), //                    :
	(1<<OPT_SUPPRESSOUTPUT),//					 ;
	(1<<OPT_ALTCURSORADVANCE) // 				<
};

void param_set_enable( void )
{
//Enable Setting. ESC xn
//This turns on one of the settings below, depending on the character n
//char	setting					meaning
//1		line_25					[not supported]
//2		key_click				[not supported (never clicks)]
//3		hold_screen				[not supported (never holds)]
//4		block_cursor			use a block cursor (as opposed to an underline cursor)
//5		hide_cursor				hides the cursor
//6		shift_keypad			[not supported (no keyboard)]
//7		alt_keypad				[not supported (no keyboard)]
//8		auto_append_LF_to_CR	append LF to any CR received
//9		auto_append_CR_to_LF	append CR to any LF received
//:		hide_display			turn the screen black
//;		suppress_output			disable output completely (no sync etc.)
//<		alt. cursor advance		cursor is moved down rather than across (but no auto newlines, scroll etc.)

	g_char_poll = poll_ground ;// there is only ever one parameter to set_enable, hence return to base mode.

	uint8_t setting = g_char - '1'; // convert the character to an index into the mapping array
	if (setting <= 10)
	{
		uint8_t mask = pgm_read_byte( &(f_options_bit_lookup[ setting ]) ) ;
		g_options |= mask ;
	}
	else if (setting <= 11)
	{
		uint8_t mask = pgm_read_byte( &(f_options_bit_lookup[ setting ]) ) ;
		g_framecount |= mask ;
	}
}

void param_set_disable( void )
{
// cancels the a setting (as listed in param_set_enable).
	g_char_poll = poll_ground ;// there is only ever one parameter to set_enable, hence return to base mode.
	
	uint8_t setting = g_char - '1'; // convert the character to an index into the mapping array
	if (setting <= 10)
	{
		uint8_t mask = pgm_read_byte( &(f_options_bit_lookup[ setting ]) ) ;
		g_options &= ~mask ;
	}
	else
	if (setting <= 11)
	{
		uint8_t mask = pgm_read_byte( &(f_options_bit_lookup[ setting ]) ) ;
		g_framecount &= ~mask ;
	}
}

// handler for row attributes parameter...
void param_set_row_attribute( void )
{
/*
Set Row attributes. ESC _n
This sets the attributes for the row that contains the cursor, depending on the character n
char	settings
0		single width, single height
1		double width, single height
2		single width, double height (top half)
3		single width, double height (bottom half)
4		double width, double height (top half)
5		double width, double height (bottom half)
6		use fontbank 0 (default)
7		use fontbank 1
8		use fontbank 2
9		use fontbank 3
:		use fontbank 4
;		use fontbank 5
<		use fontbank 6
=		use fontbank 7
>		use fontbank 8
?		use fontbank 9
@		use fontbank 10
A		use fontbank 11
B		use fontbank 12
C		use fontbank 13
D		use fontbank 14
E		use fontbank 15
*/
// lookup table to translate from escape-code parameter to row-flags.
static uint8_t PROGMEM sf_rowinfolookup[] ={
	0b00000000,  // single-width, single-height
	0b00001000,  // double-width, single-height
	0b00000010,  // single-width, double-height (top)
	0b00000110,  // single-width, double-height (bottom)
	0b00001010,  // double-width, double-height (top)
	0b00001110   // double-width, double-height (bottom)
#if FONTBANK_COUNT > 1
	,0b00000000
	,0b00010000
#endif
#if FONTBANK_COUNT > 2
	,0b00100000
#endif
#if FONTBANK_COUNT > 3
	,0b00110000
#endif	
#if FONTBANK_COUNT > 4
	,0b01000000
#endif	
#if FONTBANK_COUNT > 5
	,0b01010000
#endif	
#if FONTBANK_COUNT > 6
	,0b01100000
#endif	
#if FONTBANK_COUNT > 7
	,0b01110000
#endif	
#if FONTBANK_COUNT > 8
	,0b10000000
#endif	
#if FONTBANK_COUNT > 9
	,0b10010000
#endif	
#if FONTBANK_COUNT > 10
	,0b10100000
#endif	
#if FONTBANK_COUNT > 11
	,0b10110000
#endif	
#if FONTBANK_COUNT > 12
	,0b11000000
#endif	
#if FONTBANK_COUNT > 13
	,0b11010000
#endif	
#if FONTBANK_COUNT > 14
	,0b11100000
#endif	
#if FONTBANK_COUNT > 15
	,0b11110000
#endif	

};
#define ROWINFO_DOUBLEMASK ((1<<ROWINFO_DOUBLE_HEIGHT)|(1<<ROWINFO_DOUBLE_HEIGHT_BOTTOM)|(1<<ROWINFO_DOUBLE_WIDTH))
#define ROWINFO_FONTBANKMASK 0b11110000

	g_char_poll = poll_ground ; // only one parameter, so return to base state.
	// let's see if this is a valid setting for a row attribute...
	uint8_t setting = g_char - '0'; // deliberately unsigned calculation. results wrap-round to the top.
	if (setting <= 5)
	{ // only '0' to '5' are currently understood.
		uint8_t value = pgm_read_byte( &sf_rowinfolookup[setting] ) ;
		uint8_t rowinfo = g_rowinfo[ g_current_row ] ;
		rowinfo &= ~(ROWINFO_DOUBLEMASK);
		rowinfo |= value ;
		g_rowinfo[ g_current_row ] = rowinfo;
	}
#if FONTBANK_COUNT > 1
	else
	if (setting <= (FONTBANK_COUNT + 5))
	{
		uint8_t value = pgm_read_byte( &sf_rowinfolookup[setting] ) ;
		uint8_t rowinfo = g_rowinfo[ g_current_row ] ;
		rowinfo &= ~(ROWINFO_FONTBANKMASK);
		rowinfo |= value ;
		g_rowinfo[ g_current_row ] = rowinfo;
	}
#endif
}


void control_BEL( void )
{
// do nothing. At some point, a peizo speaker could be added to annoy people.
// (toggling an output pin every 16 scanlines would give a nice 488hz tone).
}


void control_TAB( void )
{
	uint8_t newcol = (g_current_col & 0b11111100) + 4 ;
	if (newcol >= COL_COUNT) newcol = (COL_COUNT - 1);
	g_current_col = newcol;
}


void control_BS( void )
{
// backspace - simply move the cursor back one.
	if (g_current_col > 0)
	{
		g_current_col -= 1 ;
	}
}


void control_LF( void )
{
	// move the cursor down
	_cursor_down( 1 );

	if (g_options & (1<<OPT_AUTO_CR))
	{
		g_current_col = 0;
	}
}


#define ON_TOP_ROW() (g_current_row == g_row_scroll_offset)
void control_FF( void )
{
// clear every line to blanks
// if scrolling, position the cursor on the bottom row.
// if not scrolling, position the cursor on the top row.
	_clear_screen();
	
	// the top row is g_row_scroll_offset, hence the bottom row is 24 more than that.
	g_current_row = g_row_scroll_offset + (ROW_COUNT - 1) ;
	if ((g_current_row) >= (ROW_COUNT))
	{
		g_current_row -= ROW_COUNT ;
	}
	g_current_col = 0;
}

void clear_screen( void )
{
	_clear_screen();
	g_row_scroll_offset = 0;
	g_current_col = 0;
	g_current_row = 0;
}

void control_CR( void )
{
	g_current_col = 0;

	if (g_options & (1<<OPT_AUTO_LF))
	{
		_cursor_down( 1 );
	}
}

void control_ESC( void )
{
// simply switch the handler to the ESC handler routine.	
    g_char_poll = poll_esc;
}

void control_CAN( void )
{
// switch into ground state, regardless of where we are.
// Any partially complete escape sequences will be abandoned.
	g_char_poll = poll_ground;
}

void control_default( void )
{
	// Do nothing, because it's a control code that's not supported.
}

void control_DLE( void )
{
	// find which character processing routine to use
	// (there is one per state - the routines are different in that they return to different states once the second character is received)
	if (g_char_poll == poll_ground)		g_char_process = dle_ground ;
	else if (g_char_poll == poll_esc)	g_char_process = dle_esc ;
	else if (g_char_poll == poll_param)	g_char_process = dle_param ;
	else return ; // unknown state!
	// switch into DLE state
	g_char_poll = poll_dle ;
}

void dle_ground( void )
{
	if (g_char)
	{	// A non-null character has been received.
		_output_char() ;
		g_char_poll = poll_ground ; // return to ground state
	}
}

void dle_esc( void )
{
	if (g_char)
	{	// A non-null character has been received
		_output_char() ;
		g_char_poll = poll_esc ; // return to escape state
	}
}

void dle_param( void )
{
	if (g_char)
	{	// a non-null character has been received
		_output_char() ;
		g_char_poll = poll_param ; // return to param state
	}
}


void cursor_up( void )
{
	_cursor_up( 0 ) ;
}

void cursor_up_scroll( void )
{
	_cursor_up( 1 ) ;
}

void cursor_down( void )
{
	_cursor_down( 0 ) ;

}

void cursor_right( void )
{
	if (g_current_col < (COL_COUNT-1))
	{
		g_current_col ++ ;
	}
}

void cursor_left( void )
{
	if (g_current_col > 0)
	{
		g_current_col-- ;
	}
}

void esc_command_with_param( void )
{
 	g_char_poll = poll_param ;
	g_param_command = g_char ;
	g_param_count = 0;
	
}

static void output_string_P( char *str )
{
	g_char = pgm_read_byte( str++ ) ;
	while (g_char)
	{
		_output_char() ;
		g_char = pgm_read_byte( str++ ) ;
	}
}

static void output_byte_as_hex( uint8_t i )
{
uint8_t ii = (i & 0b11110000) >> 4 ;
	g_char = (ii > 9)?(('a' - 10) + ii):('0' + ii) ;
	_output_char();
	ii = (i & 0b1111) ;
	g_char = (ii > 9)?(('a' - 10) + ii):('0' + ii) ;
	_output_char();
}

char f_str_ON[] PROGMEM = "ON";
char f_str_OFF[] PROGMEM = "OFF";

static void itoa_to_screen(uint32_t n)
{
  uint8_t i;

  i = 0;
  do
  {
	g_char = n % 10 + '0' ;
	_output_char();
	g_current_col -= 2 ;
	i++ ;
  }
  while ((n /= 10) >= 1);
} 

void show_state( void )
{
// output the current state of the device to the screen.
// Note: This is never at the 'current cursor'. It's always at the top-left of the screen.
//
// example output:
//
// Version:   1.0.6
//
// Serial:
//   Jumpers       Autobaud
//   Actual Baud   55555
//   UBBR          0x0010
//
// Options:
//   LINEOVERFLOW  OFF
//   HIDECURSOR    ON
//   BLOCKCURSOR   OFF
//   AUTO_LF       OFF
//   AUTO_CR       OFF
//   PAL           ON
//

// If nul_process is called directly, the compiler attempts
// to inline the contents of nul_process every time. There's
// not space to do that, so g_char_process is used.


	void inline output_byte_as_ONOFF( uint8_t i )
	{
		// assumes that g_char_process is pointign to nul_process
		g_current_col = 16 ;
		if (i)
		{
			output_string_P( f_str_ON );
		}
		else
		{
			output_string_P( f_str_OFF );
		}
		g_current_row++ ;
		g_current_col = 2;
		g_char_process();
	}

	void inline nextline( void )
	{
		// assumes that g_char_process is pointign to nul_process
		g_current_row ++ ;
		g_char_process();
		g_current_col = 2 ;
	}


	g_char_process = nul_process ;

	clear_screen();
	g_char_process();

	output_string_P (PSTR("Version:  ")) ;
	output_string_P (PSTR(VERSION)) ;
	nextline();
	nextline();
	
	g_current_col = 0 ;

	output_string_P( PSTR("Serial: ") );
	nextline();
	output_string_P( PSTR("Jumpers" )) ;
	g_current_col = 16 ;

	static char PROGMEM baud_rates[] = "57600  \00038400  \00019200  \0009600   \0004800   \0001200   \000300    \000Autobaud" ;
	char *baudrate = baud_rates + ((PINC & 0b111) << 3) ;
	output_string_P( baudrate ) ;
	

	nextline();
	
	output_string_P( PSTR("Actual Baud") ) ;
	

	// converting UBBR to baud rate:
	// baud = 1000000 / (UBBR + 1)
	uint32_t baud = 1000000 / (((USART0_BAUD_H <<8) + (USART0_BAUD_L))+ 1) ;

	// the itoa_to_screen prints from the current location, backwards.
	// e.g. the following will have the least significant digit in column 20.
	// e.g. right-justified to column 20.
	g_current_col = 20 ;
	itoa_to_screen( baud ) ;

	nextline();
	output_string_P( PSTR("UBBR") ) ;
	g_current_col = 16 ;
	output_string_P( PSTR("0x") ) ;
	output_byte_as_hex( USART0_BAUD_H ) ;
	output_byte_as_hex( USART0_BAUD_L ) ;
	
	nextline();
	nextline();
	g_current_col = 0 ;

	output_string_P( PSTR("Options:") ) ;
	nextline();
	
#define showoption( str , onoff )\
do\
{\
		output_string_P( str ) ;\
		output_byte_as_ONOFF( onoff );\
} while (0)


	showoption( PSTR("LINE_OVERFLOW"), g_options & (1<<OPT_LINEOVERFLOW) ) ;
	showoption( PSTR("HIDE_CURSOR"), g_options & (1<<OPT_HIDECURSOR) ) ;
	showoption( PSTR("BLOCK_CURSOR"), g_options & (1<<OPT_BLOCKCURSOR) ) ;
	showoption( PSTR("AUTO_LF"), g_options & (1<<OPT_AUTO_LF) ) ;
	showoption( PSTR("AUTO_CR"), g_options & (1<<OPT_AUTO_CR) ) ;
	showoption( PSTR("PAL"), g_options & (1<<OPT_PAL) ) ;
	
	g_current_col= 0 ;
	
}

void FILL_SCREEN( void )
{
	for( uint8_t col = 0 ; col < COL_COUNT ; col++ )
	{
		g_display[0][col] = '0';
	}
	g_rowinfo[0] = (1<<ROWINFO_DISPLAY) ;
	for( uint8_t row = 1 ; row < ROW_COUNT ; row++ )
	{
		for( uint8_t col = 0 ; col < COL_COUNT ; col++ )
		{
			g_display[row][col] = 'E';
		}
		g_rowinfo[row] = (1<<ROWINFO_DISPLAY);
	}
}

void clear_start_of_screen( void )
{
 // Clear from start of screen (inclusive)
 
	g_char_process = clear_line_from_start ; //
	g_char_poll = poll_split ;
 
	if (g_current_row < g_row_scroll_offset)
	{
		// clear between 0 and the current-row [not included]
		// the largest cr can be is 23, because g_row_scroll_offset is 0..24
		_mem_clear( g_rowinfo , g_current_row ) ;
		_mem_clear( g_rowinfo + g_row_scroll_offset , ROW_COUNT - g_row_scroll_offset ) ;
	}
	else
	{
		// clear between the scroll position [included] and the current row [not included]
		_mem_clear( g_rowinfo + g_row_scroll_offset , g_current_row - g_row_scroll_offset ) ;
	}
}

void cursor_send( void )
{
	// put the character under the cursor into the transmit buffer...
	USART0_DATA = g_display[ g_current_row ][ g_current_col ] ;
}

void cursor_show( void )
{
	g_options &= ~(1<<OPT_HIDECURSOR);
}

void cursor_hide( void )
{
	g_options |= (1<<OPT_HIDECURSOR);
}

static uint8_t g_saved_row = 0;
static uint8_t g_saved_col = 0;
void cursor_save( void )
{
// save cursor position - allow for scrolling...

	uint8_t row = g_current_row + ROW_COUNT ;
	row -= g_row_scroll_offset ;
	
	if (row >= ROW_COUNT) row -= ROW_COUNT ;
	
	g_saved_row = row ;
	g_saved_col = g_current_col ;
}


void cursor_restore( void )
{
// restore cursor position - allow for scrolling...
	uint8_t row = g_saved_row + g_row_scroll_offset ;
	
	if (row >= ROW_COUNT) row -= ROW_COUNT ;
		
	g_current_row = row ;
	g_current_col = g_saved_col ;
	
}

void _cursor_up( uint8_t withscroll )
{
 // cursor up (scrolling if already at top of screen)
	if (ON_TOP_ROW())
	{// scrolling
		if (withscroll)
		{
			if(g_row_scroll_offset == 0)
			{
				g_row_scroll_offset = ROW_COUNT - 1;
				g_current_row = g_row_scroll_offset ;
			}
			else
			{
				g_row_scroll_offset -= 1 ;
				g_current_row = g_row_scroll_offset ;
				g_rowinfo[g_current_row] &= ~(1<<ROWINFO_DISPLAY);
			}
		}
	}
	else
	{// non-scrolling
		if (g_current_row == 0)
		{
			g_current_row = ROW_COUNT - 1 ;
		}
		else
		{
			g_current_row -= 1 ;
		}
	} 
}


void cursor_home( void )
{
	g_current_col = 0;
	g_current_row = g_row_scroll_offset ;
}


void clear_end_of_screen( void )
{
 // Clear from end of screen (inclusive)
 
	g_char_process = clear_end_of_line ; //
	g_char_poll = poll_split ;
	
	if (g_current_row < g_row_scroll_offset)
	{
		_mem_clear( g_rowinfo + (g_current_row + 1) , g_row_scroll_offset - (g_current_row + 1)) ;
	}
	else
	{
		_mem_clear( g_rowinfo , g_row_scroll_offset ) ;
		_mem_clear( g_rowinfo + (g_current_row + 1) , ROW_COUNT - (g_current_row + 1)) ;
	}
}

void clear_end_of_line( void )
{
// clear the line from the current column (inclusive) onwards
	uint8_t *p = &(g_display[ g_current_row ][ g_current_col ]) ;
	_mem_clear( p , COL_COUNT - g_current_col ) ;
}


void clear_line( void )
{
// clear current line and move to start
	g_rowinfo[ g_current_row ] &= ~(1<<ROWINFO_DISPLAY) ;
	g_current_col = 0;
}

void clear_line_from_start( void )
{
// clear the line up to the current column (inclusive)
	uint8_t *p_char = &(g_display[ g_current_row ][0]) ;
	_mem_clear( p_char , g_current_col + 1 ) ;
}


void set_overflow_on( void )
{
// line overflow on
	g_options |= (1<<OPT_LINEOVERFLOW);
}


void set_overflow_off( void )
{
// line overflow off
	g_options &= ~(1<<OPT_LINEOVERFLOW);
}

void reset( void )
{
	// reset to power-on equivalent
	
	// enable the watchdog timer, with a timeout of 15ms
	cli();
	wdt_enable( WDTO_15MS );
	// now simply loop forever!
	while(1)
	{
		// do nothing.
		// The watchdog will timeout and reset the processor.
		// (the watchdog timer is disabled on entry to main())
	}
	
}

void esc_default( void )
{
	// Do nothing - the escape command wasn't understood.
	// The state has already been changed back to ground.
}

static void inline _output_char( void )
{
	g_display[ g_current_row ][ g_current_col ] = g_char ;
	
	//advance the cursor
	if (g_framecount & (1<<OPT_ALTCURSORADVANCE))
	{
		// this is a special case - the alternative cursor advance is set.
		// This advances the cursor *down* the screen. No scrolling
		// or fancy behaviour is supported. If it reaches the bottom of
		// the screen, it stays there, not scrolling.
		_cursor_down( 0 ) ;
	}
	else
	{
		if (g_current_col < ( COL_COUNT - 1 ))
		{	// simple case - we've got room to advance the cursor.
			g_current_col += 1 ;
		}
		else
		{	// we've run out of room to advance the cursor...
			if (g_options & (1<<OPT_LINEOVERFLOW))
			{
				// we need to do a LF CR
				// move the cursor to the start of the row, and call _newline
				g_current_col = 0;
				// move the cursor down
				_cursor_down( 1 ); // scrolling
				//
				// NB, 20th Feb 2009 - The next line is *not* cleared.
				// originally, the next line was cleared when line overflow was enabled.
				// I've changed the behaviour as the H19 and VT52 manuals don't explicitly
				// state that they get cleared, and hyperterminal doesn't clear the next line
				// either.
				// (obviously the row will be cleared if it has been scrolled)
				///g_rowinfo[ g_current_row ] &= ~(1<<ROWINFO_DISPLAY);
			}
		}
	}
}

// used by FF and clear_screen - these should probably be common routines.
static void inline _clear_screen( void )
{
// simply clears the screen. The cursor is NOT moved.
// All row attributes are cleared.
	_mem_clear( g_rowinfo , ROW_COUNT ) ;
}

static void inline _normalise_current_row()
{
	uint8_t *p_rowinfo = &(g_rowinfo[ g_current_row ]) ;
	uint8_t *p_char = &(g_display[ g_current_row ][ 0 ]);

	// check to see if it needs clearing...
	if (!(*p_rowinfo & (1<<ROWINFO_DISPLAY)))
	{
		// mark it as displayable
		*p_rowinfo = (1<<ROWINFO_DISPLAY);
		
		// turn the whole row into spaces/NULLs.
		// it's *way* too slow as a loop, so it's been unrolled.
		
		asm(
			"	st		%a0+,__zero_reg__ ;38\n\t"
			"	st		%a0+,__zero_reg__ ;37\n\t"
			"	st		%a0+,__zero_reg__ ;36\n\t"
			"	st		%a0+,__zero_reg__ ;35\n\t"
			"	st		%a0+,__zero_reg__ ;34\n\t"
			"	st		%a0+,__zero_reg__ ;33\n\t"
			"	st		%a0+,__zero_reg__ ;32\n\t"
			"	st		%a0+,__zero_reg__ ;31\n\t"
			"	st		%a0+,__zero_reg__ ;30\n\t"
			"	st		%a0+,__zero_reg__ ;29\n\t"
			"	st		%a0+,__zero_reg__ ;28\n\t"
			"	st		%a0+,__zero_reg__ ;27\n\t"
			"	st		%a0+,__zero_reg__ ;26\n\t"
			"	st		%a0+,__zero_reg__ ;25\n\t"
			"	st		%a0+,__zero_reg__ ;24\n\t"
			"	st		%a0+,__zero_reg__ ;23\n\t"
			"	st		%a0+,__zero_reg__ ;22\n\t"
			"	st		%a0+,__zero_reg__ ;21\n\t"
			"	st		%a0+,__zero_reg__ ;20\n\t"
			"	st		%a0+,__zero_reg__ ;19\n\t"
			"	st		%a0+,__zero_reg__ ;18\n\t"
			"	st		%a0+,__zero_reg__ ;17\n\t"
			"	st		%a0+,__zero_reg__ ;16\n\t"
			"	st		%a0+,__zero_reg__ ;15\n\t"
			"	st		%a0+,__zero_reg__ ;14\n\t"
			"	st		%a0+,__zero_reg__ ;13\n\t"
			"	st		%a0+,__zero_reg__ ;12\n\t"
			"	st		%a0+,__zero_reg__ ;11\n\t"
			"	st		%a0+,__zero_reg__ ;10\n\t"
			"	st		%a0+,__zero_reg__ ;9\n\t"
			"	st		%a0+,__zero_reg__ ;8\n\t"
			"	st		%a0+,__zero_reg__ ;7\n\t"
			"	st		%a0+,__zero_reg__ ;6\n\t"
			"	st		%a0+,__zero_reg__ ;5\n\t"
			"	st		%a0+,__zero_reg__ ;4\n\t"
			"	st		%a0+,__zero_reg__ ;3\n\t"
			"	st		%a0+,__zero_reg__ ;2\n\t"
			"	st		%a0+,__zero_reg__ ;1\n\t"
			:: "x" (p_char) ) ; 
		
//		_mem_clear( p_char , 38 ) ;
//		_mem_clear_38( p_char ) ;
	}
}

static void inline _cursor_down( uint8_t withscroll )
{
// move the cursor down a line.
// scroll the screen if neccesary

	if ( withscroll )
	{
		// the cursor should move down a row.
		g_current_row += 1 ;
		
		// and now check to see if it's gone off the end of the array...
		if (g_current_row >= ROW_COUNT)
		{	// yup - it's gone off the end of the array - start again from the start of thearray.
			g_current_row = 0;
		}

		// does it need scrolling?
		if (g_current_row == g_row_scroll_offset)
		{
			// the screen should be scrolled...
			g_row_scroll_offset += 1 ;
			if (g_row_scroll_offset >= ROW_COUNT)
				g_row_scroll_offset = 0;
				
			// and clear the new line.
			g_rowinfo[ g_current_row ] &= ~(1<<ROWINFO_DISPLAY);
		}
	
	}
	else
	{
		uint8_t bottom_row ;
		if (g_row_scroll_offset == 0)
			bottom_row = ROW_COUNT - 1 ;
		else
			bottom_row = g_row_scroll_offset - 1 ;

		// if g_current_row == bottom_row, then there is no movement.
		if ( g_current_row != bottom_row )
		{
			if (g_current_row == (ROW_COUNT-1))
				g_current_row = 0;
			else
				g_current_row += 1;
		}
	}
	
}
