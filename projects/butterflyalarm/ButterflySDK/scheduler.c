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

/** @file scheduler.c Scheduler Module. It implements a sequential cyclical
		menu, which contains all the user tasks. It defines a non ISR
		task that must be iteratively called by the main routine.
		Every time the scheduler task is run, a single user function is called.
		The user moves from one task to an other based on the return code of
		the user task functions.
*/

#include <inttypes.h>
#include "c_ext.h"
#include "button.h"
#include "scheduler.h"

/*******************************************************************************
* External Functions
*******************************************************************************/

extern int8_t MeasurementMain(char key, BOOL init);
extern int8_t ClockMain(char key, BOOL init);
extern int8_t DateMain(char key, BOOL init);
extern int8_t Alarm1Main(char key, BOOL init);
extern int8_t Alarm2Main(char key, BOOL init);
extern int8_t OptionsMain(char key, BOOL init);
extern int8_t CardMain(char key, BOOL init);
#ifdef __USE_DATAFLASH_TEST__
extern int8_t DataFlashTestMain(char key, BOOL init);
#endif // __USE_DATAFLASH_TEST__
extern int8_t PlaySongMain(char key, BOOL init);
extern int8_t PlayDataflashSongMain(char key, BOOL init);

/*******************************************************************************
* Static variables
*******************************************************************************/

/** A table with the user task function. The functions of this table form a
	sequential cyclical menu. The user can move from one task to the
	immediate previous or to the imediate following, based on the
	return code of the function.*/
UserTaskFnType userTasksTable[] = {	ClockMain,
									DateMain,
									Alarm1Main,
									Alarm2Main,
									MeasurementMain,
									OptionsMain,
									CardMain,
									PlaySongMain,
									PlayDataflashSongMain,
#ifdef __USE_DATAFLASH_TEST__
									DataFlashTestMain,
#endif // __USE_DATAFLASH_TEST__

									0};

/** The number of the current task in the #userTasksTable. */
int8_t currentTask;

/** The size of the #userTasksTable table. */
uint8_t statesTableSize;

/*******************************************************************************
* Functions
*******************************************************************************/

/** Initializes the scheduler module.*/
void UserTaskSchedulerInit(void)
{
	statesTableSize = 0;
	while(userTasksTable[statesTableSize]!=0)
		statesTableSize++;

	currentTask = 0;
	userTasksTable[0](KEY_NULL, TRUE);
}

/** Scheduler task. It must be called by the main routine iteratively.
	Precise timing isn't necessary. It invokes a single user task every
	time is called. The user moves from one task to an other based on the
	return code of the user task functions.
	@param key The key pressed by the user. #KEY_NULL if no key is pressed.
	*/
void UserTaskSchedulerRun(char key)
{
	int8_t taskResult;
	UserTaskFnType currentTaskFn = userTasksTable[currentTask];

	taskResult = currentTaskFn(key, FALSE);

	if (taskResult != 0)
	{

		currentTask+=taskResult;
		if (currentTask < 0)
			currentTask = statesTableSize - 1;
		if (currentTask >= statesTableSize)
			currentTask = 0;

		currentTaskFn = userTasksTable[currentTask];
		currentTaskFn(KEY_NULL, TRUE);
	}
}


