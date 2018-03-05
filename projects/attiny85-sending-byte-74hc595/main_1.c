/*
 * Final Attiny85-74hc595.c
 *Attiny85	74hc595
 *pin5	-	pin11	PINB0	SHIFT
 *pin6	-	pin12	PINB1	LATCH
 *pin7	-	pin14	PINB2	DATA		
 *	
 */ 
#define RAND_MAX 255;



#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>

uint8_t sendbyte = 0xF1;			//Value to send to 74hc595
void latch74hc595(void){
	PORTB	|=  (1<<PINB1);
	_delay_ms(1);
	PORTB	&= ~(1<<PINB1);

}
void shiftin74hc595(uint8_t bit8value){
	PORTB	=	0;
	int n = 7;
	while(n>=0){
		if (bit8value & (1<<(7-n)))
		{
			PORTB	|=	(1<<PINB2);
			}else{
			PORTB	&= ~(1<<PINB2);
		}
		_delay_ms(1);
		PORTB	|=	(1<<PINB0);
		_delay_ms(1);
		PORTB	&= ~(1<<PINB0);
		n--;
	}
	
}
int main(void)
{
	//Configure pins
	DDRB	|=	(1<<PINB0)|(1<<PINB1)|(1<<PINB2);
	PORTB	&= ~(1<<PINB0);
    while (1) 
    {
		//sendbyte = rand();			//Make sendbyte a random number
		//sendbyte++;					//Make sendbyte count in binary;
		shiftin74hc595(sendbyte);
		_delay_ms(1);
		latch74hc595();
		_delay_ms(1);
    }
}



