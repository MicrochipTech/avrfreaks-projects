
#include <avr/io.h>


int AVRInit_InputPort (void)
{
	DDRB = 0xFF;	// set PORTB for input
	return 1;
}

int AVRInit_OutputPort (void)
{
	DDRD = 0xFF;	// set PORTD for output
	return 1;
}	
	
