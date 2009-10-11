


#include <avr\io.h>
#include <avr\interrupt.h>
#include <avr\iomx8.h>
#include <stdbool.h> 
#include "Init.h"
#define outp(a, b) b = a


void USART_Transmit( unsigned char data );
void Term_Send_Value_as_Digits( uint16_t value );


volatile uint16_t CH0=0;	//ADC0
volatile uint16_t  CH1=0;	//ADC1
volatile uint16_t  CH2=0;	// ADC2
volatile uint16_t  CH3=0;	//ADC3
volatile uint16_t  CH4=0;	// ADC4
volatile uint16_t  CH5=0;	//ADC5



 uint8_t  volatile channel =0;


int main(void) {
uint16_t LineNumber=0;
Init_all();

sei();

while (1) {//loop forever


LineNumber++;
Term_Send_Value_as_Digits(LineNumber);
USART_Transmit(' ');
USART_Transmit(' ');
USART_Transmit(' ');
USART_Transmit(32);		//Transmit Space
USART_Transmit('C');
USART_Transmit('H');
USART_Transmit('0');
USART_Transmit(' ');
Term_Send_Value_as_Digits(CH0);
USART_Transmit(32);		//Transmit Space
USART_Transmit('C');
USART_Transmit('H');
USART_Transmit('1');
USART_Transmit(' ');
Term_Send_Value_as_Digits(CH1);
USART_Transmit(32);		//Transmit Space
USART_Transmit('C');
USART_Transmit('H');
USART_Transmit('2');
USART_Transmit(' ');
Term_Send_Value_as_Digits(CH2);
USART_Transmit(32);		//Transmit Space
USART_Transmit('C');
USART_Transmit('H');
USART_Transmit('3');
USART_Transmit(' ');
Term_Send_Value_as_Digits(CH3);
USART_Transmit(32);		//Transmit Space
USART_Transmit('C');
USART_Transmit('H');
USART_Transmit('4');
USART_Transmit(' ');
Term_Send_Value_as_Digits(CH4);
USART_Transmit(32);		//Transmit Space
USART_Transmit('C');
USART_Transmit('H');
USART_Transmit('5');
USART_Transmit(' ');
Term_Send_Value_as_Digits(CH5);
USART_Transmit(0xa);
USART_Transmit(0xd);  

	
}


}











void USART_Transmit( unsigned char data )
{

/* Wait for empty transmit buffer */
while ( !( UCSR0A & (1<<UDRE0)) );
/* Put data into buffer, sends the data */
UDR0 = data;

}
//***************************
// Convert byte to 3 ASCII digits and send
//***************************
void Term_Send_Value_as_Digits( uint16_t value )
{
    unsigned char digit;
	digit = '0';
    while( value >= 10000 )                // Still larger than 10000 ?
    {
        digit++;                         // Increment first digit
        value -= 10000;
    }
    
    USART_Transmit( digit );                  // Send first digit
	
	digit = '0';
    while( value >= 1000 )                // Still larger than 1000 ?
    {
        digit++;                         // Increment second digit
        value -= 1000;
    }
    
    USART_Transmit( digit );                  // Send second digit
    
    digit = '0';
    while( value >= 100 )                // Still larger than 100 ?
    {
        digit++;                         // Increment third digit
        value -= 100;
    }
    
    USART_Transmit( digit );                  // Send third digit
    
    digit = '0';
    while( value >= 10 )                 // Still larger than 10 ?
    {
        digit++;                         // Increment fourth digit
        value -= 10;
    }
    
    USART_Transmit( digit );                  // Send fourth digit
    
    USART_Transmit( '0' + value );            // Send fifth digit
}

