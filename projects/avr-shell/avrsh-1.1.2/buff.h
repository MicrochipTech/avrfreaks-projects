#ifndef _AVRSH_RBUFF_H_
#define _AVRSH_RBUFF_H_
////////////////////////////////////////////////////////////////////////////////
/// Name: rbuff.h
/// Author: B. Gian James  <gian@BattleDroids.net>
/// Description: A ring dual, multi-level ring buffer for storing data from 
/// USART, network, etc.
///
// $Id: rbuff.h,v 1.2 2009/08/29 21:04:21 gian Exp $
////////////////////////////////////////////////////////////////////////////////
#include "common.h"
#include <string.h>

#define DEFAULT_RING_BUFF_SZ	16	// power of 2!!

//! @class RingBuff
//! @brief Implements a queue or circular buffer
class RingBuff
{
public:
	RingBuff();
	~RingBuff() { }
	
	inline void		SetBuff(volatile unsigned char * b, const uint8 sz) { _buff = b; _sz = sz; _len = 0; }
	void			Put(unsigned char) volatile;
	unsigned char	Get() volatile;
	inline uint8	Empty() { return (_head == _tail); }
	inline uint8	Size() { return _sz; }
	inline uint8	Length() { return _len; }
	void			CopyBuff(char * dest);
	inline char *	Buff() { return (char*)_buff; }
	inline void		Clear() { memset((char*)_buff,0,DEFAULT_RING_BUFF_SZ); _head=_tail=0; }
	
private:
	volatile uchar *_buff;
	uint8			_sz;
	volatile uint8	_head;
	volatile uint8	_tail;
	volatile uint8	_len;
};




#endif	/* _AVRSH_RBUFF_H_ */


