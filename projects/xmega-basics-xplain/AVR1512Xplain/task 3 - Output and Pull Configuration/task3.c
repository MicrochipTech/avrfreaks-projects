/*************************************************************************
 *
 *
 *              Task3: Output and Pull Configuration
 *
 *
 *************************************************************************/

// The avr_compiler.h header file includes the MCU specific header 
// file configured in the project options and it also contains 
// compatibility definitions to be able to compile the same code 
// for both the GCC and the IAR compiler. 
#include "../avr_compiler.h"

// The board.h header file defines which IO ports peripherals like
// Switches and LEDs are connected to. The header file is configured
// for use with Xplain by default.
#include "../xplain.h"


volatile unsigned char value;


int main( void )
{
	// Prepare our bit mask for which output pins to use.
	unsigned char const mask = PIN0_bm | PIN1_bm | PIN2_bm; // We only want to use pin 0, 1, and 2 of the LED port.

	LEDPORT.DIR = mask;	

	// Read-modify-write control registers for pin 0, 1, and 2. This shows how to use the
	// "group mask" (_gm) value of a configuration bitfield to clear the bitfield before using
	// one of the "group configuration" (_gc) values to set a new value for the bitfield.
    SWITCHPORT.PIN0CTRL = (SWITCHPORT.PIN0CTRL & ~PORT_OPC_gm) | PORT_OPC_WIREDANDPULL_gc;
	SWITCHPORT.PIN1CTRL = (SWITCHPORT.PIN1CTRL & ~PORT_OPC_gm) | PORT_OPC_PULLUP_gc;
	SWITCHPORT.PIN2CTRL = (SWITCHPORT.PIN2CTRL & ~PORT_OPC_gm) | PORT_OPC_PULLUP_gc;

	// Enable inverting of pin 2 to see how this works.
	SWITCHPORT.PIN2CTRL |= PORT_INVEN_bm;

	// Now copy switch states to ledPort again and again and again...
	while(1)
	{
		// Read input, remove unwanted bits and return value.
		value = SWITCHPORT.IN & mask;

		// This construct reads existing value from port's OUT register, clears the bits
		// covered by the mask value and then applies the value.
		LEDPORT.OUT = (LEDPORT.OUT & ~mask) | value;

		nop();
	}
}
