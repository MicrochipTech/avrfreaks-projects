/*!\file timer2.c
 *Functions for controlling timer2.  NeoRS232-routines use timer2 as a
 *timeout-timer with a period of five seconds.
 */
#define NEED_USART
#define NEED_NEORS232
#include "define.h"

void initTimer2(void);
void stopTimer2(void);

/*!This ISR-routine is called when the value in timer2 reaches OCR2.
 *This happens normally after five seconds.  In good communications the
 *timeout doesn't occur.  When the acknowledge is not received within
 *five seconds, this function is called for the first time. A retransmit
 *function (implemented elsewhere) is called when timeouts occur.  After
 *25 seconds, i.e. after five timeouts, the sender gives up.  A function
 *is called to let the other functions know that no data has been 
 *received.
 */
INTERRUPT(SIG_OUTPUT_COMPARE2){
	static byte teller=0;
	if(teller++<5)retransmit();
	else {
		//error
		stopTimer2();
		noAnswer();
	}
}//interrupt on timer2 output compare

/*!This routine sets timer2.
 *Interrupt when timer2 register=OCR2,
 *enable interrupts on timer2
 *set period=5s
 */
void initTimer2(void){
	//disable timer2 interrupts
	TIMSK&=~((1<<OCIE2)|(1<<TOIE2));
	//clock timer2 from external 32.768kHz crystal
	ASSR|=(1<<AS2);
	//reset timer2 register
	TCNT2=0;
	/*set output compare register - OCR2
		fosc=32.768kHz/1024prescaler=32Hz
		time-output-Compare=160/fosc=160/32=5s
	*/
	OCR2=160;
	/*set timer2 control register - TCCR2
		clear timer2 when TCNT2=OCR2
		set clock source (/1024 prescaler)
	*/
	TCCR2|= (1<<WGM21) | (1<<CS22)|(1<<CS21)|(1<<CS20);
	//wait until TCNT2, OCR2 & TCCR2 are updated
	while(ASSR&((1<<TCN2UB)|(1<<OCR2UB)|(1<<TCR2UB)));
	//clear the timer2 interrupt flags
	TIFR&=~((1<<OCF2)|(1<<TOV2));
	//enable interrupt on output compare
	TIMSK|=(1<<OCIE2);
	//enable global interrupts
	sei();
}//initTimer2

/*!Stop timer2 (disable IRQ)
 */
void stopTimer2(void){
	//disable timer2 interrupts
	TIMSK&=~(1<<OCIE2|1<<TOIE2);				
	//Disconnect clock-source (CS22=CS21=CS20=0);
	TCCR2&=~((1<<CS22)|(1<<CS21)|(1<<CS20));
}//stopTimer2

