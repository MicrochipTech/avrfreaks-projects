#include <avr/io.h>
#include <avr/interrupt.h>

#include "serial.h"
#include "kb.h"


/* -------------------------------------------------------------------
	Atmel application note AVR313 ported for use with GCC
	by Mike Henning - September 2008

	Changes: 
	- Modified to use the ATMEGA16 instead of the obsolete 90S8515.
	- Changed the scan code table for english keyboards
	- Baud rate can be set under project configuration in Avr Studio.
	- The clr screen routine is not implemented - did not see the
	  point here.
---------------------------------------------------------------------
	A few improvements could be made:

	-Check kb parity bit and implement a timeout for keyboard scan
	-Move the call to decode out of the ISR
	-Add support for sending commands to the keyboard
	-Add support for extended keys

-------------------------------------------------------------------*/

int main(void)
{

	uint8_t key;

	// Initializes UART transmit buffer and keyboard reception
	init_uart();
	init_kb();
	sei();

	while(1)
	{
		key = get_kbchar();
		putchar(key);
	}
	return 0;
}


