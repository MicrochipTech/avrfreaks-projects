#include "ps2kbd.h"
#include "lcd.h"


int main()
{
	unsigned char	c;

	kbd_init();
	
	lcd_init(LCD_DISP_ON);

	while(1)
	{
		while(c = kbd_getchar())
			lcd_putc(c);
	}
	
	return 0;
}
