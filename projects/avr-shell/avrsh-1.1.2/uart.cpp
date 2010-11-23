////////////////////////////////////////////////////////////////////////////////
/// Name: uart.cpp
/// Author: B. Gian James  <gian@BattleDroids.net>
/// Description: Commonly used things n stuff.
///
/// $Id: uart.cpp,v 1.3 2009/08/29 21:04:21 gian Exp $
////////////////////////////////////////////////////////////////////////////////
#include "common.h"
#include "uart.h"
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <string.h>

static volatile uchar _txbuff_[MAX_TXBUFF];
static volatile uchar _rxbuff_[MAX_RXBUFF];

void
UART::WriteLine(const char * msg)
{
	///UART::WriteRAM("[AVR] ");
	while (pgm_read_byte(msg) != 0x00)
		tx(pgm_read_byte(msg++)); 
}

void
UART::WriteRAM(const char * msg)
{
	//UART::WriteLine(PSTR("[AVR] "));
	while(*msg != 0x00)
		tx(*(msg++));
}

unsigned char
UART::rx()
{
	while ( !(UCSR0A & (1<<RXC0)) ) ;
	return UDR0;	
}

void 
UART::tx(uchar c)
{
	// Check UDRE0 in UCSR0A.  If 1, then buffer empty.
	loop_until_bit_is_set(UCSR0A,UDRE0);
	UDR0 = c;
}

#ifdef __BROKEN_PGMSPACE__
char const __attribute__((__progmem__)) endl[] = "\r\n";
void
UART::uart_tx_P(const char * c)
{
 while (pgm_read_byte(c) != 0x00)
     uart_tx(pgm_read_byte(c++)); 
}
#endif

//////////////////////////////// [ class UART
UART::UART(BaudRate baud, uint8 d)
{
	switch(baud)
	{
	case Baud9600:
		#undef BAUD
		#define BAUD 9600
		break;
	case Baud57600:
		#undef BAUD
		#define BAUD 57600
		break;
	case Baud115200:
		#undef BAUD
		#define BAUD 115200
		break;
	case Baud14400:	
		#undef BAUD
		#define BAUD 14400
		break;
	case Baud28800:
		#undef BAUD
		#define BAUD 28800
		break;
	case Baud2400:	
		#undef BAUD
		#define BAUD 2400
		break;
	case Baud19200:
		#undef BAUD
		#define BAUD 19200
		break;
	case Baud4800:
		#undef BAUD
		#define BAUD 4800
		break;
	case Baud38400:
		#undef BAUD
		#define BAUD 38400
		break;
	default:
		#undef BAUD
		#define BAUD 57600
		break;
	}
	#include <util/setbaud.h>
	UBRR0H = UBRRH_VALUE;
	UBRR0L = UBRRL_VALUE;
	#if USE_2X
	UCSR0A |= (1<<U2X0);
	#else
	UCSR0A &= ~(1<<U2X0);
	#endif
	
	if (d & UART_RX) UCSR0B |= (1<<RXEN0);
	if (d & UART_TX) UCSR0B |= (1<<TXEN0);
	
	_txbuff.SetBuff(_txbuff_,MAX_TXBUFF);
	_rxbuff.SetBuff(_rxbuff_,MAX_RXBUFF);
	
}

void
UART::EnableInterrupt(uint8 d)
{
	
	if (d & UART_RX) UCSR0B |= (1<<RXCIE0);
	if (d & UART_TX) UCSR0B |= (1<<TXCIE0);
}

#ifdef __UART_STREAM__
UART & operator << (UART & u, const char * m)
{
	while(*m != 0x00) //&& *m != '\n')
		u.uart_tx(*(m++));
	return u;
}

UART & operator << (UART & u, const PPkt * p)
{
	u.uart_tx(p->ss);
	u.uart_tx(p->cmd);
	u.uart_tx(p->sz);
	uint8 x;
	for (x=0; x < (uint8)p->sz; x++)
		u.uart_tx(p->data[x]);
	u.uart_tx(p->chksum);
	return u;
}

UART & operator << (UART & u, const char c)
{
	u.uart_tx(c);
	return u;
}

UART & operator << (UART & u, const uint16 c)
{
	unsigned char uc = (unsigned char)(c >>8);
	u.uart_tx(uc);
	uc = (c & 0x0F);
	u.uart_tx(uc);
	return u;
}

UART & operator >> (UART & u, const char * m)
{
	while(*m != 0x00)
	{
		loop_until_bit_is_set(UCSR0A,UDRE0);
		// TODO - add to ringbuffer
	}
	return u;
}

#endif /* __UART_STREAM__ */
	