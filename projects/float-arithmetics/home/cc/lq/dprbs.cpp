#include <heport.h>
#include <helq.h>


// Returns a random number in range 3.2596290126721e-009 to 0.99999999394640
// That is almost from 0 to 1.0
// which is 1/(2^31-1) to 1-(1/2^32-1)
FUNCT double dprbs()
{
double v ;

v = (double)( (lprbs() & 0x7fffffffL) / (double)0x7fffffffL );
return v ;
}
