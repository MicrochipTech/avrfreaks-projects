/******************************************************************************
RTC - Real-time clock using a 32,768 KHz quarz crystal
*
*Author: 	Jidan Al-Eryani,	jidan@gmx.net 
*Date: 	3.4.2007
*
*
Reference: AVR134 - Real-Time Clock using the Asynchronous Timer
*******************************************************************************/

#include <avr/io.h>
#include <avr/interrupt.h>

#define OC0_PIN PB4
#define LED PA0

volatile int seconds;

// This function is Interrupt service routine (ISR)
ISR(TIMER0_OVF_vect)     
{	
	if (seconds==59)
		seconds=0;
	else
		seconds++;
		
	PORTA ^= (1<<LED);
}

int main(void){

	//intialise LED pins
	DDRA |= (1<<LED);
	PORTA |= (1<<LED);
	
	// Not necesserly. Works also if OC0 pin is configured as input
	//DDRB |= (1<<OC0_PIN);
	seconds=0;
	
	TIMSK &=~((1<<TOIE0)|(1<<OCIE0)); // disable Timer0 interrupts
	ASSR |= (1<<AS0); // set external quartz crstal(32,768 KHz) as clock source
	TCNT0 = 0x00; //preset Timer0 register to 0
	TCCR0 |= (1<<CS02) | (1<<CS00); // set prescaler = CK/128, so that the period of overflow is 1 second
	
	while(ASSR&0x07);           //Wait until Timer0 is updated

	TIMSK |= (1<<TOIE0); //  enable Timer0 overflow interrupt

	sei(); //enable global interrupt

	while(1){
	}
	
	return 0;
}
