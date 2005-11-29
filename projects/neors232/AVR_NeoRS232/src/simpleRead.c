/*!\file SimpleRead.c
 *Functions implementing the protocol: reading data
 */

#include "define.h"

/*! Function that reads the received characters from the 
 *  circular buffer using a state machine.
 *  The databytes are read from the circular buffer, a check if performed 
 *  if it is a flagbyte.  If it is, then the correct action is undertaken.
 *  If it is a databyte then that databyte is added to the framebuffer.
 *  A value is returned that tells us what this function has done with the
 *  data (if there was any).  
 *  \param pFr Framer to which the data will be posted.
 *  \return state of the state machine.
 */
MSG_T FramerReceive(FRAMER* pFr){
	static uint8_t dle_received=0;
	uint8_t c='\0';
	
	if(!read_USART(&c))return MSG_NO_INPUT;
	
	switch(c){
		case LEADING_FLAG:
			if(dle_received==0){
				pFr->pData=pFr->pBegin;
			}
			else {
				*pFr->pData=c;
				pFr->pData++;
				dle_received=0;
			}
			break;
		case TRAILING_FLAG:
	 		if(dle_received==0){
	 			return MSG_RX_COMPLETE;
	 		}
	 		else {
				*pFr->pData=c;
				pFr->pData++;
	 			dle_received=0;
	 		}
	 		break;
	 	case ACK0_FLAG:
	 		if(dle_received==0){
	 			stopTimer2();
	 		}
	 		else {
				*pFr->pData=c;
				pFr->pData++;
	 			dle_received=0;
	 		}
	 		break;
	 	case DLE_FLAG:
	 		if(dle_received==0)dle_received=0xFF;
	 		else {
				*pFr->pData=c;
				pFr->pData++;
	 			dle_received=0;
	 		}
	 		break;
	 	default:
			*(pFr->pData)=c;
			(pFr->pData)++;
	 		dle_received=0;
	}//switch
	
	//Buffer mustn't overflow
	if(pFr->pData>=pFr->pEnd)pFr->pData=pFr->pEnd-1;
	return MSG_DECODING;
}//FramerReceive



