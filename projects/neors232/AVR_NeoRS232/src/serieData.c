/*!\file serieData.c
 *Higher OSI-level function for sending frames.
 */

#include "define.h"

/*! Function that sends a frame.  First a CRC16 is appended.  Then the
 * frame is sent.  After that, a timeout timer is initialized.  The
 * acknowledge must come in before the timeout expires.
 *\param buffer Buffer containing the data to send.
 *\param length Number of bytes that must be sent.
 */
void writeFrame(uint8_t* buffer, uint8_t length){
	appendCRC(buffer, length);
	writeString(buffer, length+2, INFO_FRAME);
	initTimer2();
}//writeFrame




