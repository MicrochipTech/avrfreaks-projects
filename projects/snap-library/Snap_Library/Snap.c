//----------------------------------------------------------------------------------
//
// Copyright (c) 2003 Brian Low (mportobello@hotmail.com)
//
// Permission is hereby granted, free of charge, to any person obtaining a copy of
// this software and associated documentation files (the "Software"), to deal in
// the Software without restriction, including without limitation the rights to
// use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
// the Software, and to permit persons to whom the Software is furnished to do so,
// subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
// FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
// COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
// IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
// CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//
//----------------------------------------------------------------------------------

//----------------------------------------------------------------------------------
//
// SNAP Protocol Library
//
// Simple library for sending and receiving Snap packets.
//
// The library has the following features:
//	- easy to use
//	- collision avoidance (e.g. RS485 half-duplex networks)
//	- crc-16 error detection
//	- interrupt-based receiving
//	- blocking transmitting
//
// The Snap protocol is very flexible. See http://www.hth.com for more
// information. The library has the following restrictions to keep it simple:
//	- 3 byte addresses (16.7 million nodes)
//	- crc-16 error detection
//  - command-mode bits and protocol specific flags not supported
//	- 2 preamble bytes to improve packet start detection
//
// Example: the device will echo back any data received
//
//		#include <avr/interrupt.h>
//		#include "snap.h"
//
//  	int main(void)
//		{
//			MCUCR = 0x00;
//			GICR = 0x00;
//
//			#define NODE_ADDRESS  123
//			SnapInit(NODE_ADDRESS);
//
//			sei();
//
//			while (1)
//			{
//				if (SnapPacketReceived())
//				{
//					SnapSend(SnapPacketSource, SnapPacketData, SnapPacketDataLength);
//					SnapReset();
//				}
//			}
//		}
//
//----------------------------------------------------------------------------------

#include <avr/io.h>
#include <avr/signal.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include "delay.h"
#include "edm.h"
#include "snap.h"

// Snap Receiver States
#define SNAP_STATE_START			0
#define SNAP_STATE_PREAMBLE1		0
#define SNAP_STATE_PREAMBLE2		1
#define SNAP_STATE_SYNC				2
#define SNAP_STATE_HEADER2			3
#define SNAP_STATE_HEADER1			4
#define SNAP_STATE_DEST3			5
#define SNAP_STATE_DEST2			6
#define SNAP_STATE_DEST1			7
#define SNAP_STATE_SOURCE3			8
#define SNAP_STATE_SOURCE2			9
#define SNAP_STATE_SOURCE1			10
#define SNAP_STATE_DATA				11
#define SNAP_STATE_CRC1				12
#define SNAP_STATE_CRC2				13
#define	SNAP_STATE_PACKET_RECEIVED	99

// Snap receiver variables
unsigned char	SnapRxState;				// Current state of the receiver
unsigned int	SnapRxDataIndex;			// Index of next byte to write to SnapPacketData

// Address of this node
unsigned long SnapNodeAddress = 123;



//----------------------------------------------------------------------------------
// Initialize the Snap library
//  - set the device's address
//	- reset the receiver
//  - initial the UART
//----------------------------------------------------------------------------------
void SnapInit(unsigned long nodeAddress)
{
	// Save this node's address
	SnapNodeAddress = nodeAddress;

	// Reset receiver state
	SnapReset();

	// Configure hardware UART
	UCSRB = 0x00; 									// disable while setting baud rate
	UCSRA = 0x00;
	UBRRH = (unsigned char)(UART_BAUD_SELECT>>8); 	// set baud rate hi
	UBRRL = (unsigned char)UART_BAUD_SELECT; 		// set baud rate lo
	UCSRB = BV(TXEN)|BV(RXEN)|BV(RXCIE);			// enable transmit, receive, receive complete interrupts
	UCSRC = BV(URSEL)|BV(UCSZ1)|BV(UCSZ0);			// 8-bit characters

	// Configure uart pin (for detecting uart activity)
	cbi(UART_DDR, UART_PIN);						// Set direction to input
	sbi(UART_PORT, UART_PIN);						// enable pull-up resistors

	// Set seed for rand()
	srand(nodeAddress);

}


//----------------------------------------------------------------------------------
//	Interrupt handler for UART receiver
//	Process each character as it is received.
//----------------------------------------------------------------------------------
SIGNAL(SIG_UART_RECV)
{
	unsigned char ch;
	ch = UDR;
	SnapReceiveChar(ch);
}


//----------------------------------------------------------------------------------
// Reset the receiver discarding any existing data/packets received
//----------------------------------------------------------------------------------
inline void SnapReset(void)
{
	SnapRxState = SNAP_STATE_START;
}

//----------------------------------------------------------------------------------
// Returns non-zero if a complete packet has been received.
// Call SnapReset() when done with the packet to enable receiving the next packet.
//----------------------------------------------------------------------------------
inline char SnapPacketReceived(void)
{
	return( SnapRxState == SNAP_STATE_PACKET_RECEIVED );
}

//----------------------------------------------------------------------------------
// Process a single character as part of a Snap packet.
//
// To determine when a complete packet is received
//	if (SnapPacketReceived()) ...
//
// Once a complete packet is received, all subsequent incoming
// characters will be ignored until you set
//	SnapReset();
//
// This method is implemented as a state machine. As each
// character is received, if it is expected based on the Snap
// protocol then the state machine is advanced.
//----------------------------------------------------------------------------------
void SnapReceiveChar(unsigned char ch)
{
	unsigned int x;
	static unsigned int crc = 0;


	if (SnapRxState == SNAP_STATE_HEADER2)
		crc = crc16(&ch, 1);
	else if ((SnapRxState > SNAP_STATE_HEADER2) & (SnapRxState <= SNAP_STATE_DATA))
		crc = crc16_continue(&ch, 1, crc);

	switch (SnapRxState)
	{
		case SNAP_STATE_PREAMBLE1: 	if (ch == SNAP_PREAMBLE1) SnapRxState++; break;
		case SNAP_STATE_PREAMBLE2:	if (ch == SNAP_PREAMBLE2) SnapRxState++; else SnapRxState=0; break;
		case SNAP_STATE_SYNC: 		if (ch == SNAP_SYNC) SnapRxState++; else SnapRxState=0; break;
		case SNAP_STATE_HEADER2:
									if (((ch & SNAP_DAB_MASK) == SNAP_DAB) &&
										((ch & SNAP_SAB_MASK) == SNAP_SAB) &&
										((ch & SNAP_PFB_MASK) == SNAP_PFB) )
									{
										SnapPacketAck = ch & SNAP_ACK_MASK;
										SnapRxState++;
									}
									else
										SnapRxState = 0;
									break;
		case SNAP_STATE_HEADER1:
									if (((ch & SNAP_CMD_MASK) == SNAP_CMD) &&
										((ch & SNAP_EDM_MASK) == SNAP_EDM))
									{
										x = ch & SNAP_NDB_MASK;
										if (x <= 8)
											SnapPacketDataLength = x;
										else
										{
											x = x - 8;
											SnapPacketDataLength = 8;
											for ( ; x>0; x--)
												SnapPacketDataLength = SnapPacketDataLength * 2;
										}
										SnapRxState++;
									}
									else
										SnapRxState = 0;
									break;
		case SNAP_STATE_DEST3:		SnapPacketDest = ((unsigned long)ch) << 16; SnapRxState++; break;
		case SNAP_STATE_DEST2:		SnapPacketDest += ((unsigned long)ch) << 8; SnapRxState++; break;
		case SNAP_STATE_DEST1:		SnapPacketDest += ((unsigned long)ch) << 0;
									if (SnapPacketDest == SnapNodeAddress)
										SnapRxState++;
									else
										SnapRxState=0;
									break;
		case SNAP_STATE_SOURCE3:	SnapPacketSource = ((unsigned long)ch) << 16; SnapRxState++; break;
		case SNAP_STATE_SOURCE2:	SnapPacketSource += ((unsigned long)ch) << 8; SnapRxState++; break;
		case SNAP_STATE_SOURCE1:	SnapPacketSource += ((unsigned long)ch) << 0; SnapRxState++;
									SnapRxDataIndex = 0;
									if (SnapPacketDataLength == 0)
										SnapRxState++;
									break;

		case SNAP_STATE_DATA:		SnapPacketData[SnapRxDataIndex] = ch;
									SnapRxDataIndex++;
									if (SnapRxDataIndex >= SnapPacketDataLength)
										SnapRxState++;
									break;

		case SNAP_STATE_CRC1:		SnapPacketCRC = (ch << 8);
									SnapRxState++;
									break;

		case SNAP_STATE_CRC2:		SnapPacketCRC += ch;
									SnapRxState = SNAP_STATE_START;
									if (crc == SnapPacketCRC)
									{
										if (SnapPacketDest == SnapNodeAddress)
										{
											SnapRxState = SNAP_STATE_PACKET_RECEIVED;
										}
									}
									break;

	}

}



//----------------------------------------------------------------------------------
// Send a SNAP packet.
//
// This method will block until the full packet is sent.
//
// Collision avoidance:
// This method waits the time is takes to transmit 1 full character. If no activity
// is detected the packet is transmitted. If activitiy is detected the method waits
// a random amount time after the last detected activity before transmitting.
//
// Returns 0 if successful. Returns 0xFF if data is larger than 512 bytes
//----------------------------------------------------------------------------------
char SnapSend(unsigned long dest, unsigned char data[], unsigned int length)
{
	unsigned char c;
	unsigned char n;
	unsigned int  i;
	unsigned int  padding = 0;
	unsigned int  crc;
	unsigned char header[8];


	// Header byte 2
	c = SNAP_DAB | SNAP_SAB | SNAP_PFB | SNAP_ACK_REQUEST;
	header[0] = c;

	// Determine the size of the packet. The protocol allows 0 to 512 bytes of
	// data but not all sizes within that are supported. In the header, if bit 3 = 0,
	// then bits 2..0 indicate the exact size in bytes. If bit 3 = 1, then bits 2..0
	// the size of the data is 2^(bits+3). If the data is greater than 7 bytes then
	// we will pick the next largest size.
	if (length > 512)
		return( 0xFF );

	// Handle sizes >= 8 differently, see comments above
	if (length < 8)
		n = length;
	else
	{
		for (c=8; c>0; c--)
		{
			if (length & BV(c))
				break;
		}
		if (length ^ BV(c))
			c++;
		n =
		padding = BV(c) - length;
		n = (c-3) | 0x08;							// Set bit 3 to indicate size is 2^(bits+3) bytes
	}

	// Header byte 1
	c = SNAP_CMD | SNAP_EDM | n;
	header[1] = c;

	// Destination address (3 bytes)
	header[2] = (unsigned char)(dest >> 16);
	header[3] = (unsigned char)(dest >> 8);
	header[4] = (unsigned char)(dest >> 0);

	// Source address (3 bytes)
	header[5] = (unsigned char)(SnapNodeAddress >> 16);
	header[6] = (unsigned char)(SnapNodeAddress >> 8);
	header[7] = (unsigned char)(SnapNodeAddress >> 0);

	// Calculate CRC16
	crc = crc16(header, 8);
	crc = crc16_continue(data, length, crc);
	c = 0;
	for (i=padding; i>0; i--)
		crc = crc16_continue(&c, 1, crc);

	// Check for clear line
	// Wait the time it take for 1 full character (10 bits) to be sent
	for (i=(1.0/(UART_BAUD/10))*1000000/MIN_DELAY_US; i>0; i--)
	{
		if (bit_is_set(UART_PIN, UART_RX))
			break;
		DELAY(MIN_DELAY_US);
	}

	// Is the line in use? If so wait for the end of the transmission
	// plus a random amount of time to avoid colliding with other
	// devices
	while (i > 0)
	{
		// Reset the delay countdown everytime we see the pin high
		if (bit_is_set(UART_PIN, UART_RX))
		{
			i = (10.0/(UART_BAUD/10))*1000000/MIN_DELAY_US;		// i = max delay
			i = i && (unsigned int) rand();					// i = random # between 0 and max delay
			i = i + (1.0/(UART_BAUD/10))*1000000/MIN_DELAY_US;	// i = random # between min and max delay
		}
		DELAY(MIN_DELAY_US);
		i--;
	}

	// Send preamble and sync
	TxChar(SNAP_PREAMBLE1);
	TxChar(SNAP_PREAMBLE2);
	TxChar(SNAP_SYNC);

	// Send the header
	for (i=0; i<8; i++)
		TxChar(header[i]);

	// Send the data
	for (i=0; i<length; i++)
		TxChar(data[i]);

	// Send padding
	for (i=padding; i>0; i--)
		TxChar(0);

	// Send CRC
	TxChar((unsigned char)(crc>>8));
	TxChar((unsigned char)(crc>>0));

	// Send newline for easier debugging
	// The packet is done, these characters should be ignored by receivers
	TxChar('\r');
	TxChar('\n');

	return (0);
}

//----------------------------------------------------------------------------------
// Send a single character using the hardware UART
//----------------------------------------------------------------------------------
void TxChar(unsigned char data)
{
	// Wait for empty transmit buffer
	while ( !( UCSRA & (1<<UDRE)) )
		;

	// Put data into buffer, sends the data
	UDR = data;
}





