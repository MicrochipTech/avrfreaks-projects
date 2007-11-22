/*
	noise.c - LFSR noise generator on ATTiny13.
	Written by Frank Kienast in November, 2007
*/

#include <avr/io.h>

#define DELAY 100

int main()
{
	uint32_t val = 0xabcd;
	volatile uint16_t delay;
	
	DDRB = _BV(PB0); 


	while(1)
	{
		uint8_t bit1, bit2, bit3, bit4, bit;
		bit1 = ((val & (1UL << 0)) != 0 ? 1 : 0);
		bit2 = ((val & (1UL << 28)) != 0 ? 1 : 0);
		bit3 = ((val & (1UL << 30)) != 0 ? 1 : 0);
		bit4 = ((val & (1UL << 31)) != 0 ? 1 : 0);
		bit = bit1 ^ bit2 ^ bit3 ^ bit4;
		val = (val << 1) | bit;

		PORTB = bit;


		for(delay = DELAY; delay; delay--);
	}
		
}
