/*
 * USART.h
 * libavrutil++
 *
 * Created by Árpád Goretity on 04/04/2012.
 * Licensed under a CreativeCommons Attribution-ShareAlike 3.0 Unported License
 * http://creativecommons.org/licenses/by-sa/3.0/
 */

#ifndef __AVRUTILPP_USART_H__
#define __AVRUTILPP_USART_H__

#include <stdio.h>
#include <avrutil/avrutil.h>
#include "base.h"

namespace avrutil {

class USART {
	protected:
	uint8_t channel;
	uint32_t baud;
	~USART();
	public:
	USART(uint8_t channel, uint32_t baud);
	void close();
	
	void send(uint8_t byte);
	uint8_t receive();
	void write(const uint8_t *buf, size_t length);
	void read(uint8_t *buf, size_t length);
	
	size_t printf(const char *fmt, ...);
	size_t vprintf(const char *fmt, va_list args);
	
	void enableInterrupt(uint8_t mode);
	void disableInterrupt();
};

};

#endif /* __AVRUTILPP_USART_H__ */

