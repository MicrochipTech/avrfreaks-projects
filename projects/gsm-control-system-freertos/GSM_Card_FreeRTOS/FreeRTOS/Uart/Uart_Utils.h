#ifndef UART_UTILS_H
#define UART_UTILS_H
/*
   ISR_Uart.h

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
#include "Define.h"
#include "ISR_Uart.h"
#include <stdbool.h>
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

//Index to find string in SearchStrings (PRG_SPACE)
// FOLLOW THE INDEX IN SearchStrings DEFINITION
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#define OK_			0                           //Used to look up Response
#define CMTI_		1                           //Used to look up Response
#define READY_		2                           //Used to look up Response
#define CRLF_		3                           //Used to look up Response
#define END_RING_ 	4                           //Used to look up Response
#define BUSY_		5                           //Used to look up Response
#define NCARRIER_	6                           //Used to look up Response
#define CTRLCALL_	7                           //Used to look up Response
#define NANSWER_	8                           //Used to look up Response
#define NO_DEFINED_	9
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
/*
** function prototypes
*/
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
/************************************************************************************************************************************
Function: SetResponse()
Purpose:  witch response we expect from modem  (ex: "OK")
Input:    Response: is an Array index in program space where are stored all responses we expect from modem
		  Typically : OK_
Returns:  none
*************************************************************************************************************************************/
void SetResponse(unsigned char Response);

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
/*
Function: TxRxCommand(char Response, UARTMessage *QueueCMD)
Purpose:  Transmit a command and wait for a response from modem set by a SetResponse function
Input:    char Response			: is an Array index in program space where are stored all responses we expect from modem
		  UARTMessage *QueueCMD	: pointer to a structure UARTMessage where is stored message ID response.
Returns:  none

WARNING:	this function suspend task caller till response from modem or timout
			Data MUST BE already prepared in transmission structure TxCommandToModem
example:
			strcpy( TxCommandToModem.ptrUART_Buf, INITCMD1 );
			//wait for Response
			TxRxCommand(OK_, &QueueCMD);
*/
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void TxRxCommand(char Response, UARTMessage *QueueCMD);

#endif // UART_UTILS_H

