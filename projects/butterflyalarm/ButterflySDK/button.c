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

/** @file button.c Button module. The pin change interrupts are used to read
		the buttons. The module registers a task that performs the debouncing.
*/


#include <avr/io.h>
#include <avr/interrupt.h>
#include <inttypes.h>

#include "c_ext.h"
#include "button.h"
#include "sound.h"

/*******************************************************************************
* Definitions
*******************************************************************************/

#define PINB_MASK ((1<<PINB4)|(1<<PINB6)|(1<<PINB7))
#define PINE_MASK ((1<<PINE2)|(1<<PINE3))

#define BUTTON_A	6   // UP
#define BUTTON_B	7   // DOWN
#define BUTTON_C	2   // LEFT
#define BUTTON_D	3   // RIGHT
#define BUTTON_O	4   // PUSH


/*******************************************************************************
* Static Variables
*******************************************************************************/

/** A counter used for the debouncing. It is incremented every time the
	debouncing task runs. When it reachs the debounce stop value, debouncing
	stops and a new key press is allowed to be received.
	*/
volatile uint8_t buttonDebounceCounter;

/** TRUE if the module is in debouncing state. In this state new key presses
	are ignored.*/
volatile BOOL buttonDebounces;

/** The code of the last key pressed. */
volatile char keyCode = KEY_NULL;

/** TRUE when a new key is pressed. It is set back to FALSE when the
	ButtonGetKey is called. */
volatile BOOL keyEventPending = FALSE;

/*******************************************************************************
* Exported Functions
*******************************************************************************/

/** Initializes the button module. */
void ButtonInit(void)
{
	keyCode = KEY_NULL;
	keyEventPending = FALSE;
	buttonDebounces = FALSE;
	buttonDebounceCounter = 0;

	// Init port pins
	cbiBF(DDRB,7);
	cbiBF(DDRB,6);
	cbiBF(DDRB,4);
	PORTB |= PINB_MASK;
	DDRE = 0x00;
	PORTE |= PINE_MASK;

	// Enable pin change interrupt on PORTB and PORTE
	PCMSK0 = PINE_MASK;
	PCMSK1 = PINB_MASK;
	EIFR = (1<<PCIF0)|(1<<PCIF1);
	EIMSK = (1<<PCIE0)|(1<<PCIE1);
}

/** Returns the code of the last key pressed. The function doesn't buffer the
	key presses. Therefore if the function isn't called soon enough, some
	key presses can be lost.
	@result The code of the last key pressed. KEY_NULL if no key was pressed.
*/
char ButtonGetKey(void)
{
	char k;

	cli();

	if (keyEventPending)
	{
		k = keyCode;
		keyEventPending = FALSE;
	}
	else
		k = KEY_NULL;

	sei();

	return k;
}

/** Button debouncing task.
	It is called about every 62.5ms, whenever a LCD start of frame interrupt
	occurs.
*/
void ButtonDebounceTask(void)
{
	if (buttonDebounces)
	{
		buttonDebounceCounter++;
		if (buttonDebounceCounter > 1)
		{
			SoundStop();
		}
		if (buttonDebounceCounter > 2)
		{
			buttonDebounces = FALSE;
			buttonDebounceCounter = 0;
		}
	}
}

/*******************************************************************************
* Local Functions
*******************************************************************************/

/** Handling of pin change interrupt. Reads the PINE and PINB to determine
	which button triggered the interrupt. */
void PinChangeInterrupt(void)
{
	char buttons;
	char key;

	// Read the buttons:
	//
	// Bit			 7   6   5   4   3   2   1   0
	// ---------------------------------------------
	// PORTB		   B   A	   O
	// PORTE						   D   C
	// ---------------------------------------------
	// PORTB | PORTE   B   A	   O   D   C
	// =============================================

	buttons = (~PINB) & PINB_MASK;
	buttons |= (~PINE) & PINE_MASK;

	// Output virtual keys
	if (buttons & (1<<BUTTON_A))
		key = KEY_UP;
	else if (buttons & (1<<BUTTON_B))
		key = KEY_DOWN;
	else if (buttons & (1<<BUTTON_C))
		key = KEY_LEFT;
	else if (buttons & (1<<BUTTON_D))
		key = KEY_RIGHT;
	else if (buttons & (1<<BUTTON_O))
		key = KEY_ENTER;
	else
		key = KEY_NULL;

	if(key != KEY_NULL)
	{
		if(!buttonDebounces)
		{
			if (!keyEventPending)
			{
				keyCode = key;
				keyEventPending = TRUE;
			}
			buttonDebounces = TRUE;
			SoundStartTone(1000);
		}
	}

	// Delete pin change interrupt flags
	EIFR = (1<<PCIF1) | (1<<PCIF0);
}

// A newer version of GCC is required for this
//ISR_ALIAS(PCINT1_vect, PCINT0_vect);

/** @fn ISR(PCINT0_vect)
	INT0 pin change interrupt.*/
ISR(PCINT0_vect)
{
	PinChangeInterrupt();
}

/** @fn ISR(PCINT1_vect)
	INT1 pin change interrupt.*/
ISR(PCINT1_vect)
{
	PinChangeInterrupt();
}


