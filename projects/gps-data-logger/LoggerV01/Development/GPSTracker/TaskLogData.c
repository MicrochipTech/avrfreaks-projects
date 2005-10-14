//TaskLogData.c

#include <inttypes.h>

#include <..\libraries\avrx\avrx\AvrX.h>

#include <avr\pgmspace.h>
#include <avr\interrupt.h>
#include "..\libraries\dataflash\Multithread\dataflash.h"

#include "TaskUserInterface.h" //need access to the interface API
#include "ProjectCommon.h"
#include "GPSFlashRW.h"
#include "GPSSerialIO.h"



#define NewTripGap 4*60*60*10L	//All 4 hour gap before a new trip is started.

Mutex waitForGUISignal;			//UI has opportunity to stop/reset logging etc
Mutex flashInitialisedSignal;	//Indicate to UI that flash is reset & ready 

extern bool gWantsNewTrip;		// new trip requested in UI.
extern uint16_t gAveBattADC;	// batt ADC value (-1 till sample got.)

//-----------------------------------------------------------------------------------
//--------------------------- THE THREAD --------------------------------------------
//-----------------------------------------------------------------------------------

static GPSLocation loc;			//location data from GPS.
static int32_t lastTime=0;		//message time (to determine gaps in GPS data stream)

/*
AVRX_GCC_TASKDEF(taskLogData, 300, TSK_taskLogData) 
{ 
	//Dont use this AVRX contruct - fails to init Y register.
	//Use AVRX_GCC_TASK is the main.c and make the called fucntion void fx(void)
}
*/

void TaskLogData(void)
{
	
	GPSTripEntry lastEntryLoc;		//data of the last recorded trip 
								
	DBGPutStr_P (PSTR("\n#TaskLogData running\n"));
	DFSPIInit();
		
	uint16_t s = DFReadStatus();
	DBGPutStrNum_P(PSTR("status ="),s);
	
	if (!DFFlashOK())
		PCSIOPutStr_P(PSTR("\n!**** Flash Error ****\n"));
		
	if (sizeof(FRWGPSIOBuffer) > DFPageSize)
		PCSIOPutStrNum_P(PSTR("\n!GPSIO buffer too big @ "),sizeof (FRWGPSIOBuffer));

	//DBGPrint_P(PSTR("buf sz=%d\n"),sizeof(FRWGPSIOBuffer));	
	//FRWTest();
		
	// Tell the other threads the Flash has been initialised. It may
	// not be working however, users must call DFFlashOK() to find out.
	
	AvrXSetSemaphore(&flashInitialisedSignal);	
	
	//Wait for the user interface.
	//It may want to talk to PC/User before data logging starts.
	
	AvrXWaitSemaphore(&waitForGUISignal);
	
	GPSInitSerialIO();	 		//inis GPS serial io stuff
	
	// if the DataFlash isn't working, keep GPS messages outing out or die
		
	if (!DFFlashOK())
	{
		GPSMsgStatus(GPSStatus_NoFlash);
		// AvrXTaskExit();
		while (true)
			RIOReadLoc(&loc);
	}
	// Reset the flash state machine as the user interface may have made requests that
	// effect the read buffers contents etc.
	// When at this point, the data logging has begun and the UI must not make requests
	// of the Flash interface as it will alter the buffers being used by the loggin 
	// routines
	
	FRWStateReset();						//Reset after UI has finished playing.

	
	// Read the GPS and the last trip recorded, decide if a new trip is to be 
	// begun. 
	// If the flash is empty, the data in loc will be zero, which will force a
	// new trip.
	// FRWStateReset has loaded the RW buffer with the last trips last page
	
	
	DBGPutStr_P(PSTR("#Reading Flash.."));
	FRWCopyReadBufferHeader(&lastEntryLoc);		//read last stored entries trip header
	
	DBGPutStr_P(PSTR("#Reading GPS.."));
	RIOReadLoc(&loc);							//read current GPS values, (no return till got)
	
	//note: if the GPS isnt sending data (ie no comms/crap) then wont get
	//here, as RIOReadLoc wont return till valid message received.

	DBGPutStrNum_P(PSTR("#Current loctn lat="), loc.latitude);
	DBGPutStrNum_P(PSTR("#Current loctn lng="), loc.longitude);

	if ((loc.UTCDate != lastEntryLoc.UTCDate) ||
		(loc.timeInTenthsSecs - lastEntryLoc.timeInTenthsSecs >NewTripGap))
	{
		FRWStartNewTrip();
		DBGPutStrNum_P(PSTR("#New trip ="),FRWGetCurrentTripNo());
	}
	loc.lastFixTime=0;
	
	// start-up complete, enter main processing loop
	
	while(true)
	{		

		RIOReadLoc(&loc);							//read GPS data, wont return till valid msg read.
		
		//DBGPutStrNum_P(PSTR("Bt="),gAveBattADC);	//display batt ADC value
		
		// has there been a gap in the data stream (eg comms errors, or processing delay)
		
		int deltaTime = (int)(loc.timeInTenthsSecs - lastTime);
		if (lastTime && (deltaTime != 10))
		{
			DBGPutStrNum_P(PSTR("DT="),deltaTime);	//send time delta to PC
		}
		lastTime = loc.timeInTenthsSecs; 
			
		FRWAddGPSEntry(&loc);						//send data to flash interface	
	}
}
