/*! \file simpleRead.c
 *Functions implementing the low-level read functions of the NeoRS232-
 *protocol.
 */
#define NEED_USART
#define NEED_TIMER2
#include "define.h"
#include "protocol.h"


/*********************************************************************
Local functions
**********************************************************************/
static void clearBuffer(byte* buffer, byte lengte);
static void dataAvailable(byte dataIn);

/*!Higher OSI-layer function that checks the validity of the received 
 *data.
 *\param buffer BUffer containing data
 *\param teller number of bytes in buffer
 */
extern void bewerkDatagram(byte* buffer, byte teller);


/*********************************************************************
Local variables
**********************************************************************/
static byte RS232_RX_Buffer[RS232LENGTH];//!<USART receive buffer

/*! This is the ISR-routine that will be executed when data arrives at
 *the USART.  It calls one of the NeoRS232-routines that takes further
 *care of the received data.
 */
SIGNAL(SIG_UART_RECV){
	dataAvailable(UDR);
}//USART receive complete

/*! This function implements the low-level NeoRS232-routines.  The
 *NeoRS232-protocol is an idle RQ stop and wait protocol.  This 
 *means that the sender can't continue to send data unless an acknowledge
 *is received.  Two types of frames exist: an acknowledge frame and a
 *data frame.  The acknowledge frame simply consists of the \sa ACK0_FLAG.
 *A data frame always starts with a \sa LEADING_FLAG followed by some data
 *and is appended by a \sa TRAILING_FLAG.  The data itself is bytestuffed
 *with a \sa DLE_FLAG.  The data consists of the data to send, appended by
 *a CRC16-checksum.
 *This routine takes care of the above mentioned flags and sends data
 *to \sa bewerkDatagram when a complete packet has arrived.
 *\param dataIn byte received by the ISR-routine
 */
static void dataAvailable(byte dataIn){
	static byte dle_received=FALSE;
	static byte teller=0;
	
	switch(dataIn){
		case LEADING_FLAG:
			if(!dle_received){
				clearBuffer(RS232_RX_Buffer, RS232LENGTH);
				teller=0;
			}
			else {
				RS232_RX_Buffer[teller++]=dataIn;
				dle_received=FALSE;
			}
			break;
		case TRAILING_FLAG:
	 		if(!dle_received){
	 			bewerkDatagram(RS232_RX_Buffer,teller);
	 		}
	 		else {
				RS232_RX_Buffer[teller++]=dataIn;
	 			dle_received=FALSE;
	 		}
	 		break;
	 	case ACK0_FLAG:
	 		if(!dle_received){	 		
				stopTimer2();
	 		}
	 		else {
				RS232_RX_Buffer[teller++]=dataIn;
	 			dle_received=FALSE;
	 		}
	 		break;
	 	case DLE_FLAG:
	 		if(!dle_received)dle_received=TRUE;
	 		else {
				RS232_RX_Buffer[teller++]=dataIn;
	 			dle_received=FALSE;
	 		}
	 		break;
	 	default:
			RS232_RX_Buffer[teller++]=dataIn;
	 		dle_received=FALSE;
	}//switch
}//dataAvailable

/*!Function that fills a buffer with zeros
 *\param buffer buffer that must be zeroed
 *\param lengte number of zeros that will be filled in
 */
static void clearBuffer(byte* buffer, byte lengte){
	for(byte i=0;i<lengte;i++)buffer[i]=0x00;
}//clearBuffer



