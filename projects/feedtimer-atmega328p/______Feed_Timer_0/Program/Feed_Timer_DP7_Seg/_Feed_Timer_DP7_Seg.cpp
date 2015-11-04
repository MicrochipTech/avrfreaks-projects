/*
 /////////////////////////////////////////////////////////////////////////////////////////

		______            _   _____ _                       ______  _____
		|  ___|          | | |_   _(_)                     |___  / /  ___|
		| |_ ___  ___  __| |   | |  _ _ __ ___   ___ _ __     / /  \ `--.  ___  __ _
		|  _/ _ \/ _ \/ _` |   | | | | '_ ` _ \ / _ \ '__|   / /    `--. \/ _ \/ _` |
		| ||  __/  __/ (_| |   | | | | | | | | |  __/ |    ./ /    /\__/ /  __/ (_| |
		\_| \___|\___|\__,_|   \_/ |_|_| |_| |_|\___|_|    \_/     \____/ \___|\__, |
																				__/ |
																				|___/

 /////////////////////////////////////////////////////////////////////////////////////////
 --------------------------------- Main Program (tab 4) C++ ------------------------------ 

==========================================================================================
 @AUTOR   : João D´Artagnan Antunes Oliveira
 @DATA    : 1 DE SETEMBRO DE 2015
 @HORÁRIO : 18:04
==========================================================================================
 @PROPOSTA: GERENCIAR ATRAVEZ DE TEMPORIZADORES HORA PARA LIGAR O MOTOR DE ALIMENTAÇÃO,
			RAÇÃO POR PERIODO PROGRAMADO
 @HARDWARE: 
 @OBSERVAÇÃO:
==========================================================================================
         ______                                      _                 
         | ___ \                                    | |                
         | |_/ / __ ___   __ _ _ __ __ _ _ __ ___   | |     ___   __ _ 
         |  __/ '__/ _ \ / _` | '__/ _` | '_ ` _ \  | |    / _ \ / _` |
         | |  | | | (_) | (_| | | | (_| | | | | | | | |___| (_) | (_| |
         \_|  |_|  \___/ \__, |_|  \__,_|_| |_| |_| \_____/\___/ \__, |
                          __/ |                                   __/ |
                         |___/                                   |___/ 
==========================================================================================
DATA		HORA		DESCRIÇÃO
__________	_____		__________________________________________________________________________
01/09/2015	18:10		Projeto do Sofware Hardware já pronto há 5 dias.

*/

// 
//  +--------------------------------------+
//  |        AVR/GNU C++ COMPILER	       |
//  +--------------------------------------+
//  
//  Symbols			= -DF_CPU=1000000L -D_TIME_TIMER_NUMBER_=0
//  Optimization	= -Os
//  Miscellaneous	= -std=gnu++11
// 

// 
//  +--------------------------------------+
//  |        GLOBALS INCLUSION		       |
//  +--------------------------------------+
// 

//#include "____GLOBAL/_Global.h"

#define _FEED_TIMER_7_SEGMENTS_VERSION    127

// 
//  +--------------------------------------+
//  |        HARDWARE DEPENDENT INCLUDES   |
//  +--------------------------------------+
// 
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include <avr/sleep.h>
#include <avr/pgmspace.h>
// 
//  +--------------------------------------+
//  |        C LIBRARY INCLUDES		       |
//  +--------------------------------------+
// 
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

// 
//  +--------------------------------------+
//  |        LOCAL LIBRARY INCLUDES	       |
//  +--------------------------------------+
// 

#include "_Feed_Timer_IO.h"
#include "_Time_Lib.h"
 
// 
//  +--------------------------------------+
//  |        GLOBAL CLASSES          	   |
//  +--------------------------------------+
// 

class_Time			time;
class_Display_7_Seg	dp;

//
//  +--------------------------------------+
//  |       ENUMS DEFINITIONS		       |
//  +--------------------------------------+
//
enum class enum_CONTROL:uint8_t
{
	FS=0X1C,
	GS,
	RS,
	US	
};

enum class enum_BOOT_TYPE:uint8_t
{
	COLD,
	HOT
};

typedef enum class enum_EVENT_TYPE
{
	EMPTY,
	REPEAT,
	DIARY,
	WEEKLY	
} event_Type;

typedef enum  class enum_MENU_ITEM
{
	SET_TIME=1,
	INCLUDE,
	DELETE,
	RESET,
	EXIT	
} menu_Item;

typedef enum  class enum_INC_OPTION
{
	REPEAT=1,
	DIARY,
	WEEKLY,
	EXIT	
} inc_Option;
enum class enum_WEEK_DAY:uint8_t
{
	MONDAY=1,
	TUESDAY,
	WEDNESDAY,
	THUSDAY,
	FRIDAY,
	SATURDAY,
	SUNDAY,	
};
//
//  +--------------------------------------+
//  |        LOCAL DEFINITIONS		       |
//  +--------------------------------------+
//

#define		NOP asm volatile("nop")

//#define     _MENU_ENGLISH_

const uint32_t _BOOT_KEY				=0x31415926;		//Hot Boot Key Value PI
const uint8_t  EVENT_SIZE				=100;				//Max number of events is 100

const uint32_t KEY_DELAY				=350;				//Set Normal Key Delay to 350 Milliseconds
const uint32_t KEY_DELAY_RAPID			=150;				//Set Rapid Delay to 150 milliseconds
const uint32_t TIME_TO_EXIT				=30000;				//Set Time To Exit to 30 seconds
const uint32_t TIME_TO_BLINK			=500;				//Set Blink Time to 0,5 second
const uint32_t TIME_TO_RAPID			=2000;				//Set Time to Enter in Rapid
const uint32_t TIME_EVENT_NUMBER_SHOW	=2000;				//Set Time to Show Event Number 2.0 Seconds
const uint32_t TIME_TO_SHOW_ERROR		=2000;				//Set Time to Show Event Error to 2.0 Seconds

//
//  +--------------------------------------+
//  |        LOCAL GLOBAL STRUCTS	       |
//  +--------------------------------------+
//
struct EventStruct
{
	uint8_t Event,							//Event may be Repeat,Diary,Weekly
			Hour,								//in repeat mode hour,minute event duration in other modes hour,Minute start time of event
			Minute,
			WeekDays,							//BIT 0=Monday,BIT 1=TUESDAY,..,BIT 7=SUNDAY
			MinuteMotorON,
			SecondMotorON,
			XorCheck;							//xor from Event to SecondMotorON
};
union EventFlagsUnion
{
	struct  
	{
		bool	LoadFrom:1;						//false load from Hour,Minute true load from MinuteMotorON,SecondMotorOn
		bool	Counting:1;						//true if counting
	} SeparateFlag;
	uint8_t		AllFlags;	
};
struct EventCountStruct
{
	union EventFlagsUnion Flags;
	uint32_t Count;
};
struct PtrStruct
{
	uint8_t		Hour,
				Minute,
				Second,
				WeekDay;
	uint32_t	BootKey;
};
//
//  +--------------------------------------+
//  |        EEPROM VARIABLES		       |
//  +--------------------------------------+
//
uint8_t					EE_EventsSize EEMEM=0;
struct EventStruct		EE_Events[EVENT_SIZE] EEMEM;
struct EventCountStruct EventsCount[EVENT_SIZE];

//
//  +--------------------------------------+
//  |        LOCAL GLOBAL VARIABLES	       |
//  +--------------------------------------+
//
char		bf[10];
uint32_t	tExit;							//hold Time To Exit from Input Routines
uint32_t	tBlink;							//hold Time to Flash Display
/*
        +-------------------------------------------------------+
        |				CLASS FEED TIMER                        |
        +-------------------------------------------------------+
        | @PURPOSE      : GENERAL FUNCTIONS FOR WORKING FEED	|
		|				  TIMER APPROPRIATE						|
        | @OBSERVATIONS : use Timer0 and Timer2 as resource		|
        +-------------------------------------------------------+
*/

class class_Feed_Timer
{
/*
         +--------------------------------------+
         |       PRIVATE VARIABLES		        |
         +--------------------------------------+
        */
 private:
		static	char	sout[5];
        /*
         +--------------------------------------+
         |       PRIVATE FUNCTIONS      	    |
         +--------------------------------------+
        */

        /*
         +--------------------------------------+
         |        PUBLIC FUNCTIONS		        |
         +--------------------------------------+
        */
public:
		static volatile struct PtrStruct *Ptrs;
		static volatile uint32_t	TickCount;
		static volatile bool		SleepState;
		static volatile	bool		MenuMode;

		/************************************************************************/
		/* Constructor                                                          */
		/************************************************************************/
        class_Feed_Timer(void);
		/************************************************************************/
		/* Clock call back function to keep timer running                       */
		/************************************************************************/
		static void	ClockCallBack(void);
		/************************************************************************/
		/* Get Time hour,minute,second and weekday                              */
		/************************************************************************/
		void	GetTime(uint8_t &hourParm,uint8_t &minuteParm,uint8_t &secondParm,uint8_t &weekdayParm);
		/************************************************************************/
		/* Set time hour,minute,second and weeday                               */
		/************************************************************************/
		void	SetTime(uint8_t &hourParm,uint8_t &minuteParm,uint8_t &secondParm,uint8_t &weekdayParm);
		/************************************************************************/
		/* Initialize time                                                      */
		/************************************************************************/
		void	InitTime();
		/************************************************************************/
		/* Get Week Day String (4 chars ) seg,ter,...dom                        */
		/************************************************************************/
		const char * GetWeekDayStr(uint8_t weekDay);
		/************************************************************************/
		/* Get Week Day String Short (2 chars ) se,te,...do                     */
		/************************************************************************/
		const char * GetWeekDayShortStr(uint8_t weekDayParm);
		/************************************************************************/
		/* Set Timer 0 Disable                                                  */
		/************************************************************************/
		void inline SetTimer0_OFF()
		{
			//disable TIMER 0 caution when change TIMER drive TIMER USED to update bellow software, in this case dont be a TIMER2
			TIMSK0&=~(1<<OCIE0A);
		}
		/************************************************************************/
		/* Set Timer 0 Enable                                                   */
		/************************************************************************/
		void inline SetTimer0_ON()
		{
			//disable TIMER 0 caution when change TIMER drive TIMER USED to update bellow software, in this case don't be a TIMER2
			TIMSK0|=(1<<OCIE0A);
		}
		/************************************************************************/
		/* Show a sequence left to right of display point each 10 second for    */
		/* advice that feed time is operational									*/
		/************************************************************************/
		void	ShowLIVE(void);
		/************************************************************************/
		/* Process input keys and show and control according this inputs        */
		/************************************************************************/
		void	ProcessInputs(void);
		/************************************************************************/
		/* Add an Event                                                         */
		/************************************************************************/
		bool	EventAdd(struct EventStruct &Event);	
		/************************************************************************/
		/* Delete an Event                                                      */
		/************************************************************************/
		bool	EventDelete(uint8_t eventNumber);
		/************************************************************************/
		/************************************************************************/
		/* Show Menu Item String                                                */
		/************************************************************************/
		const char * GetMenuItemStr(menu_Item menuItemNumber);
		/************************************************************************/
		/* Show Inclusion Option String                                         */
		/************************************************************************/
		const char * GetIncOptionStr(inc_Option incOptionNumber);
		/************************************************************************/
		/* Show Event Type String			                                    */
		/************************************************************************/
		const char * GetEventTypeStr(uint8_t eventTypeNumber);
		/************************************************************************/
		/* Clear Screen,Disable Timer 0,Wait 10 ms			                    */
		/************************************************************************/
		void	ClearAndExit(void);
		/************************************************************************/
		/* Input Hour and Minute return true if parms entered ok                */
		/************************************************************************/
		bool	InputHourMinOrMinSec(uint8_t &hourMin,uint8_t &minSec,bool inputHourMin=true,bool newInput=true);
		/************************************************************************/
		/* Input WeekDay	1=Monday,2=Tuesdays,7=Sunday  true if input ok      */			
		/************************************************************************/
		bool	InputWeekDay(uint8_t &weekDay,bool newInput=true);	
		/************************************************************************/
		/* Input WeekDays weekDay bit0=1 Monday set,..,bit7=1 Sunday set        */
		/************************************************************************/
		bool	InputWeekDays(uint8_t &weekDays,bool newInput=true);		
		/************************************************************************/
		/* Show Event Number format EnnT nn=Event Number,T=Event Type           */
		/************************************************************************/
		void	ShowEventNumber(uint8_t eventNumber);
		/************************************************************************/
		/* Show Week Days                                                       */
		/************************************************************************/
		void	ShowWeekDays(uint8_t weekDays);
		/************************************************************************/
		/* Get Battery voltage                                                  */
		/************************************************************************/
		int16_t		GetVBat(void);
		uint16_t	GetVBatAverage(void);
		uint16_t	GetVBatPercent(void);
};

/*
    +--------------------------------------+
    |        SET STATIC CLASS VARIABLES    |
    +--------------------------------------+
*/

volatile struct PtrStruct * class_Feed_Timer::Ptrs=(struct PtrStruct*)(RAMEND-256);
volatile uint32_t			class_Feed_Timer::TickCount=0;							//Tick count pump eatch second
volatile bool				class_Feed_Timer::SleepState=true;	
char						class_Feed_Timer::sout[5]="0000";
volatile bool				class_Feed_Timer::MenuMode=true;


//	+-------------------------------------------------------------------------------+
//	| @FUNCTION:  FeedTimer constructor							                    |
//	| @NAME    :  Constructor			          		                            |
//	| @INPUT   :				  													|
//	| @OUTPUT  :  none								                                |
//	| @REMARKS :																	|
//	+-------------------------------------------------------------------------------+
class_Feed_Timer::class_Feed_Timer()
{
	dp.SetSecondCallBack(class_Feed_Timer::ClockCallBack);
}
//	+-------------------------------------------------------------------------------+
//	| @FUNCTION:  ClockCallBack process							                    |
//	| @NAME    :  ClockCallBack			          		                            |
//	| @INPUT   :				  													|
//	| @OUTPUT  :  none								                                |
//	| @REMARKS :																	|
//	+-------------------------------------------------------------------------------+
void	class_Feed_Timer::ClockCallBack()
{
	//////////////////////////////
	//PROCESS TIME
	//////////////////////////////
	//pump tick count
	//TickCount++;
	//pump second
	Ptrs->Second++;
	//if greater 59
	if (Ptrs->Second>59)	{
		//clear second
		Ptrs->Second=0;
		//pump minute
		Ptrs->Minute++;
		//if greater 59
		if (Ptrs->Minute>59)	{
			//clear minute
			Ptrs->Minute=0;
			//pump hour
			Ptrs->Hour++;
			//if greater 23
			if (Ptrs->Hour>23)  {
				//clear hour
				Ptrs->Hour=0;
				//pump week day
				Ptrs->WeekDay++;
				//if greater 7
				if (Ptrs->WeekDay>7)	Ptrs->WeekDay=1; //Set to Monday=1
			}
		}
	}
	/////////////////////////////
	/// Enable interrupts to avoid long time in this callback
	/////////////////////////////
	sei();
	/////////////////////////////
	//PROCESS EVENTS
	/////////////////////////////
	struct EventStruct		*EventP,EventTmp;
	struct EventCountStruct *EventCountP;
	event_Type				 Event;
	int8_t					 EventSize;
	//Get Events Size
	EventSize=eeprom_read_byte(&EE_EventsSize);
	//Get pointers to first elements
	EventP=&EE_Events[0];
	EventCountP=&EventsCount[0];
	//Scan all existents Events
	for(uint8_t i=0;i<EventSize;i++)
	//for(uint8_t i=0;i<EVENT_SIZE;i++)
	{
		Event=static_cast<event_Type>(eeprom_read_byte(&EventP->Event));
		switch (Event)
		{
			////////////////////////
			// REPEAT EVENT
			////////////////////////
			case event_Type::REPEAT:
				//time to load from hours,minute?
				if (EventCountP->Flags.SeparateFlag.LoadFrom==false && EventCountP->Count==0)
				{
					//yes, update count from hours and minute
					eeprom_read_block(&EventTmp,&EventP->Event,sizeof(EventTmp));
					//translate to seconds and load count
					EventCountP->Count=((uint16_t)EventTmp.Hour*60+(uint16_t)EventTmp.Minute)*60L;
					//translate to seconds motor on and subtract from hour and minutes
					EventCountP->Count-=((uint16_t)EventTmp.MinuteMotorON*60+(uint16_t)EventTmp.SecondMotorON);
					//set flag to true
					EventCountP->Flags.SeparateFlag.LoadFrom=true;

					//if second event the turn motor off
					if (EventCountP->Flags.SeparateFlag.Counting)
					{
						//set motor OFF
						dp.MotorOFF();
						//sleep
						SleepState=true;						
					}
					else
					{
						//set counting true
						EventCountP->Flags.SeparateFlag.Counting=true;
					}
				}
				else
				{
					//time to load from MinuteMotorON,SecondMotorON?
					if (EventCountP->Flags.SeparateFlag.LoadFrom==true && EventCountP->Count==0)
					{
						//yes, update count from MinuteMotorON,SecondMotorON
						eeprom_read_block(&EventTmp,&EventP->Event,sizeof(EventTmp));
						//translate to seconds and load count
						EventCountP->Count=((uint16_t)EventTmp.MinuteMotorON*60+(uint16_t)EventTmp.SecondMotorON);
						//set flag to false
						EventCountP->Flags.SeparateFlag.LoadFrom=false;
						EventCountP->Flags.SeparateFlag.Counting=true;
						//set motor ON
						dp.EnableIO();
						dp.MotorON();
						SleepState=false;
					}
					else
					{
						//decrement count
						EventCountP->Count--;
					}
				}
			break;
			////////////////////////
			// DIARY OR WEEKLY EVENT
			////////////////////////
			case event_Type::DIARY:
			case event_Type::WEEKLY:
				//check if time arise
				eeprom_read_block(&EventTmp,&EventP->Event,sizeof(EventTmp));
				//check time
				if (EventTmp.Hour==Ptrs->Hour && EventTmp.Minute==Ptrs->Minute && Ptrs->Second==0 && (EventTmp.WeekDays & (1<<(Ptrs->WeekDay-1))))
				{
					//translate to seconds and load count
					EventCountP->Count=((uint16_t)EventTmp.MinuteMotorON*60+(uint16_t)EventTmp.SecondMotorON);
					//set flag to false
					EventCountP->Flags.SeparateFlag.LoadFrom=true;
					EventCountP->Flags.SeparateFlag.Counting=true;
					//set motor ON
					dp.EnableIO();
					dp.MotorON();
					SleepState=false;					
				}
				else
				{
					if (EventCountP->Flags.SeparateFlag.LoadFrom==true && EventCountP->Count==0)
					{
						//yes, disable motor
						EventCountP->Flags.SeparateFlag.LoadFrom=false;
						EventCountP->Flags.SeparateFlag.Counting=false;
						dp.MotorOFF();
						SleepState=true;
					}
					else
					{
						//count--
						if (EventCountP->Flags.SeparateFlag.Counting) EventCountP->Count--;
					}
				}
			break;	
			////////////////////////
			// EMPTY EVENTY
			////////////////////////
			case event_Type::EMPTY:			
			break;
		}
		//next event
		EventP++;
		EventCountP++;		
	}
}
//	+-------------------------------------------------------------------------------+
//	| @FUNCTION:  Get FeedTimer Time							                    |
//	| @NAME    :  GetTime				          		                            |
//	| @INPUT   :				  													|
//	| @OUTPUT  :  hour,minute,second,weeday			                                |
//	| @REMARKS :																	|
//	+-------------------------------------------------------------------------------+
void	class_Feed_Timer::GetTime(uint8_t &hourParm,uint8_t &minuteParm,uint8_t &secondParm,uint8_t &weekdayParm)
{
	uint8_t	h1,m1,s1,w1;
	uint8_t h2,m2,s2,w2;
	
	//loop for correct get time without interrupt

	//get time 
	h1=Ptrs->Hour;
	m1=Ptrs->Minute;
	s1=Ptrs->Second;
	w1=Ptrs->WeekDay;
	for(;;)
	{
		//get time again to check if no has updated
		h2=Ptrs->Hour;
		m2=Ptrs->Minute;
		s2=Ptrs->Second;
		w2=Ptrs->WeekDay;
		//check if igual then break if yes
		if (h1==h2 && m1==m2 && s1==s2 && w1==w2) break;
		//update first time to check changes
		h1=h2;
		m1=m2;
		s1=s2;
		w1=w2;
	}
	//return correct time
	hourParm=h2;
	minuteParm=m2;
	secondParm=s2;
	weekdayParm=w2;
}
//	+-------------------------------------------------------------------------------+
//	| @FUNCTION:  Set FeedTimer Time							                    |
//	| @NAME    :  SetTime				          		                            |
//	| @INPUT   :  hour,minute,second,weekday										|
//	| @OUTPUT  :  hour,minute,second,weekday			                            |
//	| @REMARKS :																	|
//	+-------------------------------------------------------------------------------+
void	class_Feed_Timer::SetTime(uint8_t &hourParm,uint8_t &minuteParm,uint8_t &secondParm,uint8_t &weekdayParm)
{
	//save SREG
	uint8_t sreg=SREG;
	//clear interrupts
	cli();
	//set time
	Ptrs->Hour=hourParm;
	Ptrs->Minute=minuteParm;
	Ptrs->Second=secondParm;
	Ptrs->WeekDay=weekdayParm;
	//restore SREG
	SREG=sreg;
}
//	+-------------------------------------------------------------------------------+
//	| @FUNCTION:  Get Week Day String 							                    |
//	| @NAME    :  GetWeekDayStr			          		                            |
//	| @INPUT   :  weekday															|
//	| @OUTPUT  :											                        |
//	| @REMARKS :																	|
//	+-------------------------------------------------------------------------------+
const char * class_Feed_Timer::GetWeekDayStr(uint8_t weekDayParm)
{
	switch (weekDayParm)
	{
        #if defined(_MENU_ENGLISH_)
		    //Monday
		    case 1:return PSTR("MON ");	    
		    //Tuesday
		    case 2:return PSTR("TUE ");    
		    //Wednesday
		    case 3:return PSTR("WED ");  
		    //Thursday
		    case 4:return PSTR("THU ");  
		    //Friday
		    case 5:return PSTR("FRI ");
		    //Saturday
		    case 6:return PSTR("SAT ");
		    //SunDay
		    case 7:return PSTR("SUN ");
        #else
		    //Segunda
		    case 1:return PSTR("SEG ");
		    //Terca
		    case 2:return PSTR("TER ");
		    //Quarta
		    case 3:return PSTR("QUA ");
		    //Quinta
		    case 4:return PSTR("QUI ");
		    //Sexta
		    case 5:return PSTR("SEX ");
		    //Sabado
		    case 6:return PSTR("SAB ");
		    //Domingo
		    case 7:return PSTR("DOM ");
        #endif
	}
	//else invalid
	return PSTR("INVL");
}
//	+-------------------------------------------------------------------------------+
//	| @FUNCTION:  Get Week Day String Shor 2 chars				                    |
//	| @NAME    :  GetWeekDayShortStr	          		                            |
//	| @INPUT   :  weekday															|
//	| @OUTPUT  :											                        |
//	| @REMARKS :																	|
//	+-------------------------------------------------------------------------------+
const char * class_Feed_Timer::GetWeekDayShortStr(uint8_t weekDayParm)
{
	switch (weekDayParm)
	{
        #if defined(_MENU_ENGLISH_)
          //Monday
          case 1:return PSTR("MO");
          //Tuesday
          case 2:return PSTR("TU");
          //Wednesday
          case 3:return PSTR("WE");
          //Thursday
          case 4:return PSTR("TH");
          //Friday
          case 5:return PSTR("FR");
          //Saturday
          case 6:return PSTR("SA");
          //Sunday
          case 7:return PSTR("SU");      
        #else
		    //Segunda
		    case 1:return PSTR("SE");
		    //Terca
		    case 2:return PSTR("TE");
		    //Quarta
		    case 3:return PSTR("QA");;
		    //Quinta
		    case 4:return PSTR("QI");
		    //Sexta
		    case 5:return PSTR("SX");
		    //Sabado
		    case 6:return PSTR("SA");
		    //Domingo
		    case 7:return PSTR("DO");
        #endif
	}
	//else invalid
	return PSTR("IV");
}
//	+-------------------------------------------------------------------------------+
//	| @FUNCTION:  Input Hour and Minute							                    |
//	| @NAME    :  InputHourMIn			          		                            |
//	| @INPUT   :  hour,minute														|
//	| @OUTPUT  :  return true if both parms entered ok		                        |
//	| @REMARKS :																	|
//	+-------------------------------------------------------------------------------+
bool	class_Feed_Timer::InputHourMinOrMinSec(uint8_t &hourMin,uint8_t &minSec,bool inputHourMin/* =true */,bool newInput/* =true */)
{
	KeyCode		key;	
	uint8_t		hourMinTmp,
				minSecTmp,
				tmp1,
				tmp2;
	bool		bBlink=false,
				bRap=false;
	uint32_t	tRap;
				
	//New input?
	if (newInput)
	{
		//yes,set to max to flash
		hourMinTmp=0xff;
		minSecTmp=0xff;
	}
	else
	{
		//else get actual time
		if (inputHourMin)
		{
			//get Hour and Minute
			this->GetTime(hourMinTmp,minSecTmp,tmp1,tmp2);	
		}
		else
		{
			//get Minute and Second
			this->GetTime(tmp1,hourMinTmp,minSecTmp,tmp2);
		}
	}
	//set hour Minute separator
	dp.DispSetPoint(2,true);
	//Enable timer0
	this->SetTimer0_ON();
	//enable io
	dp.EnableIO();
	//Set Time To Exit and Flash
	time.SetElapsed(&tExit);
	time.SetElapsed(&tBlink);
	//Wait and key release
	dp.KeyWaitRelease();
	time.DelayMsI(KEY_DELAY);
	//////////////////////////
	//MAIN INPUT LOOP
	//////////////////////////
	for(;;)
	{
		//time to Blink
		if (time.GetElapsed(&tBlink)>TIME_TO_BLINK)
		{
			time.SetElapsed(&tBlink);
			bBlink=!bBlink;
		}
		//time to flash?
		if (bBlink)
		{
			//set point
			dp.DispSetPoint(2,true);
			//yes, show HH and MM if not entered yet
			dp.Home();
			if (hourMinTmp==0xff)
			{
				inputHourMin?dp.PutSF(PSTR("HH")):dp.PutSF(PSTR("MM"));
			}
			else
			{
				sprintf(bf,"%02d",hourMinTmp);
				dp.PutS(bf);
			}
			dp.Locate(3);
			if (minSecTmp==0xff)
			{
				inputHourMin?dp.PutSF(PSTR("MM")):dp.PutSF(PSTR("SS"));
			}
			else
			{
				sprintf(bf,"%02d",minSecTmp);
				dp.PutS(bf);
			}
		}
		else
		{
			//set point off
			dp.DispSetPoint(2,false);
			//no, show "  " and "  " if not entered yet
			dp.Home();
			if (hourMinTmp==0xff)
			{
				dp.PutSF(PSTR("--"));
			}
			else
			{
				sprintf(bf,"%02d",hourMinTmp);
				dp.PutS(bf);
			}
			dp.Locate(3);
			if (minSecTmp==0xff)
			{
				dp.PutSF(PSTR("--"));
			}
			else
			{
				sprintf(bf,"%02d",minSecTmp);
				dp.PutS(bf);
			}
		}
		//read keyboard
		key=dp.KeyReadFlash();
		//clear time to exit if any key pressed
		if (key!=KeyCode::NONE)
		{
			time.SetElapsed(&tExit);
			if (time.GetElapsed(&tRap)>TIME_TO_RAPID)
			{
				bRap=true;
			}
		}
		else
		{
			bRap=false;
			time.SetElapsed(&tRap);
		}
		//exit if exit time expired
		if (time.GetElapsed(&tExit)>TIME_TO_EXIT)
		{
			//clear and exit
			ClearAndExit();
			return false;
		}
		//process keys
		switch (static_cast<uint8_t>(key))
		{
			case static_cast<uint8_t>(KeyCode::DOWN):
				//decrement time
				hourMinTmp++;
				//input Hour Min?
				if (inputHourMin)
				{
					//process hour
					if (hourMinTmp>23) hourMinTmp=0;
				}
				else
				{
					//process minute
					if (hourMinTmp>59) hourMinTmp=0;
				}
				//key delay
				bRap?time.DelayMsI(KEY_DELAY_RAPID):time.DelayMsI(KEY_DELAY);
			break;
			case static_cast<uint8_t>(KeyCode::UP):
				//increment time
				minSecTmp++;
				if (minSecTmp>59) minSecTmp=0;
				//key delay
				bRap?time.DelayMsI(KEY_DELAY_RAPID):time.DelayMsI(KEY_DELAY);
			break;		
			case static_cast<uint8_t>(KeyCode::ENTER):
				//check if hour minute set
				if (hourMinTmp==0xff || minSecTmp==0xff)
				{
					for(uint8_t i=0;i<10;i++)
					{
						dp.Home();
						if (hourMinTmp==0xff) inputHourMin? dp.PutSF(PSTR("HH")):dp.PutSF(PSTR("MM"));
						dp.Locate(3);
						if (minSecTmp==0xff) inputHourMin? dp.PutSF(PSTR("MM")):dp.PutSF(PSTR("SS"));
						time.DelayMsI(100);
						dp.Home();
						if (hourMinTmp==0xff) dp.PutSF(PSTR("  "));
						dp.Locate(3);
						if (minSecTmp==0xff) dp.PutSF(PSTR("  "));
						time.DelayMsI(100);						
					}
				}
				else
				{
					//set New Time
					hourMin=hourMinTmp;
					minSec=minSecTmp;
					//clear screen clear point timer 0
					dp.DispSetPoint(2,false);
					//wait key release
					dp.KeyWaitRelease();					
					ClearAndExit();
					return true;					
				}
			break;	
		}
	}
}
//	+-------------------------------------------------------------------------------+
//	| @FUNCTION:  Clear Screen, Disable Timer 0, Delay 10ms			                |
//	| @NAME    :  ClearAndExit			          		                            |
//	| @INPUT   :  WeekDay															|
//	| @OUTPUT  :  return weekDay number						                        |
//	| @REMARKS :																	|
//	+-------------------------------------------------------------------------------+
void	class_Feed_Timer::ClearAndExit()
{
	//clear screen
	dp.Cls();
	//clear all points
	for(uint8_t i=1;i<=4;i++)
		dp.DispSetPoint(i,false);
	//time to clear
	time.DelayMsS(10);
	//disable timer0 
	this->SetTimer0_OFF();
}
//	+-------------------------------------------------------------------------------+
//	| @FUNCTION:  Input Week Day								                    |
//	| @NAME    :  InputWeekDay			          		                            |
//	| @INPUT   :  WeekDay															|
//	| @OUTPUT  :  return weekDay number						                        |
//	| @REMARKS :																	|
//	+-------------------------------------------------------------------------------+
bool	class_Feed_Timer::InputWeekDay(uint8_t &weekDay,bool newInput/* =true */)
{
	KeyCode		key;
	uint8_t		h,m,s,w;
	uint32_t	tExit;
	
	//New input?
	if (newInput)
	{
		//yes
		w=static_cast<uint8_t>(enum_WEEK_DAY::MONDAY);
	}
	else
	{
		//else get weekday
		this->GetTime(h,m,s,w);
	}
	//set hour Minute separator
	dp.DispSetPoint(3,true);
	//Enable timer0
	this->SetTimer0_ON();
	//enable io
	dp.EnableIO();
	//Set Time To Exit and Flash
	time.SetElapsed(&tExit);
	//Wait and key release
	dp.KeyWaitRelease();
	time.DelayMsI(KEY_DELAY);
	//////////////////////////
	//MAIN INPUT LOOP
	//////////////////////////
	for(;;)
	{
		dp.Home();
		dp.PutSF(this->GetWeekDayStr(w));
		//read keys
		key=dp.KeyReadFlash();
		//clear elapsed time if any key pressed
		if (key!=KeyCode::NONE)
		{
			time.SetElapsed(&tExit);
		}
		//exit time to exit reached
		if (time.GetElapsed(&tExit)>TIME_TO_EXIT)
		{
			//disable timer,clear screen point
			dp.DispSetPoint(3,false);
			ClearAndExit();
			//set weekday no set
			return false;
		}	
		
		
		//----------------------------------------
		//process keys
		switch (key)
		{
			case KeyCode::DOWN:
				//decrement weekday
				w--;
				if (w<static_cast<uint8_t>(enum_WEEK_DAY::MONDAY)) w=static_cast<uint8_t>(enum_WEEK_DAY::SUNDAY);
				//key delay
				time.DelayMsI(KEY_DELAY);
				break;
			case KeyCode::UP:
				//increment weekday
				w++;
				if (w>static_cast<uint8_t>(enum_WEEK_DAY::SUNDAY)) w=static_cast<uint8_t>(enum_WEEK_DAY::MONDAY);
				//key delay
				time.DelayMsI(KEY_DELAY);
			break;
			case KeyCode::ENTER:
				//set week day
				weekDay=w;
				//disable timer,clear screen point
				dp.KeyWaitRelease();
				dp.DispSetPoint(3,false);
				ClearAndExit();
				return true;				
			break;
			//not used codes
			case KeyCode::DOWN_OVER:
			case KeyCode::DOWN_REPEAT:
			case KeyCode::UP_OVER:
			case KeyCode::UP_REPEAT:
			case KeyCode::ENTER_OVER:
			case KeyCode::ENTER_REPEAT:
			case KeyCode::NONE:
			break;
		}			
	}
}
//	+-------------------------------------------------------------------------------+
//	| @FUNCTION:  Input Week Days								                    |
//	| @NAME    :  InputWeekDays			          		                            |
//	| @INPUT   :  WeekDays															|
//	| @OUTPUT  :  return in weekDay bit0=1 Monsay Set,..,bit7=1 Sunday set          |
//	| @REMARKS :																	|
//	+-------------------------------------------------------------------------------+
bool	class_Feed_Timer::InputWeekDays(uint8_t &weekDays,bool newInput/* =true */)
{
	KeyCode		key;
	uint8_t		WDay,		//Hold Day of week
				WDays;		//Hold all Days of week
	uint32_t	tExit;
	
	//New input?
	if (newInput)
	{
		//yes, clear all week days
		WDays=0;
	}
	else
	{
		//get week days
		WDays=weekDays;
	}
	//set week day as monday
	WDay=static_cast<uint8_t>(enum_WEEK_DAY::MONDAY);
	//Enable timer0
	this->SetTimer0_ON();
	//enable io
	dp.EnableIO();
	//Set Time To Exit and Flash
	time.SetElapsed(&tExit);
	//Wait and key release
	dp.KeyWaitRelease();
	time.DelayMsI(KEY_DELAY);
	//////////////////////////
	//MAIN INPUT LOOP
	//////////////////////////
	for(;;)
	{
		dp.Home();
		dp.PutSF(this->GetWeekDayShortStr(WDay));
		dp.PutSF(PSTR("-"));
		//show if weekday is selected
		dp.Locate(4);
		if ((WDays & (1<<(WDay-1)))==0)
		{
			dp.PutSF(PSTR(" "));
		}
		else
		{
			dp.PutSF(PSTR("#"));
		}
		//read keys
		key=dp.KeyReadFlash();
		//clear elapsed time if any key pressed
		if (key!=KeyCode::NONE)
		{
			time.SetElapsed(&tExit);
		}
		//exit time to exit reached
		if (time.GetElapsed(&tExit)>TIME_TO_EXIT)
		{
			//disable timer,clear screen point
			ClearAndExit();
			//set weekdays not set
			return false;
		}
		//process keys
		switch (key)
		{
	
			case KeyCode::UP:
			//increment weekday
				WDay++;
				if (WDay>static_cast<uint8_t>(enum_WEEK_DAY::SUNDAY)) WDay=static_cast<uint8_t>(enum_WEEK_DAY::MONDAY);
				//key delay
				time.DelayMsI(KEY_DELAY);
			break;
			case KeyCode::DOWN:
			//change weekday selection
				WDays^=(1<<(WDay-1));
				dp.Locate(4);
				if ((WDays & (1<<(WDay-1)))==0)
				{
					dp.PutSF(PSTR(" "));
				}
				else
				{
					dp.PutSF(PSTR("#"));
				}	
				dp.KeyWaitRelease();
				time.DelayMsI(KEY_DELAY);
			break;
			case KeyCode::ENTER:
				//set week day
				weekDays=WDays;
				//disable timer,clear screen point
				dp.KeyWaitRelease();
				ClearAndExit();
				return true;
			break;
		//not used codes
			case KeyCode::DOWN_OVER:
			case KeyCode::DOWN_REPEAT:
			case KeyCode::UP_OVER:
			case KeyCode::UP_REPEAT:
			case KeyCode::ENTER_OVER:
			case KeyCode::ENTER_REPEAT:
			case KeyCode::NONE:
			break;					
		}
	}
}
//	+-------------------------------------------------------------------------------+]
//	| @FUNCTION:  Initialize Time								                    |
//	| @NAME    :  InitTime				          		                            |
//	| @INPUT   :																	|
//	| @OUTPUT  :											                        |
//	| @REMARKS :  if boot is cold new time is requested if hot set new time			|
//	+-------------------------------------------------------------------------------+
void	class_Feed_Timer::InitTime()
{
	uint8_t		h=0xff,
				m=0xff,
				s=0,
				w=0xff;
	
	//get boot type
	if (this->Ptrs->BootKey==_BOOT_KEY)
	{
		//return if hot, no need to set time
		return;
	}
	//set boot hot
	this->Ptrs->BootKey=_BOOT_KEY;
	//Get Hour
	do{
	} while (this->InputHourMinOrMinSec(h,m,true,true)==false);
	//get Day of Week
	do{
	} while (this->InputWeekDay(w,true)==false);
	//Set Time and week day
	this->SetTime(h,m,s,w);
}
//	+-------------------------------------------------------------------------------+
//	| @FUNCTION:  Show at points of display blink left to right that feed time is	|
//	|			  operational.														|
//	| @NAME    :  ShowLIVE				          		                            |
//	| @INPUT   :				  													|
//	| @OUTPUT  :  none								                                |
//	| @REMARKS :																	|
//	+-------------------------------------------------------------------------------+
void class_Feed_Timer::ShowLIVE()
{
	static uint8_t	live=0;
	
	
	if (++live>10)
	{
		//enable IO State
		dp.EnableIO();
		live=0;
		//enable timer 0
		SetTimer0_ON();
		//show live status using displays dots
		for(uint8_t i=1;i<=4;i++)
		{
			dp.DispSetPoint(i,true);
			time.DelayMsI(65);
			dp.DispSetPoint(i,false);
		}
		//disable timer 0
		SetTimer0_OFF();
	}
}
//	+-------------------------------------------------------------------------------+
//	| @FUNCTION:  Add a New Event													|
//	| @NAME    :  EventAdd				          		                            |
//	| @INPUT   :  Event to add  													|
//	| @OUTPUT  :  return true if Add is okf			                                |
//	| @REMARKS :																	|
//	+-------------------------------------------------------------------------------+
bool class_Feed_Timer::EventAdd(struct EventStruct &Event)
{
	int8_t tmp;
	uint8_t sreg;
	struct EventStruct EventTmp;
	
	//save SREG
	sreg=SREG;
	//disable interrupts
	cli();
	if (Event.Event==static_cast<uint8_t>(enum_EVENT_TYPE::DIARY)) Event.WeekDays=0xff; //set to 1 all days of week 
	if (Event.Event==static_cast<uint8_t>(enum_EVENT_TYPE::REPEAT)) Event.WeekDays=0;	//set to 0 all days of week per precaution
	//compute Xor Check
	Event.XorCheck=Event.Event ^ Event.Hour ^ Event.Minute ^ Event.WeekDays ^ Event.MinuteMotorON ^ Event.SecondMotorON;
	//Get Event Size
	tmp=eeprom_read_byte(&EE_EventsSize);
	tmp++;
	if (tmp>EVENT_SIZE) tmp=EVENT_SIZE;
	eeprom_write_byte(&EE_EventsSize,tmp);
	// new event
	eeprom_write_block(&Event,&EE_Events[tmp-1],sizeof(Event));
	//read back to check if write ok
	eeprom_read_block(&EventTmp,&EE_Events[tmp-1],sizeof(Event));
	//clear flags and counter
	EventsCount[tmp-1].Flags.SeparateFlag.LoadFrom=false;
	EventsCount[tmp-1].Flags.SeparateFlag.Counting=false;
	EventsCount[tmp-1].Count=0;
	//restore SREG
	SREG=sreg;
	//return state
	if (memcmp(&Event,&EventTmp,sizeof(Event))==0)
		return true;
	else
		return false;
}
//	+-------------------------------------------------------------------------------+
//	| @FUNCTION:  Delete an existent Event											|
//	| @NAME    :  EventDelete			          		                            |
//	| @INPUT   :  eventNumber to delete												|
//	| @OUTPUT  :  return true if Add is okf			                                |
//	| @REMARKS :																	|
//	+-------------------------------------------------------------------------------+
bool class_Feed_Timer::EventDelete(uint8_t eventNumber)
{
	uint8_t sreg,EvCount;
	struct EventStruct *EvPSource,*EvPDestiny,EvBuffer;
	
	//save SREG
	sreg=SREG;
	//disable interrupts
	cli();
	//prevent motor on during this process
	dp.MotorOFF();
	//Get Event Size
	EvCount=eeprom_read_byte(&EE_EventsSize);
	//return false if eventNumber>eventcount
	if (eventNumber>=EvCount) return false;
	//if only one event set event count to 0
	if (EvCount==1)
	{
		EvCount=0;
		eeprom_write_byte(&EE_EventsSize,EvCount);
		return true;
	}
	else
	{
		//move Source Events to Destiny Events and decrement EventCount	
		EvPDestiny=&EE_Events[eventNumber];
		EvPSource=&EE_Events[eventNumber+1];
		for(uint8_t i=0;i<EvCount;i++)	
		{
			//move valid blocks
			eeprom_read_block(&EvBuffer,EvPSource,sizeof(EvBuffer));
			eeprom_write_block(&EvBuffer,EvPDestiny,sizeof(EvBuffer));
			EvPSource++;
			EvPDestiny++;
			EventsCount[i]=EventsCount[i+1];
		}
	}
	//event--
	EvCount--;
	eeprom_write_byte(&EE_EventsSize,EvCount);
	//restore SREG 
	SREG=sreg;
	//return true
	return true;
}

//	+-------------------------------------------------------------------------------+
//	| @FUNCTION:  Get Menu Item Number String										|
//	| @NAME    :  GetMenuItemStr		          		                            |
//	| @INPUT   :  MenuItemNumber 1..N												|
//	| @OUTPUT  :											                        |
//	| @REMARKS :																	|
//	+-------------------------------------------------------------------------------+

const char * class_Feed_Timer::GetMenuItemStr(menu_Item menuItemNumber)
{
	switch (menuItemNumber)
	{
		//Set Hour
		case menu_Item::SET_TIME:
            #if defined(_MENU_ENGLISH_)
                return PSTR("TI-1");
            #else
			    return PSTR("HR-1");
            #endif
		break;
		//Add new Item
		case menu_Item::INCLUDE:
			return PSTR("IC-2");
		break;
		//Exclude Item
		case menu_Item::DELETE:
			return PSTR("DE-3");
		break;
		//Reset
		case menu_Item::RESET:
			return PSTR("RS-4");
		break;
		//Exit
		case menu_Item::EXIT:
            #if defined(_MENU_ENGLISH_)
			    return PSTR("EXIT ");
            #else
                return PSTR("SAI ");
            #endif
		break;
	}
	//else invalid
	return PSTR("INVL");
}
//	+-------------------------------------------------------------------------------+
//	| @FUNCTION:  Get Inclusion Options String										|
//	| @NAME    :  GetIncOptionStr		          		                            |
//	| @INPUT   :  IncOption 1..N													|
//	| @OUTPUT  :											                        |
//	| @REMARKS :																	|
//	+-------------------------------------------------------------------------------+

const char * class_Feed_Timer::GetIncOptionStr(inc_Option incOptionNumber)
{
	switch (incOptionNumber)
	{
		//Inc Repeat
		case inc_Option::REPEAT:
			return PSTR("REPT");
		break;
		//Inc Diary
		case inc_Option::DIARY:
			return PSTR("DIAR");
		break;
		//inc Weekly
		case inc_Option::WEEKLY:
            #if defined(_MENU_ENGLISH_)
                return PSTR("WEEK");
            #else
			    return PSTR("SEMA");
            #endif
		break;
		//Exit
		case inc_Option::EXIT:
            #if defined(_MENU_ENGLISH_)
                return PSTR("EXIT");
            #else
			    return PSTR("SAI ");
            #endif
            
		break;
	}
	//else invalid
	return PSTR("INVL");
}
//	+-------------------------------------------------------------------------------+
//	| @FUNCTION:  Get Event Type String												|
//	| @NAME    :  GetEventTypeStr		          		                            |
//	| @INPUT   :  EventTypeNumber 1..N												|
//	| @OUTPUT  :											                        |
//	| @REMARKS :																	|
//	+-------------------------------------------------------------------------------+

const char * class_Feed_Timer::GetEventTypeStr(uint8_t eventTypeNumber)
{
	switch (eventTypeNumber)
	{
		//type Repeat
		case static_cast<uint8_t>(enum_EVENT_TYPE::REPEAT):
			return PSTR("REPT");
		break;
		//Inc Diary
		case static_cast<uint8_t>(enum_EVENT_TYPE::DIARY):
			return PSTR("DIAR");
		break;
		//inc Weekly
		case static_cast<uint8_t>(enum_EVENT_TYPE::WEEKLY):
            #if defined(_MENU_ENGLISH_)
                return PSTR("WEEK");
            #else
			    return PSTR("SEMA");
            #endif
		break;
		//Disable
		case static_cast<uint8_t>(enum_EVENT_TYPE::EMPTY):
            #if defined(_MENU_ENGLISH_)
                return PSTR("EMPT");
            #else
			    return PSTR("VAZI");
            #endif
		break;
	}
	//else invalid
	return PSTR("INVL");
}
//	+-------------------------------------------------------------------------------+
//	| @FUNCTION:  Show Event Number using format EnnT								|
//	| @NAME    :  ShowEventNumber		          		                            |
//	| @INPUT   :  EventNumber	  													|
//	| @OUTPUT  :  none								                                |
//	| @REMARKS :																	|
//	+-------------------------------------------------------------------------------+
void	class_Feed_Timer::ShowEventNumber(uint8_t eventNumber)
{
	dp.DispSetPoint(1,true);
	dp.DispSetPoint(3,true);
	sprintf(bf,"E%02d ",static_cast<int>(eventNumber));
	dp.Home();
	dp.PutS(bf);
}
//	+-------------------------------------------------------------------------------+
//	| @FUNCTION:  Show WeekDays using display segments a show below					|
//	| @NAME    :  ShowWeekDays			          		                            |
//	| @INPUT   :  WeekDays bit0=1 Monday,..,bit7=1 Sunday							|
//	| @OUTPUT  :  none								                                |
//	| @REMARKS :																	|
//	+-------------------------------------------------------------------------------+
void	class_Feed_Timer::ShowWeekDays(uint8_t weekDays)
{
	//first column
	dp.Home();
	if ((weekDays & (1<<0))==0 && (weekDays & (1<<4))==0) dp.PutC(static_cast<uint8_t>(enum_CONTROL::FS));
	if ((weekDays & (1<<0))!=0 && (weekDays & (1<<4))==0) dp.PutC(static_cast<uint8_t>(enum_CONTROL::GS));
	if ((weekDays & (1<<0))==0 && (weekDays & (1<<4))!=0) dp.PutC(static_cast<uint8_t>(enum_CONTROL::RS));
	if ((weekDays & (1<<0))!=0 && (weekDays & (1<<4))!=0) dp.PutC(static_cast<uint8_t>(enum_CONTROL::US));
	//second column
	if ((weekDays & (1<<1))==0 && (weekDays & (1<<5))==0) dp.PutC(static_cast<uint8_t>(enum_CONTROL::FS));
	if ((weekDays & (1<<1))!=0 && (weekDays & (1<<5))==0) dp.PutC(static_cast<uint8_t>(enum_CONTROL::GS));
	if ((weekDays & (1<<1))==0 && (weekDays & (1<<5))!=0) dp.PutC(static_cast<uint8_t>(enum_CONTROL::RS));
	if ((weekDays & (1<<1))!=0 && (weekDays & (1<<5))!=0) dp.PutC(static_cast<uint8_t>(enum_CONTROL::US));
	//third column
	if ((weekDays & (1<<2))==0 && (weekDays & (1<<6))==0) dp.PutC(static_cast<uint8_t>(enum_CONTROL::FS));
	if ((weekDays & (1<<2))!=0 && (weekDays & (1<<6))==0) dp.PutC(static_cast<uint8_t>(enum_CONTROL::GS));
	if ((weekDays & (1<<2))==0 && (weekDays & (1<<6))!=0) dp.PutC(static_cast<uint8_t>(enum_CONTROL::RS));
	if ((weekDays & (1<<2))!=0 && (weekDays & (1<<6))!=0) dp.PutC(static_cast<uint8_t>(enum_CONTROL::US));
	//forty column
	if ((weekDays & (1<<3))==0) dp.PutC(static_cast<uint8_t>(enum_CONTROL::FS));
	if ((weekDays & (1<<3))!=0) dp.PutC(static_cast<uint8_t>(enum_CONTROL::GS));
}
//	+-------------------------------------------------------------------------------+
//	| @FUNCTION:  Process Inputs from keys											|
//	| @NAME    :  ProcessInputs			          		                            |
//	| @INPUT   :				  													|
//	| @OUTPUT  :  none								                                |
//	| @REMARKS :																	|
//	+-------------------------------------------------------------------------------+
void class_Feed_Timer::ProcessInputs()
{	
	//My vars
	KeyCode		key;
	uint8_t		h,m1,m2,s,w;
	menu_Item	menuItem=menu_Item::SET_TIME;
	inc_Option	incOption=inc_Option::REPEAT;
	uint32_t	tExit;
	uint32_t	tShow;
	uint32_t	tRep;
	bool		bRep=false;
	struct EventStruct	Event;
	
	
	//Enable I/O Timer 0
	dp.EnableIO();
	this->SetTimer0_ON();
	//Clear Time To Exit
	time.SetElapsed(&tExit);
	time.SetElapsed(&tRep);
	
	key=dp.KeyReadFlash();
	
	switch (key)
	{
		/////////////////////
		// KEY DOWN? SHOW TIME, WEEK DAY AND SECOND
		/////////////////////
		case KeyCode::DOWN:
			for(;;)
			{
				//show Time
				time.SetElapsed(&tShow);
				dp.DispSetPoint(2,true);
				do{
					this->GetTime(h,m1,s,w);
					sprintf(bf,"%02d%02d",h,m1);
					dp.Locate(1);
					dp.PutS(bf);
				} while (time.GetElapsed(&tShow)<1500);
				dp.DispSetPoint(2,false);
				//show WeekDay
				dp.DispSetPoint(3,true);
				time.SetElapsed(&tShow);
				do{
					this->GetTime(h,m1,s,w);
					dp.Locate(1);
					dp.PutSF(this->GetWeekDayStr(w));
				} while (time.GetElapsed(&tShow)<1500);			
				dp.DispSetPoint(3,false);	
				//show Second
				time.SetElapsed(&tShow);
				dp.Cls();
				do{
					this->GetTime(h,m1,s,w);
					dp.Locate(1);
					sprintf(bf,"  %02d",s);
					dp.PutS(bf);
				} while (time.GetElapsed(&tShow)<4000);						
				//exit if key none
				key=dp.KeyReadFlash();
				if (key==KeyCode::NONE)
				{
					//disable time and IO
					ClearAndExit();
					return;
				}
			}
		break;
		/////////////////////
		// KEY UP? 
		/////////////////////
		case KeyCode::UP:
			//show MENU Item
			dp.Locate(1);
			dp.PutSF(this->GetMenuItemStr(menuItem));
			//wait key release
			dp.KeyWaitRelease();
			time.DelayMsI(KEY_DELAY);
			for(;;)
			{
				//show MENU Item
				dp.Locate(1);
				dp.PutSF(this->GetMenuItemStr(menuItem));
				//read key again
				key=dp.KeyReadFlash();
				//set time to exit if any key pressed
				if (key!=KeyCode::NONE) time.SetElapsed(&tExit);
				if (time.GetElapsed(&tExit)>TIME_TO_EXIT)
				{
					//yes, disable timer0 IO clear screen and exit
					ClearAndExit();
					return;
				}
				//process keys
				switch (key)
				{
					case KeyCode::DOWN:
						menuItem=static_cast<menu_Item>(static_cast<uint8_t>(menuItem)-1);
						if (static_cast<uint8_t>(menuItem)<static_cast<uint8_t>(menu_Item::SET_TIME)) menuItem=menu_Item::SET_TIME;
						time.DelayMsI(KEY_DELAY);
					break;
					case KeyCode::UP:
						menuItem=static_cast<menu_Item>(static_cast<uint8_t>(menuItem)+1);
						if (static_cast<uint8_t>(menuItem)>static_cast<uint8_t>(menu_Item::EXIT)) menuItem=menu_Item::EXIT;
						time.DelayMsI(KEY_DELAY);
					break;
					case KeyCode::ENTER:
						/////////////////////////////
						//  EXECUTE MENU OPTIONS
						/////////////////////////////
						dp.KeyWaitRelease();
						switch (menuItem)
						{
							////////////////////////
							//  SET TIME
							////////////////////////
							case menu_Item::SET_TIME:
								//Get Time
								this->GetTime(h,m1,s,w);
								//set time
								if (this->InputHourMinOrMinSec(h,m1,s,false)==false)
								{
									//return
									ClearAndExit();
									return;
								}
								//set week day
								if (this->InputWeekDay(w,false)==false)
								{
									//return if no parms entered
									ClearAndExit();
									return;
								}
								//set time and week day
								this->SetTime(h,m1,s,w);
								//return
								ClearAndExit();
								return;
							break;
							////////////////////////
							//  INCLUDE EVENT
							////////////////////////
							case menu_Item::INCLUDE:
								//wait key release
								dp.KeyWaitRelease();
								time.DelayMsI(KEY_DELAY);
								for(;;)
								{
									//show option message
									dp.Locate(1);
									dp.PutSF(GetIncOptionStr(incOption));									
									//read key
									key=dp.KeyReadFlash();
									//clear time to exit if key pressed
									if (key!=KeyCode::NONE)
									{
										time.SetElapsed(&tExit);
									}
									//exit from here if time to exit overflow
									if (time.GetElapsed(&tExit)>TIME_TO_EXIT)
									{
										ClearAndExit();
										return;
									}
									//process keys according
									switch (key)
									{
										//Previous item
										case KeyCode::DOWN:
											incOption=static_cast<inc_Option>(static_cast<uint8_t>(incOption)-1);
											if (static_cast<uint8_t>(incOption)<static_cast<uint8_t>(inc_Option::REPEAT)) incOption=inc_Option::REPEAT;
											time.DelayMsI(KEY_DELAY);										
										break;
										//Next item		
										case KeyCode::UP:
											incOption=static_cast<inc_Option>(static_cast<uint8_t>(incOption)+1);
											if (static_cast<uint8_t>(incOption)>static_cast<uint8_t>(inc_Option::EXIT)) incOption=inc_Option::EXIT;
											time.DelayMsI(KEY_DELAY);
										break;
										//Execute Item
										case KeyCode::ENTER:
											switch (incOption)
											{
												///////////////////////
												// INCLUSION REPEAT
												///////////////////////
												case inc_Option::REPEAT:
													for(;;)
													{
														if (this->InputHourMinOrMinSec(h,m1)==false)
														{
															//exit if no input
															ClearAndExit();
															return;
														}
														if (this->InputHourMinOrMinSec(m2,s,false)==false)
														{
															//exit if no input
															ClearAndExit();
															return;
														}
														//Confirm Yes(KEY_DOWN) or No(KEY_UP)
														this->SetTimer0_ON();
														dp.Home();
                                                        #if defined(_MENU_ENGLISH_)
                                                            dp.PutSF(PSTR("Y N "));
                                                        #else
														    dp.PutSF(PSTR("S N "));
                                                        #endif
														key=dp.KeyRead();
														if (key==KeyCode::DOWN) break;
														if (key==KeyCode::UP)
														{
															//exit
															ClearAndExit();
															return;
														}
													}
													///////////////////////////
													//  Add new Repeat Event
													///////////////////////////
													Event.Event=static_cast<uint8_t>(enum_EVENT_TYPE::REPEAT);
													Event.Hour=h;
													Event.Minute=m1;
													Event.MinuteMotorON=m2;
													Event.SecondMotorON=s;
													if (this->EventAdd(Event)==false)
													{
														//show Error and exit
														dp.Home();
														dp.PutSF(PSTR("ERRO"));
														time.DelayMsI(TIME_TO_SHOW_ERROR);
													}
													//Show Event number
													this->ShowEventNumber(eeprom_read_byte(&EE_EventsSize));
													time.DelayMsI(TIME_EVENT_NUMBER_SHOW);
													//clear timer and exit
													ClearAndExit();
													return;
												break;
												///////////////////////
												// INCLUSION DIARY
												///////////////////////
												case inc_Option::DIARY:
													for(;;)
													{
														if (this->InputHourMinOrMinSec(h,m1)==false)
														{
															//exit if no input
															ClearAndExit();
															return;
														}
														if (this->InputHourMinOrMinSec(m2,s,false)==false)
														{
															//exit if no input
															ClearAndExit();
															return;
														}
														//Confirm Yes(KEY_DOWN) or No(KEY_UP)
														this->SetTimer0_ON();
														dp.Home();
													    #if defined(_MENU_ENGLISH_)
													        dp.PutSF(PSTR("Y N "));
													    #else
													        dp.PutSF(PSTR("S N "));
													    #endif
														key=dp.KeyRead();
														if (key==KeyCode::DOWN) break;
														if (key==KeyCode::UP)
														{
															//exit
															ClearAndExit();
															return;
														}														
													}
													///////////////////////////
													//  Add new Diary Event
													///////////////////////////
													Event.Event=static_cast<uint8_t>(enum_EVENT_TYPE::DIARY);
													Event.Hour=h;
													Event.Minute=m1;
													Event.MinuteMotorON=m2;
													Event.SecondMotorON=s;
													if (this->EventAdd(Event)==false)
													{
														//show Error and exit
														dp.Home();
														dp.PutSF(PSTR("ERRO"));
														time.DelayMsI(TIME_TO_SHOW_ERROR);
													}
													//Show Event number
													this->ShowEventNumber(eeprom_read_byte(&EE_EventsSize));
													time.DelayMsI(TIME_EVENT_NUMBER_SHOW);
													//clear timer and exit
													ClearAndExit();
													return;
												break;
												///////////////////////
												// INCLUSION WEEKLY
												///////////////////////
												case inc_Option::WEEKLY:
													for(;;)
													{
														if (this->InputWeekDays(w)==false)
														{
															//exit if no input
															ClearAndExit();
															return;
														}
														if (this->InputHourMinOrMinSec(h,m1)==false)
														{
															//exit if no input
															ClearAndExit();
															return;
														}
														if (this->InputHourMinOrMinSec(m2,s,false)==false)
														{
															//exit if no input
															ClearAndExit();
															return;
														}
														//Confirm Yes(KEY_DOWN) or No(KEY_UP)
														this->SetTimer0_ON();
														dp.Home();
														#if defined(_MENU_ENGLISH_)
														    dp.PutSF(PSTR("Y N "));
														#else
														    dp.PutSF(PSTR("S N "));
														#endif
														key=dp.KeyRead();
														if (key==KeyCode::DOWN) break;
														if (key==KeyCode::UP)
														{
															//exit
															ClearAndExit();
															return;
														}
													}
													///////////////////////////
													//  Add new Weekly Event
													///////////////////////////
													Event.Event=static_cast<uint8_t>(enum_EVENT_TYPE::WEEKLY);
													Event.Hour=h;
													Event.Minute=m1;
													Event.MinuteMotorON=m2;
													Event.SecondMotorON=s;
													Event.WeekDays=w;
													if (this->EventAdd(Event)==false)
													{
														//show Error and exit
														dp.Home();
														dp.PutSF(PSTR("ERRO"));
														time.DelayMsI(TIME_TO_SHOW_ERROR);
													}
													//Show Event number
													this->ShowEventNumber(eeprom_read_byte(&EE_EventsSize));
													time.DelayMsI(TIME_EVENT_NUMBER_SHOW);
													//clear timer and exit
													ClearAndExit();
													return;
												break;
												///////////////////////
												// INCLUSION EXIT
												///////////////////////
												case inc_Option::EXIT:
													ClearAndExit();
													return;
												break;
											}
										break;	
										//not used codes
										case KeyCode::DOWN_OVER:
										case KeyCode::DOWN_REPEAT:
										case KeyCode::UP_OVER:
										case KeyCode::UP_REPEAT:
										case KeyCode::ENTER_OVER:
										case KeyCode::ENTER_REPEAT:
										case KeyCode::NONE:
										break;				
									}
								}
							break;
							////////////////////////
							//  DELETE EVENT
							////////////////////////
							case menu_Item::DELETE:
							{
								//get Size
								int8_t  EvSize=static_cast<int8_t>(eeprom_read_byte(&EE_EventsSize));
								int8_t  EvCount=0,ShowType=0;
								struct EventStruct *EvP=&EE_Events[0],EvTmp;
								//main delete loop
								for(;;)
								{
									//read event
									eeprom_read_block(&EvTmp,&EvP->Event,sizeof(EvTmp));
									//show events if size>0
									if (EvSize>0)
									{
										//show event according ShowType
										switch (ShowType)
										{
											case 0:
												dp.DispSetPoint(2,false);
												dp.DispSetPoint(4,false);
												this->ShowEventNumber(EvCount+1);
												dp.Locate(4);
												dp.PutSF(this->GetEventTypeStr(EvTmp.Event));
											break;
											case 1:
												dp.DispSetPoint(1,false);
												dp.DispSetPoint(2,true);
												dp.DispSetPoint(3,false);		
												dp.DispSetPoint(4,false);									
												sprintf(bf,"%02d%02d",EvTmp.Hour,EvTmp.Minute);
												dp.Home();
												dp.PutS(bf);
											break;
											case 2:
												dp.DispSetPoint(1,false);
												dp.DispSetPoint(2,true);
												dp.DispSetPoint(3,false);
												dp.DispSetPoint(4,true);
												sprintf(bf,"%02d%02d",EvTmp.MinuteMotorON,EvTmp.SecondMotorON);
												dp.Home();
												dp.PutS(bf);
											break;
											case 3:
												dp.DispSetPoint(1,false);
												dp.DispSetPoint(2,false);
												dp.DispSetPoint(3,false);
												dp.DispSetPoint(4,false);
												this->ShowWeekDays(EvTmp.WeekDays);
											break;									
										}
									}
									else
									{
										dp.Home();
										dp.PutSF(PSTR("E=0 "));
										time.DelayMsI(TIME_TO_SHOW_ERROR);
										ClearAndExit();
										return;
									}
									//read keyboard
									key=dp.KeyReadFlash();
									//clear time to exit if key pressed
									if (key!=KeyCode::NONE)
									{
										time.SetElapsed(&tExit);
										if (time.GetElapsed(&tRep)>TIME_TO_RAPID)
										{
											bRep=true;
										}
									}
									else
									{
										bRep=false;
										time.SetElapsed(&tRep);
									}
									//exit from here if time to exit overflow
									if (time.GetElapsed(&tExit)>TIME_TO_EXIT)
									{
										ClearAndExit();
										return;
									}
									//process according key
									switch (key)
									{
										case KeyCode::DOWN:
											//change show type
											ShowType++;
											if (ShowType>3) ShowType=0;
											dp.KeyWaitRelease();
										break;
										case KeyCode::UP:
											//next event
											EvP++;
											EvCount++;
											//if reach max the set to max
											if (EvCount>(EvSize-1))
											{
												EvCount=0;
												EvP=&EE_Events[0];
											}
											//time
											bRep?time.DelayMsI(KEY_DELAY_RAPID):time.DelayMsI(KEY_DELAY);
										break;
										case KeyCode::ENTER:
											//read option key_up(yes) key_down(no)
											this->SetTimer0_ON();
											dp.Home();
											#if defined(_MENU_ENGLISH_)
											    dp.PutSF(PSTR("Y N "));
											#else
											    dp.PutSF(PSTR("S N "));
											#endif
											dp.KeyWaitRelease();
											key=dp.KeyRead();
											//clear and exit
											ClearAndExit();
											if (key==KeyCode::UP)
											{
												//return
												return;
											}
											//delete event
											EventDelete(EvCount);
											//Show Event number
											this->SetTimer0_ON();
											dp.DispSetPoint(2,false);
											this->ShowEventNumber(eeprom_read_byte(&EE_EventsSize));
											time.DelayMsI(TIME_EVENT_NUMBER_SHOW);
											//clear and exit											
											ClearAndExit();
											return;
										break;
										//not used codes
										case KeyCode::DOWN_OVER:
										case KeyCode::DOWN_REPEAT:
										case KeyCode::UP_OVER:
										case KeyCode::UP_REPEAT:
										case KeyCode::ENTER_OVER:
										case KeyCode::ENTER_REPEAT:
										case KeyCode::NONE:
										break;
									}
								}
								ClearAndExit();
								return;
							}
							break;
							////////////////////////
							//  RESET
							////////////////////////
							case menu_Item::RESET:
								cli();
								asm("jmp 0");
							break;
							////////////////////////
							//  EXIT
							////////////////////////
							case menu_Item::EXIT:
								//exit
								ClearAndExit();
								return;
							break;
						}
					break;
					//not used codes
					case KeyCode::DOWN_OVER:
					case KeyCode::DOWN_REPEAT:
					case KeyCode::UP_OVER:
					case KeyCode::UP_REPEAT:
					case KeyCode::ENTER_OVER:
					case KeyCode::ENTER_REPEAT:
					case KeyCode::NONE:
					break;												
				}
			}
		break;		
		/////////////////////
		// KEY ENTER?
		/////////////////////
		case KeyCode::ENTER:
			//Show batt level
			time.SetElapsed(&tShow);
			dp.Cls();
			do{
				dp.Locate(1);
				sprintf(bf,"B%3d",GetVBatPercent());
				dp.PutS(bf);
			} while (time.GetElapsed(&tShow)<4000);
			//clear and exit
			ClearAndExit();
		break;
		//not used codes
		case KeyCode::DOWN_OVER:
		case KeyCode::DOWN_REPEAT:
		case KeyCode::UP_OVER:
		case KeyCode::UP_REPEAT:
		case KeyCode::ENTER_OVER:
		case KeyCode::ENTER_REPEAT:
		case KeyCode::NONE:
		break;		
	}
	//Set timer IO OFF
	this->SetTimer0_OFF();
}

//
//	+-------------------------------------------------------------------------------+
//	| @FUNCTION:  Get Battery Voltage								                |
//	| @NAME    :  GetVBat				          		                            |
//	| @INPUT   :				  													|
//	| @OUTPUT  :  none								                                |
//	| @REMARKS :																	|
//	+-------------------------------------------------------------------------------+
int16_t class_Feed_Timer::GetVBat()
{	
	//bellow code has updated in December,5 2014
	
  	//Internal Reference Voltagem in Millivolts
	const int16_t VRef	=1100; 
	//Resistor to Vcc of voltage divider in ohms
	const int32_t Rv	=15000L;
	//Resistor to Gnd of voltage divider in ohms
	const int32_t Rg	=2150L;
	/*_____________________________________________
		compute Vload 
	    
	     /(Rv+Rg)\
	     |-------|*ADCGetValue()*Vref
	     \  10   /
	VCC=-----------------------------
	             /(1024*Rg)\
	             |---------|
	             \   10    /
	 ______________________________________________
	*/
	return (((Rv+Rg)/10)*dp.ADCGetValue()*VRef)/((1024*Rg)/10);
}
uint16_t class_Feed_Timer::GetVBatAverage()
{
	uint32_t sum=0;
	
	for(uint8_t i=0;i<32;i++)
	{
		sum=sum+GetVBat();
		time.DelayMsI(16);
	}
	return sum/32;
}
uint16_t class_Feed_Timer::GetVBatPercent()
{
	const int32_t vLow=4000L;			//minimal battery level
	const int32_t vHigh=6500L;			//max battery level
	
	return ((GetVBatAverage()-vLow)*100L)/(vHigh-vLow);
}
//
//	+-------------------------------------------------------------------------------+
//	| @FUNCTION:  CallBack function to update internal time                         |
//	| @NAME    :  PumpCallBack			          		                            |
//	| @INPUT   :				  													|
//	| @OUTPUT  :  none								                                |
//	| @REMARKS : variable _seg,_min & _hour is updated								|
//	+-------------------------------------------------------------------------------+
//
// at 09/09/2015 17:51 this function not necessary
//
void	PumpCallBack(uint8_t hour,uint8_t minute,uint8_t second)
{
}


/*
       ==================================================================================
       =        ___  ___      _        ______                                           =
       =        |  \/  |     (_)       | ___ \                                          =
       =        | .  . | __ _ _ _ __   | |_/ / __ ___   __ _ _ __ __ _ _ __ ___         =
       =        | |\/| |/ _` | | '_ \  |  __/ '__/ _ \ / _` | '__/ _` | '_ ` _ \        =
       =        | |  | | (_| | | | | | | |  | | | (_) | (_| | | | (_| | | | | | |       =
       =        \_|  |_/\__,_|_|_| |_| \_|  |_|  \___/ \__, |_|  \__,_|_| |_| |_|       =
       =                                                __/ |                           =
       =                                               |___/                            =
       ==================================================================================
*/


int main()
{
	//LOCAL VARIABLES
	
	class_Feed_Timer	ft;
    
	uint8_t				h,m,s,w;

{
// AddEvent Test
//	struct EventStruct  Event;
	
// for(uint8_t i=0;i<24;i++)
// {
// 	Event.Event=(uint8_t) enum_EVENT_TYPE::DIARY;
// 	Event.Hour=i;
// 	Event.Minute=15;
// 	Event.WeekDays=0;
// 	Event.MinuteMotorON=0;
// 	Event.SecondMotorON=5;
// 	ft.EventAdd(Event);
// }
}

// ClockCallBack loop test
// 	for(;;)
// 		ft.ClockCallBack();

	//ENABLE GLOBAL INTERRUPTS
	sei();
  
// ShortStr Display Test
// 	uint8_t i=1;
// 	for(;;)
// 	{
// 		dp.Home();
// 		dp.PutSF(ft.GetWeekDayShortStr(i));
// 		dp.PutSF(PSTR("-#"));
// 		dp.KeyRead();
// 		i++;
// 		if (i>7) i=1;
// 	}

//FS GS RS US chracters display test
// 	dp.Home();
// 	dp.PutC(0X1C);dp.PutC(0X1D);dp.PutC(0x1E);dp.PutC(0X1F);
// 	for(;;);

// WeekDays Display Test
// 	w=0;
// 	for(;;)
// 	{
// 		ft.ShowWeekDays(w);
// 		w<<=1;
// 		w++;
// 		h=dp.KeyRead();
// 	}

//Battery Voltage Read Test
// 	char key;
// 	
// 	for(;;)
// 	{
// 		dp.Home();
// 		sprintf(bf,"%4d",(int)ft.GetVBatPercent());
// 		dp.PutS(bf);
// 		key=dp.KeyReadFlash();
// 		if (key==KEY_DOWN) dp.MotorON(); else dp.MotorOFF();
// 	}

	//SHOW DART IF KEY DOWN PRESSED DURING RESET
	if (dp.KeyReadFlash()==KeyCode::DOWN)
	{
		dp.Cls();
		dp.PutSF(PSTR("DART"));
		dp.KeyWaitRelease();
		time.DelayMsI(500);
	}

	//ANIMATION OF BOOT START

	dp.Cls();
	dp.PutSF(PSTR("-   "));
	time.DelayMsI(250);
	dp.Cls();
	dp.PutSF(PSTR(" -  "));
	time.DelayMsI(250);
	dp.Cls();
	dp.PutSF(PSTR("  - "));
	time.DelayMsI(250);
	dp.Cls();
	dp.PutSF(PSTR("   -"));
	time.DelayMsI(250);
	dp.Cls();
	time.DelayMsI(250);

	//SHOW VERSION

	dp.DispSetPoint(2,true);
	sprintf(bf,"V%3d",_FEED_TIMER_7_SEGMENTS_VERSION);
	dp.Cls();
	dp.PutS(bf);
	time.DelayMsI(2000);
	dp.DispSetPoint(2,false);
	dp.Cls();

	//INITIALIZE TIME IF BOOT IS COLD
	
	ft.InitTime();

	///////////////////////////////////////
	//MAIN LOOP
	///////////////////////////////////////

	dp.Cls();
	for(;;)
	{
		//ENTER IN SLEEP SAVE MODE TO REDUCE POWER MORE
		
		if (ft.SleepState)
		{
			//SAVE IO STATE TO REDUCE POWER
			dp.DisableIO();
			//SLEEP
			dp.PutToSleep();
			//SHOW FEED BACK OPERATIONAL
			ft.ShowLIVE();			
		}
		
		//SHOW TIME IF MOTOR ON
		
		if (dp.MotorGetState()==true)
		{
			dp.EnableIO();
			ft.SetTimer0_ON();
			dp.DispSetPoint(2,true);
			dp.Cls();
		
			//SHOW TIME WHILE MOTOR ON
		
			for(;;)
			{
				dp.Locate(1);
				ft.GetTime(h,m,s,w);
				sprintf(bf,"%02d%02d",h,m);
				dp.PutS(bf);
				if (dp.MotorGetState()==false) break;
			}
			dp.DispSetPoint(2,false);
			dp.Cls();
			ft.SetTimer0_OFF();
			dp.DisableIO();
		}
		
		//PROCESS INPUT KEYS
		
		ft.ProcessInputs();
		
	}
}    

