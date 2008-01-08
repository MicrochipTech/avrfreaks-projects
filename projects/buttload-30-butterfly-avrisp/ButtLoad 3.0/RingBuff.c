/*
             BUTTLOAD - Butterfly ISP Programmer

              Copyright (C) Dean Camera, 2007.
              
             dean [at] fourwalledcubicle [dot] com
                  www.fourwalledcubicle.com

           Released under the GPL Licence, Version 2.
*/

#include "RingBuff.h"

volatile char    RingBuffer[BUFF_BUFFLEN] = {};
volatile uint8_t InPos                    = 0;
volatile uint8_t OutPos                   = 0;
volatile uint8_t BuffElements             = 0;

// ======================================================================================

/*
 NAME:      | USART0_RX_vect (ISR, blocking)
 PURPOSE:   | ISR to handle the reception of serial data, placing recieved bytes into a ring buffer
 ARGUMENTS: | None
 RETURNS:   | None
*/
ISR(USART0_RX_vect, ISR_BLOCK)
{
	RingBuffer[InPos] = UDR;
	BuffElements++;

	InPos++;

	if (InPos == BUFF_BUFFLEN)
	  InPos = 0;
}	

// ======================================================================================

/*
 NAME:      | BUFF_InitializeBuffer
 PURPOSE:   | Resets and initializes the ring buffer ready for byte storage
 ARGUMENTS: | None
 RETURNS:   | None
*/
void BUFF_InitializeBuffer(void)
{
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
	{
		InPos  = 0;
		OutPos = 0;
		BuffElements = 0;
	}
}

/*
 NAME:      | BUFF_GetBuffByte
 PURPOSE:   | Fetches the next byte from the ringbuffer
 ARGUMENTS: | None
 RETURNS:   | Next byte in ringbuffer, zero if buffer is empty
*/
char BUFF_GetBuffByte(void)
{
	uint8_t RetrievedData;

	if (!(BuffElements))
	  return 0;

	ATOMIC_BLOCK(ATOMIC_FORCEON)
	{
		RetrievedData = RingBuffer[OutPos];
		BuffElements--;
		
		OutPos++;
		
		if (OutPos == BUFF_BUFFLEN)
		  OutPos = 0;
	}
		
	return RetrievedData;
}
