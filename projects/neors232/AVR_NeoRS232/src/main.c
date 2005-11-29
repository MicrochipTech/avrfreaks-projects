/*!\mainpage AVR_NeoRS232
 * This project implements an Idle RQ Stop & Wait protocol.  This is a
 * complicated manner to say that a transmitter must wait for an answer
 * of the receiver before it can send something again.
 * The receiver checks the incoming frame using the CRC16-checksum.  If
 * the checksum is correct, then an acknowledge is sent back.  In the
 * meanwhile, the sender had started a timer after sending his frame.  It
 * waits for the acknowledge of the receiver.  If that acknowledge doesn't
 * arrive in five seconds, then it tries to resend the data.
 * \n
 * A frame consists of a startflag (0xAA), the data and a stop flag (0xFF).
 * If a 0xFF (or some other flag character) occurs in the data, the receiver
 * would falsely think that the end of the frame has been reached.  To
 * prevent this, bytestuffing is used.  Before every flag character in the
 * data we send the DLE-flag.
 * \n
 * When the receiver sees the DLE-flag, it knows that the next character 
 * must be considered as a databyte and not as a flag byte.
 *
 * \n
 * Oh, remember: to use this package, you must have some version of 
 * javacomm installed.  It's available from the 
 * <a href="http://java.sun.com">Java</a> website.
 *
 * \image html LieBtrau_anim.gif
 * \author 	LieBtrau
 * \author	locomotieff@tiscali.be
 * \version	version 1.1
 *
 */
 
 
 /*!\file main.c
 *The files in this project define a RS232-protocol.
 */


#include "define.h"


/*!The framer contains pointers to work with the incoming data.*/
FRAMER framer;
/*!Buffer that will contain the incoming frames*/
uint8_t frameData[FRAMESIZE];


//!Main function
int main(void){
	init_USART(115200,TRUE);
	receiveInit(&framer);
	while(1){
		receiveRun(&framer);
	}
	return 0;
}//main


/*! Initialize the frame so that it is ready to receive data
 *\param pFr Pointer to the frame that will be initialized.
 */
void receiveInit(FRAMER* pFr){
	pFr->pData=pFr->pBegin=frameData;
	pFr->pEnd=pFr->pBegin+FRAMESIZE-1;
}//receiveInit


/*!Receive run routine: checks if data has come in and act accordingly.
 *\param pFramer Check if data has come in on this framer
 */
void receiveRun(FRAMER* pFramer){

	switch(FramerReceive(pFramer)){
		case MSG_RX_COMPLETE:
			if(stripCRC(pFramer)){
				//Frame goed ontvangen
				writeString(0,0,ACK_FRAME);
				//Do something with pFramer
				writeFrame(
					pFramer->pBegin,pFramer->pData-pFramer->pBegin);
			}
			break;
		case MSG_DECODING:
		case MSG_NO_INPUT:
			break;
	}
}//receiveRun

