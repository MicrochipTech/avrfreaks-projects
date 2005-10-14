
//Buffered version of GPS Flash writing
//A Page of the size of the flash buffer is used to buffer the output. 
//When the buffer is complete, it is comitted to the flash device

#include <avr\pgmspace.h>
#include <limits.h>

#include "GPSFlashRW.h"
#include "..\libraries\dataflash\Multithread\dataflash.h"
#include "TaskUserInterface.h" //need access to the interface API
#include "stdlib.h"

#define FRW_OUTBUF 1			//flash device buffer 1 for writing
#define FRW_INBUF  2			//flash device buffer 2 for reading

extern void *memset(void *, int, size_t);

extern uint8_t gResetCause;		//last reason for reset
uint8_t recordedResetCause=0;

static int8_t   currentIndex=-1;	//slot index in entry in page being built
static uint16_t nextWritePage =0;   //page that will be written on next commit to Flash
static uint16_t currentTripNo=0;
static int16_t  lastReadPage=-1;	//page number that is in read buffer
static int16_t  tripStartPage=-1;

GPSLocation lastLoc;

// have two seperate buffers fo reading and writing. This allows reading
// of the flash to take place while an output record is being constructed.
// Also, sperate on chip flash buffers are used for reading  writing.

uint8_t  flashGenericReadBuffer[DFPageSize];
uint8_t  flashGenericWriteBuffer[DFPageSize];

// point to the generic buffer.
// by making this a const, it should act as an equate, ie the pointer stuff will be
// optimised out and direct reference to memory address substituted.
// doing it like this allows other routines to share the same buffer and void having to
// use unions.
// eg same as #define flashGPSWriteBuffer  ((FRWGPSIOBuffer *)(&flashGenericWriteBuffer))

FRWGPSIOBuffer *const flashGPSWriteBuffer = (FRWGPSIOBuffer *)(&flashGenericWriteBuffer);
FRWGPSIOBuffer *const flashGPSReadBuffer  = (FRWGPSIOBuffer *)(&flashGenericReadBuffer);


// READ & WRITE THREAD ACCESS
// There can be a reader and writer both accessing the flash device at the same time.
// This is because the low level DF routines use the multithreaded SPI access library.
// While the SPI library is designed to share more than one device on the SPI, it does
// it with a mutex. This mutex will guard access even if its the same SPI device trying 
// to be accessed from multiple threads. (its a cool side effect of the SPI implemetation).
// Obviously this library must emsure that the function calls used by the readers and 
// writers dont effect each other within this library (ie shareing buffers/data etc)
// ie reading and writing threads must not interfere with each other within this lib.
// There can be only one reader and/or one writer thread accessing at anyone time as 
// the reading & writing functions are reenterent (within themselves).

static void     AddHeaderToRecord(GPSLocation* loc);
static void     AddEntryToRecord(GPSLocation* loc);
static uint16_t ReadLastUsedPage(void);
static uint16_t GenerateCRC(void);


//--------------------------------------------------------------------------
//Initialise the FlashRW utilities.
//On exit, the read buffer will contain the last trips last page entry.
//ie where we last stopped.
//and currentTripNo will be the last trip number.

uint16_t FRWStateReset(void)
{
	nextWritePage = ReadLastUsedPage();
	nextWritePage = (nextWritePage+1) % FRW_NUMGPS_PAGES; 	//next slot

	return currentTripNo;											//last trip
}
//--------------------------------------------------------------------------
// page # of the current read buffer - what page number the flash read buffer
// (FRW_INBUF) came from.
// only valid after a call to read a page. If written then set not valid, as teh write
// may have overwritten the page that the buffer held a copy of (caller needs to re-read)

int16_t  FRWGetLastReadPageNo(void)		{	return lastReadPage; }

uint16_t FRWGetCurrentTripNo(void)  	{	return currentTripNo;}
int8_t   FRWGetCurrentEntryIndex(void)	{	return currentIndex; }
int16_t  FRWGetTripStartPageNo(void)	{	return tripStartPage;}
uint16_t FRWGetNextWritePage(void)		{   return nextWritePage;}

//--------------------------------------------------------------------------

uint16_t FRWGetNumEntriesInCurrentTrip (void)
{
	if (tripStartPage<0)
		return 0;
		
	if (tripStartPage < nextWritePage)
		return nextWritePage - tripStartPage;
	else
		return FRW_FLASHPAGES-tripStartPage + nextWritePage;
}


//--------------------------------------------------------------------------
// Start new trip, best only used at initialisation, any data in current entry 
// will be lost.

void FRWStartNewTrip(void)
{
	currentTripNo++;					//next trip number		
	currentIndex =-1;
}
//--------------------------------------------------------------------------

void FRWAddGPSEntry(GPSLocation* loc)
{
	if (currentIndex <0)		//new Page has started
	{
		AddHeaderToRecord(loc);	//copy to FRWPage
		currentIndex=0;
		lastLoc =*loc;			//save current loc, used for storing deltas in block entries
		return;
	}
	
	AddEntryToRecord(loc);		//copy to FRWPage
	currentIndex++;
	
	// if the buffer is full, commit it to flash
	// and start a new buffer
	
	if (currentIndex == FRW_NUMSLOTS)
	{
		flashGPSWriteBuffer->header.pageCRC = GenerateCRC();
		FRWCommit();
		currentIndex =-1;
	}
	else
		lastLoc =*loc;		//save current loc, used for storing deltas in block entries	
}
//--------------------------------------------------------------------------
// commit the current Page, (can used to comit at any point)
// perform a compare, & returns true if data written successfully.
// If the current trip fill the entire memory then the start of the trip will be
// over written by the later data.

bool FRWCommit(void)
{	
	if (currentIndex<0)			//empty Page?
		return true;	
	
	GPSMsgStatus(GPSStatus_Commit);
	DFWriteToBuffer(FRW_OUTBUF,0,sizeof(flashGenericWriteBuffer),(char*)flashGPSWriteBuffer);
	DFBufferToPage(FRW_OUTBUF,nextWritePage);
												
	if (tripStartPage <0)						//if first entry of trip then save start page
	{
		tripStartPage=nextWritePage;
	}
		
	bool compare = DFPageToBufferCompare(nextWritePage,FRW_OUTBUF);
	
	if (!compare)
	{
		GPSMsgStatus(GPSStatus_FlashErr);
		DBGPutStrNum_P(PSTR("#Flash Cmp Err"),nextWritePage);
	}	
	
	DBGPutStrNum_P(PSTR("#commit"),nextWritePage);

	if (++nextWritePage == FRW_NUMGPS_PAGES) 		// if the flash is full, wrap
		nextWritePage =0;

	return compare;		
}
//--------------------------------------------------------------------------
//copy data from the GPSLocation structure to the header

static void AddHeaderToRecord(GPSLocation* loc)
{	
	static bool haveRecorded=false;
	
	memset((char*)flashGPSWriteBuffer,0,sizeof(flashGenericWriteBuffer));	//clear entire buffer

	flashGPSWriteBuffer->header.tripNo    = currentTripNo;
	flashGPSWriteBuffer->header.timeInTenthsSecs   = loc->timeInTenthsSecs;
	flashGPSWriteBuffer->header.UTCDate   = loc->UTCDate;
	flashGPSWriteBuffer->header.latitude  = loc->latitude;
	flashGPSWriteBuffer->header.longitude = loc->longitude;
	flashGPSWriteBuffer->header.antHeight = loc->antHeight;
	flashGPSWriteBuffer->header.info.all  = loc->info.all;
	
	//this var identifies the reason for the reset and its stored in the first commit.
	//(will be zero on subsiquent commits, until another reset)
	
	if (!haveRecorded)
	{
		flashGPSWriteBuffer->header.resetCause=gResetCause;
		haveRecorded=true;
	}
	
	flashGPSWriteBuffer->entry[0].dTime = (uint16_t)-1;	//mark the 1st slot with the end marker
}
//--------------------------------------------------------------------------
// stores only the delta between the last and current location (saves space)

static void AddEntryToRecord(GPSLocation* loc)
{
	int32_t delta;
	loc->info.bits.entryDeltaOverflow=0;

	// check the deltas, but in reality, they should never overflow as the 
	// time range is 1.8 hours or several thousand meters
	
	// no midnight check (unlikley anyway!)
	
	delta= loc->timeInTenthsSecs  - lastLoc.timeInTenthsSecs;
	if ((delta >INT_MAX) || (delta <INT_MIN))
		loc->info.bits.entryDeltaOverflow=1;
	else
		flashGPSWriteBuffer->entry[currentIndex].dTime =(int)delta;
	
	delta = loc->latitude - lastLoc.latitude;	
	if ((delta >INT_MAX) || (delta <INT_MIN))
		loc->info.bits.entryDeltaOverflow=1;
	else
		flashGPSWriteBuffer->entry[currentIndex].dLat =(int)delta;
	
	delta = loc->longitude - lastLoc.longitude;
	if ((delta >INT_MAX) || (delta <INT_MIN))
		loc->info.bits.entryDeltaOverflow=1;
	else
		flashGPSWriteBuffer->entry[currentIndex].dLong =(int)delta;
	
	flashGPSWriteBuffer->entry[currentIndex].info.all   = loc->info.all;
	
	// mark the next slot with the end marker
	
	if (currentIndex+1 < FRW_NUMSLOTS)
	{
		flashGPSWriteBuffer->entry[currentIndex+1].dTime = (uint16_t)-1;
	}
}


//--------------------------------------------------------------------------
// Read last used page.
// search through the memory looking for highest trip number, and the last Page in
// this trips list.
// do a binary search on the flash - read just trip no from Page

// TBD need to check CRC?

//(could use CPU EEPROM to record last trip number & its Page no.)

static uint16_t ReadLastUsedPage(void)
{
	uint16_t sPage=0;
	uint16_t ePage=FRW_NUMGPS_PAGES-1;
	uint16_t mPage;
	uint16_t sTripNo;
	uint16_t eTripNo;
	uint16_t mTripNo;
	uint16_t readPage=(uint16_t)-1;
	
	sTripNo = FRWReadPageHeader(sPage);
	eTripNo = FRWReadPageHeader(ePage);
	
	while (true)
	{
		mPage=(sPage+ePage)/2;
		if (mPage==sPage)										//done
		{
			if (eTripNo >= sTripNo )							//which of the last two is less?
			{
				mPage=ePage;
				currentTripNo = eTripNo;
			}
			else
				currentTripNo = sTripNo;	
			
			FRWReadPage(mPage);									// as we only read header and/or previous.	

			return mPage;										//last used page
		}	
		mTripNo = FRWReadPageHeader(mPage);
		readPage = mPage;
		
		if (mTripNo >= sTripNo)
		{
			sPage=mPage;
			sTripNo = mTripNo;
		}
		else
		{
			ePage=mPage;
			eTripNo = mTripNo;
		}
	}
}

//--------------------------------------------------------------------------
// return 1st page of specified trip. look from specified start point.
// need to lookback from current position, will have to go past the other entries of the 
// wanted trip to find the fitst (hence the logic below)
// eg, if looking for trip 2 below, and at start point X, need to skip over other members of 2
// to locate the first entry. Will endup with the first that isn't a member of trip 2
// (ie last of trip 1). Need then to identify that we've gone off the end of trip 2 and
// return the current location 'Y' +1.
// trips: 1 1 1 1 1 2 2 2 2 2 3 3 3 3
//                Y            <--X 

//startingPage is a hint. The routine will start at this point. It searches back from this
//point. will wrap round at 0 then search from highest
//note: if the entire flash contains the same trip then this routine will fail.
//(code will need to be modified to look at header details to determine the 1st entry) - 
//or the raw data uploaded to teh PC and the start/end determined for there.

int16_t FRWFindTrip1stPage(uint16_t wantedTrip, uint16_t startingPage)
{
	int16_t page = startingPage;
	bool found =false;
	
	
	//search back from specified page.
	
	do
	{
	    FRWReadPageHeader(page);
		if (flashGPSReadBuffer->header.tripNo == wantedTrip)	//found
		{
			found =true;										//remember found, cont till past 1st
		}
		else													//(now)not found
		{
			if (found)											//just gone beyond 1st entry	
			{
				page = (page+1)% FRW_NUMGPS_PAGES;				//forward 1
				FRWReadPage(page);								//as we read before (&just header)
				tripStartPage = page;							//remeber this
				return page;
			}
		}	
		if (--page <0) 											//go backwards
			page = FRW_NUMGPS_PAGES-1;
	}
	while (page != startingPage);
	
	tripStartPage=-1;											//not found
	return -1;
}
//--------------------------------------------------------------------------
// read the entire page into the apps buffer
// to do - need to check the CRC to prove the data is valid
// returns trip number, 0 for invalid trip no, as trips start at 1

uint16_t FRWReadPage(uint16_t pageNo)
{
	DFPageToBuffer(pageNo,FRW_INBUF);
	DFReadBuffer(FRW_INBUF,0,sizeof(flashGenericReadBuffer),(char*)flashGPSReadBuffer);
	lastReadPage = pageNo;

	//trip no 0 and -1 are same, ie non valid, so return 0.
	
	if ((int16_t)flashGPSReadBuffer->header.tripNo == -1)		//flash not initalised?
		return 0;
		
	//update reset cause when reading header
	//this var identifies the reason for the reset and its stored in the first commit.
	
	recordedResetCause = flashGPSReadBuffer->header.resetCause;
	
	return flashGPSReadBuffer->header.tripNo;
}
//--------------------------------------------------------------------------
// read the page header ONLY into the apps buffer
// to do - need to check the CRC to prove the data is valid
// returns trip number, 0 for invalid trip no, as trips start at 1

uint16_t FRWReadPageHeader(uint16_t pageNo)
{
	DFPageToBuffer(pageNo,FRW_INBUF);
	DFReadBuffer(FRW_INBUF,0,sizeof(flashGPSReadBuffer->header),(char*)flashGPSReadBuffer);
	lastReadPage = pageNo;
	
	//trip no 0 and -1 are same, ie non valid, so return 0.
	
	if ((int16_t)flashGPSReadBuffer->header.tripNo == -1)		//flash not initalised?
		return 0;

	recordedResetCause = flashGPSReadBuffer->header.resetCause;
	
	return flashGPSReadBuffer->header.tripNo;
}

//--------------------------------------------------------------------------
bool FRWCopyReadBufferHeader(GPSTripEntry* loc)
{
	
	loc->timeInTenthsSecs = flashGPSReadBuffer->header.timeInTenthsSecs;
	loc->UTCDate  		  = flashGPSReadBuffer->header.UTCDate;
	loc->latitude 		  = flashGPSReadBuffer->header.latitude; 
	loc->longitude		  = flashGPSReadBuffer->header.longitude;
	loc->antHeight	      = flashGPSReadBuffer->header.antHeight;
	loc->info.all 		  = flashGPSReadBuffer->header.info.all;
	
	return true;
	
}
//--------------------------------------------------------------------------
//get & add the entry values (deltas) to the loc.
//As such, this function is called itteratively through the read buffer.
//loc must have started with the page header, as they contain the absolute values
//for the page, then called for each entry to increment loc by the deltas.

bool FRWCopyReadBufferEntry(GPSTripEntry* loc, uint8_t entryNo)
{
	if (entryNo >= FRW_NUMSLOTS)
		return false;
		
	if (flashGPSReadBuffer->entry[entryNo].dTime == (uint16_t)-1)
		return false;

	loc->timeInTenthsSecs += flashGPSReadBuffer->entry[entryNo].dTime;
	loc->latitude 		  += flashGPSReadBuffer->entry[entryNo].dLat; 
	loc->longitude		  += flashGPSReadBuffer->entry[entryNo].dLong;
	loc->info.all 		   = flashGPSReadBuffer->entry[entryNo].info.all;	
	
	//height is only stored in header, so just copy that!
	//(so height same for all entries in same Flash slot)
	//commented out as already in struct, as header will have read it.
	
	//loc->antHeight         = flashGPSReadBuffer->header.antHeight;
	
	return true;	
}
//--------------------------------------------------------------------------

FRWGPSIOBuffer *const GetFlashGPSReadBufferPtr(void)
{
	return flashGPSReadBuffer;
}
//--------------------------------------------------------------------------
// generate crc from first byte after CRC to end of buffer
// flashIOBuffer ( has been defined so that the last data value will run to a 16bit boundary

static uint16_t GenerateCRC(void)
{

	uint16_t mCRCaccum = 0xffff;
	uint16_t *p;
	
	p=(uint16_t*)&flashGPSWriteBuffer->header.pageCRC +1; //1st value after CRC in header
	
	while ( p < ((uint16_t*)flashGPSWriteBuffer+sizeof(FRWGPSIOBuffer)/sizeof(uint16_t)))
	{
		mCRCaccum = mCRCaccum ^ *p;
		for(int j=0; j<8; j++)
		{
 			if (mCRCaccum & 0x0001)
				mCRCaccum = (mCRCaccum >> 1) ^  0xa001;
			else
				mCRCaccum >>= 1;
		}
		p++;
	}	
	return mCRCaccum;
}

//--------------------------------------------------------------------------
//--------------------------------------------------------------------------

void FRWTest(void)
{
/*
	int i;
	char *p;
	
	
	// fill buffer with crap
	
	p = (char*)&flashIOBuffer;
	for (i=0;i<sizeof(flashIOBuffer);i++)
		*p++=1;

	// read flash
	
	DFPageToBuffer(0,FRW_OUTBUF);											//f buffer from flash
	DFBufferReadStr(FRW_OUTBUF,0,sizeof(flashIOBuffer),(char*)&flashIOBuffer);	//fbuffer to me

	// fill buffer with what to commit
	// and write to flash
	
	p = (char*)&flashIOBuffer;
	for (i=0;i<sizeof(flashIOBuffer);i++)
		*p++=2;
	
	DFBufferWriteStr(FRW_OUTBUF,0,sizeof(flashIOBuffer),(char*)&flashIOBuffer);
	DFBufferToPage(FRW_OUTBUF,0);

	// fill buffer with crap
	// and set flash buffer to this 
	// (this ensures that we see if the buffer is read from flash)
	
	p = (char*)&flashIOBuffer;
	for (i=0;i<sizeof(flashIOBuffer);i++)
		*p++=3;

	DFBufferWriteStr(FRW_OUTBUF,0,sizeof(flashIOBuffer),(char*)&flashIOBuffer);	//reset fbuffer
	
	// read the flash

	DFPageToBuffer(0,FRW_OUTBUF);												//fbuffer from flash
	DFBufferReadStr(FRW_OUTBUF,0,sizeof(flashIOBuffer),(char*)&flashIOBuffer);	//fbuffer to me
*/
}
