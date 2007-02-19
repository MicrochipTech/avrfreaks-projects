#ifndef __hevt_h__
#define __hevt_h__


#ifndef __heport_h__
#include <heport.h>
#endif

#ifndef __helq_h__
#include <helq.h>
#endif

#ifndef __st_h__
#include <st.h>
#endif


#if defined( AVR ) || defined( OSK ) || defined( LINUX ) || defined( MDOS40 ) || defined( MDOS10 )
#ifndef _WIN31_
#define VTANSI
#endif
#endif


extern uint16_t vt_xmax ;
extern uint16_t vt_ymax ;
extern bool vt_opened ;			// true if screen addressing enabled

void	vtopen( void );				// Start dialog on Video Terminal 80x24

void	vtclose( void );

bool	vtiskey( void );
bool	vtiskeyfast( void );
uint	vtkey( void );

#if defined( DOSGNU ) || defined( MDOS40 ) || defined ( __TURBOC__ )
void fillchartable(void) ;
#endif

// Return values of vtkey :
#define VTK_TAB		 0x009								// the TAB key
#define VTK_BACKTAB	 0x008								// the back-Tab-Key
// #define VTK_BACKTAB	0x10F							// the back-Tab-Key

#ifdef LINUX
#define VTK_ENTER	 0x00A				// Enter Key
#else
#define VTK_ENTER	 0x00D				// Enter Key
#endif
#define VTK_DOWN	 0x150				// Cursor Down
#define VTK_UP		 0x148				// Cursor Up
#define VTK_LEFT	 0x14B
#define VTK_RIGHT	 0x14D
#define VTK_F1		 0x13B				// Function Key 1
#define VTK_F2		 0x13C
#define VTK_F3		 0x13D
#define VTK_F4		 0x13E
#define VTK_F5		 0x13F
#define VTK_F6		 0x140
#define VTK_F7		 0x141
#define VTK_F8		 0x142
#define VTK_F9		 0x143
#define VTK_F10		 0x144
// Function keys with Shift-Key (on PC)
#define VTK_S1		 0x154				// Shift Function Key
#define VTK_S2		 0x155
#define VTK_S3		 0x156
#define VTK_S4		 0x157
#define VTK_S5		 0x158
#define VTK_S6		 0x159
#define VTK_S7		 0x15A
#define VTK_S8		 0x15B
#define VTK_S9		 0x15C
#define VTK_S10		 0x15D
// Function keys with CTRL-Key (on PC)
#define VTK_C1		 0x15E
#define VTK_C2		 0x15F
#define VTK_C3		 0x160
#define VTK_C4		 0x161
#define VTK_C5		 0x162
#define VTK_C6		 0x163
#define VTK_C7		 0x164
#define VTK_C8		 0x165
#define VTK_C9		 0x166
#define VTK_C10		 0x167
// Function keys with ALT-Key (on PC)
#define VTK_A1			 0x168
#define VTK_A2			 0x169
#define VTK_A3			 0x16A
#define VTK_A4			 0x16B
#define VTK_A5			 0x16C
#define VTK_A6			 0x16D
#define VTK_A7			 0x16E
#define VTK_A8			 0x16F
#define VTK_A9			 0x170
#define VTK_A10			 0x171

#define VTK_PAGEUP	 0x149
#define VTK_PAGEDOWN 0x151
#define VTK_HOME	 0x147
#define VTK_END		 0x14F
#define VTK_INSERT	 0x152
#define VTK_DELETE	 0x153
#define VTK_ACK		 0x1B				// ACK , usually Escape
#define VTK_ABORT	 0x1B				// ABORT, usually Escape


#define CUR_LEFT(r) (r == 0x14b || r == 0x13 )
#define CUR_RIGHT(r)	(r == 0x14d || r == 0x04 )
#define CUR_UP(r)	(r == 0x148 || r == 0x05 )
#define CUR_DOWN(r) (r == 0x150 || r == 0x18 )



// The conrol character are: a=1, b=2 c=3 d=4 e=5 f=6 g=7 h=8
//							 i=9 j=0x0a k=0x0b l=0x0c m=0x0d n=0x0e
//							 o=0x0f p=0x10 q=0x11 r=0x12 s=0x13 t=0x14
//							 u=0x15 v=0x16 w=0x17 x=0x18 y=0x19 z=0x1a

typedef uint8_t vteattrib ;
#define VTANORM 0	// Attributes normal
#define VTASTRONG 1 // Attributes strong
#define VTAINV 2	// Attributes invers
#define VTABLINK 3	// Attributes blink
			// these 4 codes may be used in strings to vtputs vtprintf
			// or as characters to vtocha also in form \001 .. \004

#define VTASELECT 2

void vtcls(void);				// Clear screen, enable screen addressing
void vtcurpos(uint x, uint y);	  // x=0..79, y=0..23
void vtattr(vteattrib how);
void vtcolors( uint8_t fg, uint8_t bg );	// set colors, where colors are:

// #ifdef LINUX
// Gilt für LINUX und Windows (ANSI)
#define VTC_BLACK 0
#define VTC_BLUE 4
#define VTC_RED 1
#define VTC_VIOLET 5
#define VTC_GREEN 2
#define VTC_CYAN 6
#define VTC_BROWN 3
#define VTC_WHITE 7
// #else
/*
#define VTC_BLACK 0
#define VTC_BLUE 1
#define VTC_RED 2
#define VTC_VIOLET 3
#define VTC_GREEN 4
#define VTC_CYAN 5
#define VTC_BROWN 6
#define VTC_WHITE 7
*/
// #endif

void vtocha(char zch);
void vtbig( char zch, uint32_t  delay=0 );		 // big, max 10 chars per line, max one line
void vtbig( char *text, uint32_t	delay=0 );

void vtputs(const char *pstr);
int	 vtgetlin( char *where, int how_much);
int	 vtgetlin( Cst &there );


int	 vtfileselect(
	char *where,
	int max_string,
	const char *tDir,
	const char *tExt,
	const int usage = O_RDONLY );	// for create O_WRONLY or open existing file

void vthex8(uint32_t value);
void vthex4(uint16_t value);
void vthex2(uint8_t value);
void vtdump( const char *ptr, const uint size,
			const char *key, const uint32_t add=0);

void vtnl(void);
void vtdeleol(void);
void vtdelbol(void);
void vtcurdn( void );		// used by trmansi()
void vtcurup( void );		// used by trmansi()
void vtsetscrol( uint8_t ymin, uint8_t ymax );
void vtrefresh(void);			// vtiskey of vtkey() automatically refreshes

inline void vthome(void) { vtcurpos( 0 , 0 ); }

void vtcompil();								// Displays used compiler
												// See also: logvtcompil

void  vtcurdis( void ) ;
void  vtgcurpos(  uint8_t *pcol, uint8_t *pline );

#ifndef DEF_VTPRINTF				  // don´t make prototypes
extern "C" {
void vtprintf(char const *fmt, ... );
}
#endif
#ifndef DEF_VTERR					  // don´t make prototypes
extern "C" {
void vterr(int line, char const *fmt, ... );	// with waiting for key
}
#endif
#ifndef DEF_VTMSG										  // don´t make prototypes
extern "C" {
void vtmsg(uint16_t line, const char *fmt, ... );	// without waiting for Key
}
#endif




// is screen emulated inside a part of memory - as required for Windows
// or not - if we have a real ANSI terminal
#if	 defined( AVR ) || defined( OSK ) || defined( LINUX ) || defined( MDOS40 )
// not emulated - having a real terminal or an ANSI emulation
#define VT_MEMORY 1
#define VT_ANSI 1
#endif


#if defined( _WIN31_ ) || defined( __TURBOC__ ) || defined( DOSGNU ) || defined( MDOS10 ) || defined( MSVC10 )
// emulated - then emulate in a part of memory which is defined here :
#define VT_MEMORY 1
#endif	// emulated terminal in memory

#if VT_MEMORY
struct sscreenemu {
	uint8_t attr ;
	char code ;
	} ;

#if defined( MDOS40 ) || defined( MDOS10 ) || defined( __TURBOC__ )
#define NLINES 25
#else
#if	 defined( AVR )
#define NLINES 1
#else
#define NLINES 24
#endif
#endif

#if	 defined( AVR )
#define NCOLS 40
#else
#define NCOLS 80
#endif

extern struct sscreenemu screen[NLINES][NCOLS] ;
extern struct sscreenemu act_screen[NLINES][NCOLS] ;
extern uint vt_curr_line ;
extern uint vt_curr_col ;
extern vteattrib vt_current_attribute ;
#endif

const char *vtkeyname( uint keycode );

// next definitions are used by templates - if contents should be printed
// type dependend
#ifndef DEF_VTPRINTF				  // don´t make prototypes
inline void vt( int x )			// Display - somehow - an int
	{
	vtprintf( " %d", x );
	}
inline void vt( uint x )		 // Display - somehow - an int
	{
	vtprintf( " %u", x );
	}
inline void vt( double x )				// Display - somehow - an int
	{
	vtprintf( " %12.6f", x );
	}
inline void vt( long x )				// Display - somehow - an long
	{
	vtprintf( " %ld", x );
	}
#if !defined( LINUX )
inline void vt( uint32_t x )				// Display - somehow - an unsigned long
	{
	vtprintf( " %lu", x );
	}
#endif
inline void vt( char x )				// Display - somehow - an char
	{
	vtocha( x );
	}
inline void vt( char  *px )	// Display - somehow - an char array
	{
	vtputs( px );
	}
inline void vt( Cst &x )				// Display - somehow - an char array
	{
	vtputs( x.Start() );
	}
#endif			// DEF_VTPRINTF don´t make prototypes


// Change a value of type ...
// if modified, then return 0, so caller knows he must rewrite
int vtc_tok( const char *pname, char *pstore, uint n );		// Change a token
int vtc_str( const char *pname, char *pstore, uint n );		// Change a string
int vtc_sint( const char *pname, sint *pstore );			// Change a signed int
int vtc_uchar( const char *pname, uint8_t *pstore );			 // Change an unsigned char
int vtc_uword( const char *pname, uint16_t *pstore );			// Change a unsigned word
int vtc_uword_hex4( const char *pname, uint16_t *p );			// Change a unsigned word - HEX
int vtc_uint( const char *pname, uint *pstore );			// Change an unsigned int
int vtc_ulong( const char *pname, uint32_t *pstore );			// Change an unsigned long
int vtc_sword( const char *pname, int16_t *pstore );			// Change a signed word
int vtc_slong( const char *pname, int32_t *pstore );			// Change a signed long
int vtc_double( const char *pname, double *pstore );		// Change a double number
int vtc_bool( const char *pname, bool *pstore );


#ifdef _WIN31_
// Helper functions only for MS Win
void vt_win_keypress( uint16_t keycode );
void vt_win_keyup( UINT wParam, LONG lParam );
long vt_win_keydown( UINT wParam, LONG lParam );
#endif

#endif	// __hevt_h__
