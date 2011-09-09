/**
 * \file
 *
 * \brief USART driver for AVR XMEGA.
 *
 * This file contains basic functions for the AVR XMEGA USART, with support for all
 * modes, settings and clock speeds.
 *
 * Copyright (C) 2009 - 2011 Atmel Corporation. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * 3. The name of Atmel may not be used to endorse or promote products derived
 * from this software without specific prior written permission.
 *
 * 4. This software may only be redistributed and used in connection with an
 * Atmel AVR product.
 *
 * THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
 * DAMAGE.
 */
#ifndef _USART_H_
#define _USART_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "compiler.h"
#include "pmic.h"

/**
 * \defgroup usart_group USART module (USART)
 *
 * This is a driver for configuring, enablig, disabling and use of the on-chip
 * USART.
 *
 * \section dependencies Dependencies
 *
 * The USART module depends on the following modules:
 *  - \ref sysclk_group for peripheral clock control.
 *  - \ref port_driver_group for peripheral io port control.
 *
 * @{
 */

//! Input parameters when initializing RS232 and similar modes.
typedef struct usart_rs232_options {
	//! Set baud rate of the USART (unused in slave modes).
	uint32_t baudrate;

	//! Number of bits to transmit as a character (5 to 9).
	USART_CHSIZE_t charlength;

	//! Parity type: USART_PMODE_DISABLED_gc, USART_PMODE_EVEN_gc,
	//! USART_PMODE_ODD_gc.
	USART_PMODE_t paritytype;

	//! Number of stop bits between two characters:
	//! true: 2 stop bits
	//! false: 1 stop bit
	bool stopbits;

} usart_rs232_options_t;

//! Input parameters when initializing SPI master mode.
typedef struct usart_spi_options {
	//! Set baud rate of the USART in SPI mode.
	uint32_t baudrate;

	//! SPI transmission mode.
	uint8_t spimode;

	uint8_t data_order;
} usart_spi_options_t;

/**
 * \brief Enable USART receiver.
 *
 * \param usart Pointer to the USART module
 */
static inline void usart_rx_enable(USART_t *usart)
{
	(usart)->CTRLB |= USART_RXEN_bm;
}

/**
 * \brief Disable USART receiver.
 *
 * \param usart Pointer to the USART module.
 */
static inline void usart_rx_disable(USART_t *usart)
{
	(usart)->CTRLB &= ~USART_RXEN_bm;
}

/**
 * \brief  Configure the USART frame format.
 *
 *  Sets the frame format, Frame Size, parity mode and number of stop bits.
 *
 *  \param usart Pointer to the USART module
 *  \param charSize The character size. Use USART_CHSIZE_t type.
 *  \param parityMode The parity Mode. Use USART_PMODE_t type.
 *  \param twoStopBits Enable two stop bit mode. Use bool type.
 */
static inline void usart_format_set(USART_t *usart, USART_CHSIZE_t charSize,
		USART_PMODE_t parityMode, bool twoStopBits)
{
	(usart)->CTRLC = (uint8_t)charSize | parityMode
		| (twoStopBits ? USART_SBMODE_bm : 0);
}

/**
 * \brief Enable USART transmitter.
 *
 * \param usart Pointer to the USART module.
 */
static inline void usart_tx_enable(USART_t *usart)
{
	(usart)->CTRLB |= USART_TXEN_bm;
}

/**
 * \brief Disable USART transmitter.
 *
 * \param usart Pointer to the USART module.
 */
static inline void usart_tx_disable(USART_t *usart)
{
	(usart)->CTRLB &= ~USART_TXEN_bm;
}

/**
 * \brief Set USART RXD interrupt level.
 *
 * Sets the interrupt level on RX Complete interrupt.
 *
 * \param usart Pointer to the USART module.
 * \param level Interrupt level of the RXD interrupt.
 */
static inline void usart_set_rx_interrupt_level(USART_t *usart,
		enum pmic_level level)
{
	(usart)->CTRLA = ((usart)->CTRLA & ~USART_RXCINTLVL_gm) | level;
}

/**
 * \brief Set USART TXD interrupt level.
 *
 * Sets the interrupt level on TX Complete interrupt.
 *
 * \param usart Pointer to the USART module.
 * \param level Interrupt level of the TXD interrupt.
 */
static inline void usart_set_tx_interrupt_level(USART_t *usart,
		enum pmic_level level)
{
	(usart)->CTRLA = ((usart)->CTRLA & ~USART_TXCINTLVL_gm) | level;
}

/**
 * \brief Set USART DRE interrupt level.
 *
 * Sets the interrupt level on Data Register interrupt.
 *
 * \param usart Pointer to the USART module.
 * \param level Interrupt level of the DRE interrupt.
 *              Use USART_DREINTLVL_t type.
 */
static inline void usart_set_dre_interrupt_level(USART_t *usart,
		enum pmic_level level)
{
	(usart)->CTRLA = ((usart)->CTRLA & ~USART_DREINTLVL_gm) | level;
}

/**
 * \brief Set the mode the USART run in.
 *
 * Set the mode the USART run in. The default mode is asynchronous mode.
 *
 * \param usart Pointer to the USART module register section.
 * \param usartmode Selects the USART mode. Use USART_CMODE_t type.
 *
 * USART modes:
 * - 0x0        : Asynchronous mode.
 * - 0x1        : Synchronous mode.
 * - 0x2        : IrDA mode.
 * - 0x3        : Master SPI mode.
 */
static inline void usart_set_mode(USART_t *usart, USART_CMODE_t usartmode)
{
	(usart)->CTRLC = ((usart)->CTRLC & (~USART_CMODE_gm)) | usartmode;
}

/**
 * \brief Check if data register empty flag is set.
 *
 * \param usart The USART module.
 */
static inline bool usart_data_register_is_empty(USART_t * usart)
{
	return (usart)->STATUS & USART_DREIF_bm;
}

/**
 * \brief Checks if the RX complete interrupt flag is set.
 *
 * Checks if the RX complete interrupt flag is set.
 *
 * \param usart The USART module.
 */
static inline bool usart_rx_is_complete(USART_t * usart)
{
	return (usart)->STATUS & USART_RXCIF_bm;
}

/**
 * \brief Checks if the TX complete interrupt flag is set.
 *
 * Checks if the TX complete interrupt flag is set.
 *
 * \param usart The USART module.
 */
static inline bool usart_tx_is_complete(USART_t * usart)
{
	return (usart)->STATUS & USART_TXCIF_bm;
}

/**
 * \brief Clear TX complete interrupt flag.
 *
 * \param usart The USART module.
 */
static inline void usart_clear_tx_complete(USART_t * usart)
{
	(usart)->STATUS = USART_TXCIF_bm;
}

/**
 * \brief Clear RX complete interrupt flag.
 *
 * \param usart The USART module.
 */
static inline void usart_clear_rx_complete(USART_t *usart)
{
	(usart)->STATUS = USART_RXCIF_bm;
}

/**
 * \brief Write a data to the USART data register.
 *
 * \param usart The USART module.
 * \param txdata The data to be transmited.
 */
static inline void usart_put(USART_t * usart, uint8_t txdata)
{
	(usart)->DATA = txdata;
}

/**
 * \brief  Read a data to the USART data register.
 *
 * \param usart The USART module.
 *
 * \return The received data
 */
static inline uint8_t usart_get(USART_t * usart)
{
	return (usart)->DATA;
}

/**
 * \brief Performs a data transfer on the USART in SPI mode.
 *
 * \param usart The USART module.
 * \param txdata The data to be transmited.
 *
 * \return The received data
 */
static inline uint8_t usart_spi_transmit(USART_t * usart,
		uint8_t txdata)
{
	while (usart_data_register_is_empty(usart) == false);
	usart_put(usart, txdata);
	while (!usart_tx_is_complete(usart));
	usart_clear_tx_complete(usart);
	return usart_get(usart);
}

void usart_init_rs232(USART_t *usart, const usart_rs232_options_t *opt);
void usart_init_spi(USART_t * usart, const usart_spi_options_t * opt);

enum status_code usart_putchar(USART_t * usart, uint8_t c);
uint8_t usart_getchar(USART_t * usart);

bool usart_set_baudrate(USART_t * usart, uint32_t baud,
		uint32_t cpu_hz);
void usart_spi_set_baudrate(USART_t * usart, uint32_t baud,
		uint32_t cpu_hz);

//! @}

#ifdef __cplusplus
}
#endif

#endif // _USART_H_
