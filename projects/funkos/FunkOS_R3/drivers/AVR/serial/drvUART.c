//---------------------------------------------------------------------------
// FunkOS - Copyright (c) 2009, Funkenstein Software, See license.txt for details
//---------------------------------------------------------------------------
// File:  drvUART.c
//
// Description:
//	Buffered UART driver for Atmel AVR microcontrollers
//--------------------------------------------------------------------------- 

#include "types.h"
#include "drvUART.h"
#include "driver.h"

#include <avr\io.h>
#include <avr\interrupt.h>

//---------------------------------------------------------------------------
static void UART_SetBaud(DRIVER_STRUCT *pstDriver_);
static BOOL UART_IsTxActive(void);
static BOOL UART_IsRxActive(void);
static void UART_StartTx(void);

static UART_DRIVER_STRUCT *pstDriverCopy;	//!< Driver copy - necessary for use with ISRs.

//---------------------------------------------------------------------------
static void UART_SetBaud(DRIVER_STRUCT *pstDriver_)
{
	UART_DRIVER_STRUCT *pstUART = (UART_DRIVER_STRUCT*)pstDriver_;
	USHORT usBaudTemp;
	
	// Calculate the baud rate from the value in the driver.	
	usBaudTemp = (USHORT)(((SYSTEM_FREQ/16)/pstUART->ulBaudRate) - 1);

	// Save the current port config registers
	usBaudTemp = UART_SRB;
	
	// Clear the registers (disable rx/tx/interrupts)
	UART_SRB = 0;
	UART_SRA = 0;
	
	// Set the baud rate high/low values.
	UART_BAUDH = (UCHAR)(usBaudTemp >> 8);
	UART_BAUDL = (UCHAR)(usBaudTemp & 0x00FF);
	
	// Restore the Rx/Tx flags to their previous state
	UART_SRB = usBaudTemp;
}

//---------------------------------------------------------------------------
void UART_Init(DRIVER_STRUCT *pstDriver_)
{
	UART_DRIVER_STRUCT *pstUART = (UART_DRIVER_STRUCT*)pstDriver_;
	
	// Pointer to the driver used by the interrupts, local to the module.
	pstDriverCopy = (UART_DRIVER_STRUCT*)pstDriver_;
	
	// Set up the FIFOs
	pstUART->ucTxHead = 0;	
	pstUART->ucTxTail = 0;
	pstUART->ucRxHead = 0;
	pstUART->ucRxTail = 0;
	pstUART->bRxOverflow = FALSE;
	// Clear flags
	UART_SRA = 0;
	UART_SRB = 0;
	
	// Set frame format: 8 N 1
	UART_SRC = 0x06;		
}

//---------------------------------------------------------------------------
void UART_Start(DRIVER_STRUCT *pstDriver_)
{
	// Enable Rx/Tx + Interrupts
	UART_SRB = (1 << UART_RXEN) | ( 1 << UART_TXEN);
	UART_SRB |= (1 << UART_TXCIE) | (1 << UART_RXCIE);	
}

//---------------------------------------------------------------------------
void UART_Stop(DRIVER_STRUCT *pstDriver_)
{
	// Disable Rx/Tx + Interrupts 
	UART_SRB &= ~((1 << UART_RXEN) | ( 1 << UART_TXEN));
	UART_SRB &= ~((1 << UART_TXCIE) | (1 << UART_RXCIE));
}

//---------------------------------------------------------------------------
DRIVER_RETURN UART_Control(DRIVER_STRUCT *pstDriver_, USHORT usID_, void *pvData_)
{
	UART_DRIVER_STRUCT *pstUART = (UART_DRIVER_STRUCT*)pstDriver_;	
	switch ((CMD_UART)usID_)
	{
		case CMD_SET_BAUDRATE:
		{
			ULONG ulBaudRate = *((ULONG*)pvData_);
			pstUART->ulBaudRate = ulBaudRate;						
			UART_SetBaud(pstDriver_);			
		}
			break;
		default:
			break;
	}
}

//---------------------------------------------------------------------------
USHORT UART_Read(DRIVER_STRUCT *pstDriver_, UCHAR *pucData_, USHORT usLen_)
{
	// Read a string of characters of length N.  Return the number of bytes
	// actually read.  If less than the true length, this indicates that
	// the buffer is full and that the app needs to wait.
	UART_DRIVER_STRUCT *pstUART = (UART_DRIVER_STRUCT*)pstDriver_;	
	USHORT i = 0;
	USHORT usRead = 0;
	UCHAR ucNext;
	BOOL bExit = FALSE;
	
	for (i = 0; i < usLen_; i++)
	{		
		// If Tail != Head, there's data in the buffer.
		CS_ENTER();
		if (pstUART->ucRxTail != pstUART->ucRxHead)
		{
			// We have room to add the byte, so add it.
			pucData_[i] = pstUART->pucRxBuffer[pstUART->ucRxTail];
			
			// Update the buffer head pointer.
			pstUART->ucRxTail = (pstUART->ucRxTail + 1) % pstUART->ucRxSize;
			usRead++;
		}
		else
		{
			// Can't do anything else - the buffer is empty
			bExit = TRUE; 
		} 
		CS_EXIT();
		
		// If we have to bail because the buffer is empty, do it now.
		if (bExit == TRUE)
		{
			break;
		}		
	}
	return usRead;
}

//---------------------------------------------------------------------------
USHORT UART_Write(DRIVER_STRUCT *pstDriver_, UCHAR *pucData_, USHORT usLen_)
{
	// Write a string of characters of length N.  Return the number of bytes
	// actually written.  If less than the true length, this indicates that
	// the buffer is full and that the app needs to wait.
	UART_DRIVER_STRUCT *pstUART = (UART_DRIVER_STRUCT*)pstDriver_;	
	USHORT i = 0;
	USHORT usWritten = 0;
	UCHAR ucNext;
	BOOL bActivate = FALSE;
	BOOL bExit = FALSE;
	
	// If the head = tail, we need to start sending data out the data ourselves.
	if (pstUART->ucTxHead == pstUART->ucTxTail)
	{
		bActivate = TRUE;
	}
	
	for (i = 0; i < usLen_; i++)
	{
		CS_ENTER();
		// Check that head != tail (we have room)
		ucNext = (pstUART->ucTxHead + 1) % pstUART->ucTxSize;
		
		if (ucNext != pstUART->ucTxTail)
		{
			// We have room to add the byte, so add it.
			pstUART->pucTxBuffer[pstUART->ucTxHead] = pucData_[i];
			
			// Update the buffer head pointer.
			pstUART->ucTxHead = ucNext;
			usWritten++;
		}
		else
		{
			// Can't do anything - buffer is full
			bExit = TRUE;
		} 
		CS_EXIT();
		
		// bail if the buffer is full
		if (bExit == TRUE)
		{
			break;
		}		
	}
	
	// Activate the transmission if we're currently idle
	if (bActivate == TRUE)
	{
		// We know we're idle if we get here.
		CS_ENTER();
		UART_StartTx();
		CS_EXIT();
	}
	
	return usWritten;
}

//---------------------------------------------------------------------------
// Return TRUE if UART TX is busy, FALSE otherwise
static BOOL UART_IsTxActive(void)
{
	return (BOOL)(!(UART_SRA & ( 1<< UART_UDRE)));
}
//---------------------------------------------------------------------------
// Return TRUE if UART RX is busy, FALSE otherwise
static BOOL UART_IsRxActive(void)
{
	return (BOOL)(!(UART_SRA & (1 << UART_RXC)));
}
//---------------------------------------------------------------------------
static void UART_StartTx(void)
{
	// Send the tail byte out.
	UCHAR ucNext;
	UCHAR ucTemp;

	// Send the byte at the tail index
	UART_UDR = pstDriverCopy->pucTxBuffer[pstDriverCopy->ucTxTail];
	
	// Update the tail index
	ucNext = (pstDriverCopy->ucTxTail + 1) % pstDriverCopy->ucTxSize;
	pstDriverCopy->ucTxTail = ucNext;		
}

//---------------------------------------------------------------------------
ISR(UART_RX_ISR)
{
	UCHAR ucTemp;
	UCHAR ucNext;
	
	// Read the byte from the data buffer register
	ucTemp = UART_UDR;

	ucNext = pstDriverCopy->ucRxHead;
	// Push the byte into our receive buffer.
		
	// Check that head != tail (we have room)
	ucNext = (pstDriverCopy->ucRxHead + 1) % pstDriverCopy->ucRxSize;
	
	// Always add the byte to the buffer (but flag an error if it's full...)
	pstDriverCopy->pucRxBuffer[pstDriverCopy->ucRxHead] = ucTemp;
	
	// Update the buffer head pointer.
	pstDriverCopy->ucRxHead = ucNext;
	
	// If the buffer's full, discard the oldest byte in the buffer and flag an error
	if (ucNext == pstDriverCopy->ucRxTail)
	{	
		// Update the buffer tail pointer
		pstDriverCopy->ucRxTail = (pstDriverCopy->ucRxTail + 1) % pstDriverCopy->ucRxSize;

		// Flag an error - the buffer is full
		pstDriverCopy->bRxOverflow = TRUE;
	}
}

//---------------------------------------------------------------------------
ISR(UART_TX_ISR)
{
	// If the head != tail, there's something to send.
	if (pstDriverCopy->ucTxHead != pstDriverCopy->ucTxTail)
	{		
		UART_StartTx();
	}
	// Otherwise, there's nothing to send, so we're good for now...
}

