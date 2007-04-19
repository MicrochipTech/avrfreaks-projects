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

/** @file clock.h Clock module header file. */

#ifndef __CLOCK_HEADER__
#define __CLOCK_HEADER__

/*******************************************************************************
* Definitions
*******************************************************************************/

#define CLOCK_FORMAT_12H	0
#define CLOCK_FORMAT_24H	1

#define DATE_EUROPE_FORMAT		0
#define DATE_AMERICAN_FORMAT	1
#define DATE_CANADIAN_FORMAT	2

/*******************************************************************************
* Enumerations
*******************************************************************************/

/** Enumeration of time editing states.*/
typedef enum {
	CLOCK_SHOW = 0,
	CLOCK_EDIT_HOUR,
	CLOCK_EDIT_MINUTE,
	CLOCK_EDIT_SECOND
} ClockTimeState;

/** Enumeration of date editing states.*/
typedef enum {
	DATE_SHOW = 10,
	DATE_EDIT_DAY = 0,
	DATE_EDIT_MONTH = 1,
	DATE_EDIT_YEAR = 2
} ClockDateState;

/*******************************************************************************
* Exported functions
*******************************************************************************/

extern void ClockInit(void);
extern void ClockTimeUpdate(void);
extern void ClockTimeEditInit(void);
extern void ClockTimeEdit(char key);
extern ClockTimeState ClockGetTimeState(void);
extern void ClockShowTimeFormat(void);
extern void ClockToggleTimeFormat(void);
extern uint8_t ClockGetTimeFormat(void);
extern void ClockDateUpdate(void);
extern void ClockDateEditInit(void);
extern void ClockDateEdit(char key);
extern ClockDateState ClockGetDateState(void);
extern void ClockShowDateFormat(void);
extern void ClockChangeDateFormat(BOOL next);

#endif // __CLOCK_HEADER__
