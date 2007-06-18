// servo-test
//
// Tom Benedict

// This example demonstrates the use of the servo subsystem.  It will
// work on all three Orangutan platforms.  In this example servos are
// plugged into PC0 and PC1.  If you use this example on the Orangutan-X2,
// these will need to be changed to I/O pins on PORTA or PORTD since these
// are the only two ports brought out to I/O headers.

// Include our I/O definitions
#include <avr/io.h>

// Include information about the device we're building for
#include "device.h"

// Include for using delay routines
#include <util/delay.h>

// Include for the analog subsystem
#include "servo.h"

// And now for our main routine:
int main(void)
{

	// Two servos
	unsigned char left, right;

	// Some loop variables
	unsigned char i;
	unsigned int  time;

	// Initialize the servo system
	servo_init();

	// Define our servos to use PC0 and PC1, respectively
	left  = servo_define(_SFR_IO_ADDR(DDRC), _SFR_IO_ADDR(PORTC), 0);
	right = servo_define(_SFR_IO_ADDR(DDRC), _SFR_IO_ADDR(PORTC), 1);

	// Turn them both on
	servo_active(left);
	servo_active(right);

	// Our endless loop
	for(;;)
	{
		// Run from 1000us to 2000us in 10us increments
		for(time = 1000; time <= 2000; time += 10)
		{
			// Sort of boring...  Set both to "time"
			servo_set(left, time);
			servo_set(right, time);

			// 50ms delay
			for(i = 0; i < 5; i++)
			{
				_delay_ms(10);
			}
		}
	}
}

