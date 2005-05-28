/*! \file authenticate.c
 *Routines that implement the IBM-Kryptoknight authentication protocol.
 */
#define NEED_RANDOM
#define NEED_HMAC
#define NEED_NEORS232
#include "define.h"


/*********************************************************************
Local variables
**********************************************************************/
static byte* p_RX=0;	//!<will point to RX-buffer after data has been received
extern volatile FLAGS flags;//!< contains some flags (see define.h)

/*!This function contains the authentication algorithm according to 
 *the IBM-Kryptoknight algorithm.  This function can only perform the 
 *function of ALICE, i.e.  this function always starts the
 *authentication procedure. 
 *The procedure consists of three flows or messages.  Every flow
 *contains the address of the sender, followed by the address of the
 *receiver.  In the first flow, a nonce (=large random number) and a
 *MESSAGE are attached.  This message, generated by our ALICE is sent
 *to BOB.  In the second flow, this function receives a
 *packet from BOB and checks the contents.  This flow 2 package
 *contains the two addresses, followed by a nonce nB generated by BOB,
 *followed by an HMAC. This HMAC is calculated over the nonce nA, the 
 *MESSAGE, nB and the and the BOB address.  If the HMAC is correct,
 *then generate the third flow package.  This package is again 
 *consituted of the two addresses, followed by an HMAC.  Now the HMAC
 *is calculated over nA and nB
 *\param me my address consisting of four bytes (e.g. IP-number)
 *\param other address of the other party, also four bytes
 *\param message message to send, consisting of nineteen bytes
 *\return an error code indicating the status of this function
 */
AUTH_ERROR authenticate(byte me[4], byte other[4], byte message[19]){
	byte* p;
	byte buffer[55];//general purpose buffer
	byte nA[16], nB[16];
	
	
	//FLOW1 -- send (me, other, nA, message) --
	//Fill source and destination address in send buffer
	for(byte i=0;i<4;i++){
		buffer[i]=me[i];
		buffer[i+4]=other[i];
	}
	//Generate nA
	getRandom(nA);
	//Fill nA and message in send buffer
	for(byte i=0;i<16;i++)buffer[i+8]=nA[i];
	for(byte i=0;i<19;i++)buffer[i+24]=message[i];
	//send FLOW1 package
	writeData(buffer,43);
	
	//reset communication error-flag
	flags.commError=0;
	//wait for answer
	while(!flags.waitflow2 && !flags.commError);
	//check for occurance of communication errors
	if(flags.commError)return COMM_ERROR;
	//reset wait-flag
	flags.waitflow2=0;
	
	
	//FLOW2 -- receive (other, me, Nb, HMAC(nA, message, nA, other)) --
	//check destination and source address
	for(byte i=0;i<4;i++){
		if(p_RX[i]!=other[i])return FROM_ERROR;
		if(p_RX[i+4]!=me[i])return TO_ERROR;
	}
	//fill buffer for checking HMAC received in FLOW 2
	for(byte i=0;i<16;i++)buffer[i]=nA[i];
	for(byte i=0;i<19;i++)buffer[i+16]=message[i];
	for(byte i=0;i<16;i++)nB[i]=buffer[i+35]=p_RX[i+8];
	for(byte i=0;i<4;i++)buffer[i+51]=other[i];
	//calculate HMAC
	p=getHmac(buffer);
	//compare received and calculated HMAC
	for(byte i=0;i<20;i++)if(p[i]!=p_RX[i+24])return HMAC_ERROR;

	
	//FLOW 3 -- send (me, other, HMAC(nA, nB)) -- 
	//fill buffer for calculating HMAC in flow 3
	for(byte i=0;i<16;i++){
		buffer[i]=nA[i];
		buffer[i+16]=nB[i];
	}
	//Pad the end of the buffer with zeros
	for(byte i=32;i<55;i++)buffer[i]=0;
	//Calculate HMAC
	p=getHmac(buffer);
	//Fill sendbuffer with source and destination address
	for(byte i=0;i<4;i++){
		buffer[i]=me[i];
		buffer[i+4]=other[i];
	}
	//fill HMAC in send buffer
	for(byte i=0;i<20;i++)buffer[i+8]=p[i];
	//send buffer to RS232
	writeData(buffer,28);
	return ALL_OK;
}//authenticate

/*!Function called by the NeoRS232-routines that take care of the
 *lower level RS232 communications.  DataRXready sets the flag that
 *lets function authenticate know that data has arrived.  Pointer
 *p_RX is also set to point to the received data buffer.
 *When the size of the data is wrong, then a communication error flag
 *is set.
 *\param buffer Buffer containing received data
 *\param length length of received data
 */
void dataRXready(byte* buffer,byte length){
	if(length!=44)flags.commError=1;
	else{
		p_RX=buffer;
		//FLOW2 packet of correct length has arrived
		flags.waitflow2=1;
	}
}//dataRXready

/*!Function called by NeoRS232-routines when no acknowledge is
 *received after sending a package.  This function could resend the
 *data.
 */
void retransmit(void){
}//retransmit

/*!Function called by NeoRS232-routines when no acknowledge is 
 *received after several trials.
 */
void noAnswer(void){
	/*release infinite loop in function authenticate(..) and indicate
	 *error
	 */
	flags.commError=1;
}//noAnswer