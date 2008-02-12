#include <avr/io.h>
#include "global.h"
#include "keypad.h"
#include <string.h>
#include <avr/pgmspace.h>
#include "lcd.h"


volatile unsigned char keypad_timer ;
volatile unsigned char index;


void keypad_init(void)
{
	keypad_timer = 0;
	keypad_ddr = 0x0f;
	keypad_port = 0xff;

}


////////////////////////////////////////////////////////////////////
//////////		return 1 key pressed
////////////////////////////////////////////////////////////////////
char keypad_c(void)
{
	unsigned char hold_time = 250; 
		
	again:
		
		// if key pressed in keypad_s function  
		if(keypad_timer > 0)
		{
			keypad_timer ++;
			if (keypad_timer >= 80)
			{
				keypad_timer = 0;
				return 20;
			
			}
		}
		for(unsigned char i=0;i<15;i+=4)
		{
			switch (i)
			{
				case 0:
					keypad_port = 0xfe;
				break;
				case 4:
					keypad_port = 0xfd;
				break;
				case 8:
					keypad_port = 0xfb;
				break;
				case 12:
					keypad_port = 0xf7;
				break;
			
			}
		
			
			_delay_ms(5);
			if( (keypad_pin&0xf0) != 0xf0)
			{
				_delay_ms(10);_delay_ms(10);
				switch(keypad_pin & 0xf0)
				{
					case 0xe0:
						keypad_timer = 0;
						while ( (keypad_pin & 0xf0) != 0xf0)
						{
							_delay_ms(1);
							hold_time--;
							if(hold_time == 0)
								break;
						};
						return 1+i;
					break;
					case 0xd0:
						keypad_timer = 0;
						while ( (keypad_pin & 0xf0) != 0xf0)
						{
							_delay_ms(1);
							hold_time--;
							if(hold_time == 0)
								break;
						};

						return 2+i;
					break;
					case 0xb0:
						keypad_timer = 0;
						while ( (keypad_pin & 0xf0) != 0xf0)
						{
							_delay_ms(1);
							hold_time--;
							if(hold_time == 0)
								break;
						};

						return 3+i;
					break;
					case 0x70:
						keypad_timer = 0;
						while ( (keypad_pin & 0xf0) != 0xf0)
						{
							_delay_ms(1);
							hold_time--;
							if(hold_time == 0)
								break;
						};
						return 4+i;
					break;
				
				}
			}
		
		
		}
	
		//////////////
		goto again;


}

////////////////////////////////////////////////////////////////////
//////////		return string , end to end_char
////////////////////////////////////////////////////////////////////
char* keypad_s(unsigned char x,unsigned char y)
{
	char temp = 255;
	//char last_key = 255;
	
	char *str = "               ";
	//char str[max_str]; 
	//for(unsigned char ww=max_str;ww>0;ww--)
	//	str[ww] = 0;
	index = 0;
	
	lcdControlWrite(1<<LCD_ON_CTRL | 1<<LCD_ON_DISPLAY | 1<<LCD_ON_BLINK );
//	LCDcursorOnBlink();
	again:
	lcdGotoXY((x+index),y);
	//last_key = temp;	
	temp = keypad_c();

	/*if((last_key != temp) && (keypad_timer>0) )
	{
		if(index <= max_str-2)
			index ++;		
	}
	*/
	if(temp == end_char)
	{
		lcdControlWrite(1<<LCD_ON_CTRL | 1<<LCD_ON_DISPLAY);
		//LCDcursorOFF();
		index = 0;
		return (str);
	}
	else if(temp == next_char)
	{
		if(index <= max_str-2)
			index ++;
	}
	else if(temp == del_char)
	{
		if(index != 0)
		{
			str[index] = ' ';
			lcdGotoXY((x+index),y);
			lcdDataWrite(' ');
			index--;
		}
		else
		{
			lcdControlWrite(1<<LCD_ON_CTRL | 1<<LCD_ON_DISPLAY);
			strcpy_P(str,PSTR("               "));
			return str;
		}
	}
	else if(temp == 20)	//	time out for next char
	{
		if(index <= max_str-2)
			index++;
	
	}
	else
	{
			keypad_timer = 1;
			lcdGotoXY((x+index),y);
			
			switch(temp)
			{
				////////////////////////////   keypad 1 pressed
				case 1:
					switch(str[index])
					{
						default:
							str[index] = '_';
						break;
						case '_':
							str[index] = '.';
						break;
						case '.':
							str[index] = ',';
						break;
						case ',':
							str[index] = '1';
						break;
						case '1':
							str[index] = ' ';
						break;
					}
					lcdDataWrite(str[index]);
				break;
				////////////////////////////// keypad 2 pressed
				case 2:
					switch(str[index])
					{
						default:
							str[index] = 'A';
						break;
						case 'A':
							str[index] = 'B';
						break;
						case 'B':
							str[index] = 'C';
						break;
						case 'C':
							str[index] = '2';
						break;
						case '2':
							str[index] = 'A';
						break;
					}
					lcdDataWrite(str[index]);
				break;
				////////////////////////////// keypad 3 pressed
				case 3:
					switch(str[index])
					{
						default:
							str[index] = 'D';
						break;
						case 'D':
							str[index] = 'E';
						break;
						case 'E':
							str[index] = 'F';
						break;
						case 'F':
							str[index] = '3';
						break;
						case '3':
							str[index] = 'D';
						break;
					}
					lcdDataWrite(str[index]);
				break;
				
				//////////////////////////////	keypad 4 pressed
				case 5:
					switch(str[index])
					{
						default:
							str[index] = 'G';
						break;
						case 'G':
							str[index] = 'H';
						break;
						case 'H':
							str[index] = 'I';
						break;
						case 'I':
							str[index] = '4';
						break;
						case '4':
							str[index] = 'G';
						break;
					}
					lcdDataWrite(str[index]);
				break;
				//////////////////////////////	keypad 5 pressed
				case 6:
					switch(str[index])
					{
						default:
							str[index] = 'J';
						break;
						case 'J':
							str[index] = 'K';
						break;
						case 'K':
							str[index] = 'L';
						break;
						case 'L':
							str[index] = '5';
						break;
						case '5':
							str[index] = 'J';
						break;
					}
					lcdDataWrite(str[index]);
				break;
				//////////////////////////////		keypad 6 pressed
				case 7:
					switch(str[index])
					{
						default:
							str[index] = 'M';
						break;
						case 'M':
							str[index] = 'N';
						break;
						case 'N':
							str[index] = 'O';
						break;
						case 'O':
							str[index] = '6';
						break;
						case '6':
							str[index] = 'M';
						break;
					}
					lcdDataWrite(str[index]);
				break;
				//////////////////////////////	keypad 7 pressed
				case 9:
					switch(str[index])
					{
						default:
							str[index] = 'P';
						break;
						case 'P':
							str[index] = 'Q';
						break;
						case 'Q':
							str[index] = 'R';
						break;
						case 'R':
							str[index] = 'S';
						break;
						case 'S':
							str[index] = '7';
						break;
						case '7':
							str[index] = 'P';
						break;
					}
					lcdDataWrite(str[index]);
				break;
				//////////////////////////////		keypad 8 pressed
				case 10:
					switch(str[index])
					{
						default:
							str[index] = 'T';
						break;
						case 'T':
							str[index] = 'U';
						break;
						case 'U':
							str[index] = 'V';
						break;
						case 'V':
							str[index] = '8';
						break;
						case '8':
							str[index] = 'T';
						break;
					}
					lcdDataWrite(str[index]);
				break;
				//////////////////////////////		keypad 9 pressed
				case 11:
					switch(str[index])
					{
						default:
							str[index] = 'W';
						break;
						case 'W':
							str[index] = 'X';
						break;
						case 'X':
							str[index] = 'Y';
						break;
						case 'Y':
							str[index] = 'Z';
						break;
						case 'Z':
							str[index] = '9';
						break;
						case '9':
							str[index] = 'W';
						break;
					}
					lcdDataWrite(str[index]);
				break;
				//////////////////////////////		keypad 0 pressed
				case 14:
					switch(str[index])
					{
						default:
							str[index] = '0';
						break;
						case '0':
							str[index] = ' ';
						break;
					}
					lcdDataWrite(str[index]);
				break;
				//////////////////////////////
			}
			
	
	
	
	}
	goto again;
}
/////////////////////////////////////////////////////////////////////
