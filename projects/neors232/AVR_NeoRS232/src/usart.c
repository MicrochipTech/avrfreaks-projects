/*!\file usart.c
 *This function takes care of the USART.  This is the lowest possible
 *OSI-layer on the MCU.
 */


#include "define.h"


/*!<Size of the circular buffer for receiving characters.*/
#define RXBUFSIZE 100


/*!Struct for working with the circular buffer for receiving characters.
 */
typedef struct{
	uint8_t* pRxPut;				/*!<pointer to put next received byte*/
	uint8_t* pRxGet;				/*!<pointer to get next received byte*/
	uint8_t* pRxBegin;				/*!<begin of RX-buffer*/
	uint8_t* pRxEnd;				/*!<end of RX-buffer*/
	uint8_t rxCount;				/*!<number of unread bytes received*/
}COM_HANDLE_T;


/*!Circular array containing the incoming data bytes.*/
uint8_t circRXArray[RXBUFSIZE];


static COM_HANDLE_T comHandle;
static COM_HANDLE_T* pCom;


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
	
	/*Preventing division by zero! (error was hard to track)*/
	if(baud)ubrr=(FOSC/(8*baud)+1)/2-1;
	
	//set the baudrate of the USART with fosc=16MHz
	UBRRH=ubrr>>8;
	UBRRL=ubrr&0xFF;
	/* set frame format: No parity, 1 stop bit, 8 data bit
	 */
	if(RX_ON)UCSRB|=(1<<RXEN)|(1<<RXCIE);
	UCSRB|=(1<<TXEN);
	
	pCom=&comHandle;
	//set read and write pointers to begin of buffer
	pCom->pRxGet=pCom->pRxPut=pCom->pRxBegin=circRXArray;
	//set end pointer at end of buffer
	pCom->pRxEnd=circRXArray+RXBUFSIZE-1;
	pCom->rxCount=0;
	
	//enable global interrupts
	sei();
}//init_USART


/*!Send a characterbuffer to the USART
 *\param buf 		Pointer to buffer that contains data to be sent
 *\param length		Number of bytes to be sent
 */
void write_USART(uint8_t* buf, uint8_t length){//test OK
	do{
		//wait for an empty transmit buffer
		while(!(UCSRA&(1<<UDRE)));
		UDR=*(buf++);
	}while(--length);
}//write_USART


/*! Write one character to the UART
 *\param c The character that must be sent.
 */
void write_USART_B(uint8_t c){
	//wait for an empty transmit buffer
	while(!(UCSRA&(1<<UDRE)));
	UDR=c;
}//write_USART_B


/*! Function that reads a character from the circular receive buffer.
 *\param c Pointer to a character.  The read character will be written to 
 *	that address.
 */
BOOL read_USART(uint8_t* c){
	if(!(pCom->rxCount))return FALSE;
	
	*c=*(pCom->pRxGet);
	pCom->pRxGet++;
	if(pCom->pRxGet==pCom->pRxEnd)pCom->pRxGet=pCom->pRxBegin;
	//The following operation MUST be atomic (dumb way: disable interrupts)
	cli();
	pCom->rxCount--;
	sei();
	return TRUE;
}//read_USART


/*!ISR-UART receive.  This ISR will be called every time a character comes
 *	in.  The character will then be written to a receive buffer.
 */
SIGNAL(SIG_UART_RECV){
	//Read the UDR-register to clear the interrupt
	*(pCom->pRxPut)=UDR;
	
	pCom->rxCount++;
	if(pCom->rxCount>=RXBUFSIZE)
		/*RX-buffer overflow:
		 *If buffer is full, pRxPut will remain at the end of the buffer
		 *and will overwrite the last byte in the buffer with every new
		 *byte that comes in.
		 */
		pCom->rxCount=RXBUFSIZE-1;
	else{
		pCom->pRxPut++;
		if(pCom->pRxPut==pCom->pRxEnd)pCom->pRxPut=pCom->pRxBegin;		
	}
}//USART receive complete

