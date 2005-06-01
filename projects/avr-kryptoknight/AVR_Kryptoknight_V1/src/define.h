#ifndef _DEFINE_H_
	/*! \file define.h 
	 *This file contains the declarations of several functions
	 */ 
	 
	#define _DEFINE_H_ 1/*!<to prevent multiple definitions of 
	the same constant in one file*/

	//AVR-libC include libraries
	#include <avr/signal.h>
	#include <avr/interrupt.h>
	
	typedef unsigned long	dword;//!<Unsigned 32-bit data
	typedef unsigned short 	word;//!<Unsigned 16-bit data
	typedef unsigned char	byte;//!<Unsigned 8-bit data
	
	//! Structure combining some flags with different function
	typedef struct{
		byte waittimer1:1;//!<becomes 1 when timer1 finished
		byte waitflow2:1;//!<becomes 1 when flow2 package is received
		byte commError:1;//!<becomes 1 when an communication error occurred
	} FLAGS;
	
	/*! Enumeration of possible errors returned from the authenticate
	 *function.
	 */
	typedef enum{
		ALL_OK=0,		//!<No error in authentication routine
		FROM_ERROR,		//!<Source address incorrect
		TO_ERROR,		//!<Destination address incorrect
		HMAC_ERROR,		//!<HMAC not correct
		COMM_ERROR		//!<Communication error in NeoRS232 routines
	} AUTH_ERROR;

	//!Boolean constants (in fact integers), substituting one and zero.
	typedef enum{
		FALSE=0,		//!<An constant integer that is zero.
		TRUE			//!<A constant integer that is one.
	} BOOL;
	

	//authenticate.c
	#ifdef NEED_AUTH
	//!Performs IBM-Kryptoknight authentication protocol
	AUTH_ERROR authenticate(byte me[4], byte other[4], byte message[19]);
	#endif
	
	//timer1.c
	#ifdef NEED_TIMER1
	//!Start timer1 (enable IRQ, set timeout)
	void timer1_Activate(void);
	//!Stop timer1 (disable IRQ)
	void timer1_DeActivate(void);
	#endif
	
	//usart.c
	#ifdef NEED_USART
	//!Initializes USART (enable IRQ, set baudrate)
	void init_USART(void);
	//!Low level routine that sends a character to RS232
	void send_USART(byte character);
	#endif
	
	//hash1.S
	#ifdef NEED_HASH
	/*!Routine that initializes the SHA1-digest routine
	 *must be called before first calcHash-call
	 */
	void resetHash(void);
	/*!calculate 160-bit digest.  Pass the 512-bit datablock as 
	 *an argument. A pointer to the digest is returned
	 */
	byte* calcHash(byte* Wt);
	#endif
	
	//random.c
	#ifdef NEED_RANDOM
	/*!Routine that uses a hardware random number generator to 
	 *calculate 128-bit (16-byte) random numbers
	 */
	void getRandom(byte bit128Random[16]);
	#endif
	
	/*test.c
	 *The implementations of the code in test.c can be easily disabled
	 *by removing the TEST_xxx defines.  The use of the defines makes
	 *it possible to prevent other test-routines from compiling in the
	 *code when they are not needed
	 */
	#ifdef TEST_RANDOM2
	//!Test-routine that checks the generation of random numbers
	void testRandom2(void);
	#endif
	#ifdef TEST_HMAC
	//!Test-routine that checks the calculation of HMAC's
	void testHmac(void);
	#endif
	#ifdef TEST_AUTHENTICATE
	//!Test-routine that checks the authentication protocol
	void testAuthenticate(void);
	#endif
	
	//hmac.c
	#ifdef NEED_HMAC
	//!Routine for calculating HMAC of a 55-byte buffer
	byte* getHmac(byte text[55]);
	#endif
		
	//timer2.c
	#ifdef NEED_TIMER2
	//!initialize timer2 (enable IRQ, set timeout)
	void initTimer2(void);
	//disable timer2 (disable IRQ)
	void stopTimer2(void);
	#endif

	//serieData.c
	#ifdef NEED_NEORS232
	/*!Routine called when data is ready.  User must implement this
	 *routine himself
	 */
	void dataRXready(byte* buffer,byte length);
	/*!Use this routine to write data to the RS232-bus using the
	 *NeoRS232-protocol.
	 */
	void writeData(byte* buffer, byte grootte);
	/*!Routine that must be implemented by the user.  It is called
	 *when no acknowledge is received upon the first time sending of
	 *data.
	 */
	void retransmit(void);
	/*!Routine that must be implemented by the user. It is called after
	 *several consecutive trials to send data using the NeoRS232-data
	 */
	void noAnswer(void);
	#endif

	//CRC_test.c
	#ifdef NEED_CRC
	//!Strips CRC16 off a buffer and checks if this CRC is correct
	BOOL stripCRC(byte* buffer, byte length);
	//!Calculate CRC16 and append it at the end of a buffer
	void appendCRC(byte* buffer, byte length);
	#endif

#endif
