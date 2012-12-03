#include <avr/io.h>
#include <avr/eeprom.h>
#include <avr/interrupt.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <ctype.h>


//------------------------------------------------------------------------------------------------------------------
#include "FreeRTOS.h"
#include "Parser.h"
#include "ISR_Uart.h"
#include "modem.h"
#include "Uart_Utils.h"
//---------------------------------------------------- --------------------------------------------------------------
extern volatile UARTMessage	TxCommandToModem;
extern volatile bool RunProgram;
extern volatile bool RunGateMode;
extern volatile bool RunToggleRingMode;
extern volatile char NumRingToggleRingMode;
extern Week          DayInWeekEvent[NUM_MAX_WEEK_EVENTS]			EEMEM;
extern uint32_t      WeekTimeArray[NUM_MAX_WEEK_EVENTS]				EEMEM;
extern uint8_t       GeneralModalityFlags							EEMEM;
extern uint32_t      DateTimeArray[NUM_MAX_EVENTS]					EEMEM;
extern DATA_EVENTS   T0_To_n_Event									EEMEM;
extern DATA_EVENTS   T0_To_n_WeekEvent								EEMEM;
extern char          SMSstrings[MAX_STRINGS][MAX_STRING_SMS_CHAR]	EEMEM;
extern char          IOStrings [MAX_STRINGS][MAX_STRING_CHAR_IO]	EEMEM; //Allocated strings for I/O description
extern char 		 SMSCredit [MAX_STRING_LEN_SMS_CREDIT]			EEMEM; //SMS to ask for credit
extern double        Offset0_1										EEMEM;     // °C
extern double        Offset2_3										EEMEM;     // °C
extern char          PrgNoConverted[SMS_ASCII_BUFFER_SIZE]			EEMEM;
extern ProgramStep   Program[MAX_PROG_STEP]							EEMEM;
extern GATE          GateMode										EEMEM;
extern TOGGLERING    ToggleRingMode 								EEMEM;
extern unsigned char EE_WayForCredit								EEMEM;

extern volatile uint16_t  CommandFlags;
extern volatile char 	  WayForCredit;// SMS or CALL or Unstructured Data Service

// ----------------------------------------------------------------------------------------------------------------------------------------------------------
// ----------------------------------------------------------------------------------------------------------------------------------------------------------
// char * Buf  input string to check commands
// the function put ',' with 0x0 in the command String
// ----------------------------------------------------------------------------------------------------------------------------------------------------------
bool CheckCommandString(char * Buf)
{
	int			Index;
	int			Idx1;
	int			Idx2;
	int      	ValRet;
	bool     	Val;
	bool     	OkCmd;
	bool     	RetVal = false;
	DATA_EVENTS Tmp;
	DATA_EVENTS K;
	DATA_EVENTS Tmp1;
	uint32_t    DateTime;
	Week     	DayWeek;
	GATE     	CurrGate;
	TOGGLERING  CurToggleRing;
	char     	Q;
	char		i;
	char 		Day[3];
	char 		Month[3];
	char 		Year[3];
	char 		Houre[3];
	char 		Min[3];
	char 		Sec[3];
	char 		TmpMemoPrg[SMS_ASCII_BUFFER_SIZE];
	UARTMessage	QueueCMD;

	memset(Day   , 0, 3);
	memset(Month , 0, 3);
	memset(Year  , 0, 3);
	memset(Houre , 0, 3);
	memset(Min   , 0, 3);
	memset(Sec   , 0, 3);

	//Remove Spaces and CRLF
	DelChar(Buf, ' ');
	DelChar(Buf, '\r');
	DelChar(Buf, '\n');

	Idx1 = StrIndex(Buf,'&');
	Idx2 = StrIndex(Buf,'!');
  
	if(Idx1 && Idx2 )
	{
		// memo new Prg: ALL MESSAGE INCLUDED OTHER EVENTUALLY
		strcpy( TmpMemoPrg, Buf);
		if( CheckProgram( &Buf[Idx1-1]) )
		{
			//Program is correct then Memo it
			eeprom_write_block (TmpMemoPrg, PrgNoConverted, SMS_ASCII_BUFFER_SIZE );
			RetVal = true;
		}
		else
		//will SEND SMS PROGRAM ERROR
		return false;
	}
	//shift left idx2-idx1 byte from the buffer
	if( --Idx1 && Idx2 )
	{
		i = 0;
		while(Buf[Idx2 + i])
		{
			Buf[Idx1 + i] = Buf[Idx2 + i];
			i++;
		}
		Buf[Idx1 + i] = 0;
	}

	while( *Buf )
	{
		OkCmd = false;
		Index = StrIndex(Buf,',');

		if( Index )
			Buf[Index-1] = 0;

		//Check if command is "Qx=0" or "Qx=1" where x = port number
		// ------------------------------------------------------------------------
		if( GetValOut(Buf, &Q, &Val) && !OkCmd )
		{
			OkCmd = true;
			
			if(	Val	)
				PORTB |= Q;
			else
				PORTB &= ~Q;

			//WriteEprom = 1;
			RetVal = true;
		}
		
		//Check if command is "TIME= dd/mm/yy hh:mm:ss"
		// ---------------------------------------------------------------------
		if( GetClock(Buf, Day, Month, Year, Houre, Min, Sec) && !OkCmd )
		{
			OkCmd =  true;
			memset(TxCommandToModem.ptrUART_Buf, 0, UART_TX_BUFFER_SIZE);
			
					//"yy/MM/dd,hh:mm:ss±zz" format
			sprintf(TxCommandToModem.ptrUART_Buf,
					"AT+CCLK=\"%s/%s/%s,%s:%s:%s+00\"\r\n" ,
					Year,
					Month,
					Day,
					Houre,
					Min,
					Sec);

			//Response OK
			TxRxCommand(OK_, &QueueCMD);
			RetVal = true;
   }
   
   //Check if the command is PW=0/1 (send SMS if No power)
   // -------------------------------------------------------------------
   if( GetSmsNoPower( Buf, &Val ) && !OkCmd )
   {
		OkCmd = true;

		Tmp  = eeprom_read_byte(&GeneralModalityFlags);

		//Activate or deactivate Send SMS if no power
		if( Val )
			Tmp |= SMS_NO_POWER; //activate
		else
			Tmp &= ~SMS_NO_POWER; // deactivate

		eeprom_write_byte(&GeneralModalityFlags, Tmp );

		RetVal = true;
	}
	//Check if the command is Temporal Event:
	// "Txx=21/05/09 15:20:00" or "Txx=15:20:00"
	// ------------------------------------------------------------------------------
	if( GetTemporalEvent(Buf, &Tmp, &DateTime) && !OkCmd )
	{
		OkCmd = true;
		
		for(K = 1, i=0; i<NUM_MAX_EVENTS; K <<= 1, i++)
		{
			if(Tmp & K )
			{
				eeprom_write_dword (&DateTimeArray[(int)i], DateTime);
			}
		}
		
		eeprom_read_block(&Tmp1, &T0_To_n_Event, sizeof(DATA_EVENTS) );
		Tmp  |= Tmp1;
		//memo event
		eeprom_write_block(&Tmp, &T0_To_n_Event, sizeof(DATA_EVENTS));

		RetVal = true;
	}

	//Check if the command is MESSAGE SMS to memorize (and do it)
	// ------------------------------------------------------------------------------
	if(GetSMSMessageToEEmem(Buf) && !OkCmd )
	{
		OkCmd = true;
		RetVal = true;
	}

	//Check if the command STRING to memorize (and do it)
	// ------------------------------------------------------------------------------
	if(GetIOMessageToEEmem(Buf) && !OkCmd )
	{
		OkCmd  = true;
		RetVal = true;
	}

	//Check if the command is "CALIB" calibration Pt100 (and do it)
	// ------------------------------------------------------------------------------
	//Check if the command is "Offset1 = xxx.xxx" Pt100 (and do it)
	// ------------------------------------------------------------------------------
	if(GetOffsetPTC(Buf) && !OkCmd )
	{
		OkCmd  = true;
		RetVal = true;
	}

	//Check if the command is "PRG" send current Program
	// ------------------------------------------------------------------------------
	if(GetPrg(Buf) && !OkCmd )
	{
		OkCmd  = true;
		RetVal = true;
	}

	//Check if the command is WEEK Temporal Event:
	// "Wxx=[yyy] 15:20:00"
	//      [mon, tue, wed, thu, fri, sat, sun]
	// ------------------------------------------------------------------------------
	if( GetWeekTemporalEvent(Buf, &Tmp, &DateTime,  &DayWeek, &K) && !OkCmd )
	{
		OkCmd  = true;
		//memo day in week event
		eeprom_write_byte((uint8_t *)&(DayInWeekEvent[K]), DayWeek );

		for(K = 1, i=0; i<NUM_MAX_WEEK_EVENTS; K <<= 1, i++)
		{
			if(Tmp & K )
			{
				eeprom_write_dword (&WeekTimeArray[(int)i], DateTime);
			}
		}
		
		eeprom_read_block(&Tmp1, &T0_To_n_WeekEvent, sizeof(DATA_EVENTS) );
		Tmp  |= Tmp1;

		//memo event
		eeprom_write_block( &Tmp, &T0_To_n_WeekEvent, sizeof(DATA_EVENTS));

		RetVal = true;
	}

	//Check if the command is "RUN = 0/1" execution program
	// ------------------------------------------------------------------------------
	if( ProgramOnOff( Buf, &Val) )
	{
		if( Val != RunProgram )
		{
			RunProgram = Val;
			//Read all bits
			Q = eeprom_read_byte(&GeneralModalityFlags);
			
			if( RunProgram )
				Q |=  RUN_PROGRAM;
			else
				Q &= ~RUN_PROGRAM;

			eeprom_write_byte(&GeneralModalityFlags, Q );
		}
		RetVal = true;
	}

	//GATE modality
	//Check if the command is "RING = Qx:nn"  Open/close an Out Qx impulsive for nn*100 ms
	//                                Off
	// ------------------------------------------------------------------------------
	if( CheckGate( Buf, &Q, &ValRet) && !OkCmd )
	{
		OkCmd = true;
		if( Q == (char)-1 )
		{
			RunGateMode  = false;
			//RunProgram    = true;
			Q = eeprom_read_byte(&GeneralModalityFlags);
			Q &= ~GATE_MODE;
			eeprom_write_byte(&GeneralModalityFlags, Q );
		}
		else 
		{
			CurrGate.Delay    =  ValRet;
			CurrGate.Out      =   Q - 1; //user write out 1 but here begins from 0
			RunGateMode       =   true;
			//disable toggle ring mode
			RunToggleRingMode =   false;
			NumRingToggleRingMode = 0;
			Q = eeprom_read_byte(&GeneralModalityFlags);
			Q |= GATE_MODE;
			Q &= ~TOGGLE_RING; //Stop Toggle ring mode

			eeprom_write_byte(&GeneralModalityFlags, Q );
			eeprom_write_block( &CurrGate, &GateMode, sizeof(GATE));
		}
	RetVal = true;
	}

	//TOGGLERING modality
	//Check if the command is "TOGGLE = Qx:Qy:nn"
	//close Out Qx impulsive for nn*100 ms at the first call
	//close Out Qy impulsive for nn*100 ms at the second call
	// ------------------------------------------------------------------------------
	if( ToggleRing( Buf, &Q, &i, &ValRet) && !OkCmd )
	{
		OkCmd = true;
		if( Q == (char)-1 )
		{
			RunToggleRingMode     = false;
			NumRingToggleRingMode = 0;
		   Q = eeprom_read_byte(&GeneralModalityFlags);
		   Q &= ~TOGGLE_RING;
		   
		   eeprom_write_byte(&GeneralModalityFlags, Q );
		}
		else 
		{
			CurToggleRing.Delay =  ValRet;
			CurToggleRing.Out1   =   Q - 1; //user write out 1 but here begins from 0
			CurToggleRing.Out2   =   i - 1; //user write out 1 but here begins from 0
			RunToggleRingMode    =   true;
			RunGateMode          =   false;
			Q = eeprom_read_byte(&GeneralModalityFlags);
			Q |= TOGGLE_RING;
			Q &= ~GATE_MODE; // stop ring mode
			eeprom_write_byte(&GeneralModalityFlags, Q );
			eeprom_write_block( &CurToggleRing, &ToggleRingMode, sizeof(TOGGLERING));
		}
		RetVal = true;
	}

	//Check if the command is "STATUS" to send status by SMS
	// ------------------------------------------------------------------------------
	if( CmpSubStr( strlwr(Buf) , "status" ) && !OkCmd )
	{
		OkCmd = true;
		CommandFlags |= SEND_SMS_STATUS;
		RetVal = true;
	}

	//Check if the command is "CREDIT" to send credit status by SMS
	// ------------------------------------------------------------------------------
	if( CmpSubStr( strlwr(Buf) , "credit" ) && !OkCmd )
	{
		OkCmd = true;
		CommandFlags |= SEND_CREDIT_STRING;
		RetVal = true;
	}

	//Check if the command is "CREDITMODE = MOD [[:]"sms string"] to determine the way for credit
	// ------------------------------------------------------------------------------
	if( GetCreditModality( Buf ) && !OkCmd )
	{
		OkCmd = true;
		RetVal = true;
	}

	if( !Index )
		//we finished
		break;
		
	Buf = &Buf[Index];
	}

	return RetVal;
}
// ----------------------------------------------------------------------------------------------------------------------------------------------------------
/*
Check if the command is:
   //TOGGLERING modality
   //Check if the command is "TOGGLE = Qx:Qy:nn"
   //                                  off
   //close Out1 impulsive for Delay*100 ms at the first call
   //close Out2 impulsive for Delay*100 ms at the second call
   //off stop modality
return true if yes
*/
// ----------------------------------------------------------------------------------------------------------------------------------------------------------
bool ToggleRing( char *Buf, char *Out1, char *Out2, int *Delay)
{
	int Index;
	char Q1,Q2;

	*Out1  = -1;
	*Out2  = -1;
	*Delay =  0;

	if( CmpSubStr( strlwr(Buf) , "toggle=off" ) )
		return true;

	if( !CmpSubStr( strlwr(Buf) , "toggle=q" ) )
		return false;

	Index = StrIndex(Buf,'q');

	if( !isdigit( Buf[Index] ) )
		return false;

	Q1 =  Buf[Index++] - 0x30;

	if( Buf[Index++] != ':' )
		return false;

	if( Buf[Index++] != 'q' )
		return false;

	if( !isdigit( Buf[Index] ) )
		return false;

	Q2 =  Buf[Index++] - 0x30;

	if( Buf[Index++] != ':' )
		return false;

	*Delay = atoi( &Buf[Index] );

	// not permitted delay outside 0 and 99
	if(*Delay < 0 || *Delay > 99 )
		return false;

	*Delay = *Delay * 10;
	*Out1  =  Q1;
	*Out2  =  Q2;

	return true;
}
// ----------------------------------------------------------------------------------------------------------------------------------------------------------
/*
 Check if the command is "CREDITMODE = MOD [:"sms string"] to determine the way for credit

 CREDITMODE = 0                 // call to number and wait for SMS credit from network
 CREDITMODE = 1: "PRE CRE SIN"  // Send SMS to service centre number and wait for SMS credit from network
 CREDITMODE = 2                 // Instant message and wait for string credit from network

*/
// ----------------------------------------------------------------------------------------------------------------------------------------------------------
bool GetCreditModality( char *Buf )
{
	int Index;

	if( !CmpSubStr( strlwr(Buf) , "creditmode" ) )
		return false;

	Index = StrIndex(Buf,'=');

	if( !Index )
		return false;

	Buf += Index;

	if( !isdigit(*Buf) )
		return false;

	if( *Buf == '0' )
	{
		//call to number and wait for SMS credit from network
		WayForCredit = CALL_TO_NUMBER_AND_WAIT_SMS;
	}

	if( *Buf == '2' )
	{
		//Instant message and wait for string credit from network
		WayForCredit = UNSTRUCURED_SERVICE_DATA;
	}

	if( *Buf == '1' )
	{
		Index = StrIndex(Buf,'"');
		
		if( !Index )
			return false;

		Buf += Index;

		Index = StrIndex(Buf,'"');
		
		if( !Index )
			return false;

		// Send SMS to service centre number and wait for SMS credit from network
		WayForCredit = SMS_TO_NUMBER_AND_WAIT_SMS;

		Buf[Index - 1] = 0; //terminator
		
		eeprom_write_block (Buf, SMSCredit, strlen(Buf) );
		
		return true;
	}

	eeprom_write_byte(&EE_WayForCredit, WayForCredit );

	return true;
}
// ----------------------------------------------------------------------------------------------------------------------------------------------------------
/*
Check if the command is:
	 RING = Qx:nn   Open/close GATE modality on Out Qx impulsive for nn (Sec/10)
	 RING = Qx      GATE modality Toggle Out Qx on Ring
	 RING = Off     GATE modality OFF ( return Out = -1)
return false if not a command
return true if yes
*/
// ----------------------------------------------------------------------------------------------------------------------------------------------------------
bool CheckGate( char *Buf, char *Out, int *Delay)
{
	int Index;
	*Out   = -1;
	*Delay =  0;

	if( CmpSubStr( strlwr(Buf) , "ring=off" ) )
		return true;

	if( !CmpSubStr( strlwr(Buf) , "ring=q" ) )
		return false;

	Index = StrIndex(Buf,'q');

	if( !isdigit( Buf[Index] ) )
		return false;

	*Out =  Buf[Index++] - 0x30;

	if( Buf[Index++] != ':' )
	{
		*Delay = 0;
		return true;
	}

	*Delay = atoi( &Buf[Index] );

	// not permitted delay outside 0 and 99
	if(*Delay < 0 || *Delay > 99 )
		*Delay = 0;

	*Delay = *Delay * 10;

	return true;
}
// ----------------------------------------------------------------------------------------------------------------------------------------------------------
bool GetPrg( char *Buf)
{
 
	if( !CmpSubStr( strlwr(Buf) , "prg" ) )
		return false;

	if( !(CommandFlags & SEND_SMS_PRG) )
			CommandFlags |= SEND_SMS_PRG;

	return true;
}
// ----------------------------------------------------------------------------------------------------------------------------------------------------------
bool  GetSmsNoPower(char *Buf, bool *Res)
{
	//Check if command is "PW=0 oe 1
	int Index;

	if( !CmpSubStr( strlwr(Buf) , "pw" ) )
		return false;

	Index = StrIndex(Buf,'=');
	
	if( !Index )
		return false;

	if(Buf[Index] != '0' && Buf[Index] != '1' )
		return false;

	if(Buf[Index] == '1')
		*Res = true;
	else
		*Res = false;

	return true;
}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------
// Parametres:
// char * BUf  input string with format: "Qx=y"  x=out number, y= 1(On); 0 (Off)
// char *Q      mask for output (0b00001000 where 1 is the output port number correspondent
// bool * Val    true or false for set or reset output port
// ----------------------------------------------------------------------------------------------------------------------------------------------------------
bool GetValOut(char *Buf, char *Q, bool *Val)
{
	bool Cmd = false;
	
	*Q = 0;
	*Val = false;

	if(	*Buf == 'Q' ||
		*Buf == 'q')
	{
		Cmd = true;
		Buf++;
	}
  
	if( Cmd )
	{
		if( !*Buf )
			return false;
		
		if(*Buf > 0x2F && *Buf < 0x3A )
		{
			*Q = 1;
			*Q <<= (atoi((char *)Buf++) -1 );

			if(!*Buf)
				return false;

			if(*Buf++ == '=')
			{
				if(*Buf == '0')
					*Val = false;
				else
					*Val = true;
				
				return true;
			}
		else
			return false;
		}
	else
		return false;
	}
	return false;
}
// ----------------------------------------------------------------------------------------------------------------------------------------------------------
//Check if the command is "RUN = 0/1" execution program
// ----------------------------------------------------------------------------------------------------------------------------------------------------------
bool ProgramOnOff( char *Buf, bool *Val)
{
	int Index;

	if( !CmpSubStr( strlwr(Buf) , "run" ) )
		return false;

	Index = StrIndex(Buf,'=');

	if( !Index )
		return false;

	Buf += Index;

	if( isdigit(*Buf) )
	{
		*Val = *Buf - 0x30;
		return true;
	}

	return false;
}
// ----------------------------------------------------------------------------------------------------------------------------------------------------------
bool GetClock(char *Buf, char *Day, char *Month, char *Year, char *Houre, char *Min, char *Sec)
{
	//Check if command is "TIME=dd/mm/yyhh:mm:ss"
	int Index;

	if( !CmpSubStr( strlwr(Buf) , "time" ) )
		return false;

	Index = StrIndex(Buf,'=');

	if( !Index )
		return false;

	Buf += Index;

	Index = 0;

	Day[Index++] = *Buf++;

	if( isdigit(*Buf) )
		Day[Index++] = *Buf++;

	if(atoi(Day) < 0 || atoi(Day) > 31)
		return false;

	if( *Buf++ != '/' )
		return false;

	Index = 0;

	Month[Index++] = *Buf++;

	if( isdigit(*Buf) )
		Month[Index++] = *Buf++;

	if(atoi(Month) < 0 || atoi(Month) > 12)
		return false;

	if( *Buf++ != '/' )
		return false;

	Index = 0;

	Year[Index++] = *Buf++;
	Year[Index++] = *Buf++;

	if(atoi(Year) < 0 )
		return false;

	Index = 0;

	Houre[Index++] = *Buf++;

	if( isdigit(*Buf) )
		Houre[Index++] = *Buf++;

	if(atoi(Houre) < 0 || atoi(Houre) > 23)
		return false;

	if( *Buf++ != ':' )
		return false;

	Index = 0;

	Min[Index++] = *Buf++;

	if( isdigit(*Buf) )
		Min[Index++] = *Buf++;

	if( atoi(Min) < 0 || atoi(Min) > 59)
		return false;

	if( *Buf++ != ':' )
		return false;

	Index = 0;

	Sec[Index++] = *Buf++;

	if( isdigit(*Buf) )
		Sec[Index++] = *Buf++;

	if( atoi(Sec) < 0 || atoi(Sec) > 59)
		return false;

	return true;
}
// ----------------------------------------------------------------------------------------------------------------------------------------------------------
//Check if the command is WEEK Temporal Event:
// "Txx=[yyy] 15:20:00"
//      [mon, tue, wed, thu, fri, sat, sun]
// ----------------------------------------------------------------------------------------------------------------------------------------------------------
bool  GetWeekTemporalEvent(char *Buf , DATA_EVENTS *Mask, uint32_t *Value, Week *DayWeek, DATA_EVENTS *NumEvent)
{
	char Tmp[] = {0,0,0,0};
	int  Val;

	//Remove Spaces
	DelChar(Buf, ' ');

	if( (*Buf != 'W' && *Buf != 'w') || *Buf == 0)
		return false;

	//extracts num event
	Buf++;

	if( !isdigit(*Buf) )
		return false;

	Tmp[0] = *Buf++;

	if( *Buf != '=' && !isdigit(*Buf) )
		return false;

	if( *Buf != '=' )
		Tmp[1] = *Buf++;

	*Mask = 1;
	Val = atoi(Tmp)-1;

	if(Val < 0 || Val > NUM_MAX_WEEK_EVENTS - 1)
		return false;

	//Memo num Event
	(*Mask) <<= Val;
	*NumEvent = Val;

	//extracts DateTime value
	if(*Buf++ == '=' )
	{
		memset(Tmp, 0 , sizeof(Tmp));

		Tmp[0] = *Buf++;
		Tmp[1] = *Buf++;
		Tmp[2] = *Buf++;

		if( CmpSubStr( strlwr(Tmp) , "mon" ) )
		{
			*DayWeek = Mon;
			*Value = DecodeTime(Buf);
			
			if(*Value)
				return true;
		}
		
		if( CmpSubStr( strlwr(Tmp) , "tue" ) )
		{
			*DayWeek = Tue;
			*Value = DecodeTime(Buf);
			
			if(*Value)
				return true;
		}
		
		if( CmpSubStr( strlwr(Tmp) , "wed" ) )
		{
			*DayWeek = Wed;
			*Value = DecodeTime(Buf);
			
			if(*Value)
				return true;
		}
		
		if( CmpSubStr( strlwr(Tmp) , "thu" ) )
		{
			*DayWeek = Thu;
			*Value = DecodeTime(Buf);
     
			if(*Value)
				return true;
		}
   
		if( CmpSubStr( strlwr(Tmp) , "fri" ) )
		{
			*DayWeek = Fri;
			*Value = DecodeTime(Buf);
			
			if(*Value)
				return true;
		}
   
		if( CmpSubStr( strlwr(Tmp) , "sat" ) )
		{
			*DayWeek = Sat;
			*Value = DecodeTime(Buf);
			
			if(*Value)
				return true;
		}
   
		if( CmpSubStr( strlwr(Tmp) , "sun" ) )
		{
			*DayWeek = Sun;
			*Value = DecodeTime(Buf);
     
			if(*Value)
				return true;
		}
	}
	return false;
}
// ----------------------------------------------------------------------------------------------------------------------------------------------------------
//Check if the command is Temporal Event: "Tx=21/05/09 15:20:00" or "Tx=15:20:00"
/* Mask is the correspondent bit for event number 1..32 [0..31]

*/
// ----------------------------------------------------------------------------------------------------------------------------------------------------------
bool  GetTemporalEvent(char *Buf , DATA_EVENTS *Mask, uint32_t *Value)
{
	char Tmp[] = {0,0,0,0};
	int  Val;

	if( (*Buf != 'T' && *Buf != 't') || *Buf == 0)
		return false;

	//extract event number
	Buf++;

	if( !isdigit(*Buf) )
		return false;
 
	Tmp[0] = *Buf++;

	if( *Buf != '=' && !isdigit(*Buf) )
		return false;

	if( *Buf != '=' )
		Tmp[1] = *Buf++;

	*Mask = 1;
	Val = atoi(Tmp)-1;

	if(Val < 0 || Val > NUM_MAX_EVENTS - 1)
		return false;

	(*Mask) <<= Val;

	//extract DateTime value
	if(*Buf++ == '=' )
	{
		*Value = DecodeDateTime(Buf, false);
		
		if(*Value)
			return true;
	}

	return false;
}
//------------------------------------------------------------------------------------------------------------------
// Decode Time from a Telit string "yy/mm/gg,23:53:22" (spaces in the middle doesn't matter)
/*
 32 bit : unsigned long int
  ------------------------------------------
  \ Year \ Month \ Day \ Hours \ Min \ Sec \
  ------------------------------------------
bit numbers
  -------------------------------------------
  \   6  \   4   \  5  \   6   \  6  \  5  \
  -------------------------------------------
*/
//------------------------------------------------------------------------------------------------------------------
// extracts only Time to int
//------------------------------------------------------------------------------------------------------------------
unsigned long int  DecodeTime(char *Date)
{
 unsigned long int Val    = 0;
 unsigned long int Res       ;
 char              Tmp[3] = {0,0,0};
 int               i      = 0;
 int               Idx       ;


	//"gg/mm/yy  23:53:22"
	Idx =  StrLastIndex(Date,'/');
	
	if( Idx )
		Date += (3 + Idx);

	if( !isdigit(*Date) )
	    Date++;

	if( isdigit(*Date) )
	{
		Tmp[i++] = *Date++;

		if( !isdigit(*Date) && *Date != ':')
			return 0;

		if(isdigit(*Date) )
			Tmp[i++] = *Date++;

		if(*Date++ != ':')
			return 0;

		//Decoded Hours
		Res  = atoi(Tmp);
		Val |= (Res << 11);
		i    = 0;
		memset(Tmp, 0 , sizeof(Tmp));

		if( !isdigit(*Date) )
			return 0;

		Tmp[i++] = *Date++;

		if( !isdigit(*Date) && *Date != ':' )
			return 0;

		if(isdigit(*Date) )
			Tmp[i++] = *Date++;

		if(*Date++ != ':')
			return 0;

		//Decoded Minutes
		Res  = atoi(Tmp);
		Val |= (Res << 5);
		i    = 0;
    
		memset(Tmp, 0 , sizeof(Tmp));

		if( !isdigit(*Date) )
			return 0;

		Tmp[i++] = *Date++;

		if(isdigit(*Date) )
			Tmp[i++] = *Date++;

		//Decoded Seconds
		Res  = atoi(Tmp);
		Val |= Res;
		memset(Tmp, 0 , sizeof(Tmp));

		}
		else 
			return 0;
			
	return Val;
}
//------------------------------------------------------------------------------------------------------------------
// Decode Date from a telit string "yy/mm/gg,23:53:22"
// Or Decode Date from string "gg/mm/yy  23:53:22" (spaces in the middle doesn't matter)
/*
 32 bit : unsigned long int
  ------------------------------------------
  \ Year \ Month \ Day \ Hours \ Min \ Sec \
  ------------------------------------------
bits
  -------------------------------------------
  \   6  \   4   \  5  \   6   \  6  \  5  \
  -------------------------------------------
*/
//------------------------------------------------------------------------------------------------------------------*/
unsigned long int  DecodeDate(char *Date, bool YearFirst)
{
 unsigned long int Val    = 0;
 unsigned long int Res       ;
 char              Tmp[3] = {0,0,0};
 int               i      = 0;

 //"yy/mm/gg  23:53:22"

	if(*Date == 0)
		return 0;

	if( !StrIndex(Date,'/') )
		return false;

	if( isdigit(*Date)  )
		Tmp[i++] = *Date++;

	if( !isdigit(*Date) && *Date != '/' )
		return 0;

	if(isdigit(*Date) )
		Tmp[i++] = *Date++;

	if(*Date++ != '/')
		return 0;

	if(YearFirst)
	{
		//Decoded year
		Res  = atoi(Tmp);
		Val |= (Res << 26);
	}
	else 
	{
		//Decoded Day
		Res  = atoi(Tmp);
		Val |= (Res << 17);
	}

	i    = 0;
	memset(Tmp, 0 , sizeof(Tmp));

	if( !isdigit(*Date)  )
		return 0;
 
	Tmp[i++] = *Date++;

	if( !isdigit(*Date) && *Date != '/'  )
		return 0;

	if( isdigit(*Date) )
		Tmp[i++] = *Date++;

	if(*Date++ != '/')
		return 0;

	//Decoded Month
	Res  = atoi(Tmp);
	Val |= (Res << 22);
	i    = 0;	
	
	memset(Tmp, 0 , sizeof(Tmp));

	if( !isdigit(*Date)  )
		return 0;

	Tmp[i++] = *Date++;

	if( !isdigit(*Date) )
		return 0;

	if( !isdigit(*Date) )
		return 0;

	Tmp[i++] = *Date++;

	if(!YearFirst)
	{
		//Decoded year
		Res  = atoi(Tmp);
		Val |= (Res << 26);
	}
	else 
	{
		//Decoded Day
		Res  = atoi(Tmp);
		Val |= (Res << 17);
	}

	return Val;
}
//------------------------------------------------------------------------------------------------------------------
unsigned long int  DecodeDateTime(char *Date, bool YearFirst)
{
	return  ( DecodeDate(Date, YearFirst ) | DecodeTime(Date) ) ;
}
//------------------------------------------------------------------------------------------------------------------
//Check if the command is MESSAGE SMS to memorize (and do it)
// SMSx "pippo pluto e paperino"
// ------------------------------------------------------------------------------
bool GetSMSMessageToEEmem(char *Buf)
{
	int Index;
	char Val;

	if( !CmpSubStr( strlwr(Buf) , "sms" ) )
		return false;
 
	Index = StrIndex(Buf,'\"');
	
	if( !Index )
		return false;

	if( !isdigit(Buf[3]) )
		return false;

	Val = (Buf[3]- 0x30);  //convert ascii char to int and find the char index

	//next char after \"
	Buf = &Buf[Index];

	//End String
	Index = StrIndex(Buf,'\"');
 
	if( !Index )
		return false;

	//Terminator on \"
	Buf[Index-1] = 0;

	//Verify MAX_STRING_SMS_CHAR max
	if( strlen(Buf) > MAX_STRING_SMS_CHAR)
		Buf[MAX_STRING_SMS_CHAR-1] = 0; //cut the string

	eeprom_write_block (Buf, &(SMSstrings[(int)Val]), strlen(Buf) + 1 );
	
	return true;
}
//------------------------------------------------------------------------------------------------------------------
//Check if the command is MESSAGE IO description to memorize (and do it)
/*
X = "xxx"	 X è un numero che identifica l'input o l'output :
1 = "xxx" A1
2 = "xxx" A2
3 = "xxx" I1 (per i dispositivi non programmabili questa stringa rappresenta anche quella d'allarme)
4 = "xxx" I2
5 = "xxx" I3
6 = "xxx" I4
7 = "xxx" Q1
8 = "xxx" Q2
9 = "xxx" Q3
10= "xxx" Q4
*/
// ------------------------------------------------------------------------------
bool GetIOMessageToEEmem(char *Buf)
{
	int Index;
	char Val;

	//del init spaces if any
	// while(*Buf == ' ' )
	//    Buf++;

	if( !CmpSubStr( strlwr(Buf) , "=" ) )
		return false;
 
	Index = StrIndex(Buf,'\"');
	
	if( !Index )
		return false;

	if( !isdigit(Buf[0]) )
		return false;

	if( isdigit( Buf[1]) )
	{
		Val =  ( Buf[0] - 0x30)*10;  //convert ascii char to int and find the char index
		Val += (Buf[1] - 0x30);
	}
	else
		Val = (Buf[0] - 0x30);  //convert ascii char to int and find the char index

	Val --;  //index in sms starts from 1

	//next char after \"
	Buf = &Buf[Index];

	//End String
	Index = StrIndex(Buf,'\"');
 
	if( !Index )
		return false;

	//Terminator on \"
	Buf[Index-1] = 0;

	//Verify MAX_STRING_CHAR_IO max
	if( strlen(Buf) >= MAX_STRING_CHAR_IO)
		Buf[MAX_STRING_CHAR_IO - 1] = 0; //cut the string

	eeprom_write_block (Buf, IOStrings[(int)Val], strlen(Buf) + 1);

	return true;
}
// ----------------------------------------------------------------------------------------------------------------------------------------------------------
bool GetOffsetPTC(char *Buf)
{
	int Index;
	float  Val;
	double Offset;

	if( !CmpSubStr( strlwr(Buf) , "offset" ) )
		return false;

	if( CmpSubStr(Buf,"1=") )
	{
		//offset 1
		Index = StrIndex(Buf,'=');
		
		if( !Index )
			return false;
		
		if ( sscanf( &Buf[Index], "%f", &Val) )
		{
			eeprom_read_block (&Offset, &Offset0_1, sizeof(double) );
			
			Offset += Val;
			
			eeprom_write_block(&Offset   , &Offset0_1, sizeof(double) );
			
			return true;
		}
		else 
			return false;
	}

	if( CmpSubStr(Buf,"2=") )
	{
		//offset 2
		Index = StrIndex(Buf,'=');
		
		if( !Index )
			return false;
	
		if( sscanf( &Buf[Index], "%f", &Val) )
		{
			eeprom_read_block (&Offset, &Offset2_3, sizeof(double) );
			Offset += Val;
			eeprom_write_block (&Offset, &Offset2_3, sizeof(double) );
		
			return true;
		}
	}
	
	return false;
}
// ----------------------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------
// check program Buf point to @
//------------------------------------------------------------------------------------------------------------------
/*
TESTS
	PIx, PQx	Fronte di salita dell'ingresso Ix (1..4) o dell'uscita Qx (1..4)
	PNIx, PNQx	Fronte di discesa dell'ingresso Ix (1..4) o dell'uscita Qx (1..4)
	Tx   dove x = 1..8
	A1 >xxx	        Ingresso analogico 1		xxx = -99 .. 999
	A1 <xxx	        Ingresso analogico 1
	A2 >xxx	        Ingresso analogico 2
	A2 <xxx	        Ingresso analogico 2
	A1 ><xxx yyy	Ingresso analogico 1 xxx < yyy
	A2 ><xxx yyy	Ingresso analogico 2 xxx < yyy

ACTIONS
	SENDC SMSxx		Invia l'sms numero xx (00-10); con conferma di chiamata
	SEND SMSxx  	Invia l'sms numero xx (00-10); prima voce in rubrica
    Qx = 0/1

*/
//------------------------------------------------------------------------------------------------------------------
bool CheckProgram( char *Buf)
{
	int  Index = 0;
	int  i = 0;

	Buf++;
  
	while(*Buf != '!' && i < MAX_PROG_STEP)
	{
		Index = StrIndex(Buf,';');
		
		if( Index )
			Buf[Index-1] = 0;

		if(! Analyze(Buf, i) )
			return false;

		if(!Index)
			//we finished
			break;

		Buf = &Buf[Index];
		i ++;
	}

	//Clear Program after i if any
	ClearProgramEEPROM( i );

	return true;
}
//------------------------------------------------------------------------------------------------------------------
void ClearProgramEEPROM( int Index )
{
	int i = 0;
	ProgramStep StepZero;

	if(Index < MAX_PROG_STEP -1 )
	{
		memset(&StepZero, 0 , sizeof(StepZero));

		for(i = Index; i < MAX_PROG_STEP; i++)
			eeprom_write_block( &StepZero, &(Program[i]), sizeof(StepZero));
	}
}
//------------------------------------------------------------------------------------------------------------------
bool Analyze(char *Buf, int Index)
{
	bool Ok = false;
	ProgramStep Step;
	char Tmp[4] = { 0,0,0,0 };
	int  ValMin, ValMax, i;
	char *BufAct;

	memset(&Step, 0, sizeof(Step));
	Step.Cmd = NotCommand;

	int IndexP = StrIndex(Buf,',');

	if( !IndexP )
		return false;

	//memo buffer to action
	BufAct = &Buf[IndexP];

	//Check if command is "PNIx"  x = port number
	// ------------------------------------------------------------------------
	if( CmpSubStr( strlwr(Buf) , "pni" ) &&
		Buf[3] < 0x35                    &&
		Buf[3] > 0x30     )
	{

		Step.Cmd = PNI;
		Step.CmdNum = Buf[3] - 0x30; //conversion to decimal
		Ok = true;
	}
   
	//Check if command is "PNQx"  x = port number
	// ------------------------------------------------------------------------
	if( !Ok &&
		CmpSubStr( strlwr(Buf) , "pnq" ) &&
		Buf[3] < 0x35                    &&
		Buf[3] > 0x30     )
	{

		Step.Cmd = PNQ;
		Step.CmdNum = Buf[3] - 0x30; //conversion to decimal
		Ok = true;
	}
	
	//Check if command is "PIx"  x = port number
	// ------------------------------------------------------------------------
	if( !Ok &&
		CmpSubStr( strlwr(Buf) , "pi" ) &&
		Buf[2] < 0x35                   &&
		Buf[2] > 0x30     )
	{
		Step.Cmd = PI;
		Step.CmdNum = Buf[2] - 0x30; //conversion to decimal
		Ok = true;
	}
	
	//Check if command is "PQx"  x = port number
	// ------------------------------------------------------------------------
	if( !Ok &&
		CmpSubStr( strlwr(Buf) , "pq" ) &&
		Buf[2] < 0x35                   &&
		Buf[2] > 0x30     )
	{
		Step.Cmd = PQ;
		Step.CmdNum = Buf[2] - 0x30; //conversion to decimal
		Ok = true;
	}
	
	//Check if command is "Txx"  xx = EVENT NUMBER
	// ------------------------------------------------------------------------
	if( !Ok && (*Buf == 'T' || *Buf == 't' ) )
	{
		Buf++;
		i = 0;
		memset(Tmp, 0 , sizeof(Tmp));
		
		if( isdigit( *Buf ) )
		{
			Tmp[i++] = *Buf++;
			
			if( isdigit( *Buf ) )
				Tmp[i++] = *Buf++;

			ValMin = atoi(Tmp); //conversion to decimal
			
			if( ValMin >= 0 && ValMin < NUM_MAX_EVENTS )
			{
				Step.Cmd = T;
				Step.CmdNum = ValMin;
				Ok = true;
			}
		}
	}
   
	//Check if command is "Wxx"  xx = EVENT NUMBER
	// ------------------------------------------------------------------------
	if( !Ok && ( *Buf == 'W' || *Buf == 'w' ) )
	{
		memset(Tmp, 0 , sizeof(Tmp));
		
		Buf++;
		i = 0;
		if( isdigit( *Buf ) )
		{
			Tmp[i++] = *Buf++;
			
			if( isdigit( *Buf ) )
				Tmp[i++] = *Buf++;
	
			ValMin = atoi(Tmp); //conversion to decimal
       
			if( ValMin >= 0 && ValMin < NUM_MAX_WEEK_EVENTS )
			{
				Step.Cmd = W;
				Step.CmdNum = ValMin;
				Ok = true;
			}
		}
	}

	//Check if command is "Ax><yyyzzz"  x = Analog input, yyy num zzz WARNING: Removed spaces!!!
	// ------------------------------------------------------------------------
	if(	!Ok &&
		(Buf[0] == 'A' || Buf[0] == 'a') &&
  		(Buf[1] == '1' || Buf[1] == '2') &&
		Buf[2] == '>'                    &&
		Buf[3] == '<') 
	{
		Step.Cmd = AInterval;
		Step.CmdNum = Buf[1] - 0x30; //conversion to decimal

		Buf += 4;
		i = 0;

		if( isdigit( *Buf ) || *Buf == '-')
			Tmp[i++] = *Buf++;

		if( isdigit( *Buf ) )
			Tmp[i++] = *Buf++;

		if( isdigit( *Buf ) )
			Tmp[i++] = *Buf++;

		ValMin = atoi(Tmp);

		if( *Buf != ':' )
			return false;

		Buf++;
		i = 0;
		
		memset(Tmp, 0, sizeof(Tmp) );

		if( isdigit( *Buf ) || *Buf == '-')
			Tmp[i++] = *Buf++;

		if( isdigit( *Buf ) )
			Tmp[i++] = *Buf++;

		if( isdigit( *Buf ) )
			Tmp[i++] = *Buf++;

		ValMax = atoi(Tmp);

		if( ValMin >= ValMax )
			return false;

		Step.MinVal = ValMin;
		Step.MaxVal = ValMax;
		Ok = true;
	}

	//Check if command is "Ax<yyy"  x = Analog input, yyy val
	// ------------------------------------------------------------------------
	if(	!Ok &&
		(Buf[0] == 'A' || Buf[0] == 'a') &&
  		(Buf[1] == '1' || Buf[1] == '2') &&
		Buf[2] == '<') 
	{
		Step.Cmd = AMinor;
		Step.CmdNum = Buf[1] - 0x30; //conversion to decimal

		Buf += 3;
		i = 0;
		
		if( isdigit( *Buf ) || *Buf == '-')
			Tmp[i++] = *Buf++;

		if( isdigit( *Buf ) )
			Tmp[i++] = *Buf++;

		if( isdigit( *Buf ) )
			Tmp[i++] = *Buf++;

		ValMin = atoi(Tmp);
		Step.MinVal = ValMin;
		Step.MaxVal = 0;
		Ok = true;
	}

	//Check if command is "Ax>yyy"  x = Analog input, yyy val
	// ------------------------------------------------------------------------
	if(	!Ok &&
		(Buf[0] == 'A' || Buf[0] == 'a') &&
  		(Buf[1] == '1' || Buf[1] == '2') &&
		Buf[2] == '>') 
	{
		Step.Cmd = AMajor;
		Step.CmdNum = Buf[1] - 0x30; //conversion to decimal

		Buf += 3;
		i = 0;
		
		if( isdigit( *Buf ) || *Buf == '-')
			Tmp[i++] = *Buf++;

		if( isdigit( *Buf ) )
			Tmp[i++] = *Buf++;

		if( isdigit( *Buf ) )
			Tmp[i++] = *Buf++;

		ValMin = atoi(Tmp);
		Step.MinVal = ValMin;
		Step.MaxVal = 0;
		Ok = true;
	}

	if(!Ok)
		return false;

	Ok = false;
	//Check Actions
	// ------------------------------------------------------------------------
	Buf = BufAct;
	memset(Tmp, 0, sizeof(Tmp) );

	//Check SENDCxx
	// ------------------------------------------------------------------------
	if(CmpSubStr( strlwr(Buf) , "sendc") )
	{
		Buf += 5;
		i = 0;

		if( isdigit( *Buf ) )
			Tmp[i++] = *Buf++;

		if( isdigit( *Buf ) )
			Tmp[i++] = *Buf++;

		ValMax = atoi(Tmp);

		if( ValMax < 0 || ValMax >= MAX_STRINGS )
			return false;

		Step.Act = SENDC;
		Step.ActNum = ValMax; // int inside char
		Ok = true;
	}
	else 
	{
		//Check SENDxx
		// ------------------------------------------------------------------------
		if(CmpSubStr( strlwr(Buf) , "send") )
		{
			Buf += 4;
			i = 0;

			if( isdigit( *Buf ) )
				Tmp[i++] = *Buf++;

			if( isdigit( *Buf ) )
				Tmp[i++] = *Buf++;

			ValMax = atoi(Tmp);
			
			if( ValMax < 0 || ValMax >= MAX_STRINGS )
				return false;

			Step.Act = SMS;
			Step.ActNum = ValMax; // int inside char
			Ok = true;
		}
	}

	//Check Qx=y
	// ------------------------------------------------------------------------
	if( CmpSubStr( strlwr(Buf) , "q"  ) && Buf[1] < 0x35 && Buf[1] > 0x30) 
	{
		Step.Act = Q;
		Step.ActNum = Buf[1] - 0x30; //conversion to decimal
		
		if( Buf[2] == '=' && ( Buf[3] == '0' ||  Buf[3] == '1' ) )
		{
			Step.Val = Buf[3] - 0x30; //conversion to decimal
			Ok = true;
		}
	}

	if(Ok)
	{
		eeprom_write_block (&Step, &(Program[Index]), sizeof(Step) );
		return true;
	}

	return false;
}
//------------------------------------------------------------------------------------------------------------------
void DelSpaces( char *Buf)
{
	char *Tmp;

	while(*Buf)
	{
		Tmp = Buf++;
		
		if (*Tmp == ' ') 
		{
			while( *Tmp++ ) *(Tmp-1) = *Tmp;
		}
	}
}
//------------------------------------------------------------------------------------------------------------------
// delete a char excluded if inserted between " . Example "not deleted"
//------------------------------------------------------------------------------------------------------------------
void DelChar( char *Buf, char Ch)
{
	char *Tmp;
	bool Shift = false;
	int  Idx;

	while(*Buf)
	{
		if(Shift)
		{
			Buf--;
			Shift = false;
		}

		Tmp = Buf++;
		
		if( *Tmp == '"')
		{
			//look for next "
			Idx = StrIndex(Buf,'"');
			
				if( Idx )
				//I take buffer at the end of "
				Buf = &Buf[Idx];
		}

		if( *Tmp == Ch ) 
		{
			while(*Tmp++)
				*(Tmp-1) = *Tmp;
			
			Shift = true;
		}
	}
}
//------------------------------------------------------------------------------------------------------------------
// Get day in the week
/*
 32 bit : unsigned long int
  ------------------------------------------
  \ Year \ Month \ Day \ Hours \ Min \ Sec \
  ------------------------------------------
bits
  -------------------------------------------
  \   6  \   4   \  5  \   6   \  6  \  5  \
  -------------------------------------------
*/

//------------------------------------------------------------------------------------------------------------------
Week  GetDayInWeek( unsigned long int DateTime )
{

	long int JDN;
	int Y, M, D;

	Y  =   (DateTime & 0xFC000000) >> 26;
	M  =   (DateTime & 0x03C00000) >> 22;
	D  =   (DateTime & 0x003E0000) >> 17;


	//Convert to Julian Date
	JDN    = 367 * Y - (7 * (Y + 5001 + (M - 9)/7))/4 + (275 * M)/9 + D + 1729777;
	
	return ( (JDN - 1) % 7 );
}
//------------------------------------------------------------------------------------------------------------------
/*
 32 bit : unsigned long int
  ------------------------------------------
  \ Year \ Month \ Day \ Hours \ Min \ Sec \
  ------------------------------------------
bits
  -------------------------------------------
  \   6  \   4   \  5  \   6   \  6  \  5  \
  -------------------------------------------
*/
//------------------------------------------------------------------------------------------------------------------
bool IsOnlyTime( unsigned long int DateTime )
{

	if( !(DateTime & 0xFE000000) )
		return true;

	return false;
}
//------------------------------------------------------------------------------------------------------------------
unsigned long int  GetTime( unsigned long int DateTime )
{
	return  DateTime & 0x0001FFFF;
}
//------------------------------------------------------------------------------------------------------------------
void EncodeDateTime( unsigned long int DateTime, char *Buf )
{
	int Y, M, D, Hh, Mm, Ss;

	Y  =   (DateTime & 0xFC000000) >> 26;
	M  =   (DateTime & 0x03C00000) >> 22;
	D  =   (DateTime & 0x003E0000) >> 17;
	Hh =   (DateTime & 0x0001F800) >> 11;
	Mm =   (DateTime & 0x000007E0) >> 5;
	Ss =    DateTime & 0x0000001F;

	sprintf(Buf, "%i/%i/%i %i:%i:%i\r\n", D,M,Y,Hh,Mm,Ss);

}
//------------------------------------------------------------------------------------------------------------------

