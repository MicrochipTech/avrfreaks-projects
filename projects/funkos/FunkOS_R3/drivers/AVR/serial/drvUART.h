//---------------------------------------------------------------------------
// FunkOS - Copyright (c) 2009, Funkenstein Software, See license.txt for details
//---------------------------------------------------------------------------
// File:  drvUART.h
//
// Description:
//	Buffered UART driver for Atmel AVR microcontrollers
//--------------------------------------------------------------------------- 

#ifndef __DRVUART_H_
#define __DRVUART_H_

#include "types.h"
#include "driver.h"

//---------------------------------------------------------------------------
// UART defines - user-configurable for different targets
//---------------------------------------------------------------------------
#define UART_SRA				(UCSR0A)
#define UART_SRB				(UCSR0B)
#define UART_SRC				(UCSR0C)
#define UART_BAUDH				(UBRR0H)
#define UART_BAUDL				(UBRR0L)
#define UART_RXEN				(RXEN0)
#define UART_TXEN				(TXEN0)
#define UART_TXCIE				(TXCIE0)
#define UART_RXCIE				(RXCIE0)
#define UART_UDR				(UDR0)
#define UART_UDRE				(UDRE0)
#define UART_RXC				(RXC0)

#define UART_RX_ISR				(USART_RX_vect)
#define UART_TX_ISR				(USART_TX_vect)

//---------------------------------------------------------------------------
typedef enum
{
	CMD_SET_BAUDRATE,
} CMD_UART;

//---------------------------------------------------------------------------
typedef struct
{
	DRIVER_STRUCT stDriver;		//!< Must be first!!
	
	UCHAR ucTxSize;				//!< Size of the TX Buffer
	UCHAR ucTxHead;				//!< Head index
	UCHAR ucTxTail;				//!< Tail index 
	
	UCHAR ucRxSize;				//!< Size of the RX Buffer
	UCHAR ucRxHead;				//!< Head index
	UCHAR ucRxTail;				//!< Tail index
	
	BOOL bRxOverflow;			//!< Receive buffer overflow
	
	UCHAR *pucRxBuffer;			//!< Receive buffer pointer
	UCHAR *pucTxBuffer;			//!< Transmit buffer pointer
	
	ULONG ulBaudRate;			//!< Baud rate
	
} UART_DRIVER_STRUCT;

//---------------------------------------------------------------------------
void UART_Init(DRIVER_STRUCT *pstDriver_);
void UART_Start(DRIVER_STRUCT *pstDriver_);
void UART_Stop(DRIVER_STRUCT *pstDriver_);
DRIVER_RETURN UART_Control(DRIVER_STRUCT *pstDriver_, USHORT usID_, void *pvData_);
USHORT UART_Read(DRIVER_STRUCT *pstDriver_, UCHAR *pucData_, USHORT usLen_);
USHORT UART_Write(DRIVER_STRUCT *pstDriver_, UCHAR *pucData_, USHORT usLen_);

#endif 
