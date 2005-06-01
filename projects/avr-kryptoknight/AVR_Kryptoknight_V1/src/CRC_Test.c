/*!\file CRC_Test.c
 *The protocol makes use of checksums, these functions implement these.
 */

#include "define.h"
#include <avr/crc16.h>

/*********************************************************************
Local functions
**********************************************************************/
static uint16_t get_crc(byte* buffer, byte length);


/*!This routine strips the two CRC-bytes off at the end of the buffer. 
 *The LSB CRC-byte is followed by the MSB CRC-byte (little endian order).
 *The routine calculates the CRC of the remaining bufferdata.  When the
 *calculated and stripped CRC are equal, TRUE is returned, else FALSE is
 *returned.
 *\param buffer A buffer with CRC attached at the end (little endian)
 *\param length Total number of bytes in the buffer, CRC included
 *\return TRUE when CRC is correct, otherwise FALSE
 */
BOOL stripCRC(byte* buffer, byte length){
		word crcOntvangen=0, crcBerekend;
		
		crcBerekend=get_crc(buffer,length-2);
		
		//read CRC from received data (little endian order)
		crcOntvangen=(buffer[length-1]<<8)|buffer[length-2];
		
		if(crcOntvangen!=crcBerekend)return FALSE;
		else return TRUE;
}//stripCRC


/*!This function calculates the CRC of the bytes in the buffer, up to 
 *length bytes.  Afterwards the CRC is appended to the end of the buffer.
 *As a result, the buffer is two bytes longer than before the call.
 *\param buffer contains the data of which the CRC is calculated.  Upon 
 *return the buffer will be two bytes longer, because the CRC is 
 *appended.
 *\param length number of bytes in the buffer
 */
void appendCRC(byte* buffer, byte length){
	word crcValue=get_crc(buffer,length);
	buffer[length]=(byte)(crcValue&0xFF);
	buffer[length+1]=(byte)((crcValue>>8)&0xFF);
}//appendCRC


/*!This function calculates the CRC16 of a buffer of length "length".
 *\param buffer contains the data of which the CRC is calculated
 *\param length number of bytes in the buffer
 *\return the 16-bit CRC value
 */
static uint16_t get_crc(byte* buffer, byte length){
	uint16_t crc=0xFFFF;
	for(byte i=0;i<length;i++)crc=_crc_ccitt_update(crc,buffer[i]);
	return crc;
}//get_crc



