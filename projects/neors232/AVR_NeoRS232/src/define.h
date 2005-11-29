/*!\file define.h
 */
#ifndef _DEFINE_H_	/*!<Don't include this file twice*/
	
	//to prevent multiple definitions of the same thing
	#define _DEFINE_H_ 1

	//MCU frequency
	#define FOSC 	16000000/*!<MCU operating frequency*/
	
	//AVR-libC include libraries
	#include <avr/io.h>
	#include <avr/crc16.h>
	#include <avr/signal.h>
	#include <avr/interrupt.h>
	

	//!USART protocol constants
	typedef enum{
		LEADING_FLAG=0xAA,		/*!<Start flag*/
		DLE_FLAG=0x10,			/*!<Bytestuffing flag*/
		TRAILING_FLAG=0xFF,		/*!<End flag*/
		ACK0_FLAG=0x30			/*!<Acknowledge flag*/
	}FLAGS;
	
	//!The types of frames that can occur in the protocol
	typedef enum{
		INFO_FRAME=0,			/*!<Info frame (contains data)*/
		ACK_FRAME=1				/*!<Acknowledge frame*/
	}FRAME_TYPE;

	//!USART receive buffer size
	#define FRAMESIZE 100

	//!Implementation of boolean type
	typedef enum{
		FALSE,					/*!<False, 0*/
		TRUE					/*!<True, 1*/
	} BOOL;
	
	//!States of the receiving state machine
	typedef enum{
		MSG_NO_INPUT,			/*!<No data has come in.*/
		MSG_DECODING,			/*!<State machine is decoding data.*/
		MSG_RX_COMPLETE			/*!<Complete packet received.*/
	}MSG_T;

	/*! Struct that will contain the received frame.
	 */
	typedef struct{
		/*! Points to the first character of the received frame*/
		uint8_t* pBegin;
		/*! Points to one position after the last received frame.
		 *  So the number of received bytes = pData - pBegin
		 */
		uint8_t* pData;
		/*! Points to the last position of the buffer.
		 *  This variable defines the maximum size of the buffer.
		 */
		uint8_t* pEnd;
	}FRAMER;
		
	//timer.c
	void initTimer2(void);
	void stopTimer2(void);

	//simpleWrite.c
	void writeString(
		uint8_t* buffer, 
		uint8_t length, 
		FRAME_TYPE typeFrame
	);

	//simpleRead.c
	MSG_T FramerReceive(FRAMER* pFr);	

	//serieData.c
	void writeFrame(uint8_t* buffer, uint8_t length);

	//CRC_test.c
	BOOL stripCRC(FRAMER* pFramer);
	void appendCRC(uint8_t* buffer, int length);

	//usart.c
	void init_USART(uint32_t baud, BOOL RX_ON);
	void write_USART(uint8_t* buf, uint8_t length);
	void write_USART_B(uint8_t c);
	BOOL read_USART(uint8_t* c);
	
	//main.c
	void receiveInit(FRAMER* pFr);
	void receiveRun(FRAMER* pFramer);

#endif