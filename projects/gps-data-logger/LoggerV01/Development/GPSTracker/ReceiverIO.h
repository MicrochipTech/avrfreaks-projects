//NMEA Message after conversion

#ifndef INCLUDE_ReceiverIO
#define INCLUDE_ReceiverIO

#include "projectCommon.h"


#define MaxMessageLen 80			// Max NMEA message len (more than enough)
#define NMEAMessageTypeLen 5		// length of message type , eg GPGGA
#define LF '\n'
#define CR '\r'


// Data acquired from the collection of GPS messages received
// elements will be completed from various messages. (no one message will contain
// all the data to fill the structure)
// Once completed, this data is passed to the Flash IO routined for storage


typedef struct INFOBITS  
{   
   uint8_t status:1;     			// from RMC message (field 2 ), 0 is 'V' (warn), 1 is 'A'
   uint8_t fix   :1;				// from GGA message (field 6 )  0 is no fix
   uint8_t mode  :2;				// from RMC message (field 12)	0 is 'N' (invalid)
   uint8_t entryDeltaOverflow:1;	// indicates if recorded delta too big for storage
} INFOBITS;

typedef union GPSINFO
{
	uint8_t  all;
	INFOBITS bits;
} GPSINFO;

//data structure contains info obtained from GPS receiver module
//the long/lat are stored as scaled integers. 
//eg 18030.00000 is  180 deg 30 min (from GPS) & stored as 180.5000000 without the dot
//Southen and Western are stored as negative numbers

typedef struct GPSLocation
{
  // the following bits we want to store in flash
  
  uint32_t timeInTenthsSecs;	
  uint32_t UTCDate;				// DDMMYY eg 18Feb04 180204
  int32_t  latitude;			// in ddd.dddddd scaled in format (see above & below)
  int32_t  longitude;			// in ddd.dddddd (ie mins converted to fractions)
  int32_t  antHeight;			// in 10ths meter.
  //int16_t  geoHeight;
  GPSINFO  info;
 // uint8_t  checksum;
  
  //data recorded for user interface & not for writing to flash
  //This data will not be available when reading the data back from flash.
  
  uint32_t UTCTime;		//UI thread use, of form hhmmss.ss
  int16_t  course;		//10ths degree (0.359.9)
  uint16_t speed;		//10ths knot (oh yuk!)
  uint32_t lastFixTime;	//UI thread use, in 10ths seconds


}GPSLocation;


// lat/long are held as a scaled 32 signed number
// the scaling is LatLongScaleFactor. and is always
// LatLongDigits digits.
// LatLongFractionDigits is digits after fraction.
// number is of form ddd.dddddd, in degrees.
// longitude 0 to +/-180.000000
// latitude  0 to +/-180.000000

#define LatLongScaleFactor 		   1000000L
#define LatLongFractionDigits      6	

bool RIOReadLoc(GPSLocation* loc);
bool DoChecksum(char *p);


#endif //INCLUDE_ReceiverIO
