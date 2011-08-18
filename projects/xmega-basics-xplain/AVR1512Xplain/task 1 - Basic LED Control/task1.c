/*************************************************************************
 *
 *
 *              Task1: Basic I/O
 *
 *
 *************************************************************************/

// The avr_compiler.h header file includes the MCU specific header 
// file configured in the project options and it also contains 
// compatibility definitions to be able to compile the same code 
// for both the GCC and the IAR compiler. 

// For GCC:
// This will include avr/iox128a1.h for us, since the
// MCU = atxmega128a1 is defined in the project make file.
// #include <avr/io.h>

// For IAR:
// This will include ioxm128a1.h, since target is set to
// --cpu=xm128a1 in the project configuration.

// In addition it contains some macros to make the code more 
// similar for both compilers. Look into the file for more details.
#include "../avr_compiler.h"

// The board.h header file defines which IO ports peripherals like
// Switches and LEDs are connected to. The header file is modified
// for use with Xplain by default.
#include "../xplain.h"

// Since all IO port access refers to a PORT struct defined in the header
// file, it is enough to define the module name only, instead of creating
// #defines for all registers for the port module. To see the definition
// of "LEDPORT" please look at board.h.

volatile unsigned int counter;

int main( void )
{
   LEDPORT.DIR = 0xff;    // Set all pins of port D to output.
   LEDPORT.OUT = 0xf0;    // Set upper four bits high.
   LEDPORT.OUTSET = 0x0f; // Set lower four bits high, do not touch others.
   LEDPORT.OUTCLR = 0xf0; // Clr upper four bits, do not touch others.
   LEDPORT.OUTTGL = 0xff; // Toggle all pins, ones become zeros and vice versa.
   LEDPORT.OUTTGL = 0xff; // Toggle all pins, ones become zeros and vice versa.
   LEDPORT.OUTTGL = 0xAA; // Toggle some pins, ones become zeros and vice versa.
   LEDPORT.OUTTGL = 0xAA; // Toggle some pins, ones become zeros and vice versa.
   LEDPORT.OUTTGL = 0xAA; // Toggle some pins, ones become zeros and vice versa.
   LEDPORT.OUTTGL = 0xAA; // Toggle some pins, ones become zeros and vice versa.
	
   // Now, just for fun, we toggle all LEDs, again and again and again...
   while (1)
   {
        LEDPORT.OUTTGL = 0xff;
        // Delay
        for ( counter=0; counter<10000; counter++) 
		{
			nop();
		}
   }
}
