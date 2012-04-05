/*
 * AnalogPort.h
 * libavrutil++
 *
 * Created by Árpád Goretity on 04/04/2012.
 * Licensed under a CreativeCommons Attribution-ShareAlike 3.0 Unported License
 * http://creativecommons.org/licenses/by-sa/3.0/
 */

#ifndef __AVRUTILPP_ANALOGPORT_H__
#define __AVRUTILPP_ANALOGPORT_H__

#include <avrutil/avrutil.h>
#include "base.h"

namespace avrutil {

class AnalogPort {
	protected:
	uint8_t timer;
	uint8_t channel;
	public:
	AnalogPort(uint8_t pwmTimer, uint8_t adcChannel);
	uint16_t read();
	void write(uint16_t value);
	void stopPWM();
};

};

#endif /* __AVRUTILPP_ANALOGPORT_H__ */

