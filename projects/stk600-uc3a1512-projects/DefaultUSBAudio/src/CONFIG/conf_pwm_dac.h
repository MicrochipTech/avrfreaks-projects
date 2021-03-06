/* This header file is part of the ATMEL AVR32-SoftwareFramework-AT32UC3A-1.4.0 Release */

/*This file is prepared for Doxygen automatic documentation generation.*/
/*! \file *********************************************************************
 *
 * \brief PWM mono audio DAC driver configuration file for AVR32.
 *
 * This file contains the possible external configuration of the PWM DAC.
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


#ifndef _CONF_PWM_DAC_H_
#define _CONF_PWM_DAC_H_

#include <avr32/io.h>
#include "intc.h"
#include "board.h"


/*! \name PWM Connections
 */
//! @{
#define PWM_DAC_PWM                   (&AVR32_PWM)
#define PWM_DAC_PWM_IRQ               AVR32_PWM_IRQ
#define PWM_DAC_PWM_INT_LEVEL         AVR32_INTC_INT3
#if BOARD==EVK1100
#  define PWM_DAC_PWM_CHANNEL_LSB       1             //!< Least significant 8-bit PWM channel.
#  define PWM_DAC_PWM_CHANNEL_LSB_PIN                 //!< Least significant 8-bit PWM channel pin.
#  define PWM_DAC_PWM_CHANNEL_MSB       0             //!< Most significant 8-bit PWM channel.
#  define PWM_DAC_PWM_CHANNEL_MSB_PIN                 //!< Most significant 8-bit PWM channel pin.
#elif BOARD==EVK1101
#  define PWM_DAC_PWM_CHANNEL_LSB       6             //!< Least significant 8-bit PWM channel.
#  define PWM_DAC_PWM_CHANNEL_LSB_PIN   _0            //!< Least significant 8-bit PWM channel pin.
#  define PWM_DAC_PWM_CHANNEL_MSB       4             //!< Most significant 8-bit PWM channel.
#  define PWM_DAC_PWM_CHANNEL_MSB_PIN   _1            //!< Most significant 8-bit PWM channel pin.
#endif
//! @}


#endif  // _CONF_PWM_DAC_H_
