/*
 * DigitalPort.h
 * libavrutil++
 *
 * Created by Árpád Goretity on 04/04/2012.
 * Licensed under a CreativeCommons Attribution-ShareAlike 3.0 Unported License
 * http://creativecommons.org/licenses/by-sa/3.0/
 */

#ifndef __AVRUTILPP_DIGITALPORT_H__
#define __AVRUTILPP_DIGITALPORT_H__

#include <avrutil/avrutil.h>

namespace avrutil {

class DigitalPort {
	protected:
	volatile uint8_t *addr;
	public:
	DigitalPort(volatile uint8_t *addr);

	void setBit(uint8_t bit);
	void clearBit(uint8_t bit);
	bool isBitSet(uint8_t bit);
	bool isBitClear(uint8_t bit);
	
	void writeByte(uint8_t byte);
	uint8_t readByte();
};

};

#endif /* __AVRUTILPP_DIGITALPORT_H__ */

