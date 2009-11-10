/*-------------------------------------------------------------------------
  Example main program for LCD PCD8544 (LCD of Nokia 3310)


  Copyright (c) 2008, Fandi Gunawan <fandigunawan@gmail.com>
   http://fandigunawan.wordpress.com

   This program is free software; you can redistribute it and/or modify it
   under the terms of the GNU General Public License as published by the
   Free Software Foundation; either version 2, or (__at your option) any
   later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.

   In other words, you are welcome to use, share and improve this program.
   You are forbidden to forbid anyone else to use, share and improve
   what you give them.   Help stamp out software-hoarding!

-------------------------------------------------------------------------*/
/*
   Many thanks to Jakub Lasinski for reviewing and for giving 
   suggestion to my code 
*/


#include <avr/io.h>
#include <avr/pgmspace.h>
#include <util/delay.h>
#include <math.h> 

#include "pcd8544.h"
#include "main.h"
#include "adc.h"

/* globals */
double temperature;

/***************************************************************************
* double2string
* convert a double to a string and place it in a pre-allocated space
***************************************************************************/
inline void double2string (double actualTemp, uint8_t* string)
{
	int temp;

if (actualTemp>-60)
{	
	if (actualTemp<0)
	{
	string[0] = '-';
	string[3] = '.';
	string[5] = '\0';

	temp=(int16_t)(actualTemp * 10.0);   //to include decimal point for display
	if((actualTemp*10.0 - temp) >= 0.5) temp=temp+1;
	if(temp < 0)
	{
	  temp *= -1;
	}
	
	string[4] = ((uint8_t)(temp%10)) | 0x30;
	temp=temp/10;
	
	string[2] = ((uint8_t)(temp%10)) | 0x30;
	temp=temp/10;
	
	string[1] = ((uint8_t)(temp%10)) | 0x30;
	temp=temp/10;
	}
	else
	{
	/* prep the string */

	string[0] = ' ';
	string[3] = '.';
	string[5] = '\0';

	temp=(int16_t)(actualTemp * 10.0);   //to include decimal point for display
	if((actualTemp*10.0 - temp) >= 0.5) temp=temp+1;
	if(temp < 0)
	{
	  temp *= -1;
	}
	
	string[4] = ((uint8_t)(temp%10)) | 0x30;
	temp=temp/10;
	
	string[2] = ((uint8_t)(temp%10)) | 0x30;
	temp=temp/10;
	
	string[1] = ((uint8_t)(temp%10)) | 0x30;
	temp=temp/10;
	}
}
else
{
	string[0] = ' ';
	string[1] = '*';
	string[2] = '*';
	string[3] = '.';
	string[4] = '*';
	string[5] = '\0';
}	

}

/*************************************************************************
* getTempF - return the temp in Farenheight from 
*            Vishay thermistor NTCLE100E3103JB0
* v10bit - 10 bit value read from the A/D
* pdRes - value (in ohms) of the resistor that is in series with thermistor
*************************************************************************/
double getTempF(double v10bit, double pdRes)
{
	if (v10bit == 1024)
		return -1;
	double thermResistance = (pdRes * v10bit / 1024.0)/(1.0 - (v10bit/1024.0));
	//return thermResistance;
	double thermRefResistance = 10000.0;
	// Steinhart and Hart constants for Vishay thermistor NTCLE100E3103JB0
	double a = 3.354016 * pow(10, -3);
	double b = 2.56985 * pow(10, -4);
	double c = 2.620131 * pow(10, -6);
	double d = 6.383091 * pow(10, -8);

	//return v10bit;
	double celcius = 1.0/(a + b * log(thermResistance/thermRefResistance) + c * pow(log(thermResistance/thermRefResistance), 2) + d * pow(log(thermResistance/thermRefResistance), 3)) - 272.15;
	return celcius;
}
 

int main()
{
	uint8_t tmp_string[] = "xx.x"; 
	unsigned char count = 0;
//	DDRD |= (1 << 7); // buzzer
	DDRC &= ~(0x01); // temp sensor on AN2
	
	PORTD = 0;
	DDRD = 0xFF;;
	PORTB = 0;
	DDRB =0xFF;

	/* initialize the ADC */
    adc_init();
	
	LcdInit();


    //display image
    LcdContrast(0x40);
    LcdClear();
    LcdImage(waitImage); 
    LcdUpdate();


    _delay_ms(25);
    LcdContrast(0x40);
    LcdUpdate();
    _delay_ms(25);

    //display string


    _delay_ms(1);

    LcdClear();
	while(1)
	{

	    LcdClear();

		double2string(getTempF(adc_read(T1), 8700), tmp_string);
 		LcdGotoXYFont(1,2);
    	LcdStr(FONT_1X,tmp_string);
 	    LcdGotoXYFont(7,2);
    	LcdFStr(FONT_1X,(unsigned char*)PSTR("Cel"));
 
		double2string(getTempF(adc_read(T2), 8700), tmp_string);
 		LcdGotoXYFont(1,3);
    	LcdStr(FONT_1X,tmp_string);
 	    LcdGotoXYFont(7,3);
    	LcdFStr(FONT_1X,(unsigned char*)PSTR("Cel"));


		double2string(getTempF(adc_read(T3), 8700), tmp_string);
 		LcdGotoXYFont(1,4);
    	LcdStr(FONT_1X,tmp_string);
 	    LcdGotoXYFont(7,4);
    	LcdFStr(FONT_1X,(unsigned char*)PSTR("Cel"));


		double2string(getTempF(adc_read(T4), 8600), tmp_string);
 		LcdGotoXYFont(1,5);
    	LcdStr(FONT_1X,tmp_string);
 	    LcdGotoXYFont(7,5);
    	LcdFStr(FONT_1X,(unsigned char*)PSTR("Cel"));


	 	LcdUpdate();
 
    _delay_ms(1);
	}
    return 0;
}
