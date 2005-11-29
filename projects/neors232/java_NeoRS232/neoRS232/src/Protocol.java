/*!\file Protocol.java
 *Defines some constants for use in the protocol.  These
 *are the flags and constants for defining the kind of
 *frame.
 */
package LieBTrau.comm;

/** This interface contains some useful constants
 */
interface Protocol{
	byte 
		LEADING_FLAG=	(byte)0xAA,/*!<Start flag*/
		DLE_FLAG=		(byte)0x10,/*!<Bytestuffing flag*/
		TRAILING_FLAG=	(byte)0xFF,/*!<End flag*/
		ACK0_FLAG=		(byte)0x30;/*!<Acknowledge flag*/

	int INFO_FRAME=		0,/*!<Ordinary frame*/
		ACK_FRAME=		1;/*!<Acknowledge frame*/
}//interface