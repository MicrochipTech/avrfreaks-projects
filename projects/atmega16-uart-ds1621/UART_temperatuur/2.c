/* Code adapted from Atmel AVR Application Note AVR306
 * Interrupt mode driver for UART.
 */
#include <iom16v.h>
#include <macros.h>
#include "uart.h"

/* IMPORTANT: these vector numbers are for 8515! If you use other devices
 * you must change them to the different set of numbers.
 *
 * UART_RX_interrupt set to UART, Rx Complete
 * UART_TX_interrupt set to UART Data Register Empty
 */

#pragma interrupt_handler UART_RX_interrupt:12 UART_TX_interrupt:14

/* UART Buffer Defines */
#define UART_RX_BUFFER_SIZE 128 /* 1,2,4,8,16,32,64,128 or 256 bytes */
#define UART_RX_BUFFER_MASK ( UART_RX_BUFFER_SIZE - 1 )
#define UART_TX_BUFFER_SIZE 128 /* 1,2,4,8,16,32,64,128 or 256 bytes */
#define UART_TX_BUFFER_MASK ( UART_TX_BUFFER_SIZE - 1 )

#if ( UART_RX_BUFFER_SIZE & UART_RX_BUFFER_MASK )
#error RX buffer size is not a power of 2
#endif

/* Static Variables */
static unsigned char UART_RxBuf[UART_RX_BUFFER_SIZE];
static volatile unsigned char UART_RxHead;
static volatile unsigned char UART_RxTail;
static unsigned char UART_TxBuf[UART_TX_BUFFER_SIZE];
static volatile unsigned char UART_TxHead;
static volatile unsigned char UART_TxTail;

/* initialize UART */
void InitUART( unsigned char baudrate )
	{
	unsigned char x;
	UBRR = baudrate; /* set the baud rate */
	/* enable UART receiver and transmitter, and
	receive interrupt */
	UCSRB = ( (1<<RXCIE) | (1<<RXEN) | (1<<TXEN) );
	x = 0; /* flush receive buffer */
	UART_RxTail = x;
	UART_RxHead = x;
	UART_TxTail = x;
	UART_TxHead = x;
	}

/* interrupt handlers */
void UART_RX_interrupt( void )
{
	unsigned char data;
	unsigned char tmphead;
	data = UDR; /* read the received data */
	/* calculate buffer index */
	tmphead = ( UART_RxHead + 1 ) & UART_RX_BUFFER_MASK;
	UART_RxHead = tmphead; /* store new index */
	if ( tmphead == UART_RxTail )
		{
		/* ERROR! Receive buffer overflow */
		}
	UART_RxBuf[tmphead] = data; /* store received data in buffer */
}

void UART_TX_interrupt( void )
{
	unsigned char tmptail;

	/* check if all data is transmitted */
	if ( UART_TxHead != UART_TxTail )
		{
		/* calculate buffer index */
		tmptail = ( UART_TxTail + 1 ) & UART_TX_BUFFER_MASK;
		UART_TxTail = tmptail; /* store new index */
		UDR = UART_TxBuf[tmptail]; /* start transmition */
		}
	else
		{
		UCSRB &= ~(1<<UDRIE); /* disable UDRE interrupt */
		}
	}

/* Read and write functions */
unsigned char ReceiveByte( void )
	{
	unsigned char tmptail;

	while ( UART_RxHead == UART_RxTail ) /* wait for incomming data */
		;
	tmptail = ( UART_RxTail + 1 ) & UART_RX_BUFFER_MASK;/* calculate buffer index */
	UART_RxTail = tmptail; /* store new index */
	return UART_RxBuf[tmptail]; /* return data */
	}

void TransmitByte( unsigned char data )
	{
	unsigned char tmphead;
	/* calculate buffer index */
	tmphead = ( UART_TxHead + 1 ) & UART_TX_BUFFER_MASK; 
		/* wait for free space in buffer */

	while ( tmphead == UART_TxTail )
		;
	UART_TxBuf[tmphead] = data; /* store data in buffer */
	UART_TxHead = tmphead; /* store new index */
	UCSRB |= (1<<UDRIE); /* enable UDRE interrupt */
	}

unsigned char DataInReceiveBuffer( void )
	{
	return ( UART_RxHead != UART_RxTail ); 
		/* return 0 (FALSE) if the receive buffer is empty */
	}


/* main - a simple test program*/
void main( void )
	{

	InitUART( 23 ); /* set the baudrate to 9600 bps using a 4MHz crystal */
	_SEI(); /* enable interrupts => enable UART interrupts */

	while ( 1 ) /* forever */
		{
		TransmitByte(get_ds1621_temperature()); /* echo the received character */
		}
	}
