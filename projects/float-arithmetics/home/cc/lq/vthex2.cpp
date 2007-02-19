// vthex2.cpp
// Display (fast) 2 hex caharacters
//

#include <heport.h>
#include <helq.h>
#include <hevt.h>


void vthex2(uint8_t value)
{
static char hexlookuptable[16+1] = "0123456789ABCDEF" ;

vtocha( hexlookuptable[ (value>>4) & 0x0f] );
vtocha( hexlookuptable[ (value	 ) & 0x0f] );
}
