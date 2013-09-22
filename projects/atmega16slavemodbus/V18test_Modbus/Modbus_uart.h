#ifndef MODBUS_UART_H
#define MODBUS_UART_H
/************************************************************************
Title:    Interrupt UART library with receive/transmit circular buffers
Author:   Peter Fleury <pfleury@gmx.ch>   http://jump.to/fleury
File:     $Id: uart.h,v 1.8.2.1 2007/07/01 11:14:38 peter Exp $
Software: AVR-GCC 4.1, AVR Libc 1.4
Hardware: any AVR with built-in UART, tested on AT90S8515 & ATmega8 at 4 Mhz
License:  GNU General Public License 
Usage:    see Doxygen manual

LICENSE:
    Copyright (C) 2006 Peter Fleury

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
    
************************************************************************/

/************************************************************************
uart_available, uart_flush, uart1_available, and uart1_flush functions
were adapted from the Arduino HardwareSerial.h library by Tim Sharpe on 
11 Jan 2009.  The license info for HardwareSerial.h is as follows:

  HardwareSerial.h - Hardware serial library for Wiring
  Copyright (c) 2006 Nicholas Zambetti.  All right reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
************************************************************************/

/************************************************************************
Changelog for modifications made by Tim Sharpe, starting with the current
  library version on his Web site as of 05/01/2009. 

Date        Description
=========================================================================
05/12/2009  Added Arduino-style available() and flush() functions for both
			supported UARTs.  Really wanted to keep them out of the library, so
			that it would be as close as possible to Peter Fleury's original
			library, but has scoping issues accessing internal variables from
			another program.  Go C!

************************************************************************/

/** 
 *  @defgroup pfleury_uart UART Library
 *  @code #include <uart.h> @endcode
 * 
 *  @brief Interrupt UART library using the built-in UART with transmit and receive circular buffers. 
 *
 *  This library can be used to transmit and receive data through the built in UART. 
 *
 *  An interrupt is generated when the UART has finished transmitting or
 *  receiving a byte. The interrupt handling routines use circular buffers
 *  for buffering received and transmitted data.
 *
 *  The UART_RX_BUFFER_SIZE and UART_TX_BUFFER_SIZE constants define
 *  the size of the circular buffers in bytes. Note that these constants must be a power of 2.
 *  You may need to adapt this constants to your target and your application by adding 
 *  CDEFS += -DUART_RX_BUFFER_SIZE=nn -DUART_RX_BUFFER_SIZE=nn to your Makefile.
 *
 *  @note Based on Atmel Application Note AVR306
 *  @author Peter Fleury pfleury@gmx.ch  http://jump.to/fleury
 */
 
/**@{*/


#if (__GNUC__ * 100 + __GNUC_MINOR__) < 304
#error "This library requires AVR-GCC 3.4 or later, update to newer AVR-GCC compiler !"
#endif

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#define BIT_TIME  (uint16_t)((F_CPU + UART_BAUD_RATE/2) / UART_BAUD_RATE)
#define BIT_PER_BYTE	10
#define TIME_PER_HALF_BYTE	(uint16_t)((BIT_PER_BYTE * BIT_TIME) / 2)

#define BYTE_TIMEOUT_TICS	(uint16_t)(BIT_PER_BYTE * 1.5)
#define FRAME_TIMEOUT_TICS	(uint16_t)(BIT_PER_BYTE * 3.5)

/* receive states  */
typedef enum {
	RS_Wait, RS_Receiving, RS_FrameComplete
} receive_status_t;

/* structure for received data */
typedef struct {
volatile	receive_status_t status;
volatile	uint16_t bitTics;
} uart_status_t;






#if defined(__AVR_AT90S2313__) \
 || defined(__AVR_AT90S4414__) || defined(__AVR_AT90S4434__) \
 || defined(__AVR_AT90S8515__) || defined(__AVR_AT90S8535__) \
 || defined(__AVR_ATmega103__)
/* old AVR classic or ATmega103 with one UART */
#define AT90_UART
#define UART0_RECEIVE_INTERRUPT   UART_RX_vect
#define UART0_TRANSMIT_INTERRUPT  UART_UDRE_vect
#define UART0_STATUS   USR
#define UART0_CONTROL  UCR
#define UART0_DATA     UDR
#define UART0_UDRIE    UDRIE
#elif defined(__AVR_AT90S2333__) || defined(__AVR_AT90S4433__)
/* old AVR classic with one UART */
#define AT90_UART
#define UART0_RECEIVE_INTERRUPT   UART_RX_vect
#define UART0_TRANSMIT_INTERRUPT  UART_UDRE_vect
#define UART0_STATUS   UCSRA
#define UART0_CONTROL  UCSRB
#define UART0_DATA     UDR
#define UART0_UDRIE    UDRIE
#elif  defined(__AVR_ATmega8__)  || defined(__AVR_ATmega16__) || defined(__AVR_ATmega32__) \
  || defined(__AVR_ATmega323__)
/* ATmega with one USART */
#define ATMEGA_USART
#define USART0_TX_vect   		  USART_TXC_vect
#define UART0_RECEIVE_INTERRUPT   USART_RXC_vect
#define UART0_TRANSMIT_INTERRUPT  USART_UDRE_vect
#define UART0_STATUS   UCSRA
#define UART0_CONTROL  UCSRB
#define UART0_DATA     UDR
#define UART0_UDRIE    UDRIE
#define RXCIE0         RXCIE
#define RXEN0	       RXEN
#define TXCIE0	       TXEN
#define UCSR0A		   UCSRA
#define UCSR0B		   UCSRB
#elif  defined(__AVR_ATmega8515__) || defined(__AVR_ATmega8535__)
/* ATmega with one USART */
#define ATMEGA_USART
#define UART0_RECEIVE_INTERRUPT   USART_RX_vect
#define UART0_TRANSMIT_INTERRUPT  USART_UDRE_vect
#define UART0_STATUS   UCSRA
#define UART0_CONTROL  UCSRB
#define UART0_DATA     UDR
#define UART0_UDRIE    UDRIE
#elif defined(__AVR_ATmega163__)
/* ATmega163 with one UART */
#define ATMEGA_UART
#define UART0_RECEIVE_INTERRUPT   UART_RX_vect
#define UART0_TRANSMIT_INTERRUPT  UART_UDRE_vect
#define UART0_STATUS   UCSRA
#define UART0_CONTROL  UCSRB
#define UART0_DATA     UDR
#define UART0_UDRIE    UDRIE
#elif defined(__AVR_ATmega162__)
/* ATmega with two USART */
#define ATMEGA_USART0
#define ATMEGA_USART1
#define UART0_RECEIVE_INTERRUPT   USART0_RXC_vect
#define UART1_RECEIVE_INTERRUPT   USART1_RXC_vect
#define UART0_TRANSMIT_INTERRUPT  USART0_UDRE_vect
#define UART1_TRANSMIT_INTERRUPT  USART1_UDRE_vect
#define UART0_STATUS   UCSR0A
#define UART0_CONTROL  UCSR0B
#define UART0_DATA     UDR0
#define UART0_UDRIE    UDRIE0
#define UART1_STATUS   UCSR1A
#define UART1_CONTROL  UCSR1B
#define UART1_DATA     UDR1
#define UART1_UDRIE    UDRIE1
#elif defined(__AVR_ATmega64__) || defined(__AVR_ATmega128__)
/* ATmega with two USART */
#define ATMEGA_USART0
#define ATMEGA_USART1
#define UART0_RECEIVE_INTERRUPT   USART0_RX_vect
#define UART1_RECEIVE_INTERRUPT   USART1_RX_vect
#define UART0_TRANSMIT_INTERRUPT  USART0_UDRE_vect
#define UART1_TRANSMIT_INTERRUPT  USART1_UDRE_vect
#define UART0_STATUS   UCSR0A
#define UART0_CONTROL  UCSR0B
#define UART0_DATA     UDR0
#define UART0_UDRIE    UDRIE0
#define UART1_STATUS   UCSR1A
#define UART1_CONTROL  UCSR1B
#define UART1_DATA     UDR1
#define UART1_UDRIE    UDRIE1
#elif defined(__AVR_ATmega161__)
/* ATmega with UART */
#error "AVR ATmega161 currently not supported by this libaray !"
#elif defined(__AVR_ATmega169__)
/* ATmega with one USART */
#define ATMEGA_USART
#define UART0_RECEIVE_INTERRUPT   USART0_RX_vect
#define UART0_TRANSMIT_INTERRUPT  USART0_UDRE_vect
#define UART0_STATUS   UCSRA
#define UART0_CONTROL  UCSRB
#define UART0_DATA     UDR
#define UART0_UDRIE    UDRIE
#elif defined(__AVR_ATmega48__) ||defined(__AVR_ATmega88__) || defined(__AVR_ATmega168__) || \
      defined(__AVR_ATmega48P__) ||defined(__AVR_ATmega88P__) || defined(__AVR_ATmega168P__) || \
      defined(__AVR_ATmega328P__)
/* TLS-Added 48P/88P/168P/328P */
/* ATmega with one USART */
#define ATMEGA_USART0
#define UART0_RECEIVE_INTERRUPT   USART_RX_vect
#define UART0_TRANSMIT_INTERRUPT  USART_UDRE_vect
#define UART0_STATUS   UCSR0A
#define UART0_CONTROL  UCSR0B
#define UART0_DATA     UDR0
#define UART0_UDRIE    UDRIE0
#elif defined(__AVR_ATtiny2313__)
#define ATMEGA_USART
#define UART0_RECEIVE_INTERRUPT   USART_RX_vect
#define UART0_TRANSMIT_INTERRUPT  USART_UDRE_vect
#define UART0_STATUS   UCSRA
#define UART0_CONTROL  UCSRB
#define UART0_DATA     UDR
#define UART0_UDRIE    UDRIE
#elif defined(__AVR_ATmega329__) ||\
      defined(__AVR_ATmega649__) ||\
      defined(__AVR_ATmega325__) ||defined(__AVR_ATmega3250__) ||\
      defined(__AVR_ATmega645__) ||defined(__AVR_ATmega6450__)
/* ATmega with one USART */
#define ATMEGA_USART0
#define UART0_RECEIVE_INTERRUPT   USART0_RX_vect
#define UART0_TRANSMIT_INTERRUPT  USART0_UDRE_vect
#define UART0_STATUS   UCSR0A
#define UART0_CONTROL  UCSR0B
#define UART0_DATA     UDR0
#define UART0_UDRIE    UDRIE0
#elif defined(__AVR_ATmega3290__) ||\
      defined(__AVR_ATmega6490__)
/* TLS-Separated these two from the previous group because of inconsistency in the USART_RX */
/* ATmega with one USART */
#define ATMEGA_USART0
#define UART0_RECEIVE_INTERRUPT   USART_RX_vect
#define UART0_TRANSMIT_INTERRUPT  USART0_UDRE_vect
#define UART0_STATUS   UCSR0A
#define UART0_CONTROL  UCSR0B
#define UART0_DATA     UDR0
#define UART0_UDRIE    UDRIE0
#elif defined(__AVR_ATmega2560__) || defined(__AVR_ATmega1280__) || defined(__AVR_ATmega640__)
/* ATmega with two USART */
#define ATMEGA_USART0
#define ATMEGA_USART1
#define UART0_RECEIVE_INTERRUPT   USART0_RX_vect
#define UART1_RECEIVE_INTERRUPT   USART0_UDRE_vect
#define UART0_TRANSMIT_INTERRUPT  USART1_RX_vect
#define UART1_TRANSMIT_INTERRUPT  USART1_UDRE_vect
#define UART0_STATUS   UCSR0A
#define UART0_CONTROL  UCSR0B
#define UART0_DATA     UDR0
#define UART0_UDRIE    UDRIE0
#define UART1_STATUS   UCSR1A
#define UART1_CONTROL  UCSR1B
#define UART1_DATA     UDR1
#define UART1_UDRIE    UDRIE1
#elif defined(__AVR_ATmega644__)
/* ATmega with one USART */
#define ATMEGA_USART0
#define UART0_RECEIVE_INTERRUPT   USART0_RX_vect
#define UART0_TRANSMIT_INTERRUPT  USART0_UDRE_vect
#define UART0_STATUS   UCSR0A
#define UART0_CONTROL  UCSR0B
#define UART0_DATA     UDR0
#define UART0_UDRIE    UDRIE0
#elif defined(__AVR_ATmega164P__) || defined(__AVR_ATmega324P__) || defined(__AVR_ATmega644P__)
/* ATmega with two USART */
#define ATMEGA_USART0
#define ATMEGA_USART1
#define UART0_RECEIVE_INTERRUPT   USART0_RX_vect
#define UART1_RECEIVE_INTERRUPT   USART1_RX_vect
#define UART0_TRANSMIT_INTERRUPT  USART0_UDRE_vect
#define UART1_TRANSMIT_INTERRUPT  USART1_UDRE_vect
#define UART0_STATUS   UCSR0A
#define UART0_CONTROL  UCSR0B
#define UART0_DATA     UDR0
#define UART0_UDRIE    UDRIE0
#define UART1_STATUS   UCSR1A
#define UART1_CONTROL  UCSR1B
#define UART1_DATA     UDR1
#define UART1_UDRIE    UDRIE1
#else
#error "no UART definition for MCU available"
#endif



/*
** constants and macros
*/

/** @brief  UART Baudrate Expression
 *  @param  xtalcpu  system clock in Mhz, e.g. 4000000L for 4Mhz          
 *  @param  baudrate baudrate in bps, e.g. 1200, 2400, 9600     
 */
#define UART_BAUD_SELECT(baudRate,xtalCpu) ((xtalCpu)/((baudRate)*16l)-1)

/** @brief  UART Baudrate Expression for ATmega double speed mode
 *  @param  xtalcpu  system clock in Mhz, e.g. 4000000L for 4Mhz           
 *  @param  baudrate baudrate in bps, e.g. 1200, 2400, 9600     
 */
#define UART_BAUD_SELECT_DOUBLE_SPEED(baudRate,xtalCpu) (((xtalCpu)/((baudRate)*8l)-1)|0x8000)


/** Size of the circular receive buffer, must be power of 2 */
#ifndef UART_RX_BUFFER_SIZE
#define UART_RX_BUFFER_SIZE 32
#endif
/** Size of the circular transmit buffer, must be power of 2 */
#ifndef UART_TX_BUFFER_SIZE
#define UART_TX_BUFFER_SIZE 32
#endif

/* test if the size of the circular buffers fits into SRAM */
#if ( (UART_RX_BUFFER_SIZE+UART_TX_BUFFER_SIZE) >= (RAMEND-0x60UL ) )
#error "size of UART_RX_BUFFER_SIZE + UART_TX_BUFFER_SIZE larger than size of SRAM"
#endif

/* 
** high byte error return code of uart_getc()
*/
#define UART_FRAME_ERROR      0x0800              /* Framing Error by UART       */
#define UART_OVERRUN_ERROR    0x0400              /* Overrun condition by UART   */
#define UART_BUFFER_OVERFLOW  0x0200              /* receive ringbuffer overflow */
#define UART_NO_DATA          0x0100              /* no receive data available   */


/*
** function prototypes
*/

void rs323_init(unsigned int baudrate);
void rs485_init(unsigned int baudrate);

/**
   @brief   Initialize UART and set baudrate 
   @param   baudrate Specify baudrate using macro UART_BAUD_SELECT()
   @return  none
*/
extern void uart_init(unsigned int baudrate);

/**
 *  @brief   Get received byte from ringbuffer
 *
 * Returns in the lower byte the received character and in the 
 * higher byte the last receive error.
 * UART_NO_DATA is returned when no data is available.
 *
 *  @param   void
 *  @return  lower byte:  received byte from ringbuffer
 *  @return  higher byte: last receive status
 *           - \b 0 successfully received data from UART
 *           - \b UART_NO_DATA           
 *             <br>no receive data available
 *           - \b UART_BUFFER_OVERFLOW   
 *             <br>Receive ringbuffer overflow.
 *             We are not reading the receive buffer fast enough, 
 *             one or more received character have been dropped 
 *           - \b UART_OVERRUN_ERROR     
 *             <br>Overrun condition by UART.
 *             A character already present in the UART UDR register was 
 *             not read by the interrupt handler before the next character arrived,
 *             one or more received characters have been dropped.
 *           - \b UART_FRAME_ERROR       
 *             <br>Framing Error by UART
 */
extern unsigned int uart_getc(void);


/**
 *  @brief   Put byte to ringbuffer for transmitting via UART
 *  @param   data byte to be transmitted
 *  @return  none
 */
extern void uart_putc(unsigned char data);


/**
 *  @brief   Put string to ringbuffer for transmitting via UART
 *
 *  The string is buffered by the uart library in a circular buffer
 *  and one character at a time is transmitted to the UART using interrupts.
 *  Blocks if it can not write the whole string into the circular buffer.
 * 
 *  @param   s string to be transmitted
 *  @return  none
 */
extern void uart_puts(const char *s );


/**
 * @brief    Put string from program memory to ringbuffer for transmitting via UART.
 *
 * The string is buffered by the uart library in a circular buffer
 * and one character at a time is transmitted to the UART using interrupts.
 * Blocks if it can not write the whole string into the circular buffer.
 *
 * @param    s program memory string to be transmitted
 * @return   none
 * @see      uart_puts_P
 */
extern void uart_puts_p(const char *s );

/**
 * @brief    Macro to automatically put a string constant into program memory
 */
#define uart_puts_P(__s)       uart_puts_p(PSTR(__s))


/**
 *  @brief   Flush bytes waiting in receive buffer
 *  @param   none
 *  @return  none
 */
extern void uart_flush(void);


/** @brief  Initialize USART1 (only available on selected ATmegas) @see uart_init */
extern void uart1_init(unsigned int baudrate);
/** @brief  Get received byte of USART1 from ringbuffer. (only available on selected ATmega) @see uart_getc */
extern unsigned int uart1_getc(void);
/** @brief  Put byte to ringbuffer for transmitting via USART1 (only available on selected ATmega) @see uart_putc */
extern void uart1_putc(unsigned char data);
/** @brief  Put string to ringbuffer for transmitting via USART1 (only available on selected ATmega) @see uart_puts */
extern void uart1_puts(const char *s );
/** @brief  Put string from program memory to ringbuffer for transmitting via USART1 (only available on selected ATmega) @see uart_puts_p */
extern void uart1_puts_p(const char *s );
/** @brief  Macro to automatically put a string constant into program memory */
#define uart1_puts_P(__s)       uart1_puts_p(PSTR(__s))
/** @brief   Return number of bytes waiting in the receive buffer */
extern int uart1_available(void);
/** @brief   Flush bytes waiting in receive buffer */
extern void uart1_flush(void);

/**@}*/


#endif // UART_H 

