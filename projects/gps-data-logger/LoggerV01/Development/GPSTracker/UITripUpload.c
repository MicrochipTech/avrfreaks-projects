
#include <avr\pgmspace.h>


#include "string.h" 
#include "GPSFlashRW.h"
#include "..\libraries\dataflash\Multithread\dataflash.h"

#include "TaskUserInterface.h" 	//need access to the ltoZ fn
#include "LatLongToOSGB_NoHeight.h"
#include "TextFormat.h"

#include "UITripUpload.h"


static void PrintEntry(GPSTripEntry *loc, bool dispOSGB);
static bool GetTripStart(uint16_t tripNo);
static bool WalkTripEntries(GPSTripEntry* loc);
static void DoTripUpload(int16_t tripNo, bool justHeader);

static int8_t   sEntryNo;
static int16_t  sPageNo;
static uint16_t sTripNo;

extern uint8_t recordedResetCause;

void UIDisplayHeader(int16_t tripNo)
{
	DoTripUpload(tripNo,true);
}
//--------------------------------------------------------------------------

void UITripUpload(int16_t tripNo)
{		
	DoTripUpload(tripNo,false);
}

//--------------------------------------------------------------------------
// Print Trip to serial port
// When the flash is reset (by the caller) the last trip entry will have been found
// and the flash read buffer loaded (this fact is used in the search and is used to
// provide the tripNo param)

static void DoTripUpload(int16_t tripNo, bool justHeader)
{

	GPSTripEntry loc;
	
	if (!DFFlashOK())					//flash not working, no trip upload!
	{
		PCSIOPutStr_P(PSTR("Flash Error - cant upload\n"));
		return;
	}
	
	PCSIOPutStrNum_P(PSTR("Trip No. "),tripNo);
	
	if (tripNo <= 0)								// not a valid trip.
	{
		PCSIOPutStrNum_P(PSTR("Trip No. Invalid "),tripNo);
		return;
	}

	if (!GetTripStart(tripNo))						//side effect, flash page in flash buffer 
	{
		PCSIOPutStrNum_P(PSTR("Trip Not Found "),tripNo);
		return;
	}

	PCSIOPutStrNum_P(PSTR("Start Flash Page "),sPageNo);
	
	if (WalkTripEntries(&loc))
	{
		PCSIOPutStr_P(PSTR("UTC Date "));
		PCSIOPutStr(LtoaZ( loc.UTCDate,6));
		PCSIOPutChar('\n');
					
		do
		{
			PrintEntry(&loc, false);
		}
		while (WalkTripEntries(&loc) && !justHeader);	//only display 1 if only header wanted
	}
	
	PCSIOPutStrNum_P(PSTR("End Flash Page "),sPageNo);
	
}

//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
//Get the details of the specified trip
//After this call, use WalkTripEntries to get all the entries of the trip.
//the starting point (start page) is the currently last read page, the chances are that
//if the trips are being displayed in historical order then the start page will speed things
//up as it will act as a hint for the start point. If the start page hint is wrong then the 
//search routine will endup searching all of memory (not too bad anyway)

static bool GetTripStart(uint16_t tripNo)
{
	sEntryNo =-1;											//need to read header in Walk
	sTripNo = tripNo;
	int startPage = FRWGetLastReadPageNo();					//page # the FRW_INBUF contains
	if (startPage <0)										//not valid (last trip not found?)								
		startPage=0;										//search all memory for next
	sPageNo = FRWFindTrip1stPage( sTripNo,startPage);		//finds and loads Page header (only)
	FRWReadPage(sPageNo);									//read entire page
	
	return (sPageNo >=0);									// trip found, or not
}
//--------------------------------------------------------------------------
//Display the next entry in the trip
//Itterate through the entries in the trip
//loc is updated with the current data.
//the page header contains the abs GPS values for the page, the remaining entries are deltas.
//need to read the header onto 'loc' then itterate through the page adding the deltas

static bool WalkTripEntries(GPSTripEntry* loc)
{
	if (sEntryNo ==-1)								// first entry in page is header. read it!
	{
		sEntryNo=0;
		return FRWCopyReadBufferHeader(loc);		//copy trip header to loc (flash page already in buffer)
	}
	if (!FRWCopyReadBufferEntry(loc,sEntryNo))		// get 'sEntryNo' trip entry from buffer (adds deltas)
	{												// if no more in this page, see if next page continues
		sPageNo = (sPageNo+1)%FRW_NUMGPS_PAGES;		// id next page in flash
		if (sPageNo==0)
			PCSIOPutStr_P(PSTR("FLASH WRAPPED"));

		if (FRWReadPage(sPageNo) != sTripNo)		// read entire page to flash buffer, rtns trip #
			return false;							// end of trip
		sEntryNo=0;
		return FRWCopyReadBufferHeader(loc);		//copy trip header from flash buffer to loc
	}												//header has abs values, now need to itteratively add.
	sEntryNo++;
	return true;									//we have an entry
}
//--------------------------------------------------------------------------
// print the entire contents of the flash, in a friendly form.
// no regard for trip ordering, just prints out from page 0...
/*
void UIUploadRecords(int s, int e)
{
	GPSTripEntry loc;
	
	for (int p=s; p<=e; p++)
	{
		PCSIOPutStr_P(PSTR("Trip  Time       Latitude  Longitude Height   oflw\n"));
						  //xxxxx xxxxxxxxxx xxxxxxxxx xxxxxxxxx xxxxxx    x          
		
		FRWReadPage(p);
		FRWCopyReadBufferHeader(&loc);
		PrintEntry(&loc,false);
		
		for (int i=FRW_FIRSTGPS_PAGE;i<=FRW_LASTGPS_PAGE; i++)
		{
			if (!FRWCopyReadBufferEntry(&loc,i))
				break;
			PrintEntry(&loc,false);
		}		
		PCSIOPutStr_P(PSTR("------\n"));
	}
}
*/
//--------------------------------------------------------------------------
                              
static char PROGMEM resetTxt[]= 
//012345 012345 012345 012345 012345 012345 012345 012345 
 "PORF\0 EXTRF\0BORF\0 WDRF\0 JTRF\0               JTD";

static void PrintEntry(GPSTripEntry *loc, bool dispOSGB)
{
	
	if (sEntryNo==0)					//id first in page
	{
		// display reset cause (if any) (1st commit always has a cause)
		uint8_t idx=0;
		uint8_t i=recordedResetCause;
		while (i)
		{
			if (i&1)			
			{
				PCSIOPutStr_P(PSTR("Reset :"));
				PCSIOPutStr_P(&resetTxt[idx]);	//get text
				PCSIOPutChar('\n');
			}
			idx+=6;
			i>>=1;
		}
		PCSIOPutChar('*');
	}
	else
		PCSIOPutChar('>');
		
	PCSIOPutStr(LtoaZ( loc->timeInTenthsSecs/10,6));
	PCSIOPutStr_P(PSTR(" "));

	PCSIOPutStr(LtoaZ( loc->latitude, 8));
	PCSIOPutChar(' ');
	PCSIOPutStr(LtoaZ( loc->longitude,8));
	PCSIOPutChar(' ');
	PCSIOPutStr(LtoaZ( loc->antHeight,8));		//height is only stored in header!
	PCSIOPutChar(' ');
	PCSIOPutStr(LtoaZ( loc->info.bits.entryDeltaOverflow,1));
	
	if (dispOSGB)
	{
		LLtoOSGB(loc->latitude/1000000.0,loc->longitude/1000000.0);
		PCSIOPutStr_P(PSTR("  "));
		PCSIOPutStr(OSGBGridSquare);
		PCSIOPutStr_P(PSTR("  "));
		PCSIOPutNum(  OSGBEasting);
		PCSIOPutStr_P(PSTR("  "));
		PCSIOPutNum(OSGBNorthing);
	}
	PCSIOPutChar('\n');

}

//--------------------------------------------------------------------------
//Send data in flash to serial port.

void UIDumpFlashRaw(int s, int e)
{
	
}

//--------------------------------------------------------------------------
// called only after change in data structures 

void _UIEraseFlash(void)
{
	if (!DFFlashOK())
	{
		PCSIOPutStr_P(PSTR("Cant erase Flash - init problem\n"));
		return;
	}
	PCSIOPutStr_P(PSTR("Erasing Flash...\n"));
	for (int i=0;i<FRW_FLASHPAGES ;i++)
		DFErasePage (i);
			
	PCSIOPutStr_P(PSTR("Done\n"));

}