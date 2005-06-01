/*!\file hmac.c
 *This file contains functions to implement the HMAC calculation. A hmac
 *provides authentication of data, based on a shared secret key.
 *Authentication is a way to make sure the users really are who they say
 *they are. So if data with an HMAC comes in.  You compute the HMAC 
 *yourself using the secret key you share with the other party.  If this
 *computed HMAC is exactly the same as the received HMAC, then you are
 *sure that the data came from the other communicating party.  Offcourse
 *you must be sure that only you and the other communicating party has 
 *the secret key. 
 */
#include <avr/eeprom.h>
#define NEED_HASH//!<HMAC is based on hashing, so needs hashing routines.
#include "define.h"


/*********************************************************************
Local constants
**********************************************************************/
#define IPAD 0x36		//!<defined in RFC2104                                                                                                                                                                                  
#define OPAD 0x5C		//!<defined in RFC2104

/*********************************************************************
Local variables
**********************************************************************/
/*!512-bit secret key for the authentication routines, copied to 
 *EEPROM when burning program in flash.
 *Array stored in EEPROM, can only be reached using EEPROM reading
 *routines
 */
static const byte key[64] __attribute__ ((section (".eeprom")))={
	0x02,0xe4,0x80,0xb2,0x05,0x5c,0x96,0x31,
	0x85,0x87,0xff,0x8a,0x37,0x49,0x4f,0x6d,
	0xf2,0x22,0x6c,0x65,0xd5,0x16,0x0d,0x7c,
	0x4e,0x82,0x8c,0xc5,0x1d,0x32,0xc3,0xfd,
	0xbd,0x5d,0xe9,0xdb,0xfc,0x57,0x5e,0x41,
	0x50,0x41,0x3b,0xd4,0xd9,0xee,0x61,0xee,
	0xf7,0xa8,0xfe,0x15,0x03,0x02,0x1c,0xf6,
	0x07,0x9f,0xd4,0x37,0x40,0x96,0x5b,0x2f};

/*!Calculates an HMAC (RFC2104) of 55-byte data and a 512-bit key, a
 *160-bit (20-byte) HMAC is returned
 *\param text The 55-byte data of which the HMAC will be calculated.
 */
byte* getHmac(byte text[55]){
	byte Wt[64];
	
	resetHash();
	for(byte i=0;i<64;i++){
		eeprom_busy_wait();
		Wt[i]=eeprom_read_byte(key+i)^IPAD;
	}
	calcHash(Wt);
	for(byte i=0;i<55;i++)Wt[i]=text[i];
	Wt[55]=0x80;
	for(byte i=56;i<62;i++)Wt[i]=0x00;
	Wt[62]=0x03;
	Wt[63]=0xB8;
	byte* hash=calcHash(Wt);
	for(byte i=0;i<20;i++)text[i]=hash[i];
	resetHash();
	for(byte i=0;i<64;i++){
		eeprom_busy_wait();
		Wt[i]=eeprom_read_byte(key+i)^OPAD;
	}
	calcHash(Wt);
	for(byte i=0;i<20;i++)Wt[i]=text[i];
	Wt[20]=0x80;
	for(byte i=21;i<62;i++)Wt[i]=0x00;
	Wt[62]=0x02;
	Wt[63]=0xA0;
	return calcHash(Wt);
}//getHmac