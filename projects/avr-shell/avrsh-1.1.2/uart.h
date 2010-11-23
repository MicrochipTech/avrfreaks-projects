#pragma once
////////////////////////////////////////////////////////////////////////////
/// Name: uart.h
/// Author: B. Gian James   <gian@OpenHouseware.net>
/// Description: Defines peripheral devices for OpenHouseware.
///            
///          
/// $Id: uart.h,v 1.2 2009/08/29 21:04:21 gian Exp $

#include "common.h"
#include "rbuff.h"


#define UART_RX 0x01
#define UART_TX 0x02
#define UART_TX_ACK	"y"


enum BaudRate { Baud2400, Baud4800, Baud9600, Baud14400, Baud19200, Baud28800, Baud38400, Baud57600, Baud115200 };

#define MAX_RXBUFF	32
#define MAX_TXBUFF	32
class UART
{
#ifdef __UART_STREAM__
friend UART & operator << (UART &, const char *);
friend UART & operator << (UART &, const char);
friend UART & operator << (UART &, const uint16);
friend UART & operator << (UART &, const PPkt *);
friend UART & operator >> (UART &, const char *);
#endif
public:
	UART(BaudRate b, uint8 d);
	
	void 			EnableInterrupt(uint8);
	static void 	tx(uchar c);
	static uchar 	rx();
	static void		WriteLine(const char *);
	static void		WriteRAM(const char *);
	inline void 	AddToReceive(uchar c) { _rxbuff.Put(c); }
	inline uchar 	GetFromReceive() { return _rxbuff.Get(); }
	
	inline char *			GetTXBuff() { return _txbuff.Buff(); }
	inline char *			GetRXBuff() { return _rxbuff.Buff(); }
	inline void				FlushRXBuff() { _rxbuff.Clear(); }
	//inline void				ClearRXBuff() { _rxbuff.Clear(); }
	inline void				ClearTXBuff() { _txbuff.Clear(); }
	
	
protected:
	RingBuff	_txbuff;
	RingBuff	_rxbuff;
};

