


#include <avr\io.h>
#include <avr\interrupt.h>
#include <avr\iomx8.h>
#include <stdbool.h> 
#include "Init.h"
#define outp(a, b) b = a


void USART_Transmit( unsigned char data );
void Term_Send_Value_as_Digits( uint8_t value );


volatile uint8_t  CH0=0;	//
volatile uint8_t  CH1=0;	//
volatile uint8_t  CH2=0;	//
volatile uint8_t  CH3=0;	//
volatile uint8_t  CH4=0;	//
volatile uint8_t  CH5=0;	//

volatile uint8_t channel=0;
 uint8_t STATUS_D;




int main(void) {

Init_all();

sei();

while (1) {//loop forever

//CH0 is decoded and displayed in HyperTerminal as Ch1 to be consistent with RC channel nomenclature
//It must be remembered that channels must be connected in correct order for this decoder to work
//PC0 must be connected to channel1 of RC radio, PC1 to CH2, PC3 to Channel4 and so on
//although this works with 6 channels, there is no reason why it cant be expanded to more channels


USART_Transmit(' ');
USART_Transmit(32);		//Transmit Space
USART_Transmit('C');
USART_Transmit('H');
USART_Transmit('1');
USART_Transmit(' ');
Term_Send_Value_as_Digits(CH0);
USART_Transmit(32);		//Transmit Space
USART_Transmit('C');
USART_Transmit('H');
USART_Transmit('2');
USART_Transmit(' ');
Term_Send_Value_as_Digits(CH1);
USART_Transmit(32);		//Transmit Space
USART_Transmit('C');
USART_Transmit('H');
USART_Transmit('3');
USART_Transmit(' ');
Term_Send_Value_as_Digits(CH2);
USART_Transmit(32);		//Transmit Space
USART_Transmit('C');
USART_Transmit('H');
USART_Transmit('4');
USART_Transmit(' ');
Term_Send_Value_as_Digits(CH3);
USART_Transmit(32);		//Transmit Space
USART_Transmit('C');
USART_Transmit('H');
USART_Transmit('5');
USART_Transmit(' ');
Term_Send_Value_as_Digits(CH4);
USART_Transmit(32);		//Transmit Space
USART_Transmit('C');
USART_Transmit('H');
USART_Transmit('6');
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
void Term_Send_Value_as_Digits( uint8_t value )
{
    unsigned char digit;
   
    
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
        digit++;                         // Increment 2nd digit
        value -= 10;
    }
    
    USART_Transmit( digit );                  // Send 2nd digit
    
    USART_Transmit( '0' + value );            // Send first digit
}


