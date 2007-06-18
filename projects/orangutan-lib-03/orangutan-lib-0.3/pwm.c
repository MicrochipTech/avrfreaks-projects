// pwm.c
//
// Tom Benedict
// Steve Judd

// This set of motor PWM routines uses either TIMER0 or TIMER2, both
// 8-bit timers with dual channels and interrupts for compare match 
// on each channel, plus an interrupt for timer overflow.  The output
// compare signals are used to trigger interrupts, which in turn 
// toggle the control bits for the A and B motor channels to drive 
// the motors forward or reverse.
//
// This has three big advantages over the older code, which tied OC0A
// and OC0B to PD6 and PD5:  First, speeds are symmetric forward and
// reverse, whereas the older code was asymmetric.  Second, and more
// important, the PWM code is no longer tied to particular pins, 
// making it more flexible and easier to port to other devices.
// Finally, this makes the code timer independent as well, which means
// if you have a particular need for either TIMER0 or TIMER2, you can
// move the PWM motor code to the unused timer.
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
// The original PWM0 motor code tied OC0A and OC0B to PD6 and PD5,
// and used TIMER0 to send pulse-width modulated signals to motors A
// and B.  But it meant in one direction the motors were either
// energized or coasting, and in the other they were either energized
// or braking.
//
// To avoid this we don't use the built-in connection between OC0A/B 
// and PD5/PD6.  Instead we will use the Compare-Match interrupts for
// the timer and toggle the bits ourselves.
//
// To go in reverse, we hold the motor driver's IN1 (and IN3) at 
// zero, and toggle IN2 (and IN4) on and off.  The more they're on,
// the faster the motors spin.  And when they're off the motors 
// coast.
//
// To go forward, we hold the motor driver's IN2 (and IN4) at zero,
// and toggle IN1 (and IN3) on and off.  The more they're on, the 
// faster the motors spin.  And when they're off the motors coast.
//
// Nothing is free, and the "cost" of using the interrupt-driven 
// motor control is in interrupt bandwidth - this approach generates 
// interrupts at 2x the PWM frequency.  Fortunately this is not that 
// big a deal given the processor speeds and PWM speeds involved.
//
// We can help this last problem by being careful to enable and disable
// interrupts only as we need them.  Stop the motors, the interrupts
// stop happening.
//
// So let's get to it!


// All device-specific dependencies including timer selection are 
// stored in this file.  You must edit it first before using any of
// the library routines!

#include "device.h"

// Only include this code if we have an Orangutan device that has an
// onboard LB1836M motor driver (set in "device.h").
#ifdef _ENABLE_LB1836M_MOTORS_

// Since we're doing pin I/O and interrupts, we need include files for
// each:

#include <avr/io.h>
#include <avr/interrupt.h>

// Some direction definitions we'll use later on:

#define	M_FWD	-1
#define M_STOP	 0
#define M_REV	 1

// Globals to store motor direction values:

static int8_t m_a=0, m_b=0;


// In reading through the following routines, you'll find a bunch
// of timer registers preceeded by an underline, such as _TCCRA.
// These are defined in device.h, and will either correspond to
// the registers for TIMER0 or TIMER2, depending on which has been
// selected for the PWM code to use.  If you're planning to use
// this set of routines outside of Orangutan-lib, be sure either
// to snip that section out of the device.h file and include it in
// your code, or change these register names to use the timer of
// your choice.


// Initialize the timer to do interrupt-based PWM for our motors:
void pwm_init(void)
{
	// Set the Timer Control Register to mode 0.  This is Normal
	// Mode, and treats the counter simply as a counter.

	_TCCRA = 0; 

	// Set the Timer Control Register to set a clock prescale of 256
	// This sets a PWM frequency of Clock / 256 / 256 or:
	// Orangutan:		 122.07031250 Hz
	// Baby-Orangutan:	 305.17578125 Hz

	// Mode 3 is 64, or:
	// Orangutan:		 488.281250 Hz
	// Baby-Orangutan:	1220.703125 Hz

	// Coreless motors do better with a higher PWM frequency since
	// their induction numbers are so low.  Trying a faster
	// PWM frequency (305 > 1220 Hz, or a factor of four):

	// Prescale of 256 is mode 4 for TIMER0 and mode 6 for TIMER2.

	_TCCRB = (_PRESCALE_MODE << CS00);

	// Start with interrupts off, we will enable them later when the 
	// user wants to spin a motor.

	// Set up all our motor control pins as outputs.  These defines
	// all live in the Orangutan-lib device.h file, and are set based
	// on what Orangutan device you're using:

	IN1_DDR |= (1 << IN1_BIT);
	IN2_DDR |= (1 << IN2_BIT);
	IN3_DDR |= (1 << IN3_BIT);
	IN4_DDR |= (1 << IN4_BIT);

	// Set both motors to zero speed:

	_OCRA = 0;
	_OCRB = 0;

	// Enable global interrupts:

	sei();

}

// Motor A uses IN1 and IN2
void pwm_a(int16_t speed)
{
	if (speed > 0)
	{
		// Set motor direction to forward:
		m_a = M_FWD;

		// Set our PWM duty cycle
		_OCRA = speed & 0xFF;

		// Interrupt on A compare match and overflow
		_TIMSK |= (1 << _OCIEA) | (1 << _TOIE);
	}	
	if (speed == 0)
	{
		// Set motor direction to stop:
		m_a = M_STOP;

		// Set outputs to zero to coast:
		IN1_PORT &= ~(1 << IN1_BIT);
		IN2_PORT &= ~(1 << IN2_BIT);

		// No PWM duty cycle to set

		// Turn off interrupt for A compare match
		_TIMSK &= ~(_OCIEA);

		// If both channels are off, disable overflow interrupt
		if(_TIMSK == _TOIE)
			_TIMSK = 0;
	}
	if (speed < 0)
	{
		// Set motor direction to reverse:
		m_a = M_REV;

		// Set our PWM duty cycle
		_OCRA = (-speed) & 0xFF;

		// Interrupt on A compare match and overflow
		_TIMSK |= (1 << _OCIEA) | (1 << _TOIE);
	}
}

// Motor B uses IN3 and IN4
void pwm_b(int16_t speed)
{
	if (speed > 0)
	{
		// Set motor direction to forward:
		m_b = M_FWD;

		// Set our PWM duty cycle
		_OCRB = speed & 0xFF;

		// Interrupt on B compare match and overflow
		_TIMSK |= (1 << _OCIEB) | (1 << _TOIE);
	}	
	if (speed == 0)
	{
		// Set motor direction to stop:
		m_b = M_STOP;

		// Set outputs to zero to coast:
		IN3_PORT &= ~(1 << IN3_BIT);
		IN4_PORT &= ~(1 << IN4_BIT);

		// No PWM duty cycle to set

		// Turn off interrupt for B compare match
		_TIMSK &= (1 << _OCIEB);

		// If both channels are off, disable overflow interrupt
		if(_TIMSK == _TOIE)
			_TIMSK = 0;
	}
	if (speed < 0)
	{
		// Set motor direction to reverse:
		m_b = M_REV;

		// Set our PWM duty cycle
		_OCRB = (-speed) & 0xFF;

		// Interrupt on B compare match and overflow
		_TIMSK |= (1 << _OCIEB) | (1 << _TOIE);
	}
}

void pwm_a_brake(void)
{
	// Set motor speed to zero (turns off PWM and disables interrupts):
	pwm_a(0);

	// Enable both IN1 and IN2 to hit the brakes (POWER HUNGRY):
	IN1_PORT |= (1 << IN1_BIT);
	IN2_PORT |= (1 << IN2_BIT);
}

void pwm_b_brake(void)
{
	// Set motor speed to zero (turns off PWM and disables interrupts):
	pwm_b(0);

	// Enable both IN3 and IN4 to hit the brakes (POWER HUNGRY):
	IN3_PORT |= (1 << IN3_BIT);
	IN4_PORT |= (1 << IN4_BIT);
}


// Interrupt Service Routines for Compare Match events:
//
// Each ISR turns off the bits for the associated motor based on 
// the  value of the global motor control flag for that channel:

ISR( _OUTPUT_COMPARE_A ) 
{

	// Turn off IN1 and IN2 (faster than deciding which to do):
	IN1_PORT &= ~(1 << IN1_BIT);
	IN2_PORT &= ~(1 << IN2_BIT);
}

ISR( _OUTPUT_COMPARE_B )
{

	// Turn off IN3 and IN4 (faster than deciding which to do):
	IN3_PORT &= ~(1 << IN3_BIT);
	IN4_PORT &= ~(1 << IN4_BIT);
}

// Interrupt Service Routine for Timer Overflow events:
//
// This ISR turns on the bits for both motors based on the value
// of the global motor control flag for that channel:

ISR( _OVERFLOW ) 
{

	// Handle the cases for Motor A:
	switch (m_a) {
		case M_FWD:
			// Turn on IN1:
			IN1_PORT |= (1 << IN1_BIT);
			break;
		case M_STOP:
			break;
		case M_REV:
			// Turn on IN2:
			IN2_PORT |= (1 << IN2_BIT);
			break;
	}

	// Handle the cases for MOTOR B:
	switch (m_b) {
		case M_FWD:
			// Turn on IN3:
			IN3_PORT |= (1 << IN3_BIT);
			break;
		case M_STOP:
			break;
		case M_REV:
			// Turn on IN4:
			IN4_PORT |= (1 << IN4_BIT);
			break;
	}
}

#endif // _ENABLE_LB1836M_MOTORS_
