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

/** @file options.c Routines for coniguring the Butterfly. Implements an options
	menu, from which the user can select items to edit. Each item is served from
	a single routine, which has a specific format. A #UserTaskFnType function
	select between the various options routines.
*/

#include <avr/io.h>
#include <avr/pgmspace.h>
#include <inttypes.h>

#include "c_ext.h"
#include "button.h"
#include "lcd.h"
#include "clock.h"
#include "usart.h"
#include "bcd.h"

/*******************************************************************************
* Enumerations
********************************************************************************/

/** Enumeration of option item states */
typedef enum
{
	OPTIONS_START = 0,
	OPTIONS_MAIN_MENU,
	OPTIONS_EDIT,
} OptionsState;

/** Enumeration of option item actions */
typedef enum
{
	OPTION_ITEM_SHOW_LABEL = 0,
	OPTION_ITEM_START_EDIT,
	OPTION_ITEM_EDIT
} OptionsItemAction;

/*******************************************************************************
* Definitions
********************************************************************************/

/** @def OPTIONS_MAIN_MENU_ITEMS Number of items in the options menu */
#define OPTIONS_MAIN_MENU_ITEMS		3

/*******************************************************************************
* Flash Strings
********************************************************************************/

const char optionsLabel_F[] PROGMEM = "Options";
const char clockLabel_F[] PROGMEM = "Clock";

/*******************************************************************************
* Routines
********************************************************************************/

/** Configures the clock optinos (time format and date format).
	@param key Key pressed by the user. Passed as input from the Options main
		routine.
	@param action Action to perform (show label, start editing of parameters or
		edit a parameter). Passed as input from the Options main routine.
	@result FALSE to indicate that the user aborted the editing of the item.
*/
BOOL ClockOptions(char key, OptionsItemAction action)
{
	static uint8_t editState;

	if (action == OPTION_ITEM_SHOW_LABEL)
	{
		LcdWriteString_F(clockLabel_F);
		return TRUE;
	}
	if (action == OPTION_ITEM_START_EDIT)
	{
		editState = 0;
		ClockShowTimeFormat();
		return TRUE;
	}

	switch(key)
	{
		case KEY_UP:
		case KEY_DOWN:
			if (editState == 0)
				ClockToggleTimeFormat();
			else
				ClockChangeDateFormat(key == KEY_DOWN);
			break;
		case KEY_LEFT:
			if (editState == 0)
			{
				//LcdWriteString_F(clockLabel_F);
				return FALSE;
			}
			editState = 0;
			break;
		case KEY_RIGHT:
			if (editState == 0)
			{
				editState = 1;
			}
			break;
	}

	if (editState == 0)
		ClockShowTimeFormat();
	else
		ClockShowDateFormat();
	return TRUE;
}

/** Configures the LCD contrast.
	@param key Key pressed by the user. Passed as input from the Options main
		routine.
	@param action Action to perform (show label, start editing of parameters or
		edit a parameter). Passed as input from the Options main routine.
	@result FALSE to indicate that the user aborted the editing of the item.
*/
BOOL LCDContrastOptions(char key, OptionsItemAction action)
{
	uint8_t contrast;

	if (action == OPTION_ITEM_SHOW_LABEL)
	{
		LcdWriteString_F(PSTR("LCD Contrast"));
		return TRUE;
	}
	if (action == OPTION_ITEM_START_EDIT)
	{
		key = KEY_NULL;
	}

	contrast = LCD_GET_CONTRAST_LEVEL();

	switch(key)
	{
		case KEY_UP:
			if (contrast < LCD_MAXIMUM_CONTRAST)
				contrast++;
				LCD_SET_CONTRAST_LEVEL(contrast);
			break;
		case KEY_DOWN:
			if (contrast > LCD_MINIMUM_CONTRAST)
				contrast--;
				LCD_SET_CONTRAST_LEVEL(contrast);
			break;
		case KEY_LEFT:
			return FALSE;
	}

	contrast = CharToBCD2(contrast);

	LcdWriteChar(0, 'C');
	LcdWriteChar(1, 'T');
	LcdWriteChar(2, 'R');
	LcdWriteChar(3, ' ');
	LcdWriteChar(4, BCD_HIGH_NIBBLE_CHAR(contrast));
	LcdWriteChar(5, BCD_LOW_NIBBLE_CHAR(contrast));
	LcdWriteChar(6, '\0');

	return TRUE;
}

/** Switches the RS-232 communication on and off.
	@param key Key pressed by the user. Passed as input from the Options main
		routine.
	@param action Action to perform (show label, start editing of parameters or
		edit a parameter). Passed as input from the Options main routine.
	@result FALSE to indicate that the user aborted the editing of the item.
*/
BOOL RS232Options(char key, OptionsItemAction action)
{
	if (action == OPTION_ITEM_SHOW_LABEL)
	{
		LcdWriteString_F(PSTR("RS-232"));
		return TRUE;
	}
	if (action == OPTION_ITEM_START_EDIT)
	{
		key = KEY_NULL;
	}

	switch(key)
	{
		case KEY_UP:
		case KEY_DOWN:
			if (!IsUsartEnabled())
			{
				UsartInit(USART_BAUD_RATE_9600);
			}
			else
			{
				UsartDisable();
			}
			break;
		case KEY_LEFT:
				return FALSE;
			break;
	}

	if (IsUsartEnabled())
	{
		LcdWriteString_F(PSTR("On"));
	}
	else
	{
		LcdWriteString_F(PSTR("Off"));
	}
	return TRUE;
}

/** Selects one Options routine to run.
	@param optionsItem Number of item to run
	@param key Key pressed by the user. Passed as input from the Options main
		routine.
	@param action Action to perform (show label, start editing of parameters or
		edit a parameter). Passed as input from the Options main routine.
	@result FALSE if an invalid optionsItem is given
*/
BOOL RunOptionsItem(uint8_t optionsItem, char key, OptionsItemAction action)
{
	switch(optionsItem)
	{
		case 0:
			return ClockOptions(key, action);
		case 1:
			return LCDContrastOptions(key, action);
		case 2:
			return RS232Options(key, action);
		default:
			return FALSE;
	}
}

/** Options main routine. It is a #UserTaskFnType function and must be
	included in the #userTasksTable of the Scheduler module. It allows the user
	to edit the Options items.
*/
int8_t OptionsMain(char key, BOOL init)
{
	static OptionsState state;
	static uint8_t optionsItem;

	if (init)
	{
		state = OPTIONS_START;
		LcdWriteString_F(optionsLabel_F);
		return 0;
	}

	if (state == OPTIONS_START)
	{
		if (key == KEY_DOWN)
			return 1;
		else if (key == KEY_UP)
			return -1;
		else if (key == KEY_RIGHT)
		{
			state = OPTIONS_MAIN_MENU;
			// Label of first Options Item
			ClockOptions(KEY_NULL, OPTION_ITEM_SHOW_LABEL);
			optionsItem = 0;
		}
	}
	else if (state == OPTIONS_MAIN_MENU)
	{
		OptionsItemAction action = OPTION_ITEM_SHOW_LABEL;

		switch(key)
		{
			case KEY_DOWN:
				if (optionsItem == OPTIONS_MAIN_MENU_ITEMS - 1)
					optionsItem = 0;
				else
					optionsItem++;
				break;
			case KEY_UP:
				if (optionsItem == 0)
					optionsItem = OPTIONS_MAIN_MENU_ITEMS - 1;
				else
					optionsItem--;
				break;
			case KEY_LEFT:
				state = OPTIONS_START;
				LcdWriteString_F(optionsLabel_F);
				return 0;
			case KEY_RIGHT:
				state = OPTIONS_EDIT;
				action = OPTION_ITEM_START_EDIT;
				break;
		}

		RunOptionsItem(optionsItem, KEY_NULL, action);
	}
	else
	{
		BOOL result;

		result = RunOptionsItem(optionsItem, key, OPTION_ITEM_EDIT);

		if (!result)
		{
			state = OPTIONS_MAIN_MENU;
		}
	}
	return 0;
}
