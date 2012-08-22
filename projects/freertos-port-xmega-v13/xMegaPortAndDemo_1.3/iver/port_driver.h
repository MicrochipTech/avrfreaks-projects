/* This file has been prepared for Doxygen automatic documentation generation.*/
/*! \file *********************************************************************
 *
 * \brief
 *      XMEGA I/O Port driver header file.
 *
 *      This file contains the function prototypes and enumerator definitions
 *      for various configuration parameters for the XMEGA I/O Port driver.
 *
 *		This driver is intended for rapid prototyping and documentation
 *      purposes for getting started with the XMEGA I/O Ports. The driver is not
 *      optimized for speed or size. However, functions that are just a few
 *      lines of code are written as macros to eliminate function call overhead.
 *      When using these macros it is important to use the correct data type for
 *      the arguments, as this is not checked by the compiler.
 *
 * \par Application note:
 *      AVR1313: Using the XMEGA I/O Pins and External Interrupts
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

#ifndef PORT_DRIVER_H
#define PORT_DRIVER_H

#include "avr_compiler.h"

/* Definitions of macros. */

/*! \brief This macro sets the data direction for all I/O pins in the port or
 *         the virtual port.
 *
 *  /note This macro can also be used to access virtual ports.
 *
 *  \param _port           Pointer to the PORT_t or VPORT_t instance.
 *  \param _directionMask  The direction setting mask. The direction for pin n in
 *                         the port is controlled by bit n. A 0 means input. A 1
 *                         means output.
 */
#define PORT_SetDirection( _port, _directionMask) ( (_port)->DIR = _directionMask )



/*! \brief Sets the data direction of a set of pins to output
 *
 *  This macro sets the data direction of the selected port pins to output
 *  without altering the data direction of the other pins in that port.
 *
 *  \param _port         Pointer to the PORT_t instance.
 *  \param _outputMask   A bit mask of the pins to set as output. A one in bit
 *                       location n will configure pin n as output.
 */
#define PORT_SetPinsAsOutput( _port, _outputMask ) ( (_port)->DIRSET = _outputMask )



/*! \brief Sets the data direction of a set of pins to input
 *
 *  This macro sets the data direction of the selected port pins to input
 *  without altering the data direction of the other pins in that port.
 *
 *  \param _port         Pointer to the PORT_t instance.
 *  \param _inputMask    A bit mask of the pins to set as input. A one in bit
 *                       location n will configure pin n as input.
 */
#define PORT_SetPinsAsInput( _port, _inputMask) ( (_port)->DIRCLR = _inputMask )



/*! \brief Toggle the data direction of a set of pins
 *
 *  This macro toggles the data direction of the selected port pins
 *  without altering the data direction of the other pins in that port.
 *
 *  \param _port         Pointer to the PORT_t instance.
 *  \param _toggleMask   A bit mask of the pins on which to toggle input direction.
 */
#define PORT_ToggleDirection( _port, _toggleMask) ( (_port)->DIRTGL = _toggleMask )



/*! \brief This macro sets the output value/logic level of all pins in the
 *         I/O port or virtual port.
 *
 *  /note This macro can also be used to access virtual ports.
 *
 *  \param _port         Pointer to the PORT_t or VPORT_t instance.
 *  \param _outValue     The output value. Bit n in this parameter corresponds to
 *                       pin n of the port.
 */
#define PORT_SetOutputValue( _port, _outValue) ( (_port)->OUT = _outValue )



/*! \brief Set the output value of a set of I/O pins to logic high.
 *
 *  This macro sets the output value of a set of I/O pins to logic high.
 *  (Unless inverted I/O has been enabled for the pins) It does not alter the
 *  pins not specified in the bit mask.
 *
 *  \param _port         Pointer to the PORT_t instance.
 *  \param _setMask      The bit mask of pins to set to logic high level.
 */
#define PORT_SetPins( _port, _setMask) ( (_port)->OUTSET = _setMask )



/*! \brief Set the output value of a set of I/O pins to logic low.
 *
 *  This macro sets the output value of a set of I/O pins to logic low.
 *  (Unless inverted I/O has been enabled for the pins) It does not alter the
 *  pins not specified in the bit mask.
 *
 *  \param _port         Pointer to the PORT_t instance.
 *  \param _clearMask    The bit mask of pins to set to logic low level.
 */
#define PORT_ClearPins( _port, _clearMask) ( (_port)->OUTCLR = _clearMask )



/*! \brief Toggle the output value of a set of I/O pins.
 *
 *  This macro toggles the output value of a set of I/O pins. It does not
 *  alter the output value of pins not specified in the bit mask.
 *
 *  \param _port         Pointer to the PORT_t instance.
 *  \param _toggleMask   The bit mask of pins to toggle.
 */
#define PORT_TogglePins( _port, _toggleMask ) ( (_port)->OUTTGL = _toggleMask )



/*! \brief This macro returns the current logic value of the port or virtual
 *         port.
 *
 *  /note This macro can also be used to access virtual ports.
 *
 *  \param _port     Pointer to the PORT_t or VPORT_t instance.
 *  \return          The current logic state of the port.
 */
#define PORT_GetPortValue( _port ) ( (_port)->IN )



/*! \brief This macro returns the current state of the interrupt flags, given by
 *         a bit mask, for an I/O port or a virtual port.
 *
 *  /note This macro can also be used to access virtual ports.
 *
 *  \param _port       Pointer to the PORT_t or VPORT_t instance.
 *  \param _bitMask    Bit mask for interrupt flags to read.
 *  \return            The state of the interrupt flags.
 */
#define PORT_GetInterruptFlags( _port, _bitMask ) ( (_port)->INTFLAGS & _bitMask )



/*! \brief This macro clears interrupt flags, given by a bit mask, for an I/O
 *         port or a virtual port.
 *
 *  /note This macro can be used for both I/O ports and virtual ports.
 *
 *  \param _port     Pointer to the PORT_t or VPORT_t instance.
 *  \param _bitMask  Bit mask for interrupt flags to clear.
 */
#define PORT_ClearInterruptFlags( _port, _bitMask ) ( (_port)->INTFLAGS = _bitMask )



/*! \brief This macro sets a bit in the OUT register of an I/O port or virtual
 *         port.
 *
 *  \note This macro is especially efficient with virtual ports, since only
 *        one SBI instruction is invoked.
 *
 *  \param _port         Pointer to the PORT_t or VPORT_t instance.
 *  \param _bitPosition  Position of bit to set.
 */
#define PORT_SetOutputBit( _port, _bitPosition ) ( (_port)->OUT = (_port)->OUT | (1 << _bitPosition) )



/*! \brief This macro clears a bit in the OUT register of an I/O port or virtual
 *         port.
 *
 *  \note This macro is especially efficient with virtual ports, since only
 *        one CBI instruction is invoked.
 *
 *  \param _port         Pointer to the PORT_t or VPORT_t instance.
 *  \param _bitPosition  Position of bit to cleared.
 */
#define PORT_ClearOutputBit( _port, _bitPosition ) ( (_port)->OUT = (_port)->OUT & ~(1 << _bitPosition) )



/*! \brief This macro configures a pin in an I/O port or virtual port as an
 *         output.
 *
 *  \note This macro is especially efficient with virtual ports, since only
 *        one SBI instruction is invoked.
 *
 *  \param _port         Pointer to the PORT_t or VPORT_t instance.
 *  \param _bitPosition  Position of bit to set as output.
 */
#define PORT_SetPinAsOutput( _port, _bitPosition ) ( (_port)->DIR = (_port)->DIR | (1 << _bitPosition) )



/*! \brief This macro configures a pin in an I/O port or virtual port as an
 *         input.
 *
 *  \note This macro is especially efficient with virtual ports, since only
 *        one CBI instruction is invoked.
 *
 *  \param _port         Pointer to the PORT_t or VPORT_t instance.
 *  \param _bitPosition  Position of bit to set as input.
 */
#define PORT_SetPinAsInput( _port, _bitPosition ) ( (_port)->DIR = (_port)->DIR & ~(1 << _bitPosition) )



/* Prototyping of functions. Documentation is found in source file. */

void PORT_ConfigureInterrupt0( PORT_t * port,
                               PORT_INT0LVL_t intLevel,
                               uint8_t pinMask);

void PORT_ConfigureInterrupt1( PORT_t * port,
                               PORT_INT1LVL_t intLevel,
                               uint8_t pinMask);

void PORT_ConfigurePins( PORT_t * port,
                         uint8_t pinMask,
                         uint8_t slewRateEnable,
                         uint8_t invertEnable,
                         PORT_OPC_t opc,
                         PORT_ISC_t isc);

void PORT_MapVirtualPort0(PORTCFG_VP0MAP_t realPort);
void PORT_MapVirtualPort1(PORTCFG_VP1MAP_t realPort);
void PORT_MapVirtualPort2(PORTCFG_VP2MAP_t realPort);
void PORT_MapVirtualPort3(PORTCFG_VP3MAP_t realPort);


#endif
