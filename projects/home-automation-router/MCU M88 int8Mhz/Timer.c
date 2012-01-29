#include "Timer.h"
#include "USART.h"
#include <avr/interrupt.h>

#define Max_Delay_bw_Bytes	80 //based on 8uS timer, max 255
								//min 43 or won't work

//RX Byte overtime ~120uS

void init_Timers(void)
{
	TCCR0A	= 0b00000010;		//'A'register, CTC
	//TCCR0B	= 0b00000011;	//'B'register! Div:64
	//(in macro)
	OCR0A	= Max_Delay_bw_Bytes;	//For 8Mhz
	bit_set(TIMSK0,BIT(OCIE0A));
	
	//Start of timer + Prescaler settings in MACRO

}

ISR(TIMER0_COMPA_vect)	//RX timeout, Timer0 Compare A Handler
{
	RX_Over_Timed = 1;
}
