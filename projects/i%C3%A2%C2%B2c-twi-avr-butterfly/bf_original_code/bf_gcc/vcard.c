//***************************************************************************
//
//  File........: vcard.c
//
//  Author(s)...: ATMEL Norway
//
//  Target(s)...: ATmega169
//
//  Compiler....: avr-gcc 4.1.1; avr-libc 1.4.5
//
//  Description.: AVR Butterfly Name-tag
//
//  Revisions...: 1.0
//
//  YYYYMMDD - VER. - COMMENT                                       - SIGN.
//
//  20030116 - 1.0  - Created                                       - LHM
//  20031009          port to avr-gcc/avr-libc                      - M.Thomas
//  20031205          fixed store length to eeprom from RS232       - mt
//  20060106          fixed length-check in vCard()                 - mt
//  20070129          using avr-libc's eeprom.h                     - mt
//  20070517          avoid buffer-overflows, EnterName modified    - mt
//
//***************************************************************************

//mtA
//#include <inavr.h>
//#include "iom169.h"
//#include "eeprom.h"
#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <avr/eeprom.h>
#include "bfeeprom.h"
//mtE
#include "main.h"
#include "button.h"
#include "LCD_functions.h"
#include "usart.h"
#include "vcard.h"


char gUART = FALSE;

// mt s/index/indexps
//char index = 0;         //variable to keep the lenght of the present string
uint8_t indexps = 0;
char Name[STRLENGHT];

// mt __flash char TEXT_WAIT[]                     = "waiting for input on RS232";
// mt: jw method used 
/// const char TEXT_WAIT[] PROGMEM                = "waiting for input on RS232";

/*****************************************************************************
*
*   Function name : vCard
*
*   Returns :       char ST_state (to the state-machine)
*
*   Parameters :    char input (from joystick)
*
*   Purpose :       Puts the name in EEPROM on the LCD
*
*****************************************************************************/
char vCard(char input)
{
	static char enter = 1;

	if (enter)
	{
		// mt __EEGET(indexps, EEPROM_START);           // Load the length if the name
		indexps=eeprom_read_byte(EEPROM_START);

		if((indexps < 1) || (indexps > STRLENGHT))   // if illegal length / mt: fixed, was: |    
		{
			indexps = 0;
			Name[0] = 'A';
			enter = 1;
			return ST_VCARD_ENTER_NAME;               //enter new name
		}
		else
		{
			LoadEEPROM(Name, indexps, (EEPROM_START + 1));  // Load name 
			LCD_puts(Name, 1);
			enter = 0;
		}
	}

	else if (input == KEY_NEXT)
	{
		enter = 1;
		return ST_VCARD_ENTER_NAME;
	}
	else if (input == KEY_PREV)
	{
		enter = 1;
		return ST_VCARD;
	}

	return ST_VCARD_FUNC;
}


/*****************************************************************************
*
*   Function name : EnterName
*
*   Returns :       char ST_state (to the state-machine)
*
*   Parameters :    char input (from joystick)
*
*   Purpose :       Lets the user enter a name using the joystick. Pressing the
*                   joystick UP/DOWN will browse the alphabet and NEXT/PREV 
*                   will shift between the characters in the name.
*
*   mthomas: I have modified this function a little and did not keep the 
*            original code.
*****************************************************************************/
//mtA
char EnterName(char input)
{
	static char enter = 1;
	uint8_t i, temp_index;
	char c, c_out;

	if (enter)
	{
		LoadEEPROM(Name, indexps, EEPROM_START + 1);  // Load name from EEPROM

		if(indexps) {
			indexps -= 1; // set work-char to last char (not the tailing \0)
		}
		enter = 0;
	}
	else
	{
		// copy the tailing characters of Name to LCD
		temp_index = indexps+1;
		for( i = 6; i > 0; i-- ) {
			if ( temp_index > 0 ) {
				c = Name[temp_index-1];
				temp_index--;
				if ( ( c >= ' ' ) && ( c <= 'z' ) ) { //check if it's legal character
					c_out = c;
				}
				else {
					c_out = ' '; // if not, put in a space
				}
			}
			else {
				c_out = ' ';
			}	
			if ( i == 6 ) {
				LCD_putc(i-1, c_out|0x80); // enable blink for 6th char on LCD;
			}
			else {
				LCD_putc(i-1, c_out);
			}
		}
		LCD_putc(6, '\0');

		if (input != KEY_NULL) {
			LCD_FlashReset();
		}
		LCD_UpdateRequired(TRUE, 0);
	}

	if (input != KEY_NULL) {
		LCD_FlashReset();
	}

	if ( input == KEY_MINUS ) // mt 1/06 (input == KEY_PLUS)
	{
		Name[indexps]--;
		
		c = Name[indexps];
		if( (('!' <= c) && (c <= '/')) && (c != ' '))
			Name[indexps] = ' ';
		else if((':' <= c) && (c <= '@'))
			Name[indexps] = '9';
		else if(c >= '[')
			Name[indexps] = 'Z';
		else if(c < ' ')
			Name[indexps] = 'Z';
	}
	else if ( input == KEY_PLUS ) // mt 1/06 (input == KEY_MINUS)
	{
		Name[indexps]++;
		
		c = Name[indexps];
		if( (('!' <= c) && (c <= '/')) && (c != ' '))
			Name[indexps] = '0';
		else if( (':' <= c) && (c <= '@'))
			Name[indexps] = 'A';
		else if(c >= '[')
			Name[indexps] = ' ';
		else if(c < ' ')
			Name[indexps] = ' ';
	}
	else if (input == KEY_PREV)
	{
		if(indexps)
		{
			indexps--;
		}
	}
	else if (input == KEY_NEXT)
	{
		if(indexps < STRLENGHT-1) // mt 5/2007: -1
		{
			i = Name[indexps]; // ext. mt 1/2006
			indexps++;
			Name[indexps] = i; // Name[indexps] = 'A';
		}
	}
	else if (input == KEY_ENTER)
	{
		indexps++;
		Name[indexps] = '\0';

		// mt __EEPUT(EEPROM_START, indexps);   //store the length of name in EEPROM
		eeprom_write_byte(EEPROM_START, indexps);
		StoreEEPROM(Name, indexps, EEPROM_START + 1);  //store the Name in EEPROM

		enter = 1;
		return ST_VCARD_FUNC;
	}

	return ST_VCARD_ENTER_NAME_FUNC;
}
//mtE


/*****************************************************************************
*
*   Function name : RS232
*
*   Returns :       char ST_state (to the state-machine)
*
*   Parameters :    char input (from joystick)
*
*   Purpose :       Store data from the UART to EEPROM
*
*****************************************************************************/
char RS232(char input)
{
    static char enter = 1;
    char c;
    static char buffer[STRLENGHT];
    //static char temp_index;
    static uint8_t temp_index;
    
    if (enter)
    {
        cli(); // mt __disable_interrupt();
        
        // boost IntRC to 2Mhz to achieve 19200 baudrate
        CLKPR = (1<<CLKPCE);        // set Clock Prescaler Change Enable
        // set prescaler = 4, Inter RC 8Mhz / 4 = 2Mhz
        CLKPR = (1<<CLKPS1);
        
        sei(); // mt __enable_interrupt();
        
        // mt jw-meth: LCD_puts_f(TEXT_WAIT, 0);
        LCD_puts_f(PSTR("waiting for input on RS232"), 0);
        enter = 0;
        temp_index = 0;
        c = UDR;                       // Dummy read to clear receive buffer
        gUART = TRUE;
    }

    if (UCSRA & (1<<RXC))
    {
        c = UDR;
        if (c != '\r')
        {
            if (temp_index < STRLENGHT-1) // mt 5/2007: -1
                buffer[temp_index++] = c;
        }
        else    // UART transmission completed
        {
            cli(); // mt __disable_interrupt();
                
            CLKPR = (1<<CLKPCE);        // set Clock Prescaler Change Enable
            // set prescaler = 8, Inter RC 8Mhz / 8 = 1Mhz
            CLKPR = (1<<CLKPS1) | (1<<CLKPS0);
                
            sei(); // mt __enable_interrupt();
                
            if(temp_index)
            {
                buffer[temp_index] = '\0';
                for (temp_index = 0; buffer[temp_index]; temp_index++)
                    Name[temp_index] = buffer[temp_index];
                Name[temp_index] = '\0';
                
                enter = 1;
                        
                // mt __EEPUT(EEPROM_START, temp_index);   //store the length of name in EEPROM
                eeprom_write_byte(EEPROM_START, temp_index);
                StoreEEPROM(Name, temp_index, EEPROM_START + 1);  //store the Name in EEPROM
                
                indexps = temp_index;
                
                gUART = FALSE;
                return ST_VCARD_FUNC;
            }
            else    // if no characters received 
            {
                enter = 1;
                return ST_VCARD_DOWNLOAD_NAME;
            }
        }
    }


    if (input != KEY_NULL)
    {
        enter = 1;
        
        cli(); // mt __disable_interrupt();
        
        CLKPR = (1<<CLKPCE);        // set Clock Prescaler Change Enable
        // set prescaler = 8, Inter RC 8Mhz / 8 = 1Mhz
        CLKPR = (1<<CLKPS1) | (1<<CLKPS0);
        
        sei(); // __enable_interrupt();
        
        gUART = FALSE;
        return ST_VCARD_DOWNLOAD_NAME;
    }


    return ST_VCARD_DOWNLOAD_NAME_FUNC;
}
