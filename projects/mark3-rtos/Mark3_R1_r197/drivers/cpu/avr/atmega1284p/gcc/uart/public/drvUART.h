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

    \file   drvUART.h

    \brief  Atmega328p serial port driver

*/
#ifndef __DRVUART_H_
#define __DRVUART_H_

#include "kerneltypes.h"
#include "driver.h"

//---------------------------------------------------------------------------
// UART defines - user-configurable for different targets
//---------------------------------------------------------------------------
#define UART_SRA                (UCSR0A)
#define UART_SRB                (UCSR0B)
#define UART_SRC                (UCSR0C)
#define UART_BAUDH              (UBRR0H)
#define UART_BAUDL              (UBRR0L)
#define UART_RXEN               (RXEN0)
#define UART_TXEN               (TXEN0)
#define UART_TXCIE              (TXCIE0)
#define UART_RXCIE              (RXCIE0)
#define UART_UDR                (UDR0)
#define UART_UDRE               (UDRE0)
#define UART_RXC                (RXC0)

#define UART_DEFAULT_BAUD       ((K_ULONG)57600)

#define UART_RX_ISR             (USART0_RX_vect)
#define UART_TX_ISR             (USART0_TX_vect)

//---------------------------------------------------------------------------
typedef enum
{
    CMD_SET_BAUDRATE = 0x80,
    CMD_SET_BUFFERS,    
    CMD_SET_RX_ESCAPE,
    CMD_SET_RX_CALLBACK,
    CMD_SET_RX_ECHO,
    CMD_SET_RX_ENABLE,
    CMD_SET_RX_DISABLE
} CMD_UART;

class ATMegaUART;
//---------------------------------------------------------------------------
typedef struct _UartData_t UartData_t;

//---------------------------------------------------------------------------
typedef void (*UART_Rx_Callback_t)( ATMegaUART *pclUART );

//---------------------------------------------------------------------------
/*!
    Implements a UART driver on the ATMega328p
*/
class ATMegaUART : public Driver
{
    
public:        
    virtual void Init();
    virtual K_UCHAR Open();
    virtual K_UCHAR Close();
    virtual K_USHORT Read( K_USHORT usBytes_, 
                                 K_UCHAR *pucData_ );
                                 
    virtual K_USHORT Write( K_USHORT usBytes_, 
                                  K_UCHAR *pucData_ );
                                  
    virtual K_USHORT Control( K_USHORT usEvent_, 
                                    void *pvIn_, 
                                    K_USHORT usSizeIn_, 
                                    void *pvOut_, 
                                    K_USHORT usSizeOut_ );
    /*!
        Called from the transmit complete ISR - implements a 
        callback/transmit state-machine
    */                        
    void TxISR();
    
    /*!
        Called from the receive-complete ISR - implements a 
        callback/receive state-machine
    */
    void RxISR();
    
    /*!         
        \fn K_UCHAR *GetRxBuffer(void)
        
        Return a pointer to the receive buffer for this UART.

        \return pointer to the driver's RX buffer
    */
    K_UCHAR *GetRxBuffer(void) { return m_pucRxBuffer; }
    
    /*! 
        \fn K_UCHAR *GetTxBuffer(void)
        
        Return a pointer to the transmit buffer for this UART.
        
        \return pointer to the driver's TX buffer
    */
    K_UCHAR *GetTxBuffer(void) { return m_pucTxBuffer; }

private:

    void SetBaud(void);
    void StartTx(void);
    
    K_UCHAR m_ucTxSize;                //!< Size of the TX Buffer
    K_UCHAR m_ucTxHead;                //!< Head index
    K_UCHAR m_ucTxTail;                //!< Tail index
    
    K_UCHAR m_ucRxSize;                //!< Size of the RX Buffer
    K_UCHAR m_ucRxHead;                //!< Head index
    K_UCHAR m_ucRxTail;                //!< Tail index
    
    K_UCHAR m_bRxOverflow;              //!< Receive buffer overflow
    K_UCHAR m_bEcho;                    //!< Whether or not to echo RX characters to TX
    
    K_UCHAR *m_pucRxBuffer;            //!< Receive buffer pointer
    K_UCHAR *m_pucTxBuffer;            //!< Transmit buffer pointer
    
    K_ULONG m_ulBaudRate;              //!< Baud rate
    
    K_UCHAR m_ucRxEscape;              //!< Escape character
    
    UART_Rx_Callback_t    pfCallback;    //!< Callback function on matched escape character        
};

#endif 
