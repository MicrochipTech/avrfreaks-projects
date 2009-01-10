//**********************************************************************//
//FILE: main.c 
//AUTHOR: Adam Kadolph
//DATE:	01-07-2009
//DESCRIPTION: This program is a Temperature Data Logger. It will read temperature
// from a LM34 sensor interfaced to the A/D Converter on the Atmega8 AVR
// and process this data for min, max, average, and current values. A DS1307
// RTC and EEPROM are interfaced to allow for time triggered events and then
// store them in memory.
//**********************************************************************//

#include <avr/io.h>
#include "lcd.h"
#include "adc.h"
#include "rtc.h"
#include "twi.h"

#define pTemp	0
#define pLow	3
#define pHigh	6
#define pAve	9

void PrintScreen(void);
void ScreenUpdate(void);
void Line1Update();
void Line2Update();
void Line3Update();

unsigned char STATE;
unsigned int temp,current,Low,High,Ave;
unsigned long total,n;

unsigned char Line1[20];
unsigned char Line2[20];
unsigned char Line3[20];
//unsigned char Line4[20];

int main(void)
{
	STATE = 0;	//start at the first state

	total = 0;	//total sample value
	n = 1;		//number of samples
	
	LCDinit();
	ADCinit();
	I2Cinit();
	RTCinit();

	LCDclr();

	rtc_register[0] = 0x00;
	rtc_register[1]	= 0x00;
	rtc_register[2]	= 0x00;
	rtc_register[3]	= 0x00;
	rtc_register[4] = 0x00;
	rtc_register[5] = 0x00;
	rtc_register[6] = 0x00;

	RTC_writeTime();

	ADCstart();	//start the A/D converter and discard first data reading

	while(1)
	{
		PrintScreen();
		
		if(STATE == 0)
		{
			//initial state for data logger
			ADCstart();

			temp = ADCL;	//only need to use ADC low register
			temp |= (ADCH<<8);

			TempConvert(temp,pTemp);	

			current = temp;	//store first measurement for all
			Low = temp;
			High = temp;
			Ave = temp;

			STATE = 1;
		}

		if(STATE == 1)
		{
			//timeout state to trigger a sample
			RTC_getTime();	//read time to determine when to get next sample

			if(time[7] == 0x30)
			{
				STATE = 2;		//trigger next state after 10 seconds
			}
			else
			{
				STATE = 1;		//stay in current state until 10 seconds have elasped
			}
		}

		if(STATE == 2)
		{
			//ADC data receive state
			ADCstart();

			temp = ADCL;
			temp |= (ADCH<<8);

			TempConvert(temp,pTemp);	//only need to use ADC low register

			current = temp;	//store for data processing

			STATE = 3;
		}

		if(STATE == 3)
		{
			if(current < Low)
			{
				Low = current;		//check for low value
			}

			if(current > High)
			{
				High = current;		//check for high value
			}

			total += current;	//calculate total sample value

			Ave = total / n;	//get average of samples

			n++;	

			STATE = 4;
		}

		if(STATE == 4)
		{
			TempConvert(Low,pLow);
			TempConvert(High,pHigh);
			TempConvert(Ave,pAve);

			ScreenUpdate();

			STATE = 1;
		}
	}

	return 0;
}

void PrintScreen(void)
{
	//functions to handle all of the data to be displayed
	LCDGotoXY(0,0);			//display current time
	LCDstring(Line1,5);
	RTC_displayTime();

	LCDGotoXY(0,1);			//display temperature
	LCDstring(Line2,11);

	LCDGotoXY(0,2);			//display temp high, low, and average
	LCDstring(Line3,19);

	/*LCDGotoXY(0,3);
	LCDstring(Line4,20);		//display memory usage*/
}

void ScreenUpdate(void)
{
	Line1Update();		//function to update all values to be displayed
	Line2Update();
	Line3Update();
	//Line4Update();
}
	
void Line1Update()
{
	Line1[0] = 'T';
	Line1[1] = 'i';
	Line1[2] = 'm';
	Line1[3] = 'e';
	Line1[4] = ':';
}

void Line2Update()
{
	Line2[0] = 'T';
	Line2[1] = 'e';
	Line2[2] = 'm';
	Line2[3] = 'p';
	Line2[4] = ':';
	Line2[5] = temperature[0];
	Line2[6] = temperature[1];
	Line2[7] = '.';
	Line2[8] = temperature[2];
	Line2[9] = 0xDF;
	Line2[10] = 'F'; 
}

void Line3Update()
{
	Line3[0] = 'L';
	Line3[1] = 'o';
	Line3[2] = ':';
	Line3[3] = temperature[3];
	Line3[4] = temperature[4];
	Line3[5] = ' ';
	Line3[6] = 'H';
	Line3[7] = 'i';
	Line3[8] = ':';
	Line3[9] = ' ';
	Line3[10] = temperature[6];
	Line3[11] = temperature[7];
	Line3[12] = ' ';
	Line3[13] = 'A';
	Line3[14] = 'v';
	Line3[15] = 'e';
	Line3[16] = ':';
	Line3[17] = temperature[9];
	Line3[18] = temperature[10];
}

