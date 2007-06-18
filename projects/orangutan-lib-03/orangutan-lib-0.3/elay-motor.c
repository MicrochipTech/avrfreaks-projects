// relay-motor.c
//
// Tom Benedict


// This set of motor routines uses no timers or interrupts at
// all.  It drives the pins connected to the H-bridge cihp directly,
// behaving a lot like relays.  A motor is on full-forward, or
// full-reverse, or it's coasting or it's braking.  No speed
// control at all.
//
// Before turning up your nose at this simplistic approach, keep
// in mind that the resulting code is tiny, with a little effort
// it can be done as inline #defines (which saves you clock cycles
// and stack space), and for a lot of applications this works
// perfectly well.  My mini-sumo robot works this way, and has
// done just fine.  As someone put it, in mini sumo if you're not
// hitting your opponent with everything you've got, you're letting
// them win.
//
// But back to driving motors...
//
// The documentation for the LMB1836M gives the following output
// for each combination of IN1 and IN2 (or IN3 and IN4):
//
//  IN1		IN2		Function
//  ------	------	--------------------
//  0		0		Coast
//	1		0		Forward
//  0		1		Reverse
//  1		1		Brakes
//
// Couldn't be simpler.  To go forward, we hold IN1 (and IN3) at 1,
// and set IN2 (and IN4) at zero.  Vice-versa for reverse.  To coast
// we set them to zero.  To brake we set them to one.
//
// So let's get to it!


// All device-specific dependencies are stored in this file.  You must
// edit it first before using any of the library routines!

#include "device.h"


#include <avr/io.h>


// Fair warning:  All the motor driver pin assignments are stored in
// device.h.  Future Orangutans may not use the same I/O pins as the
// existing ones for motor control, so it seemed wise to keep those
// definitions separate so it's easy to port the code to new
// Orangutans as they're developed.  All of the register names 
// starting with "IN" are defined in device.h.


// Initialize our relay-style routines for our motors:
void motor_init(void) 
{
	// Set up the data direction registers so our four control
	// pins are outputs:

	IN1_DDR |= (1 << IN1_BIT);
	IN2_DDR |= (1 << IN2_BIT);
	IN3_DDR |= (1 << IN3_BIT);
	IN4_DDR |= (1 << IN4_BIT);

	// Set both motors to coast (all four pins low)
	IN1_PORT &= ~(1 << IN1_BIT);
	IN2_PORT &= ~(1 << IN2_BIT);
	IN3_PORT &= ~(1 << IN3_BIT);
	IN4_PORT &= ~(1 << IN4_BIT);
}



// Motor A uses IN1 and IN2:
void motor_a_fwd(void)
{
	IN1_PORT |=  (1 << IN1_BIT);
	IN2_PORT &= ~(1 << IN2_BIT);
}

// Motor B uses IN3 and IN4:
void motor_b_fwd(void)
{
	IN3_PORT |=  (1 << IN3_BIT);
	IN4_PORT &= ~(1 << IN4_BIT);
}

// Motor A uses IN1 and IN2:
void motor_a_rev(void)
{
	IN1_PORT &= ~(1 << IN1_BIT);
	IN2_PORT |=  (1 << IN2_BIT);
}

// Motor B uses IN3 and IN4:
void motor_b_rev(void)
{
	IN3_PORT &= ~(1 << IN3_BIT);
	IN4_PORT |=  (1 << IN4_BIT);
}

// Motor A uses IN1 and IN2:
void motor_a_coast(void)
{
	IN1_PORT &= ~(1 << IN1_BIT);
	IN2_PORT &= ~(1 << IN2_BIT);
}

// Motor B uses IN3 and IN4:
void motor_b_coast(void)
{
	IN3_PORT &= ~(1 << IN3_BIT);
	IN4_PORT &= ~(1 << IN4_BIT);
}

// Motor A uses IN1 and IN2:
void motor_a_brake(void)
{
	IN1_PORT |=  (1 << IN1_BIT);
	IN2_PORT |=  (1 << IN2_BIT);
}

// Motor B uses IN3 and IN4:
void motor_b_brake(void)
{
	IN3_PORT |=  (1 << IN3_BIT);
	IN4_PORT |=  (1 << IN4_BIT);
}


// And that's it!  With those routines, we've got complete
// relay-style control over two motors including coast and
// brake.
