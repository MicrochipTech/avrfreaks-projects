// counter-test
//
// Tom Benedict

// This is a not so good example of how to use a really neat
// feature of practically any microcontroller: counters and timers.
// The implementation of the counter code for Orangutan-lib 
// represents only a microscopically small subset of the things you 
// can do with a timer.  (Good case in point, the PWM code and servo
// code both use timers as well.)
//
// In this example we're using timers to run delays while blinking
// an LED once a second.  This is a bad example because there are 
// other, lighter-weight routines that'll do this just as well as we
// are here.  The only thing that makes this interesting is that
// we're printing out our delay counter value to the LCD while we're
// waiting.  (This assumes you're using an Orangutan.)
//
// A better example would be to use the counter as a time-to-live
// timer.  For example, in the mini-sumo competition the robots must
// not move for five seconds after their start buttons are pressed.
// This is to give humans a chance to clear the playing area.  You
// can't move, but you CAN read your sensors, size up your opponent,
// etc.  To do this you could load a time-to-live timer with five
// seconds worth of count, then take your time initializing 
// everything, enabling sensors, running calibration routines, etc.
// until that timer reads zero.
//
// The point is the CPU is not busy while the counting is going on,
// unlike the _delay_ms() macros available in WinAVR.  In this 
// example we're tying up the CPU ourselves with a wait() loop.  
// But there's oh so much more we could do while the countdown
// is going on!
//
// Tom Benedict


// Include our I/O definitions
#include <avr/io.h>

// Include information about the device we're building for
#include "device.h"

// Include for the counters subsystem
#include "counter.h"

// Include for the LCD subsystem
#include "lcd.h"

#include <util/delay.h>

// And now for our main routine:
int main(void)
{
	// Define a variable to be a countdown timer
	// The "volatile" is there so the compiler's optimization
	// doesn't just remove our countdown loop.
	volatile unsigned int countdown;

	// Set up our LED as an output:
	LED_DDR |= (1 << LED0);

	// Initialize the counter subsystem
	counter_init();

	// And set up our variable with the system
	counter_define_down(&countdown);

#ifdef _ENABLE_LCD_
	// Initialize the LCD
	lcd_init();

	// Put a nice message on the top line
	lcd_string("Cnt-down");
#endif

	// The endless loop
	for(;;)
	{
		// Set up the counter for 1000ms (1 sec)
		countdown = 1000;

		// Wait until it spins down
		while(countdown > 0)
		{
#ifdef _ENABLE_LCD_
			// Go to line 2
			lcd_moveto(2,0);
			// Print out our countdown value
			lcd_int(countdown);
#endif
			// Wait for a bit (otherwise we really are blurring
			// stuff on the LCD a little TOO much)
			_delay_ms(5);
		}

		// Toggle the LED
		LED_PORT ^= (1 << LED0);
	}

	// We never get here, but return a zero if we do:
	return(0);
}
