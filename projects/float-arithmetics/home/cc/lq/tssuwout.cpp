#include <heport.h>		// special type names, uint16_t, int16_t ...
#include <helq.h>		// uprintf
#include <tshar.h>
#include <tsser.h>		// prototypes for all functions defined here



FUNCT void tsserdec_uw_out( uint16_t h )	// Output a decimale number 0 .. 65535
// output always 5 characters, padd by blanks if the number is too small
{
char a[5] ;
uint16_t r ;
uint16_t z ;
uint8_t i ;
bool must ;

r = h ;
for ( i = 5 ; i ; )
	{
	--i ;
	z = r ;
	r = r / 10 ;
	a[i] = z - r * 10 ;
	}
must = false ;
for ( i = 0 ; i < 5 ; ++i )
	{
	if ( i == 4 || a[i] != 0 || must )
		{
		must = true ;
		tsserochaw( a[i] + '0' );
		}
	else
		{
		tsserochaw( ' ' );
		}
	}
}
