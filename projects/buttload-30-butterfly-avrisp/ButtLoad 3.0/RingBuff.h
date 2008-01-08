/*
             BUTTLOAD - Butterfly ISP Programmer

              Copyright (C) Dean Camera, 2007.
              
             dean [at] fourwalledcubicle [dot] com
                  www.fourwalledcubicle.com

           Released under the GPL Licence, Version 2.
*/

#ifndef RINGBUFF_H
#define RINGBUFF_H

	// INCLUDES:
	#include <avr/io.h>
	#include <avr/interrupt.h>
	#include <util/atomic.h>
	
	#include "GlobalMacros.h"
	
	// DEFINES:
	#define BUFF_BUFFLEN  64

	// EXTERN VARIABLES:
	extern volatile char    RingBuffer[BUFF_BUFFLEN];
	extern volatile uint8_t InPos;
	extern volatile uint8_t OutPos;
	extern volatile uint8_t BuffElements;
	
	// PROTOTYPES:
	void BUFF_InitializeBuffer(void);
	char BUFF_GetBuffByte(void);
	
#endif
