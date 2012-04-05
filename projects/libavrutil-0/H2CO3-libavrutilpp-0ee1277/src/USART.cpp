/*
 * USART.cpp
 * libavrutil++
 *
 * Created by Árpád Goretity on 04/04/2012.
 * Licensed under a CreativeCommons Attribution-ShareAlike 3.0 Unported License
 * http://creativecommons.org/licenses/by-sa/3.0/
 */

#include "USART.h"

avrutil::USART::USART(uint8_t channel, uint32_t baud)
{
	this->channel = channel;
	this->baud = baud;
	avr_usart_init(this->channel, this->baud);
}

avrutil::USART::~USART()
{
	this->close();
}

void avrutil::USART::close()
{
	avr_usart_close(this->channel);
}

void avrutil::USART::send(uint8_t byte)
{
	avr_usart_send(this->channel, byte);
}

uint8_t avrutil::USART::receive()
{
	return avr_usart_recv(this->channel);
}

void avrutil::USART::write(const uint8_t *buf, size_t length)
{
	avr_usart_write(this->channel, buf, length);
}

void avrutil::USART::read(uint8_t *buf, size_t length)
{
	avr_usart_read(this->channel, buf, length);
}

size_t avrutil::USART::printf(const char *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	size_t len = this->vprintf(fmt, args);
	va_end(args);
	return len;
}

size_t avrutil::USART::vprintf(const char *fmt, va_list args)
{
	return avr_usart_printf(this->channel, fmt, args);
}

void avrutil::USART::enableInterrupt(uint8_t mode)
{
	avr_usart_intr_enable(this->channel, mode);
}

void avrutil::USART::disableInterrupt()
{
	avr_usart_intr_disable(this->channel);
}

