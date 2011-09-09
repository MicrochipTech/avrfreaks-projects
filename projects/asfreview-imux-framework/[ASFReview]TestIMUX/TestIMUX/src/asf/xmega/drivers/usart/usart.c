/**
 * \file
 *
 * \brief USART driver for AVR XMEGA.
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
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#include <stdint.h>

#include "compiler.h"
#include "usart.h"
#include "sysclk.h"
#include "ioport.h"
#include "status_codes.h"

/**
 * \brief Enable the system clock to an USART module.
 *
 * This function will enable the system clock to the provided \arg usart
 * module.
 *
 * \brief usart Pointer to an USART module.
 */
static inline void usart_enable_module_clock(USART_t *usart)
{
#ifdef USARTC0
	if ((uintptr_t)usart == (uintptr_t)&USARTC0) {
		sysclk_enable_module(SYSCLK_PORT_C, SYSCLK_USART0);
	}
#endif
#ifdef USARTC1
	if ((uintptr_t)usart == (uintptr_t)&USARTC1) {
		sysclk_enable_module(SYSCLK_PORT_C, SYSCLK_USART1);
	}
#endif
#ifdef USARTD0
	if ((uintptr_t)usart == (uintptr_t)&USARTD0) {
		sysclk_enable_module(SYSCLK_PORT_D, SYSCLK_USART0);
	}
#endif
#ifdef USARTD1
	if ((uintptr_t)usart == (uintptr_t)&USARTD1) {
		sysclk_enable_module(SYSCLK_PORT_D, SYSCLK_USART1);
	}
#endif
#ifdef USARTE0
	if ((uintptr_t)usart == (uintptr_t)&USARTE0) {
		sysclk_enable_module(SYSCLK_PORT_E, SYSCLK_USART0);
	}
#endif
#ifdef USARTE1
	if ((uintptr_t)usart == (uintptr_t)&USARTE1) {
		sysclk_enable_module(SYSCLK_PORT_E, SYSCLK_USART1);
	}
#endif
#ifdef USARTF0
	if ((uintptr_t)usart == (uintptr_t)&USARTF0) {
		sysclk_enable_module(SYSCLK_PORT_F, SYSCLK_USART0);
	}
#endif
#ifdef USARTF1
	if ((uintptr_t)usart == (uintptr_t)&USARTF1) {
		sysclk_enable_module(SYSCLK_PORT_F, SYSCLK_USART1);
	}
#endif
}

/**
 * \brief Initialize USART in RS232 mode.
 *
 * This function initializes the USART module in RS232 mode using the
 * usart_rs232_options_t configuration structure and CPU frequency.
 *
 * \param usart The USART module.
 * \param opt The RS232 configuration option.
 */
void usart_init_rs232(USART_t *usart, const usart_rs232_options_t *opt)
{
	usart_enable_module_clock(usart);
	usart_set_mode(usart, USART_CMODE_ASYNCHRONOUS_gc);
	usart_format_set(usart, opt->charlength, opt->paritytype,
			opt->stopbits);
	usart_set_baudrate(usart, opt->baudrate, sysclk_get_per_hz());
	usart_tx_enable(usart);
	usart_rx_enable(usart);
}

/**
 * \brief Initialize USART in SPI master mode.
 *
 * This function initializes the USART module in SPI master mode using the
 * usart_spi_options_t configuration structure and CPU frequency.
 *
 * \param usart The USART module.
 * \param opt The RS232 configuration option.
 */
void usart_init_spi(USART_t *usart, const usart_spi_options_t *opt)
{
	usart_enable_module_clock(usart);
	usart_set_mode(usart, USART_CMODE_MSPI_gc);
	port_pin_t sck_pin;

	if (opt->spimode == 1 || opt->spimode == 3) {
		//! \todo Fix when UCPHA_bm is added to header file.
		usart->CTRLC |= 0x02;
	} else {
		//! \todo Fix when UCPHA_bm is added to header file.
		usart->CTRLC &= ~0x02;
	}

	// configure Clock polarity using INVEN bit of the correct SCK I/O port
	if (opt->spimode == 2 || opt->spimode == 3) {
#ifdef USARTC0
		if ((uint16_t)usart == (uint16_t)&USARTC0) {
			sck_pin = IOPORT_CREATE_PIN(PORTC, 1);
			ioport_configure_port_pin(ioport_pin_to_port(sck_pin),
					ioport_pin_to_mask(sck_pin),
					IOPORT_DIR_OUTPUT | IOPORT_INIT_HIGH | IOPORT_INV_ENABLED);
		}
#endif
#ifdef USARTC1
		if ((uint16_t)usart == (uint16_t)&USARTC1) {
			sck_pin = IOPORT_CREATE_PIN(PORTC, 5);
			ioport_configure_port_pin(ioport_pin_to_port(sck_pin),
					ioport_pin_to_mask(sck_pin),
					IOPORT_DIR_OUTPUT | IOPORT_INIT_HIGH | IOPORT_INV_ENABLED);
		}
#endif
#ifdef USARTD0
		if ((uint16_t)usart == (uint16_t)&USARTD0) {
			sck_pin = IOPORT_CREATE_PIN(PORTD, 1);
			ioport_configure_port_pin(ioport_pin_to_port(sck_pin),
					ioport_pin_to_mask(sck_pin),
					IOPORT_DIR_OUTPUT | IOPORT_INIT_HIGH | IOPORT_INV_ENABLED);
		}
#endif
#ifdef USARTD1
		if ((uint16_t)usart == (uint16_t)&USARTD1) {
			sck_pin = IOPORT_CREATE_PIN(PORTD, 5);
			ioport_configure_port_pin(ioport_pin_to_port(sck_pin),
					ioport_pin_to_mask(sck_pin),
					IOPORT_DIR_OUTPUT | IOPORT_INIT_HIGH | IOPORT_INV_ENABLED);
		}
#endif
#ifdef USARTE0
		if ((uint16_t)usart == (uint16_t)&USARTE0) {
			sck_pin = IOPORT_CREATE_PIN(PORTE, 1);
			ioport_configure_port_pin(ioport_pin_to_port(sck_pin),
					ioport_pin_to_mask(sck_pin),
					IOPORT_DIR_OUTPUT | IOPORT_INIT_HIGH | IOPORT_INV_ENABLED);
		}
#endif
#ifdef USARTE1
		if ((uint16_t)usart == (uint16_t)&USARTE1) {
			sck_pin = IOPORT_CREATE_PIN(PORTE, 5);
			ioport_configure_port_pin(ioport_pin_to_port(sck_pin),
					ioport_pin_to_mask(sck_pin),
					IOPORT_DIR_OUTPUT | IOPORT_INIT_HIGH | IOPORT_INV_ENABLED);
		}
#endif
#ifdef USARTF0
		if ((uint16_t)usart == (uint16_t)&USARTF0) {
			sck_pin = IOPORT_CREATE_PIN(PORTF, 1);
			ioport_configure_port_pin(ioport_pin_to_port(sck_pin),
					ioport_pin_to_mask(sck_pin),
					IOPORT_DIR_OUTPUT | IOPORT_INIT_HIGH | IOPORT_INV_ENABLED);
		}
#endif
#ifdef USARTF1
		if ((uint16_t)usart == (uint16_t)&USARTF1) {
			sck_pin = IOPORT_CREATE_PIN(PORTF, 5);
			ioport_configure_port_pin(ioport_pin_to_port(sck_pin),
					ioport_pin_to_mask(sck_pin),
					IOPORT_DIR_OUTPUT | IOPORT_INIT_HIGH | IOPORT_INV_ENABLED);
		}
#endif
	}

	usart_spi_set_baudrate(usart, opt->baudrate, sysclk_get_per_hz());
	usart_tx_enable(usart);
	usart_rx_enable(usart);
}

/**
 * \brief Send a data with the USART module
 *
 * This function outputs a data using the USART module.
 *
 * \param usart The USART module.
 * \param c The data to send.
 *
 * \return STATUS_OK
 */
enum status_code usart_putchar(USART_t *usart, uint8_t c)
{
	while (usart_data_register_is_empty(usart) == false);
	(usart)->DATA = c;
	return STATUS_OK;
}

/**
 * \brief Receive a data with the USART module
 *
 * This function returns the received data from the USART module.
 *
 * \param usart The USART module.
 *
 * \return The received data.
 */
uint8_t usart_getchar(USART_t *usart)
{
	while (usart_rx_is_complete(usart) == false);
	return ((uint8_t) (usart)->DATA);
}

/**
 * \brief Set the baudrate value in the USART module
 *
 * This function sets the baudrate register regarding the CPU frequency.
 *
 * \param usart The USART module.
 * \param baud The baudrate.
 * \param cpu_hz The CPU frequency.
 *
 * \retval true if the hardware supports the baud rate
 * \retval false if the hardware does not support the baud rate (i.e. it's
 *               either too high or too low.)
 */
bool usart_set_baudrate(USART_t *usart, uint32_t baud, uint32_t cpu_hz)
{
	int8_t exp;
	uint32_t div;
	uint32_t limit;
	uint32_t ratio;
	uint32_t min_rate;
	uint32_t max_rate;

	/*
	 * Check if the hardware supports the given baud rate
	 */
	// 8 = (2^0) * 8 * (2^0) = (2^BSCALE_MIN) * 8 * (BSEL_MIN)
	max_rate = cpu_hz / 8;
	// 4194304 = (2^7) * 8 * (2^12) = (2^BSCALE_MAX) * 8 * (BSEL_MAX+1)
	min_rate = cpu_hz / 4194304;

	if (!((usart)->CTRLB & USART_CLK2X_bm)) {
		max_rate /= 2;
		min_rate /= 2;
	}

	if ((baud > max_rate) || (baud < min_rate)) {
		return false;
	}

	/*
	 * Check if double speed is enabled.
	 */
	if (!((usart)->CTRLB & USART_CLK2X_bm)) {
		baud *= 2;
	}

	/*
	 * Find the lowest possible exponent.
	 */
	limit = 0xfffU >> 4;
	ratio = cpu_hz / baud;

	for (exp = -7; exp < 7; exp++) {
		if (ratio < limit) {
			break;
		}

		limit <<= 1;

		if (exp < -3) {
			limit |= 1;
		}
	}

	/*
	 * Depending on the value of exp, scale either the input frequency or
	 * the target baud rate. By always scaling upwards, we never introduce
	 * any additional inaccuracy.
	 *
	 * We are including the final divide-by-8 (aka. right-shift-by-3) in this
	 * operation as it ensures that we never exceeed 2**32 at any point.
	 *
	 * The formula for calculating BSEL is slightly different when exp is
	 * negative than it is when exp is positive.
	 */
	if (exp < 0) {
		/*
		 * We are supposed to subtract 1, then apply BSCALE. We want to apply
		 * BSCALE first, so we need to turn everything inside the parenthesis
		 * into a single fractional expression.
		 */
		cpu_hz -= 8 * baud;
		/*
		 * If we end up with a left-shift after taking the final divide-by-8
		 * into account, do the shift before the divide. Otherwise, left-shift
		 * the denominator instead (effectively resulting in an overall right 
		 * shift.)
		 */
		if (exp <= -3) {
			div = ((cpu_hz << (-exp - 3)) + baud / 2) / baud;
		} else {
			baud <<= exp + 3;
			div = (cpu_hz + baud / 2) / baud;
		}
	} else {
		/*
		 * We will always do a right shift in this case, but we need to shift
		 * three extra positions because of the divide-by-8.
		 */
		baud <<= exp + 3;
		div = (cpu_hz + baud / 2) / baud - 1;
	}

	(usart)->BAUDCTRLB = (uint8_t)(((div >> 8) & 0X0F) | (exp << 4));
	(usart)->BAUDCTRLA = (uint8_t)div;

	return true;
}

/**
 * \brief Set the baudrate value in the USART_SPI module
 *
 * This function sets the baudrate register regarding the CPU frequency.
 *
 * \param usart The USART(SPI) module.
 * \param baud The baudrate.
 * \param cpu_hz The CPU frequency.
 */
void usart_spi_set_baudrate(USART_t *usart, uint32_t baud, uint32_t cpu_hz)
{
	uint16_t bsel_value = (uint16_t)((((((cpu_hz) << 1)
						/ (baud * 2)) + 1) >> 1) - 1);
	(usart)->BAUDCTRLB = (uint8_t)((~USART_BSCALE_gm) & (bsel_value >> 8));
	(usart)->BAUDCTRLA = (uint8_t)(bsel_value);
}
