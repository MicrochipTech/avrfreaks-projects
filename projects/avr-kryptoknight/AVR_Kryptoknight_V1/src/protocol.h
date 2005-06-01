/*! \file protocol.h
 *This file contains the declarations of constants used in the NeoRS232
 *routines.
 */ 
	//USART protocol constants
	#define	LEADING_FLAG	0xAA//!<Start of package
	#define	DLE_FLAG		0x10//!<Byte stuffing flag
	#define	TRAILING_FLAG	0xFF//!<End of package
	#define	ACK0_FLAG		0x30//!<Acknowledge flag (separate packet)
	
	//! Possible frametypes in the NeoRS232-protocol
	typedef enum{
		INFO_FRAME,		//!<Constant defining a frame containing data
		ACK_FRAME		/*!<Constant defining a frame containing an 
						 *acknowledge
						 */
	} FRAME;
	
#define RS232LENGTH		50	//!<USART receive buffer size
