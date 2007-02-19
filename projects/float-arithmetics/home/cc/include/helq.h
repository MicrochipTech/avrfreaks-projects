/* here are defined all the prototypes of all functions which are coded
   in subdirectory home/cc/lq
*/


#define E // mark an error when a program returns error code
			// used as	 return E 4545	 or	  rv = E 6767

#ifndef __helq_h__
#define __helq_h__

#include <stdio.h>
#include <string.h>		// strcmp strcpy

#include <limits.h>		// CHAR_MAX,

#ifndef AVR
#include <locale.h>		// defines struct lconv
#endif

#ifndef __heport_h__
#include <heport.h>
#endif

// char *getcwd( char *where_to_store, int how_many_characters );
#if defined( MSVC40 ) || defined( MDOS40 ) || defined( MSVC10 ) || defined( MDOS10 )
#include <direct.h>
#define getcwd(x,y) _getcwd(x,y)
#define lstat(a,b) _stat(a,b)
#define stat _stat
#define mkdir(a,b) _mkdir(a)	// no mode parameter for DOS / Win3.11
								// a = file name, b=mode
#endif

#ifdef DOSGNU
#include <unistd.h>			// read, write
#endif

uint32_t	lprbs(void) ;			// long pseudo random 0..FFFFFFFF
extern uint32_t lprbs_word ;		// Start value
double	dprbs(void);			// double pseudo random 0.0000 .. 1.00000


bool	isword(const char *p1,	   // Are two strings identical
			const char *p2 );	   // ignore case
// return value : true : they are as equal as "abc" with "Abc"

int inword(const char *p1,		// where is the string p2 contained in p1, -1 = not
		   const char *p2 );		// ignore case
// return value : index, 0... : p2 is contained inp1 as
//						  p1="DC-Language" p1="lang"

inline bool isnum(char x)
	{
	return ( x >= '0' && x <= '9' ) ? true : false ;
	}

bool	isindir(const char *pdir,
			const char *pfile );

bool isdouble( const char *ps );

uint	itolower(					// make ANSI to lower
			uint c);				// this integer number 0 .. 255
uint	itoupper(					// make ANSI to upper
			uint c);				// this integer number 0 .. 255

// Conversion of internal numbers into ASCII string representation
// all subroutines start with st_...()
// all subroutines return 0 when no error detected.


// conversion of ASCII code into internal codes,
// all subroutines start with ascto...()
// all subroutines return 0 when no error detected.
// all routines modify the pointer, so it points after
// decoding behind the processed part of a string.
// Also all routines return 0 when no error detected.
//

// Convert ASCII numeric strings to binary data
int		asctouc(					// ASCII ANSI code to integer binary
			const char **ppch,		// input string
			uint8_t *where );			// where to save result

int		asctosc(					// ASCII ANSI code to integer binary
			const char **ppch,		// input string
			int8_t *where );			// where to save result
int		asctouw(					// ASCII ANSI code to integer binary
			const char **ppch,	   // input string
			uint16_t *where );			// where to save result

int		asctosw(					// ASCII ANSI code to integer binary
			const char **ppch,		// input string
			int16_t *where );			// where to save result

int		asctoul(					// ASCII ANSI code to integer binary
			const char **ppch,		 // input string
			uint32_t *where );			// where to save result

int		asctosl(					// ASCII ANSI code to integer binary
			const char **ppch,		 // input string
			int32_t *where );			// where to save result

int		asctofl(					// ASCII ANSI code to double
			const char **ppch,		// input string
			float *where );		// where to save result

int		asctod(						// ASCII ANSI code to double
			const char **ppch,		// input string
			double *where );		// where to save result

int		asctobool(					// ASCII ANSI code "0" or "1" to bool
			const char **ppch,		// input string
			bool *where );			// where to save result

int		asctodat(					// ASCII ANSI code to daycount() number
			const char **ppch,		// input string
			uint32_t *where );			// where to save result

/* See file hegettim.h :
int		asctotim(					// ASCII ANSI code to daycount() number
			const char **ppch,		// input string
			CEpoch *where );		 // where to save result
*/
/* use asctotim instead :
int		asctoepoch(					// ASCII ANSI code to uint32_t epoch value
			const char **ppch,	   // String-pointer
			uint32_t *pepoch );		// where to save the epoch value
*/
int		asctochar(					// get a single character
			const char **ppch,		// input string
			char *where );		   // where to save result

int		asctohw(					// ASCII HEX code to integer binary
			const char **ppch,		// input string
			uint16_t *where );			// where to save result

int		asctohx2(					// ASCII HEX code get exactly 2 characters
			const char **ppch,		// input string
			uint8_t *where );			// where to save result

int		asctohx4(					 // ASCII HEX code get exactly 4 characters
			const char **ppch,		 // input string
			uint16_t *where );			// where to save result

int		asctohx6(					 // ASCII HEX code get exactly 6 characters
			const char **ppch,		 // input string
			uint32_t *where );			// where to save result

int		asctohx8(					 // ASCII HEX code get exactly 8 characters
			const char **ppch,		 // input string
			uint32_t *where );			// where to save result


#if INTSIZE == LONGSIZE
inline int asctoui(
			const char **ppch,		 // input string
			uint *where )		  // where to save result
	{
	uint32_t ulb ;
	int rv ;

	rv = asctoul( ppch, &ulb );
	*where = ulb ;
	return rv ;
	}

inline int asctosi(
			const char **ppch,		 // input string
			sint *where )		  // where to save result
	{
	int32_t slb ;
	int rv ;

	rv = asctosl( ppch, &slb );
	*where = slb ;
	return rv ;
	}

#else			// INTSIZE != LONGSIZE, then 16 We have Borlands TCPP
inline int asctoui(
			const char **ppch,		 // input string
			uint *where )		  // where to save result
	{
	uint16_t uwb ;
	int rv ;

	rv = asctouw( ppch, &uwb );
	*where = uwb ;
	return rv ;
	}

inline int asctosi(
			const char **ppch,		 // input string
			sint *where )		  // where to save result
	{
	int16_t swb ;
	int rv ;

	rv = asctosw( ppch, &swb );
	*where = swb ;
	return rv ;
	}
#endif

int		asctohl(					// ASCII HEX code to integer binary
			const char **ppch,		 // input string
			uint32_t *where );			// where to save result

int		asctotok(					 // read from an ASCII string: a token, (number or name)
			const char **ppch,		 // input string
			char *where,			// where to save result
			int nmax );				 // but do not overfill result buffer

int		asctonam(					 // read from an ASCII string: a name
			const char **ppch,		 // input string
			char *where,			// where to save result
			int nmax );				 // but do not overfill result buffer

int		asctostr(					 // ASCII code delimited word (token)
			const char **ppch,		 // input string
			char *where,			// where to save result
			int nmax );				 // but do not overfill result buffer
// limit is anything, not same type of character

int		asctostb(					 // ASCII code delimited basic token
			const char **ppch,		 // input string
			char *where,			// where to save result
			int nmax );				 // but do not overfill result buffer
// limit is ','

int		asctostc(					 // ASCII code delimited basic token
			const char **ppch,		 // input string
			char *where,			// where to save result
			int nmax );				 // but do not overfill result buffer
// limit is ',' '\0'

int		asctostrquot(				 // ASCII code " delimited string to C-String
			const char **ppch,		 // input string
			char *where,			// where to save result
			int nmax );				 // but do not overfill result buffer
// limit is anything, not same type of character


int		asctowrd(					 // ASCII code delimited word (token)
			const char **ppch,		 // input string
			char *where,			// where to save result
			int nmax );				 // but do not overfill result buffer
// limit is blank, tab or ,

int		asctofilename(				// gets a file name, copies string
					// as long found a-z A-Z . : \ / _
					// finishes without error when found " > null , blank tab
			const char **ppch,		 // input string
			char *where,			// where to save result
			int nmax );				 // but do not overfill result buffer

int txtcpy( char *pdest, uint maxsize, const char *psrc );	// savecpy strcpy
int txtcat( char *pdest, uint maxsize, const char *psrc );	// savecat strcat


// sending voltages to an interface board
void dac( int channel, int16_t value );	// output to dac card , value -32767..32767
int16_t mvolt_to_dac( int channel, int16_t mvolt ); // convert millivolts to dac value


// Reading vltages from an interface board
int16_t adc( int channel );
int adcopen( void );
int16_t adc_to_mvolt( int channel, int16_t rawvalue );	// precalculate
int16_t mvolt_to_adc( int channel, int16_t millivoltvalue );
		// convert millivolts to adc value, used f.e. for trigger values
int nadc(void );			 // count of adc converters in system


int AdcCalibRead( const char *fn );		// read calib file
int AdcCalibWrite( const char *fn );		// write Calib file
int AdcCalib( const uint channel );

// routines for conversion, depending on locale settings
// (German Windows95: Ländereinstellungen)

void getlocal( void );
void getlocal_default( void );

int loctolistelement(	// locale definition depend ASCII to list element (until separator ;/, or '\n' or 0)
			const char **ppc,		// input string
			char *where,			// where to save result
			int nmax );				 // but do not overfill result buffer


bool	cisalpha( char what );	   // true if ANSI-Alphabetical


int		st2toh(						// Convert ASCII HEX string, 2 digits
			char   **pptr );	   // Point to Pointer of "E5"



// Converions of binary data into basic data types
// opposite of FUNCT int Cst::binlow( uint32_t z )
// _low will tell, that the lower byte to be saved in lower string index
int bintoul_low( const char **plst ,		// String-pointer
					uint32_t *ptr_ul ) ;				// where to save result

int bintouw_low( const char **plst ,		// String-pointer
					uint16_t *ptr_uw ) ;				// where to save result
inline int bintosw_low( const char **plst ,		// String-pointer
					int16_t *ptr_sw )				// where to save result
	{
	return bintouw_low( plst, (uint16_t *)ptr_sw ) ;
	}

inline int bintouc( const char **plst ,		// String-pointer
					uint8_t *uc )				// where to save result
	{										// opposite is binlow( (uint8_t) x );
	*uc = *(*plst)++ ;
	return 0 ;
	}

inline int bintosc( const char **plst ,		// String-pointer
					int8_t *sc )				// where to save result
	{										// opposite is binlow( (uint8_t) x );
	*sc = *(*plst)++ ;
	return 0 ;
	}

inline int bintoc( const char **plst ,	   // String-pointer
					char *c )			  // where to save result
	{										// opposite is binlow( (uint8_t) x );
	*c = *(*plst)++ ;
	return 0 ;
	}

const int EPOCHTOTXT_BUFFER_SIZE = 30 ; // recommended


inline bool older( uint32_t a, uint32_t b )
	{		// is date a older than b
	uint32_t c ;
	c = a - b ;
	if ( c > 0x80000000L )
		{
		if ( c == 3600L ) return false ;		// not older but undeterminable
		if ( c == 7200L ) return false ;		// not older but undeterminable
		return true ; // not a older than b
		}
	return false ;
	}

// read a text line from a file
#define skipwhite( p ) while ( *p == ' ' || *p == '\t' ) ++p ;

int cprntab(			// Compress to tabs in one line
	char *to ,			// Store there
	char *from ,		// Source ist there
	int tomax ,			// sizeof to
	int whytext ,		// why text and no expand
	int ntab );			// Tab - raster

int expntab(			// Expand tabs in one line
	char *to ,			// Store there
	char *from ,		// Source ist there
	int tomax ,			// sizeof to
	int whytext ,		// why text and no expand. Is ' or "
	int ntab ,
	int tx );			// 0: ignore ' or " , 1: look for ' or "


long filefind(			// find occurance of a string in a binary file
	tfh fdes,			// File handle
	char *inbuffer, // start of string to find
	int inblng,			// length of string to find
	long adr			// start from this position
	);

bool regexp(const char *pstring, const char *ppattern);
					  // check if string "mnjaf" matches pattern "*a?"

void trm_emulate( uint8_t receivecode );	// Terminal emulation
void trmansi( char c );		   // ansi terminal emulation
int cvstring( char *pchw, char *pchr, int maxlen );
		// Interpret String descriptors as \xx ...

// banking
double diraba_fee( uint32_t date, double volume );

// date / time conversion
void	todate(
		uint *pyear,
		uint *pmonth,
		uint *pday,				   // where to save day
		uint32_t datevalue );
uint32_t daycount( uint year, uint month, uint day);
extern uint montab[2][13] ;


const uint32_t DAYCOUNT_1970_1_1 = 719528L;

bool leapyear( int year );		 // if year is a leap year


#ifdef MSVCXX
#define bufcpy(d,s,l) memcpy(d,s,l)
#endif

#ifndef bufcpy
void bufcpy(void *dest, void *src, uint n );
#endif

bool bufequ(void *dest, void *src, uint n);

void *getsharedmemory( uint size ); // sharemem.c
void *getsharedmemory( const char *modulname, uint size ); // sharemem.c
void *linksharedmemory( const char *modulname ); // sharemem.c
void freesharedmemory(const char *modulname,void *prv);

int fnamext(						// Make similar file name with other .ext
	char *pnewname ,				// save it here
	uint len ,						// Buffer size
	const char *poldname ,			// old name
	const char *pextension );		// extension to be set

int fnambak(						// Make same file name, without path, but other extension
	char *pnewname ,				// save it here
	uint len ,						// Buffer size
	const char *poldname ,			// old name
	const char *pextension );		// extension to be set



int addext(						// Add extension to a file name
	char *pdestination ,		   // buffer
	uint len ,						// Buffer size
	char *ext );					   // f.e. ".cal"


// Check if a file name means ASCII, Binary or Temporary
bool isansifile( const char *fn );
bool isbinfile( const char *fn );
bool isnofile( const char *fn );	// Temoprary

// create a file name from any string, extract a-z, 0-9
int mkfilename(char *pdest,		   // save generated name here
				int ndest,			// maximum space
				const char *psource );

int cmpbinfile( const char *fn1, const char *fn2);

int copyfile( const char *name_from, const char *name_to );


int getpairs( const char *filename, uint nmaxval, double *pvector1, double *pvector2 );
// reads pairs x= y= from an ASCII file
// 16.300,-17.2
// 2,4.556	; Comment



bool filetyp(						 // check if a given
		const char *filename,		// filename means a
		char const *type );			// special extension

void lpcreini(
		uint8_t HEFAR *pbasis
	);


FILE *fopenw( char *fn );			// may modify char fn[NAME_MAX]


int	  mkdir_forfile( const char *pfn );	   // create all directory tree
											// neccesary to save this file


char *urheber( void );			   // who prgrammed this application

int	  checkurht( void ) ;		 //

void  beep( uint32_t freq = 1000L, uint32_t duration = 500L );	// make noise

#ifndef DEF_ASK				// not the definition of uprintf follows
extern "C" {
char ask( const char *answers,	   // List "yn" or "ynaq"
		const char *fmt, ... );
}
#endif	// PRINTF


// uprintf reports an error
// extern bool flag_uprintf_quit ;	// and sets this flag if answered 'Q'

// ask_uprintf() answerelement is given to call as 1. parameter
#define ANSWER_ELEMENT_ABORTRETRYIGNORE 1
#define ANSWER_ELEMENT_OK 2
#define ANSWER_ELEMENT_OKCANCEL 3
#define ANSWER_ELEMENT_RETRYCANCEL 4
#define ANSWER_ELEMENT_YESNO 5
#define ANSWER_ELEMENT_YESNOCANCEL 6

// The answertype is returned and put into	 i_uprintf_answer = myrv ;
extern int i_uprintf_answer ;
extern bool uprintf_wait_for_answer ;	// wait or not to wait for answer
										// block or do not block
#define ANSWERTYPE_OK 1
#define ANSWERTYPE_CANCEL 2
#define ANSWERTYPE_RETRY 3
#define ANSWERTYPE_YES 4
#define ANSWERTYPE_NO 5
#define ANSWERTYPE_ABORT 6
#define ANSWERTYPE_IGNORE 7

#ifndef DEF_UPRINTF				// not the definition of uprintf follows
extern "C" {
void uprintf( const char *fmt, ... );	// Display a message
										// which user has to confirm
										// User data error

int ask_uprintf( int answertype, const char *fmt, ... );   // Display a message
										// which user has to confirm by an answer (yes no ok, retry ... )
}
#endif	// PRINTF

#ifndef DEF_NSPRINTF				// not the definition of uprintf follows
extern "C" {
void nsprintf( char *dest, int ndest, const char *fmt, ... );
}
#endif	// PRINTF
#ifndef DEF_LOGVTPRINTF				// not the definition of uprintf follows
extern "C" {
void logvtprintf(char const *fmt, ... );
}
#endif	// PRINTF

extern void logvthex8( uint32_t x );
extern void logvthex4( uint16_t x );
extern void logvthex2( uint8_t x );
extern void logvtdump( const char *ptr, const uint size,
			const char *key, const uint32_t add=0);

extern void logvtocha( char x );
extern void logvtochavisible( uint8_t cDisplay );
extern void logvtputs( const char *px );
extern void logvtmsec( void );
extern void logvtmsecval( uint32_t msec ); // as 12:34:45.060
extern void logvtcompil( void );			// Displays used compiler




enum ELineType {			// for ComDirection()
	E_UNKNOWN ,
	E_RECV ,
	E_SEND ,
	E_RECV2 ,
	E_SEND2 ,
	E_DIAL ,
	E_MSG ,
	E_ERR ,
	E_KEY ,
	E_SER0 ,
	E_SER1 } ;

extern enum ELineType last_reason_for_logging ;
extern bool atlinepos1 ;

// logvtPrint the direction, if it has changed
void ComDirection( enum ELineType n );			// 1 = from PC ti COM1, 2 = from COM1 to PC





#ifdef DOSGNU
int int21( union REGS *preg );			 // The DOS interrupt
uint16_t int21ax( uint16_t ax );			 // The DOS interrupt faster
#endif

#ifdef BORWIN
#include <dos.h>			 // union REGS
int int21( union REGS *preg );			 // The DOS interrupt
#endif

#ifdef MDOS10
int int21( union REGS *preg );			 // The DOS interrupt
#endif

#ifdef MSVC10
// int int21( union REGS *preg );		 // The DOS interrupt
// int int10( union REGS *preg );		 // The DOS interrupt
#endif

// #ifndef __hevt_h__		// disturbes uprintf
// #include <hevt.h>
// #endif

#include "hedeb.h"
#include "hegr.h"
#include "heheap.h"

#ifdef MDOS40

struct DWORDREGS {
  unsigned long edi;
  unsigned long esi;
  unsigned long ebp;
  unsigned long cflag;
  unsigned long ebx;
  unsigned long edx;
  unsigned long ecx;
  unsigned long eax;
  unsigned short eflags;
};

struct WORDREGS {
  unsigned short di, _upper_di;
  unsigned short si, _upper_si;
  unsigned short bp, _upper_bp;
  unsigned short cflag, _upper_cflag;
  unsigned short bx, _upper_bx;
  unsigned short dx, _upper_dx;
  unsigned short cx, _upper_cx;
  unsigned short ax, _upper_ax;
  unsigned short flags;
};

struct BYTEREGS {
  unsigned short di, _upper_di;
  unsigned short si, _upper_si;
  unsigned short bp, _upper_bp;
  unsigned long cflag;
  unsigned char bl;
  unsigned char bh;
  unsigned short _upper_bx;
  unsigned char dl;
  unsigned char dh;
  unsigned short _upper_dx;
  unsigned char cl;
  unsigned char ch;
  unsigned short _upper_cx;
  unsigned char al;
  unsigned char ah;
  unsigned short _upper_ax;
  unsigned short flags;
};

union REGS {		/* Compatible with DPMI structure, except cflag */
  struct DWORDREGS d;
  struct WORDREGS x;
//	struct WORDREGS w;
  struct BYTEREGS h;
};


#endif		// MDOS40

#ifdef __TURBOC__
#define mkdir(a,b) mkdir(a)	   // no mode parameter for DOS / Win3.11

#include <dos.h>

extern int int10( union REGS *preg );
extern int intx( int x,	 union REGS *preg );
extern int int21( union REGS *preg );
uint16_t int21ax( uint16_t ax );			   // The DOS interrupt faster
#endif

//
// conversion from DOS CP437 to ANSI
// used for display as well as for data storage in DOS compatible formats
// as for exemple DBASE III (see: class Cdbf in hedbf.h)
//
// But usable after
//
// if ( !chartablefilled ) fillchartable() ;
extern void fillchartable( void );
extern bool chartablefilled ;
extern uint8_t charintable[256] ; // IBM-CODE CP437 to ANSI
extern uint8_t charouttable[256] ;

// Get a single lins of a AK control file
extern int GetXoneAKCommand(
	const char *txFilename,		// The files name
	uint uiX,					// The requested macro number
	char *pSendBuffer,			// save send string here (0 terminated)
	uint uiSizeSendBuffer,		// maximum size
	char *pComment,				// Save the comment here
	uint uiComment				// Size limit
	);

#define ansi_to_cp437(a) charouttable[ (uint8_t)(a) ]
#define cp437_to_ansi(a) charintable[ (uint8_t)(a) ]


typedef int dotree_callback( const char *filename );

int dotree(					// rv == 0 : no error
	const char *subdir, // "." or "/home/cc"
	const char *pattern,	// "*.cpp;a*.h;makefile"
	dotree_callback *cbf,
	bool opt_r,				// recursive
	bool opt_f,				// perform also for files
	bool opt_d,				// perform call also for subdirs,
	uint32_t epoch_from,		// if zero, then no from time
	uint32_t epoch_to );		// if zero, then no to time



#if defined( LINUX ) || defined( DOSGNU )
#include <sys/stat.h>	// prototype of mkdir()
#endif


#ifndef AVR
inline int		normmkdir( const char *name )
	{

#ifdef OSK
/*
mode octal		Attributes created		rv
0003			dsewrewr				6291459
0203			d--w----				6291587
0222			dsewrewr				6291602
0300			dsewrewr				6291648 rv=030000300
0303			d--w--wr				-1		rv=037777777777
0103			d-----wr				rv=6291523 rv=030000103
0302			ds----wr				rv=6291650 rv=030000302
0301			ds----w-				rv=6291649 rv=030000301
Attribute bits : 0200 : -s------
*/
//		return system( name );				// OSK - System-Call
		return makdir( name, 0003, 0003 );	// OSK - Call
#elif defined( MDOS10 ) || defined( MSVC10 )
		return _mkdir( name );
#else
		return mkdir( name, 0750);
#endif
	}

inline int normrename( const char *oldname, const char *newname )
	{
	return rename( oldname, newname );
	}

inline int normunlink( const char *filename )
	{
	return unlink( filename );
	}
#endif

// note : Assembly requires operands as	  mov  source, destination
// while disassembly shows in reversed order mov destination, source
#ifdef DOSGNU
/*
inline uint8_t inportb(uint16_t port)
{
portaddresse = (uint32_t)port ;
asm( "mov _portaddresse, %dx\n" );
asm( ".byte 0xec\n" );					// in %al, %dx\n
asm( "mov %al, _iobyte\n" );
// DBprintf(( "gotten %XH from port %XH", iobyte, portaddresse ));
return (uint8_t)iobyte ;
}
*/
#endif		// DOSGNU

#if defined( MSVC40 ) || defined( MDOS40 ) || defined( MSVC10 ) || defined( MDOS10 )
inline void disable()	  // Borlands Turbo CPP for DOS, has this built in
{

__asm
	{
	cli		// clear interrupt
	}
}

inline void enable()
{

__asm
	{
	sti		// Set	interrupt
	}
}

inline uint8_t inportb( uint16_t portnumber )
{
uint8_t rv ;
__asm
	{
	mov dx, portnumber
	in	al, dx
	mov rv , al
	}
return rv ;
}


inline void outportb( uint16_t portnumber, uint8_t bdata )
{
__asm
	{
	mov dx, portnumber
	mov al, bdata
	out	 dx, al
	}
return ;
}


inline uint16_t inport( uint16_t portnumber )
{
uint16_t rv ;
__asm
	{
	mov dx, portnumber
	in	ax, dx
	mov rv , ax
	}
return rv ;
}


inline void outport( uint16_t portnumber, uint16_t bdata )
{
__asm
	{
	mov dx, portnumber
	mov ax, bdata
	out	 dx, ax
	}
return ;
}

#endif		// #if defined( MSVC40 ) || defined( MDOS40 )

#endif		// __helq_h__
