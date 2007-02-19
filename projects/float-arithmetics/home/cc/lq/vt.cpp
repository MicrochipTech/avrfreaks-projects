#define DB_ENABLED 0
#define DB_INFO 0

// This module contains :
// vtocha( char z ); Display a single character
// uint vtkey();	Get a key stroke
// bool vtiskey();	Check if a key pressed
// vtcls();			Clear screen
// vtattr();		Set Display Attribute

// vt.cpp
// Dialogue on a simple terminal emulation with 80 x 24 lines

#include <heport.h>

#if defined( AVR )
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/signal.h>
#include <tshar.h>
#endif

#if defined( MDOS40 ) || defined( MDOS10 )
#  include <conio.h>
#  include <windows.h>
#  if defined( MDOS40 )
#	 include <Mapiwin.h>	// Sleep()
#  endif
#endif

#include <hegettim.h>	 // getmsec()
#include <helq.h>
#include <hevt.h>
#include <hedeb.h>

#ifdef MSVC40
// Visual C++ Version 4 or 5
#include <hegettim.h>
#endif

// This module contains :
//	   vtopen()
//	   vtclose()
//	   vtrefresh()
//	   vtkey()
//	   vtiskey()
//
// Common to all screen emulators which use internal mirror
//	vtattr()
//	vtcls()
//	vtocha()
//	vtputs()
//	vtnl()
//
uint16_t vt_xmax = NCOLS ;	   // maybe overwritten at vtopen
uint16_t vt_ymax = NLINES ;	//
bool vt_uses_screen = false ;	 // set by vtcls call
		// false : stdout mode
		// true : screen mode

#if VT_MEMORY
struct sscreenemu screen[NLINES][NCOLS] ;
struct sscreenemu act_screen[NLINES][NCOLS] ;	// only maintained by helper functions
uint vt_curr_line = 0 ;
uint vt_curr_col = 0 ;
uint act_vt_curr_line = 0 ;
uint act_vt_curr_col = 0 ;
uint8_t vt_current_attribute = ( 0 << 4 ) | 7 ;	// fg:Black, bg:white



#define VTCOLNORM 0x07
#define MAXATTR 4
static uint8_t attriblist[4] = { 0x07, 0x06, 0x70, 0x74 } ;
// The index to attriblist[] is VTANORM 0, VTASTRONG 1, VTAINV 2, VTABLINK 3


uint vt_scrol_ymin = 0 ;
uint vt_scrol_ymax = NLINES - 1	 ;

// helper functions for in-memory screen emulation
extern void xxvtopen( void );	// initialize windows or hardware
extern void xxvtcurpos(uint x, uint y);	  // set physical cursor
static char *sOperatingsys ;

// Colors may be VTC_BLACK ...
FUNCT void vtcolors( uint8_t fg, uint8_t bg ) // for VT_MEMORY
{
vt_current_attribute = ( ( fg << 4 ) & 0xf0 ) | ( bg & 0x0f ) ;
}


FUNCT void vtattr( uint8_t  how ) // for VT_MEMORY
{
if ( how < MAXATTR )
	{
	vt_current_attribute = attriblist[how] ;
	}
}


// clears the screen buffer - not the actual screen
FUNCT void vtcls()		// VT_MEMORY since 26.03.1999 for all memory screen emulators
{
uint x , y ;			// coordinates
struct sscreenemu *pscreen ; // DEBUG: 9326msec to 9202 msec with pointer


if ( sOperatingsys != 0 )
	{
	if ( strcmp( sOperatingsys , "Windows_NT" ) ==	0 ) return ;	// because too dumb
	}


if ( !vt_uses_screen )
	{
	xxvtopen();				// If want to use screen emulation :
		// then initialize
	}
if ( vt_uses_screen )
	{
	pscreen = screen[0] ;
	for ( y = 0 ; y < NLINES ; ++y )
	{
	for ( x = NCOLS+1 ; --x ; )
		{
//						screen[y][x].code = ' ' ;
//						screen[y][x].attr = attriblist[VTANORM] ;
		pscreen->code = ' ' ;
		pscreen->attr = vt_current_attribute ;	// VTCOLNORM ;
		++pscreen ;
		}
	}
	vt_curr_line = 0 ;
	vt_curr_col = 0 ;
	vt_current_attribute =	vt_current_attribute ;	// VTCOLNORM ;
	}
}



// ----------------------------------------------
// ----------------------------------------------
void vtcurdis()
{
vtrefresh() ;
}



// ----------------------------------------------
// ----------------------------------------------
void vtsetscrol( uint8_t ymin, uint8_t ymax ) // numbers 0 .. 23
{
if ( ymin <= ymax && ymax < NLINES )
	{
	vt_scrol_ymin = ymin ;
	vt_scrol_ymax = ymax ;
	}
}



// ----------------------------------------------
// ----------------------------------------------
FUNCT void vtocha( char cc )		// for VT_MEMORY
{
// union REGS inregs ;


DBent("vtocha");

if ( vt_uses_screen )
	{
#ifdef DB_ENABLED
	if ( vt_curr_col >= 80 || vt_curr_line >= 25 )
		{
		Eprintf(( "vt_curr_col = %d vt_curr_line = %d", vt_curr_col, vt_curr_line ));
		}
#endif
	if ( vt_curr_col >= 80 ) vt_curr_col = 0 ;
	if ( vt_curr_line >= 25 ) vt_curr_line = 0 ;

	if ( cc >= 0 && cc < 0x20 )
		{
		if ( cc == 0x08 )
			{			// Backspace
			if ( vt_curr_col == 0 )
				{
				if ( vt_curr_line != 0 )
					{
					vt_curr_col = 79 ;
					--vt_curr_line ;
					}
				}
			else
				{
				--vt_curr_col ;
				}
			DBret("vtocha");
			return ;
			}

		if ( cc == 0x0a ) goto nl ;		// Newline
		if ( cc == 0x0d ) goto nl ;		// Newline
		if ( cc == VTK_ENTER ) goto nl ;	 // Newline
		if ( cc == '\n' ) goto nl ;		// Newline

		if (	cc == VTANORM ||
			cc == VTASTRONG ||
			cc == VTAINV ||
			cc == VTABLINK )
			{
			vtattr(cc) ;
			}
		}
		else
		{
		DBprintf(( "At vt_curr_col=%d vt_curr_line=%d cc=%XH = %c",
			vt_curr_col, vt_curr_line, cc, cc ));

		screen[vt_curr_line][vt_curr_col].code = cc ;
		screen[vt_curr_line][vt_curr_col].attr = vt_current_attribute ;

		++vt_curr_col ;
		if ( vt_curr_col >= vt_xmax )
			{							// end of line reached now
nl:			vtnl();
			}
		}
	}		// end if vt_uses_screen
else
	{
#if defined( MDOS40 ) || defined( MDOS10 ) || defined( _TURBOCDOS_ ) || defined( DOSGNU )
//	  cc = charouttable[(uint8_t)cc] ;
#endif
	if ( cc == 0x0d || cc == 0x0a )
		{
		vt_curr_col = 0 ;
		}
	else if ( cc == 0x09 )
		{
		vt_curr_col += 8 ;
		vt_curr_col /= 8 ;
		vt_curr_col *= 8 ;
		}
	else
		{
		++vt_curr_col ;
		}
	putchar( cc );		// to standard out
	}
DBret("vtocha");
}



FUNCT void vtputs( const char *pt)		// VT_MEMORY
{
for ( ; *pt ; ++pt )
	{
	if ( *pt == 1 ) vtattr( VTANORM );
	else if ( *pt == 2 ) vtattr( VTAINV );
	else vtocha(*pt);
	}
}

// -----------------------------------------------------------------------
FUNCT void vtdeleol()		// VT_MEMORY
{
int istart ;

for ( istart = vt_curr_col ; istart < NCOLS ; ++istart )
	{
	screen[ vt_curr_line ][ istart ].code = ' ' ;
	screen[ vt_curr_line ][ istart ].attr = vt_current_attribute ;
	}
return ;
}

// -----------------------------------------------------------------------
FUNCT void vtdelbol()		// VT_MEMORY
{
int istart ;

for ( istart = vt_curr_col ; --istart >= 0 ; )
	{
	screen[ vt_curr_line ][ istart ].code = ' ' ;
	}
return ;
}


// -----------------------------------------------------------------------
FUNCT void vtcurdn()	   // VT_MEMORY
{
uint x , y ;

if ( vt_curr_line < NLINES-1 && vt_curr_line < vt_scrol_ymax )
	{
	++vt_curr_line ;
	}
else if ( vt_curr_line == vt_scrol_ymax )
	{		// scroll up
	if ( vt_scrol_ymax >= NLINES )
		{
		return ;	// cannot scroll
		}
	for ( y = vt_scrol_ymin ; y < vt_scrol_ymax ; ++y )
		{
		for ( x = 0 ; x < vt_xmax ; ++x )
			{
			screen[y][x] = screen[y+1][x] ;
			}
		}
	for ( x = 0 ; x < vt_xmax ; ++x )
		{
		screen[y][x].attr = vt_current_attribute ;
		screen[y][x].code = ' ' ;
		}
	}

return ;
}

// -----------------------------------------------------------------------
FUNCT void vtcurup()	   // VT_MEMORY
{
uint x, y ;

if ( vt_curr_line > 0 && vt_curr_line > vt_scrol_ymin )
	{
	--vt_curr_line ;
	}
else if ( vt_curr_line == vt_scrol_ymin )
	{		// scroll down
	if ( vt_scrol_ymax >= NLINES )
		{
		return ;	// cannot scroll
		}
	for ( y = vt_scrol_ymax ; y > vt_scrol_ymin ; --y )
		{
		for ( x = 0 ; x < vt_xmax ; ++x )
			{
			screen[y][x] = screen[y-1][x] ;
			}
		}
	for ( x = 0 ; x < vt_xmax ; ++x )
		{
		screen[y][x].attr = vt_current_attribute ;
		screen[y][x].code = ' ' ;
		}
	}
return ;
}


// -------------------------------------------
FUNCT void vtnl()						// VT_MEMORY
{
uint x ;
uint y ;

DBent("vtnl");
		 // or when Newline character
if ( vt_uses_screen )
	{
	DBprintf(( "old cursor x=%d", vt_curr_col ));
	vt_curr_col = 0 ;			   // Start of
	++vt_curr_line ;				// next line
	if ( vt_curr_line > vt_scrol_ymax )
	{						// Then scroll up one line
	vt_curr_line = vt_scrol_ymax ;	// last line
	for ( y = 0 ; y < vt_curr_line ; ++y )
		{
		for ( x = 0 ; x < vt_xmax ; ++x )
		{
		screen[y][x].code = screen[y+1][x].code ;
		screen[y][x].attr = screen[y+1][x].attr ;
		}
		}
	for ( x = 0 ; x < vt_xmax ; ++x )
		{
		screen[y][x].code = ' ' ;
		screen[y][x].attr = vt_current_attribute ;
		}
	}
	vt_curr_col = 0 ;			   // Start of line
	DBprintf(( "new cursor x=%d", vt_curr_col ));
	}
else
	{
	vt_curr_col = 0 ;
	putchar( '\n' );
	}
DBret("vtnl");
}

FUNCT void vtcurpos( uint x, uint y )	  // VT_MEMORY
{
if ( x < vt_xmax && y < vt_ymax )
	{
	vt_curr_line = y ;
	vt_curr_col = x ;
	}
else
	{
	vtnl();
	}
}


FUNCT void vtgcurpos( uint8_t *pcol, uint8_t *pline )	// VT_MEMORY
{
*pcol = vt_curr_col ;
*pline = vt_curr_line ;
}

#endif	 // VT_MEMORY






// All terminals or emulations which support ansi codes
// As OSK, MDOS40 (MS on DOS with DEVICE=ANSI.SYS), LINUX


#if defined( VTANSI )

uint8_t getrawkeycode(void);
// Dieser Teil (VTANSI) für MDOS40 / LINUX / 68000

#ifdef LINUX
#		include <termios.h>
#		include <fcntl.h>
#		include <stdio.h>

	struct termios xt_0original ;	// Terminal setup at program start
	uint8_t ansi_key_code_c1 ; // holt key
#endif

#define NSEQ 5
char vt_raw_sequence[NSEQ] ;


#ifdef OSK
bool bu0valid = false ;
struct	sgbuf bu0save ;
struct	sgbuf bu0path ;
#endif




FUNCT void vtrefresh()		// VTANSI
{
char cc ;
uint8_t at ;
uint8_t atselected ;
uint x ;
uint y ;
uint xact ;
uint yact ;
uint xmax ;

atselected = 255 ;				// unknown attribute is selected
xact = 888 ;
yact = 888 ;
if ( !vt_uses_screen )
	{
#ifndef AVR
	fflush( stdout );
#endif
	}
else
	{
	for( y = 0 ; y < NLINES ; ++y )
	{
	xmax = NCOLS ;
	if ( y == NLINES - 1 ) --xmax ;
	for ( x = 0 ; x < xmax ; ++x )
		{
		cc = screen[y][x].code ;
		if (	( ( cc & 0x7f ) >= 0 ) && ( ( cc & 0x7f ) < 0x20 ) ||
			( cc & 0x7f ) == 0x7f )
			{
			cc = 0x20 ;
			}
		at = screen[y][x].attr ;
		if ( act_screen[y][x].code != cc ||
		 act_screen[y][x].attr != at )
			{				// must refresh this position also
			if ( y != yact || x != xact )
				{
				act_vt_curr_line = NLINES ;
				printf( "\033[%d;%dH", y+1, x+1 );
				yact = y ;
				xact = x ;
				}
			act_screen[y][x].code = cc ;
			act_screen[y][x].attr = at ;	// Color Mix
			if ( at != atselected )
				{
				printf( "\033[3%dm", ( at >> 4 ) & 0x07 );	// FG
				printf( "\033[4%dm", ( at ) & 0x07 );	// BG
				atselected = at ;
				}
#if defined( MDOS40 ) || defined( MDOS10 )
			cc = charouttable[(uint8_t)cc] ;
#endif
			putchar( cc ) ;
			++xact ;
			}
		}
	}
	if ( act_vt_curr_line != vt_curr_line ||
		 act_vt_curr_col != vt_curr_col )
		{
		printf( "\033[%d;%dH", vt_curr_line+1, vt_curr_col+1 );
		act_vt_curr_line = vt_curr_line ;
		act_vt_curr_col = vt_curr_col ;
		}
	}

#ifndef AVR
fflush( stdout );
#endif
}				// end vtrefresh()


/* ---------------------------------------- */
FUNCT void vtclose()	// vtclose for VTANSI
{
if ( !vt_uses_screen )
	{
	vtrefresh();
	vtattr(VTANORM);
	vtcurpos(0,24);			// only performed if vt_opened
	vtrefresh();		// refresh screen to show last information
#if defined( MDOS40 ) && defined( _DEBUG )
// vtprintf( "finished" );
// vtkey();
#endif
	vt_uses_screen = false ;
	}
else
	{
#ifndef AVR
	fflush( stdout );
#endif
	}

#ifdef OSK
if ( bu0valid )
	{
	_ss_opt( 1,&bu0save );
	}
#endif

#ifdef LINUX
xt_0original.c_lflag |= ECHO ;
tcsetattr( 0, TCSANOW, &xt_0original );
#endif

return ;
}


/* ---------------------------------------- */
FUNCT void vtopen()		// vtopen for ANSI, defined VTANSI
{
#ifdef OSK
int nul ;
_gs_opt(1,&bu0save) ;		/* Get terminalcharacteristic */
_gs_opt(1,&bu0path) ;		/* Get terminalcharacteristic */

nul = 0 ;
bu0path.sg_backsp = nul ;
bu0path.sg_echo	  = nul ;
bu0path.sg_pause  = nul ;
bu0path.sg_eorch  = nul ;
bu0path.sg_eofch  = nul ;
bu0path.sg_rlnch  = nul ;
bu0path.sg_dulnch = nul ;
bu0path.sg_psch	  = nul ;
bu0path.sg_bsech  = nul ;
bu0path.sg_kbich  = nul ;	// Keyboard input character
bu0path.sg_kbach  = nul ;	// Keyboard abort character

_ss_opt(1,&bu0path) ;		// For direct dialogue

bu0valid = true ;			// may use b0save later
#endif			// OSK

#ifdef LINUX
int rv ;
struct termios xt ;

fcntl( 2, F_SETFL, O_NONBLOCK );
fcntl( 0, F_SETFL, O_NONBLOCK );
tcgetattr( 0, &xt );			/* Aktuelle Einstellung holen für Änderung */
tcgetattr( 0, &xt_0original );	/* Aktuelle Einstellung für Exit */
xt.c_lflag &= ~ICANON ;
xt.c_lflag &= ~ECHO ;
/* xt.c_iflag &= ~ICRNL ; */
rv = tcsetattr( 0, TCSANOW, &xt );
if ( rv < 0 )
	{
	Eprintf(( "ichaw_ocha_init: rv=%d, errno=%d", rv, errno ));
	}
#endif

#if defined( MDOS40 ) || defined( MDOS10 ) || defined( _TURBOCDOS_ ) || defined( DOSGNU )
sOperatingsys = getenv("OS");

if ( !chartablefilled ) fillchartable() ;
#endif

vt_uses_screen = false ;			 // is opened, but no vtcls for screeen addressing
return ;
}



FUNCT void xxvtopen( void )			 // set everything to be able to use vt for VTANSI
{
int x ;
int y ;


// clear internal screen buffer memory
vt_uses_screen = true ;
for ( y = 0 ; y < vt_ymax ; ++y )
	{
	for ( x = 0 ; x < vt_xmax ; ++x )
	{
	act_screen[y][x].code = screen[y][x].code = ' ' ;
	act_screen[y][x].attr = screen[y][x].attr = VTANORM ;
	}
	}
vtattr(VTANORM);
printf( "\033[2J" );
}



FUNCT bool vtiskey()	// for VTANSI
{

#ifdef OSK
if ( _gs_rdy(1) >= 1 ) return true ;
vtrefresh();			// If no key pressed, display actual code
tsleep(1);				// Assume there is nothing to do
return false ;
#endif

#ifdef LINUX
int rv ;
uint8_t cz ;

fflush( stdout );
if ( ansi_key_code_c1 != 0 )
	{			// there has been a catched key press
	return 1 ;
	}
cz = 0 ;
rv = read(0, &cz, 1);
ansi_key_code_c1 = cz ;
if ( rv == 1 )
	{			// Any key has been pressed
	return 1 ;
	}
vtrefresh();			// If no key pressed, display actual code
usleep( 4000L );	   // Pause 0.004 seconds
return 0 ;		// no key pressed - assume there is nothing to do
#endif

#if defined( MDOS40 ) || defined( MDOS10 )
if ( _kbhit() == 0 )
	{
	vtrefresh();
	vtrefresh();			// If no key pressed, display actual code
	vtcurdis();							// if no key pressed - display cursor
			// at actual cursor position
#if defined( MDOS40 )
	Sleep( 4L );			// give time to other threads / processes
#endif
	return false ;
	}
else
	{																	// a key is pressed
	return true ;
	}
#endif
}




FUNCT bool vtiskeyfast()	// for VTANSI
{

#ifdef OSK
if ( _gs_rdy(1) >= 1 ) return true ;
return false ;
#endif

#ifdef LINUX
int rv ;
uint8_t cz ;

if ( ansi_key_code_c1 != 0 )
	{			// there has been a catched key press
	return 1 ;
	}
cz = 0 ;
rv = read(0, &cz, 1);
ansi_key_code_c1 = cz ;
if ( rv == 1 )
	{			// Any key has been pressed
	return 1 ;
	}
return 0 ;		// no key pressed - assume there is nothing to do
#endif

#if defined( MDOS40 ) || defined( MDOS10 )
if ( _kbhit() == 0 )
	{
#if defined( MDOS40 )
#endif
	return false ;
	}
else
	{																	// a key is pressed
	return true ;
	}
#endif
}

FUNCT uint8_t getrawkeycode(void)
{
uint8_t c2 ;

#if defined( LINUX )
while ( !vtiskey() );							// wait until key pressed
if ( ansi_key_code_c1 == 0 )
	{
	Eprintf(( "ERROR vtkey: vtiskey true, ansi linux_key_code_c1==0" ));
	}
c2 = ansi_key_code_c1 ;
ansi_key_code_c1 = 0 ;
#elif defined( AVR )

while( UCSRA & ( 1<<RXC ) == 0 )
	{
	tsnext("wait for key");
	}
c2 = UDR ;
#else
if ( read( 1, &c2, 1 ) != 1 )
	{
	Eprintf(( "vtkey: read error" ));
	c2 = 0 ;		// read error
	return 0 ;
	}
#endif
return c2 ;
}



FUNCT uint vtkey()		 // for VTANSI (LINUX, OS9)
{
vtrefresh();


#if defined( MDOS40 ) || defined( MDOS10 )
uint keycode ;

keycode = _getch() ;
if ( keycode == 0x00 || keycode == 0xe0 )
	{															// a function key, lead in by 0
	keycode = _getch() + 0x100 ;		// 0x15E = Ctrl-F1
	}
else
	{
	keycode = charintable[ keycode ] ;
	}
DBprintf(( "keycode=%XH = %c", keycode, keycode ));
DBret("vtkey");
return keycode ;

#else
int sequence ;
uint32_t startmsec ;
uint8_t codenr ;

struct slookup {
	char text[NSEQ] ;
	uint8_t  keycode ;
	} ;

// list of ANSI one-Key Keystrokes which send through serial line more than
// one byte started by an escape character, which is not contained in this
// list.
#define MAXCODE 27
static struct slookup pattern[MAXCODE] =
	{
	{ "[A", VTK_UP	  -0x100 } ,	// Cursor Up
	{ "[B", VTK_DOWN  -0x100 } ,	// Cursor Down
	{ "[D", VTK_LEFT  -0x100 } ,	// Cursor Left
	{ "[C", VTK_RIGHT -0x100 } ,	// Cursor Right
	{ "[17~", VTK_F1  -0x100 } ,	// Function Key
	{ "[18~", VTK_F2  -0x100 } ,	// Function Key
	{ "[19~", VTK_F3  -0x100 } ,	// Function Key
	{ "[20~", VTK_F4  -0x100 } ,	// Function Key
	{ "[21~", VTK_F5  -0x100 } ,	// Function Key
	{ "[23~", VTK_F6  -0x100 } ,	// Function Key
	{ "[24~", VTK_F7  -0x100 } ,	// Function Key
	{ "[24~", VTK_F8  -0x100 } ,	// Function Key
	{ "[25~", VTK_F9  -0x100 } ,	// Function Key
	{ "[26~", VTK_F10  -0x100 },	// Function Key
	{ "[29~", VTK_S1  -0x100 } ,	// Function Key
	{ "[30~", VTK_S2  -0x100 } ,	// Function Key
	{ "[31~", VTK_S3  -0x100 } ,	// Function Key
	{ "[32~", VTK_S4  -0x100 } ,	// Function Key
	{ "[33~", VTK_S5  -0x100 } ,	// Function Key
	{ "[34~", VTK_S6  -0x100 } ,	// Function Key
	{ "[35~", VTK_S7  -0x100 } ,	// Function Key
	{ "[1~", VTK_HOME	  - 0x100 } ,
	{ "[2~", VTK_INSERT	  - 0x100 } ,
	{ "[3~", VTK_DELETE	  - 0x100 } ,
	{ "[4~", VTK_END	  - 0x100 } ,
	{ "[5~", VTK_PAGEUP	  - 0x100 } ,
	{ "[6~", VTK_PAGEDOWN - 0x100 }
	} ;
#ifndef AVR
fflush( stdout );
#endif

vt_raw_sequence[0] = getrawkeycode();			 // first byte read in

#ifdef LINUX	//
if ( vt_raw_sequence[0] & 0x80 )
	{							// Lead in a wide character
	sequence  = 1 ;
	if ( vtiskey() )
		{
		return ( (vt_raw_sequence[0] & 0xFF ) << 8) | (getrawkeycode() & 0xFF );
		}
	/* illegal Key */
	return vt_raw_sequence[0] & 0xFF ;
	}
#endif

if ( vt_raw_sequence[0] == 0x01b )
	{							// Lead in an ANSI Escape Sequence
	startmsec = getmsec() ;		// Escape sequences must be able to be
								// read fast
	sequence  = 0 ;
	for(;;)
		{
		if ( vtiskey() )
			{
			vt_raw_sequence[sequence] = getrawkeycode() ;
			startmsec = getmsec() ;
			++sequence ;
			if ( sequence >= NSEQ-1 ) break ;
			continue ;
			}
		if ( diffmsec(startmsec) > 80 ) break ;		// Time out
		vt_raw_sequence[sequence] = 0 ;
		for ( codenr = 0 ; codenr < MAXCODE ; ++codenr )
			{
			if ( isword( pattern[codenr].text, vt_raw_sequence ) )
				{			// finished known pattern
				return 0x100 + pattern[codenr].keycode ;
				}
			}
		}
		// time out
	vt_raw_sequence[sequence] = 0 ;
	if ( sequence == 0 ) return 0x01b ;
	DBprintf(( "ANSI : %v\n", vt_raw_sequence ));
	for ( codenr = 0 ; codenr < MAXCODE ; ++codenr )
		{
		if ( isword( pattern[codenr].text, vt_raw_sequence ) )
			{
			return 0x100 + pattern[codenr].keycode ;
			}
		}
	Eprintf(( "ANSI : %v\n", vt_raw_sequence ));
	/* illegal Key */
	return 0x1b ;			/* use as ESCAPE */
	}
return vt_raw_sequence[0] & 0xFF ;
#endif
}				// end vtkey()

// for VTANSI
FUNCT void xxvtcurpos(int x, int y) /* X = 0 .. 23	 y = 0 .. 79 */
{
DBent("xxvtcurpos");
if ( x <  0 ) x =  0 ;
if ( y <  0 ) y =  0 ;
if ( x > 79 ) x = 79 ;
if ( y > 23 ) y = 23 ;

printf( "\033[%d;%dH", y+1, x+1 );
#ifndef AVR
fflush(stdout);
#endif
DBret("xxvtcurpos");
}

#endif // VTANSI




#if defined( DOSGNU ) || defined( __TURBOCDOS__ )
extern bool chartablefilled ;
extern uint8_t charintable[256] ;
extern uint8_t charouttable[256] ;


FUNCT void vtrefresh()		// DOSGNU || __TURBOCDOS__
{
char cc ;
uint8_t at ;
union REGS inregs ;
uint x ;
uint y ;


if ( !vt_uses_screen )
	{
	fflush( stdout );
	}
else
	{
	for( y = 0 ; y < NLINES ; ++y )
	{
	x = 0 ;

	for ( x = 0 ; x < NCOLS ; ++x )
		{
		if ( act_screen[y][x].code != screen[y][x].code ||
		 act_screen[y][x].attr != screen[y][x].attr )
		{
		inregs.h.ah = 0x02 ;			// Set cursor
		inregs.h.bh = 0 ;				// Page number
		inregs.h.dh = y ;				// Line number (0 to 24)
		inregs.h.dl = x ;				// coloumn number (0 to 79)
		int10( &inregs);
		inregs.h.ah = 0x09 ;			// Set Attribute and character
		cc = act_screen[y][x].code = screen[y][x].code ;
		at = act_screen[y][x].attr = screen[y][x].attr ;

		inregs.h.al = charouttable[(uint8_t)cc] ;
						// Set Attribute and character
		inregs.x.cx = 1 ;				// number of characters
		inregs.h.bh = 0 ;				// page number
		inregs.h.bl = at ;
		int10( &inregs);				// display character at screen
		}
		}
	}
	inregs.h.ah = 0x02 ;			// Set cursor
	inregs.h.bh = 0 ;				// Page number
	inregs.h.dh = vt_curr_line ;		// Line number (0 to 24)
	inregs.h.dl = vt_curr_col ;		   // coloumn number (0 to 79)
	int10( &inregs);
	}

}


#endif



/* *************************************************************************
   *************************************************************************
*/
#if defined( DOSGNU )

#ifdef MDOS40
this should not happed
#endif

#define VIDEO_MODE 0x003		// 80 coloumns *25*16 colours
#include <stdio.h>


// extern "C" {
/* struct reg {
	int eax; int ebc;
	int ecx; int edx;
	int esi; int edi; int ebp ;
	};

extern int int10( struct reg *pr );
*/

// extern int int16( int eax, int ebc, int ecx, int edx,
//					int esi, int edi, int ebp);

/* extern bool kbhit(void); */
//	};

FUNCT void vtopen( void )				// set everything to be able to use vt
{
vt_uses_screen = false ;							 // use stdio, not screen io
}


FUNCT void xxvtopen()			// for DOSGNU
{
union REGS inregs ;

vt_xmax = 80 ;
vt_ymax = NLINES ;				// 24, 25, or up to 43
if ( !chartablefilled ) fillchartable() ;
// inregs.x.ax=17;	// VGA-Modus HIRES (640*480) BW

inregs.x.ax = 0x0003 ;					// Video Mode
inregs.x.bx = VIDEO_MODE ;		// 80 x 25 x 16

int10( &inregs);								// this will clear screen
vt_curr_col = 0 ;
vt_curr_line = 24 ;
putchar('\n'); putchar('\n'); putchar('\n'); putchar('\n'); putchar('\n');
putchar('\n'); putchar('\n'); putchar('\n'); putchar('\n'); putchar('\n');
putchar('\n'); putchar('\n'); putchar('\n'); putchar('\n'); putchar('\n');
putchar('\n'); putchar('\n'); putchar('\n'); putchar('\n'); putchar('\n');
putchar('\n'); putchar('\n'); putchar('\n'); putchar('\n'); putchar('\n');
vt_uses_screen = true ;
}



FUNCT void xxvtcurpos(int x, int y)	  // for DOSGNU
{
union REGS inregs ;

DBent("xxvtcurpos");

DBprintf(( "Set cursor to x=%d y=%d", x, y ));
if ( vt_uses_screen )						 // the vtcls() call enabled direct
	{											// screnn access
	if ( x >= 0 && x < vt_xmax && y >= 0 && y < vt_ymax )
	{
	vt_curr_col = x ;
	vt_curr_line = y ;
	// inregs.x.ax=17;	// VGA-Modus HIRES (640*480) BW

	inregs.h.ah = 0x02 ;		// Set cursor
	inregs.h.bh = 0 ;			// Page number
	inregs.h.dh = y ;			// Line number (0 to 24)
	inregs.h.dl = x ;			// coloumn number (0 to 79)
	int10( &inregs);
	}
	else
	{
	Eprintf(( "illegal cursor value x=%d y=%d, vt_xmax=%d, vt_ymax=%d",
		x, y, vt_xmax, vt_ymax ));
	}
	}
else
	{			// not opened by vtcls()
	vtocha( '\n' );
	}
DBret("xxvtcurpos");
}

FUNCT void vtclose()		// for DOSGNU
{
DBent("vtclose");
vtcurpos(0,vt_ymax-1);
vtputs("\n\n");					// scroll a little bit
vt_uses_screen = false ;
DBprintf(( "vt closed" ));
DBret("vtclose");
}




FUNCT bool vtiskey()					// for DOSGNU
{
// call dos function 11 (0x0B) : get key status

if ( ( int21ax( 0x0b00 ) & 0xff ) == 0 )
	{
	vtrefresh();
	return false ;
	}
else
	{																	// a key is pressed
	return true ;
	}
}


FUNCT bool vtiskeyfast()				// for DOSGNU
{
// call dos function 11 (0x0B) : get key status

if ( ( int21ax( 0x0b00 ) & 0xff ) == 0 )
	{
	vtrefresh();
	return false ;
	}
else
	{																	// a key is pressed
	return true ;
	}
}



// return 0-FF ASCII / ANSI key code 100-1FF extended Code, ALT_Keys,
// Function Keys
FUNCT uint vtkey()						 // for DOSGNU
{
uint keycode ;

DBent("vtkey");
if ( !chartablefilled ) fillchartable() ;

vtrefresh();					// dump output buffer to dialog screen

// 0x0700 ;				// Get key code (from DOS)

keycode = int21ax( 0x700 ) & 0xff ;
if ( keycode == 0 )
	{							// a function key, lead in by 0
	keycode = 0x100 + ( int21ax( 0x700 ) & 0xff ) ;
	}
else
	{
	keycode = charintable[ keycode ] ;
	}
DBprintf(( "keycode=%XH = %c", keycode, keycode ));
DBret("vtkey");
return keycode ;
}



#endif	// DOSGNU


// __TURBOCDOS__
/* *************************************************************************
   *************************************************************************
*/
#ifdef __TURBOCDOS__
#define VIDEO_MODE 0x003		// 80 coloumns *25*16 colours
#include <stdio.h>
#include <dos.h>				// union REGS
extern bool chartablefilled ;
extern uint8_t charintable[256] ;
extern uint8_t charouttable[256] ;



FUNCT bool vtiskey()					// for __TURBOCDOS__
{
union REGS inregs ;


inregs.x.ax = 0x0b00 ;					// check if keyboard is hit
vtrefresh();
int86( 0x21, &inregs, &inregs);
if ( ( inregs.x.ax & 0xff ) == 0 )
	{
	return false ;
	}
else
	{
	return true ;
	}
}



FUNCT bool vtiskeyfast()				// for __TURBOCDOS__
{
union REGS inregs ;


inregs.x.ax = 0x0b00 ;					// check if keyboard is hit
int86( 0x21, &inregs, &inregs);
if ( ( inregs.x.ax & 0xff ) == 0 )
	{
	return false ;
	}
else
	{
	return true ;
	}
}




FUNCT uint vtkey()						 // for __TURBOCDOS__
{
union REGS inregs ;


DBent("vtkey");
vtrefresh();							// refresh screen if neccesary
inregs.x.ax = 0x0700 ;	// Get Key Code

int86( 0x21, &inregs, &inregs);
if ( ( inregs.x.ax & 0xff ) == 0 )
	{			// a function key, lead in by 0
	inregs.x.ax = 0x0700 ;	// Get Key Code extension

	int86( 0x21,&inregs, &inregs);
	DBret("vtkey");
	return 0x100 + ( inregs.x.ax & 0xff ) ;
	}
else
	{
	DBret("vtkey");
	return charintable[ inregs.x.ax & 0xFF ] ;
	}
}


FUNCT void xxvtopen()		  // for __TURBOCDOS__
{
uint y ;
union REGS inregs ;
union REGS otregs ;

if ( !chartablefilled ) fillchartable() ;
// inregs.x.ax=17;	// VGA-Modus HIRES (640*480) BW

inregs.x.ax = 0x0003 ;	// Video Mode
inregs.x.bx = VIDEO_MODE ;	 // 80 x 25 x 16

int86( 0x10,  &inregs, &otregs );
vtattr( VTANORM );
for ( y = 0 ; y < vt_ymax ; ++y )
	{
	xxvtcurpos(0,y);			// set physical cursor

	inregs.h.ah = 0x09 ;		// Set Attribute and character
	inregs.h.al = ' ' ;			// character

	inregs.x.cx = vt_xmax ;		// number of characters
	inregs.h.bh = 0 ;			// page number
	inregs.h.bl = VTANORM ;
	int86( 0x10,  &inregs, &otregs );
	}
xxvtcurpos(0,1);
vt_uses_screen = true ;
}

FUNCT void vtopen()
{
if ( !chartablefilled ) fillchartable() ;

vt_uses_screen = false ;
}



FUNCT void xxvtcurpos(uint x, uint y)	// for __TURBOCDOS__
{
union REGS inregs ;
union REGS otregs ;

DBent("xxvtcurpos");

DBprintf(( "Set cursor to x=%d y=%d", x, y ));
if ( x < vt_xmax && y < vt_ymax )
	{
	act_vt_curr_line = y ;
	act_vt_curr_col = x ;

	// inregs.x.ax=17;	// VGA-Modus HIRES (640*480) BW
	inregs.h.ah = 0x02 ;		// Set cursor
	inregs.h.bh = 0 ;			// Page number
	inregs.h.dh = y ;			// Line number (0 to 24)
	inregs.h.dl = x ;			// coloumn number (0 to 79)
	int86(0x10, &inregs, &otregs );
	}
else
	{
	Eprintf(( "illegal cursor value x=%d y=%d, vt_xmax=%d, vt_ymax=%d",
		x, y, vt_xmax, vt_ymax ));
	}
DBret("xxvtcurpos");
}



FUNCT void vtclose()		// for __TURBOCDOS__
{
DBent("vtclose");
vtcurpos(0,vt_ymax-1);
vtputs("\n\n");					// scroll a little bit
vt_uses_screen = false ;
DBprintf(( "vt closed" ));
DBret("vtclose");
}



#endif	// __TURBOCDOS__




// /////////////////////////////////////////////////////////////
#ifdef _WIN31_


bool vt_redisplayed = false ;	// displayed but not read
bool vt_want_quit = false ;
bool vt_shiftstatus = false ;	// Shift key is pressed

bool win_refresh = false ;		// WMPAINT for Refresh - Paint just the differences

HWND vtWnd ;		// my windows handle

#include <windows.h>
MSG	  msg_win31_mainloop ;
#define NWIN31_KEYBUFFER 0x40
#define MASKWIN31_KEYBUFFER (NWIN31_KEYBUFFER-1)


struct swin31_keybuffer {
	int ifill ;
	int iempty ;
	int array[ NWIN31_KEYBUFFER ];
	} ;

struct swin31_keybuffer win31_keybuffer ;
HANDLE vt_hInstance ;		// graphics requires this instance

extern void vtnl( void );

// Note: keep here the keyword _export, even if compiler tells not to use it.
extern long FAR PASCAL _export MainWndProc(HWND hWnd,UINT message, UINT wParam, LONG lParam) ;






#ifndef NO_WINMAIN_IN_VT
// FUNCT int PASCAL WinMain(HANDLE hInstance, HANDLE hPrevInstance,
		//LPSTR lpCmdLine, int nCmdShow)
FUNCT int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
		LPSTR lpCmdLine, int nCmdShow)

{
#define NARG 25			// maximum argument count
#define NARGBUF 256 // all arguments
int iarg ;
int isave ;
// char *argp ;			// pointer to currently detected argument

const char *pparg[NARG]= { "myprog" } ;
char allargs[NARGBUF] ;

vt_hInstance = hInstance ;
//	  if (!hPrevInstance)
//	  if (!InitApplication(hInstance))
//		  return (false);

//	  if (!InitInstance(hInstance, nCmdShow))
//		  return (false);

vt_ymax = NLINES ;
vt_xmax = NCOLS ;
vtcls();				// Clear screen emualation buffer

WNDCLASS  wc;
isave = 0 ;
for ( iarg = 1 ; ; ++iarg )
	{								// check next Byte
	skipwhite( lpCmdLine );
	if ( *lpCmdLine == 0 ) break ;		// End of Commandline
	if ( iarg >= NARG )
		{
		MessageBox( NULL, "Too many different arguments in invocation line","CPP", MB_OK );
		return false ;
		}
	pparg[iarg] = allargs+isave ;
	for(;;)		// copy rest of line
		{
		if ( isave >= NARGBUF-1 )
			{
			MessageBox(NULL,"Arguments exceed NARGBUF","CPP",MB_OK );
			return false ;
			}
		if ( *lpCmdLine == ' ' || *lpCmdLine == '\t' ) break ;
		if ( *lpCmdLine == 0 ) break ;
		allargs[isave++] = *lpCmdLine++ ;	// copy this Byte, too
		}
	allargs[isave++] = 0 ;
	}								// next iarg
wc.style = CS_HREDRAW | CS_VREDRAW;
wc.lpfnWndProc = MainWndProc;
wc.cbClsExtra = 0;
wc.cbWndExtra = 0;
wc.hInstance = hInstance;
wc.hIcon = LoadIcon(hInstance, "sort");
wc.hCursor = LoadCursor(NULL, IDC_ARROW);

// For MSVC60 or 70 (MFC)
#if defined( _MFC )
wc.hbrBackground = (HBRUSH__ *) GetStockObject(WHITE_BRUSH);
#else
wc.hbrBackground = (HBRUSH) GetStockObject(WHITE_BRUSH);
#endif


wc.lpszMenuName =  "cppMainMenu";
wc.lpszClassName = "cppwinclass";

if ( !RegisterClass(&wc) )
	{
	return (false) ;
	}


vtWnd = CreateWindow(
			"cppwinclass",
			"cppmain",
			WS_OVERLAPPEDWINDOW,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			12*NCOLS,
			18*NLINES,
			NULL,
			NULL,
			hInstance,
			NULL
			);

if (!vtWnd)
	{
	return (false);		// could not create Window - cannot use it
	}

ShowWindow(vtWnd, nCmdShow);
UpdateWindow(vtWnd);

main(iarg,pparg);	// main is a macro and defines mymain
return (true) ;		// all done
}
#endif			// NO_WINMAIN_IN_VT







// ---------------------------------------------------------------------------
FUNCT void vtopen()					// for _WIN31_ (MSVC10||MSVC40)
{
vtcls();
vt_uses_screen = true ;
}



// ---------------------------------------------------------------------------
FUNCT void vtclose()				// for _WIN31_
{
MSG msgThisMsg ;
int iloop ;

if ( vt_redisplayed )				// HJH980629
	{		// user could not read all display
	// so wait for user input
	vtputs( "\nPress space to exit\n" );
	vtkey();
	}
vt_uses_screen = false ;


for ( iloop = 1 ; iloop < 6 ; ++iloop )
	{
	if ( PeekMessage( &msgThisMsg, vtWnd, 0, 0, PM_REMOVE ) != false )
		{
		TranslateMessage( &msgThisMsg );
		DispatchMessage( &msgThisMsg );
		}
	}
}




FUNCT void xxvtopen(void)
{
vt_uses_screen = true ;
return ;		// all initialization already done
}


// ---------------------------------------------------------------------------
// extern int _kbhit();
FUNCT bool vtiskey( void )			  // for _WIN31_
{

if ( vt_want_quit ) exit(0);

vtrefresh();					// refresh screen

if ( win31_keybuffer.ifill != win31_keybuffer.iempty )
	{
	vt_redisplayed = false ;		// user has seen display
	return true ;
	}
else
	{
	return false ;
	}
}

// -----------------------------------------------------------------
// refreshes screen, if neccesary
// called when vtiskey() is idling
// als c alled when waiting for any input
// should be called periodically from any loop
FUNCT void vtrefresh()				// _WIN31_
{
MSG msgThisMsg ;
uint il ;
uint ic ;
bool is_different ;
static uint32_t last_refresh ;
static bool has_refreshed ;
int iloop ;
#define MAX_LOOP 0

DBent("vtrefresh");

if ( has_refreshed == false )
	{			// never yet
	has_refreshed = true ;
	}
// Process possible windows messages to my vtWnd Window
iloop = 0 ;
for(;;)
	{
	if ( iloop > MAX_LOOP ) break ; // 4, 6, 10, 40
	if ( PeekMessage( &msgThisMsg, vtWnd, 0, 0, PM_REMOVE ) != false )
		{
		iloop = 0 ;
		TranslateMessage( &msgThisMsg );
		DispatchMessage( &msgThisMsg );
		}
	++iloop ;
	}

is_different = false ;
if ( act_vt_curr_line != vt_curr_line ||
	 act_vt_curr_col  != vt_curr_col )
	{
	is_different = true ;
	}
else
	{

	for( il = 0 ; is_different == false && il < NLINES ; ++il )	  // for all lines on screen emulation
		{
		for ( ic = 0 ; ic < NCOLS ; ++ic )
			{
		// optimization: Do not repaint character which have not changed
			if ( screen[il][ic].code == act_screen[il][ic].code	 &&
			 screen[il][ic].attr == act_screen[il][ic].attr )
				{
				continue ;	// Is already paint
				}
			is_different = true ;
			break ;
			}
		}
	}
if ( is_different )
	{
//	  Invalidate( vtWnd );	// just display differences
	win_refresh = true ;
	InvalidateRect( vtWnd,
		0,					// RECT - coordinates
		0 );				// 0 do not erase background
	win_refresh = false ;
	// Process possible windows messages to my vtWnd Window
	iloop = 0 ;
#ifndef NO_WINMAIN_IN_VT
	for(;;)
		{
		if ( iloop > MAX_LOOP ) break ; // 4, 6, 10, 40
		if ( PeekMessage( &msgThisMsg, vtWnd, 0, 0, PM_REMOVE ) != false )
			{
			iloop = 0 ;
			TranslateMessage( &msgThisMsg );
			DispatchMessage( &msgThisMsg );
			}
		++iloop ;
		}
#endif

	}



vt_redisplayed = true;		// has displayed - but did user see it _
DBret("vtrefresh");
return ;
}


FUNCT uint vtkey()			// _WIN31_
{
int rv ;

while( !vtiskey() ) ;		// wait until key pressed

rv = win31_keybuffer.array[ win31_keybuffer.iempty ] ;
win31_keybuffer.iempty = ( win31_keybuffer.iempty + 1 ) & MASKWIN31_KEYBUFFER ;
return rv ;
}
// The windows callback function

/****************************************************************************

	FUNCTION: MainWndProc(HWND, UINT, UINT, LONG)

	PURPOSE:  Processes messages

	MESSAGES:

	WM_COMMAND	  - application menu
	WM_CREATE	  - create window and objects
	WM_PAINT	  - update window, draw objects
	WM_DESTROY	  - destroy window


****************************************************************************/

FUNCT void vt_win_keypress(uint16_t keycode)	// _WIN31_
{
int inewfill ;

DBent("vt_win_keypress");
inewfill = ( win31_keybuffer.ifill + 1 ) & MASKWIN31_KEYBUFFER ;
DBprintf(( "inewfill=%d keycode=%d", inewfill, keycode ));
if ( inewfill != win31_keybuffer.iempty )
	{	// can fill more
	win31_keybuffer.array[win31_keybuffer.ifill] = keycode ;
	win31_keybuffer.ifill = inewfill ;
	}
else
	{	// application internal keyboard buffer full
	Eprintf(( "Keyboard buffer full" ));
	}
DBret("vt_win_keypress");
}




FUNCT void vt_win_keyup( UINT wParam, LONG lParam)
{
DBprintf(( "WM_KEYUP, wParam=%XH, lParam=%lXH", wParam, lParam ));
	if ( wParam == 0x0010 && lParam == 0x002A0001L )
	{				// Shift Key released
	vt_shiftstatus = false ;
	}
}



FUNCT long vt_win_keydown( UINT wParam, LONG lParam)
{
DBent("vt_win_keydown");
DBprintf(( "WM_KEYDOWN, wParam=%XH, lParam=%lXH", wParam, lParam ));
// Cursor Down :  lParam=22020097=0x01500001, wparam=40=0x0028
// Cursor Up :	  lParam=21495809=0x01480001, wparam=38=0x0026
// Cursor left :  lParam=21692417=0x014B0001, wparam=37=0x0025
// Cursor right : lParam=21823489=0x014D0001, wparam=39=0x0027

	if ( lParam == 0x01500001 )
		{
		vt_win_keypress( VTK_DOWN );
		DBret("vt_win_keydown");
		return 0 ;
		}
	if ( lParam == 0x01480001 )
		{
		vt_win_keypress( VTK_UP );
		DBret("vt_win_keydown");
		return 0 ;
		}
	if ( lParam == 0x014B0001 )
		{
		vt_win_keypress( VTK_LEFT );
		DBret("vt_win_keydown");
		return 0 ;
		}
	if ( lParam == 0x014D0001 )
		{
		vt_win_keypress( VTK_RIGHT );
		DBret("vt_win_keydown");
		return 0 ;
		}

	if ( lParam == 0x01520001 )
		{
		vt_win_keypress( VTK_INSERT );
		DBret("vt_win_keydown");
		return 0 ;
		}
	if ( lParam == 0x01470001 )
		{
		vt_win_keypress( VTK_HOME );
		DBret("vt_win_keydown");
		return 0 ;
		}
	if ( lParam == 0x01490001 )
		{
		vt_win_keypress( VTK_PAGEUP );
		DBret("vt_win_keydown");
		return 0 ;
		}
	if ( lParam == 0x01530001 )
		{
		vt_win_keypress( VTK_DELETE );
		DBret("vt_win_keydown");
		return 0 ;
		}
	if ( lParam == 0x014F0001 )
		{
		vt_win_keypress( VTK_END );
		DBret("vt_win_keydown");
		return 0 ;
		}
	if ( lParam == 0x01510001 )
		{
		vt_win_keypress( VTK_PAGEDOWN );
		DBret("vt_win_keydown");
		return 0 ;
		}
	if ( wParam == 0x0010 && lParam == 0x002A0001L )
		{				// Shift Key pressed
		vt_shiftstatus = true ;
		}
	if ( wParam >= 0x70 && wParam <= 0x7A &&
		 ( lParam & 0xff0000L) >= 0x3b0000L &&
		 ( lParam & 0xff0000L) <= 0x440000L &&
		 (lParam & 0x0000ffffL) == 0x0001	)

		{
		vt_win_keypress( wParam - 0x70 + VTK_F1 ) ;			   // F1 .. F10
		DBret("vt_win_keydown");
		return 0 ;		// Message done
		}
	DBret("vt_win_keydown");
	return 1 ;
}




// Also called from CWSIMATMView::OnDraw
void RefreshVtWindow( HDC hDC )
{
uint ic ;				// coloumn counter
uint icstart ;
uint il ;				// line counter
static uint icleft = 0 ;		   // left start of compared field
uint8_t actattr ;
char ach[NCOLS+2] ;
HGDIOBJ hgdiobj ;
TEXTMETRIC textmetric ;
int i ;

hgdiobj = GetStockObject( ANSI_FIXED_FONT );

SelectObject( hDC, hgdiobj );
GetTextMetrics( hDC, &textmetric );

if ( win_refresh == false )
	{
	for( il = 0 ; il < NLINES ; ++il )	 // for all lines on screen emulation
		{
		for ( ic = icleft ; ic < NCOLS ; ++ic )
			{
			act_screen[il][ic].code = 0 ;
			}
		}
	}

for( il = 0 ; il < NLINES ; ++il )	 // for all lines on screen emulation
	{
	icleft = 0 ;
	for( ; icleft < NCOLS ; )
		{
		for ( ic = icleft ; ic < NCOLS ; ++ic )
			{
			// optimization: Do not repaint character which have not changed
			actattr = screen[il][ic].attr ;
			if ( screen[il][ic].code == act_screen[il][ic].code	 &&
				 actattr == act_screen[il][ic].attr )
				{
				continue ;	// Is already paint
				}
			if ( il == vt_curr_line && ic == vt_curr_col )
				{			// this single character will be set as cursor position after refresh
				act_screen[il][ic].code = screen[il][ic].code ;
				act_screen[il][ic].attr = actattr ;		// and its attribute
				continue ;
				}
			ach[0] = screen[il][ic].code ;
			// Differences start at Position ic
			if ( actattr == attriblist[VTAINV] )
				{
				SetTextColor(hDC, RGB(255,255,255) ) ;
				SetBkColor(hDC, RGB(0,0,0) );
				}
			else if ( actattr == attriblist[VTASTRONG] )
				{
				SetTextColor(hDC, RGB(0,0,255) ) ;
				SetBkColor( hDC, RGB(255,255,0) );
				}
			else // if ( actattr == attriblist[VTANORM] )
				{
				SetTextColor(hDC, RGB(0,0,0) ) ;
				SetBkColor(hDC, RGB(255,255,255) );
				}
			break ;		// have found differences
			}	// next Coloumn ic
		if ( ic >= NCOLS ) break ;	// exit icleft loop - nothing to refresh in this line
		i = 1 ;		// has first character in string buffer ach[]
		icstart = ic ;
		act_screen[il][ic].code = ach[0] ;		// the first non match character
		act_screen[il][ic].attr = actattr ;		// and its attribute
		while ( ++ic < NCOLS )
			{	// Is next character also different, (Then repaint is neccesary)
				// Will now repaint all different characters of same attribute
			if ( screen[il][ic].code == act_screen[il][ic].code ||
				screen[il][ic].attr != actattr )
				{	// same code or other attr
				break ;
				}
			// other code and same attr
			ach[i] = screen[il][ic].code ;
			act_screen[il][ic].code = ach[i]  ;
			act_screen[il][ic].attr = actattr ;
			++i ;
			}		/* end while */
		ach[i] = 0 ;		// refreshes icleft to ic - string in ach
		// --ic ;
		TextOut( hDC, icstart * textmetric.tmMaxCharWidth, il * textmetric.tmHeight , (LPSTR)ach, i );
		icleft = ic ;
		}
	}		// check if have to refresh next line il
if ( act_vt_curr_line != vt_curr_line || act_vt_curr_col != vt_curr_col )
	{				// Where the cursor is:
	SetTextColor(hDC, RGB(0,0,0) ) ;
	SetBkColor(hDC, RGB(0,196,255) );
	act_screen[vt_curr_line][vt_curr_col].attr = 126 ;	// will be refreshed next time
	act_vt_curr_line = vt_curr_line ;
	act_vt_curr_col = vt_curr_col ;
	TextOut( hDC, vt_curr_col * textmetric.tmMaxCharWidth, vt_curr_line * textmetric.tmHeight ,
			&screen[vt_curr_line][vt_curr_col].code, 1 );
	}
}



// ---------------------------------------------------------
long FAR PASCAL _export MainWndProc(HWND hWnd,UINT message,
			UINT wParam,
			LONG lParam)

{
HDC hDC ;			 //
PAINTSTRUCT ps;		 // paint structure
long rv ;

DBent("MainWndProc");
DBprintf(( "message=%u wParam=%u lParam=%lu", message, wParam, lParam ));
switch (message)
	{
case WM_COMMAND: // one of the menu points is selected
	break ;

case WM_CREATE:
	DBret("MainWndProc");
	return 0;

case WM_KEYUP :
	vt_win_keyup(wParam,lParam);
	break ;

case WM_KEYDOWN :
	if ( vt_win_keydown(wParam,lParam) == 0 ) return 0 ;
	break ;
case WM_CHAR:
	DBprintf(( "WM_CHAR, wParam=%XH, lParam=%lXH", wParam, lParam ));
	vt_win_keypress(wParam) ;
	DBret("MainWndProc");
	return 0 ;

case WM_ERASEBKGND:
	lParam = lParam;
	wParam = wParam;
	break ;

case WM_PAINT:
//		InvalidateRect( hWnd, NULL, true );			//invalidate entire window, erase

/*	  for( il = 0 ; il < NLINES ; ++il )		  // for all lines on screen emulation
	{
	for ( ic = 0 ; ic < NCOLS ; ++ic )
		{
		act_screen[il][ic].code = 0 ;		// Blank
		// screen[il][ic].code = ' ' ;		// Blank
		act_screen[il][ic].attr = VTANORM ; // IS all screen
		}
	}
*/
	// case MESSAGE_UPDATE :
	hDC = BeginPaint( hWnd, &ps );
	RefreshVtWindow( hDC );
//	  EndPaint( hDC, &ps );
	EndPaint( hWnd, &ps );
	DBret("MainWndProc");
	return 0;

case WM_DESTROY:
	DBprintf(( "WM:DESTROY" ));
	PostQuitMessage(0);
	vt_want_quit = true ;
	DBret("MainWndProc");
	return 0;

default:
	// 70 =
	// 133 =
	rv = (DefWindowProc(hWnd, message, wParam, lParam));
	DBret("MainWndProc");
	return rv ;
	}

rv = (DefWindowProc(hWnd, message, wParam, lParam));
DBret("MainWndProc");
return rv ;
}
#endif


#if __TURBOC__
void dummy_xxx_ttt( void )
{
double xyz ;
xyz = 5.6 ;
printf( "%12.6f", xyz );
}
#endif

