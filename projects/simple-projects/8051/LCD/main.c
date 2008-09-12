#include <reg51.h>
#include <stdio.h>
#include <lcd.h>

main()
	{
	char x;
	SCON  = 0x50; /* 	SCON: mode 1, 8-bit UART, enable rcvr      		*/
    TMOD |= 0x20; /* 	TMOD: timer 1, mode 2, 8-bit reload        		*/
    TH1   = 0xfd; /* 	TH1:  reload value for 9600 baud @ 11.0592MHz  	*/
    TR1   = 1;    /* 	TR1:  timer 1 run                          		*/
    TI    = 1;    /* 	TI:   set TI to send first char of UART    		*/
	ES=1;
	EA=1;

	LCD_init();

	while(1)
		{
		LCD_putc(getchar());
		}
	}