//	pin7 = 32768Hz precision  clock input
//	Assumes an ATTiny85 running on the internal 8MHz oscillator!


#include <inttypes.h>
#include <avr/io.h>
#include <avr/interrupt.h>



//global variables

volatile uint8_t	high_byte;				//overflow counter

int main (void)
{

	
	uint8_t direction = 0;	//0 = unset, 1 = initially too high, 2 = initially too low
	uint8_t complete = 0;
	uint16_t clocks;		// this will be the elapsed time in CPU clocks

	while (!complete)
	{
		high_byte = 0;
		TIFR = 0xff;	// clear all interrupt flags
		TCNT0 = 0;
		TCNT1 = 0;
		TIMSK = 0;
		TCCR0A = 0x02;	// set mode to CTC (2)
		OCR0A = 209;	//set to wait for 209+1 external clock transitions



		TCCR0B = 0x07;	//enable clock input to rising edge on external clock source
		while (!(TIFR & (1<<OCF0A)));  //wait for the interrupt flag to get set

		//start counting on T/C1

		TCCR1 = 1;		//start timer1 counting mode 0, overflow interrupts
	
		TIMSK |=(1<<TOIE1);	//enable  the T/C1 overflow interrupt
		TIFR |= (1<<OCF0A);	//clear the flag
		sei();				//enable all interrupts

		while (!(TIFR & (1<<OCF0A)));  //wait for the interrupt flag to get set

		TCCR1 = 0;			//turn off timer1 immediately.  Maybe 4 clocks latency?
		TCCR0B = 0;			//turn off timer 0
		cli();
	
		clocks = ((uint16_t)high_byte << 8) + (uint16_t)TCNT1;

		if (clocks > 51369)		//it was too slow!
		{
			OSCCAL--;
			if (direction == 0) //if direction is not set then set it
			{
				direction = 1;
			}
			else if (direction ==2)	//if it changed sides then we are done
			{
				complete = 1;
			}
		}

		else if (clocks < 51169)	//It was too fast!
		{
			OSCCAL++;
			if (direction == 0) //if direction is not set then set it
			{
				direction = 2;
			}
			else if (direction ==1)	//if it changed sides then we are done
			{
				complete = 1;
			}
		}

		else complete = 1;	//there is a +/-100 clock cycle window of "acceptable"

	}		

	// Write the OSCCAL byte to the top of EEPROM.
	// The next-to-last is a signature value (0xA5).
	// When reading the OSCCAL value from EEPROM, first check
	// that the next-to-last character actually is the 0xA5 value. 1:256 chance it is not an OSCCAL value
	// It would not be a bad idea to do some simple bounds checking of the
	// OSCCAL value that is read just to be extra sure it is real

	EEAR = E2END-1;		//set address to next to last byte
	EEDR = 0xA5;		//use a specific bit pattern to signify that the next byte is the OSCCAL
	EECR |= (1<<EEMPE);	//enable the write
	EECR |= (1<<EEPE);	//perform the write

	while ((EECR & (1<<EEPE)));	//wait for the write to complete

	EEAR = E2END;		//set address to last byte
	EEDR = OSCCAL;		//Load the data (OSCCAL)
	EECR |= (1<<EEMPE);	//enable the write
	EECR |= (1<<EEPE);	//perform the write

	while ((EECR & (1<<EEPE)));	//wait for the write to complete

	while (1);			//enter a loop
}


ISR (TIM1_OVF_vect)
{
	high_byte++;
}
