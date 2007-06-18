// counters.c

// The counter routines use the other half of TIMER1, the half that
// servo.c and servo.h aren't using.  For that reason the counter
// routines can't dink with the timer prescale value.  servo.c needs
// TIMER1 to run at full CPU speed, prescale of 1.  So the counters.c
// routines need to use TIMER1 the same way: at full CPU speed.

// All device-specific dependencies are stored in this file.  You must
// edit it first before using any of the library routines!

#include "device.h"

// As with almost all the servo code out there for the AVR processors,
// this is interrupt-based.  So it needs some additional includes
// that the other examples don't use.

#include <avr/io.h>
#include <avr/interrupt.h>

// Because there are some user-settable values in this subsystem's .h
// file, we also include it in the .c file as well:

#include "counter.h"

// Next we need two arrays of pointers to point to the timer variables 
// the user hands us, and a couple of variables we'll use in the
// interrupt routines:

volatile uint16_t *count_down[COUNTER_MAX], *count_up[COUNTER_MAX];
uint8_t num_down = 0, num_up = 0;
volatile uint8_t idx;

#define COUNTER_TIME_DIV	(F_CPU / COUNTER_DIVISOR)

// A routine to initialize the counter subsystem
void counter_init(void)
{
    // Set up our compare match for our time division (and hope it's
	// an integer so we don't get skewed timing!)
	OCR1B = TCNT1 + COUNTER_TIME_DIV;

	// Stop interrupts
	cli();

    // start timer 1 with no prescaler (same way servo.c uses it, so
	// if servo_init() has already done this, it doesn't hurt anything)
    TCCR1B = (1 << CS10);      

    // Enable interrupt for Output Compare on TIMER1B
    TIMSK1 |= (1 << OCIE1B);

	// Start interrupts
	sei();
}

// Register a count-down timer in the first available slot
void counter_define_down(uint16_t *timer)
{
	// As long as we're not at our max
	if(num_down < COUNTER_MAX)
	{
		// Store the counter in our table
		count_down[num_down] = timer;
		// Increment our count
		num_down += 1;
	}
}

// Register a count-up timer in the first available slot
void counter_define_up(uint16_t *timer)
{
	// As long as we're not at our max
	if(num_up < COUNTER_MAX)
	{
		// Store the counter in our table
		count_up[num_up] = timer;
		// Increment our count
		num_up += 1;
	}
}

// Interrupt handler for the TIMER1B compare match:
ISR(TIMER1_COMPB_vect)
{
	// Set the next counter compare value.  This works because
	// both OCR1B and TCNT1 (where the timer actually stores
	// its value) are both 16-bit numbers.  A roll-over on
	// OCR1B just means TCNT1 will roll over as well and keep
	// counting up until it hits OCR1B's value.  It's still
	// only going to be COUNTER_TIME_DIV counts regardless of whether
	// it rolls over or not.  (This is the same way the servo
	// code works.)
	OCR1B += COUNTER_TIME_DIV;

	// Run through each of our down-counters
	for(idx = 0; idx < num_down; idx++)
	{
		// If our countdown timers are above zero...
		if(*count_down[idx] > 0)
			// Decrement them by one
			--*count_down[idx];		
	}

	// Run through each of our up-counters
	for(idx = 0; idx < num_up; idx++)
	{
		// We assume (naively?) that the programmer is going to
		// do something with the timer before it rolls over
		++*count_up[idx];
	}
}


// That's it for the counter routines!
