////////////////////////////////////////////////////////////////////////////////
/// Name: rbuff.cpp
/// Author: B. Gian James  <gian@BattleDroids.net>
/// Description: A ring dual, multi-level ring buffer for storing data from 
/// USART, network, etc.
///
// $Id: rbuff.cpp,v 1.1.1.1 2009/08/28 09:29:50 gian Exp $
#include "rbuff.h"
#include <string.h>

void
RingBuff::CopyBuff(char * dest)
{
	memcpy(dest, (char*)_buff, DEFAULT_RING_BUFF_SZ);
}


// Push onto the tail
void
RingBuff::Put(unsigned char c) volatile
{
	_buff[_tail++] = c;
	if (_tail >= _sz) _tail = 0;
	++_len;
}

// Pop from the head
unsigned char
RingBuff::Get() volatile
{
	if (_len == 0)
		return 0;
		
	unsigned char c = _buff[_head++];
	if (_head >= _sz) _head = 0;
	--_len;
	
	return c;
}


RingBuff::RingBuff()
{
	_buff = NULL;
	_head = _tail = _sz = _len = 0;
}