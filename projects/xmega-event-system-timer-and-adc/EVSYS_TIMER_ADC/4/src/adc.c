/**
 * \file
 *
 * \brief ADC related functionality implementation.
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

#include <adc.h>

/**
 * \brief Initialize ADC interface
 */
int8_t ADC_0_init()
{

	// ADCA.CAL = 0; /* Calibration Value: 0 */

	ADCA.CH0.CTRL = ADC_CH_GAIN_1X_gc                  /* 1x gain */
	                | ADC_CH_INPUTMODE_SINGLEENDED_gc; /* Single-ended input, no gain */

	ADCA.CH0.MUXCTRL = ADC_CH_MUXPOS_PIN1_gc    /* Input pin 1 */
	                   | ADC_CH_MUXNEG_PIN0_gc; /* Input pin 0 (Input Mode = 2) */

	ADCA.CH0.INTCTRL = ADC_CH_INTMODE_COMPLETE_gc /* Interrupt on conversion complete */
	                   | ADC_CH_INTLVL_LO_gc;     /* Low level */

	// ADCA.CH0.SCAN = 0 << ADC_CH_SCANNUM_gp /* Number of Channels included in scan: 0 */;

	// ADCA.CH1.CTRL = ADC_CH_GAIN_1X_gc /* 1x gain */
	//		 | ADC_CH_INPUTMODE_INTERNAL_gc; /* Internal inputs, no gain */

	// ADCA.CH1.MUXCTRL = ADC_CH_MUXINT_TEMP_gc /* Temperature Reference */
	//		 | ADC_CH_MUXNEG_PIN0_gc; /* Input pin 0 (Input Mode = 2) */

	// ADCA.CH1.INTCTRL = ADC_CH_INTMODE_COMPLETE_gc /* Interrupt on conversion complete */
	//		 | ADC_CH_INTLVL_OFF_gc; /* Interrupt disabled */

	// ADCA.CH2.CTRL = ADC_CH_GAIN_1X_gc /* 1x gain */
	//		 | ADC_CH_INPUTMODE_INTERNAL_gc; /* Internal inputs, no gain */

	// ADCA.CH2.MUXCTRL = ADC_CH_MUXINT_TEMP_gc /* Temperature Reference */
	//		 | ADC_CH_MUXNEG_PIN0_gc; /* Input pin 0 (Input Mode = 2) */

	// ADCA.CH2.INTCTRL = ADC_CH_INTMODE_COMPLETE_gc /* Interrupt on conversion complete */
	//		 | ADC_CH_INTLVL_OFF_gc; /* Interrupt disabled */

	// ADCA.CH3.CTRL = ADC_CH_GAIN_1X_gc /* 1x gain */
	//		 | ADC_CH_INPUTMODE_INTERNAL_gc; /* Internal inputs, no gain */

	// ADCA.CH3.MUXCTRL = ADC_CH_MUXINT_TEMP_gc /* Temperature Reference */
	//		 | ADC_CH_MUXNEG_PIN0_gc; /* Input pin 0 (Input Mode = 2) */

	// ADCA.CH3.INTCTRL = ADC_CH_INTMODE_COMPLETE_gc /* Interrupt on conversion complete */
	//		 | ADC_CH_INTLVL_OFF_gc; /* Interrupt disabled */

	// ADCA.CTRLB = 0 << ADC_IMPMODE_bp /* Gain Stage Impedance Mode: disabled */
	//		 | ADC_CURRLIMIT_NO_gc /* No limit */
	//		 | 0 << ADC_CONMODE_bp /* Conversion Mode: disabled */
	//		 | 0 << ADC_FREERUN_bp /* Free Running Mode Enable: disabled */
	//		 | ADC_RESOLUTION_12BIT_gc; /* 12-bit right-adjusted result */

	ADCA.EVCTRL = ADC_SWEEP_0_gc      /* ADC Channel 0 */
	              | ADC_EVSEL_0123_gc /* Event Channel 0,1,2,3 */
	              | ADC_EVACT_CH0_gc; /* First event triggers channel 0 */

	ADCA.REFCTRL = ADC_REFSEL_AREFA_gc    /* External reference on PORT A */
	               | 0 << ADC_BANDGAP_bp  /* Bandgap enable: disabled */
	               | 0 << ADC_TEMPREF_bp; /* Temperature Reference Enable: disabled */

	// ADCA.PRESCALER = ADC_PRESCALER_DIV4_gc; /* Divide clock by 4 */

	// ADCA.CMP = 0; /* Compare Value: 0 */

	ADCA.CTRLA = 0 << ADC_FLUSH_bp    /* Flush Pipeline: disabled */
	             | 1 << ADC_ENABLE_bp /* Enable ADC: enabled */
	             | ADC_DMASEL_OFF_gc; /* Combined DMA request OFF */

	return 0;
}
