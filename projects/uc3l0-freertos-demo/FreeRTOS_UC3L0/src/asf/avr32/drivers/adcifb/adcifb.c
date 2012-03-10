/*****************************************************************************
 *
 * \file
 *
 * \brief ADCIFB driver for AVR32 UC3.
 *
 * This file defines a useful set of functions for ADC on AVR32 devices.
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


#include <avr32/io.h>
#include "compiler.h"
#include "adcifb.h"


long int adcifb_configure( volatile avr32_adcifb_t *adcifb,
                           const adcifb_opt_t *p_adcifb_opt )
{
  unsigned long int prescal_tempo;


  // Enable the ADCIFB module.
  adcifb->cr = AVR32_ADCIFB_CR_EN_MASK;

  // Compute the PRESCAL field according to the ratio Fclk_adcifb/Fclk_adc.
  // PRESCAL = ((Fclk_adcifb/Fclk_adc)/2)-1
  prescal_tempo = (p_adcifb_opt->ratio_clkadcifb_clkadc >> 1) -1;

  // Set the ADC configuration
  adcifb->acr  = (p_adcifb_opt->sleep_mode_enable << AVR32_ADCIFB_ACR_SLEEP_OFFSET)
               | (p_adcifb_opt->resolution << AVR32_ADCIFB_ACR_RES_OFFSET)
               | (prescal_tempo << AVR32_ADCIFB_ACR_PRESCAL_OFFSET)
               | (p_adcifb_opt->startup << AVR32_ADCIFB_ACR_STARTUP_OFFSET)
               | (p_adcifb_opt->shtim << AVR32_ADCIFB_ACR_SHTIM_OFFSET);

  // Wait Startup Time, with a timeout.
  return(adcifb_sr_statushigh_wait(adcifb, AVR32_ADCIFB_SR_READY_MASK));
}


long int adcifb_configure_trigger( volatile avr32_adcifb_t *adcifb,
                                   unsigned char trgmod,
                                   unsigned short int trgper )
{
  // If the trigger mode is not set to "Periodic Trigger", clear the trgper field.
  if(AVR32_ADCIFB_TRGMOD_PT != trgmod)
    trgper = 0;
  // Configure the trigger mode
  adcifb->trgr = ((trgmod<<AVR32_ADCIFB_TRGMOD_OFFSET)&AVR32_ADCIFB_TRGMOD_MASK)
               | ((trgper<<AVR32_ADCIFB_TRGPER_OFFSET)&AVR32_ADCIFB_TRGPER_MASK);
  return PASS;
}


void adcifb_channels_enable( volatile avr32_adcifb_t *adcifb,
                             unsigned long int channels_mask )
{
  adcifb->cher |= channels_mask;
}

void adcifb_channels_disable( volatile avr32_adcifb_t *adcifb,
                              unsigned long int channels_mask )
{
  adcifb->chdr |= channels_mask;
}

void adcifb_start_conversion_sequence( volatile avr32_adcifb_t *adcifb )
{
  adcifb->cr = AVR32_ADCIFB_CR_START_MASK;
}


unsigned long adcifb_get_last_data( volatile avr32_adcifb_t *adcifb )
{
  return(adcifb->lcdr);
}


long int adcifb_sr_statushigh_wait( volatile avr32_adcifb_t *adcifb,
                                    unsigned long statusMask )
{
  unsigned int timeout = ADCIFB_POLL_TIMEOUT;

  while(!(adcifb->sr & statusMask))
  {
    if(--timeout == 0)
      return -1;
  }
  return PASS;
}


bool adcifb_is_ready( volatile avr32_adcifb_t *adcifb )
{
  return((adcifb->sr & AVR32_ADCIFB_SR_READY_MASK)>>AVR32_ADCIFB_SR_READY_OFFSET);
}


bool adcifb_is_drdy( volatile avr32_adcifb_t *adcifb )
{
  return((adcifb->sr & AVR32_ADCIFB_SR_DRDY_MASK)>>AVR32_ADCIFB_SR_DRDY_OFFSET);
}


bool adcifb_is_ovre( volatile avr32_adcifb_t *adcifb )
{
  return((adcifb->sr & AVR32_ADCIFB_SR_OVRE_MASK)>>AVR32_ADCIFB_SR_OVRE_OFFSET);
}
