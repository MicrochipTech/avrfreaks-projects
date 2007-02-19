#define DB_ENABLED 0
#define DB_INFO 0
// epoch.cpp :
// convert gregorian time to seconds after 1.1.1970 0:0:0

#include <heport.h>
#include <helang.h>
#include <hedeb.h>
#include <helq.h>
#include <hegettim.h>



// This function converts a given date (with time) into unix epoch value
// the back conversion ist done by etodate()
// special epoch values are :
//
FUNCT CEpoch::CEpoch( uint yr,uint mo,uint dy,	uint hr,uint mi,uint sc)
{
uint32_t rv_day ;
uint32_t rv_time ;


DBent("epoch");
DBprintf(( "%d.%d.%d ", dy, mo, yr ));
DBprintf(( "%02d:%02d:%02d\n", hr, mi, sc ));

rv_day = daycount( yr, mo, dy );
rv_day -= DAYCOUNT_1970_1_1 ; /* the daycount(1970,1,1) */
rv_time = (uint32_t)((uint32_t)( hr * 60 + mi ) * 60L) + (uint32_t)sc ;
ulE = rv_time + rv_day * 86400L ;
DBprintf(( "rv_day=%lu rv_time=%lu ulE=%lu\n", rv_day, rv_time, ulE ));
DBret("epoch");
return ;
}


#ifdef __TURBOC__
CEpoch::CEpoch( uint32_t val )
	{ ulE = val; }				// Number initalize
#endif
