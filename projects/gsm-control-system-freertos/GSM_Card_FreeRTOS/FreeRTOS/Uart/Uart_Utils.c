/*
   Uart_Utils.c

   GSM_ControlSystem "GSA" V1.0.0  Copyright (C) 2012 Easy System


    *********************************************************************************
    *    >>> See http://www.esyste.com for Hw Documentation <<<						*
    *																				*
    *    Thank you for using GSA, and thank you for your support!					*
    *																				*
    *********************************************************************************
    see header file for details
*/
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include <string.h>
#include <avr/pgmspace.h>

#include "ISR_Uart.h"
#include "Uart_Utils.h"
#include "DefineIO.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

//extern functions
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
extern bool CmpSubStr(char *, char *);

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Global Variables used by ISR Uart. see ISR_Uart.c
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
extern volatile unsigned char 		SearchStr;
extern volatile uint8_t 			rx_i;
extern volatile unsigned char *		SearchFor;
extern const unsigned char * const	SearchStrings[];
extern volatile	xQueueHandle		TxMessageIdQueue;	//this queue is only for TxRxCommand function to wait for answer done from a command to modem
extern volatile	xQueueHandle		RxMessageIdQueue;

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
/************************************************************************************************************************************
Function: SetResponse()
Purpose:  witch response we expect from modem  (ex: "OK")
Input:    Response: is an Array index in program space where are stored all responses we expect from modem
Returns:  none
*************************************************************************************************************************************/
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void SetResponse(unsigned char Response)
{
	SearchStr	= Response;
	rx_i		= 0;
	SearchFor	= (unsigned char *)pgm_read_word(&(SearchStrings[Response]));  //Set desired search ptr to string
}
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//TxRxCommand: Transmit a command and wait for a response from modem 
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void TxRxCommand( char Response, UARTMessage *QueueCMD)
{

	static portTickType		MsToWait = 6000/portTICK_RATE_MS; // 6000 ms

	//Enable right Rx answer
	SetResponse( Response );
				
	//reset queue if data exists for syncronization security
	xQueueReset( TxMessageIdQueue );
	xQueueReset( RxMessageIdQueue );
	
	//data already present			
	UartStartTX( );

	//suspend till  tx complete
	xQueueReceive( TxMessageIdQueue, &(QueueCMD->MessageID), portMAX_DELAY );
		
	//if command was one of these, Timeout is 0.5 Sec (no need to wait 6 seconds)
	if (	QueueCMD->MessageID == CCLK_ID			||
			QueueCMD->MessageID == HANG_UP_ID		||
			QueueCMD->MessageID == SMSSENDHEADR_ID	||
			QueueCMD->MessageID == ESC_ID
		)
		MsToWait = 500/portTICK_RATE_MS;		//change period to 0.5 sec

	// Suspend for Rx complete or timeout or Error
	// vPrepareCMDFromModem() task do it
	if( xQueueReceive( RxMessageIdQueue, QueueCMD, MsToWait*portTICK_RATE_MS ) == pdTRUE ) 
	{
		switch( QueueCMD->MessageID )
		{
			//-----------------------------------------------------------------------------------
			case DATA_READY_ID:
							
				//if message sent from modem = "ERROR"
				if( CmpSubStr(QueueCMD->ptrUART_Buf, "ERROR") )
					QueueCMD->MessageID = MODEM_NO_DATA;

				return;
						
			break;
			//-----------------------------------------------------------------------------------
		}
	}
	else 
	{
		//Timeout to answer
		QueueCMD->MessageID = MODEM_NO_DATA;
	}	
	return;
}
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
