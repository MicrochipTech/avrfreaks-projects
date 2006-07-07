//******************************************
//***  RTC Time Calculater with DS1302	 ***
//*** 									 ***
//*** This file calculates the time in   ***
//*** in seconds form 01-01-1970 till	 ***
//*** given time from DS1302 TimeKeeper  ***
//***									 ***
//*** file:DS1302.c						 ***
//***									 ***
//*** (C)2006 by AL Development			 ***
//***									 ***
//*** Ref: 1.00			Creator: Alex	 ***
//******************************************
//
// RTC: Leap years are calculated until 2096.
// RTC: Time is calculated in seconds from 01-01-1970 till given time.
// RTC: for GPS-Buddy we want the time from 01-01-2000 till given time.
// RTC: The amount of seconds from 01-01-1970 till 01-01-2000 = 946684800 seconds = 0x386D4380
// RTC: So withdraw our time with 0x386D4380 and we've timestamp for GPS-Buddy.
//

#include <avr/io.h>
#include "mega48.h"

#define _BASE_YEAR 	  0x46					// 1970 is the base year (70L)
//Define if 'y' is a leap year, else zero
#define leap(y) (((y)%4==0 && (y)%100!=0) || (y)%400==0)
//Define number of leap years from 1970 to 'y'
#define nleap(y) (((y)-0x45)/0x04-((y)-0x01)/0x64 + ((y)-0x12C)/0x190)	//0x45=1969 / 0x01=1901 / 0x12C=1601
//Static data for days in a month comulative
static const short ydays[] = {0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365};
//****************************************************************************************

unsigned char read_byte(unsigned char w_byte)				//read the byte with register w_byte
{
	unsigned char temp;
	reset();
	write(w_byte);
	temp = read();
	PORTC&=~(1<<PC3);			
	PORTC&=~(1<<PC4);			
	return temp;
}

void write_byte(unsigned char w_byte, unsigned char w_2_byte)	//read the byte with register w_byte
{
	reset();
	write(w_byte);
	write(w_2_byte);
	PORTC&=~(1<<PC3);			
	PORTC&=~(1<<PC4);			
}

void reset(void)				//sets the pins to begin and end the ds1302 communication
{
	PORTC|=(1<<PC3);			
	PORTC&=~(1<<PC4);			
	PORTC&=~(1<<PC3);			
	PORTC|=(1<<PC3);		
}

void write(unsigned char W_Byte)	//writes the W_Byte to the DS1302
{
	unsigned char i;
	DDRC |= (1<<PC5)|(1<<PC4)|(1<<PC3);		//0xFF; Do not set all bits because of LM9507

	for(i = 0; i < 8; i++)
	{
		PORTC&=~(1<<PC5);		
		if(W_Byte &0x01)
		{
			PORTC|=(1<<PC5);	
		}
		PORTC|=(1<<PC4);		
		_delay_us(2);
		PORTC&=~(1<<PC4);		
		_delay_us(2);
		W_Byte >>=1;
	}
}

unsigned char read(void)		//reads the ds1302 reply
{
	unsigned char i;
	unsigned char R_Byte, R_Byte2, TmpByte;

	DDRC|=(1<<PC5);				

	R_Byte = 0x00;
	R_Byte2 = 0x00;
	PORTC&=~(1<<PC5);			
	DDRC&=~(1<<PC5);			

	for(i = 0; i < 4; i++) //get the first 4 bits
	{
                TmpByte = 0;
		if(bit_is_set(PINC,io))
                        TmpByte = 1;
		TmpByte <<= 7;
		R_Byte >>= 1;
		R_Byte |= TmpByte;
		PORTC|=(1<<PC4);		
		_delay_us(2);
		PORTC&=~(1<<PC4);		
		_delay_us(2);
	}
	for(i = 0; i < 4; i++) //get the next 3 bits
	{
                TmpByte = 0;
		if(bit_is_set(PINC,io))
                        TmpByte = 1;
		TmpByte <<= 7;
		R_Byte2 >>= 1;
		R_Byte2 |= TmpByte;

		PORTC|=(1<<PC4);		
		_delay_us(2);
		PORTC&=~(1<<PC4);		
		_delay_us(2);
	}
	R_Byte >>= 4;
	R_Byte2 >>= 4;
	R_Byte = (R_Byte2 * 10) + R_Byte;
	return R_Byte;
}

void RTC_calc(void)
{
	int years, months, date, hours, minutes, seconds, jdays;
	xdays = 0x00;								//Reset RTC time
	years = read_byte(year_r) - 0x00;			//Read year - the year 1900. We read years 2-digit so only 0x00
	months = read_byte(month_r);				//Read months 
	date = read_byte(date_r);					//Read days maybe add 1
	hours = read_byte(hour_r);					//Read hours
	minutes = read_byte(min_r);					//Read minutes
	seconds = read_byte(sec_r);					//Read seconds.

/* Now calculate elapsed days since base date (midnight, 01/01/1970, UTC)
   365 days for each elapsed year since 1970, plus one more day for each
   elapsed leap year.
*/
//Check if DS1302 is programmed right. When wrong or empty 'months' will return 0x00
	if (months == 0x00 || months > 0x0C)
		{
		PORTD |=(1<<PD5);						//Switch on error LED.
		}
	months = (months-0x01);						//Change 'months' from 1...12 to 0....11
//Set 'jdays' to the number of days into the year.
	jdays = ydays[months]+ (months > 1 && leap(years));
//Now calculate the number of days from 01-01-1970 till 01-01-given year included leap years
	xdays = (unsigned)(nleap (years + 0x64));	//First determe how many leap years have passed.
	years = ((years + 0x64)-_BASE_YEAR);		//Determe the amount of years from 01-01-1970 till 01-01-given year
//Calculate years * 365 + leap years + days until given month + days of the month	
	xdays = ((((years * 0x16D) + xdays)+ jdays)+ date);	
//**** HERE!: xdays holds all days from 01-01-1970 till given date.
	xdays = ((xdays* 0x18) + hours);			//Calculate elapsed hours 0x18 = 24h 
//**** HERE!: xdays holds elapsed hours from 01-01-1970 till given date and hour. 						
	xdays = ((xdays*0x3C) + minutes);			//Calculate elapsed minutes 0x3C = 60min
//**** HERE!: xdays holds elapsed minutes from 01-01-1970 till given date and hour and minute.
	xdays= ((xdays*0x3C) + seconds);			//Calculate elapsed seconds 0x3C = 60sec.
//**** HERE!: xdays holds elapsed seconds from 01-01-1970 till given date, hour, min and seconds.
	xdays = (xdays-0xE10);						//0xE10 = 3600 = 1hour
//**** HERE!: xdays holds elapsed seconds in GMT time from 01-01-1970 until given time and date.
	xdays = (xdays-0x386D4380); 
//**** HERE!: xdays holds elapsed seconds GMT time with the seconds from 01-01-1970 till 01-01-2000 withdrawn,
//****        This is done because the servers from GPS-Buddy start calculating from 01-01-2000.
//************************************************************************************************************
union
{							//**** HERE!: I'm going to divide 'xdays' into 4 seperate parts for
unsigned long value; 		//****        sending it by uart.
unsigned char bytes[4]; 
}longWithBytes; 
{
longWithBytes.value = xdays; 
rtc_days[0] = longWithBytes.bytes[4]; 
rtc_days[1] = longWithBytes.bytes[3]; 
rtc_days[2] = longWithBytes.bytes[2]; 
rtc_days[3] = longWithBytes.bytes[1]; 

//**** HERE!: I'm going to add all 4 bytes up for cheksum.
check_rtc = (rtc_days[0]+rtc_days[1]+rtc_days[2]+rtc_days[3]); 
}
}


