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

/** @file card.c Electronic Card module. It provides functions for editing and
		displaying a name in the Butterfly's LCD screen. The card name can only
		contains alphanumeric characters (upper case only) or one of '+', '-'
		and ' '. The card name can be modified either through the module's
		user task (user key presses) or through the Communication module. The
		card name is stored in the EEPROM, so that it isn't lost when power
		is removed.
*/

#include <avr/io.h>
#include <avr/pgmspace.h>
#include <inttypes.h>

#include "c_ext.h"
#include "eeprom.h"
#include "button.h"
#include "lcd.h"
#include "card.h"

/*******************************************************************************
* Definitions
*******************************************************************************/

/* @def DISPLAY_SIZE Number of charactes the LCD screen can display */
#define DISPLAY_SIZE		6

/** Address of EEPROM where the card name is stored */
#define EEPROM_NAME_START_ADDRESS	0x100

/*******************************************************************************
* Static Variables
*******************************************************************************/

/** Number of character being edited. It is initialized to -1, which means that
	no editing is in progess. */
static int8_t editing;

/** Buffer in RAM to hold the card name. */
static char name[CARD_NAME_MAX_SIZE + 1];

/*******************************************************************************
* Exported Functions (to use with Comm module)
*******************************************************************************/

/** Initializes the card module */
void CardInit(void)
{
	editing = -1;
}

/** Tests whether the card name is being edited.
	@return TRUE if the card name is being edited by the user
*/
BOOL CardEditInProgress(void)
{
	return (editing != - 1);
}

/** Sets a character of the card name
	@param pos The position of the character to be set
	@param c New value of the character
*/
void CardSetNameChar(uint8_t pos, char c)
{
	if (pos < CARD_NAME_MAX_SIZE)
		name[pos] = c;
}

/** Stores the card name in the EEPROM */
void CardSaveName(void)
{
	EEPROMWriteBytes(name, CARD_NAME_MAX_SIZE, EEPROM_NAME_START_ADDRESS);
}

/*******************************************************************************
* Local Functions
*******************************************************************************/

/** Displays the card name in the LCD screen. If editing is in progress, then
	the character being edited flashes.
*/
void ShowCardName(void)
{
	if (editing ==-1)
	{
		LcdWriteString(name);
	}
	else
	{
		uint8_t i, firstChar;
		char c;

		if (editing < DISPLAY_SIZE)
			firstChar = 0;
		else
			firstChar = editing - DISPLAY_SIZE + 1;

		for(i=0; i<DISPLAY_SIZE && name[firstChar + i]; i++)
		{
			c = name[firstChar + i];
			if ((firstChar == 0 && i == editing) ||
				(firstChar > 0 && i == (DISPLAY_SIZE - 1)))
			{
				// Flash this character
				c = c | 0x80;
			}
			else
			{
				c = c & 0x7F; // In case of an invalid char
			}
			LcdWriteChar(i, c);
		}
		for(;i<DISPLAY_SIZE;i++)
		{
			LcdWriteChar(i, ' ');
		}
		LcdWriteChar(6, '\0');
	}
}

/** Finds the new value for a card name's character, based on user's input and
	the previous value of the character.
	@param key The key pressed by the user. Could be either KEY_UP or KEY_DOWN.
	@return The new value of the card name's character. It could only be an
			alphanumeric character (Upper case only) or one of '+','-' and ' '
*/
char GetNameModifiedChar(char key)
{
	// Allowed characters: [0-9],[A-Z],'+','-',' '
	char c = name[editing];

	if (c >= '0' && c <= '9')
	{
		if (key == KEY_UP)
			c++;
		else if (key == KEY_DOWN)
			c--;
		if (c == '0' - 1)
			return ' ';
		else if (c == '9' + 1)
			return 'A';
		return c;
	}
	if (c >= 'a' && c <= 'z')
	{
		// To upper case
		c = 'A' + c - 'a';
	}
	if (c >= 'A' && c <= 'Z')
	{
		if (key == KEY_UP)
			c++;
		else if (key == KEY_DOWN)
			c--;
		if (c == 'A' - 1)
			return '9';
		else if (c == 'Z' + 1)
			return '+';
		return c;
	}
	if (c == '+')
	{
		return key == KEY_UP ? '-' : 'Z';
	}
	if (c == '-')
	{
		return key == KEY_UP ? ' ' : '+';
	}
	if (c == ' ')
	{
		return key == KEY_UP ? '0' : '-';
	}

	return 'A';
}

/** Tests that the card name contains only the displayable characters. This
	operation is necessary in case of a corrupted EEPROM or invalid data sent
	by the Communication module. If an invalid character is encountered, it
	is replaced by a null character.
*/
void NameSanityCheck(void)
{
	// Allowed characters: [0-9],[A-Z],'+','-',' '
	uint8_t i;
	char c;

	for(i=0; i<CARD_NAME_MAX_SIZE && name[i]; i++)
	{
		c = name[i];

		if (c >= 'a' && c <= 'z')
		{
			// To upper case
			c = 'A' + c - 'a';
		}
		if ((c >= 'A' && c <= 'Z') ||
			(c >= '0' && c <= '9') ||
			(c == '+' || c == '-' || c == '_' || c == ' '))
		{
			continue;
		}
		name[i] = '\0';
		break;
	}
}

/*******************************************************************************
* Card Edit Task Function
*******************************************************************************/

/** Card editing main routine. It is a #UserTaskFnType function and must be
	included in the #userTasksTable of the Scheduler module. It displays the
	card name in the LCD screen and allows the user to modify it, by using the
	joystick.
	@param key The key pressed by the user
	@param init If is set to TRUE, it indicates that the editing module
				must be initialized
	@return 1 or -1 if the user selected to leave this task. A value of
			1 requests a move to the next menu item and a value of -1
			a move to the previous one. A value of 0 indicates that this
			task is still running. */
int8_t CardMain(char key, BOOL init)
{
	if (init)
	{
		EEPROMReadBytes(name, CARD_NAME_MAX_SIZE, EEPROM_NAME_START_ADDRESS);
		NameSanityCheck();
		name[CARD_NAME_MAX_SIZE] = '\0';
		editing = -1;
		ShowCardName();
		return 0;
	}

	if (editing == -1)
	{
		if (key == KEY_DOWN || key == KEY_UP)
		{
			return (key == KEY_DOWN) ? 1 : -1;
		}
		else if (key == KEY_ENTER)
		{
			editing = 0;
			ShowCardName();
		}
		return 0;
	}
	else
	{
		switch(key)
		{
		case KEY_ENTER:
			CardSaveName();
			editing = -1;
			break;
		case KEY_LEFT:
			if (editing > 0)
				editing--;
			break;
		case KEY_RIGHT:
			if (editing < CARD_NAME_MAX_SIZE - 1)
				editing++;
			break;
		case KEY_UP:
		case KEY_DOWN:
			name[editing] = GetNameModifiedChar(key);
			break;
		}
	}

	if (key != KEY_NULL)
		ShowCardName();

	return 0;
}

