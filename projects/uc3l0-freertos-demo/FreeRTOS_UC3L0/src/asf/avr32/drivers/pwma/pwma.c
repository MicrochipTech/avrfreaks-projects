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


#include "pwma.h"


void pwma_config_and_enable(volatile avr32_pwma_t *pwma,
                            unsigned long long int channel_mask,
                            int period_cycles, int duty_cycles)
{
  pwma->cr =
      (period_cycles << AVR32_PWMA_CR_TOP_OFFSET) // Set the TOP and SPREAD values of the timebase counter
      | AVR32_PWMA_CR_TCLR_MASK                   // Clear the timebase counter
      | AVR32_PWMA_CR_EN_MASK;                    // Enable the PWMA

  pwma_set_channels_value(pwma, channel_mask, duty_cycles);
}


void pwma_set_channels_value( volatile avr32_pwma_t *pwma,
                              unsigned long long int channel_mask,
                              int duty_cycles)
{
  unsigned long int channel_mask_low = channel_mask & 0xffffffff;
  unsigned long int channel_mask_high = channel_mask >> 32;

  //#
  //# Interlinked single value PWM operation: several channels are interlinked
  //# to allow multiple channels to be updated simultaneously with the same duty
  //# cycle
  //#

  // Wait for the PWMA to be ready for writing.
  while (pwma->sr & AVR32_PWMA_SR_BUSY_MASK);

  //#
  //# First 32 channels
  //#
  // Set the common duty cycle value.
  pwma->isduty = duty_cycles;
  // Define the interlinked channels
#if (UC3D)
  pwma->chgroup[0].ischset = channel_mask_low;
#else
  pwma->ischset0 = channel_mask_low;
#endif

  // Wait for the PWMA to be ready for writing.
  while (pwma->sr & AVR32_PWMA_SR_BUSY_MASK);

  //#
  //# Last 32 channels
  //#
  // Set the common duty cycle value.
  pwma->isduty = duty_cycles;
  // Define the interlinked channels
#if (UC3D)
  pwma->chgroup[1].ischset = channel_mask_high;
#else
  pwma->ischset1 = channel_mask_high;
#endif
  // Wait for the PWMA to be ready for writing.
  while (pwma->sr & AVR32_PWMA_SR_BUSY_MASK);
}


void pwma_set_multiple_values( volatile avr32_pwma_t *pwma,
                               unsigned long long int channel_mask,
                               unsigned char *channel_duty_cycles)
{
  int           current_channel = 0;
  unsigned char channels[4];
  int           i = 0;


  do
  {
    if ((channel_mask >> i) & 0x1)
    {
      channels[current_channel++] = i;
    }
    i++;
  } while (current_channel < 4 && i < 64);

  // Wait for the PWMA to be ready for writing.
  while (pwma->sr & AVR32_PWMA_SR_BUSY_MASK);

  pwma->imduty = (channel_duty_cycles[3] << 24)
      | (channel_duty_cycles[2] << 16)
      | (channel_duty_cycles[1] << 8)
      | (channel_duty_cycles[0]);
  pwma->imchsel = (channels[3] << 24)
      | (channels[2] << 16)
      | (channels[1] << 8)
      | (channels[0]);

  // Wait for the PWMA to be ready for writing.
  while (pwma->sr & AVR32_PWMA_SR_BUSY_MASK);
}


void pwma_disable(volatile avr32_pwma_t *pwma)
{
  pwma->cr &= ~AVR32_PWMA_EN_MASK;
}
