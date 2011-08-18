/*************************************************************************
 *
 *
 *              Task4: Multiconfiguration
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
// for use with XMEGA-A1 Xplained by default.
#include "../xplain.h"

int main( void )
{
    // Setting TESTPORT and LEDPORT as output and 0xFF as default output value on LEDPORT
    TESTPORT.DIR = 0xFF;    
	LEDPORT.DIR = 0xFF;
	LEDPORT.OUT = 0xFF;

    // MPCMASK can be used to set a bit mask for the pin configuration registers.
    // When setting bit n in MPCMASK, PINnCTRL is added to the pin configuration mask.
    // During the next write to any of the port's pin configuration registers, the 
    // same value will be written to all the port's pin configuration registers set by the mask.	
    PORTCFG.MPCMASK = 0xFF;
    
    // The WIREDANDPULL enables the internal pull-up. 
    // Also, when the port is configured as output (dir=1), 
    // setting a pin low will drive low, 
    // but setting a pin high will set the pin in tri-state
    TESTPORT.PIN0CTRL = (TESTPORT.PIN0CTRL & ~PORT_OPC_gm) | PORT_OPC_WIREDANDPULL_gc;

    // The MPCMASK register is cleared automatically after the write operation to the pin 
    // configuration registers is finished.

    // Connect TESTPORT pin 0 and 1 with a jumper on the board
               
    while(1)
    {
        // Set pins 0 and 1 high
        TESTPORT.OUTSET = PIN0_bm;
        TESTPORT.OUTSET = PIN1_bm;
        // Now, the pins are in tri-state 
        // The internal pull-up will drive the line high 
    
		// Check state of pin 1
        if(TESTPORT.IN & PIN1_bm)
        {
			// Pin 1 is high
            // No other pins are driving low
			LEDPORT.OUTCLR= PIN0_bm;
            nop();
        }

        // Let pin 0 drive the line low
	    TESTPORT.OUTCLR = PIN0_bm;

        // Recheck state of pin 1
        if((~TESTPORT.IN) & PIN1_bm)
        {            
			// Pin 1 is low
            // Another pin is driving the line low!
			LEDPORT.OUTSET= PIN0_bm;
            nop();
        }
	}
}
