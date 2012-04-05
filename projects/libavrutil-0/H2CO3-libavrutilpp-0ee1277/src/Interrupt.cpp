/*
 * Interrupt.cpp
 * libavrutil++
 *
 * Created by Árpád Goretity on 04/04/2012.
 * Licensed under a CreativeCommons Attribution-ShareAlike 3.0 Unported License
 * http://creativecommons.org/licenses/by-sa/3.0/
 */

#include "Interrupt.h"

avrutil::Interrupt::Interrupt(uint8_t intnum)
{
	avrutil::Interrupt::enableGlobalInterrupts();
	this->intnum = intnum;
}

avrutil::Interrupt::~Interrupt()
{
	this->removeHandler();
}

void avrutil::Interrupt::setHandler(uint8_t intnum, avr_interrupt_handler handler, uint8_t mode)
{
	avr_interrupt_set_handler(intnum, handler, mode);
}

void avrutil::Interrupt::removeHandler(uint8_t intnum)
{
	avr_interrupt_remove_handler(intnum);
}

void avrutil::Interrupt::enableGlobalInterrupts()
{
	avr_interrupt_enable();
}

void avrutil::Interrupt::disableGlobalInterrupts()
{
	avr_interrupt_disable();
}

void avrutil::Interrupt::setHandler(avr_interrupt_handler handler, uint8_t mode)
{
	avr_interrupt_set_handler(this->intnum, handler, mode);
}

void avrutil::Interrupt::removeHandler()
{
	avr_interrupt_remove_handler(this->intnum);
}

uint8_t avrutil::Interrupt::getInterruptNum()
{
	return this->intnum;
}

