/**
 * \file
 *
 * \brief TC related functionality implementation.
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
 */

#include <tc.h>

/**
 * \brief Initialize TC interface
 */
int8_t TIMER_0_init()
{

	// TCC0.CTRLB = 0 << TC0_CCDEN_bp /* Compare or Capture D Enable: disabled */
	//		 | 0 << TC0_CCCEN_bp /* Compare or Capture C Enable: disabled */
	//		 | 0 << TC0_CCBEN_bp /* Compare or Capture B Enable: disabled */
	//		 | 0 << TC0_CCAEN_bp /* Compare or Capture A Enable: disabled */
	//		 | TC_WGMODE_NORMAL_gc; /* Normal Mode */

	// TCC0.CTRLC = 0 << TC0_CMPD_bp /* Compare D Output Value: disabled */
	//		 | 0 << TC0_CMPC_bp /* Compare C Output Value: disabled */
	//		 | 0 << TC0_CMPB_bp /* Compare B Output Value: disabled */
	//		 | 0 << TC0_CMPA_bp /* Compare A Output Value: disabled */;

	// TCC0.CTRLD = TC_EVACT_OFF_gc /* No Event Action */
	//		 | 0 << TC0_EVDLY_bp /* Event Delay: disabled */
	//		 | TC_EVSEL_OFF_gc; /* No Event Source */

	// TCC0.CTRLE = 0; /* Byte Mode: 0 */

	// TCC0.INTCTRLB = TC_CCDINTLVL_OFF_gc /* Interrupt Disabled */
	//		 | TC_CCCINTLVL_OFF_gc /* Interrupt Disabled */
	//		 | TC_CCBINTLVL_OFF_gc /* Interrupt Disabled */
	//		 | TC_CCAINTLVL_OFF_gc /* Interrupt Disabled */;

	// TCC0.INTCTRLA = TC_ERRINTLVL_OFF_gc /* Interrupt Disabled */
	//		 | TC_OVFINTLVL_OFF_gc; /* Interrupt Disabled */

	// TCC0.CCA = 0 /* Compare or Capture A: 0 */;

	// TCC0.CCB = 0 /* Compare or Capture B: 0 */;

	// TCC0.CCC = 0; /* Compare or Capture C: 0 */

	// TCC0.CCD = 0; /* Compare or Capture D: 0 */

	// TCC0.CNT = 0; /* Count: 0 */

	TCC0.PER = 32768; /* Period: 32768 */

	TCC0.CTRLA = TC_CLKSEL_DIV64_gc; /* System Clock / 64 */

	return 0;
}
