/* This file has been prepared for Doxygen automatic documentation generation.*/
/*! \file *********************************************************************
 *
 * \brief  XMEGA DAC driver source file.
 *
 *      This file contains the function implementations for the XMEGA DAC driver.
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
 *      Several functions use the following construct:
 *          "some_register = ... | (some_parameter ? SOME_BIT_bm : 0) | ..."
 *      Although the use of the ternary operator ( if ? then : else ) is
 *      discouraged, in some occasions the operator makes it possible to
 *      write pretty clean and neat code. In this driver, the construct is
 *      used to set or not set a configuration bit based on a boolean input
 *      parameter, such as the "some_parameter" in the example above.
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
 * $Revision: 1646 $
 * $Date: 2008-05-14 16:04:55 +0200 (on, 14 mai 2008) $  \n
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
#include "dac_driver.h"


/*! \brief Enable Singe Channel.
 *
 *  This function configures the DAC to enable channel 0 output only,
 *  in single channel operation. It apply configuration parameters, save old
 *  values, clear affected bit field and set appropriate values.
 *
 *  \param  dac         Pointer to DAC module register section.
 *  \param  convRef     Selects reference voltage for all conversions.
 *  \param  leftAdjust  Set to true to make data registers left adjusted.
 */
void DAC_SingleChannel_Enable( volatile DAC_t * dac,
                              DAC_REFSEL_t convRef,
                              bool leftAdjust )
{
	dac->CTRLB = ( dac->CTRLB & ~DAC_CHSEL_gm ) | DAC_CHSEL_SINGLE_gc;
	dac->CTRLC = ( dac->CTRLC & ~(DAC_REFSEL_gm | DAC_LEFTADJ_bm) ) |
	             convRef | ( leftAdjust ? DAC_LEFTADJ_bm : 0x00 );
	dac->CTRLA = ( dac->CTRLA & ~DAC_CH1EN_bm ) |
	             DAC_CH0EN_bm | DAC_ENABLE_bm;
}


/*! \brief Enable Dual Channel.
 *
 *  This function configures the DAC to enable both channel outputs,
 *  in dual channel operation. It apply configuration parameters, save old
 *  values, clear affected bit field and set appropriate values.
 *
 *  \param  dac              Pointer to DAC module register section.
 *  \param  convRef          Selects reference voltage for all conversions.
 *  \param  leftAdjust       Set to true to make data registers left adjusted.
 *  \param  sampleInterval   Interval between refreshing channel A and B.
 *  \param  refreshInterval  Interval between refresh cycles.
 */
void DAC_DualChannel_Enable( volatile DAC_t * dac,
                            DAC_REFSEL_t convRef,
                            bool leftAdjust,
                            DAC_CONINTVAL_t sampleInterval,
                            DAC_REFRESH_t refreshInterval )
{
	dac->CTRLB = ( dac->CTRLB & ~DAC_CHSEL_gm ) | DAC_CHSEL_DUAL_gc;
	dac->CTRLC = ( dac->CTRLC & ~( DAC_REFSEL_gm | DAC_LEFTADJ_bm ) ) |
	             convRef |
	             ( leftAdjust ? DAC_LEFTADJ_bm : 0x00 );
	dac->TIMCTRL = (uint8_t) sampleInterval | refreshInterval;
	dac->CTRLA |= DAC_CH1EN_bm | DAC_CH0EN_bm | DAC_ENABLE_bm;
}


/*! \brief  Write data to selected channel.
 *
 *  This function writes data to the selected channel data register. The program
 *  should wait for the data register to be ready if necessary. This ensures
 *  that no intermediate values are lost with very high update rates.
 *
 *  \note  The data must be in the format currently configured for the DAC,
 *         right or left adjusted.
 *
 *  \param  dac     Pointer to DAC module register section.
 *  \param  data    Data to be converted.
 *  \param  channel Selected channel in the DAC module, either CH0 or CH1.
 */
void DAC_Channel_Write( volatile DAC_t * dac, uint16_t data, DAC_CH_t channel )
{
	if ( channel == CH0 ) {
		dac->CH0DATA = data;
	} else {
		dac->CH1DATA = data;
	}
}


/*! \brief Check if channel data register is empty.
 *
 *  This function return the status of the data register empty flag for
 *  the selected channel in the given DAC module. This can be used to get the
 *  status of the register before writing a new value to it.
 *
 *  \param  dac     Pointer to DAC module register section.
 *  \param  channel Selected channel in the DAC module, either CH0 or CH1.
 *
 *  \retval dacStatus True if data register is empty.
 *  \retval dacStatus False if data register is not empty.
 */
bool DAC_Channel_DataEmpty( volatile DAC_t * dac, DAC_CH_t channel )
{
	bool dacStatus = ( dac->STATUS &
	                 ( channel ? DAC_CH1DRE_bm : DAC_CH0DRE_bm ));
	return dacStatus;
}


/*! \brief Configure event actions.
 *
 *  This function configures the event action for the DAC module. It clears
 *  both control bits and set required bits.
 *
 *  \note  There is no checking if the event line number is valid. The value
 *         is simply truncated to fit the bit field in the corresponding register.
 *
 *  \param  dac         Pointer to DAC module register section.
 *  \param  trigChannel The channels to be triggered by events. Values can be
 *                      DAC_TRIG_0_0, DAC_TRIG_0_1, DAC_TRIG_1_0 or DAC_TRIG_0_0.
 *  \param  eventLine   Event line (0..7) to use for triggering conversions.
 */
void DAC_EventAction_Set( volatile DAC_t * dac,
                          DAC_TRIG_t trigChannel,
                          uint8_t eventLine )
{
	dac->CTRLB = ( dac->CTRLB & ~DAC_TRIG_1_1 ) | trigChannel;
	dac->EVCTRL = eventLine & DAC_EVSEL_gm;
}
