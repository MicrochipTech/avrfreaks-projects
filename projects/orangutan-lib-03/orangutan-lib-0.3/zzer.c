// buzzer
//
// Courtesy of Jim Remington
// Comments by Tom Benedict

// The Orangutan and Orangutan-X2 both have piezo buzzers built into
// them.  Jim Remington has written some cool code to play music out
// the speaker using TIMER2 to provide both frequency control and
// note duration.  This code is posted on the Pololu Forum.
//
// Jim also wrote a very lightweight routine that will simply play a
// given frequency (in Hz) for a given duration (in ms).  The trade-
// off is that unlike his TIMER2 based routines, when the note is
// being played, the CPU is tied up.  For some applications this is
// more than acceptable, and it means TIMER2 is still available for
// other purposes.
//
// The neatest part of Jim's code is the math that determines how
// many clock cycles to wait between turning the buzzer's I/O line on
// and off (note frequency) and how many clock cycles to play the
// note (duration).  The resulting equations are very small.
//
// So let's get to it!

// All device-specific dependencies are stored in this file.  You must
// edit it first before using any of the library routines!

#include "device.h"

// Only include this code if we have an Orangutan device that has a
// buzzer attached (set in "device.h").

#ifdef _ENABLE_BUZZER_

#include <avr/io.h>

// Unlike most of the other library code, the buzzer relies heavily 
// on delays.  So it's required to set F_CPU and include 
// <util/delay.h> for the buzzer code to work at all.  F_CPU is set 
// in "device.h".

#include <util/delay.h>


// buzzer() plays a tone with the frequency in Hz and the duration
// in milliseconds.

void buzzer(uint16_t frequency, uint16_t duration)
{
	uint16_t wait_units, loops, i;

	// Set the buzzer as an output (Putting this here saves having 
	// to call an initialization routine!)
	BUZZER_DDR |= (1 << BUZZER_BIT);

	// Calculate how long we need to wait between turning the buzzer
	// on and off.  This should be half of the tone frequency, 
	// expressed in delay loop units.
	wait_units = F_CPU / (8UL * (uint32_t) frequency);

	// Calculate the number of loops to execute in order to hit the
	// duration asked for.
	loops = duration * F_CPU / (8000UL * wait_units);
	
	// The loop
	for (i=1; i<=loops; i++) 
	{
		// Turn the buzzer on
		BUZZER_PORT |= (1 << BUZZER_BIT);

		// Wait 1/2 tone period
		_delay_loop_2(wait_units);

		// Turn the buzzer off
		BUZZER_PORT &= ~(1 << BUZZER_BIT);
		
		// Wait 1/2 tone period
		_delay_loop_2(wait_units);
		}
}

#endif // _ENABLE_BUZZER_
