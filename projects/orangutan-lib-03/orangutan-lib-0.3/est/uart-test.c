//
// uart-test.c
//
// Steve Judd
//
// This is a simple program that shows how to use the routines in the Orangutan-lib uart module
// for basic serial communications.
//
//	Connect the Orangutan or Baby Orangutan to a terminal or terminal emulator via the serial
//	TX and RX lines (PD1 - data FROM the Orangutan and PD0, data TO the Orangutan).
//
// 11-Feb-2007
//

#define ORANGUTAN

#include <string.h>

#include "device.h"
#include "uart.h"

int16_t main(void);

	uint8_t banner[]="Orangutan UART Demo 1.0";
	uint8_t complete[]="Task Complete";
	uint8_t prompt[]="Enter some text and ENTER:";
	uint8_t response[]="You Typed:";

	static uint8_t done_flag;

int16_t main(void) {

	uint16_t bytes=0;

	uint8_t i=0, rcvBuf[20];


// initialize the uart and global data used by the uart routines
//
	uartInit();
	uartSetBaudRate(9600);

// First send a banner out the serial port a byte at a time.

	for (i=0;i<strlen(banner);i++) {
		uartSendByte(banner[i]);
	}
		uartSendByte(0x0d);		// cr
		uartSendByte(0x0A);		// lf

// Now send the same banner using buffer send

	uartSendBuffer(banner,strlen(banner),&done_flag);
	uartWait(&done_flag);

	uartSendByte(0x0d);		// cr
	uartSendByte(0x0A);		// lf

	uartSendBuffer(complete,strlen(complete),&done_flag);
	uartWait(&done_flag);

	uartSendByte(0x0d);		// cr
	uartSendByte(0x0A);		// lf

// Display a prompt and get the response 1 char at a time:

	uartSendBuffer(prompt,strlen(prompt),&done_flag);
	uartWait(&done_flag);

	i=0;
	while (i != 0x0A){
		do {
		i=uartGetByte();
		 } while (i==0xFF);
	 uartSendByte(i);
	}
	
	uartSendBuffer(complete,strlen(complete),&done_flag);
	uartWait(&done_flag);

	uartSendByte(0x0d);		// cr
	uartSendByte(0x0A);		// lf

	// Display the prompt and get the text in a user-provided buffer

	uartSendBuffer(prompt,strlen(prompt),&done_flag);
	uartWait(&done_flag);

	uartReceiveBuffer(rcvBuf,sizeof(rcvBuf),&bytes,0x0A,&done_flag);
	uartWait(&done_flag);
	uartSendByte(0x0d);		// cr
	uartSendByte(0x0A);		// lf

	uartSendBuffer(response,strlen(response),&done_flag);
	uartWait(&done_flag);
	uartSendBuffer(rcvBuf,bytes,&done_flag);
	uartWait(&done_flag);
	uartSendByte(0x0d);		// cr
	uartSendByte(0x0A);		// lf

	uartSendBuffer(complete,strlen(complete),&done_flag);
	uartWait(&done_flag);

	uartSendByte(0x0d);		// cr
	uartSendByte(0x0A);		// lf


	

	return(0);
}
