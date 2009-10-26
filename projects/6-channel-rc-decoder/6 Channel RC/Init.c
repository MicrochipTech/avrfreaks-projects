
#include <avr\io.h>
#include <avr\interrupt.h>
#include "Init.h"

void Init_all(void)
{

PortInit();
UART_Init();
Counter_Timer2_Init();
PCINTInit();


}
///////////////////////////////////////
////  PWM_Init	Initializing PWM system
//////////////////
void PortInit(void)
{
DDRC=0; //all set as inputs 
DDRB=0;
DDRD=(1<PIND1);//for USART TXD


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
UCSR0B=(0<<RXEN0)|(1<<TXEN0);




}

/////////////
///////void Counter_Timer_Init
//////////////////////////////////////

void Counter_Timer2_Init(void)
{

	
	TIMSK2=0;  //if 1 only overflow flag is enabled

	TCCR2A=0;

	TCCR2B=(1<<CS20)|(1<<CS21)|(0<<CS22);//setting prescaler ==clk/32

	}
	
void PCINTInit (void)
	
	
	{
	
	PCICR=(1<<PCIE1);
	PCMSK1=(1<<PCINT8)|(1<<PCINT9)|(1<<PCINT10)|(1<<PCINT11)|(1<<PCINT12)|(1<<PCINT13);
	
	
	}
	
