#ifndef INCLUDE_GPSFlashRW
#define INCLUDE_GPSFlashRW

#include <inttypes.h>
#include "ReceiverIO.h"

//structure for read back from flash
//(this structure is filled from the data read from flash - this is not the structure of
//the data in flash)
//Only the header entry in each flash page contains the data verbatim, the 
//rest of the entries are held as detas from the header. When the entry is read,
//the maths is done to calculate the values needded to fill this structure.
//The caller therefore need never be aware of this 'compressed' storage format.

typedef struct GPSTripEntry
{
  //data returned from flash
  
  uint32_t timeInTenthsSecs;	
  uint32_t UTCDate;				//eg 250304 is 25 Mar 04
  int32_t  latitude;			// in ddd.dddddd (ie mins converted to fractions)
  int32_t  longitude;			// in ddd.dddddd (ie mins converted to fractions)
  int32_t  antHeight;			// in 10ths meter.
  //int16_t  geoHeight;
  GPSINFO  info;

}GPSTripEntry;


uint16_t FRWStateReset(void);	
uint16_t FRWGetCurrentTripNo(void);	
int16_t  FRWGetLastReadPageNo(void);
int16_t  FRWGetTripStartPageNo(void);
uint16_t FRWGetNextWritePage(void);
void     FRWStartNewTrip(void);			
void 	 FRWAddGPSEntry(GPSLocation* loc);
bool 	 FRWCommit(void);

bool     FRWCopyReadBufferHeader(GPSTripEntry* loc);
bool     FRWCopyReadBufferEntry(GPSTripEntry* loc, uint8_t sEntryNo);
int8_t 	 FRWGetCurrentEntryIndex(void);
uint16_t FRWGetNumEntriesInCurrentTrip (void);

void 	 FRWPrintFlash(int s, int e);
void 	 FRWDumpFlashRAW(int s, int e);   

// access to low level functions provided for use by the utils

uint16_t FRWReadPage(uint16_t pageNo);
uint16_t FRWReadPageHeader(uint16_t pageNo);
int16_t  FRWFindTrip1stPage(uint16_t wantedTrip, uint16_t startPage);


//----------------private data--------------------------------


#ifdef DFPageSize264
#define FRW_NUMSLOTS 30			//n entries in a slot
#endif

#ifdef DFPageSize528
#define FRW_NUMSLOTS 60			//n entries in a slot
#endif


// Flash structures.
// the header is held only once for each block, the block then contains deltas

typedef struct FRWDataHeader
{
  uint16_t tripNo;					//id start of trip (what trip this block is a member of)
  uint16_t pageCRC;	    			//CRC of this page (is this block valid?)
  uint32_t timeInTenthsSecs;		//this is in 10th seconds of the day.
  uint32_t UTCDate;					//eg 250304 is 25 Mar 04
  int32_t  latitude;				//scaled integer. eg 18030.00000  180 deg 30 min (from GPS)
  int32_t  longitude;			 	//stored as 180.5000000 without the dot
  int32_t  antHeight;			    // in 10ths meter.
  //int16_t  geoHeight;
  //uint8_t  speed;
  GPSINFO  info;
  uint8_t  resetCause;				// first commit after a reset, ids the reason.
  
} FRWDataHeader;


// each entry in the page only holds the time,lat,long delta.
// the heights aren't recorded. This means that the height
// is only recorded only once per page. (this is ok, as height
// wont change much over 1 min, and doesn't add value).
// This restriction is based on the size of the page. The array stored
// is either 30/60 sec (depend on memory size).

typedef struct FRWDataEntry
{
	uint16_t  dTime;		//delta time from last in 10th seconds!!!
	int16_t   dLat;		    //delta latitude from last
	int16_t   dLong;		//delta longitude from last
	GPSINFO   info;
	
}FRWDataEntry;

//The flash record. This structure is sent to and from flash
//data must fit in one page

typedef struct FRWGPSIOBuffer
{
	FRWDataHeader header;
	FRWDataEntry  entry[FRW_NUMSLOTS];
	uint8_t _dummy;						//ensure it goes to next 16bit boundary for CRC
	
}FRWGPSIOBuffer;


FRWGPSIOBuffer *const GetFlashGPSReadBufferPtr(void);

#endif //INCLUDE_GPSFlashRW
