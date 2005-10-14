
#include <avr\pgmspace.h>

#include <stdlib.h>  //labs() only
#include <string.h>

#include "ReceiverIO.h"
#include "GPSSerialIO.h"
#include "TaskUserInterface.h" //need access to the interface API
#include "ProjectCommon.h"

static bool ReadAndProcessMessage(void);
static void ReadNMEAToken(void);
static void SkipNMEATokens(uint8_t num);
static void ProcessGPGGA(void);
static void ProcessGPRMC(void);

static void ConvertDotNumber(void);
static void TimeToTenthsSec(void);
static void ConvertDMStoD(void);

static int8_t HexCharToInt(char c);

#define MSGGotNone 0x00
#define MSGGot1st  0x01
#define MSGGotDone 0x02



static char *pNMEAMessage;
static char *pParse;				//current parse pt
static char *pToken;				//start of token
static int32_t number;
static uint32_t GGATime;

static GPSLocation* pLoc;

static uint8_t recvState;			//control sequence of location messages.
									//used to ensure correct sequence of msgs
									//used in obtaining a location. Any other 
									//msg receipts should not change this state.
													
//---------------------------------------------------------------------------
// read entire NMEA sequence from GPS module
// There are several mesages that comprise the sequence, but we are only
// interested in $GPGGA & $GPRMC. Rest are discareded by the serial comms interface
// location structure is returned
// once we have the data required, (ie filled out the GPSLocation Structure) return.
//
// WONT RETURN TILL VALID GPS MESSAGE IS READ 
//
//(will return if no fix, as this is a valid GPS message!!)

extern uint8_t GPSSIObufferOverflow;

bool RIOReadLoc(GPSLocation* loc)
{	
	pLoc = loc;			//save ptr
	
	// read messages until we have received a complete set.
	// note: if there is a CRC failure on the last message in the set,
	// we drop the entire sequence
	
	recvState = MSGGotNone;
	
	bool complete = false;
	bool tellUI;
	
	while (!complete)
	{
		complete =ReadAndProcessMessage();		//block read & decode 
												//if no GPS comms will block, therefore
												//watchdog will timeout and AVR reset!
		GPSIOReleaseNMEABuffer();				//tell ISR buffer is free
												//do ASAP so ISR has it available		
				
		gReceiverWatchdog=0;					//tell controlling thread all ok
		
		tellUI= complete;

		if (complete )
		{			
			if( (pLoc->info.bits.mode == 0)	||  // invalid data, comms ok, but
				(pLoc->info.bits.fix  == 0)) 	// GPS receiver didn't get a fix
			{
				// choose msg
				if(pLoc->info.bits.mode == pLoc->info.bits.fix) //ie, both 0
					GPSMsgStatus(GPSStatus_NoFixOrMode);
				else if(pLoc->info.bits.fix == 0)				
					GPSMsgStatus(GPSStatus_NoFix);
				else
					GPSMsgStatus(GPSStatus_NoMode );
				
				recvState= MSGGotNone;			//start new sequence
				complete = false;
			}
			else	
			{
				pLoc->lastFixTime = pLoc->timeInTenthsSecs;	//all ok, record fix time
			}
			
			// ensure the two messages are from the same sequence.
			// if there has been missing data, or corruption, might
			// skip messages and get GGA from a later sequence.
			// forget what we have as its skew.
			
			if (GGATime != pLoc->timeInTenthsSecs)
			{
				GPSMsgStatus(GPSStatus_TimeSlip);
				recvState= MSGGotNone;			//start new sequence
				complete = false;	
				tellUI= false;					//dont tell UI of data, its misleading

			}			
		}

		if (tellUI)
			UIDisplayUpdate(pLoc);				//tell UI new GPS data available

		if (GPSSIObufferOverflow)				//debug info (comms prob)
		{
			GPSMsgStatus(GPSStatus_CommsOvflow);
			GPSSIObufferOverflow=0;
		}
	} 
	
	GPSMsgStatus(GPSStatus_GotGPS);				//tell UI new GPS data received
	return true;
}

//---------------------------------------------------------------------------
//A checksum has been calculated during msg receipt, this value is 
//stored after the final null terminator.
//the returned checksum if before the message terminating <CR><LF>

bool DoChecksum(char *p)
{
	uint8_t i;
	for (i=0; i<MaxMessageLen; i++)		//look for end null terminator
		if (*p++ == 0)					//leaves pointing at checksum
			break;
		
	if (i==MaxMessageLen)				//corrupt message
	{
		GPSMsgStatus(GPSStatus_CommsCorrpt);
		return false;
	}
		
	uint8_t calcChecksum = *p;	
	uint8_t itsCheck = HexCharToInt(*(p-5))*16 + HexCharToInt(*(p-4)); //back past <CR><LF>

	if (itsCheck != calcChecksum)		//checksum failure
		GPSMsgStatus(GPSStatus_CommsCRC);

	return itsCheck == calcChecksum;	//rtn true of match.

}
//---------------------------------------------------------------------------
// chance that if CRC errors occur then RMC & GGA could appear in same message group,
// but be of diff groups because intermetiate RMC & GGA has errors
// Will need to look at time of messages to avoid this.

static bool ReadAndProcessMessage(void)
{
	eReadMessage bufferId;
	
	bufferId = GPSSIOGetNMEAMessage();		//blocking read till got message
	pNMEAMessage= GPSIOGetBufferPtr(bufferId);
	
	if (!pNMEAMessage)						//really bad error!	
	{
		gForceReset = true;					//reset CPU
		return false;
	}
			
	pParse = pNMEAMessage;
	
	if (!DoChecksum(pNMEAMessage))
		return false;
	
	GPSMsgStatus(GPSStatus_CommsOK);		//tell UI comms is ok
	
	// since the comms stuff filters out the messages we dont want,
	// only need to look of uniquness in the messages we can get in order to 
	// id which they are.
	
	if (bufferId == eRMC)						//$GPRMC, the 1st message is sequence
	{
		// send this message to the PC/PDA, its all it needs!
		// Also see GPSSIOISR() for alternate output
		
		//DBGPutStr_P(PSTR("\n$"));
		//PCSIOPutStr (pNMEAMessage);
		//DBGPutc('\n');

		ProcessGPRMC();
		recvState=MSGGot1st;					//set state of receipt sequence			
	}
	else
	if (bufferId == eGGA)						//$GPGGA
	{	
		if (recvState == MSGGot1st)				//check for sequence error
		{
			ProcessGPGGA();						
			recvState=MSGGotDone;				// sequence complete, process it
			return true;						//we have filled the loc structure
		}
	}
		
	return false;								//not interested/more to come
}


//---------------------------------------------------------------------------
// process the GGA message, placing data in GPSLocation structure
// $GPGGA,104549.04,2447.2038,N,12100.4990,E,1,06,01.7,00078.8,M,0016.3,M,,*5C<CR><LF>
//			time     lat           long     fix | prcn ant hgt   goe hgt   check
//											  num sat

static void ProcessGPGGA(void)
{
	SkipNMEATokens(1);			//skip msg name

	ReadNMEAToken();			//read time
	
	// need time only for checking that the message is part of the pair
	
	ConvertDotNumber();			//convert to number (get rid of dot)
	TimeToTenthsSec();			//to (10th) secs.used for validation only
	GGATime = number;
	
	
	ReadNMEAToken();			//read latitude
	ConvertDotNumber();			//convert to number (get rid of dot)
	ConvertDMStoD();			//dddmm.mmmm to ddd.dddddd
	
	ReadNMEAToken();			//read hemishpere
	if (*pToken == 'S')
		number = -number;
	pLoc->latitude = number;
	
	ReadNMEAToken();			//read longitude
	ConvertDotNumber();			//convert to number (get rid of dot)
	ConvertDMStoD();			//dddmm.mmmm to ddd.dddddd

	ReadNMEAToken();			//read hemishpere
	if (*pToken == 'W')
		number = -number;
	pLoc->longitude = number;
	
	ReadNMEAToken();			// read fix, 
	pLoc->info.bits.fix = *pToken!='0';	//fix=0 if no fix.
	
	SkipNMEATokens(2);			//loose precision
	
	ReadNMEAToken();			//read antenna height
	ConvertDotNumber();			//convert to number (get rid of dot)
	pLoc->antHeight = number;
	
//	SkipNMEATokens(1);			//loose 'M' (units?)

//	ReadNMEAToken();			//read geo height
//	ConvertDotNumber();			//convert to number (get rid of dot)
//	pLoc->geoHeight = number;
	
}

//---------------------------------------------------------------------------

static void ProcessGPRMC(void)
{

	SkipNMEATokens(1);			//skip msg name

	ReadNMEAToken();			//read time
	ConvertDotNumber();			//convert to number (get rid of dot)
	pLoc->UTCTime = number;		//for display only, converted time (below) is stored

	TimeToTenthsSec();			//conv to seconds as we store deltas in flash
								//and time from GPS is hhmmss.ss (& u get a jump)

	pLoc->timeInTenthsSecs = number;
	
	ReadNMEAToken();			//read satus
	pLoc->info.bits.status = *pToken == 'A';
			
	SkipNMEATokens(4);
	ReadNMEAToken();			//get speed
	ConvertDotNumber();			//convert to number (get rid of dot)
	pLoc->speed = number;

	ReadNMEAToken();			//get course (0.359.9)
	ConvertDotNumber();			//convert to number (get rid of dot)
	pLoc->course = number;

	ReadNMEAToken();			//get date
	ConvertDotNumber();			//convert to number (get rid of dot)
	pLoc->UTCDate = number;

	SkipNMEATokens(2);
	ReadNMEAToken();										//get mode
	if      (*pToken == 'N') pLoc->info.bits.mode=0;		//invalid data
	else if (*pToken == 'A') pLoc->info.bits.mode=1;
	else if (*pToken == 'D') pLoc->info.bits.mode=2;
	else if (*pToken == 'E') pLoc->info.bits.mode=3;

}

//---------------------------------------------------------------------------
//skip 'num' tokens
//pToken will return pointing to statr of next token

static void SkipNMEATokens(uint8_t num)
{
	pToken = pParse;
	while (num--)
	{
		while (*pParse++ != ',')
		{
		}
	}
}
//---------------------------------------------------------------------------

// tokens are terminated with ',' or CR, must do as CRC has been tested
// read the token, replace terminator with 0, to make null terminates string
// current position is at first char to be read

static void ReadNMEAToken(void)
{	
	pToken = pParse;
	while (1)
	{
		if ((*pParse == CR) || (*pParse == ','))
		{
			*pParse++ =0;
			return;
		}
		pParse++;
	}
}
//---------------------------------------------------------------------------
// convert a text number (in token) that has a dot in it to a 32bit number
// dont care about the position of the dot, the caller knows its a fixed point
// number and what the scaling is.

static void ConvertDotNumber(void)
{
	char *p =pToken;
	number=0;
	
	while (*p)
	{
		if (*p != '.')
		{
			number*=10;
			number+=*p-'0';
		}
		p++;
	}
}
//---------------------------------------------------------------------------
/*
static void ConvertNumber(void)
{
	char *p =pToken;
	number=0;
	
	while (*p)
	{		
		number*=10;
		number+=*p-'0';		
		p++;
	}
}
*/
//---------------------------------------------------------------------------
//dddmm.mmmm to ddd.dddddd (minutes to fractions of degree)

static void ConvertDMStoD(void)
{
	//always 6 minute digits after . and 1..3 before
	
	uint32_t m;
	
	m= labs(number%LatLongScaleFactor);
	number -=m;
	m=m*10/6;	//minutes to fracts of degree ie m/600000*1000000;
	number +=m;	
}
//---------------------------------------------------------------------------
//convert time in hhmmss.ss to 10th seconds (since midnight)

static void TimeToTenthsSec(void)
{
	uint32_t t;
	
	t= number;
	
	t/=10;					// value is100th, but only want tenths.
	number=t%10;			// get 10ths
	t/=10; 					// loose 10ths
	number +=t%100*10;		// get secs	
	t/=100; 				// loose sec
	number+=t%100*60*10L;	// get mins
	t/=100;					// loose mins
	number+=t*60*60*10L;	// get hrs	
}
//---------------------------------------------------------------------------

static int8_t HexCharToInt(char c)
{
	if (c>='0' && c<='9')
		return c-'0';
	if (c>='A' && c<='F')
		return c-'A' + 10;
	if (c>='a' && c<='f')
		return c-'a' + 10;
		
	return -1;	
}
