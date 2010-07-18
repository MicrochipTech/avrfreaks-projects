#include <avr/io.h>
#include "screenkeys.h"
#include "start.h"
#include "utility.h"


int main()
{
	SK_SWITCH( DDR) = IN;  // set SK switch pin to input
	SK_SWITCH( PORT) = ON;  // set SK switch pullup on

	sk_init();
	sk_set_color( SK_DARK_GREEN);
	sk_set_text( "Left", SK_TEXT_LEFT, SK_LINE_1);
	sk_set_text( "Right", SK_TEXT_RIGHT, SK_LINE_2);
	sk_set_text( "Cnt", SK_TEXT_CENTER, SK_LINE_3);

	uint8_t flag = 0;
	uint8_t btn_db = 0;
	while( TRUE)
	{
		// Button Debounce
		if( SK_SWITCH( PIN) == LOW)
		{
			if (btn_db < 0x4)
				btn_db ++;

			if (btn_db == 0x4)
			{
				btn_db = 0xFF;
				if( !flag)
				{
					sk_set_pixels( start_img, sizeof( start_img), 0);
					sk_set_text( "99", SK_TEXT_LEFT, SK_LINE_3);
					sk_set_color( SK_BRIGHT_BLUE);
					flag = 1;
				}
				else
				{
					sk_set_text( "1234", SK_TEXT_RIGHT, SK_LINE_1);
					sk_clear_text( SK_LINE_2);
					sk_set_color( SK_BRIGHT_RED);
					flag = 0;
				}
			}	
		}
		else
			btn_db = 0;
	}
}
