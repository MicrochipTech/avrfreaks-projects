/*
 * DigitalPort.cpp
 * libavrutil++
 *
 * Created by Árpád Goretity on 04/04/2012.
 * Licensed under a CreativeCommons Attribution-ShareAlike 3.0 Unported License
 * http://creativecommons.org/licenses/by-sa/3.0/
 */

#include "DigitalPort.h"

avrutil::DigitalPort::DigitalPort(volatile uint8_t *addr)
{
	this->addr = addr;
}

void avrutil::DigitalPort::setBit(uint8_t bit)
{
	avr_bit_set(*this->addr, bit);
}

void avrutil::DigitalPort::clearBit(uint8_t bit)
{
	avr_bit_clear(*this->addr, bit);
}

bool avrutil::DigitalPort::isBitSet(uint8_t bit)
{
	return avr_bit_isset(*this->addr, bit);
}

bool avrutil::DigitalPort::isBitClear(uint8_t bit)
{
	return avr_bit_isclear(*this->addr, bit);
}
	
void avrutil::DigitalPort::writeByte(uint8_t byte)
{
	*this->addr = byte;
}

uint8_t avrutil::DigitalPort::readByte()
{
	return *this->addr;
}

