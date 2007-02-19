#define DB_ENABLED 0
#define DB_INFO 0

// daycount, calculates the day number of a date.
//
// The opposit function is todate()
//
// build and run ../qq/qqdaycnt to verify this procedure
//
// This daycount returns :

#include <heport.h>
#include <hedeb.h>
#include <helq.h>

/*****************************************************************************/
uint montab[2][13] = {
		{ 0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 } ,
		{ 0, 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 }
																} ;

FUNCT uint32_t daycount(	 // calculate days number from given date
uint year ,				  // year
uint m ,			   // month 1 - 12
uint t )			   // day
{
uint32_t lx ;
uint32_t lj ;
uint32_t jbase ;
uint sj ;			   // leap year
uint add4 ;			   // corrections
uint sub100 ;
uint add400 ;
uint mon1 ;
uint imon ;
uint tsum ;

DBent("daycount");
if (			 year < 70 ) year += 2000 ;		  // after 2000
if ( year >= 70 && year <= 200 ) year += 1900 ;
sj = leapyear(year) ;					   // is this a leap year ?
/*
		   1		1		1
365	   + _____	- _____ + _____
		   4	   100	   400

 365 * 400		  100		4		1
___________	   + _____	- _____ + _____
	400			  400	   400	   400

*/
lj = year ;			   // the year number

DBprintf(( "year=%d, sj=%d", year, sj ));
--year ;
add4   = year / 4	;
sub100 = year / 100 ;
add400 = year / 400 ;

mon1 = 0 ;
if ( m >= 13 ) m = 12 ;
for ( imon = 1 ; imon < m ; ++imon )
		{
		mon1 += montab[sj][imon] ;
		}

DBprintf(( "m=%d, mon1=%d", m ,mon1 ));
DBprintf(( "add4=%d sub100=%d add400=%d", add4, sub100, add400 ));
tsum = add4 - sub100  + add400 ;
jbase = tsum + lj * 365L ;
lx = jbase + mon1 + t ;

DBprintf(( "lx=%lu jbase=%lu, mon1=%d t=%d", lx, jbase, mon1, t ));
DBret("daycount");
return lx ;
}

