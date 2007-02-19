#include <heport.h>		// special type names, uint8_t, int8_t ...
#include <helq.h>		// uprintf
#include <tshar.h>
#include <tsser.h>		// prototypes for all functions defined here


// read an unsigned char, in hex format
FUNCT char tsseruchar_hex2( uint8_t *p )		// Save the converted hex number here
{
char a[3] ;
const char *pa ;


a[0] = tsserichaw();	// read the first character
tsserochaw( a[0] );	// echo it
a[1] = tsserichaw();	// read the second character
tsserochaw( a[1] );	// echo it
a[2] = 0 ;
pa = a ;
return asctohx2( &pa, p );
}

