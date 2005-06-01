/*!\file usart.c
 *This function takes care of the USART.  This is the lowest possible
 *OSI-layer on the MCU.
 */
#include "define.h"

/*!Set the USART with 9600baud, enable RX and TX, enable IRQ on receiving
 *data.
 */
void init_USART(void){
	//set the baudrate of the USART with fosc=16MHz
	UBRRH=0;//0
	UBRRL=103;//UBRRL=103 for 9600baud, UBRRL=8 for 115200baud
	/*enable receiver and transmitter
	 *set frame format: No parity, 1 stop bit, 8 data bit
	 *enable receive complete interrupt
	 */
	UCSRB|=(1<<RXEN)|(1<<TXEN)|(1<<RXCIE);
	//enable global interrupts
	sei();
}//init_USART

/*!Send a character to the USART
 *\param character a byte to send
 */
void send_USART(byte character){
	//wait for an empty transmit buffer
	while(!(UCSRA&(1<<UDRE)));
	UDR=character;
}//send_USART
