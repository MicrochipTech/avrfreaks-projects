
#include <avr\io.h>
#include <avr\interrupt.h>
#include "Init.h"

void Init_all(void)
{

UART_Init();
A2D_Init();


}
///////////////////////////////////////
////  PWM_Init	Initializing PWM system
//////////////////
void PortInit(void)
{
DDRD=(1<<PIND1);
DDRC=0;


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
/////A2D_Init
////////Initializing A2D Converter
/////////////
void A2D_Init()
{




ADMUX=(1<<REFS0)|(0<<REFS1)|(1<<MUX2)|(1<<MUX0);//AVCC voltage reference,Convert ADC5

ADCSRB=0;//Set ADC to Free running mode

ADCSRA=(1<<ADEN)|(1<<ADSC)|(1<<ADIE)|(1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0);




}
