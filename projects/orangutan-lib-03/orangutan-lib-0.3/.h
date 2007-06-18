/*! \file uart.h \brief UART driver with buffer support. */
//*****************************************************************************
//
// File Name	: 'uart.h'
// Title		: lightweight UART driver with buffer support
// Author		: Original version - Pascal Stang - Copyright (C) 2000-2002
//				: Revised, simpler version - Steve Judd
// Created		: 11/22/2000
// Revised		: 02/10/2007
// Version		: 1.4
// Target MCU	: Pololu Orangutan / Baby Orangutan ATMEL ATmega168
// Editor Tabs	: 4
//
// This code is distributed under the GNU Public License
//		which can be found at http://www.gnu.org/licenses/gpl.txt
//
///	\ingroup driver_avr
/// \defgroup uart UART Driver/Function Library (uart.c)
/// \code #include "uart.h" \endcode
/// \par Overview
///		This library provides both buffered and unbuffered transmit and receive
///		functions for the AVR processor UART.  Buffered access means that the
///		UART can transmit and receive data in the "background", while your code
///		continues executing.
///
///		Unlike the original Procyon Avrlib version, the buffered support in this
///		version is provided using user-managed buffers, for reasons of size.
///
/// \par About UART operations
///		Most Atmel AVR-series processors contain one or more hardware UARTs
///		(aka, serial ports).  UART serial ports can communicate with other 
///		serial ports of the same type, like those used on PCs.  In general,
///		UARTs are used to communicate with devices that are RS-232 compatible
///		(RS-232 is a certain kind of serial port).
///	\par
///		By far, the most common use for serial communications on AVR processors
///		is for sending information and data to a PC running a terminal program.
///		Here is an exmaple:
///	\code
/// uartInit();					// initialize UART (serial port)
/// uartSetBaudRate(9600);		// set UART speed to 9600 baud
/// uartSendbyte('x');			// send a single character
/// uartSendBuf(strBuf, strlen(buf),&flag) // send a user-managed buffer
///                                        // and set "flag" when complete		
/// \endcode
///
/// \warning The CPU frequency (F_CPU) must be set correctly
///		for the UART library to calculate correct baud rates.  Furthermore,
///		certain CPU frequencies will not produce exact baud rates due to
///		integer frequency division round-off.  See your AVR processor's
///		 datasheet for full details.
//
//*****************************************************************************
//@{

#ifndef UART_H
#define UART_H

// set F_CPU if undefined
//
#ifndef F_CPU
#define F_CPU 8000000UL
#endif

//! Default uart baud rate.
/// This is the default speed after a uartInit() command,
/// and can be changed by using uartSetBaudRate().
#define UART_DEFAULT_BAUD_RATE	9600

// compatibility with megaXX8 processors
#if	defined(__AVR_ATmega88__)	|| \
	defined(__AVR_ATmega168__)	|| \
	defined(__AVR_ATmega644__)
	#define UDR					UDR0
	#define UCR					UCSR0B
	#define RXCIE				RXCIE0
	#define TXCIE				TXCIE0
	#define RXC					RXC0
	#define TXC					TXC0
	#define RXEN				RXEN0
	#define TXEN				TXEN0
	#define UBRRL				UBRR0L
	#define UBRRH				UBRR0H
	#define SIG_UART_TRANS		SIG_USART_TRANS
	#define SIG_UART_RECV		SIG_USART_RECV
	#define SIG_UART_DATA		SIG_USART_DATA
#endif
// compatibility with mega169 processors
#if	defined(__AVR_ATmega169__)
	#define SIG_UART_TRANS		SIG_USART_TRANS
	#define SIG_UART_RECV		SIG_USART_RECV
	#define SIG_UART_DATA		SIG_USART_DATA
#endif

// functions

//! Initializes uart.
/// \note	After running this init function, the processor
/// I/O pins that used for uart communications (RXD, TXD)
/// are no long available for general purpose I/O.
void uartInit(void);

//! Sets the uart baud rate.
/// Argument should be in bits-per-second, like \c uartSetBaudRate(9600);
void uartSetBaudRate(u32 baudrate);

//	send a single byte
//
void uartSendByte(uint8_t data);

//! Gets a single byte from the uart receive buffer.
/// Returns the byte, or -1 if no byte is available (getchar-style).
int16_t uartGetByte(void);

//! Gets a single byte from the uart receive buffer.
/// Function returns TRUE if data was available, FALSE if not.
/// Actual data is returned in variable pointed to by "data".
/// Example usage:
/// \code
/// char myReceivedByte;
/// uartReceiveByte( &myReceivedByte );
/// \endcode
uint8_t uartReceiveByte(uint8_t* data);

//! Sends a block of data via the uart using interrupt control.
/// \param buffer	pointer to data to be sent
///	\param nBytes	length of data (number of bytes to sent)
uint8_t  uartSendBuffer(uint8_t *buffer, uint16_t nBytes,  uint8_t *flag);

/// Receive a block of data using interrupt control and set the user-provided flag
/// when the buffer is full or the character in "rcvDone" is received.
///
uint8_t uartReceiveBuffer(uint8_t *buffer, uint16_t nbytes, uint16_t *rxBytes, uint8_t rcvDone, uint8_t *flag);

//! Wait for a send or receive operation to complete and set the provided flag.
///
void uartWait(uint8_t *flag);


#endif
//@}


