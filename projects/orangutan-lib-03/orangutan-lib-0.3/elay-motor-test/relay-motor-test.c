// relay-motor-test
//
// Tom Benedict

// In this example we demonstrate the use of some of the
// commands in the lightweight relay-motor subsystem.  These
// do not use any system timers, and treat the motors as strictly
// on/off forward/reverse devices.  These routines are specifically
// for the Orangutan and Baby-orangutan.  Since the Orangutan-X2
// uses SPI calls to control its motors, this example will not work
// with it.

// Include our I/O definitions
#include <avr/io.h>

// Include information about the device we're building for
#include "device.h"

// Include for using delay routines
#include <util/delay.h>

// Include for the analog subsystem
#include "relay-motor.h"

// Delay for N seconds
void delay_sec(unsigned char sec)
{
	unsigned int cycles;

	// Delay 25ms at a time (38.4ms is the most we can delay with a
	// 20MHz processor, unfortunately.  See the delay.h include file
	// for more info.)

	for(cycles = 0; cycles < (sec * 40); cycles ++)
	{
		_delay_ms(25);
	}
}


// And now for our main routine:
int main(void)
{

#ifdef _ENABLE_LB1836M_MOTORS_

	// Initialize the relay motor routines:

	motor_init();

	// Motors are stopped when we initialize them, so there's
	// no need to do that after the init routine.

	// The endless loop

	for(;;)
	{
		// Both motors forward
		motor_a_fwd();
		motor_b_fwd();
		delay_sec(2);

		// Coast to a stop
		motor_a_coast();
		motor_b_coast();
		delay_sec(2);

		// Both motors reverse
		motor_a_rev();
		motor_b_rev();
		delay_sec(2);

		// Coast to a stop
		motor_a_coast();
		motor_b_coast();
		delay_sec(2);

		// Spin opposite directions
		motor_a_fwd();
		motor_b_rev();
		delay_sec(2);

		// Brake to a stop
		motor_a_brake();
		motor_b_brake();
		delay_sec(2);

		// Spin opposite directions (the other way)
		motor_a_rev();
		motor_b_fwd();
		delay_sec(2);

		// Brake to a stop
		motor_a_brake();
		motor_b_brake();
		delay_sec(2);
	}

#endif // _ENABLE_LB1836M_MOTORS_

	// We never get here, but return a zero if we ever do.
	return(0);
}
