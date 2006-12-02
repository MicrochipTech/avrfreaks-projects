#include <avr/io.h>
#include <avr/delay.h>

int main (void)
{
	unsigned char counter;
	
	//set PORTE for output
	DDRE = 0xFF;
	PORTE = 0x18;
	
	// Move robot FORWARD
	counter = 0;
	PORTE = 0x78;
	while (counter != 20)
		{
			//wait (30000 x 4) cycles = wait 120000 cycles
			_delay_loop_2(30000);
			counter++;
		}
		
	// Move robot BACKWARD	
	counter = 0;
	PORTE = 0x9C;
	while (counter != 20)
		{
			//wait (30000 x 4) cycles = wait 120000 cycles
			_delay_loop_2(30000);
			counter++;
		}
	
	// Turn robot RIGHT
	counter = 0;
	PORTE = 0xB8;
	while (counter != 20)
		{
			//wait (30000 x 4) cycles = wait 120000 cycles
			_delay_loop_2(30000);
			counter++;
		}
	
	// Turn robot LEFT
	counter = 0;
	PORTE = 0x5C;
	while (counter != 20)
		{
			//wait (30000 x 4) cycles = wait 120000 cycles
			_delay_loop_2(30000);
			counter++;
		}
	
	PORTE = 0x18;

	return 1;
}