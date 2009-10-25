
#include <avr\io.h>
#include <avr\interrupt.h>
#include "Init.h"

void Init_all(void)
{
//PortInit();
UART_Init();


}
///////////////////////////////////////
////  PWM_Init	Initializing PWM system
//////////////////
void PortInit(void)
{
DDRD=(0<<PIND0)|(1<<PIND1)|(0<<PIND2)|(0<<PIND3)|(0<<PIND4)|(1<<PIND5)|(1<<PIND6)|(1<<PIND7); 



}





////UART_Init function   expects clock to be 8 MHz
////////////////////////////////////////////////////
void UART_Init(void)
{

// Baud Rate = 4800	
	UBRR0L=103;
	UBRR0H=0;

	// Set frame format: 8data, 1stop bit

UCSR0C=	(0<<USBS0)|(1<<UCSZ00)|(1<<UCSZ01); // Enable receiver and transmitter
UCSR0B=(1<<RXEN0)|(1<<RXCIE0)|(1<<TXEN0);




}
