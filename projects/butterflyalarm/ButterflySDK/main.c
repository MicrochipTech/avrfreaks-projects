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

/** @file main.c Main file. It includes the main routine.
*/

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <avr/sleep.h>
#include <inttypes.h>

#include "c_ext.h"
#include "usart.h"
#include "button.h"
#include "timer0.h"
#include "lcd.h"
#include "clock.h"
#include "sensors.h"
#include "scheduler.h"
#include "hw.h"
#include "card.h"
#include "dfSongs.h"
#include "comm.h"
#include "sound.h"
#include "alarm.h"

/*******************************************************************************
* Main Routine
*******************************************************************************/

/**
	Main routine
	@result The main routine never returns.
*/
int main(void)
{
	char key;

	HWInit();

	//UsartInit(USART_BAUD_RATE_9600);

	ClockInit();
	SensorsInit();
	CardInit();
	CommInit();
	DfSongsInit();
	AlarmInit();

	UserTaskSchedulerInit();

	sei();

	for (;;)
	{
		key = ButtonGetKey();

		if (AlarmIsOn())
		{
			if (key != KEY_NULL)
			{
				AlarmStop();
			}
			UserTaskSchedulerRun(KEY_NULL);
		}
		else
		{
			UserTaskSchedulerRun(key);
		}

		if (!UsartNewDataIn() && !lcdInterruptFlag && !TMR0_INTERRUPT_FLAG())
		{
			PowerSave();
		}

		if (lcdInterruptFlag)
		{
			LcdTask();
			ButtonDebounceTask();
		}

		if (UsartNewDataIn())
		{
			CommTask();
		}

		AlarmCompareTask();

		if (TMR0_INTERRUPT_FLAG())
		{
			TMR0_CLEAR_INTERRUPT_FLAG();
			DfSongsTask();
		}
	}

	return 0;
}

/** @mainpage
	@section intro Introduction
	ButterflySDK is a software project that aims to provide high quality,
	reusable code for the ATMEL's <a href="http://www.atmel.com/dyn/products/tools_card.asp?tool_id=3146">
	AVR Butterfly Evaluation Kit</a>.
*/
