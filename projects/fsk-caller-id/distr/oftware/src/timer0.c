/*! \file timer0.c
 *  Routines to make use of timer0
 */

#include "define.h"

#define MAX_TIMERS	1	//!< Maximum number of software timers possible

uint8_t timers[MAX_TIMERS];//!< Array containing the software timers

/*! Function to set up the software timers.
 *\param index	Number of the software timer to setup
 *\param val	Initial countdown value to write to the timer
 */
void setTimer0(uint8_t index, uint8_t val){
	if(index<MAX_TIMERS)timers[index]=val;
}//setTimer

/*! ISR of timer0.  
 * This ISR decreases the software timers
 */
INTERRUPT(SIG_OUTPUT_COMPARE0){	
	uint8_t i=MAX_TIMERS-1;
	do{
		if(timers[i])timers[i]--;
	}while(i--);
}//interrupt on timer0 output compareA


/*! Poll timer to see if it has run out.
 *\param index of the timer to check
 *\return TRUE when timer has run out, else FALSE
 */
BOOL timer0isElapsed(uint8_t index){
	if(index<MAX_TIMERS)
		return !timers[index];
	else
		return TRUE;
}//timer0isElapsed


//! Initialize timer0 so that an interrupt occurs every 33ms.
void timer0_init(){
	//clear global interrupts
	cli();
	//set CTC-mode (Clear timer on compare match)
	TCCR0=(1<<WGM01)|(0<<WGM00);
	//COM01 and COM00 remain cleared, OC0 is disconnected
	//select clock source (/1024 prescaler)
	TCCR0|=(1<<CS02)|(1<<CS00);
	//set output compare register
	OCR0=255;//30Hz
	//enable output compare match interrupt
	TIMSK|=(1<<OCIE0);
	//enable global interrupts
	sei();
}//timer0_init

