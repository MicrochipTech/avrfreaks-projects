
#ifndef INCLUDE_GPSSerialIO
#define INCLUDE_GPSSerialIO

#include "projectCommon.h"

typedef enum enumReadMessage {eNone, eGSA,eGSV, eRMC, eGGA} eReadMessage;

void  		 GPSInitSerialIO(void);
int   		 GPSSIOPutChar(char d);
void  		 GPSIOReleaseNMEABuffer(void);
void  		 GPSIOWantSatelliteInfo(bool yes);
char *		 GPSIOGetBufferPtr(eReadMessage bufferId);
eReadMessage GPSSIOGetNMEAMessage(void);


extern bool gSendGPSToSerial;

#endif //INCLUDE_GPSSerialIO