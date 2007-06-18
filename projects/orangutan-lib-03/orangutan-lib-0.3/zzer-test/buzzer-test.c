// buzzer-test
//
// Tom Benedict
//
// Use Jim Remington's lightweight buzzer code to play tones
// out the Orangutan's speaker.  Since the Baby-O doesn't have
// a buzzer, and since the X2's buzzer is controlled through
// SPI routines, this only works on the Orangutan.

// Include our I/O definitions
#include <avr/io.h>

// Include information about the device we're building for
#include "device.h"

// Include for the buzzer subsystem
#include "buzzer.h"

// And now for our main routine:
int main(void)
{
	for(;;)
	{
#ifdef _ENABLE_BUZZER_
		buzzer(440, 1000); // Middle C
		buzzer(220, 1000); // Low C
		buzzer(880, 1000); // High C
#endif // _ENABLE_BUZZER_
	}

	return(0);
}
