/*!\file timer.c
 *Timer functions used for controlling the time-out of the communication:
 *You still have to fill in what to do when the message isn't sent
 *correctly (e.g. send the frame again, generate an error, ...).
 */


#include "define.h"


/*! ISR of Timer2, will be called every 5s when timer2 is enabled.
 */
INTERRUPT(SIG_OUTPUT_COMPARE2){
	static uint8_t teller=0;
	if(teller++<5)
		//retransmit message
		//??????????????????????????????????????????????
		write_USART_B(0xDD);
	else {
		//error
		stopTimer2();
	}
}//interrupt on timer2 output compare


/*! Initialize timer2, so that it will generate an interrupt after 5s.
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


/*!Stop timer2 by disconnecting clock source and disabling interrupts on 
 * this timer.
 */
void stopTimer2(void){
	TIMSK&=~(1<<OCIE2|1<<TOIE2);				//disable interrupts
	//Disconnect clock-source (CS22=CS21=CS20=0);
	TCCR2&=~((1<<CS22)|(1<<CS21)|(1<<CS20));
}//stopTimer2
