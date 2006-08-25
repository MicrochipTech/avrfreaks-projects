#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/signal.h>

#include "timer.h"
#include "event.h"

// tick happen every 10ms, 100 times per second

volatile uint8_t timerTicks, timerTicks5;

void timerInit(void)
{
	timerTicks = 0;
	timerTicks5 = 0;
#if defined (__AVR_ATmega168__)	
	TIMSK0 = _BV(TOIE0);				// authorise interrupts	
	TCCR0B = _BV(CS02);					// divide clock by 256
#elif defined (__AVR_ATmega8__)
	TIMSK = _BV(TOIE0);					// authorise interrupts
	TCCR0 = _BV(CS02);					// divide clock by 256
#else
#  error "Device not defined"
#endif 
}

SIGNAL (SIG_OVERFLOW0)
{
	//eventAddEvent(EVENT_TIME_10MS);
	timerTicks++;
	if (timerTicks < 5) return;
	eventAddEvent(EVENT_TIME_50MS);
	timerTicks = 0;
	timerTicks5 ++;
	if (timerTicks5 < 20) return;
	eventAddEvent(EVENT_TIME_SEC);
	timerTicks5 = 0;
}
