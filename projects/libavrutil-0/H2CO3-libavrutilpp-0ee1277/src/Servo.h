/*
 * Servo.h
 * libavrutil++
 *
 * Created by Árpád Goretity on 04/04/2012.
 * Licensed under a CreativeCommons Attribution-ShareAlike 3.0 Unported License
 * http://creativecommons.org/licenses/by-sa/3.0/
 */

#ifndef __AVRUTILPP_SERVO_H__
#define __AVRUTILPP_SERVO_H__

#include <avrutil/avrutil.h>

namespace avrutil {

class Servo {
	protected:
	uint8_t timer;
	public:
	Servo(uint8_t timer);

	void connect();
	void disconnect();

	void setPosition(uint16_t position);
	uint16_t getPosition();
};

};

#endif /* __AVRUTILPP_SERVO_H__ */

