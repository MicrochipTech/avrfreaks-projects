///////////////////////////////////////////////////////////////////////////////
//	Low pin count seven-segment display. Demo application.
//	Version 1.0
//		(c) 2011 sashman@gmail.com
///////////////////////////////////////////////////////////////////////////////
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <string.h>
#include "lpc7seg.h"
#include "misc.h"
///////////////////////////////////////////////////////////////////////////////
ISR(SIG_OVERFLOW0)
{
	sevenseg_refresh();

	
	//the  block below implements fading. Not required.

	static int direction = 1, a=0;
	if(a++>50) 
	{
		
		s7brightness += direction;
		if(s7brightness == 0 || s7brightness > S7_DELAY_US) 
			direction=-direction;
		a=0;
	}

}
///////////////////////////////////////////////////////////////////////////////
int main()
{
	sevenseg_init();
	strncpy((char*)s7cbuf, "BOOBIES", S7N); //note: uppercase only
	sevenseg_update_fbuf();

	//enable timer overflow interrupt
	#ifdef TIMSK	// Mega16 etc
		TIMSK = _BV(TOIE0);
		TCCR0 = _BV(CS01)|_BV(CS00); //interrupts at F_CPU/64/256
	#else			// Mega88 etc
		TIMSK0 = _BV(TOIE0);
		TCCR0B = _BV(CS01)|_BV(CS00); 
	#endif
	sei();

	for(;;);
}
///////////////////////////////////////////////////////////////////////////////
