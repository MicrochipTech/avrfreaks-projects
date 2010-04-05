#ifndef ZO_AVR_UART_0_H
#define ZO_AVR_UART_0_H

#include <avr/io.h>
#include <avr/interrupt.h>
#include "zoMcu.h"
#include "zoTypes.h"
#include "zoError.h"

typedef enum {
	ZO_UART_PARITY_NONE = 0x00,
	ZO_UART_PARITY_ODD = 0x30,
	ZO_UART_PARITY_EVEN = 0x20
}ZO_UART_PARITY;

typedef enum {
	ZO_UART_DATA_BITS_5 = 0x00,
	ZO_UART_DATA_BITS_6 = 0x02,
	ZO_UART_DATA_BITS_7 = 0x04,
	ZO_UART_DATA_BITS_8 = 0x06,
	ZO_UART_DATA_BITS_9 = 0x0E
}ZO_UART_DATA_BITS;

typedef enum {
	ZO_UART_STOP_BITS_1=0x00,
	ZO_UART_STOP_BITS_2=0x80
}ZO_UART_STOP_BITS;

//Error codes_____________________________________________________________________________
#define	ZO_UART_ERROR_NONE						0x00	//Function call was successful
#define	ZO_UART_ERROR_MEMORY_ALLOCATION_ERROR	0x01	//During initialization memory was not enough
#define	ZO_UART_ERROR_RX_BUFFER_EMPTY			0x02	//There is no character in buffer or fifo
#define	ZO_UART_ERROR_RX_FRAME_ERROR			0x03	//Frame receive error (check host settings)
#define	ZO_UART_ERROR_RX_PARITY_ERROR			0x04	//Parity receive error (check host settings)
#define	ZO_UART_ERROR_RX_BUFFER_OVERFLOW		0x05	//Buffer overflow, user code needs to call getChar() more frequently
#define	ZO_UART_ERROR_RX_DATA_OVERRUN			0x06	//FIFO overflow, more ISR time is needed
#define	ZO_UART_ERROR_TX_TIMEOUT				0x07	//No space in Tx buffer for transmit_timeout_miliseconds

//Defaults
#define ZO_UART_DEFAULT_BAUD_RATE						115200
#define ZO_UART_DEFAULT_DATA_BITS						ZO_UART_DATA_BITS_8
#define ZO_UART_DEFAULT_PARITY							ZO_UART_PARITY_NONE
#define ZO_UART_DEFAULT_STOP_BITS						ZO_UART_STOP_BITS_1
#define ZO_UART_DEFAULT_TRANSMIT_TIMEOUT_MILISECONDS	1000
#define ZO_UART_DEFAULT_BUFFER_SIZE						32

//error buffer
extern ZO_ERROR zoUartError;

//initialization functions
bool zoUartInit(void);
bool zoUartSetBaud(u32 baudRate);

void zoUartSetDataBits(ZO_UART_DATA_BITS dataBits);
void zoUartSetParity(ZO_UART_PARITY parity);			//0 for none, 1 for odd, 2 for even
void zoUartSetStopBits(ZO_UART_STOP_BITS stopBits);

void zoUartSetTransmitTimeOut(u16 miliseconds);
bool zoUartSetBuffersSize(u08 size);

void zoUartInitRs485(volatile u08 *port, u08 pinConnectedToReDe);

//High level transaction functions
bool zoUartPutChar(const u08 c);
bool zoUartGetChar(u08* byte);
bool zoUartPutString(const u08* buffer);
bool zoUartPutData(const u08* buffer, u08 length);
bool zoUartTxBufferIsFull(void);
bool zoUartRxBufferIsEmpty(void);
void zoUartRxFlush(void);

#endif /* ZO_AVR_UART_0_H */
