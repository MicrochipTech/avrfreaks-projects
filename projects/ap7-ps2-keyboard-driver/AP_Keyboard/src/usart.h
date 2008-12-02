/*This file has been prepared for Doxygen automatic documentation generation.*/
/*! \file *********************************************************************
 *
 * \brief USART driver library.
 *
 * This file contains basic drivers for the AVR32 USART, with support for all
 * all modes, settings and clock speeds.
 *
 * - Compiler:           IAR EWAAVR32 and GNU GCC for AVR32
 * - Supported devices:  All AVR32 devices with a USART module can be used.
 * - AppNote:            AVR32100 - Communication with the AVR32 USART
 *
 * \author               Atmel Corporation: http://www.atmel.com \n
 *                       Support email: avr32@atmel.com
 *
 * $Name$
 * $Revision: 67 $
 * $RCSfile$
 * $Date: 2006-05-10 15:00:07 +0200 (Wed, 10 May 2006) $
 **************************************************************************** */

/* *****************************************************************************
Copyright (c) 2006, Atmel Corporation All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice,
this list of conditions and the
following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice,
this list of conditions and the following disclaimer in the documentation
and/or other materials provided with the distribution.

3. The name of ATMEL may not be used to endorse or promote products
derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY ATMEL ``AS IS'' AND ANY EXPRESS
OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
PARTICULAR PURPOSE ARE EXPRESSLY AND SPECIFICALLY
DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR ANY DIRECT,
INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY
WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE

POSSIBILITY OF SUCH DAMAGE.

************************************************************************ */


#ifndef _USART_H_
#define _USART_H_

#ifdef __GNUC__
	#include <avr32/io.h>
#elif __ICCAVR32__
	#include <avr32/ioap7000.h>
#else
	#error No known compiler used
#endif


/*!
 * Input parameters when initializing rs232 mode, and some similar modes.
 */
struct usart_options_t
{
	/*!
	 * Set baudrate of the usart
	 */
	unsigned long baudrate;

	/*!
	 * Number of bits to transmit as a character (5-9)
	 */
	unsigned char charlength;

	/*!
	 * How to calculate the parity bit:
	 * 0=even parity, 1=odd parity, 2=space, 3=mark,
	 * 4|5=no parity bit, 6|7=multidrop mode
	 */
	unsigned char paritytype;

	/*!
	 * Number of stop bits between to characters:
	 * 0=1bit, 1=1.5bit(async mode only), 2=2bits,
	 * the numbers 3 to 257 will result in a time guard period of that
	 * length between characters.
	 */
	unsigned short stopbits;

	/*!
	 * Run the channel in testmode:
	 * 0=normal tx/rx, 1=auto echo(tx->rx), 2=local loopback(tx->rx),
	 * 3=remote loopback(rx->tx)
	 */
	unsigned char channelmode;
};


struct iso7816_options_t
{
	/*
	 * Set the frequency of the ISO7816 clock
	 */
	unsigned long iso7816_hz;

	/*
	 * The number of ISO7816 clock tick in every bit period (1-2047, 0=disable clock)
	 * Bit rate = iso7816_hz / fidi_ratio
	 */
	unsigned short fidi_ratio;

	/*
	 * Inhibit Non Acknowledge
	 * - 0 - The NACK is generated
	 * - 1 - The NACK is not generated
	 * .
	 * Note: This bit will be used only in ISO7816 mode, protocol T = 0 receiver
	 */
	int inhibit_nack;

	/*
	 * Disable Successive NACK \n\n
	 * Successive parity errors are counted up to the value in the max_iterations field. \n
	 * These parity errors generate a NACK on the ISO line. As soon as this value is reached, \n
	 * no addititional NACK is sent on the ISO line. The flag ITERATION is asserted. \n
	 */
	int dis_suc_nack;

	/*
	 * Max number of repetitions (0 - 7)
	 */
	unsigned char max_iterations;

	/*
	 * Bitorder in transmitted characters
	 * - 0 - LSB first
	 * - 1 - MSB first
	 */
	int bit_order;
};

 /*---------------------------------------------------------------------------+
 |                                                                                                |
 |                           INITIALIZATION FUNCTIONS                |
 |                                                                                                |
 +---------------------------------------------------------------------------*/

/*!
 * This function will reset the USART, and disable TX and RX
 * \param *usart Base address of the usart
 * \return nothing
 */
void usart_reset( volatile struct avr32_usart_t * usart );


/*!
 * Setup the usart to use the standard RS232 protocol
 * \param *usart Base address of the usart
 * \param *opt   Options needed to set up RS232 communcation
 *                  (see \ref usart_options_t)
 * \param cpu_hz The usart clk frequency
 * \return USART_SUCCESS or USART_INVALID_INPUT
 */
int usart_init_rs232( volatile struct avr32_usart_t * usart, struct usart_options_t * opt, long cpu_hz );


 /*---------------------------------------------------------------------------+
 |                                                                                                |
 |                         TRANSMIT/RECEIVE FUNCTIONS         |
 |                                                                                                |
 +---------------------------------------------------------------------------*/

/**
 * Description: While in RS485-mode, receviers only accept data addressed to them.
 *              A packet/char with the address tag set has to preceed any data.
 *              usart_send_addr() is used to address a receiver. This receiver should read
 *              all the following data, until an address packet addresses someone else.
 * Arguments:   *usart:  Base address of the usart
 *              addr: the address of the target device
 * Returns:     USART_SUCCESS if the current mode is RS485
 *              USART_MODE_FAULT if called while in wrong mode
 */
int usart_send_addr(volatile struct avr32_usart_t * usart, int addr);


/*!
 * If the transmitter is ready; write the given character to the TX buffer
 * \param *usart  Base address of the usart
 * \param c       The character (up to 9 bits) to transmit
 * \return        USART_SUCCESS when the transmitter is ready
 *                USART_TX_BUSY when the transmitter is busy
 */
int usart_write_char(volatile struct avr32_usart_t * usart, int c);


/*!
 * A busy wait for writing a character to the usart. Use with *caution*
 * \param *usart  Base address of the usart
 * \param c       The character (up to 9 bits) to transmit
 * \return        None
 */
inline void usart_bw_write_char(volatile struct avr32_usart_t * usart, int c);

/*!
 * Send a character with the usart
 * \param *usart Base address of the usart
 * \param c      Character to write
 * \return 0 if char was written, -1 if the function timed out before the
 *         usart transmitter became ready to send
 */
int usart_putchar(volatile struct avr32_usart_t * usart, int c);

/*!
 * Checks the RX buffer for a received character, and puts this at
 * the memory location given.
 * \param *usart  Base address of the usart
 * \param *c      Pointer to the where the read charcter should be writen
 *                (must be short in order to accept 9 bit characters)
 * \return USART_SUCCESS if the character was read successfully\n
 *         USART_RX_EMPTY if the RX buffer was empty\n
 *         USART_RX_ERROR if an error was deteceted\n
 *         USART_ADDR_RECEIVED if an address is received while in multidrop mode
 */
int usart_read_char(volatile struct avr32_usart_t * usart, int * c);

/*!
 * Wait until a character is recevied, and return this.
 * \param *usart Base address of the usart
 * \return The received character, or -1 upon error
 */
int usart_getchar(volatile struct avr32_usart_t * usart);

/*! \brief Reset error status
 *
 * This function resets the status bits indicating that a parity error,
 * framing error or overrun has occured. The rxbreak bit, indicating
 * a start/end of break condition on the rx-line, is also reset.
 * \param *usart  Base address of the usart
 * \return        nothing
 */
void usart_reset_status(volatile struct avr32_usart_t * usart);

/*! Value returned by function when it completed successfully */
#define USART_SUCCESS 0
/*! Value returned by function when it was unable to complete successfully
    for some unspecified reason */
#define USART_FAILURE -1
/*! Value returned by function when the input paramters are out of range */
#define USART_INVALID_INPUT 1
#define USART_INVALID_ARGUMENT -1
/*! Value returned by transmission function when transmitter was busy */
#define USART_TX_BUSY  2
/*! Value returned by receiver function when nothing was received */
#define USART_RX_EMPTY 3
/*! Value returned by receiver function when transmission error occured */
#define USART_RX_ERROR 4
/*! Value returned by a function when the usart is not in the appropriate
    mode */
#define USART_MODE_FAULT 5
/*! Value returned by receiver function when an address character was
    received */
#define USART_ADDR_RECEIVED 1

/*! Default timeout value; number of tries before timing out */
#define USART_DEFAULT_TIMEOUT  10000

/*! Use even parity on character transmission */
#define USART_EVEN_PARITY	0
/*! Use odd parity on character transmission */
#define USART_ODD_PARITY	1
/*! Use a space as parity bit */
#define USART_SPACE_PARITY	2
/*! Use a mark as parity bit */
#define USART_MARK_PARITY	3
/*! Don't use a parity bit */
#define USART_NO_PARITY		4
/*! Parity bit is used to flag address characters */
#define USART_MULTIDROP_PARITY	6

/*! Operate usart in normal rs232 mode */
#define USART_MODE_NORMAL	0x00
/*! Operate usart in rs485 mode */
#define USART_MODE_RS485	0x01
/*! Operate usart in rs232 with hardware handshaking */
#define USART_MODE_HW_HSH	0x02
/*! Operate usart in modem mode */
#define USART_MODE_MODEM	0x03
/*! Operate usart in iso7816, T=0 mode */
#define USART_MODE_ISO7816_T0	0x04
/*! Operate usart in iso7816, T=1 mode */
#define USART_MODE_ISO7816_T1	0x06
/*! Oparte usart in irda mode */
#define USART_MODE_IRDA		0x08
/*! Operate usart in rs232 with software handshaking */
#define USART_MODE_SW_HSH	0x0C

/*! Set usart channel to normal communcation */
#define USART_NORMAL_CHMODE	0
/*! Set usart channel to echo data */
#define USART_AUTO_ECHO		1
/*! Set usart channel to local loopback */
#define USART_LOCAL_LOOPBACK	2
/*! Set usart channel to remote loopback */
#define USART_REMOTE_LOOPBACK	3

/*! Use 1 stop bit */
#define USART_1_STOPBIT		0
/*! Use 1.5 stop bits */
#define USART_1_5_STOPBITS 	1
/*! Use 2 stop bits (for more, just give the number of bits) */
#define USART_2_STOPBITS	2

/*
	Interrupt sources. Use with interrupt registers in USART
*/

#define INT_RXRDY      0
#define INT_TXRDY      1
#define INT_RXBRK      2
#define INT_ENDRX      3
#define INT_ENDTX      4
#define INT_OVRE       5
#define INT_FRAM       6
#define INT_PARE       7
#define INT_TIMEOUT    8
#define INT_TXEMPTY    9
#define INT_ITERATION 10
#define INT_TXBUFE    11
#define INT_RXBUFF    12
#define INT_NACK      13
#define INT_RIIC      16
#define INT_DSRIC     17
#define INT_DCDIC     18
#define INT_CTSIC     19
#define INT_MANE      20


void print(volatile struct avr32_usart_t * usart, char *str);

void print_ulong(volatile avr32_usart_t *usart, unsigned long n);

void print_long(volatile avr32_usart_t *usart, long n);

void print_char_hex(volatile avr32_usart_t *usart, unsigned char n);

void print_short_hex(volatile avr32_usart_t *usart, unsigned short n);

void print_hex(volatile avr32_usart_t *usart, unsigned long n);



#endif //#ifndef _AVR32_USART_H_
