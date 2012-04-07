/* This file has been prepared for Doxygen automatic documentation generation.*/
/*! \file *********************************************************************
 *
 * \brief  XMEGA DAC driver header file.
 *
 *      This file contains the function prototypes and enumerator definitions
 *      for various configuration parameters for the XMEGA DAC driver.
 *
 *      The driver is not intended for size and/or speed critical code, since
 *      most functions are just a few lines of code, and the function call
 *      overhead would decrease code performance. The driver is intended for
 *      rapid prototyping and documentation purposes for getting started with
 *      the XMEGA DAC module.
 *
 *      For size and/or speed critical code, it is recommended to copy the
 *      function contents directly into your application instead of making
 *      a function call.
 *
 * \par Application note:
 *      AVR1301: Using the XMEGA DAC
 *
 * \par Documentation
 *      For comprehensive code documentation, supported compilers, compiler
 *      settings and supported devices see readme.html
 *
 * \author
 *      Atmel Corporation: http://www.atmel.com \n
 *      Support email: avr@atmel.com
 *
 * $Revision: 1569 $
 * $Date: 2008-04-22 13:03:43 +0200 (ti, 22 apr 2008) $  \n
 *
 * Copyright (c) 2008, Atmel Corporation All rights reserved.
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
 * THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE EXPRESSLY AND
 * SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *****************************************************************************/
#ifndef DAC_DRIVER_H
#define DAC_DRIVER_H

#include "avr_compiler.h"

/*! \brief Enumerator for DAC channels. */
typedef enum DAC_CH_enum {
	CH0 = 0x00,
	CH1 = 0x01,
} DAC_CH_t;


/*! \brief Enumerator for DAC event trigger. */
typedef enum DAC_TRIG_enum {
	DAC_TRIG_0_0 = 0x00,                                /*!< Trigger on no channel. */
	DAC_TRIG_0_1 = DAC_CH0TRIG_bm,                      /*!< Trigger on channel 0. */
	DAC_TRIG_1_0 = DAC_CH1TRIG_bm,                      /*!< Trigger on channel 1. */
	DAC_TRIG_1_1 = ( DAC_CH1TRIG_bm | DAC_CH0TRIG_bm ), /*!< Trigger on both channels. */
} DAC_TRIG_t;


/* Definition of macros. */

/*! \brief This macro disables the selected DAC module.
 *
 *  \param  _dac  The DAC module, either DACA or DACB.
 */
#define DAC_Disable( _dac )                ( (_dac)->CTRLA &= ~DAC_ENABLE_bm )


/*! \brief Enable output to internal ADC.
 *
 *  This macro enables routing of the internal DAC output to
 *  the ADC module. This feature is useful for diagnostics and
 *  calibration purposes.
 *
 *  \param  _dac  The DAC module, either DACA or DACB.
 */
#define DAC_InternalOutput_Enable( _dac )  ( (_dac)->CTRLA |= DAC_IDOEN_bm )


/*! \brief This macro disables internal routing of the DAC output to an
 *         internal ADC module.
 *
 *  \param  _dac  The DAC module, either DACA or DACB.
 */
#define DAC_InternalOutput_Disable( _dac ) ( (_dac)->CTRLA &= ~DAC_IDOEN_bm )


/* Prototyping of functions. Documentation is found in source file. */

void DAC_SingleChannel_Enable( volatile DAC_t * dac,
                               DAC_REFSEL_t convRef,
                               bool leftAdjust );
void DAC_DualChannel_Enable( volatile DAC_t * dac,
                             DAC_REFSEL_t convRef,
                             bool leftAdjust,
                             DAC_CONINTVAL_t sampleInterval,
                             DAC_REFRESH_t refreshInterval );
void DAC_Channel_Write( volatile DAC_t * dac, uint16_t data, DAC_CH_t channel );
bool DAC_Channel_DataEmpty( volatile DAC_t * dac, DAC_CH_t channel );
void DAC_EventAction_Set( volatile DAC_t * dac,
                          DAC_TRIG_t trigChannel,
                          uint8_t eventLine );

#endif

