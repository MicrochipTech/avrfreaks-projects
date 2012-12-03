/*
   GSM_Card.c

   GSM_ControlSystem "GSA" V1.0.0  Copyright (C) 2012 Easy System

	*****************************************************************************
	*    >>> See http://www.esyste.com for Hw Documentation <<<              	*
	*                                                                        	*
	*    Thank you for using GSA, and thank you for your support!            	*
	*                                                                        	*
	*****************************************************************************
	see header file for details
*/
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#include <avr/io.h>
#include <avr/eeprom.h>
#include <avr/pgmspace.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <util/delay.h>
#include <avr/wdt.h>
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#include "FreeRTOS.h"
#include "Init.h"
#include "Define.h"
#include "a2d.h"
#include "Modem.h"
#include "Parser.h"
#include "ISR_Uart.h"
#include "ExecuteProgram.h"
#include "Ptc_KTY81-210.h"
#include "task.h"
#include "timers.h"
#include "GSM_Card.h"

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Functions
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

/// This task is the action to execute user program ADC conversion and filter I/O
//Create main continuous task to execute programs
void 	vTaskMain( void *pvParametres );

/// This task is a server for modem, progresses all RX queue from modem and prepare a general command
/// queue to be executed by a modem task
/// This Task has priority higher than all other tasks
void 	vPrepareCMDFromModem( void *pvParametres );

/// This task is the action for modem, progresses all general commands
/// and executes them
void vTaskManageModem( void *pvParametres );

//returns if a time is elapsed form an origin
bool 	IsTimeOver( portTickType , portTickType );

//Prepare status to send
void   	PrepareStatus(char *);

//Filter each input and power pin
void   	FilterIO(void);

//Check I/O status for included rising and falling edge
void   	CheckIn( bool , unsigned char , unsigned char );

// Read ADC channel and convert it to Temperature
double	ADCConvertToTemp( char );

// Read ADC channel and convert it to Temperature from a PTC
double 	ReadVadcPTC( char );

// calculate equivalent resistance from Volt Value read
double 	ReadVadcRo( char );

// send status to led to understand what modem is doing
void	StatusModemToLed(void);

// blink led
void	Blink( bool AddStartUpBlink, char NumBlink, bool Reset );

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// External variables
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
extern volatile	uint16_t GeneralStatusModem;//Status for blink led

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Global variables
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
volatile	unsigned char		MemVcc														;
volatile	unsigned long int	CurrentDateTime												;
volatile	bool 				RunProgram													; //status bit for modality execution
volatile	bool 				RunGateMode													; //status bit for modality execution
volatile	bool 				RunToggleRingMode											; //status bit for modality execution
volatile	char 				NumRingToggleRingMode										;
volatile	uint8_t 			StatusFlags													;//General status flags
volatile	uint16_t 			CommandFlags												;//Command flag to execute commands
			char				ProgramVers[]						EEMEM = "01.00"			;// GSA Version
			char				Amount[]							EEMEM = "000.00"		;
			uint8_t				GeneralModalityFlags				EEMEM = RUN_PROGRAM		;//enabled Run Program default
			uint32_t			DateTimeArray[NUM_MAX_EVENTS]		EEMEM					;//Date array for events
			uint32_t			WeekTimeArray[NUM_MAX_WEEK_EVENTS]	EEMEM					;//Date for Week events
			DATA_EVENTS			T0_To_n_Event						EEMEM = 0				;//to use for managing user events program
			DATA_EVENTS			T0_To_n_WeekEvent					EEMEM = 0				;//to use for managing user events program
			Week				DayInWeekEvent[NUM_MAX_WEEK_EVENTS]	EEMEM					;//Date for events inside a week
			double				Offset0_1							EEMEM = 0				; // °C
			double				Offset2_3							EEMEM = 0				; // °C
			short				AdcOffset0_1						EEMEM = -4				; // LSB
			short				AdcOffset2_3						EEMEM = -4				; // LSB
			GATE				GateMode							EEMEM = {0, 0}			;//to use for managing Gate mode execution
			TOGGLERING			ToggleRingMode						EEMEM = {0, 0, 0}		;//to use for managing ToggleRing mode execution
//-------------------------------------------------------------------------------------------------------------------------------------------------
//Allocated strings for I/O description
char	IOStrings [MAX_STRINGS][MAX_STRING_CHAR_IO]	EEMEM =	{	"A0",
																"A1",
																"I1",
																"I2",
																"I3",
																"I4",
																"Q1",
																"Q2",
																"Q3",
																"Q4"
															};
//--------------------------------------------------------------------------------------------------------------------------------------------------
// user strings
char	SMSstrings[MAX_STRINGS][MAX_STRING_SMS_CHAR] EEMEM ={	"Alarm Input I1 ON",
																"String 01 Empty",
																"String 02 Empty",
																"String 03 Empty",
																"String 04 Empty",
																"String 05 Empty",
																"String 06 Empty",
																"String 07 Empty",
																"String 08 Empty",
																"String 09 Empty"
															};
//--------------------------------------------------------------------------------------------------------------------------------------------------
//Step for program MAX_PROG_STEP = 20. Default Program
ProgramStep	Program[MAX_PROG_STEP] EEMEM = {	{PI        ,1,0,0,SENDC   ,0,0},
												{NotCommand,0,0,0,NoAction,0,0},
												{NotCommand,0,0,0,NoAction,0,0},
												{NotCommand,0,0,0,NoAction,0,0},
												{NotCommand,0,0,0,NoAction,0,0},
												{NotCommand,0,0,0,NoAction,0,0},
												{NotCommand,0,0,0,NoAction,0,0},
												{NotCommand,0,0,0,NoAction,0,0},
												{NotCommand,0,0,0,NoAction,0,0},
												{NotCommand,0,0,0,NoAction,0,0},
												{NotCommand,0,0,0,NoAction,0,0},
												{NotCommand,0,0,0,NoAction,0,0},
												{NotCommand,0,0,0,NoAction,0,0},
												{NotCommand,0,0,0,NoAction,0,0},
												{NotCommand,0,0,0,NoAction,0,0},
												{NotCommand,0,0,0,NoAction,0,0},
												{NotCommand,0,0,0,NoAction,0,0},
												{NotCommand,0,0,0,NoAction,0,0},
												{NotCommand,0,0,0,NoAction,0,0},
												{NotCommand,0,0,0,NoAction,0,0}
											};

//--------------------------------------------------------------------------------------------------------------------------------------------------
//Store the program in execution in ascii format
char	PrgNoConverted[SMS_ASCII_BUFFER_SIZE] EEMEM = { "PI1,SENDC0" };

//--------------------------------------------------------------------------------------------------------------------------------------------------
// I/O to manage fronts
//--------------------------------------------------------------------------------------------------------------------------------------------------
volatile	unsigned	char	PIn  ;						// rising edge input  front(0-7).
volatile	unsigned	char	PNIn ;						// falling edge input front (0-7).
volatile	unsigned	char	POut ;						// rising edge output  front(0-7).
volatile	unsigned	char	PNOut;						// falling edge output front (0-7).
//--------------------------------------------------------------------------------------------------------------------------------------------------
volatile 	bool				PVcc ;						// rising edge VCC
volatile 	bool				PNVcc;						// falling edge VCC
//--------------------------------------------------------------------------------------------------------------------------------------------------
volatile	double				T1,T2;						//input analog Temperature
volatile	xTimerHandle		IdTimer[2];
//--------------------------------------------------------------------------------------------------------------------------------------------------
volatile	portTickType		InFilterTime[NUM_FILTERS];	//TIMERS for input filter and power.


//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Variable To store  MCUSR
uint8_t mcusr_mirror __attribute__ ((section (".noinit")));
//BEFORE MAIN CALL PROGRAM
void get_mcusr(void) __attribute__((naked)) __attribute__((section(".init3")));
void get_mcusr(void)
{
  mcusr_mirror = MCUSR;
  MCUSR = 0;
  wdt_disable();
}
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
int main()
{
	/*Perform any hardware setup necessary*/
	prvSetupHardware();

	//Copy Program status
	RunProgram			= eeprom_read_byte(&GeneralModalityFlags) & RUN_PROGRAM;
	RunGateMode			= eeprom_read_byte(&GeneralModalityFlags) & GATE_MODE;
	RunToggleRingMode	= eeprom_read_byte(&GeneralModalityFlags) & TOGGLE_RING;

	/// This task is a server for modem, progresses all RX queue from modem and prepare a general command
	/// queue to be executed by a modem task
	/// This Task has priority higher than all other tasks
	xTaskCreate(	vPrepareCMDFromModem,		//pointer to task function
					"Modem",					//Name
					600,						// stack depth in word
					NULL,						// no parameters used
					configMAX_PRIORITIES-1,		// priority setting. Max Assigned
					NULL						// handle not used
					);

	/// This task is the action for modem, progresses all general commands
	/// and executes them
	xTaskCreate(	vTaskManageModem,			//pointer to task function
					"Modem",					//Name
					2600,						// stack depth in word
					NULL,						// no parameters used
					configMAX_PRIORITIES-2,		// priority setting. normal priority
					NULL						// handle not used
					);

	/// This task is the action to execute user program ADC conversion and filter I/O
	//Create main continuous task to execute programs
	xTaskCreate(	vTaskMain,					//pointer to task function
					"Main",						//Name
					800,						// stack depth in word
					NULL,						// no parameters used
					configMAX_PRIORITIES-2,		// priority setting
					NULL						// handle not used
					);


	/* In this port, to use preemptive scheduler define configUSE_PREEMPTION
	as 1 in portmacro.h.  To use the cooperative scheduler define
	configUSE_PREEMPTION as 0. */
	vTaskStartScheduler();

	/*reach here if there was insufficient heap to start scheduler */
	while(1);

	return 0;
}
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
/// This task is the action to execute user program ADC conversion and filter I/O
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void vTaskMain( void *pvParametres )
{
	portTickType	xCurrentTime;
	bool			InitProgram = true;

	CommandFlags= 0;
	StatusFlags	= 0;
	PIn			= 0;
	PNIn		= 0;
	PVcc		= false;
	PNVcc		= false;

	T1 = T2	= FAULT_TEMP_PTC;

	NumRingToggleRingMode = 0;

	//first power on doesn't send SMS
	//---------------------------------
	if( PIND & _BV(I_VCC_OK) )
		MemVcc |= BIT_0;

	if( !(PIND & _BV(I_VCC_OK)) )
		MemVcc |=  BIT_1;

	MemVcc |= BIT_2;
	//---------------------------------

	//check sim inserted
	if( !(PIND & _BV(I_GsmSimNotInserted)) )
		StatusFlags |= SIM_INSERTED;

	//memorize actual time
	xCurrentTime = xTaskGetTickCount();

	while(1)
	{
		//Filter Input values
		FilterIO();

		//check if execute program
		if( RunProgram )
		{
			ExecuteProgram( InitProgram );
			InitProgram = false;
		}

		//Fall down Power
		if( PNVcc )
		{
			if( (eeprom_read_byte(&GeneralModalityFlags) & SMS_NO_POWER) && !(CommandFlags & SEND_SMS_VCC) )
			{
				//Send SMS for power
				CommandFlags |=  SEND_SMS_VCC;
			}
		}

		//Restored Power
		if( PVcc )
		{
			if( (eeprom_read_byte(&GeneralModalityFlags) & SMS_NO_POWER) && !(CommandFlags & SEND_SMS_VCC) )
			{
				//Send SMS for power
				CommandFlags |=  SEND_SMS_VCC;
			}
		}

		// WARNING: portTICK_RATE_MS = 1000/configTICK_RATE_HZ = 1 in this specific case
		// portTickType for this CPU is integer (see portmacro.h) then 16BIT
		// MAXIMUM VALUE IS 65535 TICS = 65,5 SECONDS

		//each  10 seconds we call ADC temperature conversion when modem is not busy
		if( IsTimeOver( 10000/portTICK_RATE_MS, xCurrentTime ) && !(StatusFlags & MODEM_BUSY) )
		{
			StatusFlags |= ADC_CONVERSION_ON;

			T1 = ADCConvertToTemp( 1 );
			T2 = ADCConvertToTemp( 2 );

			StatusFlags &= ~ADC_CONVERSION_ON;

			//reset time
			xCurrentTime = xTaskGetTickCount();
		}

		//blink management
		//----------------------------------------------------------------------------------------------------------------------------------------------------------
		StatusModemToLed();
		
		//only to permit switch context. IS NOT PREEMPITIVE
		taskYIELD();

	} // END WHILE

} // END MAIN
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Read ADC and convert to temperature
/*
  TEORY:
  R = V / I

  VRadc  = V digital differential
  VRoadc = V digital to  Ro (1k 1%)

   Trough Ro there is the same current of PTC

  V = VRadc * VCC / 512
  I = VRoadc * VCC / (1024 * Ro)

  R = V / I =  Ro * [ VRadc * 1024 ]/[ VRoadc * 512 ]

  Ro = 1000;

  R =  2048 * (VRadc / VRoadc)

*/

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// ADC Conversion
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
double ADCConvertToTemp( char Channel )
{
	double  VRadc  =  ReadVadcPTC( Channel );
	double  VRoadc =  ReadVadcRo ( Channel );
	double  Offset, R, Temp, Toll;

	if( !VRadc )
		return FAULT_TEMP_PTC; // Fault Value ( we will see xx.xx in status SMS )

	if(Channel == 1 )
		eeprom_read_block (&Offset,     &Offset0_1,   sizeof(double) );
	if(Channel == 2 )
		eeprom_read_block (&Offset,     &Offset2_3,   sizeof(double) );

	if( VRoadc )
		R = 1000 * ( 2 * VRadc / VRoadc );
	else
		return FAULT_TEMP_PTC;

#ifdef PT100

	//   R = R0(1+aT); R = 100 + 100*(3,85*exp-3)*T
	return ((R - 100)*10)/(3.85) + Offset;

#else

	if( ConvRtoT( R, &Temp, &Toll ) )
		return Temp + Offset;
	else
		return FAULT_TEMP_PTC;

#endif
}
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// ADC converdione with PTC sensor
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
double ReadVadcPTC(char Channel)
{

	short Offset, adc, loop, ch, Max = -512, Min = 512;
	long  Val = 0;

	//wait 125us before changing channel
	_delay_us(130);

	if( Channel == 1 )
	{
		#ifdef PT100
		ch = ADC_CH_1_0_DIFF10X;
		#else
		ch = ADC_CH_0_1_DIFF1X;
		#endif
		eeprom_read_block (&Offset, &AdcOffset0_1, sizeof(short) );
	}
	if( Channel == 2 )
	{
		#ifdef PT100
		ch = ADC_CH_2_3_DIFF10X;
		#else
		ch = ADC_CH_3_2_DIFF1X;
		#endif
		eeprom_read_block (&Offset, &AdcOffset2_3, sizeof(short) );
	}

	//we need to discard the first n conversion: see datasheet
	loop = 3;
	while(loop--)
		a2dConvert10bit(ch);

	loop = LOOP_ADC;
	while(loop--)
	{
		adc = a2dConvert10bit(ch);
		if( adc )
			adc = adc + Offset;

		//two complement for 10 bit
		if( adc > 0x1FF )
			adc = ((~adc) & 0x3FF) + 1;

		if( adc > Max )
			Max = adc;

		if( adc < Min )
			Min = adc;

		Val += adc;
	}

	Val = Val - Max - Min;

 //Volt
	#ifdef PT100

	return (double)Val/(LOOP_ADC - 2) / 10;

	#else

	return (double)Val/(LOOP_ADC - 2);

	#endif

 }
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//Prepare status to send
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void PrepareStatus(char *Buf)
{
	char Temp[]  = "000.00";
	char Str[MAX_STRING_CHAR_IO];


	EncodeDateTime( CurrentDateTime, Buf );
/*
#ifdef PREPAYED
 cli();
 eeprom_read_block (Temp, Amount, 7 );
 sei();

 sprintf(&Buf[strlen(Buf)], "E:%s\r\n", Temp);
#endif
*/
 //Channel 1
	eeprom_read_block (Str, IOStrings[STR_IDX_A0], MAX_STRING_CHAR_IO );

	memset(Temp, 0, sizeof(Temp));
	if( T1 < -50 || T1 > 150  )
		//Sensor not connected
		strcpy( Temp, "xxx.xx");
	else
		dtostrf (T1, 4, 1, Temp);

	sprintf( &Buf[strlen(Buf)], "%s:%s\r\n", Str, Temp);

	//Channel 2
	eeprom_read_block (Str, IOStrings[STR_IDX_A1], MAX_STRING_CHAR_IO );

	memset(Temp, 0, sizeof(Temp));
	if( T2 < -50 || T2 > 150  )
		//Sensor not connected
		strcpy( Temp, "xxx.xx");
	else
		dtostrf (T2, 4, 1, Temp);

	sprintf( &Buf[strlen(Buf)], "%s:%s\r\n", Str, Temp);

	//IO ARE NC: WORK INVERTED
	//I1
	eeprom_read_block (Str, IOStrings[STR_IDX_I1], MAX_STRING_CHAR_IO );

	if( !(PINA & _BV(In_Ext_1)) )
		sprintf( &Buf[strlen(Buf)], "%s: On\r\n", Str);
	else
		sprintf( &Buf[strlen(Buf)], "%s: Off\r\n", Str);

	//I2
	eeprom_read_block (Str, IOStrings[STR_IDX_I2], MAX_STRING_CHAR_IO );

	if( !(PINA & _BV(In_Ext_2)) )
		sprintf( &Buf[strlen(Buf)], "%s: On\r\n", Str);
	else
		sprintf( &Buf[strlen(Buf)], "%s: Off\r\n", Str);

	//I3
	eeprom_read_block (Str, IOStrings[STR_IDX_I3], MAX_STRING_CHAR_IO );

	if( !(PINA & _BV(In_Ext_3)) )
		sprintf( &Buf[strlen(Buf)], "%s: On\r\n", Str);
	else
		sprintf( &Buf[strlen(Buf)], "%s: Off\r\n", Str);

	//I4
	eeprom_read_block (Str, IOStrings[STR_IDX_I4], MAX_STRING_CHAR_IO );

	if( !(PINA & _BV(In_Ext_4)) )
		sprintf( &Buf[strlen(Buf)], "%s: On\r\n", Str);
	else
		sprintf( &Buf[strlen(Buf)], "%s: Off\r\n", Str);

	//Q1
	eeprom_read_block (Str, IOStrings[STR_IDX_Q1], MAX_STRING_CHAR_IO );

	if(PINB & _BV(O_Rele_1))
		sprintf( &Buf[strlen(Buf)], "%s: On\r\n", Str);
	else
		sprintf( &Buf[strlen(Buf)], "%s: Off\r\n", Str);

	//Q2
	eeprom_read_block (Str, IOStrings[STR_IDX_Q2], MAX_STRING_CHAR_IO );

	if(PINB & _BV(O_Rele_2))
		sprintf( &Buf[strlen(Buf)], "%s: On\r\n", Str);
	else
		sprintf( &Buf[strlen(Buf)], "%s: Off\r\n", Str);

	//Q3
	eeprom_read_block (Str, IOStrings[STR_IDX_Q3], MAX_STRING_CHAR_IO );

	if(PINB & _BV(O_Rele_3))
		sprintf( &Buf[strlen(Buf)], "%s: On\r\n", Str);
	else
		sprintf( &Buf[strlen(Buf)], "%s: Off\r\n", Str);

	//Q4
	eeprom_read_block (Str, IOStrings[STR_IDX_Q4], MAX_STRING_CHAR_IO );

	if(PINB & _BV(O_Rele_4))
		sprintf( &Buf[strlen(Buf)], "%s: On\r\n", Str);
	else
		sprintf( &Buf[strlen(Buf)], "%s: Off\r\n", Str);

	if(PIND & _BV(I_VCC_OK))
		strcat(Buf, "Pw: On\r\n");
	else
		strcat(Buf, "Pw: Off\r\n");

	if(RunProgram && !RunGateMode && !RunToggleRingMode )
		strcat(Buf, "Progr: RUN");

	if(!RunProgram && !RunGateMode && !RunToggleRingMode )
		strcat(Buf, "Progr: OFF");

	if(!RunProgram && RunGateMode )
		strcat(Buf, "Progr: GATE");

	if(RunProgram && RunGateMode )
		strcat(Buf, "Progr: RUN & GATE");

	if(!RunProgram && RunToggleRingMode )
		strcat(Buf, "Progr: TOGLE");

	if(RunProgram && RunToggleRingMode )
		strcat(Buf, "Progr: RUN & TOGGLE");
}
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//Filter and I/O front management
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void FilterIO(void)
{

	//Only on for 1 cycle
	PVcc    = PNVcc   = false;

	// Rising and falling edge VCC
	if( !(PIND & _BV(I_VCC_OK)) && !(MemVcc & BIT_1) )
	{
		MemVcc |=  BIT_1;
		MemVcc &= ~BIT_2;
		MemVcc &= ~BIT_0;
		InFilterTime[IN_VCC] = (xTaskGetTickCount()*portTICK_RATE_MS); //current time in milliseconds
	}
	if( (PIND & _BV(I_VCC_OK)) && !(MemVcc & BIT_0) )
	{
		MemVcc |=  BIT_0;
		MemVcc &= ~BIT_2;
		MemVcc &= ~BIT_1;
		InFilterTime[IN_VCC] = (xTaskGetTickCount()*portTICK_RATE_MS); //current time in milliseconds
	}

	// 300 ms filter
	if(	IsTimeOver( VAL_FILTER_VCC, InFilterTime[IN_VCC]) 	&&
		(MemVcc & BIT_0)									&&
		!(MemVcc & BIT_2)									)
	{

		MemVcc |=  BIT_2;
		if( PIND & _BV(I_VCC_OK) )
			PVcc = true;
	}
	if(	IsTimeOver( VAL_FILTER_VCC, InFilterTime[IN_VCC]) 	&&
		(MemVcc & BIT_1)									&&
		!(MemVcc & BIT_2)									)
	{
		MemVcc |= BIT_2;
		if( !(PIND & _BV(I_VCC_OK)) )
			PNVcc = true;
	}
 // end Rising and falling edge VCC

 // Rising and falling edge In0-In3: INPUT NC
	CheckIn( !(PINA & _BV(In_Ext_1)), BIT_0,  IO_0);
	CheckIn( !(PINA & _BV(In_Ext_2)), BIT_1,  IO_1);
	CheckIn( !(PINA & _BV(In_Ext_3)), BIT_2,  IO_2);
	CheckIn( !(PINA & _BV(In_Ext_4)), BIT_3,  IO_3);

}
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Generate  input Rising and falling edge
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void CheckIn( bool ValIO, unsigned char MaskBit, unsigned char MaskIndex)
{
	static unsigned char	MemPIn = 0;
	static unsigned char	MemPNIn = 0;
	static unsigned char	MemIn = 0;
	static unsigned char	Val = 0;
	static unsigned char	MemFirstValNeg = 0xFF;

 /*
    inputs can be AC rectified
    Here the inputs are inverting configuration: no input = input on
    We can not filter On input because has not information
    It must then be filtered off to give a stable output that does not follow
    the alternating input.

    1) Frequency 50Hz:
         the input remains low up to a height of approximately 1.92 ms and 18.08 ms.
         Filter off with at least 10ms so we eliminate the pulse of 1.92 ms

    2) Frequency 60Hz:
         the input remains low up to a height of approximately 1.62 ms and 15.05 ms.
         Filter off with at least 10ms so we eliminate the pulse of 1.62 ms

   IS OK FOR BOTH THE FILTER FREQUENCIES
*/

	if( ValIO )
	{
		Val |= MaskBit;
		InFilterTime[MaskIndex] = (xTaskGetTickCount()*portTICK_RATE_MS); //current time in milliseconds
	}
	else
	{
		if( IsTimeOver( VAL_FILTER_IO, InFilterTime[MaskIndex]) )
		Val &= ~MaskBit;
	}

// Val is now filtered and run a real on / off regardless of the input AC

/*
	MAY PUT THE FILTERS ON INPUT FROM MANAGEMENT PROGRAM
   HERE, THE VALUE OF THE VALLEY OF DELAYS TO DO
*/

	//Only on for 1 cycle
	PIn     &= ~MaskBit;
	PNIn    &= ~MaskBit;

	if( !(Val & MaskBit) && !(MemPNIn & MaskBit) )
	{
		MemPNIn |=  MaskBit;
		MemPIn  &= ~MaskBit;
		MemIn   &= ~MaskBit;
	}

	if( (Val & MaskBit) && !(MemPIn & MaskBit) )
	{
		MemPNIn &= ~MaskBit;
		MemPIn  |=  MaskBit;
		MemIn   &= ~MaskBit;
	}

	if(	(MemPIn & MaskBit)	&&
		!(MemIn  & MaskBit)	)
	{
		if( Val & MaskBit )
			PIn   |= MaskBit;
		MemIn |= MaskBit;
	}

	if(	(MemPNIn & MaskBit)		&&
		!(MemIn   & MaskBit)	)
	{
		//eliminates the first negative edge on startup
		if(!(Val & MaskBit) )
		{
			if(!(MemFirstValNeg & MaskBit) )
				PNIn  |= MaskBit;
			else
				MemFirstValNeg &= ~MaskBit;
		}
		MemIn |= MaskBit;
	}
}
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// ADC conversion Read equivalent resistance
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
double ReadVadcRo( char Channel )
{
	int    i;
	long   Val = 0;
	short  Max = -1, loop, Tmp;

	//wait 130us before changing channel
	_delay_us(130);

	if( Channel == 1 )
		Channel = ADC_CH_ADC1;
	else
		Channel = ADC_CH_ADC2;

	loop = 3;
	while(loop--)
		a2dConvert10bit( Channel );

	loop = LOOP_ADC;

	//average loop
	for( i=0; i<loop; i++)
	{
		// ADC
		Tmp  = a2dConvert10bit( Channel );
		// PTC not connected
		if( !Tmp )
			return 0;

		if( Max < Tmp )
			Max = Tmp;

		Val += Tmp;
	}

	return   (double)(Val - Max)/(LOOP_ADC - 1)  ;
}
/* ----------------------------------------------------------------------------------------------------------------------------------------------
 * check if xAmountTime is elapset from an initial tima value
 * All time espressed in TIC
 * Maximum Tics evaluable is =oxFFFF
 * ----------------------------------------------------------------------------------------------------------------------------------------------
 */
bool IsTimeOver( portTickType xAmountTime, portTickType xInitTime )
{

	portTickType xCurrentTime = xTaskGetTickCount();

	// WARNING: portTICK_RATE_MS = 1000/configTICK_RATE_HZ = 1 in this specific case
	// portTickType for this CPU is integer (see portmacro.h) then 16BIT
	// MAXIMUM VALUE IS 65535 TICS = 65,5 SECONDS

	if( (	( xCurrentTime < xInitTime ) && ( xCurrentTime +(0xFFFF - xInitTime) >= xAmountTime ) )
		||	// no overflow counter 16 bit
		(	( xCurrentTime >= xInitTime ) && (xCurrentTime - xInitTime > xAmountTime  )) 
	  )
		return true;

	return false;

}
/* ----------------------------------------------------------------------------------------------------------------------------------------------
   send status to led to understand what modem is doing
   How is working: 
   blink is 0.30 sec on and 0.30 sec off always
   
   after a number of blink that mean a sense there is a pause for 0.75 seconds
   
	During startup each number of blink has 2 blink of 0.1 seconds (two on and tow off) meaning we are in startup
   
   GeneralStatusModem
	
	Startup: 2 fast blinks followed by
	
	POWER_ON_LED			1 blink
	CHECK_SIM_LED			2  "
	SEND_INIT_CMD_LED		3
	REGISTER_TO_NET_LED		4
	READ_BOOK_LED			5
	GET_CREDIT_LED			6
	STARTUP_ERROR_LED		7
	TIMESTAMP_LED			8

	Normal cycle: no fast blinks

	WAIT_FOR_COMMAND_LED		1
	INCOMING_SMS_MODEM_LED		2
	INCOMING_CALL_MODEM_LED		3
	MESSAGE_TO_READ_LED			4
	SEND_SMS_STATUS_LED			5
	SEND_SMS_STR_LED			6
	SEND_SMS_PRG_LED			7
	SEND_SMS_STR_CALL_LED		8
	SEND_SMS_VCC_LED			9
	ASK_FOR_NET_LED				10
	SEND_CREDIT_STRING_LED		11
	ASK_FOR_DATETIME_LED		12
 
*/ 
/* ----------------------------------------------------------------------------------------------------------------------------------------------*/
void StatusModemToLed( void )
{
	static uint16_t OldGeneralStatusModem = POWER_ON_LED;
	
	switch( GeneralStatusModem )
	{
		case POWER_ON_LED:
			
			if( OldGeneralStatusModem == GeneralStatusModem )
			{
				//startup + blink + no restart sequence
				Blink( true, 1, false );
			}
			else
			{
				OldGeneralStatusModem = GeneralStatusModem;
				//startup + blink + restart sequence
				Blink( true, 1, true );
			}
		
		break;
		//-----------------------------------------------------------------------------------------------------------------------

		//-----------------------------------------------------------------------------------------------------------------------
		case CHECK_SIM_LED:
		
			if( OldGeneralStatusModem == GeneralStatusModem )
			{
				//startup + blink + no restart sequence
				Blink( true, 2, false );
			}
			else
			{
				OldGeneralStatusModem = GeneralStatusModem;
				//startup + blink + restart sequence
				Blink( true, 2, true );
			}

		break;
		//-----------------------------------------------------------------------------------------------------------------------

		//-----------------------------------------------------------------------------------------------------------------------
		case SEND_INIT_CMD_LED:
		
			if( OldGeneralStatusModem == GeneralStatusModem )
			{
				//startup + blink + no restart sequence
				Blink( true, 3, false );
			}
			else
			{
				OldGeneralStatusModem = GeneralStatusModem;
				//startup + blink + restart sequence
				Blink( true, 3, true );
			}

		
		break;
		//-----------------------------------------------------------------------------------------------------------------------

		//-----------------------------------------------------------------------------------------------------------------------
		case REGISTER_TO_NET_LED:
		
			if( OldGeneralStatusModem == GeneralStatusModem )
			{
				//startup + blink + no restart sequence
				Blink( true, 4, false );
			}
			else
			{
				OldGeneralStatusModem = GeneralStatusModem;
				//startup + blink + restart sequence
				Blink( true, 4, true );
			}

		
		break;
		//-----------------------------------------------------------------------------------------------------------------------

		//-----------------------------------------------------------------------------------------------------------------------
		case READ_BOOK_LED:
		
			if( OldGeneralStatusModem == GeneralStatusModem )
			{
				//startup + blink + no restart sequence
				Blink( true, 5, false );
			}
			else
			{
				OldGeneralStatusModem = GeneralStatusModem;
				//startup + blink + restart sequence
				Blink( true, 5, true );
			}

		
		break;
		//-----------------------------------------------------------------------------------------------------------------------

		//-----------------------------------------------------------------------------------------------------------------------
		case GET_CREDIT_LED:
		
			if( OldGeneralStatusModem == GeneralStatusModem )
			{
				//startup + blink + no restart sequence
				Blink( true, 6, false );
			}
			else
			{
				OldGeneralStatusModem = GeneralStatusModem;
				//startup + blink + restart sequence
				Blink( true, 6, true );
			}

		
		break;
		//-----------------------------------------------------------------------------------------------------------------------

		//-----------------------------------------------------------------------------------------------------------------------
		case STARTUP_ERROR_LED:
		
			if( OldGeneralStatusModem == GeneralStatusModem )
			{
				//startup + blink + no restart sequence
				Blink( true, 7, false );
			}
			else
			{
				OldGeneralStatusModem = GeneralStatusModem;
				//startup + blink + restart sequence
				Blink( true, 7, true );
			}

		
		break;
		//-----------------------------------------------------------------------------------------------------------------------

		//-----------------------------------------------------------------------------------------------------------------------
		case TIMESTAMP_LED:
		
			if( OldGeneralStatusModem == GeneralStatusModem )
			{
				//startup + blink + no restart sequence
				Blink( true, 8, false );
			}
			else
			{
				OldGeneralStatusModem = GeneralStatusModem;
				//startup + blink + restart sequence
				Blink( true, 8, true );
			}

		
		break;
		//-----------------------------------------------------------------------------------------------------------------------

		//-----------------------------------------------------------------------------------------------------------------------
		case WAIT_FOR_COMMAND_LED:
		
			if( OldGeneralStatusModem == GeneralStatusModem )
			{
				//no startup + blink + no restart sequence
				Blink( false, 1, false );
			}
			else
			{
				OldGeneralStatusModem = GeneralStatusModem;
				//no startup + blink + restart sequence
				Blink( false, 1, true );
			}

		
		break;
		//-----------------------------------------------------------------------------------------------------------------------

		//-----------------------------------------------------------------------------------------------------------------------
		case INCOMING_SMS_MODEM_LED:

			if( OldGeneralStatusModem == GeneralStatusModem )
			{
				//no startup + blink + no restart sequence
				Blink( false, 2, false );
			}
			else
			{
				OldGeneralStatusModem = GeneralStatusModem;
				//no startup + blink + restart sequence
				Blink( false, 2, true );
			}
		
		break;
		//-----------------------------------------------------------------------------------------------------------------------

		//-----------------------------------------------------------------------------------------------------------------------
		case INCOMING_CALL_MODEM_LED:

			if( OldGeneralStatusModem == GeneralStatusModem )
			{
				//no startup + blink + no restart sequence
				Blink( false, 3, false );
			}
			else
			{
				OldGeneralStatusModem = GeneralStatusModem;
				//no startup + blink + restart sequence
				Blink( false, 3, true );
			}
		
		break;
		//-----------------------------------------------------------------------------------------------------------------------

		//-----------------------------------------------------------------------------------------------------------------------
		case MESSAGE_TO_READ_LED:

			if( OldGeneralStatusModem == GeneralStatusModem )
			{
				//no startup + blink + no restart sequence
				Blink( false, 4, false );
			}
			else
			{
				OldGeneralStatusModem = GeneralStatusModem;
				//no startup + blink + restart sequence
				Blink( false, 4, true );
			}
		
		break;
		//-----------------------------------------------------------------------------------------------------------------------

		//-----------------------------------------------------------------------------------------------------------------------
		case SEND_SMS_STATUS_LED:

			if( OldGeneralStatusModem == GeneralStatusModem )
			{
				//no startup + blink + no restart sequence
				Blink( false, 5, false );
			}
			else
			{
				OldGeneralStatusModem = GeneralStatusModem;
				//no startup + blink + restart sequence
				Blink( false, 5, true );
			}
		
		break;
		//-----------------------------------------------------------------------------------------------------------------------

		//-----------------------------------------------------------------------------------------------------------------------
		case SEND_SMS_STR_LED:

			if( OldGeneralStatusModem == GeneralStatusModem )
			{
				//no startup + blink + no restart sequence
				Blink( false, 6, false );
			}
			else
			{
				OldGeneralStatusModem = GeneralStatusModem;
				//no startup + blink + restart sequence
				Blink( false, 6, true );
			}
		
		break;
		//-----------------------------------------------------------------------------------------------------------------------

		//-----------------------------------------------------------------------------------------------------------------------
		case SEND_SMS_PRG_LED:

			if( OldGeneralStatusModem == GeneralStatusModem )
			{
				//no startup + blink + no restart sequence
				Blink( false, 7, false );
			}
			else
			{
				OldGeneralStatusModem = GeneralStatusModem;
				//no startup + blink + restart sequence
				Blink( false, 7, true );
			}
		
		break;
		//-----------------------------------------------------------------------------------------------------------------------

		//-----------------------------------------------------------------------------------------------------------------------
		case SEND_SMS_STR_CALL_LED:

			if( OldGeneralStatusModem == GeneralStatusModem )
			{
				//no startup + blink + no restart sequence
				Blink( false, 8, false );
			}
			else
			{
				OldGeneralStatusModem = GeneralStatusModem;
				//no startup + blink + restart sequence
				Blink( false, 8, true );
			}
		
		break;
		//-----------------------------------------------------------------------------------------------------------------------

		//-----------------------------------------------------------------------------------------------------------------------
		case SEND_SMS_VCC_LED:

			if( OldGeneralStatusModem == GeneralStatusModem )
			{
				//no startup + blink + no restart sequence
				Blink( false, 9, false );
			}
			else
			{
				OldGeneralStatusModem = GeneralStatusModem;
				//no startup + blink + restart sequence
				Blink( false, 9, true );
			}
		
		break;
		//-----------------------------------------------------------------------------------------------------------------------

		//-----------------------------------------------------------------------------------------------------------------------
		case ASK_FOR_NET_LED:

			if( OldGeneralStatusModem == GeneralStatusModem )
			{
				//no startup + blink + no restart sequence
				Blink( false, 10, false );
			}
			else
			{
				OldGeneralStatusModem = GeneralStatusModem;
				//no startup + blink + restart sequence
				Blink( false, 10, true );
			}
		
		break;
		//-----------------------------------------------------------------------------------------------------------------------

		//-----------------------------------------------------------------------------------------------------------------------
		case SEND_CREDIT_STRING_LED:

			if( OldGeneralStatusModem == GeneralStatusModem )
			{
				//no startup + blink + no restart sequence
				Blink( false, 11, false );
			}
			else
			{
				OldGeneralStatusModem = GeneralStatusModem;
				//no startup + blink + restart sequence
				Blink( false, 11, true );
			}
		
		break;
		//-----------------------------------------------------------------------------------------------------------------------

		//-----------------------------------------------------------------------------------------------------------------------
		case ASK_FOR_DATETIME_LED:

			if( OldGeneralStatusModem == GeneralStatusModem )
			{
				//no startup + blink + no restart sequence
				Blink( false, 12, false );
			}
			else
			{
				OldGeneralStatusModem = GeneralStatusModem;
				//no startup + blink + restart sequence
				Blink( false, 12, true );
			}
		
		break;
		//-----------------------------------------------------------------------------------------------------------------------

		//-----------------------------------------------------------------------------------------------------------------------
		
	}
	
}
/* ----------------------------------------------------------------------------------------------------------------------------------------------*/
/*
   blink is 0.30 sec on and 0.30 sec off always
   
   after a number of blink that mean a sense there is a pause for 0.75 seconds
   
   During startup each number of blink has 2 blink of 0.1 seconds (two on and tow off) meaning we are in startup
   
   bool AddStartUpBlink	: 2 blink 100 ms
   char NumBlink		: nr blink at 300 ms
   bool Reset			: restart sequence
   
*/   
/* ----------------------------------------------------------------------------------------------------------------------------------------------*/
#define TON_TOF_STARTUP		100		//ms
#define TON_TOF_CYCLE		300		//ms
#define PAUSE_LED			750		//ms
/* ----------------------------------------------------------------------------------------------------------------------------------------------*/
void Blink( bool AddStartUpBlink, char NumBlink, bool Reset )
{
	static portTickType		xCurrentTime;
	static char				CaseBlink = 0;
	static char				StartUpBlink;
	static char				NormalBlink;
	
	if( Reset )
		CaseBlink = 0;

	switch( CaseBlink )
	{
		//----------------------------------------------------------------------------------------------------------------
		// reset output
		case 0:
			
			xCurrentTime = xTaskGetTickCount();
			
			PORTC &= ~_BV(O_Led);
			
			CaseBlink++;
	
		break;
		//----------------------------------------------------------------------------------------------------------------
		
		//----------------------------------------------------------------------------------------------------------------
		// 
		case 1:
			
			if ( AddStartUpBlink )
			{
				StartUpBlink = 2;
				CaseBlink =10 ;
			}			
			else
				CaseBlink =20 ;

		break;
		//----------------------------------------------------------------------------------------------------------------
		
		//----------------------------------------------------------------------------------------------------------------
		//startup blink
		case 10:
				
				if( StartUpBlink )
				{
					PORTC |= _BV(O_Led);
				
					if( IsTimeOver( TON_TOF_STARTUP/portTICK_RATE_MS, xCurrentTime ) )
					{
						xCurrentTime = xTaskGetTickCount();
						StartUpBlink--;
						
						CaseBlink++;
					}	
				}
				else
					CaseBlink = 20; //end startup

		break;
		//----------------------------------------------------------------------------------------------------------------
		
		//----------------------------------------------------------------------------------------------------------------
		//startup blink
		case 11:
		
			PORTC &= ~_BV(O_Led);
		
			if( StartUpBlink )
			{
				if( IsTimeOver( TON_TOF_STARTUP/portTICK_RATE_MS, xCurrentTime ) )
				{
					xCurrentTime = xTaskGetTickCount();
					CaseBlink = 10;
				}
			}			
			else 
			{	//end startup blink. We need normal pause
				if( IsTimeOver( TON_TOF_CYCLE/portTICK_RATE_MS, xCurrentTime ) )
				{
					xCurrentTime = xTaskGetTickCount();
					CaseBlink = 10;
				}
			}			
		break;
		//----------------------------------------------------------------------------------------------------------------
		
		//----------------------------------------------------------------------------------------------------------------
		//normal blink
		case 20:
			
			NormalBlink = NumBlink;
			xCurrentTime = xTaskGetTickCount();
					
			CaseBlink++;

		break;
		//----------------------------------------------------------------------------------------------------------------
		
		//----------------------------------------------------------------------------------------------------------------
		//normal blink
		case 21:
		
			if( NormalBlink )
			{
				PORTC |= _BV(O_Led);
					
				if( IsTimeOver( TON_TOF_CYCLE/portTICK_RATE_MS, xCurrentTime ) )
				{
					xCurrentTime = xTaskGetTickCount();
					NormalBlink--;
						
					CaseBlink++;
				}
			}
			else
			{
				xCurrentTime = xTaskGetTickCount();
				CaseBlink = 30; //end CYCLE
			}			

		break;
		//----------------------------------------------------------------------------------------------------------------
		
		//----------------------------------------------------------------------------------------------------------------
		//normal blink
		case 22:
		
			PORTC &= ~_BV(O_Led);
		
			if( IsTimeOver( TON_TOF_CYCLE/portTICK_RATE_MS, xCurrentTime ) )
			{
				xCurrentTime = xTaskGetTickCount();
				CaseBlink = 21;
			}

		break;
		//----------------------------------------------------------------------------------------------------------------
		
		//----------------------------------------------------------------------------------------------------------------
		//End: include pause
		case 30:
		
			PORTC &= ~_BV(O_Led);
		
			if( IsTimeOver( PAUSE_LED/portTICK_RATE_MS, xCurrentTime ) )
			{
				CaseBlink = 0;
			}

		break;
		//----------------------------------------------------------------------------------------------------------------
	}		

}
/* ----------------------------------------------------------------------------------------------------------------------------------------------*/
