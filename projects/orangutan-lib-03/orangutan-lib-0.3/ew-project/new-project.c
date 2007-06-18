// new-project

// So how to get started using Orangutan-lib?  By jumping in feet-
// first, of course!
//
// This blank project template has been set up for you to start 
// including and linking in the library code of your choice.  So 
// what makes this setup different from the stock AVR Studio 4 
// setup?  Two things really:
//
// The first is a compiler option has been added so it knows where 
// to find the Orangutan-lib .h files.  In the menu bar select 
// Project -> Configuration Options -> Custom Options  to bring up
// the compiler flag dialog.  In a stock AVR Studio 4 project, the
// following flags are there by default:
//
// 		-Wall
// 		-gdwarf-2
// 		-O0
// 		-fsigned-char
//
// To let the compiler know where the library's files are, we've added
// one additional flag:
//
// 		-I"../../"
//
// That adds "../../", or "go back two directories" to the search path
// for .h files.
//
// The second difference between this project setup and a stock AVR
// Studio 4 setup is that the Orangutan-lib .c files are added to the
// Source Files list by clicking "Add Existing Source File(s)" and
// navigating back one directory to where the library files live.  You
// can see this same sort of setup with all of the *-test projects.
//
// And that's about it!
//
// Have fun, enjoy your Orangutan, and make cool stuff.
//
// -- The Orangutan-lib Crew


// Start by including the device-specific stuff for Orangutan-lib
// (And be sure to edit that file to let it know what kind of Orangutan
// you're programming!
#include "device.h"

// Now include the subsystems you want to use:

// Commands to use the A/D subsystem (must include analog.c in
// Source Files)
// #include "analog.h"

// Defines to use bitfields to access individual bits in data
// registers.  (No associated .c file)
// #include "bitfield.h"

// Commands to use the LCD on the Orangutan (must include lcd.c
// in Source Files)
// #include "lcd.h"

// For motor control you can only use one motor control system
// at a time.  Right now the two available are relay-style (fwd,
// rev, brake, coast), and PWM speed control using TIMER0. 
// Include only one!  If you include both you'll get compile
// errors and unpredictable behavior.

// Commands to use TIMER0 in PWM mode to do speed control on
// motors (must include pwm0.c in Source Files)
// #include "pwm0.h"

// Commands to do relay-style motor control (must include 
// relay-motor.c in Source Files)
// #include "relay-motor.h"

// Commands to drive the onboard buzzer (must include buzzer.c
// in Source Files)
// #include "buzzer.h"

// Commands to drive RC servos on our I/O pins (must include servo.c
// in Source Files)
// #include "servo.h"

// Past here the library has been loaded, and the rest is up to you.


int main(void)
{

	// The endless loop
	for(;;)
	{
	}

	// We never get here
	return 0;
}
