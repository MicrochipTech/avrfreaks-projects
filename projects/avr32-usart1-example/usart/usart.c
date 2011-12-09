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
 * $Name:  $
 * $Revision: 527 $
 * $RCSfile: usart.c,v $
 * $Date: 2007-07-24 14:15:23 +0200 (Tue, 24 Jul 2007) $
 **************************************************************************** */

/* ************************************************************************

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

\* ************************************************************************ */

#include "usart.h"

/*
 * Description: Check if the usart is in multidrop
 * Arguments:   *usart: Base address of the usart
 * Returns:     1 if the usart is in multidrop mode, otherwise 0
 */

static int usart_mode_is_multidrop(volatile struct avr32_usart_t * usart)
{
  return ( (usart->mr & 0x00000600) >> AVR32_USART_MR_PAR_OFFSET );
}

void usart_reset( volatile struct avr32_usart_t * usart)
{
	/* Disable all usart interrupts, interrupts needed should be set
	   explicitly on every reset */
	usart->idr = 0xFFFFFFFF;

	/* Reset mode and other registers that could cause unpredictable
	   behaviour after reset */
	usart->mr = 0;
	usart->rtor = 0;
	usart->ttgr = 0;

	/* Shutdown RX and TX (will be reenabled when setup
	   is completed successfully), reset status bits and turn
	   off DTR and RTS */
	usart->cr = (1 << AVR32_USART_CR_RSTRX_OFFSET) |
	            (1 << AVR32_USART_CR_RSTTX_OFFSET) |
	            (1 << AVR32_USART_CR_RSTSTA_OFFSET) |
	            (1 << AVR32_USART_CR_RSTIT_OFFSET) |
	            (1 << AVR32_USART_CR_RSTNACK_OFFSET) |
	            (1 << AVR32_USART_CR_DTRDIS_OFFSET) |
	            (1 << AVR32_USART_CR_RTSDIS_OFFSET);
}


/*!
 * This function will calculate a clock divider(CD) and fractional
 * part(FP) that gets the usart as close to a wanted baudrate as
 * possible
 * \param *usart   Base address of the usart
 * \param baudrate Wanted baudrate
 * \param cpu_hz  Frequency of the selected clock
 * \return USART_SUCCESS or USART_INVALID_INPUT if wanted baudrate is
 *            impossible with given clockspeed
 */
static int usart_set_baudrate( volatile struct avr32_usart_t * usart, unsigned int baudrate, unsigned long cpu_hz)
{
	int cd, fp; /* Clock divider */
	double tempBaud;

	/*
	 *             ** BAUDRATE CALCULATION **
	 *
	 *                 Selected Clock                       Selected Clock
	 *     baudrate = ----------------   or     baudrate = ----------------
	 *                16 x (CD+(FP/8))                      8 x (CD+(FP/8))
	 *
	 *       (with 16x oversampling)              (with 8x oversampling)
	 */
	if ( baudrate < (cpu_hz/16)  ){
		/* Use 8x oversampling */
		usart->mr |= (1<<AVR32_USART_MR_OVER_OFFSET);
		tempBaud = cpu_hz / (8*baudrate);
		cd = (int)tempBaud;
		fp = (int)((tempBaud-cd)*8);

		if (cd < 2) {
			return USART_INVALID_INPUT;
		}
	} else {
		/* Use 16x oversampling */
		usart->mr &= ~(1<<AVR32_USART_MR_OVER_OFFSET);
		tempBaud = cpu_hz / (16*baudrate);
		cd = (int)tempBaud;
		fp = (int)((tempBaud-cd)*8);

		if (cd > 65535) {
		  /* Baudrate is too low */
		  return USART_INVALID_INPUT;
		}
	}
	usart->brgr = (cd << AVR32_USART_BRGR_CD_OFFSET)|(fp << AVR32_USART_BRGR_FP_OFFSET);
	return USART_SUCCESS;
}




 /*---------------------------------------------------------------------------+
 |                                                                            |
 |                           INITIALIZATION FUNCTIONS                         |
 |                                                                            |
 +---------------------------------------------------------------------------*/


int usart_init_rs232(volatile struct avr32_usart_t * usart, usart_options_t * opt, unsigned long cpu_hz)
{
	int retval;

	/* Reset the usart and shutdown RX and TX */
	usart_reset(usart);

	/* Control input values */
	if (opt == 0) /* Null pointer */
		return USART_INVALID_INPUT;
	if (opt->charlength < 5 || opt->charlength > 9)
		return USART_INVALID_INPUT;
	if (opt->paritytype > 7)
		return USART_INVALID_INPUT;
	if (opt->stopbits > 2+255)
		return USART_INVALID_INPUT;
	if (opt->channelmode > 3)
		return USART_INVALID_INPUT;

	if ((retval = usart_set_baudrate(usart, opt->baudrate, cpu_hz)) != \
	     USART_SUCCESS)
		return retval;

	if (opt->charlength == 9) {
		/* Charlength set to 9 bits; MODE9 dominates CHRL */
		usart->mr |= (1<<AVR32_USART_MR_MODE9_OFFSET);
	} else {
		/* CHRL gives the charlength( - 5) when USART_MODE9=0 */
		usart->mr |=
			((opt->charlength-5) << AVR32_USART_MR_CHRL_OFFSET);
	}

	usart->mr |= (opt->channelmode << AVR32_USART_MR_CHMODE_OFFSET) |
	             (opt->paritytype << AVR32_USART_MR_PAR_OFFSET);

	if (opt->stopbits > 2)
	{
		/* Set two stop bits */
		usart->mr |= (2 << AVR32_USART_MR_NBSTOP_OFFSET);
		/* And a timeguard period gives the rest */
		usart->ttgr = (opt->stopbits-2);
	}
	else
		/* Insert 1, 1.5 or 2 stop bits */
		usart->mr |= (opt->stopbits << AVR32_USART_MR_NBSTOP_OFFSET);


		
	/* Setup complete; enable communication */
	/* Enable input and output */
	usart->cr |= (1<<AVR32_USART_CR_TXEN_OFFSET) |
	             (1<<AVR32_USART_CR_RXEN_OFFSET);

	return USART_SUCCESS;
}

/*
 * Description: This function is meant to be run after rs232_init().
 *              It sets up the usart to use handshaking in its communication.
 * Arguments:   *usart:    Base address of the usart
 *              *opt:      Options needed to set up RS232 communcation (see usart_options_t)
 *				cpu_hz:    The clock frequency of the usart module
 *              software_handshaking:
 *                         1= Use software handshaking
 *                         0= Use hardware handshaking (requires extra wiring)
 *              xon_char:  Character sent from receiver to transmitter when more
 *                         data can be sent. (Software handshaking only)
 *              xoff_char: Sent from recv. to trans. when recv. buffers are full (sw)
 * Returns:     USART_SUCCESS or USART_INVALID_INPUT
 */
int usart_init_handshaking(volatile struct avr32_usart_t * usart, usart_options_t * opt,
                           long cpu_hz, int software_handshaking,
                           char xon_char, char xoff_char)
{
	int retval;

	/* First: Setup standard RS323 */
	if ((retval = usart_init_rs232(usart, opt, cpu_hz)) != USART_SUCCESS)
		return retval;

	if (software_handshaking == 0)
	{
		/* Clear previous mode */
		usart-> mr &= ~(0xf << AVR32_USART_MR_MODE_OFFSET);
		/* Hardware handshaking */
		usart-> mr |= (USART_MODE_HW_HSH << AVR32_USART_MR_MODE_OFFSET);
	}
	else if (software_handshaking == 1)
	{
		/* Clear previous mode */
		usart-> mr &= ~(0xf << AVR32_USART_MR_MODE_OFFSET);
		/* Software handshaking */
		usart-> mr |= (USART_MODE_SW_HSH << AVR32_USART_MR_MODE_OFFSET);
	}
	else
		return USART_INVALID_INPUT;

	return USART_SUCCESS;
}

/*
 * Description: Setup the usart to use the IrDA protocol
 * Arguments:   *usart:      Base address of the usart
 *              *opt:        Options needed to set up RS232 communcation (see usart_options_t)
 *				cpu_hz:		 The module's clock frequency
 *              irda_filter: Counter used to seperate received ones from zeros
 * Returns:     USART_SUCCESS or USART_INVALID_INPUT
 */
int usart_init_IrDA(volatile struct avr32_usart_t * usart, usart_options_t * opt,
                    long cpu_hz, unsigned char irda_filter)
{
	int retval;

	/* First: Setup standard RS323 */
	if ((retval = usart_init_rs232(usart, opt, cpu_hz)) != USART_SUCCESS)
		return retval;

	/* Set IrDA counter */
	usart->ifr = irda_filter;

	/* Activate "low-pass filtering" of input */
	usart->mr |= (1 << AVR32_USART_MR_FILTER_OFFSET);
	return USART_SUCCESS;
}

/*
 * Description: Setup the usart to use the Modem protocol, activating special inputs/outputs
 * Arguments:   *usart: Base address of the usart
 *              *opt:   Options needed to set up RS232 communcation (see usart_options_t)
*				cpu_hz:		 The module's clock frequency
 * Returns:     USART_SUCCESS or USART_INVALID_INPUT
 */
int usart_init_modem(volatile struct avr32_usart_t * usart, usart_options_t * opt, long cpu_hz)
{
	int retval;

	/* First: Setup standard RS323 */
	if ((retval = usart_init_rs232(usart, opt, cpu_hz)) != USART_SUCCESS)
		return retval;

	/* Clear previous mode */
	usart-> mr &= ~(0xf << AVR32_USART_MR_MODE_OFFSET);
	/* Set Modem mode */
	usart-> mr |= (USART_MODE_MODEM << AVR32_USART_MR_MODE_OFFSET);
	return USART_SUCCESS;
}


/*
 * Description: Setup the usart to use the RS485 protocol
 * Arguments:   *usart: Base address of the usart
 *              *opt:   Options needed to set up RS232 communcation (see usart_options_t)
 *				cpu_hz: The module's clock frequency
 * Returns:     USART_SUCCESS: mode successfully initialized
 *              USART_INVALID_INPUT: one of the arguments are out of valid range
 *              USART_MODE_FAULT: mode not initialized with multidrop parity
 */
int usart_init_rs485(volatile struct avr32_usart_t * usart, usart_options_t * opt, long cpu_hz)
{
	int retval;

	/* First: Setup standard RS323 */
	if ((retval = usart_init_rs232(usart, opt, cpu_hz)) != USART_SUCCESS)
		return retval;

	/* Clear previous mode */
	usart->mr &= ~(0xf << AVR32_USART_MR_MODE_OFFSET);
	/* Set Modem mode */
	usart->mr |= (USART_MODE_RS485 << AVR32_USART_MR_MODE_OFFSET);
	return USART_SUCCESS;
}


/*
 * Description: Setup the usart to use ISO7816 T=0 or T=1 smartcard protocol
 * Arguments:   *usart: Base address of the usart
 *              *opt:   Options needed to set up ISO7816 (see iso7816_options_t)
 *              t:      Which ISO7816 mode to use (T=0 or T=1)
 * Returns:     USART_SUCCESS or USART_INVALID_INPUT
 */
int usart_init_iso7816(volatile struct avr32_usart_t * usart, const struct iso7816_options_t * opt, int t, const long cpu_hz)
{
	int retval;

	/* Reset the usart and shutdown RX and TX */
	usart_reset(usart);

	if (opt == 0)
		/* Null pointer */
		return USART_INVALID_INPUT;

	/* Don't care about charlength, parity or channelmode; All these fields
	   are ignored in iso7816 mode. 8bit characters and even parity is always
	   used */

	if (t == 0)
	{
		/* Set USART mode to ISO7816, T=0 */
		/* The T=0 protocol always use 2 stop bits */
		usart->mr = (USART_MODE_ISO7816_T0 << AVR32_USART_MR_MODE_OFFSET) |
		            (2 << AVR32_USART_MR_NBSTOP_OFFSET) |
		            (opt->bit_order << AVR32_USART_MR_MSBF_OFFSET); /* Allow MSBF in T=0 */
	}
	else if (t == 1)
	{
		/* Only LSB first in the T=1 protocol */
		if (opt->bit_order != 0)
			return USART_INVALID_INPUT;
		/* max_iterations field is only used in T=0 mode */
		if (opt->max_iterations != 0)
			return USART_INVALID_INPUT;
		/* Set USART mode to ISO7816, T=1 */
		usart->mr = (USART_MODE_ISO7816_T1 << AVR32_USART_MR_MODE_OFFSET);
		/* The T=1 protocol always use 1 stop bit (no change needed) */
	}
	else
		return USART_INVALID_INPUT;

	if ((retval = usart_set_baudrate(usart, opt->iso7816_hz, cpu_hz)) != USART_SUCCESS)
		return retval;

	/* Set FIDI register: bit rate = selected clock/FI_DI_ratio/16 */
	usart->fidi = opt->fidi_ratio;
	/* Set ISO7816 spesific options in the MODE register */
	usart->mr |= (opt->inhibit_nack << AVR32_USART_MR_INACK_OFFSET) |
	             (opt->dis_suc_nack << AVR32_USART_MR_DSNACK_OFFSET) |
	             (opt->max_iterations << AVR32_USART_MR_MAX_ITERATION_OFFSET) |
		     (1 << AVR32_USART_MR_CLKO_OFFSET); /* Enable clock output */

	/* Setup complete; enable input */
	/* Leave TX disabled for now */
	usart->cr |= (1<<AVR32_USART_CR_RXEN_OFFSET);

	return USART_SUCCESS;
}




 /*---------------------------------------------------------------------------+
 |                                                                            |
 |                     READ AND RESET ERROR STATUS BITS                       |
 |                                                                            |
 +---------------------------------------------------------------------------*/

/*
 * Description: This function resets the status bits indicating that a parity error,
 *              framing error or overrun has occured. The rxbreak bit, indicating
 *              a start/end of break condition on the rx-line, is also reset.
 * Arguments:   *usart:  Base address of the usart
 * Returns:     nothing
 */
void usart_reset_status(volatile struct avr32_usart_t * usart)
{
	usart->cr |= (1<<AVR32_USART_CR_RSTSTA_OFFSET);
}

/*
 * Description: Checks if a parity error has occured since last status reset
 * Arguments:   *usart:  Base address of the usart
 * Returns:     1 if a parity error has been detected, otherwise 0
 */
int usart_parity_error(volatile struct avr32_usart_t * usart)
{
	return ((usart->csr & (1<<AVR32_USART_CSR_PARE_OFFSET)) != 0);
}


/*
 * Description: Checks if a framing error has occured since last status reset
 * Arguments:   *usart:  Base address of the usart
 * Returns:     1 if a framing error has been detected, otherwise 0
 */
int usart_framing_error(volatile struct avr32_usart_t * usart)
{
	return ((usart->csr & (1<<AVR32_USART_CSR_FRAME_OFFSET)) != 0);
}


/*
 * Description: Checks if a overrun error has occured since last status reset
 * Arguments:   *usart:  Base address of the usart
 * Returns:     1 if a overrun error has been detected, otherwise 0
 */
int usart_overrun_error(volatile struct avr32_usart_t * usart)
{
	return ((usart->csr & AVR32_USART_CSR_OVRE_OFFSET)) != 0;
}



 /*---------------------------------------------------------------------------+
 |                                                                            |
 |                         TRANSMIT/RECEIVE FUNCTIONS                         |
 |                                                                            |
 +---------------------------------------------------------------------------*/

/*
 * Description: While in RS485-mode, receviers only accept data addressed to them.
 *              A packet/char with the address tag set has to preceed any data.
 *              usart_send_address() is used to address a receiver. This receiver should read
 *              all the following data, until an address packet addresses someone else.
 * Arguments:   *usart:  Base address of the usart
 *              addr: the address of the target device
 * Returns:     USART_SUCCESS if the current mode is RS485
 *              USART_MODE_FAULT if called while in wrong mode
 */

int usart_send_address(volatile struct avr32_usart_t * usart, int address)
{
	/* Check if usart is in multidrop / RS485 mode */
	if ( usart_mode_is_multidrop(usart) )
	{
		/* Prepare to send an address */
		usart->cr |= (1<<AVR32_USART_CR_SENDA_OFFSET);

		/* Write the address to TX */
		usart_bw_write_char(usart, address);
		return USART_SUCCESS;
	} else {
		return USART_MODE_FAULT;
	}
}

/*
 * Description: Wait until the transmitter it ready (potentially forever),
 *              then transmit the given character
 * Arguments:   *usart:  Base address of the usart
 *              c:       The character (up to 9 bits) to transmit
 * Returns:     nothing
 */
inline void usart_bw_write_char(volatile struct avr32_usart_t * usart, int c)
{
	while (usart_write_char(usart, c) != USART_SUCCESS) {
	}

	return;
}

int usart_write_char(volatile struct avr32_usart_t * usart, int c)
{

	if ((usart->csr & (1<<AVR32_USART_CSR_TXRDY_OFFSET)) != 0) {
		usart->thr = c;
		return USART_SUCCESS;
	}
	else
		return USART_TX_BUSY;
}


int usart_read_char(volatile struct avr32_usart_t * usart, int * c)
{
	/* Check for errors; Frame, parity and overrun In RS485 mode a parity
		error would mean that we received an address char */
	if (usart->csr &
			((1 << AVR32_USART_CSR_OVRE_OFFSET) |
			(1 << AVR32_USART_CSR_FRAME_OFFSET) |
			(1 << AVR32_USART_CSR_PARE_OFFSET))) {
		return USART_RX_ERROR;
	}
	/* No error; if we really did receive a char, read it and return SUCCESS */
	else if ((usart->csr & (1<<AVR32_USART_CSR_RXRDY_OFFSET)) != 0) {
		*c = (unsigned short)usart->rhr;
		return USART_SUCCESS;
	} else {
		return USART_RX_EMPTY;
	}
} /* usart_read */


int usart_getchar(volatile struct avr32_usart_t * usart)
{
	int c, ret;

  	while (((ret = usart_read_char(usart, &c)) == USART_RX_EMPTY)) {
	}

	if (ret == USART_RX_ERROR)
		return -1;
	else
		return c;
}


int usart_putchar(volatile struct avr32_usart_t * usart, int c)
{
	int timeout = USART_DEFAULT_TIMEOUT;

	if (c == '\n'){
	  while ((usart_write_char(usart, '\r') != USART_SUCCESS) && (timeout>0) ){
	    timeout--;
	  }
	
	  if (timeout == 0)
	    return -1;
	  timeout = USART_DEFAULT_TIMEOUT;
	}

	while ((usart_write_char(usart, c) != USART_SUCCESS) && ( timeout>0 )){
	  timeout--;
	}
	if (timeout == 0)
		return -1;
	else
		return 0;
}

