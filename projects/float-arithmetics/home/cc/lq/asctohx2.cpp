
// Programmiert von Hans-Juergen Herbert


#include <heport.h>
#include <helq.h>


/* ---------------------------------------------------------- */
// convert exactlty 2 hex characters (0-9, A-F or a-f) into a uint8_t
// return error if bad character inside found
// Update Pointer to string, so end of hex string character
// might be examined.
FUNCT int asctohx2(		 // HEX represented string to binary
char const	**plst ,		   // where to find the string
uint8_t	*adr )			// where to store the result
{
uint8_t zif1 ;
uint8_t zif2 ;
const char *lst ;


lst = *plst ;

if ( lst[0] == '0' && ( lst[1] == 'X' || lst[1] == 'x' ) ) lst += 2 ;

if ( *lst >= '0' && *lst < '9'+1 )
	{
	zif1 = *lst - '0' ;
	}
else if ( *lst >= 'A' && *lst <='F' )
	{
	zif1 = *lst + ( -'A' + 0x0a ) ;
	}
else if ( *lst >= 'a' && *lst <= 'f' )
	{
	zif1 = *lst + ( -'a' + 0x0a ) ;
	}
else
	{
	*plst = lst ;
	return 1 ;		// bad hex character 1
	}

++lst ;
if ( *lst >= '0' && *lst < '9'+1 )
	{
	zif2 = *lst - '0' ;
	}
else if ( *lst >= 'A' && *lst <='F' )
	{
	zif2 = *lst + ( -'A' + 0x0a ) ;
	}
else if ( *lst >= 'a' && *lst <= 'f' )
	{
	zif2 = *lst + ( -'a' + 0x0a ) ;
	}
else
	{
	*plst = lst ;
	return 2 ;		// bad hex character 2
	}
++lst ;
*adr = ( zif1 << 4 ) | zif2 ;
*plst = lst ;
return 0 ;

}		/* end asctohx2() */
