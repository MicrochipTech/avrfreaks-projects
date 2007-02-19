#ifndef __hegettim_h__
#define __hegettim_h__

#ifdef OSK
#ifndef CLK_TCK // is defined in /dd/DEFS/time.h
#include <time.h>
#endif
#endif


#ifdef DOSGNU

#ifndef __dj_include_Inline_pc_h_
#include <inlines/pc.h>
#endif // #ifndef __dj_include_Inline_pc_h_

#ifndef __dj_include_sys_farptr_h_
#include <sys/farptr.h>
#endif // #ifndef __dj_include_sys_farptr_h_

#endif



#define EPOCH_INVALID 0L

class CEpoch
	{
public:
	uint32_t ulE ; // The epoch value : Always UTC since 1.1.1970
public:

	operator long()					// convert the value to long
		{
		return (long)ulE ;
		}

	long Diff( CEpoch epochStart ) const	// Returns seconds since event
		{
		return ulE - epochStart.ulE ;
		} ;

	void Add( uint32_t ulSec )		// Adds seconds to Events time
		{
		ulE += ulSec ;
		} ;

	void Sub( uint32_t ulSec )		// Subtracts seconds from Events time
		{
		ulE -= ulSec ;
		} ;

	uint32_t Time( void )				// only the time of the Day
		{
		return ulE % 86400L ;
		}

	bool Isintime( CEpoch from, CEpoch to );	// See file epoisint.cpp

	bool IsAfter( CEpoch from );	// See file epoisaft.cpp

	bool IsBefore( CEpoch from );	// See file epoisbef.cpp

	void GetActual( void );			// See file epochget.cpp

	CEpoch( void ) {; }				// Standard Constructor

	CEpoch( uint32_t val )
#ifdef __TURBOC__
;
#else
	{ ulE = val; }				// Number initalize
#endif

	CEpoch(uint yr,uint mn,uint dy,uint hr,uint mi,uint sc );
									// See file epochepo.cpp

	void Todate( uint *pyr, uint *pmth, uint *pday, uint *phur, uint *pmin, uint *psec) const ;
									// See file etodate.cpp

	void Txt(				// convert epoch value to date text as "2002/12/31 14:56:55", size=20Bytes
		char *dpDest,
		int maxlen ) const ;
									// See file epochtxt.cpp

	} ;



// get set a file date, where the file name is given
CEpoch getfndat(const char *files_name );
void setfndat(const char *files_name, CEpoch epoch);

// get set a file date of an open file
CEpoch getfdate(tfh fd);
void setfdate(tfh fd, CEpoch epoch);

int		asctotim(					// ASCII ANSI code to daycount() number
			const char **ppch,		// input string
			CEpoch *where );		 // where to save result


struct sdate {
	uint year ;
	uint month ;
	uint day ;
	uint weekday ;	   // 0 = Sunday ...
	} ;
struct sday {
	uint hour ;
	uint minute ;
	uint second ;
	uint msecond ;	   // 1/1000 of seconds
	} ;


struct sdate_uc {
	uint32_t day ;
	uint8_t hour ;
	uint8_t minute ;
	uint8_t second ;
	} ;

extern void tsdaytim(void);
extern void tsdaytim_open(void);


#if defined( __TURBOC__ ) || defined( DOSGNU )
struct stickuchars {
	uint8_t ucl ;
	uint8_t uch ;
	uint16_t uwh ; } ;
struct stickuwords {
	uint16_t uwl ;
	uint16_t uwh ; } ;
struct stickulongs {
	uint32_t ull ;
	} ;

union uticks {
	struct stickulongs ul ;
	struct stickuwords uw ;
	struct stickuchars uc ;
		} ;


#if defined( __TURBOC__ )
uint16_t far * const tick_in_dos_page_46 = (uint16_t far *)0x0000046C ;
uint16_t far * const hitick_in_dos_page_46 = (uint16_t far *)0x0000046E ;

#define ul_mod_tick_in_dos_page_46	(*(uint32_t far *)0x0000046C)
#endif
// ... continued #if defined( __TURBOC__ ) || defined( DOSGNU )

static union uticks pre ;
static union uticks post ;
static uint16_t olddistick ;

FUNCT inline void tickopen( void )		// if defined( __TURBOC__ ) || defined( DOSGNU )
{
disable();
outportb( 0x43, 0x34 );
// 0x34 = 0b00110100
//			00		 Select Counter 0
//			  11	 Reload timer lsb msb
//				010	 Mode 2 Rate Generator
//				   0 Binary, not BCD
outportb( 0x40, 255 );	// Low Timer Value
outportb( 0x40, 255 );	// High Timer Value
enable();
}


// Warning : After Midnight the cell 46c will be reset to 0 !
// this is not handled in this program
FUNCT inline uint32_t gettick( void )		// if defined( __TURBOC__ ) || defined( DOSGNU )
{

disable();			// Interrupt off

#if defined( __TURBOC__ )
pre.uw.uwh = *tick_in_dos_page_46 ;
#else
pre.uw.uwh = _farnspeekw( 0x0000046C ) ;
#endif
outp( 0x43, 0 );	// Latch counter
pre.uc.ucl = inportb( 0x40 );
pre.uc.uch = inportb( 0x40 );
enable();			// interrupt on

#if defined( __TURBOC__ )
post.uw.uwh = *tick_in_dos_page_46 ;
#else	// DOSGNU = DJGPP
post.uw.uwh = _farnspeekw( 0x0000046C ) ;
#endif

if ( pre.uw.uwh != post.uw.uwh )
	{
	post.uw.uwl = 0 ;
	return post.ul.ull ;
	}
pre.uw.uwl = ~pre.uw.uwl ;
return pre.ul.ull ;
}

						// if defined( __TURBOC__ ) || defined( DOSGNU )
#define getwordtick( result )				\
outp( 0x43, 0 );							\
((uint8_t *)&result)[0] = inportb( 0x40 );	\
((uint8_t *)&result)[1] = inportb( 0x40 );

// ... continued #if defined( __TURBOC__ ) || defined( DOSGNU )


#if defined( __TURBOC__ )

#define getdistick( result )					\
outp( 0x43, 0 );								\
((uint8_t *)&result)[0] = ~inportb( 0x40 );		\
((uint8_t *)&result)[1] = ~inportb( 0x40 );		\
if ( ((uint16_t *)&result)[0] < olddistick )		\
	{											\
	if ( ++*tick_in_dos_page_46 == 0 )			\
		{										\
		++*hitick_in_dos_page_46 ;				\
		}										\
	}											\
olddistick = ((uint16_t *)&result)[0] ;			\
((uint16_t *)&result)[1] = *tick_in_dos_page_46 ;

#else			// DOSGNU

#define getdistick( result )					\
outp( 0x43, 0 );								\
((uint8_t *)&result)[0] = ~inportb( 0x40 );		\
((uint8_t *)&result)[1] = ~inportb( 0x40 );		\
if ( ((uint16_t *)&result)[0] < olddistick )		\
	{											\
	short tck ;									\
	tck = _farnspeekw( 0x0000046C )+1 ;			\
	_farnspokew( 0x0000046C, tck  ) ;			\
	if ( tck == 0 )								\
		{										\
		tck = _farnspeekw( 0x0000046C+2 )+1 ;	\
		_farnspokew( 0x0000046C+2, tck	) ;		\
		}										\
	}											\
olddistick = ((uint16_t *)&result)[0] ;			\
((uint16_t *)&result)[1] = _farnspeekw( 0x0000046C ) ;


#endif

// ... continued #if defined( __TURBOC__ ) || defined( DOSGNU )

/*
FUNCT inline uint32_t difftick( uint32_t start, uint32_t act )
{
return act - start ;
}
*/
/*
FUNCT inline uint32_t difftick( uint32_t start )
{
return gettick() - start ;
}
*/

				// The macro is faster than the inline
#define difftick( start, actual ) ( (actual)-(start) )
#define diffdistick( start, actual ) ( (actual)-(start) )
#define diffwordtick( start, actual ) ( (actual)-(start) )
#define addtick( start, plus ) ( (start)+(plus) )

#define TICKPERMSEC 1192.5	// 4.77 Mhz / 4

FUNCT inline double ticktomsec( uint32_t tick )
{
return (double)tick / TICKPERMSEC ;
}

FUNCT inline uint32_t msectotick( double msec )
{
return (uint32_t) ( msec * TICKPERMSEC );
}

#define tickpersec 1192500.0	// 4.77 Mhz / 4

FUNCT inline double ticktosec( uint32_t tick )
{
return (double)tick / tickpersec ;
}

FUNCT inline uint32_t sectotick( double sec )
{
return (uint32_t) ( sec * tickpersec );
}

#endif		// if defined( __TURBOC__ ) || defined( DOSGNU )




#if defined( MSVC40 ) || defined( MDOS40 )

// There is no possibility to get the DOS tick counter from 463
// So use the getmsec instead tick
// If you need more accurate timers, then use DOS6.22
// TurboC++(Version 1.0)

#define getdistick(x) x = getmsec();
#define diffdistick diffmsec


#define gettick getmsec
#define difftick diffmsec
#define addtick addmsec
#define ticktomsec(x) (double)(x)
#define msectotick(x) (uint32_t)(x)
#define ticktosec(x) ((double)(x)/1000.0)
#define sectotick(x) (uint32_t)((x)*1000.0)
const double tickpersec = 1000.0 ;
#define TICKPERSEC 1000
#define TICKPERMSEC 1
FUNCT inline void tickopen( void )
	{;
	}
#endif


#if defined LINUX

#define tickopen()
#define ticktomsec(x) (double)(x)
#define getwordtick(r) r=(uint16_t)getmsec()
#define diffwordtick(s,a) (uint16_t)diffmsec(s,a)
#define gettick getmsec
#define difftick diffmsec
#define addtick addmsec
#define ticktomsec(x) (double)(x)
#define msectotick(x) (uint32_t)(x)
#define ticktosec(x) ((double)(x)/1000.0)
#define sectotick(x) (uint32_t)((x)*1000.0)
const double tickpersec = 1000.0 ;
#define TICKPERSEC 1000
#endif



#ifdef AVR
#define tickpersec ((double)(F_CPU)/8.0)
#define TICKPERSEC (F_CPU/8L)
#define TICKPERMSEC (F_CPU/8000L)

extern uint32_t gettick(void);
extern void tickopen(void);
				// The macro is faster than the inline
#define difftick( start, actual ) ( (actual)-(start) )
#define diffdistick( start, actual ) ( (actual)-(start) )
#define diffwordtick( start, actual ) ( (actual)-(start) )
#define addtick( start, plus ) ( (start)+(plus) )

#define sectotick( secs ) (uint32_t)( (secs) * (float)TICKPERSEC )
#define ul_msectotick( msecs ) (uint32_t)( (msecs) * TICKPERMSEC )

#endif


// Millisecond timer
uint32_t getmsec(void) ;				// gets milliseconds ... somehow
uint32_t diffmsec( uint32_t start ) ;				// gets difference since last call
uint32_t diffmsec( uint32_t start,uint32_t actmsec) ; // gets difference since last call
uint32_t addmsec( uint32_t start, uint32_t plus);	 // adds even correct after midnight

void dlmsec(uint32_t milliseconds);
	// converstoonmsef to text see Cst::msecst() file cstmsec.cpp
// Microsecond timer
uint32_t getusec(void) ;				// gets micro seconds ... somehow
uint32_t diffusec( uint32_t start ) ;				// gets difference since last call
uint32_t diffusec( uint32_t start,uint32_t actusec) ; // gets difference since last call
uint32_t addusec( uint32_t start, uint32_t plus);	 // adds even correct after midnight

uint32_t getsecmn(void) ;				// gets seconds after midnight ... somehow


// raw maschine ticks, for PC is 4.77 MHz / 4 = 1192500 Hz
// Only for DOS machines
uint16_t getticks_disabled( void ) ;	// call this only when interrupt is disabled

// nanoseconds delay
void dlnsec(uint32_t nanoseconds);
void caldlnsec( bool bReport = false );


// get date and clock time in UTC (GMT) time
void getutcdatetime( struct sdate *pdate, struct sday *pday );

// get date and clock time in local time
void getlocaldatetime( struct sdate *pdate, struct sday *pday );

// get time zome information
int gettzname ( const char **ppTimezonename, long *pulBias );

#define getenatick(x) x=gettick()
#endif // #if defined( _hegettim_h__ )

