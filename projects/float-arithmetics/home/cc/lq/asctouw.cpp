#include <heport.h>
#include <helq.h>

/* ---------------------------------------------------------- */
FUNCT int asctouw(			// String to signed word
const char **ppc ,			// There is the string, f.e. "-12", "32765"
uint16_t	 *adr )				// save binary to this location
{
uint8_t			knz ;		// Flag: digits are detected
register uint16_t	  zzz ;

zzz = 0 ;
knz = 0 ;

skipwhite( *ppc );

while ( **ppc >= '0' && **ppc < '9'+1 )
	{
	knz = 1 ;
	if ( zzz <= 6552 )
		{
		if ( zzz == 6553 && **ppc > '5' )
			{
			return 2 ;	// number too big
			}
		zzz = zzz * 10 + **ppc - '0' ;
		}
	else
		{
		return 2 ;		// number too big
		}
	++*ppc ;
	}


/* numbers are divided by follow characters */
if ( knz )
	{				// Digits detected
	*adr = zzz ;
	return 0 ;		// No error
	}
else
	{
	return 1 ;		// error - no number
	}
}		 /* end asctouw() */

