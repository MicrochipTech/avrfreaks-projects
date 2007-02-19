// check if a given year is a leap ye4ar (with 366 days)

#include <heport.h>
#include <helq.h>

FUNCT bool leapyear( int year )
// decides if a year is a leap year
// return true = leapyear
// return false != leapyear
{
if ( (year&0x0003) != 0 )		// not a leap year as every year
	return false ;				// dividable by 4
if ( year%100 != 0 )			// 1904, 1908 ... 1996, 2004
	return true ;
if ( year % 400 != 0 )			// 1700 1800 1900
	return false ;				// 1600 , 2000
return true ;
}
