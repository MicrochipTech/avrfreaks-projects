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
// Test program for Snap library. See Snap.c for more information.
//
//----------------------------------------------------------------------------------


#include <avr/interrupt.h>
#include "snap.h"

int main(void)
{

	// Clear MCU and General Interupt control registers
	MCUCR = 0x00;
	GICR = 0x00;

	// Initialize the Snap library
	#define NODE_ADDRESS  123
	SnapInit(NODE_ADDRESS);

	// Enable interrupts
	sei();

	// Loop forever
	while (1)
	{
		// Wait for packet
		if (SnapPacketReceived())
		{

			// The SnapPacket* variables contain the received packet
			// In this example we will send back the same data that we
			// just received
			SnapSend(SnapPacketSource, SnapPacketData, SnapPacketDataLength);

			// Reset the Snap receiver. Once a full packet has been received
			// the Snap library won't begin receiving the next packet
			// until this is called.
			SnapReset();
		}
	}
}
