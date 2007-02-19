#include <heport.h>		// special type names, uint16_t, int16_t ...
#include <helq.h>		// uprintf
#include <tshar.h>
#include <tsser.h>		// prototypes for all functions defined here


FUNCT void tsserdec_ul_out( uint32_t h)	// Output a decimale number 0 .. 4294967295
{
uint32_t r ;
uint32_t z ;
uint8_t i ;
bool must ;

r = h ;
for ( i = 10 ; i ; )
	{
	--i ;
	z = r ;
	r = r / 10 ;
	tsser_charbuffer[i] = z - r * 10 ;
	}
must = false ;
for ( i = 0 ; i < 10 ; ++i )
	{
	if ( i == 9 || tsser_charbuffer[i] != 0 || must )
		{
		must = true ;
		tsserochaw( tsser_charbuffer[i] + '0' );
		}
	}
}

