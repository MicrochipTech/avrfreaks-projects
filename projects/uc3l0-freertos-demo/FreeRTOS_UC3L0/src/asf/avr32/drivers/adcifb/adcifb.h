/*****************************************************************************
 *
 * \file
 *
 * \brief ADCIFB Software driver interface for AVR32 UC3.
 *
 * This file defines a useful set of functions to interface with the ADCIFB module on AVR32 devices.
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


#ifndef _ADCIFB_H_
#define _ADCIFB_H_

/**
 * \defgroup group_avr32_drivers_adcifb ANALOG - ADCIFB - ADC Interface B
 *
 * Driver for the ADCIFB (Analog-to-Digital Converter Interface B).
 * Provides functions for configuration of conversion parameters
 * (up to 12-bit signed with 16x interpolation at 6 MHz), enabling channels
 * (max. 32, see device datasheet for details) and conversion triggering.
 * Resistive touch screen mode is not supported by this driver.
 *
 * \{
 */

#include <avr32/io.h>
#include "compiler.h"


//! The timeguard used for polling.
#define ADCIFB_POLL_TIMEOUT         10000

//! Parameters for initialization of the ADCIFB module
typedef struct
{
  //! Resolution mode
  unsigned short  resolution;

  //! Channels Sample & Hold Time in [0,15], where Ts&h = shtim * Tclk_adc.
  unsigned short  shtim;

  //! Ratio Fclk_adcifb/Fclk_adc = 2*i with i an integer in [1,64]
  unsigned short  ratio_clkadcifb_clkadc;

  //! ADC Startup time in [0,127], where Tstartup = startup * 8 * Tclk_adc
  unsigned short  startup;

  //! ADCIFB Sleep Mode enable/disable
  bool            sleep_mode_enable;
} adcifb_opt_t;



/*! \brief Initialize the ADCIFB module (enable and configure). Mandatory to call.
 *
 * \param *adcifb       Base address of the ADCIFB module
 * \param p_adcifb_opt  Structure for the ADCIFB module configuration
 *
 * \note To avoid an infinite loop, this function checks the ADCIFB READY status
 *       bit but no longer than ADCIFB_POLL_TIMEOUT times.
 *
 * \return Status.
 *  \retval 0   ADCIFB successfully enabled and configured
 *  \retval <0  ADCIFB initialization failed.
 */
extern long int adcifb_configure( volatile avr32_adcifb_t *adcifb,
                                  const adcifb_opt_t *p_adcifb_opt );

/*! \brief Configure the ADCIFB trigger mode.
 *
 * \param *adcifb Base address of the ADCIFB module
 * \param trgmod  Trigger mode, one of AVR32_ADCIFB_TRGR_TRGMOD_* defines (cf adcifb_xxx.h part header file)
 * \param trgper  defines the Trigger period where Trigger Period = trgper *Tclk_adc (effective only if trigger_mode==AVR32_ADCIFB_TRGMOD_PT)
 *
 * \return Status.
 *  \retval 0   ADCIFB trigger mode successfully configured
 *  \retval <0  ADCIFB trigger mode configuration failed.
 */
long int adcifb_configure_trigger( volatile avr32_adcifb_t *adcifb,
                                   unsigned char trgmod,
                                   unsigned short int trgper );


/*! \brief Enable one or several channels.
 *
 * \param *adcifb         Base address of the ADCIFB module
 * \param  channels_mask  Bitmask of channels to enable
 */
extern void adcifb_channels_enable( volatile avr32_adcifb_t *adcifb,
                                    unsigned long int channels_mask );

/*! \brief Disable one or several channels.
 *
 * \param *adcifb         Base address of the ADCIFB module
 * \param  channels_mask  Bitmask of channels to disable
 */
extern void adcifb_channels_disable(volatile avr32_adcifb_t *adcifb,
                                    unsigned long int channels_mask );


/*! \brief Start an ADC conversion sequence (Software trigger)
 *
 *  \param *adcifb Base address of the ADCIFB module
 */
extern void adcifb_start_conversion_sequence( volatile avr32_adcifb_t *adcifb );


/*! \brief Get Last Converted data
 *
 * \param *adcifb   Base address of the ADCIFB module
 *
 * \return The value acquired (unsigned long)
 * \warning the return value contains the converted data in the bits[0-11]; bits[16-23]
 *          contain the channel id that was last converted.
 */
extern unsigned long adcifb_get_last_data( volatile avr32_adcifb_t *adcifb );


/*! \brief Wait for a status high in the ADCIFB SR status register.
 *
 * \param *adcifb     Base address of the ADCIFB module
 * \param statusMask  Mask field of the status to poll
 *
 * \return Status.
 *   \retval 0   Status is high.
 *   \retval <0  ADCIFB_POLL_TIMEOUT Timeout expired before the status was high.
 */
extern long int adcifb_sr_statushigh_wait(  volatile avr32_adcifb_t *adcifb,
                                            unsigned long statusMask );


/*! \brief Check if the ADCIFB is ready to perform a conversion sequence.
 *
 * \param *adcifb Base address of the ADCIFB module
 *
 * \return bool   true if the ADCIFB is ready to perform a conversion sequence.
 *                false if the ADCIFB is not ready to perform a conversion sequence.
 */
extern bool adcifb_is_ready( volatile avr32_adcifb_t *adcifb );


/*! \brief Check if one or more data has been converted since the start of conversion and is available
 *
 * \param *adcifb Base address of the ADCIFB module
 *
 * \return bool   true if one or more data has been converted since the start of conversion and is available
 *                false if no data has been converted since the start of conversion
 */
extern bool adcifb_is_drdy( volatile avr32_adcifb_t *adcifb );

/*! \brief Check if one or more Overrun Error has occurred since the start of conversion.
 *
 * \param *adcifb Base address of the ADCIFB module
 *
 * \return bool   true if an overrun has occured
 *                false if no overrun occured
 */
extern bool adcifb_is_ovre( volatile avr32_adcifb_t *adcifb );

/**
 * \}
 */

#endif  // _ADCIFB_H_
