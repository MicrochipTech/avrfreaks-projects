//This program converts an analog 0-5V signal on PINA0 into a number between 0 and 255 (ADCH) and writes the number to BORTB.
//BORTB can be connected to LEDs to see the input binary.

#include <avr/io.h>
#include <avr/interrupt.h>

unsigned char temp = 0;

void ioinit (void)		//Startup routine
{
	//Analog input setup
	ADMUX = 0b00100000;	//See datasheet PINA0 is used for input
	ADCSR = 0b11101010;	//See datasheet
	DDRB = 0b11111111;	//Makes PORTB a output
	sei();			//Enable global interrupt
}
ISR(ADC_vect)			//Interrupt subrutine, this sub runs automaticly every time ADC conversion is complete
{
PORTB = ADCH;			//Wiews the converted value in BORTB
}

int main(void)
{
	ioinit();		//Run the ioinit function at startup
	while(1)
		{
				//Main loop
		}
}

//This program is written of a farfmer from Norway without Copyright