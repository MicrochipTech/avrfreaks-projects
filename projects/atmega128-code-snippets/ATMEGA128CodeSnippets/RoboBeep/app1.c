#include <avr/io.h>
#include <avr/delay.h>

int main (void)
{
	unsigned char counter;
	
	//set PORTB for output
	DDRB = 0xFF;
	while (1)
	{
		//set PORTB.7 high
		PORTB |= 1<<7;
		//wait (10 * 120000) cycles = wait 1200000 cycles
		counter = 0;
		while (counter != 5)
		{
			//wait (30000 x 4) cycles = wait 120000 cycles
			_delay_loop_2(30000);
			counter++;
		}
		//set PORTB.7 low
		PORTB &= ~(1<<7);
		//wait (10 * 120000) cycles = wait 1200000 cycles
		counter = 0;
		while (counter != 5)
		{
			//wait (30000 x 4) cycles = wait 120000 cycles
			_delay_loop_2(30000);
			counter++;
		}
	}
	return 1;
}