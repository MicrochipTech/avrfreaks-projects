/*
   MODEM.h

   GSM_ControlSystem "GSA" V1.0.0  Copyright (C) 2012 Easy System


    ***************************************************************************
     *    >>> See http://www.esyste.com for Hw Documentation <<<              *
     *                                                                                                                         *
     *    Thank you for using GSA, and thank you for your support!                    *
     *                                                                                                                         *
    ***************************************************************************


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

    All FreeRTOS files follow their own license.

    1 tab == 4 spaces!

*/
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//Include
#include "FreeRTOS.h"
#include "timers.h"

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// messages define for modem
#define OK								0
#define SMS_TOO_LONG					1
#define ERROR_COMPRESSING				2
#define NO_ANSWER_FROM_CMD				3
#define NO_NETWORK_AVAILABLE			4
#define NO_PHONE_AVAILABLE				5
#define NO_SMS_CREDIT_AVAILABLE			6
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

//Index in the string returned by the modem where is SMS number (ONLY FOR TELIT)
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#define IDX_SMS							8

//----------------------------------------------------------------------------------------------------------------------------------------------------------
// phones array
//----------------------------------------------------------------------------------------------------------------------------------------------------------
typedef struct
{
	unsigned char ID;
	char PhoneNr[MAX_PHONE_NUMBER_SIZE];

} NPHONES;
//----------------------------------------------------------------------------------------------------------------------------------------------------------
// management  multiple SMS
//----------------------------------------------------------------------------------------------------------------------------------------------------------
typedef struct
{
	unsigned char IndexSMS;
	unsigned char Lenght;
	unsigned char Identifier;
	unsigned char TotSMS;
	char Str[SINGLE_SMS_BUFFER_SIZE];

} SMS_List;

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Functions
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
/*
Function: StartUpModem( bool WithPowerOff )
Purpose:  initialize and startup Telit modem and sets correct parameters.
Input:    bool WithPowerOff	: power off modem or not before initialyze modem
Returns:
		* false = not yet terminated startup
		* true = terminated startup
*/
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
bool StartUpModem(bool WithPowerOff);
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
/*
Function: SMSSend(char *Msg, char *Phone, char *Err )
Purpose:  SMS MESSAGES SEND: call this function till true
Input:    char *Msg		:ptr to ascii message. Max char are SMS_ASCII_BUFFER_SIZE and can be 3 SMS
		  char *Phone	:ptr to phone number
		  char *Err		:ptr to error (if any) when done.
Returns:
		* false = not yet terminated SMS
		* true = terminated SMS
WARNING: We use SMSRxBuf as temporary buffer!!!!

*/
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
bool SMSSend(char *Msg, char *Phone, char *Err );

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
/*
Function: ReadBookFromSIM( void )
Purpose:  read phonebook and compile list of phone available
Input:    none
Returns:
		* false = not read book
		* true = ok. book list ready
*/
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
bool ReadBookFromSIM( void );

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
/*
Function: ReadCreditCentreFromSIM( void )
Purpose:  read credit cneter number from sim and store it in a global variable
Input:    none
Returns:
		* false = not available number
		* true = Ok.
*/
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
bool ReadCreditCentreFromSIM( void );

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
/*
Function:	StrIndex(char *InBuf, char c)
Purpose:	return index's char c inside InBuf string starting search from the beginning. Index starts from 1 and return 0 if 'c' not found
Input:		char *InBuf:	input null terminated string
			char c:			char to find
Returns:	'c' index or 0 if not found
*/
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
int StrIndex(char *InBuf, char c);

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
/*
Function:	StrLastIndex(char *InBuf, char c)
Purpose:	return index's char c inside InBuf string starting search from the end. Index starts from 0 and return -1 if 'c' not found
Input:		char *InBuf:	input null terminated string
Returns:	'c' index or 0 if not found
*/
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
int StrLastIndex(char *InBuf, char c);

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
/*
Function:	CmpSubStr(char *Buf, char *Str)
Purpose:	compare string to an sub string
Input:		char *Buf:	input null terminated string
			char *Str:	substring to find
Returns:	true if match
*/
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
bool CmpSubStr(char *Buf, char *Str);

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
/*
Function:	vTaskManageModem( void *pvParametres )
Purpose:	This task is a server for modem, processes all RX queue from modem and executes command for modem
Input:		void *pvParametres: paremetres for modem task (NOT USED)
Returns:	none
*/
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void vTaskManageModem( void *pvParametres );

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
/*
Function:	SendPowerOn(bool WithPowerOff)
Purpose:	send a power on to modem
Input:		bool WithPowerOff:	if already on with this bit send before a power off
Returns:
		* false = not yet terminated command
		* true = terminated command
*/
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
bool SendPowerOn(bool WithPowerOff);

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
/*
Function:	DecodeCMTI(char *Buf)
Purpose:	return index's CMTI inside a string
Input:		char *Buf:	input null terminated string
Returns:	index or 0 if not found
*/
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
int DecodeCMTI(char *Buf);

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
/*
Function:	ReadMSG( char *Buf )
Purpose:	return index's CMTI inside a string
Input:		char *Buf:	input null terminated string
Returns:	true if no error format
*/
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
bool ReadMSG( char *Buf );

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
/*
Function:	SMSSendWithConfirm(char *String, char *Err);
Purpose:	Send SMS with call confirm
Input:		char *String: string to send
			char *Err	: error type when done
Returns:	
		* false = not yet terminated command
		* true = terminated command
*/
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
bool  SMSSendWithConfirm(char *String, char *Err);

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
/*
Function:	GetCredit( char *DestString, char *Err )
Purpose:	ask for credit and put the result in DestString
Input:		char *DestString: destination string used to try to send a call to service number to receive a message for credit
			char *Err		: error type when done
Returns:	
		* false = not yet terminated command
		* true = terminated command
*/
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
bool GetCredit( char *DestString, char *Err );

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
/*
Function:	AskForNetOK(char *Err)
Purpose:	Ask for net registration
Input:		char *Err: error type when done
Returns:	
		* false = not yet terminated command
		* true = terminated command
*/
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
bool AskForNetOK(char *Err);

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
/*
Function:	CallServiceNumerAndWaitString(char *ServicePhone, char *DestString, char *Err )
Purpose:	Unstructured request data: to service number and wait answer string
Input:		char *ServicePhone:	service center number
			char *DestString:	modem answer to command
			char *Err: error type
Returns:	true = no error
*/
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
bool  CallServiceNumerAndWaitString(char *ServicePhone, char *DestString, char *Err );


//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
/*
Function:	CheckNumber( char *Dest, char *Src)
Purpose:	Unstructured request data: to service number and wait answer string
			ONLY INTERNATIONAL FORMAT WITHOUT OPERATOR CODE
			NOT THIS 00[operator][country code][number] (00 41 39 3473847300)
			BUT THIS +[country code][number] (+ 39 347 3847300)
			(+ 55 54  81500009)

			if Dest begins with +  => international numer. Check All
			if Dest = 8  char      => same area      , check only last 8 char from Src       (       81438046 )
			if Dest = 11 char      => different area , check only last 10 char               (0   54 81438046 )
			if Dest = 13 char      => different area with operator number.Check last 10 char (041 54 81438046 )
			if Dest begins with 00 => different country. Check starts with 5th character dest and second src.

Input:		char *Dest:	Number to check (given by network)
			char *Src:	stored number in SIM
Returns:	true = number match
*/
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
bool  CheckNumber( char *Dest, char *Src);

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
/*
Function:	SetNumber(char *Phone)
Purpose:	set correct format number to send sms or call
			if we are in BRAZIL AND PhoneNr begins with 00 => phone come from different country.
			If possible to send SMS to other country only in EUROPEAN Format.

Input:		char *Phone: phone number given by network
Returns:	none
*/
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void  SetNumber(char *Phone);

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
/*
Function:	void vCallbackFunction( xTimerHandle xTimer )
Purpose:	Callback for timer. Only ask for net every 60 seconds
Input:		xTimerHandle xTimer: timer handler
Returns:	none
*/
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void vCallbackFunction( xTimerHandle xTimer );
