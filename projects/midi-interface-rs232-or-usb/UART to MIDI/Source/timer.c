// Timerfunctions for 32-bit software-timer, uses timer0 
//
// Author: L. Lisseck

#include <avr/io.h>
#include <avr/interrupt.h>
#include "hardware.h"
#include "timer.h"

// Initialization of the timersystem
void timer_init( void )
{
	// Turn off Timer0- und Output-Compare0 interrupt 
	TIMSK &= ~(_BV(TOIE0)); // | _BV(OCIE0));
	timer_value = (unsigned long)0;	// Set softwaretimer to 0
    
    // Set prescaler to clkIO/64
	#if defined(__AVR_ATmega128__) 
		TCCR0 = _BV(CS02);
  	#else
		TCCR0 = _BV(CS01) | _BV(CS00);	
  	#endif
	TCNT0 = 0;				// Set hardwaretimer to 0
	TIFR |= _BV(TOV0);		// clear Timer0-Interruptflag 
	TIMSK |= _BV(TOIE0);	// activate Timer0-Interrupt 
}

// Please look at timer.h for explanation in english 
/* Gibt die Zeitdifferenz zwischen zwei Timestamps zur�ck
 * Der zweite Wert sollte immer der vom sp�teren Zeitpunkt sein.
 * Ansonsten erh�lt man einen sehr gro�en falschen Wert.
 * Wenn aber der Timer zwischen den zwei Zeitpunkten
 * �bergelaufen ist, kommt trotzdem die richtige Zeitdifferenz herraus!	
 */ 
inline unsigned long timer_diff_time( unsigned long first_timepoint, unsigned long second_timepoint)
{
  return( second_timepoint - first_timepoint );
}

// Please look at timer.h for explanation in english
/*  Gibt true zur�ck, wenn die �bergebene Zeit abgelaufen ist. 
 *  Es muss ein auf 0 initialisiertes Handle �bergeben werden.
 *  Wenn true zur�ckgegeben wird, wird auch das Handle wieder auf 0 (=ung�ltig) gesetzt.
 *  Wenn die Funktion auf ein ung�ltiges Handle trifft, wird in dieses die aktuelle Zeit gespeichert
 *  und false zur�ck gegeben. So kann die Funktion immer wieder aufgerufen werden, ohne das das
 *  Handle neu initialisiert werden muss.	
 */
unsigned char timer_autodelay(unsigned long *handle, unsigned long time)
{
	unsigned char retval=0;
	if(!handle) return(0);
	if(*handle) {
	    // time elapsed?
		if(timer_elapsed_time(*handle)>time) {
			retval=!0;
			*handle = 0;
		}
	} else {
		// Write timestamp to handle if handle was set to 0
		*handle = timer_getvalue();
		// to avoid a very rare errorcondition...
		if(!(*handle)) *handle++;
	}
	return(retval);
}
// EOF

