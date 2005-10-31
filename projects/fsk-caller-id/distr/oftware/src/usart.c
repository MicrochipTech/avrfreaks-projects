/*!\file usart.c
 *This function takes care of the USART.  This is the lowest possible
 *OSI-layer on the MCU.
 */
#include "define.h"

//!Buffer containing data that has been received through the UART
uint8_t circRXArray[RXBUFSIZE];
uint8_t teller;//!<Current position in the circRXArray buffer.

/*!Set the USART with baud, enable RX and TX, enable IRQ on receiving
 *data.
 *\param baud	Baudrate in [bps]
 *\param RX_ON	Enable receiving UART-data when true.
 */
void init_USART(uint32_t baud, BOOL RX_ON){
	/* Rounding error in :
	 * 		uint16_t ubrr=FOSC/(16*baud)-1;
	 * So used a trick: x/y becomes (x/(y/2)+1)/2
	 */
	uint16_t ubrr=0;
	
	/*Preventing division by zero! (hard to track error)*/
	if(baud)ubrr=(FOSC/(8*baud)+1)/2-1;
	
	//set the baudrate of the USART with fosc=16MHz
	UBRRH=ubrr>>8;
	UBRRL=ubrr&0xFF;
	/* set frame format: No parity, 1 stop bit, 8 data bit
	 */
	if(RX_ON)UCSRB|=(1<<RXEN)|(1<<RXCIE);
	UCSRB|=(1<<TXEN);
	
	//enable global interrupts
	sei();
}//init_USART

/*!Send a character to the USART
 *\param buf Pointer to the databuffer containing the data to be sent.
 *\param length Length of the data to be sent.
 */
void send_USART(uint8_t* buf, uint8_t length){//test OK
	do{
		//wait for an empty transmit buffer
		while(!(UCSRA&(1<<UDRE)));
		UDR=*(buf++);
	}while(--length);
}//send_USART

//! Receive ISR of the UART.  It puts all the data in the circRXArray.
SIGNAL(SIG_UART_RECV){		//test OK
	uint8_t c=UDR;
	if(teller<RXBUFSIZE)
		circRXArray[teller++]=c;
}//USART receive complete

