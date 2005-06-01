/*! \mainpage AVR-Kryptoknight
 *
 * \section intro_sec Introduction
 * \subsection what_in What's in this package
 * This program consists of five smaller projects put together in a
 * single project.  
 * The first project is the NeoRS232-protocol.  This is an idle-RQ stop
 * and wait protocol with a CRC16-checksum.
 * The second and following project all have more or less to do with
 * cryptograpy.  The second project is an implementation of a hardware
 * number generator.  Secure hardware numbers are very important in 
 * cryptography for generating keys and authentication.
 * The third project is the SHA-1 hash calculation algorithm.  This has
 * been implemented in assembly language.
 * The fourth project, the HMAC calculation makes use of the SHA-1
 * calculation to sign messages for authentication purposes.
 * The fifth project, namely the Kryptoknight authentication algorithm
 * combines all of the above projects in a process that enables two
 * machines to authenticate one another.
 * \subsection why Why did I make this project?
 * The main incentive to start this project was to improve security in 
 * wireless systems for garagedoors.  People lock there houses with 
 * sophisticated mechanical keys, but leave their garagedoor nearly
 * unprotected.  Current wireless garagedoor systems are actually very
 * simple and easy to hack when one has a sender and a transmitter.
 */

/*! \file main.c
 *This is the main file of the project, containing the main-loop.
 */

#define NEED_USART
#define TEST_AUTHENTICATE
#include "define.h"

/*!Main function.  After boot-code finishes, it jumps to this routine.
 *This function configures PORTB as an output port.  I connected eight
 *leds to this port.  It also initializes the USART and calls a test-
 *routine, in this case the authentication test.
 */
int main(void){
	DDRB=0xFF;
	PORTB=0x00;
	init_USART();
	//testHmac();
	testAuthenticate();
	while(1);
	return 0;
}

