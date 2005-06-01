/*!\file serieData.c
 *Higher OSI-level function of the NeoRS232-protocol for writing and
 *receiving the data.
 */
 
#define NEED_CRC
#define NEED_TIMER2
#define NEED_NEORS232
#include "define.h"
#include "protocol.h"

//simpleWrite.c
extern void schrijfString(byte* buffer, byte lengte, byte typeFrame);

/*! A user wanting to use the NeoRS232-protocol must call this function
 *to send data.  This upper OSI-layer function sends the data down to
 *the lower OSI-levels.  The functions send the data then to the RS232.
 *\param buffer Buffer is a buffer containing the data to send
 *\param grootte The number of bytes to send
 */
void writeData(byte* buffer, byte grootte){
	appendCRC(buffer, grootte);
	schrijfString(buffer, grootte+2, INFO_FRAME);
	initTimer2();
}//writeData

/*! When data comes in.  This function checks if the checksum is correct.
 *If it is, then the dataRSReady routine is called.  The user must 
 *implement the dataRXReady-routine him-/herself.  The user should never
 *call the bewerkDatagram-routine.  This function is called by the lower
 *OSI-level when data comes in.
 *\param buffer A buffer containing the received data
 *\param teller The number of bytes that is received
 */
void bewerkDatagram(byte* buffer, byte teller){
	 	if(stripCRC(buffer, teller)){
	 		//Frame goed ontvangen
	 		schrijfString(0,0,ACK_FRAME);
	 		dataRXready(buffer,teller-2);
	 	}
	 	//Frame niet goed ontvangen
}//bewerkDatagram



