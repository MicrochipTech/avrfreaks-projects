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
// Header for the Snap library. See Snap.c for more information.
//
//----------------------------------------------------------------------------------

#ifndef __snap_h
#define __snap_h

// UART contants
#define XTAL				8000000
#define UART_BAUD			9600
#define UART_BAUD_SELECT 	(XTAL/(UART_BAUD*16l)-1)


// Define input pin to be used to detect UART activity
#define UART_DDR			DDRC
#define UART_PORT			PORTC
#define UART_PIN			PINC
#define UART_RX				5


// Snap constants
#define SNAP_PREAMBLE1		'!'				// Preamble byte 1
#define SNAP_PREAMBLE2		'#'				// Preamble byte 2
#define SNAP_SYNC			'T'				// Sync byte (start of SNAP packet)
#define SNAP_DAB			0xC0			// Number of Destination Address Bytes -> 3
#define SNAP_SAB			0x30			// Number of Source Address Bytes -> 3
#define SNAP_PFB			0x00			// Protocol specific Flag Bytes -> 0, unused
#define SNAP_ACK_REQUEST	0x01			// Request ACK
#define SNAP_ACK			0x02			// Send ACK
#define SNAP_NAK			0x03			// Send NAK
#define SNAP_CMD			0x00			// Command mode bit -> 0
#define SNAP_EDM			0x00			// Error detection method -> none
#define SNAP_NDB_MASK		0x0F			// Mask bits: Number of data bytes
#define SNAP_DAB_MASK		0xC0			// Mask bits: Number of Destination Address Bytes
#define SNAP_SAB_MASK		0x30			// Mask bits: Number of Source Address Bytes
#define SNAP_ACK_MASK		0x03			// Mask bits: ACK
#define SNAP_CMD_MASK		0x80			// Mask bits: Command mode bit
#define SNAP_EDM_MASK		0x70			// Mask bits: Error detection method
#define SNAP_PFB_MASK		0x0C			// Mask bits: Num Protocol Flag Bytes


// Received Snap Packet
unsigned char 	SnapPacketData[512];		// Data of the current packet
unsigned int	SnapPacketDataLength;		// Number of bytes in SnapPacketData
unsigned long	SnapPacketDest;				// For completed packets, this will always be this node's address
unsigned long	SnapPacketSource;			// Address of the sender
char			SnapPacketAck;				// Ack byte of the packet
unsigned int	SnapPacketCRC;


// Prototypes
void 			SnapInit(unsigned long);
void 			SnapReceiveChar(unsigned char);
char 			SnapSend(unsigned long, unsigned char data[], unsigned int);
inline void 	SnapReset(void);
inline char 	SnapPacketReceived(void);
void 			TxChar(unsigned char data);


#endif
