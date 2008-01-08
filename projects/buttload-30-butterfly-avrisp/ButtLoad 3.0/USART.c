/*
             BUTTLOAD - Butterfly ISP Programmer

              Copyright (C) Dean Camera, 2007.
              
             dean [at] fourwalledcubicle [dot] com
                  www.fourwalledcubicle.com

           Released under the GPL Licence, Version 2.
*/

#include "USART.h"

// ======================================================================================

/*
 NAME:      | USART_Init
 PURPOSE:   | Initializes the USART, ready for data transmission and reception
 ARGUMENTS: | None
 RETURNS:   | None
*/
void USART_Init(void)
{
	// Calibrate the internal RC oscilator
	LCD_PutStr_f(BusyText);
	OSCCAL_Calibrate();

	// Enable USART subsystem
	PRR &= ~(1 << PRUSART0);
	
    // Set baud rate
	UBRRH = (uint8_t)(USART_BAUDVALUE >> 8);
    UBRRL = (uint8_t)(USART_BAUDVALUE);

	// Reset UCSRA to clear any pre-set bits
	UCSRA = 0;

    // Enable Rx/Tx subsections and recieve complete interrupt
	UCSRB = ((1 << TXEN) | (1 << RXEN) | (1 << RXCIE));
	 
    // Async. mode, 8N1
    UCSRC = (3 << UCSZ0);
	 
	BUFF_InitializeBuffer();
}

/*
 NAME:      | USART_Rx
 PURPOSE:   | Returns the next byte out of the ring buffer, looping until data ready or timeout expires
 ARGUMENTS: | None
 RETURNS:   | Next character in ringbuffer, or 0 if timeout expires
*/
char USART_Rx(void)
{
	while (!(BuffElements) && !(PacketTimeOut))
	  SLEEPCPU(SLEEP_IDLE);      // Idle: USART interrupt on reception of data, or timeout timer ISR will resume execution

	return BUFF_GetBuffByte();
}

