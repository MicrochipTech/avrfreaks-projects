#include <avr/io.h>

void ioinit (void)
{
	// Enables both PWM timers, PINB0 is PINB1 inverted, and PINB2 is PINB3 inverted
	TCCR1A = 0b01010011;
	// Prescale factor, see datasheet
	TCCR1B = 0b00000001;

	// Choose PWM value from 0 to 255
	OCR1A = 10;
	OCR1B = 250;

	// Enable PORTB0 to 4 as output
	DDRB = 0x0F;
	//PWM output is running
}

int main(void)
{
	ioinit();	//Run the ioinit function at startup
	while(1)
		{
			//Main loop
		}
}