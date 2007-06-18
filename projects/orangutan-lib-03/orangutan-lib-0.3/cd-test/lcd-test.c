// lcd-test
//
// Tom Benedict

// This example demonstrates most of the functions of the LCD
// subsystem.  Since the Baby-Orangutan has no LCD, this example
// will only work on the Orangutan or the Orangutan-X2.

// Include our I/O definitions
#include <avr/io.h>

// Include information about the device we're building for
#include "device.h"

// Include for using delay routines
#include <util/delay.h>

// Include for the LCD subsystem
#include "lcd.h"

// Delay for N seconds
void delay_sec(unsigned char sec)
{
	unsigned int cycles;

	// Delay 25ms at a time (38.4ms is the most we can delay with a
	// 20MHz processor, unfortunately.  See the delay.h include file
	// for more info.)

	for(cycles = 0; cycles < (sec * 40); cycles ++)
	{
		_delay_ms(12);
		_delay_ms(13);
	}
}

// And now for our main routine:
int main(void)
{
	int i;

#ifdef _ENABLE_LCD_

	// Initialize the LCD
	lcd_init();

	for(;;)
	{
		// Startup Banner
		lcd_clear();
		lcd_gotoxy(0,0);
		lcd_string("LCD");
		lcd_gotoxy(0,1);
		lcd_string("Test...");
		delay_sec(2);

		// Test multiline display
		lcd_clear();
		lcd_gotoxy(0,0);
		lcd_string("Line 1");
		lcd_gotoxy(0,1);
		lcd_string("Line 2");
		delay_sec(2);

		// Test cursor hiding
		lcd_clear();
		lcd_gotoxy(0,0);
		lcd_string("Hide");
		lcd_hide();
		delay_sec(2);
		lcd_gotoxy(0,1);
		lcd_string("Show");
		lcd_show();
		delay_sec(2);

		// Now hide it 'cause it's annoying
		lcd_hide();

		// Scroll left
		lcd_clear();
		lcd_gotoxy(0,0);
		lcd_string("Scroll");
		lcd_gotoxy(0,1);
		lcd_string("Left");
		delay_sec(1);
		for(i=0; i<=LCD_MAX_X; i++)
		{
			lcd_shift(0, 1);
			delay_sec(1);
		}

		// Scroll right
		lcd_clear();
		lcd_gotoxy(0,0);
		lcd_string("Scroll");
		lcd_gotoxy(0,1);
		lcd_string("Right");
		delay_sec(1);
		for(i=0; i<=LCD_MAX_X; i++)
		{
			lcd_shift(1, 1);
			delay_sec(1);
		}

		// Show the cursor again
		lcd_show();

		// Cursor left
		lcd_clear();
		lcd_gotoxy(0,0);
		lcd_string("Cursor");
		lcd_gotoxy(0,1);
		lcd_string("Left");
		lcd_gotoxy(7,0);
		delay_sec(1);
		for(i=0; i<=LCD_MAX_X; i++)
		{
			lcd_moverel(0, 1);
			delay_sec(1);
		}

		// Cursor right
		lcd_clear();
		lcd_gotoxy(0,0);
		lcd_string("Cursor");
		lcd_gotoxy(0,1);
		lcd_string("Right");
		lcd_gotoxy(0,1);
		delay_sec(1);
		for(i=0; i<=LCD_MAX_X; i++)
		{
			lcd_moverel(1, 1);
			delay_sec(1);
		}

		// Hide the cursor again
		lcd_hide();

		// Moveto (butterfly pattern)

		lcd_clear();
		lcd_gotoxy(0,0);
		lcd_string("Move-To");
		lcd_gotoxy(0,1);
		lcd_string("Testing");

		for(i = 0; i < 4; i++)
		{
			lcd_moveto(1, i);
			lcd_string("#");
			lcd_moveto(1, 7-i);
			lcd_string("#");
			lcd_moveto(2, i);
			lcd_string("#");
			lcd_moveto(2, 7-i);
			lcd_string("#");
			delay_sec(1);
		}

		delay_sec(2);

	}

#endif // _ENABLE_LCD_

	// We never get here, but return a zero if we ever do.
	return 0;
}
