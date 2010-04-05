//****************************************************************************************
///	File Name	: \file zoUart.h 
///	Description	: \brief Uart 0 function library.
//	Created		: 21/01/2010
//	Target MCU	: Mega 48/88/168/328
//	Author		: Sissakis Giannis
//  email		: info@01mech.com
//
//  Copyright (C) 2010 Zero One Mechatronics LP
//
//  This program is free software: you can redistribute it and/or modify it under the 
//  terms of the GNU General Public License as published by the Free Software Foundation, 
//  version 3 of the License or any later version. This program is distributed in the hope
//  that it will be useful,but WITHOUT ANY WARRANTY; without even the implied warranty of 
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public 
//  License for more details: <http://www.gnu.org/licenses/>
//  
/// \ingroup driver_avr 
/// \defgroup zoUart zoUart.h : Avr uart0 function library.
/// 
/// \par Overview
/// The zoUart library implements a set of functions for manipulation of the USART0 AVR
/// peripheral. Whether you are going to use RS232 simple 5V uart or half duplex RS485
/// the functions needed for uart communications are contained in this library. 
///   
/// \par Usage example 
/// \code
/// #include "zoUart.h"
///
/// int main(void)
/// {
///     //initialize the AVR USART0 peripheral to 115200 Kbps, 
///     //8 data bits, No parity, 1 stop bit
///     zoUartInit(); 		            
/// 
///     //say hello world to a terminal on your PC!
///     zoUartPutString("Hello world\n\r");
///
/// }
/// \endcode 
/// \todo Expand this library to multi uart processors like the Mega640 etc.
//****************************************************************************************
//@{

#ifndef ZO_AVR_UART_H
#define ZO_AVR_UART_H

#include <avr/io.h>
#include <avr/interrupt.h>
#include "zoMcu.h"
#include "zoTypes.h"
#include "zoError.h"

//Types used for configuration____________________________________________________________
///Uart parity configuration enumeration. Select one of the enumeration values as an 
///argument to the zoUartSetParity() function.
typedef enum {
	ZO_UART_PARITY_NONE = 0x00, ///< configure no parity 
	ZO_UART_PARITY_ODD = 0x30,  ///< configure odd parity
	ZO_UART_PARITY_EVEN = 0x20  ///< configure even parity
}ZO_UART_PARITY;

///Uart data-bits configuration enumeration. Select one of the enumeration values as an 
///argument to the zoUartSetDataBits() function.
typedef enum {
	ZO_UART_DATA_BITS_5 = 0x00, ///< configure 5 data bits
	ZO_UART_DATA_BITS_6 = 0x02, ///< configure 6 data bits
	ZO_UART_DATA_BITS_7 = 0x04, ///< configure 7 data bits
	ZO_UART_DATA_BITS_8 = 0x06, ///< configure 8 data bits
	ZO_UART_DATA_BITS_9 = 0x0E  ///< configure 9 data bits
}ZO_UART_DATA_BITS;

///Uart stop-bits configuration enumeration. Select one of the enumeration values as an 
///argument to the zoUartSetStopBits() function.
typedef enum {
	ZO_UART_STOP_BITS_1=0x00,   ///< configure 1 stop bit
	ZO_UART_STOP_BITS_2=0x80    ///< configure 2 stop bits
}ZO_UART_STOP_BITS;

//Default settings________________________________________________________________________
#define ZO_UART_DEFAULT_BAUD_RATE						115200
#define ZO_UART_DEFAULT_DATA_BITS						ZO_UART_DATA_BITS_8
#define ZO_UART_DEFAULT_PARITY							ZO_UART_PARITY_NONE
#define ZO_UART_DEFAULT_STOP_BITS						ZO_UART_STOP_BITS_1
#define ZO_UART_DEFAULT_TRANSMIT_TIMEOUT_MILISECONDS	1000
#define ZO_UART_DEFAULT_BUFFER_SIZE						32

//Error codes_____________________________________________________________________________
#define	ZO_UART_ERROR_NONE						0x00

///When attempting to initialize the Uart there wasn't enough available memory.
///This means that memory of the AVR processor is used up. To resolve this problem:
///Optimize user code for memory consumption or use an AVR with more RAM memory :)
#define	ZO_UART_ERROR_MEMORY_ALLOCATION_ERROR	0x01

///The received uart frame had some error: ie more or less data bits than configured, more
///or less stop-bits than configured, frame was never completed and so on. 
///To resolve this error first double-check the uart settings of the device you are 
///communicating. Then check cabling. Then try a reduction of baud rate.
#define	ZO_UART_ERROR_RX_FRAME_ERROR			0x02

///The received frame had invalid parity. To resolve this error follow the same procedure 
///described for the frame error.
#define	ZO_UART_ERROR_RX_PARITY_ERROR			0x03

///The receive buffer has overflowed. This means that the device connected to our AVR 
///sends data faster than user code can handle. To resolve this error call zoUartGetChar()
///more often or reduce the baud rate of both the AVR and the connected device.
#define	ZO_UART_ERROR_RX_BUFFER_OVERFLOW		0x04

///The hardware receive register of the AVR processor has been overwritten, ie new data
///came in from the uart before the ISR implemented in the zoUart.c file had the time to
///read the receive register. This means that there is not enough ISR time. To resolve 
///this error try optimizing the user ISRs for speed or even shutting down some of them.
#define	ZO_UART_ERROR_RX_DATA_OVERRUN			0x05

///The software has waited more than the Transmit TimeOut for a space in the transmit 
///buffer. This is not a serious error since it is only natural for an AVR, running at 
///several MHz, to put bytes in a buffer much faster than the uart peripheral can send 
///them. After all the uart is sending bytes with a rate of some kilo-BITS per second...
#define	ZO_UART_ERROR_TX_TIMEOUT				0x06

///Error buffer used to store errors occuring during function or ISR execution.
extern ZO_ERROR zoUartError;

//Function declarations___________________________________________________________________
/*! \brief Initializes the USART0 peripheral, byte storage etc. Always call this
    function prior to any other in this library file.
    \return True if the memory initialization for uart was successfull */
bool zoUartInit(void);

/*! \brief Sets the uart baud rate in bits per second.
    \param baudRate Baud rate in Bps.
    \return True if the baud rate can be achieved by the USART0 peripheral. */
bool zoUartSetBaud(u32 baudRate);

/*! \brief Sets the uart frame data bits. 
    \param dataBits Use one of the ZO_UART_DATA_BITS enumeration
    members as an argument to this function*/
void zoUartSetDataBits(ZO_UART_DATA_BITS dataBits);

/*! \brief Sets the uart frame parity. 
    \param parity Use one of the ZO_UART_PARITY enumeration
    members as an argument to this function*/
void zoUartSetParity(ZO_UART_PARITY parity);

/*! \brief Sets the uart frame stop bits. 
    \param stopBits Use one of the ZO_UART_STOP_BITS 
    enumeration members as an argument to this function*/
void zoUartSetStopBits(ZO_UART_STOP_BITS stopBits);

/*! \brief Configures the time that the MCU waits for a space in the transmit buffer. 
    \param miliseconds Time to wait in miliseconds. Maximum value is 65535. */
void zoUartSetTransmitTimeOut(u16 miliseconds);

/*! \brief Configures the size of the transmit and the receive buffers. 
    \param size The size of each buffer in bytes.*/
bool zoUartSetBuffersSize(u08 size);

/*! \brief Configure the port and pin connected to the RE/DE pin of a half-duplex RS485
    onboard trancseiver (if one is used). For example if the RE/DE pin of such a device is
    connected on PORTD, PD2 then initialize its use like this: 
    \code  zoUartInitRs485(&PORTD,PD2); \endcode 
    \param *port pointer to AVR port connected to the RS485 RE/DE pin.
    \param pinConnectedToReDe the specific pin of the port.
    */
void zoUartInitRs485(volatile u08 *port, u08 pinConnectedToReDe);

/*! \brief Sends a byte over uart. The byte is first put in the transmit buffer. The uart
    transmit ISR handles the actual transmission of the byte.
    \param c The byte to send over uart. 
    \return False if the MCU waited more than the Transmit Timeout to put the byte in the
    transmit buffer. */
bool zoUartPutChar(const u08 c);

/*! \brief Receives a byte over uart. The byte is taken from the receive buffer. The uart 
    receive ISR handles incoming data and puts them in the receive buffer.Usage example:
    \code 
    u08 c;
    while(!zoUartGetChar(&c))
        zoUartPutChar(c);
    \endcode
    The above code waits until a byte is received through uart. When it is it transmits it
    back to the secder.
    \param *byte Pointer to the byte that was received. 
    \return False if the receive buffer was empty (ie no data received at this function
    call) or if there was a reception error. */
bool zoUartGetChar(u08 *byte);

/*! \brief Sends a string over uart. The string must be terminated with a null. 
    \param *buffer Pointer to the string to be sent. 
    \return False if the MCU timed out on waiting for a space in the transmit buffer. */
bool zoUartPutString(const char *buffer);

/*! \brief Sends a byte buffer over uart. 
    \param *buffer Pointer to the byte buffer to be sent. 
    \param  length The number of bytes to send.
    \return False if the MCU timed out on waiting for a space in the transmit buffer. */
bool zoUartPutData(const u08 *buffer, u08 length);

/*! \brief Checks if the transmit buffer is full. 
    \return True if the uart transmit buffer is full, false if it isn't. */
bool zoUartTxBufferIsFull(void);

/*! \brief Checks if the receive buffer is empty. 
    \return True if the uart receive buffer is empty, false if it isn't. */
bool zoUartRxBufferIsEmpty(void);

/*! \brief Flushes (ie discards) all bytes in the receive buffer. */
void zoUartRxFlush(void);

#endif // ZO_AVR_UART_H
//@}
