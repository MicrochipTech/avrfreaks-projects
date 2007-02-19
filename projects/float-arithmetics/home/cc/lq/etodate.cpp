#define DB_ENABLED 0
#define DB_INFO 0
// setfdate.cpp :
// Handle date and time stamp of files

#ifdef LINUX
#include <time.h>
#endif

#include <heport.h>
#include <helang.h>
#include <hedeb.h>
#include <helq.h>
#include <hegettim.h>

FUNCT void CEpoch::Todate(
uint *py,	   // Year
uint *pm,	   // Month
uint *pd,	   // Day
uint *ph,	   // Hour
uint *pmi,	   // Minute
uint *ps)		// Second
			const
{
#if 0 /* ifdef LINUX */
time_t xt ;		// can be a float !
struct tm *p ;

xt = ulE ;
p = localtime( &xt );	// This does not work epoch 0x7FFFFFFFL	 is 2038-01-19/03:14:07
					// epoch 0x80000000L  is 1901-12-13/20:15:32

*py = p->tm_year+1900 ;
*pm = p->tm_mon+1 ;
*pd = p->tm_mday ;
*ph = p->tm_hour ;
*pmi = p->tm_min ;
*ps = p->tm_sec ;
return ;
#define DEFINED
#endif

#ifndef DEFINED
uint32_t ldatum ;
uint32_t lzeit ;
uint minsec ;

DBent("etodate");
ldatum	= ulE / 86400L ;	  // The day number
lzeit	= ulE % 86400L ;	  // time in Day

*ph		= (uint)(lzeit / 3600L) ;
minsec	= (uint)(lzeit % 3600L ) ;
*pmi	= minsec / 60 ;
*ps		= minsec % 60 ;

ldatum += DAYCOUNT_1970_1_1 ;	  /* the daycount(1970,1,1) */


todate( py, pm, pd, ldatum );

DBprintf(( "ldatum=%ld lzeit=%ld", ldatum, lzeit ));
DBprintf(( "%d-%d-%d", *py, *pm, *pd ));
DBprintf(( "%d:%d:%d", *ph, *pmi, *ps ));
DBret("etodate");
return ;
#endif
}

