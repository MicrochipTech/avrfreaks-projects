/*! \file timer1.c
 *	For soundprocessing, a fixed sampling frequency is needed.  This timer takes
 *	care of that.
 */
#include "define.h"

/*! A variable that is used as a warning.  This variable must be reset by the
 *the soundprocessing routine when it finishes.  When it is still true when the
 *timer1 interrupt occurs, the soundprocessing can't keep up with the sampling.
 *The timer1 interrupt will signal this.
 */
bool timer1_trigger;

/*! This ISR checks if the soundprocessing is still keeping up.  If it isn't 
 *	then PORTB will be set to 0xFF.  In my implementation PORTB is connected to
 *	eight leds.
 *	This ISR will also start a new AD-conversion.
 *	According to the datasheet, it's possible to trigger AD-conversions by 
 *	timers without using the ISR of that timer, but it didn't work.
 */
ISR(TIMER1_COMPA_vect){
	
	/*Timer_trigger should be false here.  If it is still true, this means the
	 *polling routine hasn't finished the job yet and is lacking behind.  Signal
	 *this by lighting the leds that are connected to PORTB.
	 */
	if(timer1_trigger)PORTB=0xFF;
	timer1_trigger=true;
	
	//Start a new conversion
	/*There must be a way that AD-conversions are triggered automatically when
	 *a timer overflows, or when timer1 reaches OCR1B, but it doesn't seem to
	 *work.
	 */
	ADCSRA|=(1<<ADSC);
}//interrupt on timer1 output compareA*/


/*!Initialize timer1 for CTC-operation on OC1A (i.e. clear the counter when it
 * reaches OC1A.
 * frequency settings: 	OCR1A = 499 ->	f=16kHz
 *						OCR1A = 249	->	f=32kHz
 */
void timer1_init(){
	//CTC-mode, top=OCR1A
	//No prescaling
	TCCR1B=(1<<WGM12)|(1<<CS10);
	OCR1A=249;//249 for 32KHz, when Fosc=16MHz and no prescaling
	//Enable interrupt on OCR1A-match (datasheet p.100)
	TIMSK|=(1<<OCIE1A);
}//timer1_init


