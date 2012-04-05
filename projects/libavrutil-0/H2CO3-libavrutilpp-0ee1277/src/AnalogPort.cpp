/*
 * AnalogPort.cpp
 * libavrutil++
 *
 * Created by Árpád Goretity on 04/04/2012.
 * Licensed under a CreativeCommons Attribution-ShareAlike 3.0 Unported License
 * http://creativecommons.org/licenses/by-sa/3.0/
 */

#include "AnalogPort.h"

static bool isPWMInitialized = false;
static bool isADCInitialized = false;

avrutil::AnalogPort::AnalogPort(uint8_t pwmTimer, uint8_t adcChannel)
{
	this->timer = pwmTimer;
	this->channel = adcChannel;
}

uint16_t avrutil::AnalogPort::read()
{
	if (!isADCInitialized)
	{
		avr_adc_init();
		isADCInitialized = true;
	}
	
	return avr_analog_read(this->channel);
}

void avrutil::AnalogPort::write(uint16_t value)
{
	if (!isPWMInitialized)
	{
		avr_pwm_init();
		isPWMInitialized = true;
	}
	
	avr_analog_write(this->timer, value);
}

void avrutil::AnalogPort::stopPWM()
{
	avr_pwm_stop(this->timer);
}

