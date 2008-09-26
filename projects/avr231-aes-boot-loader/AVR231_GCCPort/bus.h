/*****************************************************************************
*
* Atmel Corporation
*
* File              : bus.h
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
*
* AppNote           : AVR231 - AES Bootloader
*
* Description:	    General software interface for the bus used for data
*                   transmission of encrypted frames.
*
******************************************************************************/

#ifndef BUS_H
#define BUS_H

#ifndef UBRRL
#define UBRRL  UBRR0L
#define UBRRH  UBRR0H
#endif

#ifndef UCSRA
#define UCSRA  UCSR0A
#define UCSRB  UCSR0B
#define UDR    UDR0
#define UDRE   UDRE0
#define RXC    RXC0
#define RXEN   RXEN0
#define TXEN   TXEN0

#endif

#define UBR  (unsigned int)( F_CPU / (16.0 * BAUD) - 0.5 )

#define busInit() {     \
	UBRRL = UBR;        \
	UBRRH = (UBR >> 8); \
	UCSRB = (1 << RXEN) | (1 << TXEN);}

// Prepare bus for self-programming purposes
extern void busPrepare(void);

// Receive one byte
extern unsigned char busReceiveByte(void);

// Reply one byte as a response to the received frame
extern void busReplyByte(unsigned char data);

#endif // BUS_H
