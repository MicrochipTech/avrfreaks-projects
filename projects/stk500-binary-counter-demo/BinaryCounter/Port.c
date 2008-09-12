
#include <avr/io.h>

int SetPort (set_value)
{

	PORTB = set_value;		
	return(1);

}
