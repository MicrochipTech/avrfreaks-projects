#include <stdio.h>
#include <inttypes.h>
#include "LCD_Joystick.c"
#include "RTC.c"
/*
**  AVR Butterfly Real Time Clock.
**  Demo routine adjusts RTC time using joystick as input
**  WinAVR/AVR Studio 4
**
** 	Jim Remington, sjames_remington at yahoo.com
**  March, 2008
**
*/
void SetRTC(void);

int main(void)
{

	char test=0;

	LCD_Init();
	RTC_Init();
	Joystick_Init();
	sei();

	SetRTC();
	while (1)
	{
	while (RTC_buffer[5]==test);  //hang until seconds digit changes
    ShowClock();
	test=RTC_buffer[5];
	}
   return 0;
}
 

void SetRTC(void)
{
	char RTC_max[]="295959";  //max values of each digit. Does not check for illegal entries
	char key;
	uint8_t digit=5; 		//adjust digits of BCD clock buffer according to user input

	KeyAutoRepeat=true;		//set autorepeat for keys
	LCD_Flash=(1<<digit);	//flash selected digit (to be adjusted)
	ShowClock();

    TIMSK2 &= ~(1<<TOIE2);	// disable Timer2 overflow interrupt (stop the RTC)

	while (1) 
	{   key=getkey();

	if (key == KEY_PLUS)		//increment selected digit, with rollover
		{  
		RTC_buffer[digit]++;
		if (RTC_buffer[digit]>RTC_max[digit]) RTC_buffer[digit]='0';
		LCD_Flash = (1 << digit);
		ShowClock();
		}
		
	if (key == KEY_MINUS)		//decrement selected digit, with rollover
	 	{ 
		RTC_buffer[digit]--;
		if (RTC_buffer[digit]<'0') RTC_buffer[digit]=RTC_max[digit];
		LCD_Flash = (1 << digit);
		ShowClock();
		}

	if (key == KEY_NEXT)		//select digit to right
		{
		digit++;
		if (digit>5) digit=0;
		LCD_Flash = (1 << digit);
		ShowClock();
		}

	if (key == KEY_PREV)		//select digit to left
		{
		digit--;
		if (digit>5) digit=5;  //wraparound for any invalid value
		LCD_Flash = (1<< digit);
		ShowClock();
		}

	if (key == KEY_ENTER) break;		//all done, start clock and exit

	}

	LCD_Flash=0;
	KeyAutoRepeat=false;
    TIMSK2 |= (1<<TOIE2);	// enable Timer2 overflow interrupt
}


