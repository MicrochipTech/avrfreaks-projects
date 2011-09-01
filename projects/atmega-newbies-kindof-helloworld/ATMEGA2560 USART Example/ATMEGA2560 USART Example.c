#define FOSC 8000000// Clock Speed
#define BAUD 9600
#define MYUBRR FOSC/16/BAUD-1
#include "..\..\..\..\..\..\Program Files (x86)\Atmel\AVR Studio 5.0\AVR ToolChain\avr\include\avr\io.h"
#include "..\..\..\..\..\..\Program Files (x86)\Atmel\AVR Studio 5.0\AVR ToolChain\avr\include\avr\interrupt.h"
void USART_Init( unsigned int ubrr);
void USART_Transmit( unsigned char data );
unsigned char USART_Receive( void );


ISR(USART3_RX_vect)
{ /* signal handler for external interrupt int0 */

   char received = UDR3;
	if (received == '0')
	{
	//IF 0 is received, start a new ADC conversion	
		// start single convertion
		// write '1' to ADSC
		ADCSRA |= (1<<ADSC);
		
		USART_Transmit( 'O');
		USART_Transmit( 'k');
		USART_Transmit( 13); //Carriage Return
		USART_Transmit( 10); //New Line
	}
	else
	{
		USART_Transmit( 'E');
		USART_Transmit( 'x');
		USART_Transmit( 'p');
		USART_Transmit( 'e');
		USART_Transmit( 'c');
		USART_Transmit( 't');
		USART_Transmit( 'i');
		USART_Transmit( 'n');
		USART_Transmit( 'g');
		USART_Transmit( '0');
		USART_Transmit( 13); //Carriage Return
		USART_Transmit( 10); //New Line
	}

}


ISR(ADC_vect)
{
	char analog = ADC;
	USART_Transmit( 'A');
	USART_Transmit( 'D');
	USART_Transmit( 'C');
	USART_Transmit( '=');
	USART_Transmit( ADC);	
	USART_Transmit( 13); //Carriage Return
	USART_Transmit( 10); //New Line
}



void ADC_Init() 
{

	//Initialize ADC and start async conversion
	
			//Initialize ADC (Analog Voltage Reference & Conversion Frequency
			ADMUX = (1<<REFS0);
			ADCSRA = (1<<ADEN)|(1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0)|(1<<ADIE); // prescaler = 128 => 8MHz/128=62.5KHz  //ADIE= AnalogDigital conversion Interruption Elable
	
			// select the corresponding channel 0~7
			// ANDing with '7' will always keep the value
			// of 'ch' between 0 and 7
			uint8_t ch = 0b00000111;  // AND operation with 7
			ADMUX |= ch; //CHANNEL 7

			// start single conversion
			// write '1' to ADSC
			ADCSRA |= (1<<ADSC);

}

void main( void )
{ 
	


	DDRB  = (1<<DDB4) | (1<<DDB7);   //PB4 & PB7 Outputs
	DDRG  = (1<<DDG5);  //PG5 Output
	DDRH  = (1<<DDH6);   // PH6 Output


	TCCR0A = (1<<COM0A1) | (1<<COM0B1) | (1<<WGM01) | (1<<WGM00);       // Non inverting mode on OC0A and OC0B, Mode = Mode 3 FAST PWM
	TCCR0B = (1<<CS00) ;												// No prescaling CS01 y CS02 hacen diferente prescaling.
	TCCR2A = (1<<COM2A1) | (1<<COM2B1) | (1<<WGM21) | (1<<WGM20);       // Non inverting mode on OC2A, Mode = Mode 3 FAST PWM
	TCCR2B = (1<<CS20) ;                                                // No prescaling CS21 y CS22 hacen diferente prescaling.
 

	//By Intensity level: 255=OFF 0=ON
	OCR0A = 253;  //PB7: OC0A //LED0
	OCR2A = 240; //PB4: OC2A //LED1
	OCR0B = 170; //PG5: OC0B //LED2
	OCR2B = 0; //PH6: OC2B //LED3


	USART_Init ( MYUBRR );
	ADC_Init();
	USART_Transmit( 'H');
	USART_Transmit( 'e');
	USART_Transmit( 'l');
	USART_Transmit( 'l');
	USART_Transmit( 'o');
	USART_Transmit( 13); //Carriage Return
	USART_Transmit( 10); //New Line
	
	sei(); //Interrupt Enable






		//Syncronious operation of ADC
			//char result = USART_Receive();
			//if (result == '0')
			//{
				//USART_Transmit( 'O');
				//USART_Transmit( 'k');
			//}

} // main


void USART_Init( unsigned int ubrr){
	/* Set baud rate */
	UBRR3H = (unsigned char)(ubrr>>8);
	UBRR3L = (unsigned char) ubrr;
	
	/**/
	
	/* Enable receiver and transmitter */
	UCSR3B = (1<<RXEN3)|(1<<TXEN3|(1<<RXCIE3)); //RXCIE3 Reception Interruption Enable for USART3
	/* Set frame format: 8data, 2stop bit */
	UCSR3C = (1<<USBS3)|(3<<UCSZ30);
} // USART_Init

void USART_Transmit( unsigned char data )
{
	/* Wait for empty transmit buffer */
	while ( !( UCSR3A & (1<<UDRE3)) );
	/* Put data into buffer, sends the data */
	UDR3 = data;
}

unsigned char USART_Receive( void )
{
	/* Wait for data to be received */
	while ( !(UCSR3A & (1<<RXC3)) );
	/* Get and return received data from buffer */
	return UDR3;
}