/*
 * Servo.cpp
 * libavrutil++
 *
 * Created by Árpád Goretity on 04/04/2012.
 * Licensed under a CreativeCommons Attribution-ShareAlike 3.0 Unported License
 * http://creativecommons.org/licenses/by-sa/3.0/
 */

#include "Servo.h"

static bool isInitialized = false;

avrutil::Servo::Servo(uint8_t timer)
{
	if (!isInitialized)
	{
		avr_servo_init();
		isInitialized = true;
	}
		
	this->timer = timer;
}

void avrutil::Servo::connect()
{
	avr_servo_attach(this->timer);
}

void avrutil::Servo::disconnect()
{
	avr_servo_detach(this->timer);
}

void avrutil::Servo::setPosition(uint16_t position)
{
	avr_servo_set_pos(this->timer, position);
}

uint16_t avrutil::Servo::getPosition()
{
	return avr_servo_get_pos(this->timer);
}

