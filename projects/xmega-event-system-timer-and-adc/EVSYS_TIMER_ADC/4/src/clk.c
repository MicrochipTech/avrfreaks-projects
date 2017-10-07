/**
 * \file
 *
 * \brief CLK related functionality implementation.
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

#include <clk.h>
#include <ccp.h>

/**
 * \brief Initialize CLK interface
 */
int8_t CLK_init()
{

	// ccp_write_io((void*)&(CLK.CTRL),CLK_SCLKSEL_RC2M_gc /* 2MHz Internal Oscillator */);

	// ccp_write_io((void*)&(CLK.PSCTRL),CLK_PSADIV_1_gc /* Divide by 1 */
	//		 | CLK_PSBCDIV_1_1_gc /* Divide B by 1 and C by 1 */);

	// ccp_write_io((void*)&(CLK.LOCK),0 << CLK_LOCK_bp /* Clock System Lock: disabled */);

	// CLK.RTCCTRL = CLK_RTCSRC_ULP_gc /* Ultra Low Power Internal Oscillator */
	//		 | 0 << CLK_RTCEN_bp; /* RTC Clock Source Enable: disabled */

	// CLK.USBCTRL = 0 << CLK_USBPSDIV_gp /* Prescaler Division Factor: 0 */
	//		 | CLK_USBSRC_PLL_gc /* PLL */
	//		 | 0 << CLK_USBSEN_bp; /* Clock Source Enable: disabled */

	return 0;
}
