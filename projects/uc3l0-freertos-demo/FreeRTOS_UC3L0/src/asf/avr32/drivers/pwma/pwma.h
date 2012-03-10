/*****************************************************************************
 *
 * \file
 *
 * \brief Basic Pulse Width Modulation Controller (PWMA) driver.
 *
 * Copyright (c) 2009 Atmel Corporation. All rights reserved.
 *
 * \asf_license_start
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
 *****************************************************************************/


#ifndef _PWMA_H_
#define _PWMA_H_

/**
 * \defgroup group_avr32_drivers_pwma PWMA - Pulse Width Modulation A
 *
 * The Pulse Width Modulation A Controller (PWMA) controls several pulse width modulation (PWM) channels.
 * This driver provides an API to get access to the main features of the PWMA controller.
 *
 * \{
 */

#include <avr32/io.h>
#include "compiler.h"


/*! \brief Configure and enable the PWMA.
 *
 * This function will enable the PWMA module and configure a mask of channels.
 *
 * \param pwma Pointer to the PWMA module.
 * \param channel_mask Bit mask of channels to set period_cycles for.
 * \param period_cycles Period cycles for the PWMA module, i.e. the TOP value (and eventually the SPREAD value).
 * \param duty_cycles Number of cycles for the active period for the channels
 *                    provided by channel_mask.
 */
void pwma_config_and_enable(volatile avr32_pwma_t *pwma,
                            unsigned long long int channel_mask,
                            int period_cycles, int duty_cycles);

/*! \brief Change the number of cycles for the active period for a channel mask.
 *
 * \param pwma Pointer to the PWMA module.
 * \param channel_mask Bit mask of the channels to change number of cycles for.
 * \param duty_cycles Number of cycles for the active period for the channels
 *                    provided by channel_mask.
 */
void pwma_set_channels_value( volatile avr32_pwma_t *pwma,
                              unsigned long long int channel_mask,
                              int duty_cycles);

/*! \brief Change a multiple channel period cycles into multiple channels (up to four channels).
 *
 * \param pwma pointer to the PWMA module.
 * \param channel_mask bit mask of the channels to change number of cycles for.
 * \param channel_duty_cycles pointer to an array of max four channel duty cycles,
 *        the first element in the array points to the lowest numbered channel to
 *        change and so on.
 */
void pwma_set_multiple_values(volatile avr32_pwma_t *pwma,
                              unsigned long long int channel_mask,
                              unsigned char *channel_duty_cycles);

/*! \brief Disable a PWMA module
 *
 * \param pwma pointer to the PWMA module.
 */
void pwma_disable(volatile avr32_pwma_t *pwma);

/**
 * \}
 */

#endif /* _PWMA_H_ */
