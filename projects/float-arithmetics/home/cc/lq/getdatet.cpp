//
// This module contains :
// getutcdatetime : get date and time in UTC (GMT+0) representation
// getlocaldatetime
//

#include <string.h>
#include <heport.h>
#include <helq.h>
#include <hegettim.h>

#if defined( MSVC10 ) || defined( MSVC40 ) || defined( MDOS40 )
#include <mapidefs.h>	// SYSTEMTIME ...
#endif


#ifdef _MFC
// get UTC (GMT) time
void getutcdatetime( struct sdate *pdate, struct sday *pday )
	{
	class Ctime now ;
	Ctime.theTime() ;		 // gets current date and time
	pdate->year	 = Ctime.GetYear();
	pdate->month = Ctime.GetMonth();
	pdate->day	 = Ctime.GetDay();
	pdate->weekday = Ctime.GetDayOfWeek() - 1 ;
	pday->hour	= Ctime.GetHour();
	pday->minute = Ctime.GetMinute();
	pday->second = Ctime.GetSecond();
	pday->msecond = 0 ;
	}
#endif		  // _MFC

#if defined( MSVC10 ) || defined( MSVC40 ) || defined( MDOS40 )

void getutcdatetime( struct sdate *pdate, struct sday *pday )
	{
	SYSTEMTIME st ;

	GetSystemTime( &st );
	pdate->year	   = st.wYear ;
	pdate->month   = (uint8_t)st.wMonth ;
	pdate->day		 = (uint8_t)st.wDay ;
	pdate->weekday = (uint8_t)st.wDayOfWeek ;

	pday->hour		 = (uint8_t)st.wHour ;
	pday->minute   = (uint8_t)st.wMinute	  ;
	pday->second   = (uint8_t)st.wSecond ;
	pday->msecond  = st.wMilliseconds ;
	}
#endif



#ifdef DOSGNU

#include <dos.h>

FUNCT void getutcdatetime( struct sdate *pdate, struct sday *pday )
	{
	union REGS inregs ;

	inregs.x.ax = 0x2A00 ;	  // Get date
	int21( &inregs );
	pdate->year	   = inregs.x.cx ;
	pdate->month   = inregs.h.dh ;
	pdate->day	   = inregs.h.dl ;
	pdate->weekday = inregs.h.al ;

	inregs.x.ax = 0x2C00 ;	  // Get time
	int21( &inregs );
	pday->hour	  = inregs.h.ch ;
	pday->minute  = inregs.h.cl ;
	pday->second  = inregs.h.dh ;
	pday->msecond = inregs.h.dl * 10 ;
	}
#endif




#ifdef LINUX
#include <time.h>

void getutcdatetime( struct sdate *pdate, struct sday *pday )
	{
	time_t time_now ;
	struct tm *pdecoded ;
	uint16_t lyear ;

	time( &time_now );
	pdecoded = localtime( &time_now );
	lyear = pdecoded->tm_year ;
	if ( lyear < 70 ) lyear += 2000 ;
	else			  lyear += 1900 ;
	pdate->year = lyear ;
	pdate->month = pdecoded->tm_mon +1 ;	// tm_mon 0..11
	pdate->day	   = pdecoded->tm_mday ;
	pdate->weekday = pdecoded->tm_wday ;

	pday->hour	  = pdecoded->tm_hour ;
	pday->minute  = pdecoded->tm_min ;
	pday->second  = pdecoded->tm_sec ;
	pday->msecond = 0 ;
	}
#endif






#ifdef OSK
#include <time.h>

void getutcdatetime( struct sdate *pdate, struct sday *pday )
	{
	time_t time_now ;
	struct tm *pdecoded ;

	time( &time_now );
	pdecoded = localtime( &time_now );
	if ( pdecoded->tm_year < 200 )

		{

		pdate->year = pdecoded->tm_year + 1900 ;

		}

	else

		{

		pdate->year = pdecoded->tm_year ;

		}

	pdate->month = pdecoded->tm_mon ;
	pdate->day	   = pdecoded->tm_mday ;
	pdate->weekday = pdecoded->tm_wday ;

	pday->hour	  = pdecoded->tm_hour ;
	pday->minute  = pdecoded->tm_min ;
	pday->second  = pdecoded->tm_sec ;
	pday->msecond = 0 ;
	}
#endif





#ifdef _MFC
// get locale time
void getlocaldatetime( struct sdate *pdate, struct sday *pday )
	{
	class Ctime now ;
	Ctime.theTime() ;		 // gets current date and time
	pdate->year	 = Ctime.GetYear();
	pdate->month = Ctime.GetMonth();
	pdate->day	 = Ctime.GetDay();
	pdate->weekday = Ctime.GetDayOfWeek() - 1 ;
	pday->hour	= Ctime.GetHour();
	pday->minute = Ctime.GetMinute();
	pday->second = Ctime.GetSecond();
	pday->msecond = 0 ;
	}
#endif		  // _MFC

#if defined( MSVC10 ) || defined( MSVC40 ) || defined( MDOS40 )

FUNCT void getlocaldatetime( struct sdate *pdate, struct sday *pday )
	{
	SYSTEMTIME st ;

	GetLocalTime( &st );
	pdate->year	   = st.wYear ;
	pdate->month   = (uint8_t)st.wMonth ;
	pdate->day		 = (uint8_t)st.wDay ;
	pdate->weekday = (uint8_t)st.wDayOfWeek ;

	pday->hour		 = (uint8_t)st.wHour ;
	pday->minute   = (uint8_t)st.wMinute	  ;
	pday->second   = (uint8_t)st.wSecond ;
	pday->msecond  = st.wMilliseconds ;
	}

#endif


#if defined( MSVC10 ) || defined( MSVC40 ) || defined( MDOS40 )
// get name of time zone and get bias

FUNCT int gettzname ( const char **ppTimezonename, long *pulBias )
{
DWORD tzinfo ;
TIME_ZONE_INFORMATION stz ;
static char rs[33] ;
int i ;

*pulBias = 0 ;

tzinfo = GetTimeZoneInformation( &stz );
switch ( tzinfo )
	{
default :
case TIME_ZONE_ID_UNKNOWN:	// The operating system cannot determine the current time zone. This is usually because a previous call to the SetTimeZoneInformation function supplied only the bias (and no transition dates).
	*ppTimezonename = "unknown time zone" ;
	return 1 ;				// Error

case TIME_ZONE_ID_STANDARD: // The operating system is operating in the range covered by the StandardDate member of the structure pointed to by the lpTimeZoneInformation parameter.
	*pulBias  = (stz.Bias + stz.StandardBias)*60L ; // Minutes to seconds
	for ( i = 0 ; i < 32 ; ++i )
		{
		rs[i] = (char)stz.StandardName[i] ;
		}
	*ppTimezonename = rs ;
	return 0 ;				// O.K.

case TIME_ZONE_ID_DAYLIGHT: // The operating system is operating in the range covered by the
	*pulBias  = (stz.Bias + stz.DaylightBias)*60L ; // Minutes to seconds
	for ( i = 0 ; i < 32 ; ++i )
		{
		rs[i] = (char)stz.DaylightName[i] ;
		}
	*ppTimezonename = rs ;
	return 0 ;				// O.K.
	}
}

#endif


#if defined( LINUX )
// get name of time zone
FUNCT int gettzname ( const char **ppTimezonename, long *pulBias )
{
char *tzname ;

*pulBias = 0 ;
tzname = getenv( "tz" );
if ( tzname == NULL )
	{
	*ppTimezonename = "not set tz=" ;
	return E 12 ;
	}

if ( strncmp( tzname, "gmt", 3 ) == 0 )
	{
	*ppTimezonename = tzname ;
	return 0 ;
	}

if ( strncmp( tzname, "mez", 3 ) == 0 )
	{
	*pulBias = -3600 ;
	*ppTimezonename = tzname ;
	return 0 ;
	}
if ( strncmp( tzname, "mesz", 4 ) == 0 )
	{
	*pulBias = -7200 ;
	*ppTimezonename = tzname ;
	return 0 ;
	}
*ppTimezonename = "set tz=unknown" ;
return E 13 ;
}

#endif

#if defined( OSK )
// get name of time zone
FUNCT int gettzname ( const char **ppTimezonename, long *pulBias )
{
char *tzname ;

*pulBias = 0 ;
tzname = getenv( "tz" );
if ( tzname == NULL )
	{
	*ppTimezonename = "not set tz=" ;
	return E 12 ;
	}

if ( strncmp( tzname, "gmt", 3 ) == 0 )
	{
	*ppTimezonename = tzname ;
	return 0 ;
	}

if ( strncmp( tzname, "mez", 3 ) == 0 )
	{
	*pulBias = -3600 ;
	*ppTimezonename = tzname ;
	return 0 ;
	}
if ( strncmp( tzname, "mesz", 4 ) == 0 )
	{
	*pulBias = -7200 ;
	*ppTimezonename = tzname ;
	return 0 ;
	}
*ppTimezonename = "set tz=unknown" ;
return E 13 ;
}

#endif


#ifdef DOSGNU

#include <dos.h>

void getlocaldatetime( struct sdate *pdate, struct sday *pday )
	{
	union REGS inregs ;

	inregs.x.ax = 0x2A00 ;	  // Get date
	int21( &inregs );
	pdate->year	   = inregs.x.cx ;
	pdate->month   = inregs.h.dh ;
	pdate->day	   = inregs.h.dl ;
	pdate->weekday = inregs.h.al ;

	inregs.x.ax = 0x2C00 ;	  // Get time
	int21( &inregs );
	pday->hour	  = inregs.h.ch ;
	pday->minute  = inregs.h.cl ;
	pday->second  = inregs.h.dh ;
	pday->msecond = inregs.h.dl * 10 ;
	}
#endif


#ifdef __TURBOC__

#include <dos.h>
#include <time.h>

// get name of time zone
// extern int daylight ;	// __TURBOC__ : 1 if daylight saving time
// extern char * tzname[2] ;
// extern long timezone ;
#include <sys\types.h>

void getutcdatetime( struct sdate *pdate, struct sday *pday )
	{
	union REGS inregs ;
	time_t timeval ;

	uint yr, mo, dy, hr, mi, sc ;

	time( &timeval );

	CEpoch epochval(timeval) ;

	epochval.Todate( &yr, &mo, &dy, &hr, &mi, &sc );

	pdate->year	   = yr ;
	pdate->month   = mo ;
	pdate->day	   = dy ;
	pdate->weekday = 0 ;

	pday->hour	  = hr ;
	pday->minute  = mi ;
	pday->second  = sc ;
	pday->msecond = 0 ;
	}

void getlocaldatetime( struct sdate *pdate, struct sday *pday )
	{
	union REGS inregs ;

	inregs.x.ax = 0x2A00 ;	  // Get date
	int86( 0x21, &inregs, &inregs );
	pdate->year	   = inregs.x.cx ;
	pdate->month   = inregs.h.dh ;
	pdate->day	   = inregs.h.dl ;
	pdate->weekday = inregs.h.al ;

	inregs.x.ax = 0x2C00 ;	  // Get time
	int86( 0x21, &inregs , &inregs );
	pday->hour	  = inregs.h.ch ;
	pday->minute  = inregs.h.cl ;
	pday->second  = inregs.h.dh ;
	pday->msecond = inregs.h.dl * 10 ;
	}


FUNCT int gettzname ( const char **ppTimezonename, long *pulBias ) // __TURBOC__
{

tzset();

*pulBias = timezone ;
if ( daylight == 0 || daylight == 1 )	// Standard time / daylight saving
	{
	*ppTimezonename = tzname[daylight] ;
	return 0 ;
	}
else
	{
	*ppTimezonename = "unknown time zone" ;
	return 1 ;		// An Error occured (somehow)
	}
}
#endif



#ifdef LINUX
#include <time.h>

void getlocaldatetime( struct sdate *pdate, struct sday *pday )
	{
	time_t time_now ;
	struct tm *pdecoded ;
	uint16_t lyear ;

	time( &time_now );
	pdecoded = localtime( &time_now );
	lyear = pdecoded->tm_year ;
	if ( lyear < 70 ) lyear += 2000 ;
	else			  lyear += 1900 ;
	pdate->year = lyear ;
	pdate->month = pdecoded->tm_mon +1 ;	// tm_mon 0..11
	pdate->day	   = pdecoded->tm_mday ;
	pdate->weekday = pdecoded->tm_wday ;

	pday->hour	  = pdecoded->tm_hour ;
	pday->minute  = pdecoded->tm_min ;
	pday->second  = pdecoded->tm_sec ;
	pday->msecond = 0 ;
	}
#endif


#ifdef OSK
#include <time.h>

void getlocaldatetime( struct sdate *pdate, struct sday *pday )
	{
	time_t time_now ;
	struct tm *pdecoded ;

	time( &time_now );
	pdecoded = localtime( &time_now );
	pdate->year = pdecoded->tm_year ;
	pdate->month = pdecoded->tm_mon ;
	pdate->day	   = pdecoded->tm_mday ;
	pdate->weekday = pdecoded->tm_wday ;

	pday->hour	  = pdecoded->tm_hour ;
	pday->minute  = pdecoded->tm_min ;
	pday->second  = pdecoded->tm_sec ;
	pday->msecond = 0 ;
	}
#endif

