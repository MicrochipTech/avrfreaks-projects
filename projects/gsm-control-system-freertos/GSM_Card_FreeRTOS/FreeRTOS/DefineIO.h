#ifndef DEFINEIO_H
#define DEFINEIO_H

/*
   DefineIO.h

   GSM_ControlSystem "GSA" V1.0.0  Copyright (C) 2012 Easy System


    *****************************************************************************
    *    >>> See http://www.esyste.com for Hw Documentation <<<					*
    *                                                                           *
    *    Thank you for using GSA, and thank you for your support!               *
    *                                                                           *
    *****************************************************************************


    This file is part of the GSM_ControlSystem "GSA" distribution.

    GSA is free software; you can redistribute it and/or modify it under
    the terms of the GNU General Public License (version 2) as published by the
    Free Software Foundation AND MODIFIED BY the GSA exception.
    >>>NOTE<<< The modification to the GPL is included to allow you to
    distribute a combined work that includes GSA without being obliged to
    provide the source code for proprietary components outside of the GSA
    kernel.  GSA is distributed in the hope that it will be useful, but
    WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
    or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
    more details.
    You should have received a copy of the GNU General Public
    License and the GSA license exception along with GSA.

    GNU General Public License Exception:
    Any GSA source code, whether modified or in its original release form, or whether in whole or in part,
    can only be distributed by you under the terms of the GNU General Public License plus this exception.
    An independent module is a module which is not derived from or based on GSA.

    EXCEPTION TEXT:
    Clause 1
    Linking GSA statically or dynamically with other modules is making a combined work based on GSA. Thus,
    the terms and conditions of the GNU General Public License cover the whole combination.
    As a special exception, the copyright holder of GSA gives you permission to link GSA with independent modules that communicate with GSA solely
    through the GSA or FreeRTOS API interface, regardless of the license terms of these independent modules, and to copy and distribute the resulting
    combined work under terms of your choice, provided that:

    1)   Every copy of the combined work is accompanied by a written statement that details to the recipient the version of GSA used and an offer by yourself
          to provide the GSA source code (including any modifications you may have made) should the recipient request it.
    2)  The combined work is not itself an RTOS, scheduler, kernel or related product.
    3)  The independent modules add significant and primary functionality to GSA and do not merely extend the existing functionality already present in GSA.

    Clause 2
    GSA may not be used for any competitive or comparative purpose,
    including the publication of any form of run time or compile time metric,
    without the express permission of Easy System (this is the norm within the industry and is intended to ensure information accuracy).

    All FreeRTOS files follow their own licence.

    1 tab == 4 spaces!

*/

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Define pin list for ATmega1284P
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//Analog input
#define I_Analog_1    PINA0		// Differential with PINA1
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#define I_Analog_2    PINA2		// Differential with PINA3

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// External input
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#define In_Ext_1	PINA4
#define In_Ext_2	PINA5
#define In_Ext_3	PINA6
#define In_Ext_4	PINA7

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Pin output
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#define O_Rele_1	PORTB0
#define O_Rele_2	PORTB1
#define O_Rele_3	PORTB2
#define O_Rele_4	PORTB3
#define O_Rele_5	PORTB4
#define O_Rele_6	PORTB5
#define O_Rele_7	PORTB6
#define O_Rele_8	PORTB7

#define O_GsmOnOff	PORTC1
//PC2 - PC5 reserved for JTAG
#define O_Led		PORTC6
#define O_GsmReset	PORTC7

//PD0 - PD1 reserved for UART
#define I_GsmCTS			PIND2
#define O_GsmRTS			PORTD3
#define I_VCC_OK			PIND4
#define I_GsmPowerMonitor	PIND5
#define In_7				PIND6
#define I_GsmSimNotInserted	PIND7

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#endif