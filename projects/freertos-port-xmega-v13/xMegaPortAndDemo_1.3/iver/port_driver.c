/* This file has been prepared for Doxygen automatic documentation generation.*/
/*! \file *********************************************************************
 *
 * \brief
 *      XMEGA I/O Port driver source file.
 *
 *      This file contains the function implementations the XMEGA I/O Port driver.
 *
 *      The driver is not intended for size and/or speed critical code, since
 *      most functions are just a few lines of code, and the function call
 *      overhead would decrease code performance. The driver is intended for
 *      rapid prototyping and documentation purposes for getting started with
 *      the XMEGA I/O Port module.
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
#include "port_driver.h"



/*! \brief Configures interrupt 0.
 *
 *  This function configures interrupt 0 to be associated with a set of pins and
 *  sets the desired interrupt level.
 *
 *  \param port       The port to configure.
 *  \param intLevel   The desired interrupt level for port interrupt 0.
 *  \param pinMask    A mask that selects the pins to associate with port interrupt 0.
 */
void PORT_ConfigureInterrupt0( PORT_t * port,
                               PORT_INT0LVL_t intLevel,
                               uint8_t pinMask )
{
	port->INTCTRL = ( port->INTCTRL & ~PORT_INT0LVL_gm ) | intLevel;
	port->INT0MASK = pinMask;
}


/*! \brief Configures interrupt 1.
 *
 *  This function configures interrupt 1 to be associated with a set of pins and
 *  sets the desired interrupt level.
 *
 *  \param port       The port to configure.
 *  \param intLevel   The desired interrupt level for port interrupt 1.
 *  \param pinMask    A mask that selects the pins to associate with port interrupt 1.
 */
void PORT_ConfigureInterrupt1( PORT_t * port,
                               PORT_INT1LVL_t intLevel,
                               uint8_t pinMask )
{
	port->INTCTRL = ( port->INTCTRL & ~PORT_INT1LVL_gm ) | intLevel;
	port->INT1MASK = pinMask;
}



/*! \brief This function changes the configuration of a set of pins.
 *
 *  \param port             The port.
 *  \param pinMask          A bit mask that selects the pins to configure.
 *  \param slewRateEnable   Enable/disable slew rate control.
 *  \param invertEnable     Enable/disable I/O inversion.
 *  \param opc              Output/Pull Configuration.
 *  \param isc              Input/Sense Configuration.
 */
void PORT_ConfigurePins( PORT_t * port,
                         uint8_t pinMask,
                         uint8_t slewRateEnable,
                         uint8_t invertEnable,
                         PORT_OPC_t opc,
                         PORT_ISC_t isc)
{
	/* Build pin control register value. */
	uint8_t temp = (uint8_t) opc |
	               isc |
	               (slewRateEnable ? PORT_SRLEN_bm : 0) |
	               (invertEnable ? PORT_INVEN_bm : 0);

	/* Configure the pins in one atomic operation. */

	/* Save status register. */
	uint8_t sreg = SREG;

	cli();
	PORTCFG.MPCMASK = pinMask;
	port->PIN0CTRL = temp;

	/* Restore status register. */
	SREG = sreg;
}


/*! \brief Maps a real port to virtual port 0.
 *
 *  This function maps a real port to virtual port 0 to make some of the
 *  registers available in I/O space.
 *
 *  \param realPort Selects the real port to map to virtual port 0.
 */
void PORT_MapVirtualPort0(PORTCFG_VP0MAP_t realPort)
{
	PORTCFG.VPCTRLA = ( PORTCFG.VPCTRLA & ~PORTCFG_VP0MAP_gm ) | realPort;
}


/*! \brief Maps a real port to virtual port 1.
 *
 *  This function maps a real port to virtual port 1 to make some of the
 *  registers available in I/O space.
 *
 *  \param realPort Selects the real port to map to virtual port 1.
 */
void PORT_MapVirtualPort1(PORTCFG_VP1MAP_t realPort)
{
	PORTCFG.VPCTRLA = ( PORTCFG.VPCTRLA & ~PORTCFG_VP1MAP_gm ) | realPort;
}


/*! \brief Maps a real port to virtual port 2.
 *
 *  This function maps a real port to virtual port 2 to make some of the
 *  registers available in I/O space.
 *
 *  \param realPort Selects the real port to map to virtual port 2.
 */
void PORT_MapVirtualPort2(PORTCFG_VP2MAP_t realPort)
{
	PORTCFG.VPCTRLB = ( PORTCFG.VPCTRLB & ~PORTCFG_VP2MAP_gm ) | realPort;
}


/*! \brief Maps a real port to virtual port 3.
 *
 *  This function maps a real port to virtual port 3 to make some of the
 *  registers available in I/O space.
 *
 *  \param realPort Selects the real port to map to virtual port 3.
 */
void PORT_MapVirtualPort3(PORTCFG_VP3MAP_t realPort)
{
	PORTCFG.VPCTRLB = ( PORTCFG.VPCTRLB & ~PORTCFG_VP3MAP_gm ) | realPort;
}
