
#include <heport.h>
#include <helq.h>


/* ---------------------------------------------------------- */
/*
DD asctod
DD convert an ASCII (String) into a	 flaot number
DD 1.P: char *st		   e.g. "3.1415"
DD 2.P: double *perg		   Save result here.
DD int rv 0 wenn alles o.k.
 */
FUNCT int asctofl(				 // String to double
const char	  **ppc ,						// String
float *adr )				  // save double result here
{
register const char	  *lst ;
sint siX ;
float rest ;
float z ;
bool flag_neg ;
bool flag_neg_exp ;

flag_neg = false ;

lst = *ppc ;
skipwhite( lst );

if ( *lst == '-' )
	{
	flag_neg = true ;
	++lst ;
	}
z = 0.0 ;
// pre decimale point
while( *lst >= '0' && *lst <= '9' )
	{
	z = z * 10 + (*lst-'0') ;
	++lst ;
	}
if ( *lst == '.' )		// The decimale point
	{
	rest = 0.1F ;
	++lst ;				// the part after decimale point
	while( *lst >= '0' && *lst <= '9' )
		{
		z = z + rest * (float)(*lst-'0') ;
		++lst ;
		rest /= 10.0F ;
		}
	}
if ( *lst == 'e' || *lst == 'E' )	// The Exponent
	{
	siX = 0 ;
	++lst ;
	flag_neg_exp = false ;
	if ( *lst == '-' )
		{
		flag_neg_exp = true ;
		++lst ;
		}
	siX = 0 ;
	while( *lst >= '0' && *lst <= '9' )
		{
		siX = siX * 10 + (*lst-'0') ;
		++lst ;
		}
	if ( flag_neg_exp ) siX = -siX ;
	if ( (siX < -1000) || (siX > 1000) )
		{
		*ppc = lst ;
		return E 7353 ;		// exponent error
		}
	if ( siX < 0 )
		{
		while ( siX < -31 )
			{
			z /= 1.0E32F ;
			siX += 32 ;
			}
		if ( siX < -15 )
			{
			z /= 1.0E16F ;
			siX += 16 ;
			}
		if ( siX < -7 )
			{
			z /= 1.0E8F ;
			siX += 8 ;
			}
		if ( siX < -3 )
			{
			z /= 1.0E4F ;
			siX += 4 ;
			}
		if ( siX < -1 )
			{
			z /= 1.0E2F ;	// = 100
			siX += 2 ;
			}
		if ( siX < 0 )
			{
			z /= 10.0F ;
			++siX ;
			}
		}
	if ( siX > 0 )
		{
		while ( siX >= 32 )
			{
			z *= 1.0E32F ;
			siX -= 32 ;
			}
		if ( siX >= 16 )
			{
			z *= 1.0E16F ;
			siX -= 16 ;
			}
		if ( siX >= 8 )
			{
			z *= 1.0E8F ;
			siX -= 8 ;
			}
		if ( siX >= 4 )
			{
			z *= 1.0E4F ;
			siX -= 4 ;
			}
		if ( siX >= 2 )
			{
			z *= 1.0E2F ;
			siX -= 2 ;
			}
		if ( siX >= 1 )
			{
			z *= 10.0F ;
			--siX ;
			}
		}
	}
if ( flag_neg )
	{
	z = -z ;
	}
*adr = z ;
*ppc = lst ;
return 0 ;		// converted without detected errror
}								// end asctofl()
