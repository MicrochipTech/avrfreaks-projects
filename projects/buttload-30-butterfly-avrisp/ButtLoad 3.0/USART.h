/*
             BUTTLOAD - Butterfly ISP Programmer

              Copyright (C) Dean Camera, 2007.
              
             dean [at] fourwalledcubicle [dot] com
                  www.fourwalledcubicle.com

           Released under the GPL Licence, Version 2.
*/

#ifndef USART_H
#define USART_H

	// INCLUDES:
	#include <avr/io.h>
	
	#include "GlobalMacros.h"
	#include "LCD_Driver.h"
	#include "RingBuff.h"
	#include "Timeout.h"
	#include "OSCCAL.h"
	
	// MACROS AND DEFINES:	
	#define USART_BAUDRATE        115200UL
	#define USART_BAUDVALUE       (((F_CPU / (USART_BAUDRATE * 16UL))) - 1)

	#define USART_OFF()           MACROS{ UCSRB = 0x00; PRR |= (1 << PRUSART0); }MACROE
	
	// PROTOTYPES:
	void USART_Init(void);
	char USART_Rx(void) ATTR_WARN_UNUSED_RESULT;
	
	// INLINE FUNCTIONS:
	static inline void USART_Tx(const char data) ATTR_ALWAYS_INLINE;
	static inline void USART_Tx(const char data)
	{
		while (!(UCSRA & (1 << UDRE)));
		UDR = data;
	}
#endif
