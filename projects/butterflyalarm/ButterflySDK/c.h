/*  This file is a part of ButterflySDK. ButterflySDK is a set of software
	modules used to program the Butterfly Evaluation Board for ATMEL.
	Emphasis is given in code re-use and in maintainability. ButterflySDK
	is compiled with GCC AVR compiler. Supporting tools are written in Python
	programming language.
	Copyright (C) 2007, Kostas Giannakakis

    ButterflySDK is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    ButterflySDK is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with ButterflySDK; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

/** @file rtc.h RTC driver header file.
*/

#ifndef __RTC_HEADER__
#define __RTC_HEADER__

#include "c_ext.h"

/*******************************************************************************
* Exported Functions
*******************************************************************************/

extern void RtcInit(void);
extern void RtcStart(void);
extern void RtcStop(void);

extern uint8_t RtcRegisterListener(void);
extern BOOL RtcNewEvent(uint8_t listenerId);
extern void RtcUnregisterListener(uint8_t listenerId);

extern void RtcSetSecond(uint8_t second);
extern uint8_t RtcGetSecond(void);
extern void RtcSetMinute(uint8_t minute);
extern uint8_t RtcGetMinute(void);
extern void RtcSetHour(uint8_t hour);
extern uint8_t RtcGetHour(void);
extern void RtcSetDay(uint8_t day);
extern uint8_t RtcGetDay(void);
extern void RtcSetMonth(uint8_t month);
extern uint8_t RtcGetMonth(void);
extern void RtcSetYear(uint8_t year);
extern uint8_t RtcGetYear(void);
extern BOOL RtcIsLeapYear(void);

#endif // __RTC_HEADER__
