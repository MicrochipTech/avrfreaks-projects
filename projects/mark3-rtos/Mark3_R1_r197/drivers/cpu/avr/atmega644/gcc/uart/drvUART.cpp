/*===========================================================================
     _____        _____        _____        _____
 ___|    _|__  __|_    |__  __|__   |__  __| __  |__  ______
|    \  /  | ||    \      ||     |     ||  |/ /     ||___   |
|     \/   | ||     \     ||     \     ||     \     ||___   |
|__/\__/|__|_||__|\__\  __||__|\__\  __||__|\__\  __||______|
    |_____|      |_____|      |_____|      |_____|

--[Mark3 Realtime Platform]--------------------------------------------------

Copyright (c) 2012-2015 Funkenstein Software Consulting, all rights reserved.
See license.txt for more information
===========================================================================*/
/*!

    \file   drvUART.cpp

    \brief  Atmega328p serial port driver
*/

#include "kerneltypes.h"
#include "drvUART.h"
#include "driver.h"
#include "thread.h"
#include "threadport.h"
#include "kerneltimer.h"

#include <avr/io.h>
#include <avr/interrupt.h>

//---------------------------------------------------------------------------
static ATMegaUART *pclActive;    // Pointer to the active object

//---------------------------------------------------------------------------
void ATMegaUART::SetBaud(void)
{    
    K_USHORT usBaudTemp;
    K_USHORT usPortTemp;
    
    // Calculate the baud rate from the value in the driver.    
    usBaudTemp = (K_USHORT)(((SYSTEM_FREQ/16)/m_ulBaudRate) - 1);

    // Save the current port config registers
    usPortTemp = UART_SRB;
    
    // Clear the registers (disable rx/tx/interrupts)
    UART_SRB = 0;
    UART_SRA = 0;
    
    // Set the baud rate high/low values.
    UART_BAUDH = (K_UCHAR)(usBaudTemp >> 8);
    UART_BAUDL = (K_UCHAR)(usBaudTemp & 0x00FF);
    
    // Restore the Rx/Tx flags to their previous state
    UART_SRB = usPortTemp;
}

//---------------------------------------------------------------------------
void ATMegaUART::Init(void)
{    
    // Set up the FIFOs
    m_ucTxHead = 0;    
    m_ucTxTail = 0;
    m_ucRxHead = 0;
    m_ucRxTail = 0;
    m_bEcho = 0;
    m_ucRxEscape = '\n';
    pfCallback = NULL;
    m_bRxOverflow = 0;
    m_ulBaudRate = UART_DEFAULT_BAUD;
    
    // Clear flags
    UART_SRA = 0;
    UART_SRB = 0;
    
    SetBaud();
    
    // Set frame format: 8 N 1
    UART_SRC = 0x06;        
}

//---------------------------------------------------------------------------
K_UCHAR ATMegaUART::Open()
{
  
    // Enable Rx/Tx + Interrupts
    UART_SRB |= (1 << UART_RXEN) | ( 1 << UART_TXEN);
    UART_SRB |= (1 << UART_RXCIE) | (1 << UART_TXCIE);
    pclActive = this;
    return 0;
}

//---------------------------------------------------------------------------
K_UCHAR ATMegaUART::Close(void)
{
    // Disable Rx/Tx + Interrupts 
    UART_SRB &= ~((1 << UART_RXEN) | ( 1 << UART_TXEN));
    UART_SRB &= ~((1 << UART_TXCIE) | (1 << UART_RXCIE));
    return 0;
}

//---------------------------------------------------------------------------
K_USHORT ATMegaUART::Control( K_USHORT usCmdId_, void *pvIn_, K_USHORT usSizeIn_, void *pvOut_, K_USHORT usSizeOut_)
{
    switch ((CMD_UART)usCmdId_)
    {
        case CMD_SET_BAUDRATE:
        {
            K_ULONG ulBaudRate = *((K_ULONG*)pvIn_);
            m_ulBaudRate = ulBaudRate;
            SetBaud();
        }
            break;
        case CMD_SET_BUFFERS:
        {
            m_pucRxBuffer = (K_UCHAR*)pvIn_;
            m_pucTxBuffer = (K_UCHAR*)pvOut_;
            m_ucRxSize = usSizeIn_;
            m_ucTxSize = usSizeOut_;
        }            
            break;        
        case CMD_SET_RX_ESCAPE:
        {
            m_ucRxEscape = *((K_UCHAR*)pvIn_);
        }
            break;
        case CMD_SET_RX_CALLBACK:
        {
            pfCallback = (UART_Rx_Callback_t)pvIn_;            
        }
            break;
        case CMD_SET_RX_ECHO:
        {
            m_bEcho = *((K_UCHAR*)pvIn_);
        }
            break;
        case CMD_SET_RX_ENABLE:
        {
            UART_SRB |= (1 << UART_RXEN);
        }
            break;
        case CMD_SET_RX_DISABLE:
        {
            UART_SRB &= ~(1 << UART_RXEN);
        }
            break;                        
        default:
            break;
    }
    return 0;
}

//---------------------------------------------------------------------------
K_USHORT ATMegaUART::Read( K_USHORT usSizeIn_, K_UCHAR *pvData_ )
{
    // Read a string of characters of length N.  Return the number of bytes
    // actually read.  If less than the 1 length, this indicates that
    // the buffer is full and that the app needs to wait.
    
    K_USHORT i = 0;
    K_USHORT usRead = 0;
    bool bExit = 0;
    K_UCHAR *pucData = (K_UCHAR*)pvData_;
    
    for (i = 0; i < usSizeIn_; i++)
    {        
        // If Tail != Head, there's data in the buffer.
        CS_ENTER();
        if (m_ucRxTail != m_ucRxHead)
        {
            // We have room to add the byte, so add it.
            pucData[i] = m_pucRxBuffer[m_ucRxTail];
            
            // Update the buffer head pointer.
            m_ucRxTail++;
            if (m_ucRxTail >= m_ucRxSize) 
            {
                m_ucRxTail = 0;
            }
            usRead++;
        }
        else
        {
            // Can't do anything else - the buffer is empty
            bExit = 1; 
        } 
        CS_EXIT();
        
        // If we have to bail because the buffer is empty, do it now.
        if (bExit == 1)
        {
            break;
        }        
    }
    return usRead;
}

//---------------------------------------------------------------------------
K_USHORT ATMegaUART::Write(K_USHORT usSizeOut_, K_UCHAR *pvData_)
{
    // Write a string of characters of length N.  Return the number of bytes
    // actually written.  If less than the 1 length, this indicates that
    // the buffer is full and that the app needs to wait.    
    K_USHORT i = 0;
    K_USHORT usWritten = 0;
    K_UCHAR ucNext;
    bool bActivate = 0;
    bool bExit = 0;
    K_UCHAR *pucData = (K_UCHAR*)pvData_;
    
    // If the head = tail, we need to start sending data out the data ourselves.
    if (m_ucTxHead == m_ucTxTail)
    {
        bActivate = 1;
    }
    
    for (i = 0; i < usSizeOut_; i++)
    {
        CS_ENTER();
        // Check that head != tail (we have room)
        ucNext = (m_ucTxHead + 1);
        if (ucNext >= m_ucTxSize)
        {
            ucNext = 0;
        }
        
        if (ucNext != m_ucTxTail)
        {
            // We have room to add the byte, so add it.
            m_pucTxBuffer[m_ucTxHead] = pucData[i];
            
            // Update the buffer head pointer.
            m_ucTxHead = ucNext;
            usWritten++;
        }
        else
        {
            // Can't do anything - buffer is full
            bExit = 1;
        } 
        CS_EXIT();
        
        // bail if the buffer is full
        if (bExit == 1)
        {
            break;
        }        
    }
    
    // Activate the transmission if we're currently idle
    if (bActivate == 1)
    {
        // We know we're idle if we get here.
        CS_ENTER();
        if (UART_SRA & (1 << UDRE0))
        {
            StartTx();
        }        
        CS_EXIT();
    }
    
     return usWritten;
}

//---------------------------------------------------------------------------
void ATMegaUART::StartTx(void)
{
    // Send the tail byte out.
    K_UCHAR ucNext;

    CS_ENTER();
    
    // Send the byte at the tail index
    UART_UDR = m_pucTxBuffer[m_ucTxTail];
    
    // Update the tail index
    ucNext = (m_ucTxTail + 1);
    if (ucNext >= m_ucTxSize)
    {
        ucNext = 0;
    }
    m_ucTxTail = ucNext;                
    
    CS_EXIT();
}

//---------------------------------------------------------------------------
void ATMegaUART::RxISR()
{
    K_UCHAR ucTemp;
    K_UCHAR ucNext;
    
    // Read the byte from the data buffer register
    ucTemp = UART_UDR;
    
    // Check that head != tail (we have room)
    ucNext = (m_ucRxHead + 1);
    if (ucNext >= m_ucRxSize)
    {
        ucNext = 0;
    }
    
    // Always add the byte to the buffer (but flag an error if it's full...)
    m_pucRxBuffer[m_ucRxHead] = ucTemp;
    
    // Update the buffer head pointer.
    m_ucRxHead = ucNext;
    
    // If the buffer's full, discard the oldest byte in the buffer and flag an error
    if (ucNext == m_ucRxTail)
    {
        // Update the buffer tail pointer
        m_ucRxTail = (m_ucRxTail + 1);
        if (m_ucRxTail >= m_ucRxSize)
        {
            m_ucRxTail = 0;
        }

        // Flag an error - the buffer is full
        m_bRxOverflow = 1;
    }
    
    // If local-echo is enabled, TX the K_CHAR
    if (m_bEcho)
    {
        Write(1, &ucTemp);
    }
    
    // If we've hit the RX callback character, run the callback
    // This is used for calling line-end functions, etc..
    if (ucTemp == m_ucRxEscape)
    {
        if (pfCallback)
        {
            pfCallback(this);
        }
    }
}

//---------------------------------------------------------------------------
ISR(UART_RX_ISR)
{
    pclActive->RxISR();
}

//---------------------------------------------------------------------------
void ATMegaUART::TxISR()
{
    // If the head != tail, there's something to send.
    if (m_ucTxHead != m_ucTxTail)
    {
        StartTx();
    }
}

//---------------------------------------------------------------------------
ISR(UART_TX_ISR)
{
    pclActive->TxISR();
}
