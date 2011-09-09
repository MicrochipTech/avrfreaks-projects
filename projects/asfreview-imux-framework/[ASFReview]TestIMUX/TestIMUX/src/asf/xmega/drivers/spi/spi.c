/*This file has been prepared for Doxygen automatic documentation generation.*/
/*! \file *********************************************************************
 *
 * \brief SPI software driver functions.
 *
 * - Compiler:           GCC and IAR for AVR
 * - Supported devices:  All AVR XMega devices with SPI module can be used.
 * - AppNote:
 *
 * \author               Atmel Corporation: http://www.atmel.com \n
 *                       Support and FAQ: http://support.atmel.no/
 *
 *****************************************************************************/

/* Copyright (c) 2010 Atmel Corporation. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * 3. The name of Atmel may not be used to endorse or promote products derived
 * from this software without specific prior written permission.
 *
 * 4. This software may only be redistributed and used in connection with an Atmel
 * AVR product.
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
 *
 */
#include "spi.h"

/*! \brief Calculates the SPI baudrate divider.
 *
 * \param baudrate The expected baudrate on the SPI.
 * \param clkper_hz  SPI module input clock frequency (Peripheral clock, Hz).
 * \param spi The SPI module address
 *
 * \return Status of operation.
 *   \retval >=0  Success.
 *   \retval  <0  Error.
 */
int8_t spi_xmega_set_baud_div(SPI_t *spi, uint32_t baudrate, uint32_t clkper_hz)
{
	uint32_t clkper_hz_r = clkper_hz / 10000;
	uint32_t baudrate_r  = baudrate / 10000;
	
	switch (clkper_hz_r / baudrate_r) 
	{
		case 2:
			spi->CTRL |= (1 << SPI_CLK2X_bp) |
					(0 << SPI_PRESCALER1_bp) | (0 << SPI_PRESCALER1_bp);
			return 1;
		case 4:
			spi->CTRL |= (0 << SPI_CLK2X_bp) |
					(0 << SPI_PRESCALER1_bp) | (0 << SPI_PRESCALER1_bp);
			return 1;			
		case 8:
			spi->CTRL |= (1 << SPI_CLK2X_bp) |
					(0 << SPI_PRESCALER1_bp) | (1 << SPI_PRESCALER1_bp);
			return 1;			
		case 16:
			spi->CTRL |= (0 << SPI_CLK2X_bp) |
					(0 << SPI_PRESCALER1_bp) | (1 << SPI_PRESCALER1_bp);
			return 1;
		case 32:
			spi->CTRL |= (1 << SPI_CLK2X_bp) |
					(1 << SPI_PRESCALER1_bp) | (0 << SPI_PRESCALER1_bp);
			return 1;
		case 64:
			spi->CTRL |= (0 << SPI_CLK2X_bp) |
					(1 << SPI_PRESCALER1_bp) | (0 << SPI_PRESCALER1_bp);
			return 1;
		case 128:
			spi->CTRL |= (0 << SPI_CLK2X_bp) |
					(1 << SPI_PRESCALER1_bp) | (1 << SPI_PRESCALER1_bp);
			return 1;
		default:
			return -1;
	}
}