/*!\file simpleWrite.c
 *Functions implementing the lowlevel write functions of the 
 *NeoRS232-protocol.
 */
#define NEED_USART
#include "define.h"
#include "protocol.h"

/*********************************************************************
Local functions
**********************************************************************/
static BOOL vlagKarakterInData(byte c);

/*! This function receives a buffer with a length "lengte".  This
 *function converts the buffer to the NeoRS232-protocol by inserting 
 *a startflag, bytestuffing flags and also a trailing flag.
 *\param buffer Buffer containing the data to send
 *\param lengte Number of bytes out of buffer to be sent
 *\param typeFrame Identifier indicating what type of frame must be sent
 *	(INFO_FRAME or ACK_FRAME)
 */
void schrijfString(byte* buffer, byte lengte, byte typeFrame){
		switch(typeFrame){
			case INFO_FRAME:
				//Leading flag
				send_USART(LEADING_FLAG);
				//Data	
				for(byte i=0;i<lengte;i++){		
       				if(vlagKarakterInData(buffer[i]))send_USART(DLE_FLAG);
       				send_USART(buffer[i]);
       			}
				//Trailing flag
	        	send_USART(TRAILING_FLAG);
	        	break;		        	
	        case ACK_FRAME:
	        	send_USART(ACK0_FLAG);
	        	break;		        	
	   	}    	
}//schrijfString

/*!This routine checks whether the argument is a flag character yes or no.
 *\param c Byte that will be checked to see if it's a flag character.
 */
static BOOL vlagKarakterInData(byte c){
	if(c==LEADING_FLAG||
		c==TRAILING_FLAG||
		c==DLE_FLAG||
		c==ACK0_FLAG)return TRUE;
	else return FALSE;
}//vlagKarakterInData

