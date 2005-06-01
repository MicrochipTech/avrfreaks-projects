/*!\file test.c
 *This file contains testroutines, comment them out when compiling the 
 *final realization.
 */
 
#define NEED_AUTH
#define NEED_USART
#define TEST_AUTHENTICATE
#include "define.h"


#ifdef TEST_RANDOM2
/*!This routine is used to control the function of the random number 
 *generator routine.  128-bit numbers are continuously generated and
 *sent down to the RS232-port.
 */
void testRandom2(void){
	while(1){
		byte* p=getRandom();
		for(byte i=0;i<16;i++)send_USART(p[i]);
	}
}//testRandom2
#endif


#ifdef TEST_HMAC
/*!This routine tests the hmac algorithm.  The HMAC is calculated of a
 *55-byte buffer filled with zeros.  The 160-bit HMAC is sent down to 
 *the RS232 buffer.  Calculate the HMAC yourself using Java or another
 *language and compare the HMACs.
 */
void testHmac(void){
	byte text[55];
	for(byte i=0;i<55;i++)text[i]=0;	
	byte* p=getHmac(text);
	for(byte i=0;i<20;i++)send_USART(p[i]);
}//testHmac
#endif


#ifdef TEST_AUTHENTICATE
/*!You can test the authentication routine with this routine.  The 
 *protocol is instantiated with a source address of {0,1,2,2}, a 
 *destination address of {0,1,2,3} and a message of {0,1,2,3, ... , 17,
 *18,19}
 *The returned error codes will be shown on PORTB.
 */
void testAuthenticate(void){
	byte me[4]={0,1,2,2};
	byte other[4]={0,1,2,3};
	byte message[19]={
		0,1,2,3,4,5,6,7,8,
		9,10,11,12,13,14,15,16,17,18};
	DDRB=0xFF;
	PORTB=authenticate(me, other, message);
	//lock MCU
	while(1);
}//testAuthenticate
#endif
