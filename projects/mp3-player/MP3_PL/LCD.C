/// lcd.h ///

#include "Main.h"
#include "lcd.h"
#include "timers.h"
#include <string.h>

#include <avr/io.h>

#define LCDCOLS 24
#define LCDROWS 2
//#define WINTEK 1


void lcd_ready (byte enable)
{	byte busy;
	byte itemp,count;
	itemp = DDRA;
	DDRA=itemp&0x0F;				// Switch high nibble to input
	PORTA&= ~(LCD_RW_MASK|LCD_RS_MASK|enable);  //Clear all control lines
	PORTA|= LCD_RW_MASK;					
//	delay(1);
	do
	{	for (count=0;count<8;count++)
			PORTA|= enable;
		busy = PINA&0x80;				//Get PortA input, check busy flag
		PORTA&=~enable;
		PORTA|= enable;					//clock low nibble
		PORTA&=~enable;
//		delay(1);
	}		
	while (busy);
	DDRA=itemp;			//restore DDRA
}		

void lcd_out (byte data, byte enable)
{	byte itemp,tempdat;
	
	itemp=DDRA;
	PORTA&=0x0F;					//disable display data
	PORTA|=(data&0xF0);			// Set Data to port, high nibble
	DDRA|=0xF0;						//Set PortA, high nibble  to output
	PORTA|=enable;
	PORTA&=~enable;					//Disable LCD
	data=data<<4;
	PORTA&=0x0F;
	PORTA|=data&0xF0;				// Set Data to port, low nibble
	PORTA|= enable;					//Enable LCD
	PORTA&= ~enable;				//Disable LCD
	DDRA=itemp;						//restore DDRA
}

void lcd_write_command (byte com, byte enable)
{	lcd_ready(enable);
	PORTA &= ~(LCD_RW_MASK|LCD_RS_MASK|enable);  //Clear all control lines
	lcd_out (com, enable);
}	
	
byte  lcd_write_string (char x,signed char y, char *text, char centered)
{	byte error=0, ramadress,portstat;
	static byte enable;
	
	if (centered)
		x=(LCDCOLS-strlen(text))>>1;
	error=((0x40*y)>(0x40*(LCDROWS-1)));
	if (!error)
		error=(x>LCDCOLS-1);					//adjust for different LCD, max columns-1
	if (!error)
	{	if (y >= 0)
		{	ramadress=0x40*y+x;
			enable=LCD_EN1_MASK;
#if defined WINTEK
			if (y>1)
			{	ramadress=0x40*(y-2)+x;
				enable = LCD_EN2_MASK;
			}
#endif
			lcd_write_command (ramadress|0x80,enable);
		}
		while (*text != 0)
		{  	lcd_ready(enable);
			PORTA&= ~(LCD_RW_MASK|LCD_RS_MASK|enable);  //Clear all control lines
			PORTA |= LCD_RS_MASK;
			lcd_out (*text++,enable);
		}
	}
	return error;
}



void lcd_cls (void)
{	lcd_write_command (1,LCD_EN1_MASK);
#if defined WINTEK
	lcd_write_command (1,LCD_EN2_MASK);
#endif
}



void lcd_init (void)
{	byte counter;
	delay (100);		//wait 50ms
	PORTA&=0x01;	//switch off all except #1
	for (counter=0;counter<3;counter++)
	{	PORTA|=0x30;	//set 4 bit mode
		PORTA|=LCD_EN1_MASK;
		PORTA&=~LCD_EN1_MASK;
		delay(5);
	}

	PORTA|=0x20;	//set 4 bit mode
	PORTA|=LCD_EN1_MASK;
	PORTA&=~LCD_EN1_MASK;
	delay (5);
	
	lcd_write_command (0x28,LCD_EN1_MASK);      //2 line mode,5x7 dots,4bit Interface
	lcd_write_command (0x0c,LCD_EN1_MASK);		//Display on, Cursor off,not  blinking
	lcd_write_command (1,LCD_EN1_MASK);		//Clear display
	lcd_write_command (0x06,LCD_EN1_MASK); 	//Entry mode increment, shift off
	delay (20);

#if defined WINTEK
	lcd_write_command (0x28,LCD_EN2_MASK);      //2 line mode,5x7 dots,4bit Interface
	lcd_write_command (0x0c,LCD_EN2_MASK);		//Display on, Cursor off,not  blinking
	lcd_write_command (1,LCD_EN2_MASK);		//Clear display
	delay (2);
	lcd_write_command (0x06,LCD_EN2_MASK); 	//Entry mode increment, shift off
	delay (20);
#endif

}

