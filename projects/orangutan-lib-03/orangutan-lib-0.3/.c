/*! \file uart.c \brief UART driver with buffer support. */
// *****************************************************************************
//
// File Name	: 'uart.c'
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
// *****************************************************************************
//	NOTE: this module must be compiled with -O0, otherwise setting the send and 
//	receive complete flags gets optimized away and the calling program hangs.
//
// *****************************************************************************

#include "device.h"
#include <avr/io.h>
#include <avr/interrupt.h>


#include "uart.h"


#include <util/delay.h>

// UART global variables
// flag variables
volatile uint8_t   	uartReadyTx;			///< uartReadyTx flag
volatile uint8_t   	uartBufferedTx;		///< uartBufferedTx flag
volatile uint8_t   	uartCharRx;			// boolean - true if a character is present
volatile uint8_t   	uartRxChar;			// received char if no buffer provided.
volatile uint8_t  	*txBuf;
volatile uint16_t	txLen;
volatile uint8_t	*txFlag;
volatile uint8_t  	*rxBuf;
volatile uint16_t	rxLen;
volatile uint8_t	*rxFlag;
volatile uint8_t	rxDone;
volatile uint16_t	*rxBytesRcv;


unsigned short uartRxOverflow;		///< receive overflow counter
// rx buffer and pointers


// enable and initialize the uart
void uartInit(void)
{

	// enable RxD/TxD and interrupts

	UCR = (1<<RXCIE)|(1<<TXCIE)|(1<<RXEN)|(1<<TXEN);

	// set default baud rate
	uartSetBaudRate(UART_DEFAULT_BAUD_RATE);  
	// initialize states
	uartReadyTx = TRUE;
	uartBufferedTx = FALSE;
	// clear overflow count
	uartRxOverflow = 0;
	// enable interrupts
	sei();
}

// set the uart baud rate
void uartSetBaudRate(u32 baudrate)
{
	uint32_t fcpu;
	fcpu=F_CPU;

	// calculate division factor for requested baud rate, and set it
	uint16_t bauddiv = ((F_CPU+(baudrate*8L))/(baudrate*16L)-1);
	UBRRL = (uint8_t)bauddiv;
	#ifdef UBRRH
	UBRRH = (uint8_t)(bauddiv>>8)&0xFF;
	#endif
}

// transmits a byte over the uart
void uartSendByte(uint8_t txData)
{
	// wait for the transmitter to be ready
	while(!uartReadyTx);
	// set ready state to FALSE
	uartReadyTx = FALSE;
	// send byte
	UDR=txData;
}

// gets a single byte from the uart receive buffer (getchar-style)
int uartGetByte(void)
{
	uint8_t c;
	if(uartReceiveByte(&c))
		return c;
	else
		return -1;
}

// gets a byte (if available) from the uart receive buffer
uint8_t uartReceiveByte(uint8_t* rxData)
{
		// make sure we have data
		if(uartCharRx)
		{
			// get byte received
			*rxData = uartRxChar;
			uartCharRx=FALSE;
			return TRUE;
		}
		else
		{
			// no data
			return FALSE;
		}
}


// Set up a receive buffer and let the UART fill it
//
uint8_t uartReceiveBuffer(uint8_t *buffer, uint16_t nBytes, uint16_t *rxBytes, uint8_t done, uint8_t *flag) {
	rxBuf=buffer;
	rxLen=nBytes;
	rxFlag=flag;
	rxBytesRcv=rxBytes;
	rxDone=done;
	*rxFlag=0;
	*rxBytes=0;

	return(TRUE);
}

// transmit nBytes from buffer out the uart
// make the send buffer work - SJ, 20-Jan-2007
//
uint8_t uartSendBuffer(uint8_t *buffer, uint16_t nBytes, uint8_t *flag)
{
	txBuf=buffer;
	txLen=nBytes;
	txFlag=flag;

	*txFlag=0;

	// wait for completion - this avoids a cascading race condition in async xmit
	while(!uartReadyTx);

	// Enable async transmission at interrupt level
	uartBufferedTx = TRUE;
	// send the first byte to get things going by interrupts
	uartSendByte(*txBuf);		
	// return success
	return(TRUE);
}

void uartWait(uint8_t *flag) {
	while (!*flag);
}

// UART Transmit Complete Interrupt Handler
ISR(SIG_UART_TRANS)
{
	// check if buffered tx is enabled
	if(uartBufferedTx)
	{
		// check if there's data left in the buffer
		if(--txLen)
		{
			// send byte from top of buffer
			UDR=*(++txBuf);
		}
		else
		{
			// no data left
			uartBufferedTx = FALSE;
			// return to ready state
			*txFlag = TRUE;
			uartReadyTx = TRUE;
		}
	}
	else
	{
		// we're using single-byte tx mode
		// indicate transmit complete, back to ready
		uartReadyTx = TRUE;
	}
}

// UART Receive Complete Interrupt Handler
ISR(SIG_UART_RECV)
{
	uint8_t c;
	
	// get received char
	c = UDR;

	// did the user give us a buffer to fill?
	if (rxBuf) {
		*rxBuf++=c;
		(*rxBytesRcv)++;
		if ((c == rxDone) | (*rxBytesRcv==rxLen)) {
			*rxFlag=1;
			rxBuf=0;
		}
	} else {
		if (!uartCharRx) {
			uartRxChar=c;
			uartCharRx=TRUE;
		} else 	uartRxOverflow++;

	}		

}
