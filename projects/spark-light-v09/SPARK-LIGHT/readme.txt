Project: SPARK-LIGHT v0.9
Author:  Yuri `x4mer` Nikiforov (user o5o5o5 at AVRfreaks)
Version: v0.9 (24 aug 2009)
License: GNU General Public License 

Description:
High power LED driver for 1W bike light. LED current stabilization is done 
using ADC and PWM. Efficiency is about 90%. 5 modes: 300/200/100/50 mA + 100 mA 
blink. Stabilizer control loop is very simple: ADC starts on the beginning of 
every PWM cycle and checks voltage on shunt, PWM duty cycle is adjusted if 
ADC reading differs from settings for currently selected mode. PWM is changed 
on one point every cycle taking saturation into account. So LED is slowly
turned on from smallest PWM value possible to the desired value. If batteries
are discharged so that they can't provide selected current mode the PWM value 
rises to maximum level and LED stays always on without pulses.

I'm using this light almost every day for 2 weeks now when I ride from work 
and the only problem I found with it is bad debouncing code. There is not much
about writing debounce routine, but when I think about desoldering, flashing,
soldering ATtiny13 back and testing it again I'm feeling sick. Maybe small 
capacitor on button input pin will do the trick in easier way.

Schematic is simplified to fit inside casing in replace of original driver.
LED is powered with pulsed current and care must be taken with respect to 
pulse current. It must not exceed LED specs or LED may degrade quickly. 
Maximum forward current for most 1W LEDs is about 500mA. In some cases 
schematic rework may be needed to make classical buck converter and provide 
LED with stable current instead of pulses. In case of this rework firmware 
may be kept unchanged.

Here is some test data obtained with scope and multimeter. Driver was powered 
from laboratory power supply during testing.

Idle power comsumption @6V			=	30uA
MCU power consumption  @6V (300mA mode)		=	2.5mA
Peak LED current       @6V			=	430mA
Average shunt voltage drop 300/200/100/50 	=     0.15/0.1/0.05/0.025V
MOSFET forward voltage drop (300mA mode)	=	0.17V



License:
    Copyright (C) 2009 Yuri `x4mer` Nikiforov

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
