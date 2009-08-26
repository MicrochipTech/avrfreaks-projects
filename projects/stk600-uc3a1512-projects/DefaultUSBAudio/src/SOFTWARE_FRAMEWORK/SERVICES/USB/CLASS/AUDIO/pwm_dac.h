/* This header file is part of the ATMEL AVR32-SoftwareFramework-AT32UC3A-1.4.0 Release */

/*This file is prepared for Doxygen automatic documentation generation.*/
/*! \file *********************************************************************
 *
 * \brief PWM mono audio DAC driver for AVR32.
 *
 * This driver constructs a 16-bit DAC output thanks to two 8-bit PWM channels
 * combined with a resistor ladder network.
 *
 * - Compiler:           IAR EWAVR32 and GNU GCC for AVR32
 * - Supported devices:  All AVR32 devices with a PWM module can be used.
 * - AppNote:
 *
 * \author               Atmel Corporation: http://www.atmel.com \n
 *                       Support and FAQ: http://support.atmel.no/
 *
 ******************************************************************************/

/* Copyright (C) 2006-2008, Atmel Corporation All rights reserved.
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
 * 3. The name of ATMEL may not be used to endorse or promote products derived
 * from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY ATMEL ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE EXPRESSLY AND
 * SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */


#ifndef _PWM_DAC_H_
#define _PWM_DAC_H_

#include <stddef.h>

#include "compiler.h"
#include "conf_pwm_dac.h"


/*! \brief Starts the DAC.
 */
extern void pwm_dac_start(U32 sample_rate_hz,
                          U8 num_channels,
                          U8 bits_per_sample,
                          Bool swap_channels,
                          void (*out_of_samples_callback)(void),
                          void (*samples_sent_callback)(void),
                          U32 pba_hz);

/*! \brief Sets the DAC up with new settings.
 *
 * \note The DAC must have been started beforehand.
 */
extern void pwm_dac_setup(U32 sample_rate_hz,
                          U8 num_channels,
                          U8 bits_per_sample,
                          Bool swap_channels,
                          void (*out_of_samples_callback)(void),
                          void (*samples_sent_callback)(void),
                          U32 pba_hz);

/*! \brief Outputs a sample buffer to the DAC.
 *
 * \note The DAC must have been started beforehand.
 */
extern Bool pwm_dac_output(void *sample_buffer, size_t sample_length);

/*! \brief Increases the output volume of the DAC.
 *
 * \note The DAC must have been started beforehand.
 */
extern void pwm_dac_increase_volume(void);

/*! \brief Decreases the output volume of the DAC.
 *
 * \note The DAC must have been started beforehand.
 */
extern void pwm_dac_decrease_volume(void);

/*! \brief Flushes the sample buffer being output to the DAC.
 */
extern void pwm_dac_flush(void);

/*! \brief Stops the DAC.
 */
extern void pwm_dac_stop(void);


#endif  // _PWM_DAC_H_
