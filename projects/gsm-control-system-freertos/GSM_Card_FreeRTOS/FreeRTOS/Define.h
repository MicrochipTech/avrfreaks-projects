#ifndef DEFINE_H
#define DEFINE_H

/*
   Define.h. h

   GSM_ControlSystem "GSA" V1.0.0  Copyright (C) 2012 Easy System


    *****************************************************************************
    *    >>> See http://www.esyste.com for Hw Documentation <<<					*
    *																			*
    *    Thank you for using GSA, and thank you for your support!				*
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


/****************************************************************************************
 * 			List of general define for  GSM_ControlSystem "GSA"							*
 ***************************************************************************************/

//PREPAYED SIM. Enable API asking for credit. (Comment this define in case SIM not prepayed)
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#define PREPAYED
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//COUNTRY
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
/* In different countries local operators manage incoming call numbers in different way

EUROPE:
  All incoming calls and SMS incoming numbers are in this format:

  +[country code][phone number]

  where number  may be [phone number] or [area code][phone number]

  So in SIM phone book is enough to store number in this format (+[country code][phone number])

BRAZIL: number phone is 8 digit
  Incoming call:
	if calling is in the same area code from receiving phone, displayed number is only 8 digit
	if calling is in the different area code from receiving phone, displayed number is 0[code area 2 digit ][phone number]. Ex:  0 54 81408043
	if calling is in the different country from receiving phone, displayed number is   00[operator][country code][phone number]: Ex: 00 41 39 3473847300

  Incoming SMS:
	All the same like above if calling and receiving phone are brazilian
	If calling phone is from different country format number is European. Ex: + 39 3473847300

!!!! CRAZY PEOPLE!!!

FUNCTIONS:
	CheckNumber(...)
	SetNumber(...)
Are API to manage these problems
*/
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// define for program TEST. Undefine to compile for Operative system
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//#define TEST_HW

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Definitions & LIMITATIONS
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Temperature sensor is a PT100 (undefine if not)
//#define PT100

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Max event number to program by SMS for memory reservation
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#define NUM_MAX_EVENTS				32
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Max weekly event number to program by SMS for memory reservation
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#define NUM_MAX_WEEK_EVENTS			32
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Sizeof of DATA_EVENTS MUST BE  = NUM_MAX_EVENTS. Needs to reserve one bit for each event
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#define DATA_EVENTS					uint32_t

#if 32 != NUM_MAX_EVENTS
	#error Error definition:  Sizeof of DATA_EVENTS MUST BE  = NUM_MAX_EVENTS.
#endif

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#define NUM_MAX_PHONES				6
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#define MAX_STRING_LEN_SMS_CREDIT	15
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#define MAX_PHONE_NUMBER_SIZE		17
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#define MAX_PROG_STEP				20
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#define MAX_STRINGS					10			 // max number of strings
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#define MAX_STRING_CHAR_IO			11			 // max length for each I/O pin name
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#define MAX_STRING_SMS_CHAR			50			// max length for strings to send
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#define UART_RX_BUFFER_SIZE			400			// PDU format: 350char + header => 400 chars max  for 1 SMS
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#define UART_TX_BUFFER_SIZE			350			// PDU format: 350 chars max  for 1 SMS
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#define MAX_NUM_SMS					3			//max number concatenated SMS
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#define SMS_ASCII_BUFFER_SIZE		480			// MAX for 3 SMS 160 *3 = 480
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#define SINGLE_SMS_BUFFER_SIZE		160
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#define MAX_LEN_UNSTRUCTURED_DATA	150
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#define NUM_FILTERS					5			// 4 for each input and 1 for power input
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#define VAL_FILTER_VCC				10/portTICK_RATE_MS 	// ms
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#define VAL_FILTER_IO				10/portTICK_RATE_MS		 // ms
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#define HISTERESYS					1			// Hysteresys for temperature 1°C.
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#define LOOP_ADC					10
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#define BAUD_RATE					19200		// Modem baud rate
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#define FAULT_TEMP_PTC				1000

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#if SMS_ASCII_BUFFER_SIZE < UART_RX_BUFFER_SIZE
# error "SMS_ASCII_BUFFER_SIZE must be >= di UART_RX_BUFFER_SIZE"
#endif

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Settings for I/O pins
#include "DefineIO.h"
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Strings and SMS stored
// 0-5 : Input names		A0,A1,I1,I2,I3,I4
// 6-9 : output names		Q1,Q2,Q3,Q4
// String Index for alarm
#define STR_IDX_A0			0
#define STR_IDX_A1			1
#define STR_IDX_I1			2
#define STR_IDX_I2			3
#define STR_IDX_I3			4
#define STR_IDX_I4			5
#define STR_IDX_Q1			6
#define STR_IDX_Q2			7
#define STR_IDX_Q3			8
#define STR_IDX_Q4			9
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// GENERAL MASK BITS
#define  BIT_0				0x0001
#define  BIT_1				0x0002
#define  BIT_2				0x0004
#define  BIT_3				0x0008
#define  BIT_4				0x0010
#define  BIT_5				0x0020
#define  BIT_6				0x0040
#define  BIT_7				0x0080
#define  BIT_8				0x0100
#define  BIT_9				0x0200
#define  BIT_10				0x0400
#define  BIT_11				0x0800
#define  BIT_12				0x1000
#define  BIT_13				0x2000
#define  BIT_14				0x4000
#define  BIT_15				0x8000

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// vector Index fo I/O input filters
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#define IO_0				0
#define IO_1				1
#define IO_2				2
#define IO_3				3
#define IN_VCC				4

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// MASK FLAGS for MODALITY EXECUTION
// GeneralModalityFlags Variable
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#define  SMS_NO_POWER		0x01

// EXECUTION MODALITY
#define  RUN_PROGRAM		0x02		//execution program given by SMS
#define  GATE_MODE			0x04		//Close and open an output for a time when calling GSA, otherwise sends status
#define  TOGGLE_RING		0x08		//First call close an output, second call open the output and close onother.

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// MASK FLAGS for COMMAND CONTROL
// CommandFlags Variable
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#define  MEMO_LAST_VCC			BIT_0     //  Memo last VCC
#define  SEND_SMS_VCC			BIT_1     //  Send SMS due to VCC fall down
#define  ASK_FOR_NET			BIT_2     //  ask if modem is connected to operator
#define  ASK_FOR_CREDIT			BIT_3     //  ask credit
#define  SEND_SMS_STR			BIT_4     //  Send SMS string by program
#define  SEND_SMS_STR_CALL		BIT_5     //  Send SMS string by program with phone and call confirmation
#define  SEND_SMS_PRG			BIT_6     //  Send SMS PRogram to caller
#define  ASK_FOR_DATETIME		BIT_7     //  ask for DateTime
#define  SEND_SMS_STATUS		BIT_8     //  SEND status
#define  SEND_CREDIT_STRING		BIT_9     //  SEND  credit string
#define  INCOMING_CALL_MODEM	BIT_10    //  incoming call from modem
#define  INCOMING_SMS_MODEM		BIT_11    //  incoming sms from modem

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// MASK FLAGS for STATUS CONTROL
// StatusFlags Variable
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#define  SIM_INSERTED			BIT_0		//  Sim is inserted
#define  MESSAGE_TO_READ		BIT_1		//  there is a sms to process
#define  ADC_CONVERSION_ON		BIT_2		//  very clear define
#define  MODEM_BUSY				BIT_3

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// VALUES FOR GeneralStatusModem
// to show with led blink what modem is doing
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#define POWER_ON_LED			0xF000	// POWER ON MODEM
#define	CHECK_SIM_LED			0xE000	// check for sm inserted
#define	SEND_INIT_CMD_LED		0xD000	// sending init command to modem
#define	REGISTER_TO_NET_LED		0xC000	// modem is looking for network
#define	READ_BOOK_LED			0xB000	// modem is reading sim book
#define GET_CREDIT_LED			0xA000	// modem is asking for credit
#define	STARTUP_ERROR_LED		0X9000	// startup modem error: OS tries to restart again
#define	TIMESTAMP_LED			0X8000	// read timestamp from network is available

//DIFFERENT CASE IN MODEM TASK
#define	WAIT_FOR_COMMAND_LED	0X7000	// 
#define	INCOMING_SMS_MODEM_LED	0X6000	// 
#define	INCOMING_CALL_MODEM_LED	0X5000	//
#define	MESSAGE_TO_READ_LED		0X4000	//
#define	SEND_SMS_STATUS_LED		0X3000	//
#define	SEND_SMS_STR_LED		0X2000	//
#define	SEND_SMS_PRG_LED		0X1000	//
#define	SEND_SMS_STR_CALL_LED	0X0F00	//
#define	SEND_SMS_VCC_LED		0X0E00	//
#define	ASK_FOR_NET_LED			0X0D00	//
#define	SEND_CREDIT_STRING_LED	0X0C00	//
#define	ASK_FOR_DATETIME_LED	0X0B00	//

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//  Modem command
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#define INITCMD1				"AT&F1;E0;V1;+CREG=2;+CPBS=\"SM\";#DIALMODE=0\r"
#define INITCMD2				"AT+CLIP=1;+CNMI=2,3,0,0,1;+CMGD=0,4\r"
#define NITZ					"AT#NITZ=1,1\r"
#define READ_BOOK				"AT+CPBR=%u,%u\r"
#define SMSSENDHEADR			"AT+CMGS="
#define ENDSTRING				"\r\n"
#define ENDCMDSMS				"\x1A"
#define ESC						"\x1B"
#define RING					"RING"
#define HANG_UP					"ATH\r"
#define READ_SMS				"AT+CMGR="
#define DELETE_SMS				"AT+CMGD="
#define ASK_NET_OK				"AT+CREG?\r"
#define CMD_ASK_NET				"AT#MONI?\r"
#define CMD_OK_READ_BOOK		"AT+CPBR=?\r"
#define FIND_BOOK_ENTRY			"AT+CPBF=%s\r"
#define DIAL_TO_NUM				"ATD"
#define CMTI					"CMTI"
#define CCLK					"AT+CCLK?\r"

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//Call type for request credit
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#define CALL_TO_NUMBER_AND_WAIT_SMS	1
#define SMS_TO_NUMBER_AND_WAIT_SMS	2
#define UNSTRUCURED_SERVICE_DATA	3

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//Message center for Vodafone o Tim Italy
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#define CALLCENTRE_VOD  		"404"
#define SMSCENTRE_TIM			"40916"
#define SMSCREDIT_TIM			"PRE CRE SIN"

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//callback type for SDK
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
typedef void (*CBK)(void);
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#endif
