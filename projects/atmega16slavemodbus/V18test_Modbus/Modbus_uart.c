/*************************************************************************
 Title:    Interrupt UART library with receive/transmit circular buffers
 Author:   Peter Fleury <pfleury@gmx.ch>   http://jump.to/fleury
 File:     $Id: uart.c,v 1.6.2.1 2007/07/01 11:14:38 peter Exp $
 Software: AVR-GCC 4.1, AVR Libc 1.4.6 or higher
 Hardware: any AVR with built-in UART,
 License:  GNU General Public License

 DESCRIPTION:
 An interrupt is generated when the UART has finished transmitting or
 receiving a byte. The interrupt handling routines use circular buffers
 for buffering received and transmitted data.

 The UART_RX_BUFFER_SIZE and UART_TX_BUFFER_SIZE variables define
 the buffer size in bytes. Note that these variables must be a
 power of 2.

 USAGE:
 Refere to the header file uart.h for a description of the routines.
 See also example test_uart.c.

 NOTES:
 Based on Atmel Application Note AVR306

 LICENSE:
 Copyright (C) 2006 Peter Fleury

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 *************************************************************************/

/************************************************************************
 uart_available, uart_flush, uart1_available, and uart1_flush functions
 were adapted from the Arduino HardwareSerial.h library by Tim Sharpe on
 11 Jan 2009.  The license info for HardwareSerial.h is as follows:

 HardwareSerial.cpp - Hardware serial library for Wiring
 Copyright (c) 2006 Nicholas Zambetti.  All right reserved.

 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.

 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Lesser General Public License for more details.

 You should have received a copy of the GNU Lesser General Public
 License along with this library; if not, write to the Free Software
 Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

 Modified 23 November 2006 by David A. Mellis
 ************************************************************************/

/************************************************************************
 Changelog for modifications made by Tim Sharpe, starting with the current
 library version on his Web site as of 05/01/2009.

 Date        Description
 =========================================================================
 05/11/2009  Changed all existing UARTx_RECEIVE_INTERRUPT and UARTx_TRANSMIT_INTERRUPT
 macros to use the "_vect" format introduced in AVR-Libc
 v1.4.0.  Had to split the 3290 and 6490 out of their existing
 macro due to an inconsistency in the UART0_RECEIVE_INTERRUPT
 vector name (seems like a typo: USART_RX_vect for the 3290/6490
 vice USART0_RX_vect for the others in the macro).
 Verified all existing macro register names against the device
 header files in AVR-Libc v1.6.6 to catch any inconsistencies.
 05/12/2009  Added support for 48P, 88P, 168P, and 328P by adding them to the
 existing 48/88/168 macro.
 Added Arduino-style available() and flush() functions for both
 supported UARTs.  Really wanted to keep them out of the library, so
 that it would be as close as possible to Peter Fleury's original
 library, but has scoping issues accessing internal variables from
 another program.  Go C!
 05/13/2009  Changed Interrupt Service Routine label from the old "SIGNAL" to
 the "ISR" format introduced in AVR-Libc v1.4.0.

 ************************************************************************/

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include "Modbus_uart.h"
#include "p_defines.h"

extern volatile uart_status_t uart0_status;
extern volatile uart_status_t uart1_status;
extern tGLOBAL_FLAGS status;

/*
 *  constants and macros
 */

/* size of RX/TX buffers */
#define UART_RX_BUFFER_MASK ( UART_RX_BUFFER_SIZE - 1)
#define UART_TX_BUFFER_MASK ( UART_TX_BUFFER_SIZE - 1)

#if ( UART_RX_BUFFER_SIZE & UART_RX_BUFFER_MASK )
#error RX buffer size is not a power of 2
#endif
#if ( UART_TX_BUFFER_SIZE & UART_TX_BUFFER_MASK )
#error TX buffer size is not a power of 2
#endif

/*
 *  module global variables
 */

uint8_t is485 = TRUE; //Originally it was FALSE

extern volatile uint8_t UART_RxBuf[UART_RX_BUFFER_SIZE];
extern volatile uint8_t UART_RxHead;

static volatile unsigned char UART_TxBuf[UART_TX_BUFFER_SIZE];
static volatile unsigned char UART_TxHead;
static volatile unsigned char UART_TxTail;


#if defined( ATMEGA_USART1 )
static volatile unsigned char UART1_TxBuf[UART_TX_BUFFER_SIZE];
static volatile unsigned char UART1_RxBuf[UART_RX_BUFFER_SIZE];
static volatile unsigned char UART1_TxHead;
static volatile unsigned char UART1_TxTail;
static volatile unsigned char UART1_RxHead;
static volatile unsigned char UART1_RxTail;
static volatile unsigned char UART1_LastRxError;
#endif

ISR(UART0_RECEIVE_INTERRUPT)
/*************************************************************************
 Function: UART Receive Complete interrupt
 Purpose:  called when the UART has received a character
 **************************************************************************/
{
	uint8_t data;

//	Relay_Port ^= (1<<Relay_Mag2);

	// read UART status register and UART data register
	data = UART0_DATA;
	
	// Visualize data on LED
	
	// bitTics is watched by another ISR to see a possible modbus frame timeout
	// each time we got a new byte we reset bitTics
	uart0_status.bitTics = 0;

	// asking the modbus frame state machine where we are
	// and increment state if needed
	if (uart0_status.status == RS_Wait) {
		uart0_status.status = RS_Receiving;		
	}

	// only if modbus frame state machine is on receiving
	// we store the byte to the frame
	if (uart0_status.status == RS_Receiving) {
		 
		UART_RxBuf[UART_RxHead] = data;
		UART_RxHead++;		
		if (UART_RxHead >= UART_RX_BUFFER_MASK) {
			UART_RxHead = UART_RX_BUFFER_MASK;
		}
	}
}

ISR(UART0_TRANSMIT_INTERRUPT )
/*************************************************************************
 Function: USART Rx Complete interrupt
 Purpose:  called when the UART is ready to transmit the next byte
 **************************************************************************/
{
	unsigned char tmptail;

//	Relay_Port ^= (1<<Relay_Mag2);

	if (UART_TxHead != UART_TxTail) {
		/* calculate and store new buffer index */
		tmptail = (UART_TxTail + 1) & UART_TX_BUFFER_MASK;
		UART_TxTail = tmptail;
		/* get one byte from buffer and write it to UART */
		UART0_DATA = UART_TxBuf[tmptail]; /* start transmission */
		RS485_Port &= ~((1<<RS485_LTx));//Led Tx ON
		RS485_Port |= (1<<RS485_LRx);//Led Rx OFF
	} else {
		/* tx buffer empty, disable UDRE interrupt */
		UART0_CONTROL &= ~((1<<UART0_UDRIE));
		RS485_Port |= (1<<RS485_LTx);//Led Tx OFF
		RS485_Port &= ~((1 << RS485_Send)); //Clear PIN DE+RE SN75176 for enable RX from serial line
	}
}

/*************************************************************************
 Function: UART Data Register Empty interrupt
 Purpose:  called when the UART is ready to transmit the next byte
 **************************************************************************/

ISR(USART0_TX_vect) {

	if (is485) {
		RS485_Port |= (1<<RS485_Send); //Enable to TX SN75176
	}
}
/*************************************************************************/

void rs485_init(unsigned int baudrate) {
	is485 = TRUE;
	RS485_Port &= ~((1 << RS485_Send)); //Clear PIN DE+RE SN75176 for enable RX from serial line

//	UART0_CONTROL |= (1<<TXCIE0);
	uart_init(baudrate);
//	UCSR0B |= (1<<TXCIE0);

}


void rs323_init(unsigned int baudrate) {
	uart_init(baudrate);
}

/*************************************************************************
 Function: uart_init()
 Purpose:  initialize UART and set baudrate
 Input:    baudrate using macro UART_BAUD_SELECT()
 Returns:  none
 **************************************************************************/
void uart_init(unsigned int baudrate) {
	UART_TxHead = 0;
	UART_TxTail = 0;
	UART_RxHead = 0;

#if defined( AT90_UART )
	/* set baud rate */
	UBRR = (unsigned char)baudrate;

	/* enable UART receiver and transmitter and receive complete interrupt */
	UART0_CONTROL = ((1<<RXCIE)|((1<<RXEN)|((1<<TXEN);

#elif defined (ATMEGA_USART)
	/* Set baud rate */
	if ( baudrate & 0x8000 )
	{
		UART0_STATUS = (1<<U2X); //Enable 2x speed
		baudrate &= ~0x8000;
	}
	UBRRL = (unsigned char) baudrate;
	UBRRH = (unsigned char)(baudrate>>8);
	
	/* Enable USART receiver and transmitter and receive complete interrupt */
	UART0_CONTROL = ((1<<RXCIE)|(1<<RXEN)|(1<<TXEN)); //UCSZ2 will be cleared, so it combined with UCSZ1:0 sets the caracter size to 8.

	/* Set frame format: asynchronous, 8data, no parity, 1stop bit */
// #ifdef URSEL ... Original command
	//Set data frame format: asynchronous mode,no parity, 1 stop bit, 8 bit size
	UCSRC =((1<<URSEL)|(3<<UCSZ0));//Remaining bits will be clear
//#else ... original command
//	UCSRC = (3<<UCSZ0); .. original command
//#endif ... original command

#elif defined (ATMEGA_USART0 )
	/* Set baud rate */
	if (baudrate & 0x8000) {
		UART0_STATUS = (1 << U2X0); //Enable 2x speed
		baudrate &= ~0x8000;
	}
	UBRR0H = (unsigned char) (baudrate >> 8);
	UBRR0L = (unsigned char) baudrate;

	/* Enable USART receiver and transmitter and receive complete interrupt */
	UART0_CONTROL = ((1<<RXCIE0) | (1 << RXEN0) | (1 << TXEN0));

	/* Set frame format: asynchronous, 8data, no parity, 1stop bit */
#ifdef URSEL0
	UCSR0C = (1<<URSEL0)|(3<<UCSZ00);
#else
	UCSR0C = (3 << UCSZ00);
#endif

#elif defined ( ATMEGA_UART )
	/* set baud rate */
	if ( baudrate & 0x8000 )
	{
		UART0_STATUS = (1<<U2X); //Enable 2x speed
		baudrate &= ~0x8000;
	}
	UBRRHI = (unsigned char)(baudrate>>8);
	UBRR = (unsigned char) baudrate;

	/* Enable UART receiver and transmitter and receive complete interrupt */
	UART0_CONTROL = ((1<<RXCIE)|(1<<RXEN)|(1<<TXEN);

#endif

}/* uart_init */

/*************************************************************************
 Function: uart_getc()
 Purpose:  return byte from ringbuffer
 Returns:  lower byte:  received byte from ringbuffer
 higher byte: last receive error
 **************************************************************************/
unsigned int uart_getc(void) {
	/*
	 unsigned char tmptail;
	 unsigned char data;


	 if ( UART_RxHead == UART_RxTail ) {
	 return UART_NO_DATA;   // no data available
	 }

	 //calculate /store buffer index
	 tmptail = (UART_RxTail + 1) & UART_RX_BUFFER_MASK;
	 UART_RxTail = tmptail;

	 // get data from receive buffer
	 data = UART_RxBuf[tmptail];

	 return (UART_LastRxError << 8) + data;
	 */
	return 0;
}/* uart_getc */

/*************************************************************************
 Function: uart_putc()
 Purpose:  write byte to ring buffer for transmitting via UART
 Input:    byte to be transmitted
 Returns:  none
 **************************************************************************/
void uart_putc(unsigned char data) {
	unsigned char tmphead;

	tmphead = (UART_TxHead + 1) & UART_TX_BUFFER_MASK;

	while (tmphead == UART_TxTail) {
		;/* wait for free space in buffer */
	}

	if (is485){
		RS485_Port |= (1<<RS485_Send); //Set PIN Enable to TX
		//RS485_Port |= (1<<RS485_LRx); //Led Rx OFF
		//RS485_Port &= ~(1<<RS485_LTx); //Led Tx ON
	}

	UART_TxBuf[tmphead] = data;
	UART_TxHead = tmphead;

	/* enable UDRE interrupt */
	UART0_CONTROL |= ((1<<UART0_UDRIE));

}/* uart_putc */

/*************************************************************************
 Function: uart_puts()
 Purpose:  transmit string to UART
 Input:    string to be transmitted
 Returns:  none
 **************************************************************************/
void uart_puts(const char *s) {
	while (*s)
		uart_putc(*s++);

}/* uart_puts */

/*************************************************************************
 Function: uart_puts_p()
 Purpose:  transmit string from program memory to UART
 Input:    program memory string to be transmitted
 Returns:  none
 **************************************************************************/
void uart_puts_p(const char *progmem_s) {
	register char c;

	while ((c = pgm_read_byte(progmem_s++)))
		uart_putc(c);

}/* uart_puts_p */

/*************************************************************************
 Function: uart_flush()
 Purpose:  Flush bytes waiting the receive buffer.  Actually ignores them.
 Input:    None
 Returns:  None
 **************************************************************************/
void uart_flush(void) {
	uint8_t i8;
	UART_RxHead = 0;
	for (i8 = 0; i8 < UART_RX_BUFFER_SIZE; i8++) {
		UART_RxBuf[i8] = 0;
	}
}/* uart_flush */

/*
 * these functions are only for ATmegas with two USART
 */
#if defined( ATMEGA_USART1 )

ISR(UART1_RECEIVE_INTERRUPT)
/*************************************************************************
 Function: UART1 Receive Complete interrupt
 Purpose:  called when the UART1 has received a character
 **************************************************************************/
{
	unsigned char tmphead;
	unsigned char data;
	unsigned char usr;
	unsigned char lastRxError;

	/* read UART status register and UART data register */
	usr = UART1_STATUS;
	data = UART1_DATA;

	/* */
	lastRxError = (usr & ((1<<FE1) | (1<<DOR1)));

	/* calculate buffer index */
	tmphead = (UART1_RxHead + 1) & UART_RX_BUFFER_MASK;

	if (tmphead == UART1_RxTail) {
		/* error: receive buffer overflow */
		lastRxError = UART_BUFFER_OVERFLOW >> 8;
	} else {
		/* store new index */
		UART1_RxHead = tmphead;
		/* store received data in buffer */
		if (data == 13) {
			status.rs232_cmd = TRUE;
			UART1_RxBuf[tmphead] = 0;
		} else {
			UART1_RxBuf[tmphead] = data;
		}
		uart1_putc(data);
	}
	UART1_LastRxError = lastRxError;
}

SIGNAL(UART1_TRANSMIT_INTERRUPT)
/*************************************************************************
 Function: UART1 Data Register Empty interrupt
 Purpose:  called when the UART1 is ready to transmit the next byte
 **************************************************************************/
{
	unsigned char tmptail;

	if (UART1_TxHead != UART1_TxTail) {
		/* calculate and store new buffer index */
		tmptail = (UART1_TxTail + 1) & UART_TX_BUFFER_MASK;
		UART1_TxTail = tmptail;
		/* get one byte from buffer and write it to UART */
		UART1_DATA = UART1_TxBuf[tmptail]; /* start transmission */
	} else {
		/* tx buffer empty, disable UDRE interrupt */
		UART1_CONTROL &= ~((1<<UART1_UDRIE));
	}
}

/*************************************************************************
 Function: uart1_init()
 Purpose:  initialize UART1 and set baudrate
 Input:    baudrate using macro UART_BAUD_SELECT()
 Returns:  none
 **************************************************************************/
void uart1_init(unsigned int baudrate) {
	UART1_TxHead = 0;
	UART1_TxTail = 0;
	UART1_RxHead = 0;
	UART1_RxTail = 0;

	/* Set baud rate */
	if (baudrate & 0x8000) {
		UART1_STATUS = (1 << U2X1); //Enable 2x speed
		baudrate &= ~0x8000;
	}
	UBRR1H = (unsigned char) (baudrate >> 8);
	UBRR1L = (unsigned char) baudrate;

	/* Enable USART receiver and transmitter and receive complete interrupt */
	UART1_CONTROL = ((1<<RXCIE1) | (1 << RXEN1) | (1 << TXEN1));

	/* Set frame format: asynchronous, 8data, no parity, 1stop bit */
#ifdef URSEL1
	UCSR1C = (1<<URSEL1)|(3<<UCSZ10);
#else
	UCSR1C = (3 << UCSZ10);
#endif
}/* uart_init */

/*************************************************************************
 Function: uart1_getc()
 Purpose:  return byte from ringbuffer
 Returns:  lower byte:  received byte from ringbuffer
 higher byte: last receive error
 **************************************************************************/
unsigned int uart1_getc(void) {
	unsigned char tmptail;
	unsigned char data;

	if (UART1_RxHead == UART1_RxTail) {
		return UART_NO_DATA; /* no data available */
	}

	/* calculate /store buffer index */
	tmptail = (UART1_RxTail + 1) & UART_RX_BUFFER_MASK;
	UART1_RxTail = tmptail;

	/* get data from receive buffer */
	data = UART1_RxBuf[tmptail];

	return (UART1_LastRxError << 8) + data;

}/* uart1_getc */

/*************************************************************************
 Function: uart1_putc()
 Purpose:  write byte to ringbuffer for transmitting via UART
 Input:    byte to be transmitted
 Returns:  none
 **************************************************************************/
void uart1_putc(unsigned char data) {
	unsigned char tmphead;

	tmphead = (UART1_TxHead + 1) & UART_TX_BUFFER_MASK;

	while (tmphead == UART1_TxTail) {
		;/* wait for free space in buffer */
	}

	UART1_TxBuf[tmphead] = data;
	UART1_TxHead = tmphead;

	/* enable UDRE interrupt */
	UART1_CONTROL |= ((1<<UART1_UDRIE));

}/* uart1_putc */

/*************************************************************************
 Function: uart1_puts()
 Purpose:  transmit string to UART1
 Input:    string to be transmitted
 Returns:  none
 **************************************************************************/
void uart1_puts(const char *s) {
	while (*s)
		uart1_putc(*s++);

}/* uart1_puts */

/*************************************************************************
 Function: uart1_puts_p()
 Purpose:  transmit string from program memory to UART1
 Input:    program memory string to be transmitted
 Returns:  none
 **************************************************************************/
void uart1_puts_p(const char *progmem_s) {
	register char c;

	while ((c = pgm_read_byte(progmem_s++)))
		uart1_putc(c);

}/* uart1_puts_p */

/*************************************************************************
 Function: uart1_available()
 Purpose:  Determine the number of bytes waiting in the receive buffer
 Input:    None
 Returns:  Integer number of bytes in the receive buffer
 **************************************************************************/
int uart1_available(void) {
	return (UART_RX_BUFFER_MASK + UART1_RxHead - UART1_RxTail)
			% UART_RX_BUFFER_MASK;
}/* uart1_available */

/*************************************************************************
 Function: uart1_flush()
 Purpose:  Flush bytes waiting the receive buffer.  Acutally ignores them.
 Input:    None
 Returns:  None
 **************************************************************************/
void uart1_flush(void) {
	UART1_RxHead = UART1_RxTail;
}/* uart1_flush */

#endif
