//
//  Author(s)...: ATMEL Norway
//
//  Target(s)...: ATmega169
//
//  mt - used for debugging only - may not work

//  Include files
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <inttypes.h>
#include "lcd_driver.h"
#include "lcd_functions.h"
#include "BCD.h"
//#include "main.h"


#define FALSE   0
#define TRUE    (!FALSE)

char CONTRAST = LCD_INITIAL_CONTRAST;

// Start-up delay before scrolling a string over the LCD. "LCD_driver.c"
extern char gLCD_Start_Scroll_Timer;

/****************************************************************************
*
*	Function name : LCD_puts_f
*
*	Returns :		None
*
*	Parameters :	pFlashStr: Pointer to the string in flash
*                   scrollmode: Not in use
*
*	Purpose :		Writes a string stored in flash to the LCD
*
*****************************************************************************/

// mt void LCD_puts_f(char __flash *pFlashStr, char scrollmode)
void LCD_puts_f(const char *pFlashStr, char scrollmode)
{
    // char i;
	uint8_t i;

    while (gLCD_Update_Required);      // Wait for access to buffer

    // mt: for (i = 0; pFlashStr[i] && i < TEXTBUFFER_SIZE; i++)
	for (i = 0; pgm_read_byte(&pFlashStr[i]) && i < (TEXTBUFFER_SIZE-1); i++)
    {
        // mt: gTextBuffer[i] = pFlashStr[i];
		gTextBuffer[i] = pgm_read_byte(&pFlashStr[i]);
    }

    gTextBuffer[i] = '\0';

    if (i > 6)
    {
        gScrollMode = 1;        // Scroll if text is longer than display size
        gScroll = 0;
        gLCD_Start_Scroll_Timer = 3;    //Start-up delay before scrolling the text
    }
    else
    {
        gScrollMode = 0;        
        gScroll = 0;
    }

    gLCD_Update_Required = 1;
}


/****************************************************************************
*
*	Function name : LCD_puts
*
*	Returns :		None
*
*	Parameters :	pStr: Pointer to the string
*                   scrollmode: Not in use
*
*	Purpose :		Writes a string to the LCD
*
*****************************************************************************/
void LCD_puts(char *pStr, char scrollmode)
{
	uint8_t i; // char i;
	
	while (gLCD_Update_Required);      // Wait for access to buffer

    for (i = 0; pStr[i] && i < (TEXTBUFFER_SIZE - 1); i++) 
    {
        gTextBuffer[i] = pStr[i];
    }

    gTextBuffer[i] = '\0';

    if (i > 6)
    {
        gScrollMode = 1;        // Scroll if text is longer than display size
        gScroll = 0;
        gLCD_Start_Scroll_Timer = 3;    //Start-up delay before scrolling the text
    }
    else
    {
        gScrollMode = 0;        
        gScroll = 0;
    }

    gLCD_Update_Required = 1;
}


/****************************************************************************
*
*	Function name : LCD_putc
*
*	Returns :		None
*
*	Parameters :	digit: Which digit to write on the LCD
*                   character: Character to write
*
*	Purpose :		Writes a character to the LCD
*
*****************************************************************************/
// mt void LCD_putc(char digit, char character)
// mt void LCD_putc(char digit, char character)
void LCD_putc(uint8_t digit, char character)
{
    if (digit < TEXTBUFFER_SIZE)
        gTextBuffer[digit] = character;
		
}


/****************************************************************************
*
*	Function name : LCD_Clear
*
*	Returns :		None
*
*	Parameters :	None
*
*	Purpose :		Clear the LCD
*
*****************************************************************************/
void LCD_Clear(void)
{
    uint8_t i; // char i;
	   
    for (i=0; i<(TEXTBUFFER_SIZE-1); i++)
        gTextBuffer[i] = ' '; // JP changed from ' ' to stop LCD_puts_f buffer overrun
}


/****************************************************************************
*
*	Function name : LCD_Colon
*
*	Returns :		None
*
*	Parameters :	show: Enables the colon if TRUE, disable if FALSE
*
*	Purpose :		Enable/disable colons on the LCD
*
*****************************************************************************/
void LCD_Colon(char show)
{
    gColon = show;
}


/****************************************************************************
*
*	Function name : LCD_UpdateRequired
*
*	Returns :		None
*
*	Parameters :	update: TRUE/FALSE
*                   scrollmode: not in use
*
*	Purpose :		Tells the LCD that there is new data to be presented
*
*****************************************************************************/
void LCD_UpdateRequired(char update, char scrollmode)
{

    while (gLCD_Update_Required);
    
    gScrollMode = scrollmode;
    gScroll = 0;

    gLCD_Update_Required = update;
}


/****************************************************************************
*
*	Function name : LCD_FlashReset
*
*	Returns :		None
*
*	Parameters :	None
*
*	Purpose :		This function resets the blinking cycle of a flashing digit
*
*****************************************************************************/
void LCD_FlashReset(void)
{
    gFlashTimer = 0;
}


/****************************************************************************
*
*	Function name : SetContrast
*
*
*   Parameters :    char input from PC
*
*	Purpose :		Adjust the LCD contrast
*
*****************************************************************************/



void SetContrast(char input)
{

    LCD_Clear();

    LCD_putc(0, 'A');
    LCD_putc(1, 'B');
    LCD_putc(2, 'C');
    LCD_putc(3, 'D');
    LCD_putc(4, 'E');
    LCD_putc(5, 'F');

    LCD_UpdateRequired(TRUE, 0);

    LCD_CONTRAST_LEVEL(input);

}



