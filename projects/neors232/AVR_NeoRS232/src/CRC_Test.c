/*!\file CRC_Test.c
 *The protocol makes use of checksums, these functions implement these.
 */


#include "define.h"


uint16_t get_crc(uint8_t* buf, uint8_t length);


/*!check and strip the CRC16 of the frame.
 *\param pFramer A frame containing the received data.
 *\return TRUE when correct CRC, else return FALSE, when the CRC is correct,
 *	then the CRC will be removed from the frame.
 */
BOOL stripCRC(FRAMER* pFramer){
	uint16_t crcOntvangen=0, crcBerekend;
	uint8_t length;
	
	length= pFramer->pData - pFramer->pBegin - 2;
	crcBerekend=
		get_crc(pFramer->pBegin,length);

	//read CRC from received data (little endian volgorde)
	crcOntvangen=(*(pFramer->pData-1)<<8)|*(pFramer->pData-2);
	
	if(crcOntvangen!=crcBerekend)return FALSE;
	//Remove the CRC
	pFramer->pData-=2;
	return TRUE;
}//stripCRC


/*! Calculate a CRC16 of a buffer and append it to that buffer.
 *\param buffer databuffer of which CRC16 will be calculated and to which
 *		CRC16 will be appended.
 *\param length number of bytes in the buffer
 *\return the same buffer, with CRC-appended (little endian)
 */
void appendCRC(uint8_t* buffer, int length){
	uint16_t crcValue=get_crc(buffer,length);
	buffer[length]=(uint8_t)(crcValue&0xFF);
	buffer[length+1]=(uint8_t)((crcValue>>8)&0xFF);
}//appendCRC


/*!Get the CRC16 of a databuffer.
 *\param buf containing data of which the CRC will be calculated
 *\param length total length of the buffer
 *\return CRC16
 */
uint16_t get_crc(uint8_t* buf, uint8_t length){
	uint16_t crc=0xFFFF;
	for(uint8_t i=0;i<length;i++)
		crc=_crc_ccitt_update(crc,buf[i]);
	return crc;
}//get_crc



