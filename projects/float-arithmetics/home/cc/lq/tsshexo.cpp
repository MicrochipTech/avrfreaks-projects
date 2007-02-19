#include <heport.h>		// special type names, uint16_t, int16_t ...
#include <helq.h>		// uprintf
#include <tshar.h>
#include <tsser.h>		// prototypes for all functions defined here



FUNCT void tsserhx1_out( uint8_t h )	// Output a single hex digit
{
h &= 0x0f ;
if ( h >= 10 ) tsserochaw( h + ( 'A' - 0x0a ) );
else tsserochaw( h + '0' );
}



FUNCT void tsserhx2_out( uint8_t h )	// Output two hex digits
{
tsserhx1_out( h >> 4 );
tsserhx1_out( h & 0x0f );
}



FUNCT void tsserhx4_out( uint16_t h )	// Output four hex digits
{
tsserhx2_out( (uint8_t)(h >> 8) );
tsserhx2_out( (uint8_t)h );
}
