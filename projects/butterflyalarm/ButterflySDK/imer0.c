/** @file timer0.c Timer0 driver. Timer0 is used for keeping the tempo during
	song/sound playing.
*/

#include <inttypes.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#include "c_ext.h"

/*******************************************************************************
* External Functions Declarations
*******************************************************************************/

extern void SoundISRTask(void);

/*******************************************************************************
* Global variables
*******************************************************************************/

/** Set to TRUE when a Timer0 interrupt occurs. It is cleared by an external
	module, when the interrupt is served. The external modules should avoid
	accessing it directly and use the supplied macros instead. */
volatile BOOL timer0InterruptFlag;

/*******************************************************************************
* Exported Functions
*******************************************************************************/

/** Starts the Timer0 timer. Timer0 Compare Interrupt is enabled and it will be
	triggered every 10ms.
*/
void Timer0Start(void)
{
	// Enable timer0 compare interrupt
	TIMSK0 = (1<<OCIE0A);

	// Sets the compare value
	OCR0A = 38;

	// Set Clear on Timer Compare (CTC) mode, CLK/256 prescaler
	TCCR0A = (1<<WGM01)|(0<<WGM00)|(4<<CS00);

	timer0InterruptFlag = FALSE;
}

/** Stops the Timer0 timer. */
void Timer0Stop(void)
{
	TIMSK0 = 0;
	TCCR0A = 0;
}

/*******************************************************************************
* Timer0 interrupt - Runs every 10ms
*******************************************************************************/

/** @fn ISR(TIMER0_COMP_vect)
	Timer0 Compare Interrupt. Runs every 10ms and keeps the tempo during
	sound/song playing.
*/
ISR(TIMER0_COMP_vect)
{
	timer0InterruptFlag = TRUE;
	SoundISRTask();
}

