#define DB_ENABLED 0
#define DB_INFO 0

#include <heport.h>
#include <helq.h>
#include <hedeb.h>


// a saver version of strcpy
int txtcpy( char *pdest, uint maxsize, const char *psource )
{
for(;;)
	{
	if ( maxsize <= 1 )
		{			// could not add the end of string character 0
		Eprintf(( "strcpy: source too long" ));
		if ( maxsize == 1 ) *pdest = 0 ;
		return 1 ;
		}
	if ( *psource == 0 ) break ;
	*pdest++ = *psource++ ;
	--maxsize ;
	}
*pdest = 0 ;
return 0 ;		// returns end of string for
}


// a saver version of strcat
int txtcat( char *pdest, uint maxsize, const char *psource )
{
for(;;)
	{
	if ( maxsize < 1 ) return 1 ;
	if ( *pdest == 0 ) break ;
	++pdest ;
	--maxsize ;
	}
return txtcpy( pdest, maxsize, psource );
}
