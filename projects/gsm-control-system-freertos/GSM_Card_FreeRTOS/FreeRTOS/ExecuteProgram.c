/*
   ExecuteProgram.c

   GSM_ControlSystem "GSA" V1.0.0  Copyright (C) 2012 Easy System


    *****************************************************************************
    *    >>> See http://www.esyste.com for Hw Documentation <<<					*
    *                                                                           *
    *    Thank you for using GSA, and thank you for your support!               *
    *                                                                           *
    *****************************************************************************
    see header file for details
*/

#include <avr/wdt.h>
#include <avr/io.h>
#include <avr/eeprom.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#include "ExecuteProgram.h"
#include "GSM_Card.h"
#include "Init.h"
#include "Define.h"
#include "a2d.h"
#include "Modem.h"
#include "Parser.h"
#include "ISR_Uart.h"
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Extern variables
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
extern ProgramStep					Program[MAX_PROG_STEP]						EEMEM;//program step to execute user program
extern char							IOStrings [MAX_STRINGS][MAX_STRING_CHAR_IO]	EEMEM;//User Strings for event or I/O names
extern uint32_t						DateTimeArray[NUM_MAX_EVENTS]				EEMEM;//Date array for events
extern uint32_t						WeekTimeArray[NUM_MAX_WEEK_EVENTS]			EEMEM;//Date for Week events
extern DATA_EVENTS					T0_To_n_Event								EEMEM;//to use for managing user events program
extern DATA_EVENTS					T0_To_n_WeekEvent							EEMEM;//to use for managing user events program
extern Week							DayInWeekEvent[NUM_MAX_WEEK_EVENTS]			EEMEM;//Date for events inside a week
extern volatile	uint16_t			CommandFlags								;//
extern volatile double				T1,T2										;//
extern volatile unsigned char		PIn											;//
extern volatile unsigned char		PNIn										;//
extern volatile unsigned char		POut 										;//
extern volatile unsigned char		PNOut										;//
extern volatile unsigned long int	CurrentDateTime 							;//

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Global variables
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
volatile char						IndexStringSMS;

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Program Execution
// This function executes program written by USER
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void ExecuteProgram(bool InitProgram)
{
  uint8_t			PrgIndex    = 0	;
  DATA_EVENTS		Mask, Tmp		;
  bool				DoAction		;
  ProgramStep		CurrentPrg		;
  unsigned long int DateTime		;
  Week 				DayInWeek		;

  static  bool	VettRisingEdge1[MAX_PROG_STEP];
  static  bool	VettRisingEdge2[MAX_PROG_STEP];
  static  bool	VettRisingEgeSMS[MAX_PROG_STEP];

  // PORTB image because one user program step could set an output and other step could reset.
  //we update output only at the end of program
  unsigned char     ImageOut = PINB;

	//------------------------------------------------------------------------------------------------------------
	// VettRisingEgeSMS[PrgIndex] reset is done for each command
	// because each one command can activate it
	//------------------------------------------------------------------------------------------------------------

	if( InitProgram )
	{
 		memset(VettRisingEdge1,  0, sizeof(VettRisingEdge1 ) );
 		memset(VettRisingEdge2,  0, sizeof(VettRisingEdge2 ) );
 		memset(VettRisingEgeSMS, 0, sizeof(VettRisingEgeSMS) );
	}

	//we process all program lines in a time because of Rising or falling edge I/O variables (valid only for a cycle)
	while( PrgIndex < MAX_PROG_STEP )
	{
			//Initialize action = no action to do
			DoAction = false;
			//read preogram step
			eeprom_read_block( &CurrentPrg, &(Program[PrgIndex]), sizeof(ProgramStep) );

			//Analyze Command stored inside current program step
			switch( CurrentPrg.Cmd )
			{
				//--------------------------------------------------------------------------------------------------------------------------------------
				/// Positive Input rising edge: Symbol "PI". User program write number from 1 to 4. Here Index starts from 0 to 3
				/// Only 4 input are admitted
				case PI:
					Mask = 1;

					if( PIn & ( Mask << (CurrentPrg.CmdNum-1) ) )			 //REMEBER:  1-4 instead of  0-3
						DoAction = true;
					else
						VettRisingEgeSMS[PrgIndex] = false;

				//--------------------------------------------------------------------------------------------------------------------------------------
				break;

				//--------------------------------------------------------------------------------------------------------------------------------------
				/// Negative Input rising edge: Symbol "PNI". User program write number from 1 to 4. Here Index starts from 0 to 3
				/// Only 4 input are admitted
				case PNI:
					Mask = 1;

					if( PNIn & ( Mask << (CurrentPrg.CmdNum-1) ) )		 //REMEBER:  1-4 instead of  0-3
						DoAction = true;
					else
						VettRisingEgeSMS[PrgIndex] = false;

				//--------------------------------------------------------------------------------------------------------------------------------------
				break;

				//--------------------------------------------------------------------------------------------------------------------------------------
				/// Positive Output rising edge: Symbol "PQ". User program write number from 1 to 4. Here Index starts from 0 to 3
				/// Only 4 output are admitted
				case PQ:
					Mask = 1;

					if( POut & ( Mask << (CurrentPrg.CmdNum-1) ) )		 //REMEBER:  1-4 instead of  0-3
						DoAction = true;
					else
						VettRisingEgeSMS[PrgIndex] = false;

				//--------------------------------------------------------------------------------------------------------------------------------------
				break;

				//--------------------------------------------------------------------------------------------------------------------------------------
				/// Negative Output rising edge: Symbol "PNQ". User program write number from 1 to 4. Here Index starts from 0 to 3
				/// Only 4 output are admitted
				case PNQ:
					Mask = 1;

					if( PNOut & ( Mask << (CurrentPrg.CmdNum-1) ) )		 //REMEBER:  1-4 instead of  0-3
						DoAction = true;
					else
						VettRisingEgeSMS[PrgIndex] = false;
				//--------------------------------------------------------------------------------------------------------------------------------------
				break;

				//--------------------------------------------------------------------------------------------------------------------------------------
				/// Analog Input > value: Symbol "AMajor". User program write number from 1 to 2
				/// Only 2 analog input are admitted
				case AMajor:
					//A1 (first analog input)
					if(	CurrentPrg.CmdNum == 1 	 &&
						T1 != FAULT_TEMP_PTC	 &&
						(T1 >= CurrentPrg.MinVal && !VettRisingEdge1[PrgIndex] )
					  )
					{
						VettRisingEdge1[PrgIndex] = true;
					}

					if(	CurrentPrg.CmdNum == 1 &&
						VettRisingEdge1[PrgIndex] &&
						( T1 < CurrentPrg.MinVal - HISTERESYS )
					  )
					{
						VettRisingEdge1[PrgIndex] = false;
					}

					//A2 (second analog input)
					if(	CurrentPrg.CmdNum == 2 &&
						T2 != FAULT_TEMP_PTC   &&
						( T2 >= CurrentPrg.MinVal && !VettRisingEdge1[PrgIndex] )
					  )
					{
						VettRisingEdge1[PrgIndex] = true;
					}

					if(	CurrentPrg.CmdNum == 2 &&
						VettRisingEdge1[PrgIndex] &&
						( T2 < CurrentPrg.MinVal - HISTERESYS )
					  )
					{
						VettRisingEdge1[PrgIndex] = false;
					}

					//Action always executed inside hysteresys interval
					if( VettRisingEdge1[PrgIndex] )
						DoAction = true;
					else
						VettRisingEgeSMS[PrgIndex] = false;

				//--------------------------------------------------------------------------------------------------------------------------------------
				break;

				//--------------------------------------------------------------------------------------------------------------------------------------
				/// Analog Input < value: Symbol "AMinor". User program write number from 1 to 2
				/// Only 2 analog input are admitted
				case AMinor:
					//A1  (first analog input)
					if(	CurrentPrg.CmdNum == 1 &&
						T1 != FAULT_TEMP_PTC   &&
						( T1 <= CurrentPrg.MinVal && !VettRisingEdge1[PrgIndex] )
					  )
					{
						VettRisingEdge1[PrgIndex] = true;
					}

					if(	CurrentPrg.CmdNum == 1    &&
						VettRisingEdge1[PrgIndex] &&
						( T1 > CurrentPrg.MinVal + HISTERESYS )
					  )
					{
						VettRisingEdge1[PrgIndex] = false;
					}
					//A2  (second analog input)
					if(	CurrentPrg.CmdNum == 2 &&
						T2 != FAULT_TEMP_PTC   &&
						( T2 <= CurrentPrg.MinVal && !VettRisingEdge1[PrgIndex] )
					  )
					{
						VettRisingEdge1[PrgIndex] = true;
					}

					if(	CurrentPrg.CmdNum == 2 &&
						VettRisingEdge1[PrgIndex] &&
						( T2 > CurrentPrg.MinVal + HISTERESYS )
					  )
					{
						VettRisingEdge1[PrgIndex] = false;
					}
					//Action always executed inside hysteresys interval
					if( VettRisingEdge1[PrgIndex] )
						DoAction = true;
					else
						VettRisingEgeSMS[PrgIndex] = false;

				//--------------------------------------------------------------------------------------------------------------------------------------
				break;

				//--------------------------------------------------------------------------------------------------------------------------------------
				/// Analog Input outside an interval: Symbol "AInterval". User program write number from 1 to 2
				/// Only 2 analog input are admitted
				case AInterval: //
					if(	CurrentPrg.CmdNum == 1 )
					{
						if(	(T1 < CurrentPrg.MinVal || T1 > CurrentPrg.MaxVal ) &&
							 T1 != FAULT_TEMP_PTC      							&&
							 !VettRisingEdge1[PrgIndex] 						&&
							 !VettRisingEdge2[PrgIndex]
						  )
						{
							if( T1 < CurrentPrg.MinVal )
								VettRisingEdge1[PrgIndex] = true;

							if(T1 > CurrentPrg.MaxVal)
								VettRisingEdge2[PrgIndex] = true;
						}

						if ( T1 >= CurrentPrg.MinVal + HISTERESYS && VettRisingEdge1[PrgIndex]  )
							VettRisingEdge1[PrgIndex] = false;

						if ( T1 <= CurrentPrg.MaxVal - HISTERESYS && VettRisingEdge2[PrgIndex]  )
							VettRisingEdge2[PrgIndex] = false;
					}

					if( CurrentPrg.CmdNum == 2 )
					{
						if( 	(T2 < CurrentPrg.MinVal || T2 > CurrentPrg.MaxVal ) &&
								 T2 != FAULT_TEMP_PTC      							&&
								!VettRisingEdge1[PrgIndex]							&&
								!VettRisingEdge2[PrgIndex]
						  )
						{
							if( T2 < CurrentPrg.MinVal )
								VettRisingEdge1[PrgIndex] = true;
							if( T2 > CurrentPrg.MaxVal )
								VettRisingEdge2[PrgIndex] = true;
						}

						if ( T2 >= CurrentPrg.MinVal + HISTERESYS && VettRisingEdge1[PrgIndex]  )
							VettRisingEdge1[PrgIndex] = false;

						if ( T2 <= CurrentPrg.MaxVal - HISTERESYS && VettRisingEdge2[PrgIndex]  )
							VettRisingEdge2[PrgIndex] = false;
					}
					//Action always executed inside hysteresys interval
					if(	VettRisingEdge1[PrgIndex] ||
						VettRisingEdge2[PrgIndex]
					  )
						DoAction = true;
					else
						VettRisingEgeSMS[PrgIndex] = false;

				//--------------------------------------------------------------------------------------------------------------------------------------
				break;

				//--------------------------------------------------------------------------------------------------------------------------------------
				/// Temporal event: Symbol "T".
				case T:
					Mask = 1;
					//Is it defined? (It may be used by user program but not defined)
					eeprom_read_block( &Tmp, &T0_To_n_Event, sizeof(DATA_EVENTS) );
					if( Tmp & ( Mask << (CurrentPrg.CmdNum-1) ) )																 //1-xx instead of 0-(xx-1)
					{
						//  is defined:
						DateTime = eeprom_read_dword (&DateTimeArray[CurrentPrg.CmdNum-1]);

						if( IsOnlyTime(DateTime) )
						{
							if( GetTime(CurrentDateTime) >= DateTime )
								DoAction = true;
						}
						else
						{
							if( CurrentDateTime >= DateTime )
								DoAction = true;
						}
					}
					if( !DoAction )
						VettRisingEgeSMS[PrgIndex] = false;

				//--------------------------------------------------------------------------------------------------------------------------------------
				break;

				//--------------------------------------------------------------------------------------------------------------------------------------
				/// weekly temporal event: Symbol "W"
				case W:
					Mask = 1;
					//Is it defined? (It may be used by user program but not defined)
					eeprom_read_block( &Tmp, &T0_To_n_WeekEvent, sizeof(DATA_EVENTS) );
					if( Tmp & ( Mask << (CurrentPrg.CmdNum-1)  ) )																 //1-xx instead of 0-(xx-1)
					{
						//  is defined:
						DateTime  	= eeprom_read_dword (&WeekTimeArray[CurrentPrg.CmdNum-1]);
						DayInWeek = eeprom_read_byte((uint8_t *)&(DayInWeekEvent[CurrentPrg.CmdNum-1]));
						// read day of week
						if ( DayInWeek == GetDayInWeek( CurrentDateTime ) )
						{
							if( IsOnlyTime(DateTime) )
							{
								if( GetTime(CurrentDateTime) >= DateTime )
									DoAction = true;
							}
							else
							{
								if( CurrentDateTime >= DateTime)
									DoAction = true;
							}
						}
					}
					if( !DoAction )
						VettRisingEgeSMS[PrgIndex] = false;

				//--------------------------------------------------------------------------------------------------------------------------------------
				break;

				//--------------------------------------------------------------------------------------------------------------------------------------
				default:
					//restore output image to phisical outputs
					if( PINB != ImageOut )
					{
						PORTB = ImageOut;
					}
					return; //finished
			}//END SWITCH

		//There is an Action to Do?
		if( !DoAction )
		{
			//No so we continue to read next program step
			PrgIndex++;
			continue;
		}
		//--------------------------------------------------------------------------------------------------------------------------------------------
		/// Action to do
		//Check Action
		Mask = 1;

		switch( CurrentPrg.Act )
			{
				//------------------------------------------------------------------------------------------------------------------------------------------------------------
				///	Output to force value
				///	Qx = 0/1 where x = 1-4
				case Q:
					switch( CurrentPrg.ActNum )
					{
						case 1:                  //in user program start from 1 but in a firmware starts from 0
								break;
						case 2:
								Mask <<= 1;
								break;
						case 3:
								Mask <<= 2;
								break;
						case 4:
								Mask <<= 3;
								break;
					}
					if( CurrentPrg.Val && !(ImageOut & Mask)  )
					{
						ImageOut |= Mask;
					}
					if( !CurrentPrg.Val && (ImageOut & Mask) )
					{
						ImageOut &= ~Mask;
					}

				//--------------------------------------------------------------------------------------------------------------------------------------
				break;

				//------------------------------------------------------------------------------------------------------------------------------------------------------------
				///	SMS to send: SMSxx sends sms to first phone book address
				case SMS:
					if( !(CommandFlags & SEND_SMS_STR) && !VettRisingEgeSMS[PrgIndex] )
					{
						//String to send
						IndexStringSMS 			  	= CurrentPrg.Val;
						// set command to send SMS
						CommandFlags 				|= SEND_SMS_STR;

						VettRisingEgeSMS[PrgIndex] 	= true;
					}

				//--------------------------------------------------------------------------------------------------------------------------------------
				break;

				//------------------------------------------------------------------------------------------------------------------------------------------------------------
				///	 SMSxx sends sms number xx (0-9); with call confirmation
				case SENDC:
					//Read String
					if( !(CommandFlags & SEND_SMS_STR_CALL) && !VettRisingEgeSMS[PrgIndex] )
					{
						//String to send
						IndexStringSMS 						= CurrentPrg.Val;
						// set command to send SMS
						CommandFlags 						|= SEND_SMS_STR_CALL;

						VettRisingEgeSMS[PrgIndex] 			= true;
					}

				//--------------------------------------------------------------------------------------------------------------------------------------
				break;

				//------------------------------------------------------------------------------------------------------------------------------------------------------------
				default:
					//restore output image to phisical outputs
					if( PINB != ImageOut )
					{
						PORTB = ImageOut;
					}
					return; //finished
			}//END SWITCH
	PrgIndex++;
	}//END WHILE
}
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
