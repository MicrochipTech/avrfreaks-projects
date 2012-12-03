#ifndef ISR_UART_H
#define ISR_UART_H
/*
   ISR_Uart.h

   GSM_ControlSystem "GSA" V1.0.0  Copyright (C) 2012 Easy System


		*****************************************************************************
		*    >>> See http://www.esyste.com for Hw Documentation <<<					*
		*																			*
		*    Thank you for using GSA, and thank you for your support!				*
		*																			*
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
#include "FreeRTOS.h"
#include <stdbool.h>

//UART configuration: BAUD 19200
#define BAUD_UBRR   ( (F_CPU/16)/BAUD_RATE -1 )
/* Set frame format: asynchronous, 8data, no parity, 2 stop bit */

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	//Create UART RX message queue
	/* 4 items: 1 for Tx (task only send a buffer and wait for answer)
	 * maximum we can have one message for answer, one for incoming call and one for SMS.
	 * Total 4 item so we set 5 for one message more
	 */
#define QUEUE_LENGTH		5

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//STRUCTS FOR UART
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//define type of command to or from Modem: Message ID
typedef enum	{
				CCLK_ID = 0,			//Command to modem was CCLK
				HANG_UP_ID,				//Command to modem
				SMSSENDHEADR_ID,		//Command to modem
				ESC_ID,					//Command to modem
				NO_CMD_ID,				//Command to modem without answer: example only a trasmission command
				DATA_READY_ID,			//Command from modem: data ready after a command sent to modem
				INCOMING_CALL_ID,		//Command from modem: incoming call from modem
				INCOMING_SMS_ID,		//Command from modem: incoming sms from modem
				BUSY_ID,				//Command from modem: BUSY
				NANSWER_ID,				//Command from modem: NO ANSWER
				NCARRIER_ID,			//Command from modem: NO CARRIER
				SERIAL_ERROR_ID,		//Serial Error. Data not coherent
				MODEM_NO_DATA			//error no data received or "ERROR" modem answer

				} ModemMessageID;
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
typedef struct	{
					ModemMessageID		MessageID;
					char *				ptrUART_Buf;	//only a ptr for memory reasons
					int					UART_Index;
				} UARTMessage;

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#ifndef WORD
  typedef unsigned short WORD;
#endif

#ifndef BYTE
  typedef unsigned char BYTE;
#endif

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#define UART_RX_HW_ERROR		0xF6		// Failed TX cause Hardware conflict
#define UART_TX_FAILD			0xF5		// Failed TX                   
#define UART_FRAME_ERROR		0xF4		// Framing Error by UART       
#define UART_OVERRUN_ERROR		0xF3		// Overrun condition by UART   
#define UART_BUFFER_OVERFLOW	0xF2		// receive buffer overflow     
#define UART_NO_DATA			0xF1		// no receive data available   
#define ERR_					0xFF		//some else error 
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
/*
** function prototypes
*/
void					xUartInit( unsigned int baudrate );
portBASE_TYPE			UartStartTX(  void );
#endif // ISR_UART_H

