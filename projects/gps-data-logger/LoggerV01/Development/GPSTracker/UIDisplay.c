//Display data on user interface
//Designed around a 16x2 LCD display unit

//
//
//   UNTESTED - Will compile but that all.
//
//
//
#include <inttypes.h>
#include <..\libraries\avrx\avrx\AvrX.h>

#include "ProjectCommon.h"
#include "TaskUserInterface.h"
#include "LatLongToOSGB_NoHeight.h"
#include "GPSFlashRW.h"
#include "TextFormat.h"
#include <string.h>  //memset only


#define Line1 1
#define Line2 2

#define AcceptableCommsLoss  2 		 //in seconds
#define AcceptableSignalLoss 4	 	 //in seconds

enum eDisplayLine1 {eNoSignal,eOSGB,eLngLat_Lat,eLngLat_Lng};
enum eDisplayLine2 {eTimeBat, eHiDir};

static enum eDisplayLine1 currentDisplayLine1 = eNoSignal;
static enum eDisplayLine2 currentDisplayLine2 = eTimeBat;

static bool DisplayNoFixOrComms(void);
static void DisplayLocation(void);
static void UIDisplayUTCTime(uint32_t time, uint8_t pos, uint8_t line) ; //time is of form hhmmss (ie UTC format)
static void UIDisplayTime(uint32_t time, uint8_t pos, uint8_t line);  //time is seconds of day
static inline void PutSInLine1(const char* str, uint8_t pos);
static inline void PutPInLine1(const char* str, uint8_t pos);
static inline void PutCInLine1(char c, uint8_t pos);
static inline void PutSInLine2(const char* str, uint8_t pos);
static inline void PutPInLine2(const char* str, uint8_t pos);
static inline void PutCInLine2(char c, uint8_t pos);

//                       01234567890123456789012345678901234567890
static char outLine1[] ="                ";
static char outLine2[] ="                ";

//Ht=0000m Dir=000'
//Time=00:00 Bat=000;

extern uint16_t noCommsCount;
extern uint16_t gAveBattADC;		//Batt ADC value, -1 indicates invalid till 1st sample.

//-----------------------------------------------------------------------------
// Updates the display with the current cordinates.
// Each time this fn is caled it will display the next item in the list of things 
// to display Err, OSGB, Lng,Lat etc on line 1 & Bat,Height etc on line 2

void UIUpdateDisplay(void)
{

	// line 1
	if (currentDisplayLine1 == eNoSignal)
	{
		currentDisplayLine1++;
		if (!DisplayNoFixOrComms())		//display error if necc
			DisplayLocation();			//else location data
	}
	else
	{
		DisplayLocation();
		if (currentDisplayLine1++==eLngLat_Lng)
			currentDisplayLine1=eNoSignal;
	}
	
	//line 2
	if (currentDisplayLine2 == eTimeBat)							//Time & Batt
	{
		uint32_t time =UILoc.UTCTime/100;						 	//time of message ( UTCTime is hhmmss.ss)
		PutPInLine2(PSTR("Tm=00:00 "),0),
		UIDisplayUTCTime(time,3,Line1);
		PutPInLine2(PSTR("Bat=000 "),9);
		PutSInLine2(LtoaN(gAveBattADC,4),13);
		currentDisplayLine2=eHiDir;

	}
	else															//Height & Direction
	{
		PutPInLine2(PSTR("Ht=0000m "),0),
		PutSInLine2(LtoaN(UILoc.antHeight/10,4),3);
		PutPInLine2(PSTR("Dir=000'"),9),
		PutSInLine2(LtoaZ(UILoc.course/10,3),13);
		currentDisplayLine2=eTimeBat;
	}
	
	// Data output has now been generated. Display it.
	// Use LCD libraries to display it.
	
	//..TBD
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//look for no Fix or no GPS comms	
//look for time since last fix - max out at 99:59 secs. will be 0 if all is well

static bool DisplayNoFixOrComms(void)
{
	
	if ( noCommsCount > AcceptableCommsLoss*(1000/ScreenRefreshPeriod)) // No comms at all from the GPS module?
	{																	// comms fault between AVR and GPS module
		uint16_t time =noCommsCount/(1000/ScreenRefreshPeriod);
		if (time > 5999) time=5999;	
		PutPInLine1(PSTR("No Comms 00:00s"),0);							//...put in output - "No Comms 00:00s"
		UIDisplayUTCTime(time,9,Line1);
		return true;

	}
	
	uint16_t fixDelta;
	
	if ((UILoc.timeInTenthsSecs - UILoc.lastFixTime) >=100L*60*10) 		//>100 minutes? (in 10ths sec)
		fixDelta = 5999;											 	//99:59 (in sec)
	else
		fixDelta = (UILoc.timeInTenthsSecs - UILoc.lastFixTime)/10; 	//time since last fix(in sec)
		
	if (fixDelta > AcceptableSignalLoss)								// Getting GPS messages...,
	{																	// but GPS data isn't valid. eg no satellites
		PutPInLine1(PSTR("No Fix 00:00s"),0);							//...put in output - "No Fix 00:00s"
		UIDisplayUTCTime(fixDelta,7,Line1);	
		return true;
	}
	
	return false;
}
//-----------------------------------------------------------------------------

static void DisplayLocation(void)
{
	if (currentDisplayLine1 == eOSGB)
	{
		uint8_t error =LLtoOSGB(UILoc.latitude/ (double)LatLongScaleFactor, 
								UILoc.longitude/(double)LatLongScaleFactor);
								
		if (error)
			PutPInLine1(PSTR("Invalid"),0);
		else
		{		
			PutSInLine1(OSGBGridSquare,0);				
			PutSInLine1(LtoaZ(OSGBEasting ,5),3);
			PutSInLine1(LtoaZ(OSGBNorthing,5),9);
		}
	}
	else
	if (currentDisplayLine1 == eLngLat_Lat)
	{	
		PutPInLine1(PSTR("Lat "),0);
		PutSInLine1(LtoaLL(UILoc.latitude,10),4);				//eg 054.000000	
	}
	else 
	if (currentDisplayLine1 == eLngLat_Lng)
	{
		PutPInLine1(PSTR("Lng "),0);
		PutSInLine1(LtoaLL(UILoc.longitude,10),4);				//eg -03.000000
	}
}

//-----------------------------------------------------------------------------------

static void UIDisplayUTCTime(uint32_t time, uint8_t pos, uint8_t line)  //time is of form hhmmss (ie UTC format)
{
	if (line == Line1)
	{
		PutSInLine1(LtoaZ(time/10000,2),pos);		
		PutSInLine1(LtoaZ(time/100%100,2),pos+3);
	}
	else	
	{
		PutSInLine2(LtoaZ(time/10000,2),pos);		
		PutSInLine2(LtoaZ(time/100%100,2),pos+3);
	}
	//PutSInLine(LtoaZ(time%100),pos+6);
}
//-----------------------------------------------------------------------------------

static void UIDisplayTime(uint32_t time, uint8_t pos, uint8_t line)  //time is seconds of day
{
	if (line == Line2)
	{
		PutSInLine1(LtoaZ(time/3600,2),pos);
		PutSInLine1(LtoaZ(time%3600/60,2),pos+3);
	}
	else
	{
		PutSInLine2(LtoaZ(time/3600,2),pos);
		PutSInLine2(LtoaZ(time%3600/60,2),pos+3);
	}
	
	//PutSInLine(LtoaZ(time%60,2),pos+6);
}

//-----------------------------------------------------------------------------------

static inline void PutSInLine1(const char *str, uint8_t pos)
{
	strcpy(outLine1+pos,str);
}
static inline void PutPInLine1(const char *str, uint8_t pos)
{
	strcpy_P(outLine1+pos,str);
}
static inline void PutCInLine1(char c, uint8_t pos)
{
	outLine1[pos]=c;
}
//-----------------------------------------------------------------------------------

static inline void PutSInLine2(const char *str, uint8_t pos)
{
	strcpy(outLine2+pos,str);
}
static inline void PutPInLine2(const char *str, uint8_t pos)
{
	strcpy_P(outLine2+pos,str);
}
static inline void PutCInLine2(char c, uint8_t pos)
{
	outLine2[pos]=c;
}

//-----------------------------------------------------------------------------------
// speed...
//uint32_t speedInMPH = UILoc.speed * 115/1000; //10th knots/ph to MPH	
//if (speedInMPH >999) speedInMPH =999;
