/**
 * \file
 *
 * \brief EVSYS related functionality implementation.
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

#include <evsys.h>

/**
 * \brief Initialize evsys interface
 */
int8_t EVENT_SYSTEM_0_init()
{

	EVSYS.CH0MUX = EVSYS_CHMUX_TCC0_OVF_gc; /* Timer/Counter C0 Overflow */

	// EVSYS.CH1MUX = EVSYS_CHMUX_OFF_gc; /* Off */

	// EVSYS.CH2MUX = EVSYS_CHMUX_OFF_gc; /* Off */

	// EVSYS.CH3MUX = EVSYS_CHMUX_OFF_gc; /* Off */

	// EVSYS.CH4MUX = EVSYS_CHMUX_OFF_gc; /* Off */

	// EVSYS.CH5MUX = EVSYS_CHMUX_OFF_gc; /* Off */

	// EVSYS.CH6MUX = EVSYS_CHMUX_OFF_gc; /* Off */

	// EVSYS.CH7MUX = EVSYS_CHMUX_OFF_gc; /* Off */

	// EVSYS.CH0CTRL = 0 << EVSYS_QDEN_bp /* Quadrature Decode Enable: disabled */
	//		 | 0 << EVSYS_QDIEN_bp /* Quadrature Decode Index Enable: disabled */
	//		 | EVSYS_QDIRM_00_gc /* QDPH0 = 0, QDPH90 = 0 */
	//		 | EVSYS_DIGFILT_1SAMPLE_gc; /* 1 SAMPLE */

	// EVSYS.CH1CTRL = EVSYS_DIGFILT_1SAMPLE_gc; /* 1 SAMPLE */

	// EVSYS.CH2CTRL = 0 << EVSYS_QDEN_bp /* Quadrature Decode Enable: disabled */
	//		 | 0 << EVSYS_QDIEN_bp /* Quadrature Decode Index Enable: disabled */
	//		 | EVSYS_QDIRM_00_gc /* QDPH0 = 0, QDPH90 = 0 */
	//		 | EVSYS_DIGFILT_1SAMPLE_gc; /* 1 SAMPLE */

	// EVSYS.CH3CTRL = EVSYS_DIGFILT_1SAMPLE_gc; /* 1 SAMPLE */

	// EVSYS.CH4CTRL = 0 << EVSYS_QDEN_bp /* Quadrature Decode Enable: disabled */
	//		 | 0 << EVSYS_QDIEN_bp /* Quadrature Decode Index Enable: disabled */
	//		 | EVSYS_QDIRM_00_gc /* QDPH0 = 0, QDPH90 = 0 */
	//		 | EVSYS_DIGFILT_1SAMPLE_gc; /* 1 SAMPLE */

	// EVSYS.CH5CTRL = EVSYS_DIGFILT_1SAMPLE_gc; /* 1 SAMPLE */

	// EVSYS.CH6CTRL = EVSYS_DIGFILT_1SAMPLE_gc; /* 1 SAMPLE */

	// EVSYS.CH7CTRL = EVSYS_DIGFILT_1SAMPLE_gc; /* 1 SAMPLE */

	return 0;
}
