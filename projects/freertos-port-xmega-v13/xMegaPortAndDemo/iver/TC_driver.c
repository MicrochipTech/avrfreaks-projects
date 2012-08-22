/* This file has been prepared for Doxygen automatic documentation generation.*/
/*! \file *********************************************************************
 *
 * \brief  XMEGA Timer/Counter driver source file.
 *
 *      This file contains the function implementations the XMEGA Timer/Counter
 *      driver.
 *
 *      The driver is not intended for size and/or speed critical code, since
 *      most functions are just a few lines of code, and the function call
 *      overhead would decrease code performance. The driver is intended for
 *      rapid prototyping and documentation purposes for getting started with
 *      the XMEGA Timer/Counter module.
 *
 *      For size and/or speed critical code, it is recommended to copy the
 *      function contents directly into your application instead of making
 *      a function call.
 *
 *      Several functions use the following construct:
 *          "some_register = ... | (some_parameter ? SOME_BIT_bm : 0) | ..."
 *      Although the use of the ternary operator ( if ? then : else ) is discouraged,
 *      in some occasions the operator makes it possible to write pretty clean and
 *      neat code. In this driver, the construct is used to set or not set a
 *      configuration bit based on a boolean input parameter, such as
 *      the "some_parameter" in the example above.
 *
 * \par Application note:
 *      AVR1306: Using the XMEGA Timer/Counter
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

#include "avr_compiler.h"
#include "TC_driver.h"

/*! \brief Configures clock source for the Timer/Counter 0.
 *
 *  This function clears the old clock source setting of the Timer/Counter and
 *  sets a new clock source according to the clockSelection parameter.
 *
 *  \param tc              Timer/Counter module instance.
 *  \param clockSelection  Timer/Counter clock source setting.
 */
void TC0_ConfigClockSource( volatile TC0_t * tc, TC_CLKSEL_t clockSelection )
{
	tc->CTRLA = ( tc->CTRLA & ~TC0_CLKSEL_gm ) | clockSelection;
}

/*! \brief Configures clock source for the Timer/Counter 1.
 *
 *  This function clears the old clock source setting of the Timer/Counter and
 *  sets a new clock source according to the clockSelection parameter.
 *
 *  \param tc              Timer/Counter module instance.
 *  \param clockSelection  Timer/Counter clock source setting.
 */
void TC1_ConfigClockSource( volatile TC1_t * tc, TC_CLKSEL_t clockSelection )
{
	tc->CTRLA = ( tc->CTRLA & ~TC1_CLKSEL_gm ) | clockSelection;
}


/*! \brief Configures the Waveform Generation Mode for the Timer/Counter 0.
 *
 *  This function clears the old WGM setting of the Timer/Counter and sets a
 *  new WGM setting according to the wgm parameter.
 *
 *  \param tc    Timer/Counter module instance.
 *  \param wgm   Waveform generation mode.
 */
void TC0_ConfigWGM( volatile TC0_t * tc, TC_WGMODE_t wgm )
{
	tc->CTRLB = ( tc->CTRLB & ~TC0_WGMODE_gm ) | wgm;
}


/*! \brief Configures the Waveform Generation Mode for the Timer/Counter 1.
 *
 *  This function clears the old WGM setting of the Timer/Counter and sets a
 *  new WGM setting according to the wgm parameter.
 *
 *  \param tc    Timer/Counter module instance.
 *  \param wgm   Waveform generation mode.
 */
void TC1_ConfigWGM( volatile TC1_t * tc, TC_WGMODE_t wgm )
{
	tc->CTRLB = ( tc->CTRLB & ~TC1_WGMODE_gm ) | wgm;
}


/*! \brief Configures the Timer/Counter 0 for input capture operation.
 *
 *  This function sets the Timer/Counter in input capture mode and selects
 *  the event lines that will trigger the individual input capture channels.
 *
 *  \note Output compare operation is disabled when input capture operation is
 *  enabled.
 *
 *  \param tc               Timer/Counter module instance.
 *  \param eventSource      Event source selection.
 */
void TC0_ConfigInputCapture( volatile TC0_t * tc, TC_EVSEL_t eventSource )
{
	tc->CTRLD = ( tc->CTRLD & ~( TC0_EVSEL_gm | TC0_EVACT_gm ) ) |
	            eventSource |
	            TC_EVACT_CAPT_gc;
}


/*! \brief Configures the Timer/Counter 1 for input capture operation.
 *
 *  This function sets the Timer/Counter in input capture mode and selects
 *  the event lines that will trigger the individual input capture channels.
 *
 *  \note Output compare operation is disabled when input capture operation is
 *  enabled.
 *
 *  \param tc               Timer/Counter module instance.
 *  \param eventSource      Event source selection.
 */
void TC1_ConfigInputCapture( volatile TC1_t * tc, TC_EVSEL_t eventSource )
{
	tc->CTRLD = ( tc->CTRLD & ~( TC1_EVSEL_gm | TC1_EVACT_gm ) ) |
	            eventSource |
	            TC_EVACT_CAPT_gc;
}


/*! \brief Enables compare/capture channels for Timer/Counter 0.
 *
 *  This function enables compare/capture channels according to the supplied
 *  enableMask. The enableMask can be obtained by ORing together the symbols
 *    - TC0_CCAEN_bm
 *    - TC0_CCBEN_bm
 *    - TC0_CCCEN_bm
 *    - TC0_CCDEN_bm
 *
 *  Example: TC0_EnableCCChannels( &TCC0, TC0_CCAEN_bm | TC0_CCCEN_bm ) will
 *  enable channels A and C.
 *
 *  \note No capture/compare channels are disabled by calling this function.
 *  Disabling is done by calling \ref TC0_DisableCCChannels.
 *
 *  \param tc               Timer/Counter module instance.
 *  \param enableMask       Mask of channels to enable.
 */
void TC0_EnableCCChannels( volatile TC0_t * tc, uint8_t enableMask )
{
	/* Make sure only CCxEN bits are set in enableMask. */
	enableMask &= ( TC0_CCAEN_bm | TC0_CCBEN_bm | TC0_CCCEN_bm | TC0_CCDEN_bm );

	/* Enable channels. */
	tc->CTRLB |= enableMask;
}

/*! \brief Enables compare/capture channels for Timer/Counter 1.
 *
 *  This function enables compare/capture channels according to the supplied
 *  enableMask. The enableMask can be obtained by ORing together the symbols
 *    - TC1_CCAEN_bm
 *    - TC1_CCBEN_bm
 *
 *  Example: TC1_EnableCCChannels( &TCC1, TC1_CCAEN_bm | TC1_CCBEN_bm ) will
 *  enable channels A and B.
 *
 *  \note No capture/compare channels are disabled by calling this function.
 *  Disabling is done by calling \ref TC1_DisableCCChannels.
 *
 *  \param tc               Timer/Counter module instance.
 *  \param enableMask       Mask of channels to enable.
 */
void TC1_EnableCCChannels( volatile TC1_t * tc, uint8_t enableMask )
{
	/* Make sure only CCxEN bits are set in enableMask. */
	enableMask &= ( TC1_CCAEN_bm | TC1_CCBEN_bm );

	/* Enable channels. */
	tc->CTRLB |= enableMask;
}


/*! \brief Disables compare/capture channels on Timer/Counter 0.
 *
 *  This function disables compare/capture channels according to the supplied
 *  disableMask. The disableMask can be obtained by ORing together the symbols
 *    - TC0_CCAEN_bm
 *    - TC0_CCBEN_bm
 *    - TC0_CCCEN_bm
 *    - TC0_CCDEN_bm
 *
 *  Example: TC0_DisableCCChannels( &TCC0, TC0_CCAEN_bm | TC0_CCCEN_bm ) will
 *  disable channels A and C.
 *
 *  \param tc               Timer/Counter module instance.
 *  \param disableMask      Mask of channels to disable.
 */
void TC0_DisableCCChannels( volatile TC0_t * tc, uint8_t disableMask )
{
	/* Make sure only CCxEN bits are set in disableMask. */
	disableMask &= ( TC0_CCAEN_bm | TC0_CCBEN_bm | TC0_CCCEN_bm | TC0_CCDEN_bm );

	/* Disable channels. */
	tc->CTRLB &= ~disableMask;
}


/*! \brief Disables compare/capture channels on Timer/Counter 1.
 *
 *  This function disables compare/capture channels according to the supplied
 *  disableMask. The disableMask can be obtained by ORing together the symbols
 *    - TC1_CCAEN_bm
 *    - TC1_CCBEN_bm
 *
 *  Example: TC1_DisableCCChannels( &TCC1, TC1_CCAEN_bm | TC1_CCBEN_bm ) will
 *  disable channels A and B.
 *
 *  \param tc               Timer/Counter module instance.
 *  \param disableMask      Mask of channels to disable.
 */
void TC1_DisableCCChannels( volatile TC1_t * tc, uint8_t disableMask )
{
	/* Make sure only CCxEN bits are set in disableMask. */
	disableMask &= ( TC1_CCAEN_bm | TC1_CCBEN_bm );

	/* Disable channels. */
	tc->CTRLB &= ~disableMask;
}

/*! \brief Sets the overflow interrupt level.
 *
 *  This function sets the overflow interrupt level of this Timer/Counter 0.
 *
 *  \param tc               Timer/Counter module instance.
 *  \param intLevel         New overflow interrupt level.
 */
void TC0_SetOverflowIntLevel( volatile TC0_t * tc, TC_OVFINTLVL_t intLevel )
{
	tc->INTCTRLA = ( tc->INTCTRLA & ~TC0_OVFINTLVL_gm ) | intLevel;
}


/*! \brief Sets the overflow interrupt level.
 *
 *  This function sets the overflow interrupt level of this Timer/Counter 1.
 *
 *  \param tc               Timer/Counter module instance.
 *  \param intLevel         New overflow interrupt level.
 */
void TC1_SetOverflowIntLevel( volatile TC1_t * tc, TC_OVFINTLVL_t intLevel )
{
	tc->INTCTRLA = ( tc->INTCTRLA & ~TC1_OVFINTLVL_gm ) | intLevel;
}


/*! \brief Sets the Error interrupt level.
 *
 *  This function sets the overflow interrupt level of this Timer/Counter 0.
 *
 *  \param tc               Timer/Counter module instance.
 *  \param intLevel         New error interrupt level.
 */
void TC0_SetErrorIntLevel( volatile TC0_t * tc, TC_ERRINTLVL_t intLevel )
{
	tc->INTCTRLA = ( tc->INTCTRLA & ~TC0_ERRINTLVL_gm ) | intLevel;
}


/*! \brief Sets the Error interrupt level.
 *
 *  This function sets the overflow interrupt level of this Timer/Counter 1.
 *
 *  \param tc               Timer/Counter module instance.
 *  \param intLevel         New error interrupt level.
 */
void TC1_SetErrorIntLevel( volatile TC1_t * tc, TC_ERRINTLVL_t intLevel )
{
	tc->INTCTRLA = ( tc->INTCTRLA & ~TC1_ERRINTLVL_gm ) | intLevel;
}


/*! \brief Sets the interrupt level for compare/capture channel A interrupt.
 *
 *  This function sets the interrupt level for compare/capture channel A
 *  interrupt in Timer/Counter 0.
 *
 *  \param tc               Timer/Counter module instance.
 *  \param intLevel         New compare/capture channel A interrupt level.
 */
void TC0_SetCCAIntLevel( volatile TC0_t * tc, TC_CCAINTLVL_t intLevel )
{
	tc->INTCTRLB = ( tc->INTCTRLB & ~TC0_CCAINTLVL_gm ) | intLevel;
}


/*! \brief Sets the interrupt level for compare/capture channel A interrupt.
 *
 *  This function sets the interrupt level for compare/capture channel A
 *  interrupt in Timer/Counter 1.
 *
 *  \param tc               Timer/Counter module instance.
 *  \param intLevel         New compare/capture channel A interrupt level.
 */
void TC1_SetCCAIntLevel( volatile TC1_t * tc, TC_CCAINTLVL_t intLevel )
{
	tc->INTCTRLB = ( tc->INTCTRLB & ~TC1_CCAINTLVL_gm ) | intLevel;
}


/*! \brief Sets the interrupt level for compare/capture channel B interrupt.
 *
 *  This function sets the interrupt level for compare/capture channel B
 *  interrupt in Timer/Counter 0.
 *
 *  \param tc               Timer/Counter module instance.
 *  \param intLevel         New compare/capture channel B interrupt level.
 */
void TC0_SetCCBIntLevel( volatile TC0_t * tc, TC_CCBINTLVL_t intLevel )
{
	tc->INTCTRLB = ( tc->INTCTRLB & ~TC0_CCBINTLVL_gm ) | intLevel;
}


/*! \brief Sets the interrupt level for compare/capture channel B interrupt.
 *
 *  This function sets the interrupt level for compare/capture channel B
 *  interrupt in Timer/Counter 1.
 *
 *  \param tc               Timer/Counter module instance.
 *  \param intLevel         New compare/capture channel B interrupt level.
 */
void TC1_SetCCBIntLevel( volatile TC1_t * tc, TC_CCBINTLVL_t intLevel )
{
	tc->INTCTRLB = ( tc->INTCTRLB & ~TC1_CCBINTLVL_gm ) | intLevel;
}


/*! \brief Sets the interrupt level for compare/capture channel C interrupt.
 *
 *  This function sets the interrupt level for compare/capture channel C
 *  interrupt in Timer/Counter 0.
 *
 *  \param tc               Timer/Counter module instance.
 *  \param intLevel         New compare/capture channel A interrupt level.
 */
void TC0_SetCCCIntLevel( volatile TC0_t * tc, TC_CCCINTLVL_t intLevel )
{
	tc->INTCTRLB = ( tc->INTCTRLB & ~TC0_CCCINTLVL_gm ) | intLevel;
}


/*! \brief Sets the interrupt level for compare/capture channel D interrupt.
 *
 *  This function sets the interrupt level for compare/capture channel D
 *  interrupt in Timer/Counter 0.
 *
 *  \param tc               Timer/Counter module instance.
 *  \param intLevel         New compare/capture channel A interrupt level.
 */
void TC0_SetCCDIntLevel( volatile TC0_t * tc, TC_CCDINTLVL_t intLevel )
{
	tc->INTCTRLB = ( tc->INTCTRLB & ~TC0_CCDINTLVL_gm ) | intLevel;
}


/*! \brief Resets the Timer/Counter 0.
 *
 *  This function will reset the Timer/Counter. After calling this function,
 *  the Timer/Counter will be in the same state as it would after a full
 *  reset of the device.
 *
 *  \param tc  Timer/Counter 0 module instance.
 */
void TC0_Reset( volatile TC0_t * tc )
{
	/* TC must be turned off before a Reset command. */
	tc->CTRLA = ( tc->CTRLA & ~TC0_CLKSEL_gm ) | TC_CLKSEL_OFF_gc;

	/* Issue Reset command. */
	tc->CTRLFSET = TC_CMD_RESET_gc;
}


/*! \brief Resets the Timer/Counter 1.
 *
 *  This function will reset the Timer/Counter. After calling this function,
 *  the Timer/Counter will be in the same state as it would after a full
 *  reset of the device.
 *
 *  \param tc  Timer/Counter 1 module instance.
 */
void TC1_Reset( volatile TC1_t * tc )
{
	/* TC must be turned off before a Reset command. */
	tc->CTRLA = ( tc->CTRLA & ~TC1_CLKSEL_gm ) | TC_CLKSEL_OFF_gc;

	/* Issue Reset command. */
	tc->CTRLFSET = TC_CMD_RESET_gc;
}
