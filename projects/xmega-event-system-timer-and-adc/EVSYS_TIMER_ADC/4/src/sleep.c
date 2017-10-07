/**
 * \file
 *
 * \brief SLEEP related functionality implementation.
 *
 *
 * Copyright (C) 2016 Atmel Corporation. All rights reserved.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. The name of Atmel may not be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * 4. This software may only be redistributed and used in connection with an
 *    Atmel microcontroller product.
 *
 * THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * \asf_license_stop
 *
 *
 */

#include <sleep.h>

/**
 * \brief Initialize sleep interface
 */
int8_t SLEEP_init()
{

	/* Sleep Configuration*/

	// SLEEP.CTRL = SLEEP_SMODE_IDLE_gc /* Idle mode */
	//		 | 0 << SLEEP_SEN_bp; /* Sleep Enable: disabled */

	/* Power Reduction Configuration*/

	// PR.PRGEN = 0 << PR_USB_bp /* USB power down: disabled */
	//		 | 0 << PR_AES_bp /* AES power down: disabled */
	//		 | 0 << PR_EBI_bp /* EBI power down: disabled */
	//		 | 0 << PR_RTC_bp /* RTC power down: disabled */
	//		 | 0 << PR_EVSYS_bp /* EVSYS power down: disabled */
	//		 | 0 << PR_DMA_bp /* DMA power down: disabled */;

	// PR.PRPA = 0 << PR_DAC_bp /* DACA power down: disabled */
	//		 | 0 << PR_ADC_bp /* ADCA power down: disabled */
	//		 | 0 << PR_AC_bp; /* ACA power down: disabled */

	// PR.PRPB = 0 << PR_DAC_bp /* DACB power down: disabled */
	//		 | 0 << PR_ADC_bp /* ADCB power down: disabled */
	//		 | 0 << PR_AC_bp; /* ACB power down: disabled */

	// PR.PRPC = 0 << PR_TWI_bp /* TWIC power down: disabled */
	//		 | 0 << PR_USART1_bp /* USART1C power down: disabled */
	//		 | 0 << PR_USART0_bp /* USART0C power down: disabled */
	//		 | 0 << PR_SPI_bp /* SPIC power down: disabled */
	//		 | 0 << PR_HIRES_bp /* HIRESC power down: disabled */
	//		 | 0 << PR_TC1_bp /* TC1C power down: disabled */
	//		 | 0 << PR_TC0_bp /* TC0C power down: disabled */;

	// PR.PRPD = 0 << PR_TWI_bp /* TWID power down: disabled */
	//		 | 0 << PR_USART1_bp /* USART1D power down: disabled */
	//		 | 0 << PR_USART0_bp /* USART0D power down: disabled */
	//		 | 0 << PR_SPI_bp /* SPID power down: disabled */
	//		 | 0 << PR_HIRES_bp /* HIRESD power down: disabled */
	//		 | 0 << PR_TC1_bp /* TC1D power down: disabled */
	//		 | 0 << PR_TC0_bp /* TC0D power down: disabled */;

	// PR.PRPE = 0 << PR_TWI_bp /* TWIE power down: disabled */
	//		 | 0 << PR_USART1_bp /* USART1E power down: disabled */
	//		 | 0 << PR_USART0_bp /* USART0E power down: disabled */
	//		 | 0 << PR_SPI_bp /* SPIE power down: disabled */
	//		 | 0 << PR_HIRES_bp /* HIRESE power down: disabled */
	//		 | 0 << PR_TC1_bp /* TC1E power down: disabled */
	//		 | 0 << PR_TC0_bp /* TC0E power down: disabled */;

	// PR.PRPF = 0 << PR_TWI_bp /* TWIF power down: disabled */
	//		 | 0 << PR_USART1_bp /* USART1F power down: disabled */
	//		 | 0 << PR_USART0_bp /* USART0F power down: disabled */
	//		 | 0 << PR_SPI_bp /* SPIF power down: disabled */
	//		 | 0 << PR_HIRES_bp /* HIRESF power down: disabled */
	//		 | 0 << PR_TC1_bp /* TC1F power down: disabled */
	//		 | 0 << PR_TC0_bp /* TC0F power down: disabled */;

	return 0;
}
