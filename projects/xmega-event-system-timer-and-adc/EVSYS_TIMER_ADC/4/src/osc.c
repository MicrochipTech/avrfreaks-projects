/**
 * \file
 *
 * \brief OSC related functionality implementation.
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

#include <osc.h>
#include <ccp.h>

/**
 * \brief Initialize OSC interface
 */
int8_t OSC_init()
{

	// OSC.XOSCCTRL = OSC_FRQRANGE_04TO2_gc /* 0.4 - 2 MHz */
	//		 | 0 << OSC_X32KLPM_bp /* 32.768 kHz XTAL OSC Low-power Mode: disabled */
	//		 | 0 << OSC_XOSCPWR_bp /* Crystal Oscillator Drive: disabled */
	//		 | OSC_XOSCSEL_EXTCLK_gc; /* External Clock - 6 CLK */

	// OSC.XOSCFAIL = 0 << OSC_PLLFDIF_bp /* PLL Failure Detection Interrupt Flag: disabled */
	//		 | 0 << OSC_XOSCFDIF_bp; /* XOSC Failure Detection Interrupt Flag: disabled */

	// ccp_write_io((void*)&(OSC.XOSCFAIL),0 << OSC_PLLFDEN_bp /* PLL Failure Detection Enable: disabled */
	//		 | 0 << OSC_XOSCFDEN_bp /* XOSC Failure Detection Enable: disabled */);

	// OSC.RC32KCAL = 0; /* Oscillator Calibration Value: 0 */

	// OSC.PLLCTRL = OSC_PLLSRC_RC2M_gc /* 2MHz Internal Oscillator */
	//		 | 1 << OSC_PLLDIV_bp /* PLL divided output: enabled */
	//		 | 1 << OSC_PLLFAC_gp; /* PLL Multiplication Factor: 1 */

	// OSC.CTRL = 0 << OSC_PLLEN_bp /* PLL Enable: disabled */
	//		 | 0 << OSC_XOSCEN_bp /* External Oscillator Enable: disabled */
	//		 | 0 << OSC_RC32KEN_bp /* Internal 32kHz RC Oscillator Enable: disabled */
	//		 | 0 << OSC_RC32MEN_bp /* Internal 32MHz RC Oscillator Enable: disabled */
	//		 | 1 << OSC_RC2MEN_bp; /* Internal 2MHz RC Oscillator Enable: enabled */

	// Wait for the Oscillators to be stable

	while (!(OSC.STATUS & OSC_RC2MRDY_bm)) {
		/* Wait for 2MHz Internal Oscillator to be stable */
	}

	// OSC.DFLLCTRL = OSC_RC32MCREF_RC32K_gc /* Internal 32.768 kHz RC Oscillator */
	//		 | 0 << OSC_RC2MCREF_bp; /* DFLL 2 MHz DFLL Calibration Reference Enable: disabled */

	return 0;
}
