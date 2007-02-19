
#include <heport.h>
#include <helq.h>



// asctotok.cpp asctotok() : copies only tokens a-z A-Z 0-9
// asctonam.cpp asctonam() : copies only a-z A-Z _ 0-9, must not start with 0-9
// asctostr.cpp asctostr() : until end of string (end of line , 0-Character)
// asctostb.cpp asctostb() : stops at separators if ( c == ':' || c == ',' || c == '\0' || c == ' ' || c == '\t' ) break ;
// asctostc.cpp asctostc() : stops only at separators if ( c == ',' || c == '\0' || c == '\t' ) break ;
// asctostq.cpp asctostrquot() : string must start with ", ' or [ and must then end with ", ' resp ]
/* ---------------------------------------------------------- */
/* Saver replacement for strcpy / strncpy
   ASCII to any String ( Similar result as strncpy ) (See also txtcpy)
   opposite function is Cst::puts()
 */
FUNCT int asctostr(		// get string, rest of line
const char **ppc ,	   // Pointer to String-pointer, will be modified
char *adr,				// where to save string
int nmax )				// but not more than... This ist the destination buffers size
						// The maximum string length is 1 less
{
int i ;
const char *pc ;
char c ;

pc = *ppc ;				// Get string Pointer from Argument, Pointer to Pointer

skipwhite( pc );

i = 0 ;					// buffer size counter
while ( ( c = *pc ) != 0 )
	{
	if ( ++i >= nmax )
		{
		*adr = 0 ;		// end up string even in error case
		*ppc = pc ;		// save pointer to caller
		return 2 ;		// ERROR overflow
		}
	*adr++ = c ;
	++pc ;
	}
*adr = 0 ;
*ppc = pc ;		// save pointer to caller
return 0 ;		// end of numeric
}		/* end asctostr() */
