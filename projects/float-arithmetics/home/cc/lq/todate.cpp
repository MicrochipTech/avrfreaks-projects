#define DB_ENABLED 0
#define DB_INFO 0

// #define TEST
#include <heport.h>
#include <helq.h>


FUNCT void todate(
uint *pj ,	   // save result, year, here
uint *pm ,	   // save result, month, here
uint *pt ,	   // save result, day, here
uint32_t tz )		// days integer number
{
uint32_t l1 ;
uint32_t trest ;
uint32_t tbas ;
uint16_t j ;
uint16_t sj ;
uint imon ;

DBent("todate");
if ( tz < 365000L ) tz = 365000L ;
l1 = tz * 400L ;
#if DB_INFO
trest  = l1 % ( 365*400L + 100L - 4L + 1L ) ;
#endif
l1	  = ( l1-1) /		( 365*400L + 100L - 4L + 1L ) ;
DBprintf(( "tz=%lu l1=%lu\n", tz, l1 ));

j = (int)l1 ;
*pj = j ;

sj = leapyear( j ) ;

tbas = daycount( j, 1, 1 )	;
while ( tz < tbas )
	{
	--j ;
	*pj = j ;

	sj = leapyear( j ) ;

	tbas = daycount( j, 1, 1 )	;
	}
trest = (uint32_t)(tz - tbas) ;

DBprintf(( "tbas=%lu, j=%d trest=%d, tz=%d", tbas, j, trest, tz ));

for ( imon = 1 ; imon < 13 && trest > montab[sj][imon]-1 ; ++imon )
	{
	trest -= montab[sj][imon] ;
	DBprintf(( "imon=%d  trest=%u\n", imon, trest ));
	}
if ( imon == 13 && trest == 0 )
	{
	*pj = j+1 ;
	*pm = 1 ;
	*pt = 1 ;
	DBprintf(( "imon==13 *pj=%d *pm=%d *pt=%d", *pj, *pm, *pt ));
	DBret("todate");
	return ;
	}
if ( imon == 1 && trest == -1 )
	{
	*pj = j-1 ;
	*pm = 12 ;
	*pt = 31 ;
	DBprintf(( "imon==1 *pj=%d *pm=%d *pt=%d", *pj, *pm, *pt ));
	DBret("todate");
	return ;
	}
*pm = imon ;
*pt = (uint16_t)trest+1 ;
DBprintf(( "*pj=%d *pm=%d *pt=%d", *pj, *pm, *pt ));
DBret("todate");
}
