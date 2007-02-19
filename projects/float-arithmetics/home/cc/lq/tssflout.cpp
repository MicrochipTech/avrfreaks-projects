
#include <math.h>		// isnan()
#include <heport.h>		// special type names, uint16_t, int16_t ...
#include <helq.h>		// uprintf
#include <tshar.h>
#include <tsser.h>		// prototypes for all functions defined here



FUNCT void tsserdec_fl_out( float h)	// Output a decimale number 0.0 .. 100000000.0
{
char a[10] ;
uint32_t z ;
uint32_t l ;
uint8_t i ;
uint8_t maxi ;
bool must ;
int	exponent ;	// E+xx E-xx

if( *(uint32_t *)&h == 0x7F800000 )	// overflow
	{
	tsserputs( "Max" );
	return ;
	}
if( *(uint32_t *)&h == 0xFF800000 )	// Underflow
	{
	tsserputs( "Min" );
	return ;
	}
if( h < 0 )
	{
	tsserochaw('-');
	h = -h ;
	}

exponent = 0 ;
if ( h >= 1000000.0F )
	{
	while ( h > 1000.0F )
		{
		h = h * 0.001F ;
		exponent += 3 ;
		}
	}
if ( (h < 0.01F) && (h != 0.0F) )
	{
	while ( h < 0.1F )
		{
		h = h * 1000.0F ;
		exponent -= 3 ;
		}
	}

l = (uint32_t)h ;
for ( i = 10 ; i ; )
	{
	--i ;
	z = l ;
	l = l / 10 ;
	a[i] = (char)(z - l * 10) ;
	}
must = false ;
maxi = 10 ;
for ( i = 0 ; i < 10 ; ++i )
	{
	if ( i == 9 || a[i] != 0 || must )
		{
		if ( must == false )
			{
			maxi = 10-i ;
			must = true ;
			}
		tsserochaw( a[i] + '0' );
		}
	}
// does not work : f = r-r.floor();

// h =  123.456802F ;

h = h - (uint32_t)h ;
if ( h == 0 ) return ;

if ( maxi > 7 ) return ;
tsserochaw( '.' );
for(;;)
	{
	h *= 10.0F ;
	i = (uint8_t) (uint)h;
	tsserochaw( i + '0' );
	h = h - i ;
	if ( ++maxi > 7 ) break ;
	}
if ( exponent )
	{
	tsserochaw( 'E' );
	if ( exponent < 0 )
		{
		tsserochaw( '-' );
		exponent = -exponent ;
		}
	else
		{
		tsserochaw( '+' );
		}
	tsserochaw( exponent / 10 + '0' );
	tsserochaw( exponent % 10 + '0' );
	}
return ;
}
