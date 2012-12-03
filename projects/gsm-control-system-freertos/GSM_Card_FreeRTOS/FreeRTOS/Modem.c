/*
   Modem.c

   GSM_ControlSystem "GSA" V1.0.0  Copyright (C) 2012 Easy System


    *****************************************************************************
    *		>>> See http://www.esyste.com for Hw Documentation <<<				*
    *																			*
    *			Thank you for using GSA, and thank you for your support!	  	*
    *****************************************************************************
    see header file for details
*/
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <avr/wdt.h>
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#include "Define.h"
#include "Modem.h"
#include "Uart_utils.h"
#include "ISR_Uart.h"
#include "Parser.h"
#include "PDU_Code_Decode.h"

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void PrepareStatus(char *   );

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// external Variables
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
volatile	extern bool				RunProgram									 			;
volatile	extern char				IndexStringSMS											;
volatile	extern UARTMessage		TxCommandToModem										;
			extern char				PhoneNrRx[MAX_PHONE_NUMBER_SIZE]						;
			extern char				SMSstrings[MAX_STRINGS][MAX_STRING_SMS_CHAR]	EEMEM	;
			extern char				IOStrings [MAX_STRINGS][MAX_STRING_CHAR_IO]		EEMEM	; //Allocated strings for I/O description
			extern char				PrgNoConverted[SMS_ASCII_BUFFER_SIZE]			EEMEM	;
			extern GATE				GateMode										EEMEM	;
			extern TOGGLERING 		ToggleRingMode									EEMEM	;
			extern char				Amount[]										EEMEM	;
volatile	extern uint8_t			StatusFlags												;
volatile 	extern uint16_t			CommandFlags											;
volatile 	extern bool				RunGateMode												;
volatile	extern bool				RunToggleRingMode										;
volatile	extern char				NumRingToggleRingMode									;
volatile	extern unsigned long int	CurrentDateTime										;

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// GLOBAL Variables
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
volatile 	char			WayForCredit																	;// SMS or CALL or Unstructured Data Service
			unsigned char 	EE_WayForCredit								EEMEM = UNSTRUCURED_SERVICE_DATA  	;
			NPHONES			Phones[NUM_MAX_PHONES] 						EEMEM								; // PHONE NUMBERS From SIM BOOK in EEPROM
			char			TelCreditService[MAX_PHONE_NUMBER_SIZE]		EEMEM = {0}							; //Valid Phone number to ask for credit
			char			SMSCredit[MAX_STRING_LEN_SMS_CREDIT]		EEMEM = SMSCREDIT_TIM				; //SMS to ask for credit
			char			SMSRxBuf[SMS_ASCII_BUFFER_SIZE]													; //SMS received message buffer
			char			CharBuf[SMS_ASCII_BUFFER_SIZE]													; //SMS Temporary Buffer
			char			SmsCreditString[MAX_LEN_UNSTRUCTURED_DATA]										; //SMS to send for credit
			char			PhoneNr[MAX_PHONE_NUMBER_SIZE]													; //Current Valid Clip number
			SMS_List		SmsListTx[MAX_NUM_SMS]															;
			SMS_List		SmsListRx[MAX_NUM_SMS]															;
volatile	char			SmsIndex																		;
volatile	uint16_t		GeneralStatusModem																;//Status for blink led

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
/* This  function initialize and startup Telit modem and sets correct parameters.
 * return value:
 * false = not yet terminated startup
 * true = terminated startup
 */
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
bool StartUpModem( bool WithPowerOff )
{
	UARTMessage				QueueCMD		;
	char 					Err 		= 0	;
	portTickType			xLastWakeTime	;

	static unsigned char 	Retry			;
	static char				CaseStartUp	=0	;

	// -------------------------------------------------------------------------
	// MODEM MANAGEMENT
	// -------------------------------------------------------------------------
	switch( CaseStartUp )
	{

		// -------------------------------------------------------------------------
		//This is only at power on GSA
		case 0:
				StatusFlags |= MODEM_BUSY;

				memset( SmsCreditString, 0, MAX_STRING_LEN_SMS_CREDIT);
				//read to RAM the way to ask for credit
				WayForCredit  = eeprom_read_byte( &EE_WayForCredit );

				CaseStartUp	= 10;
		break;
		// -------------------------------------------------------------------------

		// -------------------------------------------------------------------------
		// Send power on to GSM
		case 10:
				//for status led
				GeneralStatusModem	= POWER_ON_LED;

				// Send power on to GSM (toggle for 1 second) with no power off if already ON
				// function returns only when GSM is powered on
				//argument means true = force power off
				if( SendPowerOn( WithPowerOff ) )
				{
					//SUSPEND task for 2 sec. Startup after power on. Telit wants this time
					xLastWakeTime = xTaskGetTickCount();
					vTaskDelayUntil( &xLastWakeTime, 2000/portTICK_RATE_MS );

					CaseStartUp++;
				}
		break;
		// -------------------------------------------------------------------------

		// -------------------------------------------------------------------------
		case 11:

				//for status led
				GeneralStatusModem	= CHECK_SIM_LED;

				//wait for SIM INSERTED
				if( !(StatusFlags & SIM_INSERTED) )
					break;

				Retry = 1;

				CaseStartUp++;
		break;
		// -------------------------------------------------------------------------

		// -------------------------------------------------------------------------
		case 12:
				//for status led
				GeneralStatusModem	= SEND_INIT_CMD_LED;

				// Send Init1 command (see define.h)
				// for first sending command echo is enabled by default
				strcpy( TxCommandToModem.ptrUART_Buf, INITCMD1 );

				//wait for Response
				TxRxCommand(OK_, &QueueCMD);

				//retry 2 time, otherwise send power off
				if( QueueCMD.MessageID == MODEM_NO_DATA )
				{
					if( Retry > 2 )
						CaseStartUp = 100;		//error step: Send reset and wait 20 sec and jump to step 1
					Retry++;
				}
				else
				{
					//insert TIMESTAMP FROM Network
					CaseStartUp = 110;
					Retry = 1;
				}

		break;
		// -------------------------------------------------------------------------

		// -------------------------------------------------------------------------
		//ask for network registration
		case 13:

				//for status led
				GeneralStatusModem	= REGISTER_TO_NET_LED;

				if( !AskForNetOK( &Err ) )
				{
					CaseStartUp++;
					Retry++;
				}
				else
					CaseStartUp = 15;

		break;
		// -------------------------------------------------------------------------

		// -------------------------------------------------------------------------
		case 14:

			if( Retry >= 100 )
				{
					CaseStartUp = 100;		//error step: Send reset and wait 20 sec and jump to step 1
					break;
				}

				//SUSPEND task for 1 sec. Startup after power on. Telit wants this time
				xLastWakeTime = xTaskGetTickCount();
				vTaskDelayUntil( &xLastWakeTime, 1000/portTICK_RATE_MS );

				CaseStartUp = 13;

		break;
		// -------------------------------------------------------------------------

		// -------------------------------------------------------------------------
		case 15:
				//for status led
				GeneralStatusModem	= SEND_INIT_CMD_LED;

				// Send Init2 command
				strcpy( TxCommandToModem.ptrUART_Buf, INITCMD2 );

				TxRxCommand(OK_, &QueueCMD);

				if( QueueCMD.MessageID != DATA_READY_ID )
				{
					CaseStartUp = 100;		//error step: Send reset and wait 20 sec and jump to step 1
					break;
				}

				// suspend 30 seconds. Modem after previous command is able to answer only after this time
				// WARNING: portTICK_RATE_MS = 1000/configTICK_RATE_HZ = 1 in this specific case
				// portTickType for this CPU is integer (see portmacro.h) then 16BIT
				// MAXIMUM VALUE IS 65535 TICS = 65,5 SECONDS
				xLastWakeTime = xTaskGetTickCount();
				vTaskDelayUntil( &xLastWakeTime, 30000/portTICK_RATE_MS );

				Retry = 1;

				CaseStartUp++;

		break;
		// -------------------------------------------------------------------------

		// -------------------------------------------------------------------------
		// READ BOOK FROM SIM
		case 16:

				//for status led
				GeneralStatusModem	= READ_BOOK_LED;

				//if Error inside ReadBookFromSIM retry 3 times
				if( !ReadBookFromSIM() )
				{
					if( Retry > 3 )
					{
						CaseStartUp = 100;		//error step: Send reset and wait 20 sec and jump to step 1
						break;
					}
					else
						Retry++;
				}

				CaseStartUp++;

		break;
		// -------------------------------------------------------------------------

		// -------------------------------------------------------------------------
		// READ CREDIT SERVICE CENTRE
		case 17:
				#ifdef PREPAYED

				//for status led
				GeneralStatusModem	= GET_CREDIT_LED;

				//Return only when done or error
				//If No entry phone for credit available does nothing
				ReadCreditCentreFromSIM();
				#endif

				Retry = 1;
				CaseStartUp++;

		break;
		// -------------------------------------------------------------------------

		// -------------------------------------------------------------------------
		// GET CREDIT
		case 18:
				#ifdef PREPAYED

				//for status led
				GeneralStatusModem	= GET_CREDIT_LED;

				if( !GetCredit( SmsCreditString, &Err ) )
					break;

				if( Err != OK )
				{
					if( Retry++ > 2 )
					{
						StatusFlags &= ~MODEM_BUSY;
						//End of Startup
						CaseStartUp = 0;
						return true;
					}
				}
				#endif

				StatusFlags &= ~MODEM_BUSY;

				//End of Startup
				CaseStartUp = 0;
				return true;
		break;
		// -------------------------------------------------------------------------

		// -------------------------------------------------------------------------
		case 100:

				//for status led
				GeneralStatusModem	= STARTUP_ERROR_LED;

				//error step: Send reset and wait 20 sec and jump to step 1
				// Send reset min 200 ms
				PORTC |= _BV(O_GsmReset);
				CaseStartUp++;

		break;
		// -------------------------------------------------------------------------

		// -------------------------------------------------------------------------
		case 101:
				//SUSPEND task for 500 msec. Startup after power on. Telit wants this time
				xLastWakeTime = xTaskGetTickCount();
				vTaskDelayUntil( &xLastWakeTime, 500/portTICK_RATE_MS );

				PORTC &= ~_BV(O_GsmReset);

				//SUSPEND task for 20 sec. Startup after power on. Telit wants this time
				xLastWakeTime = xTaskGetTickCount();
				vTaskDelayUntil( &xLastWakeTime, 2000/portTICK_RATE_MS );

				CaseStartUp++;

		break;
		// -------------------------------------------------------------------------

		// -------------------------------------------------------------------------
		case 102:

				// If I don't have the I_GsmPowerMonitor
				//the battery isn't able to power GSM but
				//is enough for controller.
				if(!(PIND & _BV(I_GsmPowerMonitor)))
				{
					while( !(PIND & _BV(I_GsmPowerMonitor)) );
					CaseStartUp = 0;
				}

				CaseStartUp = 12;
				Retry = 1;
		break;
		// -------------------------------------------------------------------------

		// -------------------------------------------------------------------------
		// TIMESTAMP FROM NET
		// -------------------------------------------------------------------------
		case 110:

				//for status led
				GeneralStatusModem	= TIMESTAMP_LED;

				//wait startup 5 sec
				xLastWakeTime = xTaskGetTickCount();
				vTaskDelayUntil( &xLastWakeTime, 5000/portTICK_RATE_MS );

				CaseStartUp++;

		break;
		// -------------------------------------------------------------------------

		// -------------------------------------------------------------------------
		case 111:
				//ask for timestamp (if avalaible)
				strcpy( TxCommandToModem.ptrUART_Buf, NITZ );

				TxRxCommand(OK_, &QueueCMD);

				CaseStartUp = 13;

		break;
		// -------------------------------------------------------------------------

		// -------------------------------------------------------------------------
	}

	return false;
}
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// This task is a server for modem, progresses all RX queue from modem and executes command for modem
// ManageModem
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void vTaskManageModem( void *pvParametres )
{
 static unsigned char			Retry ;
 static GATE					GateConfig;
 static TOGGLERING				ToggleRingConfig;
 static unsigned char			CaseTaskModem = 0;

 portTickType	xLastWakeTime;
 UARTMessage 	QueueCMD;

 int			i;
 char			Err, Mask;
 char			TelSmsCentre[MAX_PHONE_NUMBER_SIZE];
 xTimerHandle	TimerAskForNetOk;

 //used for TMP value
 Mask = 1;
 // WARNING: portTICK_RATE_MS = 1000/configTICK_RATE_HZ = 1 in this specific case
 // portTickType for this CPU is integer (see portmacro.h) then 16BIT
 // MAXIMUM VALUE IS 65535 TICS = 65,5 SECONDS
 TimerAskForNetOk = xTimerCreate(	"Timer",				// Just a text name, not used by the kernel.
									60000/portTICK_RATE_MS,	// The timer period in ticks: Total 60 seconds.
									pdTRUE,					// The timers will auto-reload themselves when they expire.
									( void * ) &Mask,		// Assign each timer a unique id. here not used
									vCallbackFunction		// Timer callback when expires.
									);

 while(1)
 {
	switch( CaseTaskModem )
	{

	//-----------------------------------------------------------------------
	case 0:
	// ---------------------------------------------------------------------
		//Startup Modem and force power off: stay here till finished
		if( !StartUpModem( true ) )
			break;

		//start TIMER to ask for network ok
		xTimerStart( TimerAskForNetOk, portMAX_DELAY );

		//enable watchdog 4 Seconds!!
		//no for debug
		//wdt_enable(WDTO_4S);

		CaseTaskModem =10;

	break;
	// ---------------------------------------------------------------------

	//-----------------------------------------------------------------------
	case 10:
	// ---------------------------------------------------------------------
		//Start Modem Server
		// watchdog reset : system reset if not executed this command in 4 seconds
		wdt_reset();

		//check SIM
		if(	(StatusFlags & SIM_INSERTED)		&&	//Sim inserted
			( (PIND & _BV(I_GsmPowerMonitor)	||	//power ok to modem
			(StatusFlags | MODEM_BUSY)	)))			//modem is doing something
		{
			CaseTaskModem =20;
			break;
		}
		else
			CaseTaskModem++;

	break;
	// ---------------------------------------------------------------------

	//-----------------------------------------------------------------------
	case 11:
	// ---------------------------------------------------------------------
		// check conditions ok for working
		//NEED TO FORCE RESET MODEM TASK
		// watchdog disable
		wdt_disable();

		//for status led
		GeneralStatusModem	= CHECK_SIM_LED;

		if( (StatusFlags & SIM_INSERTED) && (PIND & _BV(I_GsmPowerMonitor)) )
		{
			CaseTaskModem =0;
			break;
		}

	break;
	// ---------------------------------------------------------------------

	//-----------------------------------------------------------------------
	case 20:
	// ---------------------------------------------------------------------
	// analyze commands from or to modem

		//for status led
		GeneralStatusModem	= WAIT_FOR_COMMAND_LED;

	    if(StatusFlags & ADC_CONVERSION_ON)
			break;

		// ---------------------------------------------------------------------
		//SMS notification
		if( (CommandFlags & INCOMING_SMS_MODEM) && !(CommandFlags & INCOMING_CALL_MODEM) )
		{
			StatusFlags |= MODEM_BUSY;

			//for status led
			GeneralStatusModem	= INCOMING_SMS_MODEM_LED;

			CaseTaskModem = 50;
			break;
		}

        // ---------------------------------------------------------------------
        //incoming string from GSM
        if( CommandFlags & INCOMING_CALL_MODEM )
        {
			//for status led
			GeneralStatusModem	= INCOMING_CALL_MODEM_LED;

	        StatusFlags |= MODEM_BUSY;
	        CommandFlags &= ~INCOMING_CALL_MODEM;
	        CaseTaskModem = 60;
	        break;
        }

        // ---------------------------------------------------------------------
        if(CommandFlags & MESSAGE_TO_READ)
        {
			//for status led
			GeneralStatusModem	= MESSAGE_TO_READ_LED;

	        StatusFlags |= MODEM_BUSY;
	        CommandFlags &= ~MESSAGE_TO_READ;
	        CaseTaskModem = 30;
	        break;
        }

        // ---------------------------------------------------------------------
        // Send SMS status
        if( CommandFlags & SEND_SMS_STATUS )
        {
			//for status led
			GeneralStatusModem	= SEND_SMS_STATUS_LED;

	        CommandFlags &= ~SEND_SMS_STATUS;
	        StatusFlags |= MODEM_BUSY;
	        CaseTaskModem = 40;
	        break;
        }

		// ---------------------------------------------------------------------
		// Send SMS String to first phone Nr
		if( CommandFlags & SEND_SMS_STR )
		{
			//for status led
			GeneralStatusModem	= SEND_SMS_STR_LED;

			CommandFlags &= ~SEND_SMS_STR;
			//Read Phone
			eeprom_read_block(	PhoneNr,&(Phones[0].PhoneNr),sizeof(Phones[0].PhoneNr));
			//Read String
			eeprom_read_block( CharBuf,&(SMSstrings[(int)IndexStringSMS]),MAX_STRING_SMS_CHAR );

			StatusFlags |= MODEM_BUSY;
			CaseTaskModem = 35;
			break;
		}

        // ---------------------------------------------------------------------
        // Send SMS Program to Clip Nr
        if( CommandFlags & SEND_SMS_PRG )
        {
			//for status led
			GeneralStatusModem	= SEND_SMS_PRG_LED;

	        CommandFlags &= ~SEND_SMS_PRG;

	        //Read String
	        eeprom_read_block( CharBuf, PrgNoConverted, SMS_ASCII_BUFFER_SIZE );

	        StatusFlags |= MODEM_BUSY;
	        CaseTaskModem = 35;
	        break;
        }

        // ---------------------------------------------------------------------
        // Send SMS String with call confirm
        if( CommandFlags & SEND_SMS_STR_CALL )
        {
			//for status led
			GeneralStatusModem	= SEND_SMS_STR_CALL_LED;

	        CommandFlags &= ~SEND_SMS_STR_CALL;
	        //Read String
	        memset(CharBuf, 0, SMS_ASCII_BUFFER_SIZE);

	        eeprom_read_block( CharBuf, &(SMSstrings[(int)IndexStringSMS]), MAX_STRING_SMS_CHAR );

	        StatusFlags |= MODEM_BUSY;
	        CaseTaskModem = 70;
	        break;
        }

        // ---------------------------------------------------------------------
        if( CommandFlags & ASK_FOR_CREDIT  )
        {
			//for status led
			GeneralStatusModem	= GET_CREDIT_LED;

	        CommandFlags &= ~ASK_FOR_CREDIT;
	        StatusFlags |= MODEM_BUSY;
	        CaseTaskModem = 38;
	        break;
        }

        // ---------------------------------------------------------------------
        //Request to send SMS because of power
        //Phone number: THE FIRST IN SIMBOOK
        if( CommandFlags & SEND_SMS_VCC )
        {
			//for status led
			GeneralStatusModem	= SEND_SMS_VCC_LED;

	        StatusFlags |= MODEM_BUSY;
	        //Reset command
	        CommandFlags &= ~SEND_SMS_VCC;

	        eeprom_read_block(PhoneNr, &(Phones[0].PhoneNr),sizeof(Phones[0].PhoneNr));

	        CaseTaskModem = 40;
	        break;
        }

        // ---------------------------------------------------------------------
        //Request to ask for net registration OK
        if( CommandFlags & ASK_FOR_NET )
        {
			//for status led
			GeneralStatusModem	= ASK_FOR_NET_LED;

	        StatusFlags |= MODEM_BUSY;
	        //Reset command
	        CommandFlags &= ~ASK_FOR_NET;
	        CaseTaskModem = 80;
	        break;
        }

        // ---------------------------------------------------------------------
        //Request to send credit string
        if( CommandFlags & SEND_CREDIT_STRING )
        {
			//for status led
			GeneralStatusModem	= SEND_CREDIT_STRING_LED;

	        //Reset command
	        CommandFlags &= ~SEND_CREDIT_STRING;
	        if( strlen(SmsCreditString) )
	        {
		        StatusFlags |= MODEM_BUSY;
		        strcpy(CharBuf, SmsCreditString );
		        CaseTaskModem = 35;
		        break;
	        }
        }

        // ---------------------------------------------------------------------
        //No commands then ask for DateTime
        // ---------------------------------------------------------------------
        if( CommandFlags & ASK_FOR_DATETIME )
        {
			//for status led
			GeneralStatusModem	= ASK_FOR_DATETIME_LED;

	        StatusFlags |= MODEM_BUSY;
	        //Reset command
	        CommandFlags &= ~ASK_FOR_DATETIME;
	        CaseTaskModem = 90;
	        break;
        }

        // ---------------------------------------------------------------------
		//return back
		CaseTaskModem =10;

	break;
	// ---------------------------------------------------------------------

	//-----------------------------------------------------------------------
   case 30:
	// ---------------------------------------------------------------------
	// Read Message

		eeprom_read_block (TelSmsCentre, &TelCreditService, sizeof(TelSmsCentre) );
		if( !strcmp(PhoneNr, TelSmsCentre) )
		{
			//copy to buffer read string from network
			strcpy(SmsCreditString, SMSRxBuf);
		}
		else
		{
			Err = -1;
			for (i=0; i < NUM_MAX_PHONES ; i++)
			{
				memset(CharBuf, 0, SMS_ASCII_BUFFER_SIZE);
				eeprom_read_block(CharBuf, &(Phones[i].PhoneNr),sizeof(Phones[i].PhoneNr));
				if( CheckNumber(PhoneNr, CharBuf) )
				{
					//Current phone number found
					Err = 0;
					break;
				}
			}
			if( !Err )
			{
				if( !CheckCommandString(SMSRxBuf) )
				{
					strcpy(CharBuf,"Command Error");
					CaseTaskModem = 35;
					break;
				}
				else
				{
					//Send Ok only if no sms or status to send
					if( !(CommandFlags & SEND_SMS_PRG      ) &&
						!(CommandFlags & SEND_SMS_STATUS   ) &&
						!(CommandFlags & SEND_CREDIT_STRING) )
					{
						strcpy(CharBuf,"OK");
						CaseTaskModem = 35;
						break;
					}
				}
			}
		}
		//go back to analyze new command
		StatusFlags &= ~MODEM_BUSY;
		CaseTaskModem = 10;

	break;
	// ---------------------------------------------------------------------

	//-----------------------------------------------------------------------
	case 35:
	//-----------------------------------------------------------------------
	// Send SMS to PhoneNr

		Retry=1;

		xLastWakeTime = xTaskGetTickCount();
		//SUSPEND task for 500 ms
		vTaskDelayUntil( &xLastWakeTime, 500/portTICK_RATE_MS );

		//Arrange phone number for correct sending
		SetNumber(PhoneNr);
		
		CaseTaskModem++;

	break;
	//-----------------------------------------------------------------------

	//-----------------------------------------------------------------------
	case 36:
	//-----------------------------------------------------------------------
		
		// Send SMS to PhoneNr
		if( !SMSSend(CharBuf, PhoneNr, &Err) )
			break;

		if( !Err )
		{
			#ifdef PREPAYED
			CommandFlags |= ASK_FOR_CREDIT;
			#endif

			StatusFlags &= ~MODEM_BUSY;

			CaseTaskModem = 10;
			break;
		}
		else
		{
			//Try 10 time
			if(Err == SMS_TOO_LONG)
			{
				strcpy(CharBuf,"SMS Status too long");
				wdt_reset();
				CaseTaskModem = 35;
			}
			if ( Retry++ > 10 )
			{
				//failed to send SMS: Credit? Network?
				CaseTaskModem = 10;
				StatusFlags &= ~MODEM_BUSY;
			}
		}

	break;
	//-----------------------------------------------------------------------

	//-----------------------------------------------------------------------
	case 38:
	//-----------------------------------------------------------------------
	//Ask for credit

		#ifdef PREPAYED
		///When normal cycle GetCredit could occur asyncronusly then 200 ms delay guarantes correct time
		///between two TxRx command.

		xLastWakeTime = xTaskGetTickCount();
		vTaskDelayUntil( &xLastWakeTime, 200/portTICK_RATE_MS );

		if( GetCredit(SmsCreditString, &Err ) )
		{
			StatusFlags &= ~MODEM_BUSY;
			CaseTaskModem = 10;
		}
		#else
		StatusFlags &= ~MODEM_BUSY;
		CaseTaskModem = 10;
		#endif

	break;
	//-----------------------------------------------------------------------

	//-----------------------------------------------------------------------
	case 40:
	//-----------------------------------------------------------------------
	//Send status
		memset(CharBuf, 0, SMS_ASCII_BUFFER_SIZE);
		PrepareStatus( CharBuf );
		CaseTaskModem = 35;

	break;
	//-----------------------------------------------------------------------

	//-----------------------------------------------------------------------
	case 50:
	//-----------------------------------------------------------------------
	//SMS Received

		//wait to ask command 500 msec
		xLastWakeTime = xTaskGetTickCount();
		//SUSPEND task for 500 ms
		vTaskDelayUntil( &xLastWakeTime, 500/portTICK_RATE_MS );

		memset(TxCommandToModem.ptrUART_Buf, 0, UART_TX_BUFFER_SIZE);

		// Read Number phone
		strcpy(TxCommandToModem.ptrUART_Buf,READ_SMS);
		//Insert Index SMS
		itoa( SmsIndex, &TxCommandToModem.ptrUART_Buf[IDX_SMS], 10);
		TxCommandToModem.ptrUART_Buf[strlen(TxCommandToModem.ptrUART_Buf)] = '\r';

		TxRxCommand(OK_, &QueueCMD);

		if ( (CommandFlags &= INCOMING_CALL_MODEM) && !(CommandFlags &= MESSAGE_TO_READ) )
		{
			// not yet read SMS but incoming call is arrived: modems aborts to read SMS because incoming call has higher priority
			StatusFlags &= ~MODEM_BUSY;
			CaseTaskModem = 10;
			break;
		}

		if( QueueCMD.MessageID == DATA_READY_ID )
		{
			strcpy(CharBuf, QueueCMD.ptrUART_Buf);
			if( ReadMSG( CharBuf ) )  //Read new message, and decode
			CommandFlags |= MESSAGE_TO_READ;
		}

		//Delete SMS (in any case, even in error)
		strcpy( TxCommandToModem.ptrUART_Buf, DELETE_SMS);
		//Insert Index SMS
		itoa( SmsIndex, &TxCommandToModem.ptrUART_Buf[IDX_SMS], 10);
		TxCommandToModem.ptrUART_Buf[strlen(TxCommandToModem.ptrUART_Buf)] = '\r';

		//only after we are sure to read SMS
		CommandFlags &= ~INCOMING_SMS_MODEM;

		//wait to ask command 500 msec
		xLastWakeTime = xTaskGetTickCount();
		vTaskDelayUntil( &xLastWakeTime, 500/portTICK_RATE_MS );

		TxRxCommand(OK_, &QueueCMD);

		//no check if error

		StatusFlags &= ~MODEM_BUSY;
		CaseTaskModem = 10;

	break;
	//-----------------------------------------------------------------------

	//-----------------------------------------------------------------------
	case 60:
	//-----------------------------------------------------------------------
	//RING
	//Incoming call
	//Phone
		strcpy(PhoneNr, PhoneNrRx);

		//Hang UP
		strcpy(TxCommandToModem.ptrUART_Buf, HANG_UP);

		TxRxCommand(OK_, &QueueCMD);

		if( QueueCMD.MessageID == DATA_READY_ID )
		{
			for (i=0; i < NUM_MAX_PHONES ; i++)
			{
				memset(CharBuf, 0, SMS_ASCII_BUFFER_SIZE);
				eeprom_read_block(CharBuf, &(Phones[i].PhoneNr),sizeof(Phones[i].PhoneNr));

				if( strlen(CharBuf) )
				{
					if(CheckNumber(PhoneNr, CharBuf) )
					{
						if( RunProgram && !RunGateMode && !RunToggleRingMode)
							CaseTaskModem = 40; // send status
						if( RunGateMode && !RunToggleRingMode)
							CaseTaskModem = 100; // activate Ring gate mode
						if( !RunGateMode && RunToggleRingMode)
							CaseTaskModem = 110; // activate toggle ring mode

						break;
					}
				}
			}
		}

		if(CaseTaskModem == 60)
			CaseTaskModem = 10;

		StatusFlags &= ~MODEM_BUSY;

	break;
	//-----------------------------------------------------------------------

	//-----------------------------------------------------------------------
	case 70:
	//-----------------------------------------------------------------------
	//Send SMS with call confirmation
		if ( !SMSSendWithConfirm( CharBuf, &Err ) )
			break;

		if(!Err)
			CaseTaskModem++ ;
		else
		{
			//send sms only to first in list
			memset(PhoneNr, 0, MAX_PHONE_NUMBER_SIZE);

			eeprom_read_block(	PhoneNr,
								&(Phones[0].PhoneNr),
								sizeof(Phones[0].PhoneNr)
								);
			CaseTaskModem = 35;
		}

	break;
	//-----------------------------------------------------------------------

	//-----------------------------------------------------------------------
	case 72:
	//-----------------------------------------------------------------------
		//wait to 500 msec
		xLastWakeTime = xTaskGetTickCount();
		vTaskDelayUntil( &xLastWakeTime, 500/portTICK_RATE_MS );

		StatusFlags &= ~MODEM_BUSY;

		CaseTaskModem = 10;

	break;
	//-----------------------------------------------------------------------

	//-----------------------------------------------------------------------
	case 80:
	//-----------------------------------------------------------------------
	//Ask for Net

		if( !AskForNetOK( &Err ) )
		{
			//we need do send power off and power on to modem: no net available
			//WARNING RESET ALL MODEM STEPS
			if( Err == NO_NETWORK_AVAILABLE)
				CaseTaskModem =0;
			else
				//was only a command error
				CaseTaskModem = 10;
		}
		else
			CaseTaskModem = 10;

		StatusFlags &= ~MODEM_BUSY;

	break;
	//-----------------------------------------------------------------------


	//-----------------------------------------------------------------------
	case 90:
	//-----------------------------------------------------------------------
	//Ask for DateTime

		if( (CommandFlags &= INCOMING_CALL_MODEM) ||
			(CommandFlags &= INCOMING_SMS_MODEM) )
		{
			StatusFlags &= ~MODEM_BUSY;
			CaseTaskModem = 10;

			break;
		}

		//wait to ask command 500 msec
		xLastWakeTime = xTaskGetTickCount();
		vTaskDelayUntil( &xLastWakeTime, 500/portTICK_RATE_MS );

		strcpy(TxCommandToModem.ptrUART_Buf, CCLK);

		TxRxCommand(OK_, &QueueCMD);

		if( QueueCMD.MessageID != MODEM_NO_DATA )
			CurrentDateTime = DecodeDateTime( &QueueCMD.ptrUART_Buf[ StrIndex(QueueCMD.ptrUART_Buf, ':')+1 ], true);

		StatusFlags &= ~MODEM_BUSY;
		CaseTaskModem = 10;

	break;
	//-----------------------------------------------------------------------

	//Gate Modality: Activate an OUT for x time
	//-----------------------------------------------------------------------
	case 100:
	//-----------------------------------------------------------------------

		eeprom_read_block( &GateConfig, &GateMode, sizeof(GATE));
		Mask = 1;
		Mask = Mask << GateConfig.Out;

		if (!GateConfig.Delay)
		{
			//Only toggle the out
			if(PORTB & Mask)
				PORTB &= ~Mask;
			else
				PORTB |= Mask;

			StatusFlags &= ~MODEM_BUSY;
			CaseTaskModem = 10;
		}
		else
		{
			// ON the correct out
			PORTB |= Mask;
			CaseTaskModem++;
		}

	break;
	//-----------------------------------------------------------------------

	//-----------------------------------------------------------------------
	case 101:
	//-----------------------------------------------------------------------

		//Delay
		xLastWakeTime = xTaskGetTickCount();
		vTaskDelayUntil( &xLastWakeTime, (GateConfig.Delay*10)/portTICK_RATE_MS );

		Mask = 1;
		Mask = Mask << GateConfig.Out;

		// OFF the correct out
		PORTB &= ~Mask;

		StatusFlags &= ~MODEM_BUSY;
		CaseTaskModem = 10;

	break;
	//-----------------------------------------------------------------------

	//Toggle ring Modality: Activate an OUT1, out 2 for x time alternatively
	//-----------------------------------------------------------------------
	case 110:
	//-----------------------------------------------------------------------

		eeprom_read_block( &ToggleRingConfig, &ToggleRingMode, sizeof(TOGGLERING));
		Mask = 1;
		Err  = 1; //used as TMP variable

		if( !NumRingToggleRingMode )
		{
			Mask =   Mask << ToggleRingConfig.Out1;
			//to set = 0 out for security
			Err  = ~(Err  << ToggleRingConfig.Out2);
		}
		else
		{
			Mask =   Mask << ToggleRingConfig.Out2;
			//to set = 0 out for security
			Err  = ~(Err  << ToggleRingConfig.Out1);
		}

		// ON the correct out
		PORTB &= Err;
		PORTB |= Mask;

		Err = 0;

		CaseTaskModem++;

	break;
	//-----------------------------------------------------------------------

	//-----------------------------------------------------------------------
	case 111:
	//-----------------------------------------------------------------------

		//Delay
		xLastWakeTime = xTaskGetTickCount();
		vTaskDelayUntil( &xLastWakeTime, (ToggleRingConfig.Delay*10)/portTICK_RATE_MS );

		Mask = 1;
		if( !NumRingToggleRingMode )
		{
			Mask = Mask << ToggleRingConfig.Out1;
			NumRingToggleRingMode++;
		}
		else
		{
			Mask = Mask << ToggleRingConfig.Out2;
			NumRingToggleRingMode = 0;
		}

		// OFF the correct out
		PORTB &= ~Mask;

		StatusFlags &= ~MODEM_BUSY;
		CaseTaskModem = 10;

	break;
	//-----------------------------------------------------------------------

	//-----------------------------------------------------------------------
	}// end switch

	//only to permit switch context. IS NOT PREEMPITIVE
	taskYIELD();
 }//END WHILE
}
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

// ----------------------------------------------------------------------------------------------------------------------------------------------------------
//Find index
// ----------------------------------------------------------------------------------------------------------------------------------------------------------
int DecodeCMTI(char *Buf)
{

	char *temp;
	int length, i;

	//init
	length = 0;

	temp = strstr(Buf, "\r\nOK\r\n");//Trim off trailing OK\r\n
	*temp=0;

	temp = Buf;
	length = strlen(temp);

	for( i = 0; ( i < length ) && ( *temp++ != ',' ); i++)    //Loop until we get a ','
    {
        ;
    }

	//Error
	if( i >= length )
	{
		return 0;
	}

	else
	{
		return atoi( temp );                             //Convert ascii int to integer
	}
}
// ----------------------------------------------------------------------------------------------------------------------------------------------------------
//Read new message, and decode PDU FORMAT
// ----------------------------------------------------------------------------------------------------------------------------------------------------------
bool ReadMSG( char *Buf )
{
 char *EncodedMSG;
 int  Len,i, LenPDU;
 bool MoreSMS = false;
 char UDHI;


	//erase first \r\n
	Buf += 2;

	// Get the correct pointer to message
	EncodedMSG     = strstr(Buf, "\r\nOK\r\n");//Trim off trailing \r\nOK\r\n
	*EncodedMSG    = 0;

	//look for first \n
	EncodedMSG     = strstr(Buf, "\n");

	if(!EncodedMSG)
		return false;

	EncodedMSG++; //next char
	//Lenght of SMSC
	Len           = 16*htoi( *EncodedMSG++ );
	Len          += htoi( *EncodedMSG++ );
	Len  *=2;

	//Verify UDHI: if PDU has header information for concatenated sms
	EncodedMSG   += Len;

	UDHI         = 16*htoi( *EncodedMSG++ );
	UDHI         +=   htoi( *EncodedMSG++ );

	if(UDHI & BIT_6)
		//PDU has other Header
		MoreSMS = true;

	Len          = 16* htoi( *EncodedMSG++ );    //sender phone length
	Len          +=     htoi( *EncodedMSG++ );    //sender phone length

	if( Len % 2 )
		Len++;                                        //Check for trailing F

	//check number type
	i           = 16* htoi( *EncodedMSG++ );    //sender phone length
	i          +=     htoi( *EncodedMSG++ );    //sender phone length

	//prt to phone number;
	// EncodedMSG += 2;

	//First read correct Number Phone;
	memset(PhoneNr, 0, MAX_PHONE_NUMBER_SIZE);

	if (i == 0x91)
	{//international number
		PhoneNr[0] = '+';
		for( i=0 ; i<Len; i+=2)
		{
			if(EncodedMSG [ i ] != 'F')
			{
				PhoneNr[ i + 1 ] = EncodedMSG [ i + 1 ];
				PhoneNr[ i + 2 ] = EncodedMSG [ i     ];
			}
			else
				PhoneNr[ i + 1 ] = EncodedMSG [ i + 1 ];
		}
	}
	else
	{  //national number
		for( i=0 ; i<Len; i+=2)
		{
			if(EncodedMSG [ i ] != 'F')
			{
				PhoneNr[ i     ] = EncodedMSG [ i + 1 ];
				PhoneNr[ i + 1 ] = EncodedMSG [ i     ];
			}
			else
				PhoneNr[ i     ] = EncodedMSG [ i + 1 ];
		}
	}

	EncodedMSG   += Len;                             //Skip fields in header
	EncodedMSG   += (14 + 4);                        //Skip more fields...TP-PID, TP-DCS and TP-SCTS

	LenPDU  = 16 * htoi( *EncodedMSG++ );         //in septet (1setpet = char)
	LenPDU +=      htoi( *EncodedMSG++ );

	char IdA;
	char LenA;
	char NumTotSms;
	char Seq;
	char RefNr;
	int Offset, NBits;

	//CONCATENATED SMS
	if(MoreSMS)
	{
		Len  = 16 * htoi( *EncodedMSG++ );  //length of next header
		Len +=      htoi( *EncodedMSG++ );  //Octects

		//check for padding bits
		NBits = ((Len + 1)*8);
		Offset = (((NBits/7)+1)*7-NBits)%7;     /* Fill bits for UDH to septet boundary */

		IdA   = 16 * htoi( EncodedMSG[0] ); //Id A
		IdA  +=      htoi( EncodedMSG[1] );

		if(IdA != 0 )
			return false;  // not supported other than concatenated SMS

		LenA  = 16 * htoi( EncodedMSG[2] );		//length A
		LenA +=      htoi( EncodedMSG[3] );     //octects


		if( LenA + 1 + 1 != Len )	// Lenght IdA + 1 octect ID + 1 octect Header Lenght
			return false;           // not supported more than one ID

		if( LenA != 3 )				// 3 octect
			return false;           // not supported other than concatenated SMS


		RefNr  = 16 * htoi( EncodedMSG[4] ); //Reference SMS number
		RefNr +=      htoi( EncodedMSG[5] );

		NumTotSms  = 16 * htoi( EncodedMSG[6] ); //tot concatenated SMS Number
		NumTotSms +=      htoi( EncodedMSG[7] );

		Seq  = 16 * htoi( EncodedMSG[8] ); //tot concatenated SMS sequence
		Seq +=      htoi( EncodedMSG[9] );

		if( Seq       > MAX_NUM_SMS ||
			NumTotSms > MAX_NUM_SMS )
			return false;             //not supported more than MAX_NUM_SMS concatenated

		// (LenPDU*7 - (   Len+1 + (Len+1)%7 ) *8 )/7; //lenght message in septets

		EncodedMSG += (Len * 2); //ptr to pdu message

		if(!(Len = SMS_GSMDecode( (unsigned char *)EncodedMSG, (unsigned char *)(SmsListRx[Seq-1].Str), Offset)) )
			return false;

		if( Len > SINGLE_SMS_BUFFER_SIZE )
			return false;        // some trouble: impossible to have length > 160

		//Compiling message
		SmsListRx[Seq-1].IndexSMS   = Seq;
		SmsListRx[Seq-1].Identifier = RefNr;
		SmsListRx[Seq-1].TotSMS     = NumTotSms;
		SmsListRx[Seq-1].Lenght     = Len;


		// check if END of SMS conctenated received
		RefNr = SmsListRx[0].Identifier;
		for(i=1 ; i< NumTotSms; i++)
		{
			if( SmsListRx[i].Identifier != RefNr )
				return false;
		}
		//All the same Id: Assemble Message
		memset(SMSRxBuf, 0, SMS_ASCII_BUFFER_SIZE);

		for(i = 0, Len = 0; i< NumTotSms; i++)
		{
			memcpy( &(SMSRxBuf[Len]), SmsListRx[i].Str, SmsListRx[i].Lenght);
			Len += SmsListRx[i].Lenght;
		}
		return true;
	}
	// SINGLE SMS
	else
	{
		//Decompress this message
		memset(SMSRxBuf, 0, SMS_ASCII_BUFFER_SIZE);
		if( !SMS_GSMDecode( (unsigned char *)EncodedMSG, (unsigned char *)SMSRxBuf, 0 ) )
			return false;
	}

	return true;
}
// ----------------------------------------------------------------------------------------------------------------------------------------------------------
// SMS MESSAGES SEND: call this function till true
// returns true when done false not yet finished
// Err = see coded errors
// Msg is the array to send. Max char are SMS_ASCII_BUFFER_SIZE and can be 3 SMS
// ----------------------------------------------------------------------------------------------------------------------------------------------------------
// We use SMSRxBuf as temporary buffer!!!!
// ----------------------------------------------------------------------------------------------------------------------------------------------------------
bool SMSSend(char *Msg, char *Phone, char *Err )
{
 int                    NChars, Len, i, j, k, PhoneLen, Padding;
 unsigned char          Src[5];
 char                   UDHL;
 UARTMessage			QueueCMD;
 static unsigned char   payload_len_c[3];
 static unsigned char   NrSMS;
 static unsigned char   RefSMS;
 static int             Index;
 static unsigned char	CaseSMSSend = 0;

	//Temporary buffer
	char Buf[UART_TX_BUFFER_SIZE];

	*Err = OK;

	switch(CaseSMSSend)
	{

		// --------------------------------------------------------------------
		case 0:
			payload_len_c[0] = 0;

			//Verify how much SMS I need
			Len = strlen(Msg);

			if( Len <= SINGLE_SMS_BUFFER_SIZE)
				NrSMS = 1;
			else
			{
				NrSMS = Len / (SINGLE_SMS_BUFFER_SIZE - 7); // in concatenated sms max is 160-7 char due to additional header
				if( (Len % SINGLE_SMS_BUFFER_SIZE - 7) )
					NrSMS++;
			}

			if( NrSMS > MAX_NUM_SMS)
			{
				*Err = SMS_TOO_LONG;
				CaseSMSSend = 0;
				return true;
			}

			//Get Ref SMS number from timer 1 low byte
			RefSMS = TCNT1L;

			//Generate list of SMS
			Index = 0;
			j = NrSMS;
			k = 0;

			while(j--)
			{
				SmsListTx[Index].IndexSMS   = Index+1;    //sequence SMS
				SmsListTx[Index].Identifier = RefSMS; //reference SMS
				SmsListTx[Index].TotSMS     = NrSMS;  //total SMS

				if( NrSMS > 1 )
				{
					if( j )
						SmsListTx[Index].Lenght   = SINGLE_SMS_BUFFER_SIZE - 7;        //Lenght
					else
						SmsListTx[Index].Lenght   = Len % (SINGLE_SMS_BUFFER_SIZE - 7);  //Lenght
				}
				else
					SmsListTx[Index].Lenght   = Len;

				memset( SmsListTx[Index].Str, 0      , SINGLE_SMS_BUFFER_SIZE);
				memcpy( SmsListTx[Index].Str, &Msg[k], SmsListTx[Index].Lenght   );

				CharToGSM_7( (unsigned char *)SmsListTx[Index].Str );

				k += SmsListTx[Index].Lenght;
				Index++;
			}

			CaseSMSSend = 5;
			//prepare for cycle
			Index = 0;

	break;
	// --------------------------------------------------------------------

	// --------------------------------------------------------------------
	case 5:
		//Preparing command for sending SMS
		if(NrSMS > 1)
		{
			UDHL =  5 ; // 5 octect for user header
			Padding = ((UDHL + 1 ) * 8 ) % 7;

			if( Padding )
				Padding = 7 - Padding;
		}
		else
		{
			UDHL = 0;
			Padding = 0;
		}

		NChars = 0;

		memset(Buf, 0, UART_TX_BUFFER_SIZE);
		
		if( !SMS_GSMEncode(	SmsListTx[Index].Lenght,
							SmsListTx[Index].Str ,
							Padding,
							UART_TX_BUFFER_SIZE,
							(unsigned char*)Buf,
							&NChars))
		{
			*Err = ERROR_COMPRESSING;
			CaseSMSSend = 0;
			return true;
		}

		//convert in hex (NChars * 2 char)
		memset(SMSRxBuf,   0, SMS_ASCII_BUFFER_SIZE);
		strtoh( (int8_t *)Buf , (unsigned char *)SMSRxBuf, NChars  );

		if( *Phone == '+' )
			PhoneLen = strlen(Phone) - 1;
		else
			PhoneLen = strlen(Phone);

		//check if odd
		if( PhoneLen % 2 )
			Len = PhoneLen + 1;
		else
			Len = PhoneLen ;

		//calculate total septet payload
		if(NrSMS == 1)
		{
			k = SmsListTx[Index].Lenght;
			Len = 8 + Len/2 + NChars;
			//Calculate overall length in octects
		}
		else
		{
			k = SmsListTx[Index].Lenght + ( (UDHL + 1)*8 + Padding )/7;
			//Calculate overall length in octects
			Len = 8 + Len/2 + NChars + UDHL + 1;
		}

		//Convert Integer payload to hex in string format
		itoh(k, &payload_len_c[0]);

		//convert Len in ASCII
		itoa(Len, (char *)Src, 10);

		memset(TxCommandToModem.ptrUART_Buf,   0, UART_TX_BUFFER_SIZE);
		//Put Header
		strcat(TxCommandToModem.ptrUART_Buf, SMSSENDHEADR);
		//Put Len
		strcat(TxCommandToModem.ptrUART_Buf, (char *)Src);
		//Put \r\n
		strcat(TxCommandToModem.ptrUART_Buf, ENDSTRING);

		CaseSMSSend = 10;

		//reuse this variable as retry command
		RefSMS = 1;

	// --------------------------------------------------------------------
	break;

	// --------------------------------------------------------------------
	case 10:

		TxRxCommand(READY_, &QueueCMD);

		//no answer: retry 2 time if more SMS
		if(!QueueCMD.UART_Index && ( NrSMS == 1 || (NrSMS > 1 && RefSMS++ > 2) ))
		{
			*Err = NO_ANSWER_FROM_CMD;
			CaseSMSSend = 0;
			return true;
		}
		NChars = QueueCMD.UART_Index;
		
		if( NChars )
			CaseSMSSend = 20;

	// --------------------------------------------------------------------
	break;

	// --------------------------------------------------------------------
	case 20:
/*
    HEADER PDU
    00 	Length of SMSC information. Here the length is 0, which means that the SMSC stored in the phone should be used. Note: This octet is optional. On some phones this octet should be omitted! (Using the SMSC stored in phone is thus implicit)

    11 	First octet of the SMS-SUBMIT message.
        Bit no	7	    6	    5	    4	    3	    2	    1	    0
        Name	TP-RP	TP-UDHI	TP-SRR	TP-VPF	TP-VPF	TP-RD	TP-MTI	TP-MTI

        TP-RP	Reply path. Parameter indicating that reply path exists.
        TP-UDHI	User data header indicator. This bit is set to 1 if the User Data field starts with a header
        TP-SRR	Status report request. This bit is set to 1 if a status report is requested
        TP-VPF	Validity Period Format. Bit4 and Bit3 specify the TP-VP field according to this table:
            bit4 bit3
            0       0 : TP-VP field not present
            1       0 : TP-VP field present. Relative format (one octet)
            0       1 : TP-VP field present. Enhanced format (7 octets)
            1       1 : TP-VP field present. Absolute format (7 octets)

        TP-RD	Reject duplicates. Parameter indicating whether or not the SC shall accept an SMS-SUBMIT for an SM still held in the SC which has the same TP-MR and the same TP-DA as a previously submitted SM from the same OA.
        TP-MTI	Message type indicator. Bits no 1 and 0 are set to 0 and 1 respectively to indicate that this PDU is an SMS-SUBMIT

    00 	TP-Message-Reference. The "00" value here lets the phone set the message reference number itself.

    0B 	Address-Length. Length of phone number (11)

    91 	Type-of-Address. (91 indicates international format of the phone number).
        Bit no	7	            6   5	4	    3	2	1	0
        Name	Always set to 1	Type-of-number	Numbering Plan Identification

        Bits
        6 5 4	Meaning of the Type-of-number bits (6, 5 and 4)
        0 0 0	Unknown. This is used when the user or network has no a priori information about the numbering plan. In this case, the Address-Value field is organized according to the network dialling plan, e.g. prefix or escape digits might be present.
        0 0 1	International number.
        0 1 0	National number. Prefix or escape digits shall not be included.
        0 1 1	Network specific number. This is used to indicate administration/service number specific to the serving network, e.g. used to access an operator.
        1 0 0	Subscriber number. This is used when a specific short number representation is stored in one or more SCs as part of a higher layer application. (Note that "Subscriber number" shall only be used in connection with the proper PID referring to this application).
        1 0 1	Alphanumeric, (coded according to GSM TS 03.38 7-bit default alphabet)
        1 1 0	Abbreviated number
        1 1 1	Reserved for extension

        Bits
        3 2 1 0	Meaning of the Numbering Plan Identification bits (3, 2, 1 and 0)
        0 0 0 0	Unknown.
        0 0 0 1	ISDN/telephone numbering plan (E.164/E.163).
        0 0 1 1	Data numbering plan (X.121).
        0 1 0 0	Telex numbering plan
        1 0 0 0	National numbering plan
        1 0 0 1	Private numbering plan
        1 0 1 0	ERMES numbering plan (ETSI DE/PS 3 01-3)
        1 1 1 1	Reserved for extension

    xxx	The phone number in semi octets (46708251358). The length of the phone number is odd (11), therefore a trailing F has been added, as if the phone number were "46708251358F". Using the unknown format (i.e. the Type-of-Address 81 instead of 91) would yield the phone number octet sequence 7080523185 (0708251358). Note that this has the length 10 (A), which is even.

    00 	TP-PID. Protocol identifier

        Bit 7	Bit 6	Usage
            0	0	    Assigns bits 0..5 as defined below
            0	1	    Assigns bits 0..5 as defined below
            1	0	    Reserved
            1	1	    Assigns bits 0..5 for SC specific use

        In case where bits 7 and 6 both are 0:
        Bit 5	Description
        0	    no interworking, but SME-to-SME protocol
        1	    telematic interworking

        In the case of telematic interworking, the following five bit patterns in bits 4..0 are used to indicate types of telematic devices:
        Bits 4..0	    Description
        00000	        implicit - device type is specific to this SC, or can be concluded on the basis of the address
        00001	        telex (or teletex reduced to telex format)
        00010	        group 3 telefax
        00011	        group 4 telefax
        00100	        voice telephone (i.e. conversion to speech)
        00101	        ERMES (European Radio Messaging System)
        00110	        National Paging System (known to the SC)
        00111	        Videotex (T.100/T.101)
        01000	        teletex, carrier unspecified
        01001	        teletex, in PSPDN
        01010	        teletex, in CSPDN
        01011	        teletex, in analog PSTN
        01100	        teletex, in digital ISDN
        01101	        UCI (Universal Computer Interface, ETSI DE/PS 3 01-3)
        01110....01111	(reserved, 2 combinations)
        10000	        a message handling facility (known to the SC)
        10001	        any public X.400-based message handling system
        10010	        Internet Electronic Mail
        10011....10111	(reserved, 5 combinations)
        11000....11110	values specific to each SC, usage based on mutual agreement between the SME and the SC (7 combinations available for each SC)
        11111	        A GSM mobile station. The SC converts the SM from the received TP-DCS to any data coding scheme supported by that MS (e.g. the default).

        In the case where bit 7 = 0, bit 6 = 1, bits 5..0 are used as defined below
        Bits 5..0	    Description
        000000	        Short Message Type 0
        000001	        Replace Short Message Type 1
        000010	        Replace Short Message Type 2
        000011	        Replace Short Message Type 3
        000100	        Replace Short Message Type 4
        000101	        Replace Short Message Type 5
        000110	        Replace Short Message Type 6
        000111      	Replace Short Message Type 7
        001000..011110	Reserved
        011111	        Return Call Message
        100000..111100	Reserved
        111101	        ME Data download
        111110	        ME De-personalization Short MEssage
        111111	        SIM Data download

    00 	TP-DCS. Data coding scheme.This message is coded according to the 7bit default alphabet. Having "04" instead of "00" here, would indicate that the TP-User-Data field of this message should be interpreted as 8bit rather than 7bit (used in e.g. smart messaging, OTA provisioning etc).

        Coding Group Bits 7..4	Use of bits 3..0
        00xx	General Data Coding indication
            Bits 5..0 indicate the following:
                Bit 5
                    0	Text is uncompressed
                    1	Text is compressed

                Bit 4
                    0	Bits 1 and 0 are reserved and have no message class meaning
                    1	Bits 1 and 0 have a message class meaning

                Bit 3	Bit 2	Alphabet being used
                    0	0	    Default alphabet
                    0	1	    8 bit data
                    1	0	    UCS2 (16bit)
                    1	1	    Reserved

                Bit 1	Bit 0	Message class	Description
                    0	0	    Class 0	Immediate display (alert)
                    0	1	    Class 1	ME specific
                    1	0	    Class 2	SIM specific
                    1	1	    Class 3	TE specific
                    NOTE: The special case of bits 7..0 being 0000 0000 indicates the Default Alphabet as in Phase 2

        0100..1011	Reserved coding groups
        1100	    Message Waiting Indication Group: Discard Message
                     Bits 3..0 are coded exactly the same as Group 1101, however with bits 7..4 set to 1100 the mobile may discard the contents of the message, and only present the indication to the user.
        1101	    Message Waiting Indication Group: Store Message
                    This Group allows an indication to be provided to the user about status of types of message waiting on systems connected to the GSM PLMN. The mobile may present this indication as an icon on the screen, or other MMI indication. The mobile may take note of the Origination Address for message in this group and group 1100. For each indication supported, the mobile may provide storage for the Origination Address which is to control the mobile indication.
                    Text included in the user data is coded in the Default Alphabet.
                    here a message is received with bits 7..4 set to 1101, the mobile shall store the text of the SMS message in addition to setting the indication.
                    Bit 3	Description
                        0	Set Indication Inactive
                        1	Set Indication Active

                    Bit 2 is reserved, and set to 0

                    Bit 1	Bit 0	Indication Type
                        0	    0	Voicemail Message Waiting
                        0	    1	Fax Message Waiting
                        1	    0	Electronic Mail Message Waiting
                        1	    1	Other Message Waiting*

                    * Mobile manufacturers may implement the "Other Message Waiting" indication as an additional indication without specifying the meaning. The meaning of this indication is intended to be standardized in the future, so Operators should not make use of this indication until the standard for this indication is finalized.
        1110	    Message Waiting Indication Group: Store Message
                    The coding of bits 3..0 and functionality of this feature are the same as for the Message Waiting Indication Group above, (bits 7..4 set to 1101) with the exception that the text included in the user data is coded in the uncompressed UCS2 alphabet.
        1111	    Data coding/message class
                    Bit 3 is reserved, set to 0.
                    Bit 2	Message coding
                        0	Default alphabet
                        1	8-bit data

                    Bit 1	Bit 0	Message Class	Description
                        0	    0	Class 0	Immediate display (alert)
                        0	    1	Class 1	ME specific
                        1	    0	Class 2	SIM specific
                        1	    1	Class 3	TE specific

    AA 	TP-Validity-Period. "AA" means 4 days. Note: This octet is optional, see bits 4 and 3 of the first octet
        0 to 143	(TP-VP + 1) * 5 minutes (i.e. 5 minutes intervals up to 12 hours)
        144 to 167	12 hours + ((TP-VP - 143) * 30 minutes)
        168 to 196	(TP-VP - 166) * 1 day
        197 to 255	(TP-VP - 192) * 1 week

    0A 	TP-User-Data-Length. Length of message. The TP-DCS field indicated 7-bit data, so the length here is the number of septets (10). If the TP-DCS field were set to 8-bit data or Unicode, the length would be the number of octets.

    xxx	TP-User-Data. These octets represent the message "hellohello". How to do the transformation from 7bit septets into octets is shown here

*/
		memset(TxCommandToModem.ptrUART_Buf, 0, UART_TX_BUFFER_SIZE);

		if(NrSMS == 1)
			strcat(TxCommandToModem.ptrUART_Buf, "001100" );
		else
			strcat(TxCommandToModem.ptrUART_Buf, "005100" );

		if( *Phone == '+' )
		{
			Phone++;
			TxCommandToModem.ptrUART_Buf[8] = '9'; //international number (0x91)
			TxCommandToModem.ptrUART_Buf[9] = '1';
		}
		else
		{
			TxCommandToModem.ptrUART_Buf[8] = '8'; //national number with unknown format
			TxCommandToModem.ptrUART_Buf[9] = '1';
		}

		PhoneLen = strlen(Phone);
		//check if odd
		if( PhoneLen % 2 )
			Len = PhoneLen + 1;
		else
			Len = PhoneLen ;

		itoh( Len, Src);

		TxCommandToModem.ptrUART_Buf[6] = Src[0]; //national number with unknown format
		TxCommandToModem.ptrUART_Buf[7] = Src[1];

		i = 10;

		//char total
		j = 10 + 6 + Len +
			strlen( (char *)payload_len_c) +
			strlen(SMSRxBuf);

		if(NrSMS >1 )
			j += 6;

		if(UART_TX_BUFFER_SIZE < j)
		{
			//no space in buffer
			//send abort
			*Err = SMS_TOO_LONG;
			CaseSMSSend = 40;

			break;
		}

		for( j=0; j<PhoneLen; j+=2, i+=2 )
		{

			TxCommandToModem.ptrUART_Buf[ i + 1 ] = Phone[ j ];

			if( j+1 < PhoneLen )
				TxCommandToModem.ptrUART_Buf[ i ] = Phone[ j + 1 ];
			else
			{
				//Insert trailing F
				if( PhoneLen % 2 )
					TxCommandToModem.ptrUART_Buf[ i ] = 'F';
			}
		}

		//TP-PID TP-DCS + TP-Val (AA = 4 days)
		strcat(TxCommandToModem.ptrUART_Buf, "0000AA");
		strcat(TxCommandToModem.ptrUART_Buf, (char *)payload_len_c);

		if(NrSMS >1 )
		{
			//Insert User Header
			strcat(TxCommandToModem.ptrUART_Buf, "050003"); //05 octect 00 sms concatenated 03 len IdA

			itoh( SmsListTx[Index].Identifier, payload_len_c); // identificer 1 octect
			strcat(TxCommandToModem.ptrUART_Buf, (char *)payload_len_c);

			itoh( SmsListTx[Index].TotSMS, payload_len_c); // Tot sms 1 octect
			strcat(TxCommandToModem.ptrUART_Buf, (char *)payload_len_c);

			itoh( SmsListTx[Index].IndexSMS, payload_len_c); // sequence sms 1 octect
			strcat(TxCommandToModem.ptrUART_Buf, (char *)payload_len_c);

		}

		strcat(TxCommandToModem.ptrUART_Buf, (char *)SMSRxBuf);
		strcat(TxCommandToModem.ptrUART_Buf, ENDCMDSMS);

		//reset message in buffer for security
		memset(SMSRxBuf, 0, SMS_ASCII_BUFFER_SIZE);

		CaseSMSSend = 30;
		
		//reuse this variable as retry command
		RefSMS = 1;

	// --------------------------------------------------------------------
	break;

	// --------------------------------------------------------------------
	case 30:

		//wait answer OK
		TxRxCommand(OK_, &QueueCMD);

		NChars = QueueCMD.UART_Index;
		
		if( !NChars )
		{
			//no answer: retry 2 time if more SMS
			if( NrSMS == 1 || ( NrSMS > 1 && RefSMS++ > 2 ))
			{
				*Err = NO_ANSWER_FROM_CMD;
				if( !CmpSubStr( QueueCMD.ptrUART_Buf , "ERROR" ) )
				{
					//send abort command
					CaseSMSSend = 40;
				}
				else
				{
					CaseSMSSend = 0;
					return true;
				}
			}
			break;
		}
		else
		{
			if(Index+1 >= NrSMS )
			{
				CaseSMSSend = 0;
				return true;
			}
			else
			{
				CaseSMSSend = 5;
				Index++;
			}
		}

	break;
	// --------------------------------------------------------------------

	// --------------------------------------------------------------------
	case 40:

		//send abort command
		strcpy(TxCommandToModem.ptrUART_Buf, ESC);
		TxRxCommand(OK_, &QueueCMD);

		CaseSMSSend = 0;

		return true;

	// --------------------------------------------------------------------
	}

 return false;
}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------
// read SMS center
/// Returns:
///		true = Finished without error
// ----------------------------------------------------------------------------------------------------------------------------------------------------------
bool ReadCreditCentreFromSIM( void )
{
  unsigned int	Idx;
  UARTMessage	QueueCMD;
  char 			TmpBuf[MAX_PHONE_NUMBER_SIZE];
  char			*PtrBuf;

	memset(TxCommandToModem.ptrUART_Buf, 0, UART_TX_BUFFER_SIZE);
	sprintf(TxCommandToModem.ptrUART_Buf, FIND_BOOK_ENTRY, "\"CREDIT\"" );

	TxRxCommand(OK_, &QueueCMD);

	if(QueueCMD.MessageID != DATA_READY_ID)
		return false;

	//filter ECHO + response id
	Idx  = StrIndex( QueueCMD.ptrUART_Buf, '"'); //1 for first

	if( !Idx )
	{
		//Error
		return false;
	}

	PtrBuf = &(QueueCMD.ptrUART_Buf[Idx]);

	Idx  = StrIndex( PtrBuf, '"');

	PtrBuf[Idx-1] = 0; //cut the string '"'

	//cut + inserted eventually by modem if number starts with #
	if (PtrBuf[0] == '+' && PtrBuf[1] == '*' )
		PtrBuf++;

	//Phone N°: write only if not equal
	memset(TmpBuf, 0, MAX_PHONE_NUMBER_SIZE);

	eeprom_read_block (TmpBuf, &TelCreditService, strlen(PtrBuf) );

	if( strcmp( TmpBuf, PtrBuf) )
	{
		eeprom_write_block (PtrBuf, &TelCreditService, strlen(PtrBuf) + 1 );
	}

	return true;
}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------
// Read phone book
/// Returns:
///		true = Finished without error
// ----------------------------------------------------------------------------------------------------------------------------------------------------------
bool ReadBookFromSIM(void)
{
  char *Buf, TmpChar;
  unsigned int Idx, Idy, Idz;
  int i;
  UARTMessage	QueueCMD;
  int  IniIndex, EndIndex;
  bool DontUseIt = false;
  NPHONES TmpPhone;

	memset( &TmpPhone, 0, sizeof(NPHONES) );

	i = 0;

	for(IniIndex  = 1, EndIndex = 5;
		EndIndex <= 250;
		IniIndex  = EndIndex + 1, EndIndex += 5 )
	{

		memset(TxCommandToModem.ptrUART_Buf, 0, UART_TX_BUFFER_SIZE);
		sprintf(TxCommandToModem.ptrUART_Buf, READ_BOOK, IniIndex, EndIndex );  //Read Book

		TxRxCommand(OK_, &QueueCMD);

		if( !QueueCMD.UART_Index )
			return false;

		if( QueueCMD.UART_Index == 6 &&  CmpSubStr( strlwr(QueueCMD.ptrUART_Buf) , "ok" ) )  // only response OK without numbers
			continue;

		//filter ECHO + first response
		Idx  = StrIndex(QueueCMD.ptrUART_Buf, ':'); //1 for first
		Buf = &(QueueCMD.ptrUART_Buf[Idx]); //ptr to ' '

		//Prepare phone numbers
		while(*Buf)
		{
			//Search for CREDIT phone and exclude it
			Idy  =  StrIndex(Buf, '"');        //first "  (phone Nr.)
			Idy  += StrIndex( &Buf[Idy], '"'); //second " (end phone)
			Idy  += StrIndex( &Buf[Idy], '"'); //third "  (Name)
			Idz  =  Idy + StrIndex( &Buf[Idy], '"'); //fourth " (end Name)

			if(Idy && Idz)
			{
				//memorize char
				TmpChar = Buf[Idz-1];
				Buf[Idz-1] = 0; // set end string
				if( !strcmp( strlwr(&Buf[Idy]), "credit") )
				{
					//found credit tel and discard it
					DontUseIt = true;
				}
				//restore char
				Buf[Idz-1] = TmpChar;
			}

			Idx  = StrIndex(Buf, ','); //1 for first
			if( !Idx && !i)
			{
				//Error
				return false;
			}

			if(!Idx)
				break;

			Buf[Idx-1] = 0; //cut the string

			//Id Phone
			TmpPhone.ID = (char)atoi(Buf);

			Buf += Idx + 1;
			Idx  = StrIndex(Buf, '"'); //1 for first

			if( !Idx )
			{
				//Error
				return false;
			}

			Buf[Idx-1] = 0; //cut the string '"'

			if(!DontUseIt)
			{
				//Phone N°: write only if not equal
				strcpy(TmpPhone.PhoneNr, Buf);
				eeprom_update_block (&TmpPhone, &(Phones[i]), sizeof(TmpPhone) );
				i++;
			}

			Buf      += Idx; //go to next
			Idx       = StrIndex(Buf, ':'); //next number
			DontUseIt = false;

			if( i >= NUM_MAX_PHONES )
				return true;

			if( !Idx )
				break;

			Buf += Idx;

		}
	}

	//erase eventually others numbers in EEPROM
	for( ; i < NUM_MAX_PHONES; i++)
	{
		memset(&TmpPhone, 0, sizeof(NPHONES) );
		eeprom_update_block (&TmpPhone, &(Phones[i]), sizeof(TmpPhone) );
	}

	return true;
}
// ----------------------------------------------------------------------------------------------------------------------------------------------------------
// Index starts: 0
// ----------------------------------------------------------------------------------------------------------------------------------------------------------
int StrLastIndex(char *InBuf, char c)
{
	int Iter;

	for( Iter = strlen(InBuf) - 1 ; Iter >= 0 ;  Iter-- )
	{
		if( InBuf[Iter] == c )
			return Iter;
	}
	return -1;
}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------
// Index starts: 1
// ----------------------------------------------------------------------------------------------------------------------------------------------------------
int StrIndex(char *InBuf, char c)
{
	int Index = 0;

	while(*InBuf)
	{
		Index++;

		if( *InBuf == c )
			return Index;
		else
			InBuf++;
	}
	return 0;
}
// ----------------------------------------------------------------------------------------------------------------------------------------------------------
// compare string to an sub string true if yes
// ----------------------------------------------------------------------------------------------------------------------------------------------------------
bool CmpSubStr(char *Buf, char *Str)
{
	char *MemStr = Str;

	while(*Buf)
	{
		if(*Buf == *Str)
		{
			if( *Buf++ != *Str++)
				return false;

			if(!*Str)
				return true;
		}
		else
		{
			Buf++;
			Str = MemStr;
		}
	}
	return false;
}
// ----------------------------------------------------------------------------------------------------------------------------------------------------------
bool SendPowerOn(bool WithPowerOff)
{
  static char	CasePowerOn = 0;
  portTickType	xLastWakeTime;

  switch(CasePowerOn){

    // -------------------------------------------------------------------------
	case 0:

		if( WithPowerOff )
		{
			//power off modem
			PORTC &= ~_BV(O_GsmOnOff); // =0

			xLastWakeTime = xTaskGetTickCount();
			//SUSPEND task for 1.5 sec. Time to power off Telit
			vTaskDelayUntil( &xLastWakeTime, 1500/portTICK_RATE_MS );
	  }

	   CasePowerOn++;
	   break;

    // -------------------------------------------------------------------------
	case 1:

		// Send power on to GSM
		PORTC |= _BV(O_GsmOnOff);

		xLastWakeTime = xTaskGetTickCount();
		//SUSPEND task for 1.5 sec. Time to power off Telit
		vTaskDelayUntil( &xLastWakeTime, 1500/portTICK_RATE_MS );
	    CasePowerOn++;

     break;

    // -------------------------------------------------------------------------
   case 2:

	  if( PIND & _BV(I_GsmPowerMonitor) )
	  {
        CasePowerOn = 0;
		return true;
      }
	  else
	  {
		//try again: force power off
		PORTC &= ~_BV(O_GsmOnOff); // =0

		xLastWakeTime = xTaskGetTickCount();
		//SUSPEND task for 1.5 sec. Time to power off Telit
		vTaskDelayUntil( &xLastWakeTime, 1500/portTICK_RATE_MS );

		CasePowerOn = 1;

		return false;
		}

/* if main power id not battery with power off command Telit make only a reset
*/
	}
	return false;
}
// ----------------------------------------------------------------------------------------------------------------------------------------------------------
// call this function till return true
// Send SMS with call confirm
// char *String: string to send
// char *Err: returned error
// return value: true if done or error
// ----------------------------------------------------------------------------------------------------------------------------------------------------------
bool  SMSSendWithConfirm(char *String, char *Err)
{
	UARTMessage					QueueCMD;
	static unsigned char		CaseBuf=0;
	static unsigned char		IdxCurrentPhone;
	static portTickType			xCurrentTime;

	*Err = OK;

	switch (CaseBuf)
	{

		// ---------------------------------------------------------------------
		case 0:
			IdxCurrentPhone = 0;

			//wait 0.5 seconds
			xCurrentTime = xTaskGetTickCount();
			vTaskDelayUntil( &xCurrentTime, 500/portTICK_RATE_MS );

			memset(PhoneNr, 0, MAX_PHONE_NUMBER_SIZE);

			eeprom_read_block(	PhoneNr,
								&(Phones[IdxCurrentPhone].PhoneNr),
								sizeof(Phones[IdxCurrentPhone].PhoneNr));

			memset(TxCommandToModem.ptrUART_Buf, 0, UART_TX_BUFFER_SIZE);

			if(strlen(PhoneNr)>10)
			{
				sprintf(TxCommandToModem.ptrUART_Buf,"%s %s;%s",DIAL_TO_NUM, PhoneNr, ENDSTRING);
				CaseBuf++;
			}
			else
			{
				//end phone book
				*Err = NO_PHONE_AVAILABLE;
				CaseBuf = 0;
				return true;
			}

		break;
		// ---------------------------------------------------------------------

		// ---------------------------------------------------------------------
		case 2:
		// Call to number

			//command for dialling
			TxRxCommand(OK_, &QueueCMD);

			if( QueueCMD.MessageID == MODEM_NO_DATA )
			{
				*Err = NO_ANSWER_FROM_CMD;
				CaseBuf = 0;
				return true;
			}

			//wait 0.5 seconds
			xCurrentTime = xTaskGetTickCount();
			vTaskDelayUntil( &xCurrentTime, 500/portTICK_RATE_MS );

			//memorize actual time (static var)
			xCurrentTime = xTaskGetTickCount();

			CaseBuf++;

		break;
		// ---------------------------------------------------------------------

		// ---------------------------------------------------------------------
		case 3:

			//check CALL in progress
			strcpy(TxCommandToModem.ptrUART_Buf, "AT+CPAS\r");
			TxRxCommand(OK_, &QueueCMD);

			if( QueueCMD.MessageID != MODEM_NO_DATA )
			{
				if( CmpSubStr(QueueCMD.ptrUART_Buf, "CPAS") )
				{
					if( QueueCMD.ptrUART_Buf[9] == '4' )
					{	//call accepted
						//Received information for outgoing call
						//-----------------------
						CaseBuf = 20;
						break;
					}
				}

				if( QueueCMD.MessageID == BUSY_ID )
				{
					//Received BUSY from a remote phone
					//--	---------------------------------------------------------------------
					CaseBuf = 10;
					break;
				}

				if( QueueCMD.MessageID == NANSWER_ID ||  QueueCMD.MessageID == NCARRIER_ID )
				{
					//Received  no carrier or no answer from a remote phone
					//-----------------------------------------------------------------------
					CaseBuf = 11;
					break;
				}
			}
			else
			{
				//no answer from command
				//retry it
				//-----------------------------------------------------------------------
				CaseBuf = 2;
				break;
			}

			// WARNING: portTICK_RATE_MS = 1000/configTICK_RATE_HZ = 1 in this specific case
			// portTickType for this CPU is integer (see portmacro.h) then 16BIT
			// MAXIMUM VALUE IS 65535 TICS = 65,5 SECONDS

			// try this command for 60 seconds.
			if( IsTimeOver( 60000/portTICK_RATE_MS, xCurrentTime ) )
			{
				CaseBuf = 11;
			}

		break;
		// ---------------------------------------------------------------------

		// ---------------------------------------------------------------------
		case 10:

			// Send SMS
			if( !SMSSend( String, PhoneNr, Err ) )
				break;

			CaseBuf = 0;
			// check err in calling function
			return true;

		// ---------------------------------------------------------------------
		case 11:

			IdxCurrentPhone++;

			if(IdxCurrentPhone < NUM_MAX_PHONES)
			{
				CaseBuf = 1;
			}
			else
			{
				*Err = NO_PHONE_AVAILABLE;
				CaseBuf = 0;
				return true;
			}

		break;
		// ---------------------------------------------------------------------

		// ---------------------------------------------------------------------
		case 20: //HANG UP

			strcpy(TxCommandToModem.ptrUART_Buf, HANG_UP);

			// no check error
			TxRxCommand(OK_, &QueueCMD);

			CaseBuf = 10;

		break;
		// ---------------------------------------------------------------------

	}

return false;
}
// ----------------------------------------------------------------------------------------------------------------------------------------------------------
// Ask for net registration
// Return true when modem has answered.
// Err could be OK or NO_NETWORK_AVAILABLE
// ----------------------------------------------------------------------------------------------------------------------------------------------------------
bool AskForNetOK(char *Err)
{
	UARTMessage	QueueCMD;

	*Err = OK;

	strcpy(TxCommandToModem.ptrUART_Buf, ASK_NET_OK);

	TxRxCommand(OK_, &QueueCMD);

/* Value index 11 return strinf from modem
  0 - not registered, ME is not currently searching a new operator to register to
  1 - registered, home network
  2 - not registered, but ME is currently searching a new operator to register to
  3 - registration denied
  4 - unknown
  5 - registered, roaming
*/
	///modem answers to command before to send any else message
	if(	QueueCMD.MessageID == DATA_READY_ID
		&&
		(	QueueCMD.ptrUART_Buf[11] == '1' ||
			QueueCMD.ptrUART_Buf[11] == '5' ||
			QueueCMD.ptrUART_Buf[11] == '4'
		)
	   )
		return true;
	
	if ( QueueCMD.MessageID == DATA_READY_ID )
		
		*Err = NO_NETWORK_AVAILABLE;	
	else
		*Err = NO_ANSWER_FROM_CMD;

	return false;
}
// ----------------------------------------------------------------------------------------------------------------------------------------------------------
/*
 Call this function till true;
 check Err	after returned true:
 DestString	: destination string used try to send a call to service number to receive a message for credit
*/
// ----------------------------------------------------------------------------------------------------------------------------------------------------------
bool GetCredit	( char *DestString, char *Err )
{

 UARTMessage 	QueueCMD;
 char			TelService[MAX_PHONE_NUMBER_SIZE];
 char			Credit[MAX_STRING_LEN_SMS_CREDIT]; //Current Valid Phone number to ask for credit
 static char	CaseGetCredit = 0;

 *Err = OK;

 switch (CaseGetCredit) 
 {
	// --------------------------------------------------------------------------
	case 0:
		
		eeprom_read_block (TelService, &TelCreditService, sizeof(TelService) );
		eeprom_read_block (Credit, &SMSCredit, sizeof(Credit) );

		if( strlen(TelService) <= 0 )
		{
			*Err = NO_PHONE_AVAILABLE;
			return true;
		}

		switch(WayForCredit) 
		{
			//-------------------------------------------------------------------------------------------
			case CALL_TO_NUMBER_AND_WAIT_SMS:

				CaseGetCredit = 10;
		
			break;
			// --------------------------------------------------------------------------

			// --------------------------------------------------------------------------
			case SMS_TO_NUMBER_AND_WAIT_SMS:

				CaseGetCredit = 20;
		
			break;
			// --------------------------------------------------------------------------

			// --------------------------------------------------------------------------
			case UNSTRUCURED_SERVICE_DATA:

				CaseGetCredit = 20;
		
			break;
			// --------------------------------------------------------------------------
		}
	
	break;			
	// --------------------------------------------------------------------------

	// --------------------------------------------------------------------------
	// CALL_TO_NUMBER_AND_WAIT_SMS
	case 10:
		//Phone call to SMS center
		memset(TxCommandToModem.ptrUART_Buf, 0, UART_TX_BUFFER_SIZE);
		sprintf(TxCommandToModem.ptrUART_Buf,"%s %s;%s",DIAL_TO_NUM, TelService, ENDSTRING);

		// Call to number: No OK_ answer (no answer or busy)
		// we will wait a SMS
		TxRxCommand(OK_, &QueueCMD);

		if( QueueCMD.MessageID == MODEM_NO_DATA )
		{
			*Err = NO_ANSWER_FROM_CMD;
			CaseGetCredit = 0;
			
			return true;
		}

		if(	QueueCMD.MessageID == NANSWER_ID  ||  
			QueueCMD.MessageID == NCARRIER_ID ||
			QueueCMD.MessageID == BUSY_ID )
		{
			CaseGetCredit = 0;
			return true;
		}		

	break;
   // --------------------------------------------------------------------------
   
   // --------------------------------------------------------------------------
   // SMS_TO_NUMBER_AND_WAIT_SMS
   case 20:
			
			if( strlen(Credit) <= 0 )
			{
				*Err = NO_SMS_CREDIT_AVAILABLE;
				CaseGetCredit = 0;
				
				return true;
			}
			
			CaseGetCredit++;
			
	break;
   // --------------------------------------------------------------------------

   // --------------------------------------------------------------------------
   case 21:
			// send a sms
			if( SMSSend( Credit , TelService, Err) )
			{
				CaseGetCredit = 0;
				return true;
			}			

	break;
   // --------------------------------------------------------------------------

   // --------------------------------------------------------------------------
   // UNSTRUCURED_SERVICE_DATA
   case 30:
		
		//try to send a call to service number to receive a message for credit
		if(	CallServiceNumerAndWaitString(TelService, DestString, Err ) )
		{
			CaseGetCredit = 0;
			return true;
		}		

	break;
   // --------------------------------------------------------------------------
 } 	
 return false;
}
// ----------------------------------------------------------------------------------------------------------------------------------------------------------
/* Src  = stored number in SIM:
	 ONLY INTERNATIONAL FORMAT WITHOUT OPERATOR CODE
NOT THIS 00[operator][country code][number] (00 41 39 3473847300)
BUT THIS +[country code][number] (+ 39 347 3847300)
								 (+ 55 54  81500009)

   Dest = Number to check given by network

if Dest begins with +  => international numer. Check All
if Dest = 8  char      => same area      , check only last 8 char from Src       (       81438046 )
if Dest = 11 char      => different area , check only last 10 char               (0   54 81438046 )
if Dest = 13 char      => different area with operator number.Check last 10 char (041 54 81438046 )
if Dest begins with 00 => different country. Check starts with 5th character dest and second src.

*/
// ----------------------------------------------------------------------------------------------------------------------------------------------------------
bool  CheckNumber( char *Dest, char *Src)
{
	if( *Dest == '+')
	{
		if(!strcmp(Dest, Src) )
			return true;
		else
			return false;
	}

	switch( strlen(Dest) )
	{
		// ---------------------------------------------------
		case 8 :

			// check last 8 char SRC
			if(!strcmp(Dest, &Src[ strlen(Src)-8]) )
				return true;

		// ---------------------------------------------------
		break;

		// ---------------------------------------------------
		case 11:
		case 13:

			if (strlen(Src)< 10)
				return false;

			// check last 10 char SRC
			if(!strcmp(Dest, &Src[ strlen(Src)-10]) )
				return true;

		// ---------------------------------------------------
		break;

		// ---------------------------------------------------
		default:

			if( Dest[0] == '0' && Dest[1] == '0' )
			{
				// check starts with 5th char DEST and 2th SRC
				// 0041393473847300  +393473847300
				if(!strcmp(&Dest[4], &Src[1]) )
					return true;
			}

		break;
		// ---------------------------------------------------
	}
	return false;
}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------
// if BRAZIL AND PhoneNr begins with 00 => different country.
// possible to send SMS to other country only in EUROPEAN Format
// ----------------------------------------------------------------------------------------------------------------------------------------------------------
void  SetNumber(char *Phone)
{
 int i;

	if( Phone[0] == '0' && Phone[1] == '0' )
	{
		Phone[0] = '+';

		// 00[operator code][ country code ..... ]
		for(i=4; i<MAX_PHONE_NUMBER_SIZE; i++)
			Phone[i-3] = Phone[i];
	}
}
// ----------------------------------------------------------------------------------------------------------------------------------------------------------
// Unstructured request data: to service number and wait answer string
// Check Err to see Error in case return value is false.
// return value = no error
// ----------------------------------------------------------------------------------------------------------------------------------------------------------
bool  CallServiceNumerAndWaitString(char *ServicePhone, char *DestString, char *Err )
{
	UARTMessage			QueueCMD;
	int					i, Idx, TmpIdx;
	portTickType		xLastWakeTime;


	*Err = OK;

	//wait 0.5 seconds
	xLastWakeTime = xTaskGetTickCount();
	vTaskDelayUntil( &xLastWakeTime, 500/portTICK_RATE_MS );

	memset(TxCommandToModem.ptrUART_Buf, 0, UART_TX_BUFFER_SIZE);
	sprintf(TxCommandToModem.ptrUART_Buf,"%s%s%s%s","AT+CUSD=1,\"", ServicePhone, "\" ", ENDSTRING);

	// Request string to network
	TxRxCommand(OK_, &QueueCMD);

	if( QueueCMD.MessageID != DATA_READY_ID )
	{
		*Err = NO_ANSWER_FROM_CMD;
		return false;
	}

	Idx  = StrIndex( QueueCMD.ptrUART_Buf, '"'); //first "

	if( !Idx )
	{
		*Err = NO_ANSWER_FROM_CMD;
		return false;
	}

	i = 0;
	TmpIdx = QueueCMD.UART_Index;
	TmpIdx -= Idx;

	while(TmpIdx--)
		*DestString++ = QueueCMD.ptrUART_Buf[ Idx + (i++) ];

	return true;
}
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
/* Callback for timer. Only ask for net every 60 seconds
 */
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void vCallbackFunction( xTimerHandle xTimer )
{
	if( !(CommandFlags & ASK_FOR_NET)  )
	{
		//ask for network OK
		//no need critical section. Only here is written this bit
		CommandFlags |= ASK_FOR_NET;
	}
}
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
