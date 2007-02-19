#include <heport.h> // special type names, uint16_t, int16_t ...
#include <helq.h>	// uprintf
#include <tshar.h>
#include <tsser.h>	// prototypes for all functions defined here



#ifdef AVR
// read an float (float) number from serial input and convert to float
FUNCT char tsserfloat_dec_P(
	PGM_P prompt,	// send / display this before reading characters (out of program memory)
	float *p )		// Save the converted hex number here
{
const char *pa ;

tsserputs_P( prompt );

if ( tssergetline() ) return 1 ;
pa = tsser_charbuffer ;
if(	 asctofl( &pa, p ) == 0 )
	{
	return 0 ;		// no error
	}
return 2 ;
}
#else
// read an unsigned word, in hex format
FUNCT char tsserfloat_dec(
	const char *prompt, // send / display this before reading characters
	float *p )		// Save the converted hex number here
{
const char *pa ;

tsserputs( prompt );

if ( tssergetline() )
	{
	return 1 ;
	}
pa = tsser_charbuffer ;
if(	 asctofl( &pa, p ) == 0 )
	{
	return 0 ;		// no error
	}
return 2 ;
}
#endif


