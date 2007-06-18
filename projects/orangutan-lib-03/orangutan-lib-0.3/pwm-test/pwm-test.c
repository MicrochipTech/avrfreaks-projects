// buzzer-test
//
// Tom Benedict
// Steve Judd

// In this example we demonstrate some of the commands in the PWM
// subsystem.  The PWM code is for the Orangutan and Baby-Orangutan.
// Since the Orangutan-X2 uses SPI calls to control the motor drivers,
// it cannot use this example.

// Include our I/O definitions
#include <avr/io.h>

// Include information about the device we're building for
#include "device.h"

// Include information for the delay routines
#include <util/delay.h>

// Include for the pwm subsystem
#include "pwm.h"

// Delay for N seconds
void delay_sec(uint8_t sec)
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
	int speed;

#ifdef _ENABLE_LB1836M_MOTORS_
	// Initialize the PWM system:
	pwm_init();

	// Motors are stopped when we initialize them, so there's
	// no need to do that after the init routine.

	// The endless loop

	for(;;)
	{
		for (speed = -255; speed < 255; speed += 5)
		{
			// Both motors half forward
			pwm_a(speed);
			pwm_b(speed);
			_delay_ms(25);
			_delay_ms(25);
			_delay_ms(25);
			_delay_ms(25);
			_delay_ms(25);
			_delay_ms(25);
			_delay_ms(25);
			_delay_ms(25);
			_delay_ms(25);
			_delay_ms(25);
		}
		for (speed = 255; speed > -255; speed -= 5)
		{
			// Both motors half forward
			pwm_a(speed);
			pwm_b(speed);
			_delay_ms(25);
			_delay_ms(25);
			_delay_ms(25);
			_delay_ms(25);
			_delay_ms(25);
			_delay_ms(25);
			_delay_ms(25);
			_delay_ms(25);
			_delay_ms(25);
			_delay_ms(25);
		}

		// Both motors full forward
		pwm_a(255);
		pwm_b(255);
		delay_sec(1);

		// Coast to a stop
		pwm_a(0);
		pwm_b(0);
		delay_sec(2);

		// Both motors two-thirds reverse
		pwm_a(-127);
		pwm_b(-127);
		delay_sec(1);

		// Both motors full reverse
		pwm_a(-255);
		pwm_b(-255);
		delay_sec(1);

		// Brake to a stop
		pwm_a_brake();
		pwm_b_brake();
		delay_sec(2);

	}

#endif // _ENABLE_LB1836M_MOTORS_

	// We never get here, but return a zero if we ever do.
	return(0);
}

