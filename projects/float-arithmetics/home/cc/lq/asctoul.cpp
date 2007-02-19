#include <heport.h>
#include <helq.h>

/* ---------------------------------------------------------- */

FUNCT int asctoul(		// String to uint32_t convert decimale representation
const char **ppc ,		// from this source string (pointer updated)
uint32_t *adr )			// to that uint32_t place
{
bool	flag_digits ;
register uint32_t	number_buffer ;
const char *pc ;
uint8_t uc ;
int rv ;

pc = *ppc ;
number_buffer = 0L ;
flag_digits = false ;

// skip leading white spaces
skipwhite( pc );

// Convert number
while ( ( uc = (uint8_t)(*pc -'0') ) < 10 ) 		// '0' ... '9'
	{
	flag_digits = true ;			// at least one digit processed
	if ( number_buffer <= 429496729 )		  // 0xffffffff / 10
		{
		number_buffer *= 10 ;
		number_buffer += uc ;
		}
	else
		{
		*ppc = pc ;
		return 2 ;
		}
	++pc ;
	}

// skip trailing blanks
skipwhite( pc );

if ( flag_digits )
	{				// number detected
	*adr = number_buffer ;
	rv =  0 ;	   // no error detected
	}
else
	{
	rv =  1 ;	   // no digits found
	}
*ppc = pc ;			// This character has end up the conversion
return rv ;
}				// end asctoul()
