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

/** @file lcd.c LCD driver module. Provides functions for displaying text and
		symbols in the LCD screen. It implements a scrolling and a flashing
		mechanism. The driver uses double-buffering of the LCD data registers.
		The second buffer is stored in RAM and is updated by a non-interrupt
		task. The memory buffer is copied to the LCD registers in the LCD
		Frame Start ISR. The ISR is kept as small as possible (it only copies
		the memory buffer to the LCD registers) and all the module's
		functionality is implemented by the non-interrupt task.
*/

#include <avr/io.h>
#include <avr/pgmspace.h>
#include <inttypes.h>
#include <avr/interrupt.h>

#include "c_ext.h"
#include "lcd.h"

/*******************************************************************************
* Definitions
*******************************************************************************/

/** @def LCD_DIGIT_COUNT Number of digits in the LCD screen */
#define LCD_DIGIT_COUNT			6

/** @def LCD_TIMER_SEED Defines how many times the LCD ISR must be called before
	updating the LCD registers. A value of 3 means that the LCD registers will
	be updated every 3*62.5ms = 187.5ms or more, if no new data are present */
#define LCD_TIMER_SEED			3

/** @def LCD_REGISTER_COUNT Number of LCD registers. Device specific. */
#define LCD_REGISTER_COUNT		20

/** @def LCD_TEXT_BUFFER_SIZE Maximum number of characters to be written in the
	text buffer. */
#define LCD_TEXT_BUFFER_SIZE	25

/** @def LCD_FLASH_SCROLL_PERIOD Defines the flash-scroll update time. The
	update of the scrolling and flashing of characters is controlled by the
	LCD start of frame interrupt, which has a period of 62.5ms. Therefore a
	a value of 4 for the #LCD_FLASH_SCROLL_PERIOD corresponds to 250ms.*/
#define LCD_FLASH_SCROLL_PERIOD				4

/** @def LCD_START_UP_DELAY_BEFORE_SCROLLING Time to wait before starting
	scrolling text. The time in ms is given from the formula:
	#LCD_START_UP_DELAY_BEFORE_SCROLLING*#LCD_FLASH_SCROLL_PERIOD*250ms.
*/
#define LCD_START_UP_DELAY_BEFORE_SCROLLING	3

/*******************************************************************************
* Macros
*******************************************************************************/

// Special Symbols Bit Mappings
// LCD_SYMBOL_1		LCDDR0.2
// LCD_SYMBOL_2		LCDDR0.6
// LCD_SYMBOL_3		LCDDR3.0	(shared 1)
// LCD_SYMBOL_4		LCDDR1.1
// LCD_SYMBOL_5		LCDDR1.5
// LCD_SYMBOL_7		LCDDR18.0	(shared 2)
// LCD_SYMBOL_8		LCDDR18.0	(shared 1)
// LCD_SYMBOL_9		LCDDR2.2
// LCD_SYMBOL_10	LCDDR2.6

// LCD_SYMBOL_ARROW_1	LCDDR0.1
// LCD_SYMBOL_ARROW_2	LCDDR0.5
// LCD_SYMBOL_ARROW_3	LCDDR1.2
// LCD_SYMBOL_ARROW_4	LCDDR1.6
// LCD_SYMBOL_ARROW_5	LCDDR3.0	(shared 1)
// LCD_SYMBOL_ARROW_7	LCDDR13.0	(shared 3)
// LCD_SYMBOL_ARROW_8	LCDDR13.0	(shared 3)
// LCD_SYMBOL_ARROW_9	LCDDR2.1
// LCD_SYMBOL_ARROW_10	LCDDR2.5

#define LCD_COLON_SET		lcdData[8] |= 1
#define LCD_COLON_CLEAR		lcdData[8] &= ~1
#define LCD_SYMBOL_1_SET	lcdData[0] |= (1 << 2)
#define LCD_SYMBOL_1_CLEAR	lcdData[0] &= ~(1 << 2)
#define LCD_SYMBOL_2_SET	lcdData[0] |= (1 << 6)
#define LCD_SYMBOL_2_CLEAR	lcdData[0] &= ~(1 << 6)

#define LCD_SET_COLON(active) lcdData[8] = active

// DEVICE SPECIFIC!!! (ATmega169)
#define pLCDREG ((unsigned char *)(0xEC))

/*******************************************************************************
* Static variables
*******************************************************************************/

/** Union of module's status flags
*/
typedef union
{
	uint8_t	allFields;
	struct
	{
		/** Set to 1 by the LCD ISR when the memory buffer has been copied to
			the LCD registers. It is used to block further update of the
			memory buffer, until the previous values have been copied to the
			registers. */
		uint8_t updateComplete : 1;

		/** Set to 1 to inform the LCD ISR that the values of the memory buffer
			have changed. */
		uint8_t regUpdateRequired : 1;

		/** Set to 1 when scrolling is required. */
		uint8_t scrollRequired : 1;

		/** Set to 1 when flashing is required. */
		uint8_t flashRequired : 1;

		/** Set to 1 to inform the LCD non-interrupt task that the text data
			have changed and the memory buffer needs updating. */
		uint8_t newData : 1;

		/** Set to 1 when the colon must be displayed */
		uint8_t colon : 1;

		/** Set to 1 when the symbol 1 must be displayed */
		uint8_t symbol_1 : 1;

		/** Set to 1 when the symbol 2 must be displayed */
		uint8_t symbol_2 : 1;
	};
} LcdStatus;

/** LCD status flags */
static volatile LcdStatus lcdStatus;

/** Memory buffer for the LCD registers */
static char lcdData[LCD_REGISTER_COUNT];

/** Buffer that contains the text to be displayed. If the text is more than
	6 characters long, scrolling will be automatically enabled. Bit 7 of
	every byte indicates that the corresponding character must be flashing.
*/
static char lcdTextBuffer[LCD_TEXT_BUFFER_SIZE+1];

/** Used to time the update of the LCD registers. It starts from #LCD_TIMER_SEED
	and decrements every time the LCD ISR is called. When it reaches 0, the
	LCD registers are updated and the variable is set back to #LCD_TIMER_SEED
*/
static volatile uint8_t lcdUpdateTimer;

/** Counts invocations of LCD interrupts. Used for scroll/flash timing */
static volatile uint8_t lcdISRTimer;

/** Set to TRUE when a LCD_SOF interrupt occurs. Cleared when #LcdTask is
	called. Although it is declared global, external modules should only
	read it and not modify it. */
volatile BOOL lcdInterruptFlag;

/** Used to implement a start-up delay before scrolling a string over the LCD */
static uint8_t lcdStartDelayScrollTimer;

/** Defines the flashing of characters. The characters are displayed if
	it has a value of 0. Its value is being toggled by the #LcdTask */
static uint8_t lcdFlashOnOffState;

/** Used for selecting which part of the string stored in the text buffer
	will be displayed during scrolling */
static int8_t lcdScrollIndex;

/** Look-up table used when converting ASCII to LCD display data (segment
	control). Taken from ATMEL's LCD driver. */
unsigned int LCD_character_table[] PROGMEM =
{
	0x0A51,	 // '*' (?)
	0x2A80,	 // '+'
	0x0000,	 // ',' (Not defined)
	0x0A00,	 // '-'
	0x0A51,	 // '.' Degree sign
	0x0000,	 // '/' (Not defined)
	0x5559,	 // '0'
	0x0118,	 // '1'
	0x1e11,	 // '2
	0x1b11,	 // '3
	0x0b50,	 // '4
	0x1b41,	 // '5
	0x1f41,	 // '6
	0x0111,	 // '7
	0x1f51,	 // '8
	0x1b51,	 // '9'
	0x0000,	 // ':' (Not defined)
	0x0000,	 // ';' (Not defined)
	0x0000,	 // '<' (Not defined)
	0x0000,	 // '=' (Not defined)
	0x0000,	 // '>' (Not defined)
	0x0000,	 // '?' (Not defined)
	0x0000,	 // '@' (Not defined)
	0x0f51,	 // 'A' (+ 'a')
	0x3991,	 // 'B' (+ 'b')
	0x1441,	 // 'C' (+ 'c')
	0x3191,	 // 'D' (+ 'd')
	0x1e41,	 // 'E' (+ 'e')
	0x0e41,	 // 'F' (+ 'f')
	0x1d41,	 // 'G' (+ 'g')
	0x0f50,	 // 'H' (+ 'h')
	0x2080,	 // 'I' (+ 'i')
	0x1510,	 // 'J' (+ 'j')
	0x8648,	 // 'K' (+ 'k')
	0x1440,	 // 'L' (+ 'l')
	0x0578,	 // 'M' (+ 'm')
	0x8570,	 // 'N' (+ 'n')
	0x1551,	 // 'O' (+ 'o')
	0x0e51,	 // 'P' (+ 'p')
	0x9551,	 // 'Q' (+ 'q')
	0x8e51,	 // 'R' (+ 'r')
	0x9021,	 // 'S' (+ 's')
	0x2081,	 // 'T' (+ 't')
	0x1550,	 // 'U' (+ 'u')
	0x4448,	 // 'V' (+ 'v')
	0xc550,	 // 'W' (+ 'w')
	0xc028,	 // 'X' (+ 'x')
	0x2028,	 // 'Y' (+ 'y')
	0x5009,	 // 'Z' (+ 'z')
	0x0000,	 // '[' (Not defined)
	0x0000,	 // '\' (Not defined)
	0x0000,	 // ']' (Not defined)
	0x0000,	 // '^' (Not defined)
	0x0000	  // '_'
};

/** Converts an ASCII character to a LCD Segment Control Code and stores it in
	the #lcdData buffer. The #lcdData are latched to the LCD registers in the
	LCD interrupt.
	@param c The symbol to be displayed in a LCD digit
	@param digit The position (0-5), where the digit should be displayed. Digit
	0 is the first digit on the LCD
*/
void LcdWriteDigit(char c, char digit)
{
	unsigned int seg = 0x0000;  // Holds the segment pattern
	char mask, nibble;
	volatile char *ptr;
	char i;

	if (digit > 5)
		return;

	//Lookup character table for segmet data
	if ((c >= '*') && (c <= 'z'))
	{
		// c is a letter
		if (c >= 'a')
		{
			// Convert to upper case if necessarry
			c &= ~0x20;
		}
		c -= '*';

		seg = (unsigned int) pgm_read_word(&LCD_character_table[(uint8_t)c]);
	}

	// Adjust mask according to LCD segment mapping
	if (digit & 0x01)
		mask = 0x0F;	// Digit 1, 3, 5
	else
		mask = 0xF0;	// Digit 0, 2, 4

	ptr = lcdData + (digit >> 1);  // digit = {0,0,1,1,2,2}

	for (i = 0; i < 4; i++)
	{
		nibble = seg & 0x000F;
		seg >>= 4;
		if (digit & 0x01)
			nibble <<= 4;
		*ptr = (*ptr & mask) | nibble;
		ptr += 5;
	}
}

/*******************************************************************************
* LCD_SOF interrupt
*******************************************************************************/

/** @fn ISR(LCD_vect)
	LCD start of frame interrupt. The frame rate is 32Hz, but because the Low
	Power Waveform is selected, the ISR is called every second frame, that is
	every 62.5ms. Its only job is to update the LCD registers with the values
	stored in the memory buffer (#lcdData)
*/
ISR(LCD_vect)
{
	lcdInterruptFlag = TRUE;
	lcdISRTimer++;
	lcdUpdateTimer--;

	if(lcdUpdateTimer == 0 )
	{
		if (lcdStatus.regUpdateRequired == 1)
		{
			uint8_t i;

			lcdUpdateTimer = LCD_TIMER_SEED;

			// Copy display data buffer to I/O segment registers.
			for (i = 0; i < LCD_REGISTER_COUNT; i++)
				*(pLCDREG + i) = *(lcdData+i);

			lcdStatus.updateComplete = 1;
			lcdStatus.regUpdateRequired = 0;
		}
		else
		{
			// LCD register update blocked by lcdRegUpdateRequired.
			// Re-try next time.
			lcdUpdateTimer = 1;
		}
	}
}

/*******************************************************************************
* LCD task
*******************************************************************************/

/** Helper routine used by the #LcdTask that starts the updating of the memory
	buffer.
*/
void LcdStartMemBufferUpdate(void)
{
	// Wait for previous update to complete (from the LCD_SOF ISR)
	while(!lcdStatus.updateComplete);
	lcdStatus.updateComplete = 0;

	// Disable LCD start of frame interrupt
	LCDCRA &= ~(1<<LCDIE);
}

/** Helper routine used by the #LcdTask that finalizes the updating of the memory
	buffer.
*/
void LcdEndMemBufferUpdate(void)
{
	// Update symbols
	if (lcdStatus.colon)
		LCD_COLON_SET;
	else
		LCD_COLON_CLEAR;

	if (lcdStatus.symbol_1)
		LCD_SYMBOL_1_SET;
	else
		LCD_SYMBOL_1_CLEAR;

	if (lcdStatus.symbol_2)
		LCD_SYMBOL_2_SET;
	else
		LCD_SYMBOL_2_CLEAR;

	// Enable LCD start of frame interrupt
	LCDCRA |= (1<<LCDIE);

	// Inform the LCD_SOF ISR that an update to the LCD registers is needed
	lcdStatus.updateComplete = 0;
	lcdStatus.regUpdateRequired = 1;
}

/** LCD module's non-interrupt task. It must be called by the main routine
	iteratively. It implements all the module's functionality (conversion of
	ASCII characters to LCD Segment Control Code, scrolling, flashing).
	Scrolling and flashing can't happen at the same time. Scrolling has a
	higher priority, i.e. if scrolling is enabled no character is allowed to
	flash. Scrolling and flashing share the same timing mechanism, implemented
	by the #lcdISRTimer variable and the #LCD_FLASH_SCROLL_PERIOD constant.
*/
void LcdTask(void)
{
	BOOL scrollUpdate, flashUpdate;

	lcdInterruptFlag = FALSE;

	scrollUpdate = FALSE;
	flashUpdate = FALSE;
	if (lcdISRTimer >= LCD_FLASH_SCROLL_PERIOD)
	{
		lcdISRTimer = 0;
		scrollUpdate = TRUE;
		flashUpdate = TRUE;
	}

	if (lcdStatus.newData)
	{
		uint8_t i;
		char c;

		lcdStatus.flashRequired = 0;

		LcdStartMemBufferUpdate();

		for(i=0; i<LCD_DIGIT_COUNT; i++)
		{
			c = lcdTextBuffer[i];
			LcdWriteDigit(c & 0x7F, i);
			if (c & 0x80)
				lcdStatus.flashRequired = 1;
		}

		LcdEndMemBufferUpdate();

		if (lcdStatus.scrollRequired)
		{
			lcdStatus.flashRequired = 0;
			lcdScrollIndex = 0;
			lcdStartDelayScrollTimer = LCD_START_UP_DELAY_BEFORE_SCROLLING;
		}
		lcdFlashOnOffState = 0;
		lcdStatus.newData = 0;
	}
	else if (scrollUpdate && lcdStatus.scrollRequired)
	{
		uint8_t i, endOfLine = 0;
		char c;

		if (lcdStartDelayScrollTimer > 0)
		{
			lcdStartDelayScrollTimer--;
		}
		else
		{
			LcdStartMemBufferUpdate();

			for (i = 0; i<LCD_DIGIT_COUNT; i++)
			{
				if ((lcdScrollIndex+i) >= 0 && (!endOfLine))
				{
					// We have some visible characters
					c = lcdTextBuffer[i + lcdScrollIndex];
					c = c & 0x7F;

					if (c == '\0')
						endOfLine = i+1;
				}
				else
					c = ' ';

				LcdWriteDigit(c, i);
			}

			LcdEndMemBufferUpdate();

			// If the text scrolled off the display,
			// we have to start over again.
			if (endOfLine == 1)
				lcdScrollIndex = -LCD_DIGIT_COUNT;
			else
				lcdScrollIndex++;
		}
	}
	else if (flashUpdate && lcdStatus.flashRequired)
	{
		uint8_t i;
		char c;

		lcdFlashOnOffState^=1;

		LcdStartMemBufferUpdate();

		for (i=0; i<LCD_DIGIT_COUNT; i++)
		{
			c = lcdTextBuffer[i];

			if ((c & 0x80) && lcdFlashOnOffState)
				c = ' ';
			LcdWriteDigit(c & 0x7F, i);
		}

		LcdEndMemBufferUpdate();
	}
}

/*******************************************************************************
* Exported Functions
*******************************************************************************/

/** Initializes the LCD module. */
void LcdInit (void)
{
	LCD_SET_CONTRAST_LEVEL(LCD_INITIAL_CONTRAST);

	// Select asynchronous clock source, 1/4 Duty Cycle,
	// 1/3 Bias, enable all COM pins and
	// enable all segment pins
	LCDCRB = (1<<LCDCS) | (3<<LCDMUX0) | (7<<LCDPM0);

	// Set LCD prescaler to give a framerate of 32,0 Hz
	LCDFRR = (0<<LCDPS0) | (7<<LCDCD0);

	// Enable LCD and set low power waveform
	LCDCRA = (1<<LCDEN) | (1<<LCDAB);

	// Enable LCD start of frame interrupt
	LCDCRA |= (1<<LCDIE);

	// Variable initialization
	lcdUpdateTimer = LCD_TIMER_SEED;
	lcdStartDelayScrollTimer = 0;
	lcdISRTimer = 0;

	lcdStatus.allFields = 0;
	lcdStatus.updateComplete = 1;
	lcdFlashOnOffState = 0;

	lcdInterruptFlag = FALSE;

	LcdAllSegments(FALSE);
}

/** Shows or hide all all LCD segments on the LCD
	@param show If TRUE all segments of the LCD are turned on
*/
void LcdAllSegments(BOOL show)
{
	uint8_t i;

	if (show)
		show = 0xFF;

	// Set/clear all bits in all LCD registers
	for (i=0; i < LCD_REGISTER_COUNT; i++)
		*(lcdData + i) = show;

	lcdStatus.newData = 1;
}

/** Writes one character to the LCD text buffer. This function doesn't
	automatically trigger the update of the LCD memory buffer, unless
	a null character is written.
	@param digit Position in the text buffer to write the character to
	@param character Character to write
*/
void LcdWriteChar(uint8_t digit, char character)
{
	if (digit < LCD_TEXT_BUFFER_SIZE)
		lcdTextBuffer[digit] = character;

	lcdStatus.scrollRequired = 0;

	if (character == 0)
		lcdStatus.newData = 1;
}

/** Writes a RAM string to the LCD text buffer. If the length of the string
	is more than 6, scrolling will be enabled. The function triggers the
	update of the LCD memory buffer.
	@param str The string to write to the text buffer
*/
void LcdWriteString(const char *str)
{
	uint8_t i;

	for (i = 0; str[i] && (i < LCD_TEXT_BUFFER_SIZE); i++)
	{
		lcdTextBuffer[i] = str[i];
	}
	lcdTextBuffer[i] = '\0';

	if (i > LCD_DIGIT_COUNT)
		lcdStatus.scrollRequired = 1;
	else
		lcdStatus.scrollRequired = 0;
	lcdStatus.newData = 1;

	// Fill unused digits with '\0'
	for (i=i+1;i<=LCD_DIGIT_COUNT;i++)
		lcdTextBuffer[i] = '\0';
}

/** Writes a flash string to the LCD text buffer. If the length of the string
	is more than 6, scrolling will be enabled. The function triggers the
	update of the LCD memory buffer.
	@param str_F The flash string to write to the text buffer
*/
void LcdWriteString_F(const char *str_F)
{
	uint8_t i;
	char c;

	for (i=0; i<LCD_TEXT_BUFFER_SIZE; i++)
	{
		c=(const char)(pgm_read_byte(&str_F[i]));
		if (!c)
			break;
		lcdTextBuffer[i] = c;

	}
	lcdTextBuffer[i] = '\0';

	if (i > LCD_DIGIT_COUNT)
		lcdStatus.scrollRequired = 1;
	else
		lcdStatus.scrollRequired = 0;
	lcdStatus.newData = 1;

	// Fill unused digits with '\0'
	for (i=i+1;i<=LCD_DIGIT_COUNT;i++)
		lcdTextBuffer[i] = '\0';
}

/** Shows or hides the colon in the LCD screen.
	@param show If TRUE the colon will turn on
*/
void LcdColon(BOOL show)
{
	lcdStatus.colon = show ? 1 : 0;
	lcdStatus.newData = 1;
}

/** Shows or hides a symbol in the LCD screen
	@param symbol Number of symbol to show/hide
	@param show If TRUE the symbol will turn on
*/
void LcdShowSymbol(uint8_t symbol, BOOL show)
{
	if (symbol == 0)
	{
		lcdStatus.symbol_1 = show ? 1 : 0;
	}
	else
	{
		lcdStatus.symbol_2 = show ? 1 : 0;
	}
	lcdStatus.newData = 1;
}
