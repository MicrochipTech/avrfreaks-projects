
#include <avr/io.h>
#include <avr/delay.h>

unsigned int count = 0xFF; // Binary counter, set to 0xFF (LED's Off).

extern void AVRInit_InputPort();
extern void AVRInit_OutputPort();


int main (void)
{
	AVRInit_InputPort();
	AVRInit_OutputPort();
	
	while (1)
	{
		if (count != 0x00) 			// Check to see if counter is not 0x00.
		{							// Count was initilized to 0xFF.
			SetPort(count);
			_delay_loop_2(50000); 	// Delay for 500ms.
			count = count - 1;		// Decrement counter.	
			_delay_loop_2(50000);	// Delay for aditional 500ms.
		}
		else						// If count is equal to 0x00 reset to 0xFF.
		{
			count = 0xFF;
		}
	}

	return 1;	// Program should never get here.

}

