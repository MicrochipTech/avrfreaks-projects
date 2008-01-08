/*
             BUTTLOAD - Butterfly ISP Programmer

              Copyright (C) Dean Camera, 2007.
              
             dean [at] fourwalledcubicle [dot] com
                  www.fourwalledcubicle.com

           Released under the GPL Licence, Version 2.
*/

/*
	This is a basic driver for the Butterfly LCD. It offers the ability to
	change the contrast and display strings (scrolling or static) from flash
	or SRAM memory only.
	
	This has been completly rewritten from the Atmel code; in this version, as
	much processing as possible is performed by the string display routines
	rather than the interrupt so that the interrupt executes as fast as possible.
*/

#define INC_FROM_DRIVER
#include "LCD_Driver.h"

//                                 LCD Text            + Nulls for scrolling + Null Termination
static          char    TextBuffer[LCD_TEXTBUFFER_SIZE + LCD_DISPLAY_SIZE    + 1] = {};
static          uint8_t StrStart        = 0;
static volatile uint8_t StrEnd          = 0; // Volatile here because it saves space in the ISR
static          uint8_t ScrollCount     = 0;
static volatile uint8_t UpdateDisplay   = FALSE;
       volatile uint8_t ScrollFlags     = 0;

const           uint16_t LCD_SegTable[] PROGMEM =
{
    0xEAA8,     // '*'
    0x2A80,     // '+'
    0x4000,     // ','
    0x0A00,     // '-'
    0x0A51,     // '.' Degree sign
    0x4008,     // '/'
    0x5559,     // '0'
    0x0118,     // '1'
    0x1e11,     // '2
    0x1b11,     // '3
    0x0b50,     // '4
    0x1b41,     // '5
    0x1f41,     // '6
    0x0111,     // '7
    0x1f51,     // '8
    0x1b51,     // '9'
    0x0000,     // ':' (Not defined)
    0x0000,     // ';' (Not defined)
    0x8008,     // '<'
    0x1A00,     // '='
    0x4020,     // '>'
    0x0000,     // '?' (Not defined)
    0x0000,     // '@' (Not defined)
    0x0f51,     // 'A' (+ 'a')
    0x3991,     // 'B' (+ 'b')
    0x1441,     // 'C' (+ 'c')
    0x3191,     // 'D' (+ 'd')
    0x1e41,     // 'E' (+ 'e')
    0x0e41,     // 'F' (+ 'f')
    0x1d41,     // 'G' (+ 'g')
    0x0f50,     // 'H' (+ 'h')
    0x2080,     // 'I' (+ 'i')
    0x1510,     // 'J' (+ 'j')
    0x8648,     // 'K' (+ 'k')
    0x1440,     // 'L' (+ 'l')
    0x0578,     // 'M' (+ 'm')
    0x8570,     // 'N' (+ 'n')
    0x1551,     // 'O' (+ 'o')
    0x0e51,     // 'P' (+ 'p')
    0x9551,     // 'Q' (+ 'q')
    0x8e51,     // 'R' (+ 'r')
    0x9021,     // 'S' (+ 's')
    0x2081,     // 'T' (+ 't')
    0x1550,     // 'U' (+ 'u')
    0x4448,     // 'V' (+ 'v')
    0xc550,     // 'W' (+ 'w')
    0xc028,     // 'X' (+ 'x')
    0x2028,     // 'Y' (+ 'y')
    0x5009,     // 'Z' (+ 'z')
    0x1441,     // '['
    0x8020,     // '\'
    0x1111,     // ']'
    0x0000,     // '^' (Not defined)
    0x1000      // '_'
};

// ======================================================================================

/*
 NAME:      | LCD_Init
 PURPOSE:   | Initializes the Butterfly's LCD for correct operation, ready to display data
 ARGUMENTS: | None
 RETURNS:   | None
*/
void LCD_Init(void)
{
	// Set the initial contrast level to maximum:
	LCD_CONTRAST_LEVEL(0x0F);

    // Select asynchronous clock source, enable all COM pins and enable all segment pins:
    LCDCRB  = (1<<LCDCS) | (3<<LCDMUX0) | (7<<LCDPM0);

    // Set LCD prescaler to give a framerate of 64Hz:
    LCDFRR  = (0<<LCDPS0) | (3<<LCDCD0);    

	// Enable LCD and set low power waveform, enable start of frame interrupt:
    LCDCRA  = (1<<LCDEN) | (1<<LCDAB) | (1<<LCDIE);
}

/*
 NAME:      | LCD_PutStr_f
 PURPOSE:   | Displays a string from flash onto the Butterfly's LCD
 ARGUMENTS: | Pointer to the start of the flash string
 RETURNS:   | None
*/
void LCD_PutStr_f(const char *FlashData)
{
	/* Rather than create a new buffer here (wasting RAM), the TextBuffer global
	   is re-used as a temp buffer. Once the ASCII data is loaded in to TextBuffer,
	   LCD_puts is called with it to post-process it into the correct format for the
	   LCD interrupt.                                                                */

	strcpy_P((char*)&TextBuffer[0], FlashData);
	LCD_PutStr((char*)&TextBuffer[0]);
}

/*
 NAME:      | LCD_PutStr
 PURPOSE:   | Displays a string from SRAM onto the Butterfly's LCD
 ARGUMENTS: | Pointer to the start of the SRAM string
 RETURNS:   | None
*/
void LCD_PutStr(const char *Data)
{
	uint8_t LoadB       = 0;
	uint8_t CurrByte;

	do
	{
		CurrByte = *(Data++);
		
		switch (CurrByte)
		{
			case 'a'...'z':
				CurrByte &= ~(1 << 5);                   // Translate to upper-case character
			case '*'...'_':	                             // Valid character, load it into the array
				TextBuffer[LoadB++] = (CurrByte - '*');
				break;
			case 0x00:                                   // Null termination of the string - ignore for now so the nulls can be appended below
				break;
			default:                                     // Space or invalid character, use 0xFF to display a blank
				TextBuffer[LoadB++] = LCD_SPACE_OR_INVALID_CHAR;
		}
	}
	while (CurrByte && (LoadB < LCD_TEXTBUFFER_SIZE));

	ScrollFlags = ((LoadB > LCD_DISPLAY_SIZE)? LCD_FLAG_SCROLL : 0x00);

	for (uint8_t Nulls = 0; (Nulls < LCD_DISPLAY_SIZE + 1); Nulls++)
	  TextBuffer[LoadB++] = LCD_SPACE_OR_INVALID_CHAR;  // Load in nulls to ensure that when scrolling, the display clears before wrapping
	
	TextBuffer[LoadB] = 0x00;                           // Null-terminate string
	
	StrStart      = 0;
	StrEnd        = LoadB;
	ScrollCount   = LCD_SCROLLCOUNT_DEFAULT + LCD_DELAYCOUNT_DEFAULT;
	UpdateDisplay = TRUE;
}

/*
 NAME:      | LCD_Bargraph
 PURPOSE:   | Displays a bargraph of the specified length onto the Butterfly's LCD (non persistant)
 ARGUMENTS: | Number of segments from 0 to LCD_BARGRAPH_SIZE
 RETURNS:   | None
*/
void LCD_Bargraph(const uint8_t Segments)
{
	switch (Segments)
	{
		default:
			LCDDR3 |= (1 << 0);
		case 4:
			LCDDR1 |= (1 << 6);
		case 3:
			LCDDR1 |= (1 << 2);
		case 2:
			LCDDR0 |= (1 << 5);
		case 1:
			LCDDR0 |= (1 << 1);
			break;
		case 0:
			LCDDR3 &= ~(1 << 0);
			LCDDR1 &= ~((1 << 6) | (1 << 2));
			LCDDR0 &= ~((1 << 5) | (1 << 1));
	}    
}

/*
 NAME:      | LCD_WriteChar (static, inline)
 PURPOSE:   | Routine to write a character to the correct LCD registers for display
 ARGUMENTS: | Character to display, LCD character number to display character on
 RETURNS:   | None
*/
static inline void LCD_WriteChar(const uint8_t Byte, const uint8_t Digit)
{
	uint8_t* BuffPtr = (uint8_t*)(LCD_LCDREGS_START + (Digit >> 1));
	uint16_t SegData = 0x0000;

	if (Byte != LCD_SPACE_OR_INVALID_CHAR)              // Null indicates invalid character or space
	  SegData = pgm_read_word(&LCD_SegTable[Byte]);	

	for (uint8_t BNib = 0; BNib < 4; BNib++)
	{
		uint8_t MaskedSegData = (SegData & 0x0000F);

		if (Digit & 0x01)
		  *BuffPtr = ((*BuffPtr & 0x0F) | (MaskedSegData << 4));
		else
		  *BuffPtr = ((*BuffPtr & 0xF0) | MaskedSegData);

		BuffPtr  += 5;
		SegData >>= 4;
	}	
}

/*
 NAME:      | LCD_vect (ISR, blocking)
 PURPOSE:   | ISR to handle the display and scrolling of the current display string onto the LCD
 ARGUMENTS: | None
 RETURNS:   | None
*/
ISR(LCD_vect, ISR_BLOCK)
{
	if (ScrollFlags & LCD_FLAG_SCROLL)
	{
		if (!(ScrollCount--))
		{
			UpdateDisplay = TRUE;
			ScrollCount   = LCD_SCROLLCOUNT_DEFAULT;
		}
	}

	if (UpdateDisplay)
	{
		for (uint8_t Character = 0; Character < LCD_DISPLAY_SIZE; Character++)
		{
			uint8_t Byte = (StrStart + Character);

			if (Byte >= StrEnd)
			  Byte -= StrEnd;
			
			LCD_WriteChar(TextBuffer[Byte], Character);
		}
		
		if ((StrStart + LCD_DISPLAY_SIZE) == StrEnd)    // Done scrolling message on LCD once
		  ScrollFlags |= LCD_FLAG_SCROLL_DONE;
		
		if (StrStart == StrEnd)
		  StrStart = 1;
		else
		  StrStart++;

		UpdateDisplay  = FALSE;                         // Clear LCD management flags, LCD update is complete
	}
}

