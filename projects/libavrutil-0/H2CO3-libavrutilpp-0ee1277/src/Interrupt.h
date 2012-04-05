/*
 * Interrupt.h
 * libavrutil++
 *
 * Created by Árpád Goretity on 04/04/2012.
 * Licensed under a CreativeCommons Attribution-ShareAlike 3.0 Unported License
 * http://creativecommons.org/licenses/by-sa/3.0/
 */

#ifndef __AVRUTILPP_INTERRUPT_H__
#define __AVRUTILPP_INTERRUPT_H__

#include <avrutil/avrutil.h>
#include "base.h"

namespace avrutil {

class Interrupt {
	protected:
	uint8_t intnum;
	~Interrupt();
	public:
	Interrupt(uint8_t intnum);

	static void setHandler(uint8_t intnum, avr_interrupt_handler handler, uint8_t mode);
	static void removeHandler(uint8_t intnum);
	static void enableGlobalInterrupts();
	static void disableGlobalInterrupts();

	void setHandler(avr_interrupt_handler handler, uint8_t mode);
	void removeHandler();
	
	uint8_t getInterruptNum();
};

};

#endif /* __AVRUTILPP_INTERRUPT_H__ */

