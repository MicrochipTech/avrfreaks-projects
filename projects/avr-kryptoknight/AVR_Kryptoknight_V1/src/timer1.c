/*!\file timer1.c
 *Here some routines are defined to control timer1
 */
#include "define.h"

/*!Interrupt service routine of input capture on timer1 generating
 *32-bit random-numbers.
 *When a falling edge on ICP1 occurs, the value of timer1 is copied
 *to the ICR1 register and this signal input capture interrupt is 
 *triggered.
 *This ISR then resets timer1.  The result is that ICR1 always
 *contains the time between two input capture interrupts, i.e. the 
 *period. Period N, is compared to the period N-2 of two interrupts 
 *ago. If period N-2 < period N then shift in a one in our random dword,
 *otherwise shift in a zero.
 *After 32 bits are shift in, the waittimer1-flag is set.
 *The ICP-pin of the microcontroller is connected to a simple astable
 *multivibrator, consisting of two NOT-ports, two resistors and two
 *capacitors.  The small variations in the period of such a square-wave
 *generator are the basis for the randomness of this hardware random
 *number generator.
 */
SIGNAL(SIG_INPUT_CAPTURE1){
	/*Hardware RNG must be connected to PD6!
	 */
	extern dword random;	//32-bit random number
	extern FLAGS flags;	//contains some flags (see define.h)
	
	//reset timer1
	TCNT1=0x0000;
	
	static word 			//initialize all periods at zero
		prevperiod=0,
		prev2period=0,
		currperiod=0;
	static byte counter=0;//start counting from zero
	//shift the periods
	prev2period=prevperiod;
	prevperiod=currperiod;
	currperiod=ICR1;
	/*First 2 interrupts must be ignored, because prev2period
	 *is then still zero.  So we wait until prev2period is not
	 *zero anymore.
	 *When we have a 32-bit value, no more bits are shift in until
	 *the waittimer1-flag is reset.  This resetting is done by the
	 *routine reading the random number.
	 */
	if(prev2period!=0 
			&& prev2period!=currperiod 
			&& (!flags.waittimer1)){
		random<<=1;
		if(prev2period<currperiod)random|=0x01;
		if((counter++)==32){
			flags.waittimer1=1;
			counter=0;
		}
	}
}//interrupt routine of input capture 1

/*!Activate timer1: normal operation (no PWM), interrupts on falling
 *edge of IP1, IRQ on input capture
 */
void timer1_Activate(void){
	//clear global interrupts
	cli();
	//Normal operation, OC1A/OC1B disconnected, no PWM
	TCCR1A=0;
	/*falling edge triggers ICP1
	 *timer mode 0
	 *clock source = no prescaling
	 */
	TCCR1B|=(1<<CS10);
	//enable interrupts of timer1 on input capture
	TIMSK|=(1<<TICIE1);
	//set global interrupts
	sei();
}//timer1_Activate

//!Disable timer1 input capture interrupts
void timer1_DeActivate(void){
	cli();
	TCCR1B&=~(1<<CS10);
	TIMSK&=~(1<<TICIE1);
	sei();
}


