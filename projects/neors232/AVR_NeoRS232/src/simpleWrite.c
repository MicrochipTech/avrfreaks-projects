/*!\file simpleWrite.c
 *Functions implementing the protocol: writing data
 */


#include "define.h"


BOOL flagCharacterInData(uint8_t c);


/*! Write a databuffer to the serial port.  No CRC will be appended.
 * This function adds the flags for the protocol and performs the 
 * bytestuffing.  Finally it sends the data to the UART.
 *\param buffer Buffer containing the data.
 *\param length Length of the databuffer
 *\param typeFrame Type of frame (info frame or acknowledge frame)
 */
void writeString(uint8_t* buffer, uint8_t length, FRAME_TYPE typeFrame){
		uint8_t i=0;
		switch(typeFrame){
			case INFO_FRAME:
				//Leading flag
				write_USART_B(LEADING_FLAG);
				/*Data:	
				 * if a databyte is equal to a flag character, then prepend
				 * a DLE_FLAG to it.
				 */
				for(i=0;i<length;i++){		
       				if(flagCharacterInData(buffer[i]))write_USART_B(DLE_FLAG);
       				write_USART_B(buffer[i]);
       			}
				//Trailing flag
	        	write_USART_B(TRAILING_FLAG);
	        	break;		        	
	        case ACK_FRAME:
	        	write_USART_B(ACK0_FLAG);
	        	break;		        	
	   	}    	
}//writeString


/*!Check if the character is a flag character.
 *\param c The character
 *\return TRUE when c is a flag character else FALSE.
 */
BOOL flagCharacterInData(uint8_t c){
	if(c==LEADING_FLAG||
		c==TRAILING_FLAG||
		c==DLE_FLAG||
		c==ACK0_FLAG)return TRUE;
	else return FALSE;
}//flagCharacterInData

