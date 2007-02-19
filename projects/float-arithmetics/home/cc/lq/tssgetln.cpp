#include <heport.h>		// special type names, uint16_t, int16_t ...
#include <helq.h>		// uprintf
#include <tshar.h>
#include <tsser.h>		// prototypes for all functions defined here


// read a line of text
FUNCT char tssergetline( void )
{
uint8_t i ;
char c ;
char *p ;						// Save the string here
uint8_t n ;						// Maximum space

p = tsser_charbuffer ; 			// Save the string here
n = TSSER_NCHARBUFFER ;			// Maximum space
i = 0 ;
for(;;)
	{
	c = tsserichaw();
	if ( c == 0x20 ||			// Blank
		 c == ',' ||			// Blank
		 c == 0x0d )			// Enter
		{
		p[i] = 0 ;
		return 0 ;
		}
	if ( c < ' ' )
		{
		if ( c == 0x7f || c == 0x08 )		// delete or backspace: remove one character from end
			{
			if ( i > 0 )
				{
				--i ;
				tsserputs( "\b \b" );
				}
			}
		if ( c == 'X'-'@' || c == 'U'-'@' )		// Control X - or control U : Clear line
			{
			while ( i > 0 )
				{
				--i ;
				tsserputs( "\b \b" );
				}
			}
		if ( c == 0x1b ) return 1 ; // Abort
		}
	else
		{
		if ( i < n-1 )
			{
			p[i++] = c ;
			tsserochaw(c);			// Echo the stored character
			}
		}
	}
}

