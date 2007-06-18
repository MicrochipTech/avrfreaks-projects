// servo-only
//
// Tom Benedict


// One easy source of gear motors for robotics projects is the 
// multitude of servos designed for radio controlled airplanes,
// boats, and cars.  Because of the high volume of production,
// RC servos are typically inexpensive, are available with a wide
// variety of torques, speeds, and sizes, and almost all use the 
// same one-pin interface.
//
// For a robotics project involving a great number of servos, by
// far the most cost-effective method of driving them is to get a
// commercial serial servo controller.  They are inexpensive,
// dedicated pieces of hardware that can drive eight, sixteen, and
// in at least one case, thirty two servos at once.
//
// But for small robotics projects, needing only a handful of servos,
// it's almost always easier and cheaper to include a little more 
// code and just drive the servos off of the existing microcontroller.
//
// The code presented here is not the cleanest or the prettiest way
// to drive servos with a microprocessor.  In the case of a dedicated
// device, the programmer typically knows what pin each servo is going
// to be connected to, so a number of short-cuts can be taken to make
// the code smaller, faster, and better.
//
// This code started off as an example posted to the AVRFreaks forums,
// which was then pushed, pulled, and twisted into something a lot more
// generic, and, unfortunately, a lot harder to read.
//
// Of the example code, this one is likely to be the least readable,
// and the least likely to be copied, pasted, and used somewhere 
// else.  If you're interested in writing a robot program to drive 
// servos, do yourself a favor and just use the servo.c and servo.h 
// code from Orangutan-lib.  It's the same code as is presented here,
// but that way you won't have to look at it.
//
// All that aside, it does work.  So let's get to it!
//
// Tom Benedict


// All device-specific dependencies are stored in this file.  You must
// edit it first before using any of the library routines!

#include "device.h"

// As with almost all the servo code out there for the AVR processors,
// this is interrupt-based.  So it needs some additional includes
// that the other examples don't use.

#include <stdbool.h>
#include <avr/io.h>
#include <avr/interrupt.h>

// Unlike most of the other library code, this one includes its own
// .h file.  The reason for this is that there are #defines
// describing timing values on servos stored in there.  Both the
// routines in here and the user need to know what these timing
// values are!

#include "servo.h"


// The original code assumed you'd dedicate an entire port to servo
// operation.  If you're building a device around an AVR processor,
// this is the most sane approach to take.  If you're using the 
// Baby-Orangutan, this is still a sane approach to take since the
// entire compliment of I/O pins from both PORTD and PORTC are brought
// out to headers.
//
// But on the Orangutan the I/O pins brought out to headers span
// several I/O registers, and with the exception of PC0-PC3, aren't
// really grouped in contiguous blocks.  Since PC0-PC3 also correspond
// to ADC0 - ADC3, tying up analog lines for servo operation didn't
// sound like a reasonable approach to take.
//
// So the code was changed to make servo assignment more generic,
// similar to how Procyon AVRlib does things.  You register a servo
// with the servo system, defining I/O pin, port registers, etc.
// At that point it's serviced by the servo system, and you can set
// its position on the fly.


// For starters we need a struct type to store the information for
// each servo being serviced by the system:

typedef struct {
	uint8_t 	port;
	uint8_t 	mask;
	uint16_t 	time;
	uint8_t		active;
} servo_type;


// Next we use the servo timing #defines in servo.h to set up
// our internal timer values:

// Time slot available for each servo, in microseconds.  This
// basically takes the available time (20ms) and divides it up
// by the max number of allowed servos.

#define SERVO_TIME_DIV (SERVO_FRAME / MAX_SERVOS)

// Safety check to do at compile time.  If either of these warnings
// show up, things need to be tweaked.  (Thanks to the original author
// of the code on AVRFreaks!  This snippet is a neat sanity check!)

#if (SERVO_TIME_DIV < SERVO_MAX + 50)
#warning "Output fewer servo signals or increase SERVO_FRAME"
#endif
#if ((SERVO_TIME_DIV * (F_CPU / 1000000UL)) >= 0xFF00)
#warning "Output more servo signals or decrease SERVO_FRAME (or use the prescaler)"
#endif

// Convert microseconds to timer ticks:
// This works with even F_CPU numbers (1, 2, 4, 8, 20, etc.)  But it
// needs to be an integer operation.

#define US2TIMER1(us) ((us) * (uint16_t)(F_CPU / 1E6))


// The servos do have some global variables:

// An array to store our servo information:

volatile servo_type servo[MAX_SERVOS];

// And a variable to keep track of how many live servos we have:

volatile uint8_t num_servos = 0;


// Servo Routines

// servo_init()
//
// Initializes the servo subsystem.  The idea here is to set up 
// TIMER1A to run at full CPU speed and to set an interrupt on output
// compare.
//
// The routine also goes through the array of servo data, makes sure
// they're all set to be inactive, sets their default positions to
// midline (a safety measure), and sets their bitmask to zero to make
// sure that even if something goes awry and an undefined servo begins
// to be serviced, it doesn't actually do anything.

void servo_init(void)
{
	uint8_t i;

	// Stop interrupts
	cli();

    // Setupt a first compare match
    OCR1A = TCNT1 + US2TIMER1(100);
    // start timer 1 with no prescaler
    TCCR1B = (1 << CS10);      
    // Enable interrupt
    TIMSK1 |= (1 << OCIE1A);

	// Zero out all our values
	for(i = 0; i < MAX_SERVOS; i++)
	{
		// Make sure it's off!
		servo[i].active = 0;
		// Set position to midline;
		servo[i].time = US2TIMER1(SERVO_MID);
		// Set the bitmask to 0;
		servo[i].mask = 0;
	}

	// Start interrupts
	sei();
}


// servo_define()
//
// This is a fairly simple routine that's unfortunately somewhat
// convoluted to use.  The idea is to pass it the data direction
// register, output port register, and bit number for a servo,
// and to get back a number that represents the servo for all 
// the other servo routines.  
//
// The unfortunate part is that you can't simply hand DDR and
// PORT registers to other routines and have things work out
// right.  To do that you need to use an AVR command called
// _SFR_IO_ADDR() to reference each of the registers.
//
// For example, to define two servos on PD6 and PD7, called 
// servo_left and servo_right respectively, you'd do:
//
//		uint8_t left, right;
//
//		left = servo_define(_SFR_IO_ADDR(DDRD), _SFR_IO_ADDR(PORTD), 6);
//		right = servo_define(_SFR_IO_ADDR(DDRD), _SFR_IO_ADDR(PORTD), 7);
//
// The nice thing is, once that's done you can use all the other
// servo routines using the servo_left and servo_right variables:
//
//		servo_set(left, 1500);
//		servo_set(right, 2000);
//
// You get the idea...

uint8_t servo_define(uint8_t ddr, uint8_t port, uint8_t bit)
{
	// Make sure we're not out of space in our table
	if(num_servos < MAX_SERVOS)
	{
		// Set the servo's I/O pin as an output
		_SFR_IO8(ddr) |= (1 << bit);

		// Store port and pin information
		servo[num_servos].port = port;
		servo[num_servos].mask = (1 << bit);

		// Set the servo's position at midline;
		servo[num_servos].time = SERVO_MID;

		// Increment our servo counter
		num_servos++;

		// Return the servo number of the one we just defined
		return (num_servos - 1);

	} else {

		// Return zero and hope the user traps this!
		return 0;
	}
}


// servo_active() / servo_inactive()
//
// Once they're defined, servos start off life inactive.  You need
// to turn them on using servo_active().  Once on, they can be 
// turned back off by using servo_inactive().
//
// The reason for this is that many of the servo applications in
// robotics tend to leave servos off for reasons of power management
// or to allow servos modified for continuous rotation to be able to
// coast.  If it's not a feature you're interested in, define the
// servo, activate it, and forget about it.

void servo_active(uint8_t servo_num)
{
	if(servo_num <= num_servos)
	{
		servo[servo_num].active = 1;
	}
}

void servo_inactive(uint8_t servo_num)
{
	if(servo_num <= num_servos)
	{
		servo[servo_num].active = 0;
	}
}


// servo_set()
//
// servo_set() assigns a pulse length to a given servo that you've
// already defined.  This time is in microseconds, and must lie
// between SERVO_MIN and SERVO_MAX, above.
//
// If you want to set up some routine to do scaling, say from
// -100 to 100, and pass the appropriate value to servo_set(), by
// all means do so.  For servos modified for continuous rotation,
// this is a good way to go.  It's also a good way to go for servos
// that need to be able to be positioned in degrees.  Or radians.
// Or binary radians.  Or if you have a servo turning a leadscrew,
// you may want a routine that scales to linear units of measure.
//
// But you get the idea...  Servos talk microseconds of pulse
// duration.  How you use that is up to you.

void servo_set(uint8_t servo_num, uint16_t time)
{
	// Make sure the value is in range (not doing range checking on
	// a servo can strip gears!!)
	if (time < SERVO_MIN)
		time = SERVO_MIN;

	if (time > SERVO_MAX)
		time = SERVO_MAX;

	// Stop interrupts
	cli();
	// Set the servo's time
	servo[servo_num].time = US2TIMER1(time);
	// Start interrupts
	sei();
}



// Interrupt Service Routine
//
// This is the code that gets run on the output compare for OCR1A.
// It's what actually turns on and off the I/O pins driving each
// of the servos.
//
// The idea is this:  If a servo is turning on, turn on the I/O
// pin and set your output compare value to whatever the last
// output compare time was plus the number of clock pulses necessary
// to let the clock run for the pulse duration that servo needs.
// Also calculate what the output compare should be set to for
// the next servo to start its turn.
//
// When that time has expired, the I/O pin is turned off, and
// the next output compare value is set to the start time for
// the next servo in line.
//
// Because all the timer variables, including OCR1A are sixteen
// bit numbers, overflows are handled automatically.

ISR(TIMER1_COMPA_vect)
{
	static uint16_t next_start;
	static uint8_t servo_num;
	static bool output_high = true;
	uint16_t currentTime = OCR1A;
   
	if (output_high) {

		// If this is a servo that's active...
		if (servo[servo_num].active >> 0)
		{
			// Turn on the I/O line
       		_SFR_IO8(servo[servo_num].port) |= servo[servo_num].mask;
		}

		// Set the end time for the servo pulse
		OCR1A = currentTime + servo[servo_num].time;

		// Define the next start time
		next_start = currentTime + US2TIMER1(SERVO_TIME_DIV);

	} else {

		// If this is a servo that's active...
		if (servo[servo_num].active >> 0)
		{
			// Turn off the I/O line
			_SFR_IO8(servo[servo_num].port) &= ~(servo[servo_num].mask);
		}

		// Increment the servo counter and wrap around if we need to
        if (++servo_num == MAX_SERVOS) {
            servo_num = 0;
        }

		// Set the start time for the next servo pulse
		OCR1A = next_start;
	}

	// Toggle the state change for the next cycle
	output_high = !output_high;
}


// That's it for the servo stuff.


