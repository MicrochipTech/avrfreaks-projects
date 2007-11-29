#include <inttypes.h>
#include <avr\pgmspace.h>
#include "status.h"
#include "lcd.h"
#include "RS232.h"
#include "init.h"
#include "led.h"
#include "GSMParser.h"

int getFixstate ()
{
	return fixstate;
}

void setFixstate (char c)
{
	if (c=='0')
	{
		fixstate = 0;
	}
	if (c=='1')
	{
		fixstate = 1;
	}
}

void showFixstate()
{
	if (getFixstate()==1)
	{
		lcd_gotoxy(0,2);
    	lcd_puts("GPS-Fix: Yes      ");
	}
	if (getFixstate()==0)
	{
		lcd_gotoxy(0,2);
    	lcd_puts("GPS-Fix: No     ");
	}
}


void showLatitudeFormatted()
{
	lcd_gotoxy(0,1);
	lcd_puts(getLatitudeFormatted());
}


void showLongitudeFormatted()
{
	lcd_gotoxy(0,3);
	lcd_puts(getLongitudeFormatted());
}

void setEW_indicator (char c)
{
	EWindicator=c;
}

void setNS_indicator (char c)
{
	NSindicator=c;
}

char getNS_indicator ()
{
	return NSindicator;
}

char getEW_indicator()
{
	return EWindicator;
}

char*  getLatitude()
{
return Latitude;
}

void  setLatitude(char *s)
{
	 memcpy(Latitude,s,sizeof(Latitude)-1); 
}


char*  getLongitude()
{
return Longitude;
}

void  setLongitude(char *s)
{
	 memcpy(Longitude,s,sizeof(Longitude)-1); 
}

void FormatLongitude()
{
	LongitudeFormatted[0] = EWindicator;
	LongitudeFormatted[1] = ' ';
	LongitudeFormatted[2] = Longitude[0];
	LongitudeFormatted[3] = Longitude[1];
	LongitudeFormatted[4] = Longitude[2];
	LongitudeFormatted[5] = ' ';
	LongitudeFormatted[6] = Longitude[3];
	LongitudeFormatted[7] = Longitude[4];
	LongitudeFormatted[8] = Longitude[5];
	LongitudeFormatted[9] = Longitude[6];
	LongitudeFormatted[10] = Longitude[7];
	LongitudeFormatted[11] = Longitude[8];
	LongitudeFormatted[12] = Longitude[9];
	LongitudeFormatted[13] = Longitude[10];
}

void FormatLatitude()
{
	LatitudeFormatted[0] = NSindicator;
	LatitudeFormatted[1] = ' ';
	LatitudeFormatted[2] = Latitude[0];
	LatitudeFormatted[3] = Latitude[1];
	LatitudeFormatted[4] = ' ';
	LatitudeFormatted[5] = Latitude[2];
	LatitudeFormatted[6] = Latitude[3];
	LatitudeFormatted[7] = Latitude[4];
	LatitudeFormatted[8] = Latitude[5];
	LatitudeFormatted[9] = Latitude[6];
	LatitudeFormatted[10] = Latitude[7];
	LatitudeFormatted[11] = Latitude[8];
	LatitudeFormatted[12] = Latitude[9];
}

char*  getLatitudeFormatted()
{
return LatitudeFormatted ;
}

char*  getLongitudeFormatted()
{
return LongitudeFormatted;
}


void ReportPosition()
{
	led_yellow_off();
	uart0_puts("AT+CMGS="); //on the place of xxxx place phonenumber !
	uart0_putc('0');
			int i = 10;
					for (; i < 19; i++)
					{
						uart0_putc(GSM_COMMAND[i]);
					}
    uart0_putc('\r');
	delay_ms(1000);
	uart0_puts("GPS Position: ");
	uart0_puts(getLongitudeFormatted());
	uart0_putc(' ');
	uart0_puts(getLatitudeFormatted());
	uart0_putc(' ');
	if (getFixstate())
	{
		uart0_puts("Fix: Yes");
	}
	else
	{
		uart0_puts("Fix: No");
	}
	uart0_puts("\x1a");

}



