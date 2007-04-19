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

/** @file scheduler.h Header file for the Scheduler Module.
*/

#ifndef __SCHEDULER_HEADER__
#define __SCHEDULER_HEADER__

/** Prototype fuction for the user task scheduler. It takes two arguments:
	the key pressed by the user and a flag indicating if this is the first
	time the task is started. It returns a code that indicates if the
	same task must be run by the scheduler the next time it will be invoked,
	or a move to the previous (-1) or the next (1) was requested.
	*/
typedef int8_t (*UserTaskFnType)(char key, BOOL init);

extern UserTaskFnType statesTable[];

extern void UserTaskSchedulerInit(void);
extern void UserTaskSchedulerRun(char key);

#endif //__SCHEDULER_HEADER__
