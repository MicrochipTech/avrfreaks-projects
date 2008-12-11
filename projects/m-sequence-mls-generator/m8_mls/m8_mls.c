#include <avr/io.h>
#include <util/delay_basic.h>


#define OUTPIN 4
#define CLOCKPIN 5
#define SYNCHPIN 3
#define LFSR_START 0x0001

int main(void)
{

	register unsigned int lfsr = LFSR_START;
	const unsigned int feedback=0xb400;
	const uint8_t N_delay=18;

	DDRB = 0xFF;//port B all outputs

	PORTB=0x00;
	for (;;)
	{
		//calculate lfsr and set clock pin
		if (lfsr & 1)  
		{ 
			PORTB |= (1 << OUTPIN);//set output pin
			PORTB |= (1 << CLOCKPIN);//set pin
			lfsr = (lfsr >> 1) ^ feedback; 
		}
  		else        
		{ 
		    PORTB &= ~(1 << OUTPIN);//clear output pin
			PORTB |= (1 << CLOCKPIN);//set pin
			lfsr = (lfsr >> 1);
			asm volatile("nop\n\t"
             "nop\n\t"::);

		}

		if (lfsr==LFSR_START)
		{
			PORTB |= (1 << SYNCHPIN);//set pin
		}
		else
		{
			PORTB &= ~(1 << SYNCHPIN);//clear pin
			asm volatile("nop\n\t"::);
		}
		
		PORTB &= ~(1 << CLOCKPIN);//clear pin

		_delay_loop_1(N_delay);//delay for 3*N_delay cycles
	}
}
