/*
   TaskCmdFromModem.c

   GSM_ControlSystem "GSA" V1.0.0  Copyright (C) 2012 Easy System


    *****************************************************************************
    *		>>> See http://www.esyste.com for Hw Documentation <<<				*
    *																			*
    *			Thank you for using GSA, and thank you for your support!		*
    *																			*
    *****************************************************************************
    see header file for details
*/
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#include <avr/io.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <avr/wdt.h>
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#include "Define.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "modem.h"
#include "ISR_Uart.h"
#include "task.h"
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// external Variables
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
volatile	extern xQueueHandle		UartMessageQueue; //from ISR_Uart.c
volatile 	extern uint16_t			CommandFlags;
volatile	extern uint8_t			StatusFlags;
volatile	extern char				SmsIndex;

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// GLOBAL Variables
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
volatile	xQueueHandle			TxMessageIdQueue; //this queue is only for TxRxCommand function to wait for answer done from a command to modem
volatile	xQueueHandle			RxMessageIdQueue; //this queue is only for TxRxCommand function to wait for answer done from a command to modem

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// This task is a server for modem, progresses all RX queue from modem and prepare a general command queue to execute by a modem task
// This Task has priority higher than all other tasks
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void vPrepareCMDFromModem( void *pvParametres )
{
	UARTMessage QueueCMD;


	//only one item needed
	RxMessageIdQueue = xQueueCreate( 1, sizeof( UARTMessage ) );
	TxMessageIdQueue = xQueueCreate( 1, sizeof( UARTMessage ) );

	while( 1 )
		{
			//wait for a command from UARTD
			if ( xQueueReceive( UartMessageQueue,  &QueueCMD, portMAX_DELAY ) == pdPASS )
				{
						//reset queue if data exists
						xQueueReset( TxMessageIdQueue );
						xQueueReset( RxMessageIdQueue );

						switch ( QueueCMD.MessageID )
						{
							//---------------------------------------------------------------------------------------------------------------------------------------
							case NO_CMD_ID: //used by interrupt to say trasmission end

								//send message for TxRxCommand
								xQueueSend( TxMessageIdQueue, &QueueCMD, portMAX_DELAY);

							break;
							//---------------------------------------------------------------------------------------------------------------------------------------

							//---------------------------------------------------------------------------------------------------------------------------------------
							case SERIAL_ERROR_ID:		//Serial error...

								//send message for TxRxCommand
								xQueueSend( RxMessageIdQueue, &QueueCMD, portMAX_DELAY);

							break;
							//---------------------------------------------------------------------------------------------------------------------------------------

							//---------------------------------------------------------------------------------------------------------------------------------------
							case INCOMING_CALL_ID:		//Command from modem: incoming call from modem

								CommandFlags |= INCOMING_CALL_MODEM;

								//send message for TxRxCommand
								xQueueSend( RxMessageIdQueue, &QueueCMD, portMAX_DELAY);

							break;
							//---------------------------------------------------------------------------------------------------------------------------------------

							//---------------------------------------------------------------------------------------------------------------------------------------
							case INCOMING_SMS_ID:		//Command from modem: incoming sms from modem

								CommandFlags |= INCOMING_SMS_MODEM;
								SmsIndex	  = QueueCMD.UART_Index;

								//send message for TxRxCommand
								xQueueSend( RxMessageIdQueue, &QueueCMD, portMAX_DELAY);

							break;
							//---------------------------------------------------------------------------------------------------------------------------------------

							//---------------------------------------------------------------------------------------------------------------------------------------
							case BUSY_ID:		//Command from modem: BUSY

								//send message for TxRxCommand
								xQueueSend( RxMessageIdQueue, &QueueCMD, portMAX_DELAY);

							break;
							//---------------------------------------------------------------------------------------------------------------------------------------

							//---------------------------------------------------------------------------------------------------------------------------------------
							case NANSWER_ID:	//Command from modem: no answer
							case NCARRIER_ID:	//Command from modem: no carrier

								//send message for TxRxCommand
								xQueueSend( RxMessageIdQueue, &QueueCMD, portMAX_DELAY);

							break;
							//---------------------------------------------------------------------------------------------------------------------------------------

							//---------------------------------------------------------------------------------------------------------------------------------------
							case DATA_READY_ID:	//Command from modem: a command from TxRxCommand function is correctly executed

								//send message for TxRxCommand
								xQueueSend( RxMessageIdQueue, &QueueCMD, portMAX_DELAY);

								break;
							//---------------------------------------------------------------------------------------------------------------------------------------

							//---------------------------------------------------------------------------------------------------------------------------------------
							case CCLK_ID:		//particulars command for TxRxFunction
							case HANG_UP_ID:
							case SMSSENDHEADR_ID:
							case ESC_ID:

								//send message for TxRxCommand
								xQueueSend( TxMessageIdQueue, &QueueCMD, portMAX_DELAY);

								break;
							//---------------------------------------------------------------------------------------------------------------------------------------

							//---------------------------------------------------------------------------------------------------------------------------------------
							default:
							//do nothing, only discard message from queue
							//---------------------------------------------------------------------------------------------------------------------------------------
							break;
							//---------------------------------------------------------------------------------------------------------------------------------------
						}
				}
		}
}
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
