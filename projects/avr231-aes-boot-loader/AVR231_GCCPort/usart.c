/*****************************************************************************
*
* Atmel Corporation
*
* File              : usart.c
* Compiler          : IAR EWAAVR 2.28a/3.10c
* Revision          : $Revision: 2.0 $
* Date              : $Date: Wednesday, February 15, 2006 09:52:02 UTC $
* Updated by        : $Author: raapeland $
*
* Support mail      : avr@atmel.com
*
* Supported devices : All devices with Bootloader Capabilities 
*                     , and at least 1-KB SRAM can be used.
*                     The example is written for • ATmega8
*                                                • ATmega16
*                                                • ATmega162
*                                                • ATmega169
*                                                • ATmega32
*                                                • ATmega64
*                                                • ATmega128
*                                                • ATmega2561
*
* AppNote           : AVR231 - AES Bootloader
*
* Description:	      USART interface.
*
******************************************************************************/

#include <avr/io.h>
#include "aesglobal.h"
#include "bus.h"


//=============================================================================
// Initialize bus interface - This function was made a macro to ensure inlining
//=============================================================================

/*
void busInit(void)
{
	UBRRL = UBR;
	UBRRH = (UBR >> 8);
	UCSRB = (1 << RXEN) | (1 << TXEN);
}
*/

//=============================================================================
// Prepare bus (in this case, wait for the first byte to arrive).
//=============================================================================

void busPrepare(void)
{
	while (!(UCSRA & (1 << RXC)));
}


//=============================================================================
// Reply byte
//=============================================================================

void busReplyByte(unsigned char data)
{
	while (!(UCSRA & (1 << UDRE)));
	UDR = data;
}


//=============================================================================
// Receive byte
//=============================================================================

unsigned char busReceiveByte(void)
{
	busPrepare();
	return UDR;
}
