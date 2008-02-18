/*This file has been prepared for Doxygen automatic documentation generation.*/
/*! \file *********************************************************************
 *
 * \brief Header file for uart.c.
 *
 * - File:               uart.h
 * - Compiler:           AVR GCC
 * - Supported devices:  All devices with a 16 bit timer can be used.
 *                       The example is written for ATmega8535
 * - AppNote:            AVR446 - Linear speed control of stepper motor
 *
 * \author               Atmel Corporation: http://www.atmel.com \n
 *                       Support email: avr@atmel.com
 *
 * $Name: RELEASE_1_0 $
 * $Revision: 1.2 $
 * $RCSfile: uart.h,v $
 * $Date: 2008/02/18 $
 *****************************************************************************/
#ifndef UART_H
#define UART_H

// UART Buffer Defines
#define UART_RX_BUFFER_SIZE 64 // 2,4,8,16,32,64,128 or 256 bytes
#define UART_RX_BUFFER_MASK ( UART_RX_BUFFER_SIZE - 1 )
#if ( UART_RX_BUFFER_SIZE & UART_RX_BUFFER_MASK )
  #error RX buffer size is not a power of 2
#endif

#define UART_TX_BUFFER_SIZE 64 // 2,4,8,16,32,64,128 or 256 bytes
#define UART_TX_BUFFER_MASK ( UART_TX_BUFFER_SIZE - 1 )
#if ( UART_TX_BUFFER_SIZE & UART_TX_BUFFER_MASK )
  #error TX buffer size is not a power of 2
#endif
 // UDR Empty Interrupt
#define SET_UDRIE (UCSRB |=  (1<<UDRIE))
#define CLR_UDRIE (UCSRB &= ~(1<<UDRIE))

void InitUART(void);
void uart_SendByte(char );
void uart_SendString(char *);
void uart_SendInt(int );
void uart_FlushRxBuffer(void);

//! Buffer with received string from uart.
extern char UART_RxBuffer[UART_RX_BUFFER_SIZE];

#endif
