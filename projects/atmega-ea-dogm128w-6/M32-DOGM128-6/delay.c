// Funkcia Delay sa pouziva na casove oneskorenie programu, pouzity timer1
#include "global.h"
/// Typedefs //////////

void InitDelay(void)
{
outb(TCCR1B,(0<<CS12)|(0<<CS11)|(1<<CS10));
outb(TCNT1H,0xFC);		// cas na 100us
outb(TCNT1L,0xE0);		// cas na 100us
}

void Delay(unsigned int time)
{
while (time>0)
	{
	outb(TCNT1H,0xFC);		// cas na 0,1s
	outb(TCNT1L,0xE0);		// cas na 0,1s
	while(!bit_is_set(TIFR,TOV1)); // caka na koniec
	sbi(TIFR,TOV1);			// nuluje priznak
	time-=1;
	}
}
