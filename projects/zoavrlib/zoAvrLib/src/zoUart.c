#include <avr/io.h>
#include <avr/interrupt.h>
#include "zoTypes.h"
#include "zoUart.h"
#include "zoMcu.h"
#include "zoBuffer.h"
#include "zoSystemTimer.h"

#define DATA_BITS_MASK_UCSR0C	0x06
#define DATA_BITS_MASK_UCSR0B	0x04
#define PARITY_BITS_MASK		0x30
#define STOP_BITS_MASK			0x80

//__Globals_______________________________________________________________________________
static ZO_BUFFER RxBuffer = ZO_BUFFER_DEFAULTS ;
static ZO_BUFFER TxBuffer = ZO_BUFFER_DEFAULTS ;
static u16 TransmitTimeOut = ZO_UART_DEFAULT_TRANSMIT_TIMEOUT_MILISECONDS;

static bool Rs485Used = FALSE;
static volatile u08* Rs485ReDePort;
static volatile u08 Rs485ReDePin;

ZO_ERROR zoUartError = ZO_ERROR_DEFAULTS;

//Implementation__________________________________________________________________________
bool zoUartInit(void)
{
	UCSR0C = 0;

	if(!zoUartSetBuffersSize(ZO_UART_DEFAULT_BUFFER_SIZE))	//initialize buffers
		return FALSE;

	zoErrorInit(&zoUartError);								//initialize the error storage
	zoSystemTimerInit();

   //general settings
	UCSR0A |= _BV(U2X0);									//double speed mode
	UCSR0A &= ~_BV(MPCM0);									//no multiprocessor
	UCSR0C &= ~(_BV(UMSEL01)|_BV(UMSEL00));					//asynchronous USART mode

	zoUartSetBaud(ZO_UART_DEFAULT_BAUD_RATE);
	zoUartSetDataBits(ZO_UART_DEFAULT_DATA_BITS);
	zoUartSetParity(ZO_UART_DEFAULT_PARITY);
	zoUartSetStopBits(ZO_UART_DEFAULT_STOP_BITS);

	UCSR0B |= _BV(TXEN0) | _BV(RXEN0) | _BV(RXCIE0) ;	//enable transmit,receive and receive interrupt
	zoUartRxFlush();

	sei();

   return TRUE;
}

bool zoUartSetBaud(u32 baudRate)
{
	u32 ubrrReg = 0;

	//configure baud rate
	ubrrReg = (F_CPU/baudRate/8 - 1);
	if( ( ubrrReg > 65535) || ( ubrrReg < 1 ) )
		return FALSE;

	UBRR0H = (u08)((ubrrReg >> 8) & 0x00FF);		//baud rate divisor high byte
	UBRR0L = (u08)(ubrrReg & 0x00FF);				//baud rate divisor low byte

	return TRUE;
}

inline void zoUartSetDataBits(ZO_UART_DATA_BITS dataBits)
{
	UCSR0C = (UCSR0C & ~DATA_BITS_MASK_UCSR0C) | (dataBits & DATA_BITS_MASK_UCSR0C);
	UCSR0B = (UCSR0B & ~DATA_BITS_MASK_UCSR0B) | ((dataBits>>1) & DATA_BITS_MASK_UCSR0B);
}

inline void zoUartSetParity(ZO_UART_PARITY parity)
{
	UCSR0C = (UCSR0C & ~PARITY_BITS_MASK)|parity;
}

inline void zoUartSetStopBits(ZO_UART_STOP_BITS stopBits)
{
	UCSR0C = (UCSR0C & ~STOP_BITS_MASK)|stopBits;
}

void zoUartInitRs485(volatile u08 *port, u08 pinConnectedToReDe)
{
	Rs485Used = TRUE;
	Rs485ReDePort = port;
	Rs485ReDePin = pinConnectedToReDe;

	*(port-1) |= _BV(pinConnectedToReDe);	//configure DDR register
	*port &= ~_BV(pinConnectedToReDe);		//configure PORT register 
											//reset reDePin -> receive mode
}

inline bool zoUartSetBuffersSize(u08 size)
{
	if ( zoBufferInit(&RxBuffer,size) && zoBufferInit(&TxBuffer,size) )
		return TRUE;
	else
	{
		zoErrorPut(&zoUartError,ZO_UART_ERROR_MEMORY_ALLOCATION_ERROR);
		return FALSE;
	}
}

inline void zoUartSetTransmitTimeOut(u16 miliseconds)
{
	TransmitTimeOut = miliseconds;
}

inline bool zoUartTxBufferIsFull(void)
{
	return zoBufferIsFull(&TxBuffer);
}

inline bool zoUartRxBufferIsEmpty(void)
{
	return zoBufferIsEmpty(&RxBuffer);
}

void zoUartRxFlush(void)
{
	u08  dummy;

    while (bit_is_set(UCSR0A, RXC0))
         dummy = UDR0;

	zoBufferFlush(&RxBuffer);
}

bool zoUartPutChar(const u08 c)
{
	u16 timer;
	
	//wait with timeout for a space in the transmit buffer
	zoSystemTimerTimeOutInit(&timer);
    while(zoBufferIsFull(&TxBuffer))
	{
		if(zoSystemTimerTimeOutExpired(&timer,TransmitTimeOut))
		{
			zoErrorPut(&zoUartError,ZO_UART_ERROR_TX_TIMEOUT);
			return FALSE;
		}
	}
	
	zoBufferPut(&TxBuffer,c);

	if(TxBuffer.ctr == 1)						// See if this is the first character
		UCSR0B |= _BV(UDRIE0);					// Yes, Enable Tx interrupts

	if(Rs485Used)
		*Rs485ReDePort |= _BV(Rs485ReDePin);

	return TRUE;
}

bool zoUartGetChar(u08* byte)
{
	if(!zoErrorIsEmpty(&zoUartError))		//check if receive ISR got some error
		return FALSE;

    if( zoUartRxBufferIsEmpty() )
        return FALSE;

	*byte = zoBufferGet(&RxBuffer);

	return TRUE;
}

bool zoUartPutString(const char* buffer)
{
	while(*buffer != '\0')
	{
		if(!zoUartPutChar(*buffer++))
			return FALSE;
	}
	return TRUE;
}

bool zoUartPutData(const u08* buffer, u08 length)
{
	u08 i;

	for(i=0;i<length;i++)
	{
		if(!zoUartPutChar(buffer[i]))
			return FALSE;
	}
	return TRUE;
}

ISR(USART_RX_vect)
{
    u08   c;

	if ((UCSR0A & _BV(FE0)) != 0x00)					//is there a frame error?
		zoErrorPutIsr(&zoUartError,ZO_UART_ERROR_RX_FRAME_ERROR);	//Yes->indicate it.

	if ((UCSR0A & _BV(UPE0)) != 0x00)					//is there a parity error?
		zoErrorPutIsr(&zoUartError,ZO_UART_ERROR_RX_PARITY_ERROR);	//Yes->indicate it.

	if ( (UCSR0A & _BV(DOR0)) != 0x00 )					//Is there data overrun?
		zoErrorPutIsr(&zoUartError,ZO_UART_ERROR_RX_DATA_OVERRUN);	//Yes->indicate it.
														 
	//Above three bits are cleared automatically when UDR0 is read.
	c  = UDR0;											

    if( !zoBufferIsFull(&RxBuffer) )
		zoBufferPutIsr(&RxBuffer,c);
	else
		zoErrorPutIsr(&zoUartError,ZO_UART_ERROR_RX_BUFFER_OVERFLOW);
}

ISR(USART_UDRE_vect)
{
	if(!zoBufferIsEmpty(&TxBuffer))
		UDR0 = zoBufferGetIsr(&TxBuffer);
	else
	{
        UCSR0B &= ~_BV(UDRIE0);		// Buffer empty, Disable Tx interrupts

		if(Rs485Used)					// this is for 485 mode only
			UCSR0B |= _BV(TXCIE0);		// Enable Transmit Complete Interrupt
										// the tx complete interrupt is used to reset 
										// the rs485 rede pin
	}
}

ISR(USART_TX_vect)
{
	if(Rs485Used)
	{
		if (TxBuffer.ctr == 0)
		{
			*Rs485ReDePort &=~_BV(Rs485ReDePin);	// Clear RS485 Pin for receive mode
			UCSR0B &=~ _BV(TXCIE0);					// Disable trasnmit complete interrupt
		}
	}
}