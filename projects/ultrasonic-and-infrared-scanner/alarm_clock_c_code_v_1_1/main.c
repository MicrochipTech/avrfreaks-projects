/*
Author:  Matt Meerian
Date:  July 11, 2005
Target Processor:  ATMEGA32 from Atmel
Language used:  AVR GCC compiler (in the "C" language)
Overview:
	This file will be used to test the basic operation of the 128x64 pixel,
	2.8", blue and white, LCD from Optrex.  (Part Number F-51553GNBJ-LW-AB)
	The end use for this code is an alarm clock.
Hardware:  009-0009-00 (this is my board)
compiler:  WINAVR avr-gcc (GCC) 3.4.1 (it is listed as 20040720 on the install file)
debugger:  AVR studio 4.11 build 401
Text Editor used:  Programmers Notepad, version: 2.0.5.32
*/

#include <io.h>
#include <interrupt.h>  //used to allow interrupts
#include <sig-avr.h>
#include <stdbool.h>    //this file allows boolean data types in "C"
#include <pgmspace.h>   //must be included to store arrays in FLASH?
#include <eeprom.h>     //used for writing to the EEPROM
#include <wdt.h>        //for the watchdog timer
#include <string.h>		//for strlen
#include <progmem.h>
#include <stdlib.h>		//for the itoa function
//my header files
#include <define.h>		//defines for the current project, of course.
#include <flash.h>		//contains strings that go into flash for the LCD

//subroutine definitions
void integerToArray(unsigned int intNumToString);
void SrvTock(void);
//void write_land_char(unsigned char uWriteLcdChar, bool boolCharInvert);	//write a character to the LCD
void write_command(unsigned char uLcdCommand);    //write a command to the graphical LCD
void write_date(unsigned char uStartColumn,unsigned char uRow, unsigned char uBlank, bool boolInvert);
void write_small_numbers(unsigned int intSmallNum, bool boolBlankZero, unsigned char uNumbersDisplayed);
void ShowLargeTime(unsigned char uStartColumn, unsigned char uRow);
void ShowLargeAlarmTime(unsigned char uStartColumn, unsigned char uRow);
unsigned char not_leap(void); //check for leap year

void InitLCD(void);		//initialize the LCD
void setBacklightOffTime(void);
void setBacklightOnTime(void);


/*---------------------------------------------------------------------------*/

//Declair the data types
typedef unsigned char u08;  //A character, 8 bits
typedef unsigned short u16; //a short integer, big endian 16 bits, 0 to 65535
typedef bool ubool;         //a boolean, true or false, must include the header file "stdbool.h"

/*global varibles*/
//The volatile keyword should be used when a variable is used in an interrupt routine
volatile u16 uTock=defTock;	//A counter used for the 20mS tock
u08 uState=0;		//the state for the state machine
u08 uTmrState=210;	//a state timer
u08 uBtnUpTmr=0;		//A debounce timer for button 1
u08 uBtnDwnTmr=0;		//A debounce timer for button 2
u08 uBtnEnterTmr=0;		//A debounce timer for button 3
u08 uBob=10;
u08 uTmrGeneral=10;
u08 uSelectedLine;		//keeps track of which line the user highlights in the menu
u08 uContrast=0x30;
unsigned char str[5];		//for the interger to string routine
unsigned char *pts;
u08 uTimeSetState;		//a sub-state timer
unsigned int intAlarmMin;
unsigned int intAlarmHour;
bool boolSoundAlarm=false;		//The alarm is set to fire when true
bool boolAlarmOn=false;			//The alarm is going off when true
bool boolPowerupBlink=true;	//Blink the display when the unit is first powered up and the time has not been set
unsigned int intUserPhotoTriggerValue;		//sets the initial user photocell trigger value
u08 uBacklightOnHour;				//This holds the hour the backlight comes on in the morning
u08 uBacklightOffHour;			//This holds the hour the backlight goes off in the evening, 22 is 10pm
unsigned int intPhotocellValue;	//stores the 10 bit ADC photocell value
u08 uRefreshDisplay=0;
const int EEKey __attribute__ ((section (".eeprom"))) = 0xFFFF;
const int EEMinute __attribute__ ((section (".eeprom"))) = 0xFFFF;
const int EEHour __attribute__ ((section (".eeprom"))) = 0xFFFF;
const int EEPhotoTriggerValue __attribute__ ((section (".eeprom"))) = 0xFFFF;
const unsigned char EEBacklightOnHour __attribute__ ((section (".eeprom"))) = 0xFF;
const unsigned char EEBacklightOffHour __attribute__ ((section (".eeprom"))) = 0xFF;


typedef struct{
unsigned int intSec; //enter the current time, date, month, and year
unsigned int intMin;
unsigned int intHour;
unsigned int intDay;
unsigned int intMonth;
unsigned int intYear;
}time;
time t;

/*
const char strOne[] PROGMEM = "Fool";
const char strTwo[] PROGMEM = "heck";

PGM_P array[2] PROGMEM = {
	strOne,
	strTwo
};
*/

#include "lcd.h"		//contains routines specific for the Optrex, 128x64 pixel display
#include "display.h"
#include "menu.h"
#include "misc.h"


//**************************************************************************
//----------------------------------------------------------------------------
//Routine:     signal(sig_overflow0)  (timer 0 overflow interrupt)
//
//Function:    signal handler for tcnt0 overflow interrupt
//              happens every 64.39 microseconds
//Varibles passed:
//       none
//Returns:
//       none
//----------------------------------------------------------------------------
SIGNAL(SIG_OVERFLOW0)
{
	if (uTock > 0)
	{
      uTock--;      //This counts down to a 20ms tock.  (which is run in the main loop)
	}
}

//----------------------------------------------------------------------------
//Routine:     signal(sig_overflow2)  (timer 2 overflow interrupt)
//
//Function:    signal handler for tcnt2 overflow interrupt
//              happens every second.  This is based upon the 32.768 KHz crystal
//Varibles passed:
//       none
//Returns:
//       none
//----------------------------------------------------------------------------
SIGNAL(SIG_OVERFLOW2)	//timer 2 overflow
{
	if(uState!=60)	//when we are in the "set time" state, don't advance the time
	{
		if (++t.intSec==60) //keep track of time, date, month, and year
		{
			t.intSec=0;
			if (++t.intMin==60)
			{
				t.intMin=0;
				if (++t.intHour==24)
				{
					t.intHour=0;
					if (++t.intDay==32)
					{
						t.intMonth++;
						t.intDay=1;
					}
					else if (t.intDay==31)
					{
						if ((t.intMonth==4) || (t.intMonth==6) || (t.intMonth==9) || (t.intMonth==11))
						{
							t.intMonth++;
							t.intDay=1;
						}
					}
					else if (t.intDay==30)
					{
						if(t.intMonth==2)
						{
							t.intMonth++;
							t.intDay=1;
						}
					}
					else if (t.intDay==29)
					{
						if((t.intMonth==2) && (not_leap()))
						{
							t.intMonth++;
							t.intDay=1;
						}
					}
					if (t.intMonth==13)
					{
						t.intMonth=1;
						t.intYear++;
					}
				}
			}
		}
	}
}


SIGNAL(SIG_UART_RECV)
/* signal handler for receive complete interrupt */
{
    //register unsigned char inByte;

    //inByte = inp(UDR);        /* read byte for UART data buffer */
}

//----------------------------------------------------------------------------
//Routine:		SIGNAL(SIG_INTERRUPT0)	(signal handler for external interrupt int0)
//
//Function:
//
//
//Varibles passed:
//       none
//Returns:
//       integer (no idea where this integer is returned to)
SIGNAL(SIG_INTERRUPT0)
{
}


//----------------------------------------------------------------------------
//Routine:     main (the main loop of the program)
//
//Function:    This is where to program will exicute until an interrupt happens or
//              someone yanks the power plug.
//
//Varibles passed:
//       none
//Returns:
//       integer (no idea where this integer is returned to)
//----------------------------------------------------------------------------
// Main loop
int main( void )
{
	wdt_reset();	//reset the watchdog
	uCinit();		//go through the initialization process for the microcontroller
	InitLCD();		//initialize the graphical LCD, lcd.h file
	sei();        // enable interrupts
	sbi(PORTD,defBacklightOn);	//turn on the backlight
	t.intDay=1;		//It is the first day of the month on powerup
	t.intMonth=1;	//It is January on powerup
	t.intYear=5;		//start out at 2005 (the t.year variable only keeps track of the last 3 digits)
	while(1)       // go into an infinite loop here
	{
        SrvTock();        //go to the tock every 20ms
	}
	return 0;
}

//----------------------------------------------------------------------------
//Routine:       not_leap
//
//Function:      This routine is used for determining if it is not a leap year.
//
//Varibles passed:
//       none
//Returns:
//       returns a true if it is not a leap year
//		 returns a false if it is a leap year
//----------------------------------------------------------------------------
unsigned char not_leap(void) //check for leap year
{
	if (!(t.intYear%100))
		return (char)(t.intYear%400);
	else
		return (char)(t.intYear%4);
}

//----------------------------------------------------------------------------
//Routine:       my_itoa  (my interger to array conversion)
//
//Function:      Takes an integer and moves it into an array.
//                Otherwise known as binary to BCD conversion in assembly.
//                This routine was copied off of a newsgroup.
//                The binary value is passed to the routine and the destination
//                is a string called str
//
//Varibles passed:
//       arg:  a character to be converted to BCD
//Returns:
//       none
//----------------------------------------------------------------------------
void integerToArray(unsigned int intNumToString)
{
    int minus = 0;
    int d;
    char *ptr;

    strcpy (str,"0000");
    ptr = &str[sizeof(str) -1];
    *ptr-- = '\0';
    if (intNumToString < 0)
    {
          minus = 1; intNumToString = -intNumToString;
    }
    while (intNumToString > 0)
    {
         d = intNumToString % 10;
         *ptr-- = d + '0';
         intNumToString /= 10;
     }
    if (minus)
        *ptr = '-';
    else
        ptr++;
}


//----------------------------------------------------------------------------
//Routine:       SrvTock  (Service the tock)
//
//Function:      Exicute the tock every 20mS from the main loop.
//
//
//Varibles passed:
//       none
//Returns:
//       none
//----------------------------------------------------------------------------
void SrvTock(void)
{
    //unsigned char uCanteen;
	//bool boolNewData=false;

    if (uTock == 0)
    {
        uTock = defTock;   //reload the Tock (20mS) timer
        if(uBtnUpTmr!=0)     //the debounce timer for Menu Button 1
        {
            uBtnUpTmr--;     //decrement the menu button 1 debounce timer
        }
        if(uBtnDwnTmr!=0)     //the debounce timer for Menu Button 2
        {
            uBtnDwnTmr--;     //decrement the menu button 2 debounce timer
        }
        if(uBtnEnterTmr!=0)     //the debounce timer for Menu Button 3
        {
            uBtnEnterTmr--;     //decrement the menu button 3 debounce timer
        }

        switch (uState)
        {
            case 0:  //Does the current state match state 0?
                if(uTmrState==200)
                {
					stop_image();		//write the stop image
				}
                if(uTmrState==100)
                {
                    kustom_image();     //write the logo to the screen
				}
                uTmrState--;

                if(uTmrState==0)
                {
					clear_display();	//clear the whole display
					uTmrState=1;
					uState=2;
					//write_eagle_display();		//This writes a  big 1 through 9 to the display
                }
            break;

			case 2:		//This shows the main clock display
                if(uTmrState==0)
                {
					//uTmrState = 255;	//Go in here every (.02)(255) = 5.1 seconds
					if(boolPowerupBlink==true)
					{
						uTmrState = 75;		//update the display at much higher rate when the time has not been set
						sbi(defPORTD,defBacklightOn);	//always have the backlight on when we are  in the powerup flashing
					}
					else
					{
						uTmrState = 255;	//Go in here every (.02)(255) = 5.1 seconds, update the display
						if(uBacklightOffHour<=t.intHour || uBacklightOnHour>t.intHour)	//is it nighttime?
						{
							if(intPhotocellValue>intUserPhotoTriggerValue)	//is it dark out?
							{
								cbi(defPORTD,defBacklightOn);	//turn off the backlight
							}
							else	//it is not dark out
							{
								sbi(defPORTD,defBacklightOn);	//so, turn on the backlight
							}
						}
						else	//it is not nighttime, so turn on the backlight during the day
						{
							sbi(defPORTD,defBacklightOn);	//turn on the backlight
						}
					}

					if(t.intHour<8)	//This moves the time and date every 8 hours to prevent screen burn in
					{
						if(uRefreshDisplay!=0)	//when we enter this 8 hour time period, the display needs to be cleared for the new data
						{
							uRefreshDisplay=0;	//do not go in here again for this 8 hour period
							clear_display();	//clear the whole display
						}
						ShowLargeTime(20,3);		//uStartColumn, uRow
						if(boolAlarmOn==false)	//Is the alarm off?
						{
							drawText((unsigned char *)strAlarmOff,40,6,false);	//show alarm on/off state (column, row)
						}
						else
						{
							drawText((unsigned char *)strAlarmOn,40,6,false);	//show that the alarm is on (column, row)
						}

						write_date(30,7,0,false);	//show the month, day, and year (column, row, numbers to blank)
					}
					else if(t.intHour<16)	//This moves the time and date every 8 hours to prevent screen burn in
					{
						if(uRefreshDisplay!=1)	//when we enter this 8 hour time period, the display needs to be cleared for the new data
						{
							uRefreshDisplay=1;	//do not go in here again for this 8 hour period
							clear_display();	//clear the whole display
						}
						ShowLargeTime(0,2);		//uStartColumn, uRow
						if(boolAlarmOn==false)	//Is the alarm off?
						{
							drawText((unsigned char *)strAlarmOff,0,6,false);	//show alarm on/off state (column, row)
						}
						else
						{
							drawText((unsigned char *)strAlarmOn,0,6,false);	//show that the alarm is on (column, row)
						}
						write_date(0,7,0,false);	//show the month, day, and year (column, row, numbers to blank)
					}
					else	//This moves the time and date every 8 hours to prevent screen burn in
					{
						if(uRefreshDisplay!=2)	//when we enter this 8 hour time period, the display needs to be cleared for the new data
						{
							uRefreshDisplay=2;	//do not go in here again for this 8 hour period
							clear_display();	//clear the whole display
						}
						ShowLargeTime(0,0);		//uStartColumn, uRow
						if(boolAlarmOn==false)	//Is the alarm off?
						{
							drawText((unsigned char *)strAlarmOff,10,6,false);	//show alarm on/off state (column, row)
						}
						else
						{
							drawText((unsigned char *)strAlarmOn,10,6,false);	//show that the alarm is on (column, row)
						}
						write_date(defTab,7,0,false);	//show the month, day, and year (column, row, numbers to blank)
					}
					if((intAlarmHour==t.intHour) && (intAlarmMin==t.intMin) && (boolAlarmOn==true))
					{	//sound the alarm if it is correct hour, minute, and if the alarm is enabled
						boolSoundAlarm=true;	//sound the alarm!!!
					}
                }
				else
				{
	                uTmrState--;	//count down to the next time we can update the LCD display

					if(uTmrState==15)
					{
						ADMUX=0;	//select analog channel 0 for the ADC conversion
						ADCSRA|=0x40;	//start the ADC conversion process
					}
					if(uTmrState==10)
					{
						uTmrGeneral=40;
						ADCSRA|=0x10;	//return the ADC, the 10 bit value is stored in the integer ADC
						intPhotocellValue=ADC;	//get the ADC value and store it
					}
					if(uTmrState==50 && boolPowerupBlink==true)	//blink the whole display when the unit has just
					{		// been powered up and the time has not been set
						clear_display();	//clear the whole display
					}
					if((boolSoundAlarm==true) && (uTmrState<15))	//Is it time to turn on the alarm, boolAlarmOn
					{							//	for the alarm clock?
						sbi(defPORTD,defAlarmPin);	//take the I/O pin high to sound the alarm
					}
					else
					{
						cbi(defPORTD,defAlarmPin);	//this turns off the alarm for a few seconds
					}
				}
                if(bit_is_clear(defPushBtnPort,defUpBtn) && uBtnUpTmr==0)  //is the Menu button 1 pressed?
				{
					boolSoundAlarm=false;	//when any button is pressed, turn off the alarm
					boolPowerupBlink=false;		//don't blink the time on powerup anymore
					cbi(defPORTD,defAlarmPin);	//this turns off the alarm off, the bool value above too
					sbi(defPORTD,defBacklightOn);	//turn on the backlight, when a button is pressed
					uBtnUpTmr=defBtnDbn;   //load up the debounce timer, so we don't immediately go back in here
				}
                if(bit_is_clear(defPushBtnPort,defDownBtn) && uBtnDwnTmr==0)  //is the Menu button 2 pressed?
				{
					boolSoundAlarm=false;	//when any button is pressed, turn off the alarm
					boolPowerupBlink=false;		//don't blink the time on powerup anymore
					cbi(defPORTD,defAlarmPin);	//this turns off the alarm off, the bool value above too
					sbi(defPORTD,defBacklightOn);	//turn on the backlight, when a button is pressed
					uBtnDwnTmr=defBtnDbn;   //load up the debounce timer, so we don't immediately go back in here
				}
                if(bit_is_clear(defPushBtnPort,defEnterBtn) && uBtnEnterTmr==0)  //is the Menu button 2 pressed?
				{
					sbi(defPORTD,defBacklightOn);	//turn on the backlight, when a button is pressed
					boolPowerupBlink=false;		//don't blink the time since the menu has been entered
					if(boolSoundAlarm == true)	//is the alarm sounding?
					{	//the alarm is sounding, turn it off!
						boolSoundAlarm=false;	//when any button is pressed, turn off the alarm
						cbi(defPORTD,defAlarmPin);	//this turns off the alarm off, the bool value above too
					}
					else	//the alarm is not sounding
					{
						uBtnEnterTmr=defBtnDbn;   //load up the debounce timer, so we don't immediately go back in here
						uState=50;
						uSelectedLine=1;	//start out with the alarm set button highlighted
					}
                    uTmrState=defStateTm;       //reload the state timer to stay in this state
				}
			break;

			case 50:	//displays the main menu, allows the user to make a selection from the main menu
				if(bit_is_clear(defPushBtnPort,defUpBtn) && uBtnUpTmr==0)  //is the Menu button 1 pressed?
				{
					uSelectedLine--;
					paintMenu((unsigned char *)str50,defState50HighlightMin,defState50HighlightMax);
					uBtnUpTmr=defBtnDbn;	//load up the debounce timer, so we don't immediately go back in here
				}
				if(bit_is_clear(defPushBtnPort,defDownBtn) && uBtnDwnTmr==0)  //is the Menu button 2 pressed?
				{
					uSelectedLine++;
					paintMenu((unsigned char *)str50,defState50HighlightMin,defState50HighlightMax);
					uBtnDwnTmr=defBtnDbn;	//load up the debounce timer, so we don't immediately go back in here
				}
				if(bit_is_clear(defPushBtnPort,defEnterBtn) && uBtnEnterTmr==0)  //is the Menu button 2 pressed?
				{
			        switch (uSelectedLine)	//the set button was pressed
					{
						case 1:		//Set the Alarm On/off
							if(boolAlarmOn==true)	//was the alarm on?
							{
								boolAlarmOn=false;	//if so, then turn off the alarm
							}
							else
							{
								boolAlarmOn=true;	//the alarm was not on, so turn on the alarm
							}
							uTmrState=3;	//This lets state 2 paint the screen correctly
							uState=0;		//go back to main clock display, clearing the display first
						break;
						case 2:		//Set the Clock
							clear_display();	//clear the whole display
							uTimeSetState=0;	//start out in the set the hour state
							uState=60;		//move to the set-the-clock state
							uTmrState=5;
							drawText((unsigned char *)strSetTheTime,0,0,false);		//show the "set the time" title
						break;
						case 3:		//Set the Alarm time
							clear_display();	//clear the whole display
							uTimeSetState=0;	//start out in the set the alarm hour state
							uState=61;		//move to the set-the-alarm state
							uTmrState=5;
							drawText((unsigned char *)strSetTheAlarm,0,0,false);		//show the "set the Alarm" title
						break;
						case 4:		//set the backlight times
							uState=62;
							clear_display();	//clear the whole display
							uTimeSetState=0;	//set the "backlight off time" first
							uTmrState=defStateTm+2;		//allow the new state to paint the new screen
						break;
						case 5:		//set the photo trigger value
							uState=63;
							clear_display();	//clear the whole display
							uTmrState=defStateTm+2;		//allow the new state to paint the new screen
						break;
						case 6:		//Show the "About this clock" information
							uState=64;
							uTmrState=5;
						break;
						case 7:		//the exit button
							uTmrState=3;	//This lets state 0 paint the screen correctly
							uState=0;	//go back to the main display, clearing the display first
						break;
					}
					uBtnEnterTmr=defBtnDbn;   //load up the debounce timer, so we don't immediately go back in here
				}
				if(uTmrState==defStateTm)
				{
					clear_display();	//clear the whole display
					paintMenu((unsigned char *)str50,defState50HighlightMin,defState50HighlightMax);	//string, column, line, inverted
					uTmrState--;	//don't go in here again
				}
			break;

			//uTimeSetState = 0 set the hour
			//uTimeSetState = 1 set the minute
			//uTimeSetState = 2 set the month
			//uTimeSetState = 3 set the day
			//uTimeSetState = 4 set the year
			case 60:	//We are going to set the time
				//has the up button been pressed?
				if(bit_is_clear(defPushBtnPort,defUpBtn) && uBtnUpTmr==0)  //is the Menu button 1 pressed?
				{
					switch(uTimeSetState)	//what are we going to set?
					{
						case 0:		//set the hour
							if(t.intHour<23)		//have we reached the maximum hour count?
							{
								t.intHour++;		//no, increment the hour
							}
							else
							{
								t.intHour=0;		//reset the hour count since we have reached the 23rd hour
							}
							ShowLargeTime(0,2);		//uStartColumn, uRow

						break;
						case 1:		//set the minute
							if(t.intMin<59)		//have we reached the maximum minute count?
							{
								t.intMin++;		//no, then increment the minute
							}
							else
							{
								t.intMin=0;		//reset the hour count since we have reached the 59th minute
							}
							ShowLargeTime(0,2);		//uStartColumn, uRow
						break;
						case 2:		//set the month
							if(t.intMonth<12)		//have we reached passed december?
							{
								t.intMonth++;		//no, then increment the month
							}
							else
							{
								t.intMonth=1;		//yes, then reset the month to January
							}
							write_date(defTab,7,0,false);
						break;
						case 3:		//set the day
							if(t.intDay<31)		//have we reached the maximum day?
							{
								t.intDay++;		//no, then increment the day
							}
							else
							{
								t.intDay=1;		//yes, then reset the day to the start of the month
							}
							write_date(defTab,7,0,false);
						break;
						case 4:		//set the year
							if(t.intYear<999)		//have we reached the maximum year number?
							{
								t.intYear++;		//no, then increment the year
							}
							write_date(defTab,7,0,false);
						break;
					}
					uTmrState=defDlyAfterBtnPress; //load up the blink timer for the range level number
					uBtnUpTmr=defBtnDbn;	//load up the debounce timer, so we don't immediately go back in here
				}
				//has the down button been pressed?
				if(bit_is_clear(defPushBtnPort,defDownBtn) && uBtnDwnTmr==0)  //is the Menu button 2 pressed?
				{
					switch(uTimeSetState)	//what are we going to set?
					{
						case 0:		//set the hour
							if(t.intHour!=0)		//have we reached the minimum day?
							{
								t.intHour--;		//no, then decrement the day
							}
							else
							{
								t.intHour=23;		//yes, then rese the hour to the highest possible value
							}
							ShowLargeTime(0,2);		//uStartColumn, uRow
						break;
						case 1:		//set the minute
							if(t.intMin!=0)		//have we reached the minimum minute?
							{
								t.intMin--;		//no, then decrement the minute
							}
							else
							{
								t.intMin=59;		//yes, then reset the minute to highest possible value
							}
							ShowLargeTime(0,2);		//uStartColumn, uRow
						break;
						case 2:		//set the month
							if(t.intMonth!=1)		//have we reached the minimum month
							{
								t.intMonth--;			//no, then decrement the month
							}
							else
							{
								t.intMonth=12;		//yes, then reset the month to decmenber
							}
							write_date(defTab,7,0,false);
						break;
						case 3:		//set the day
							if(t.intDay!=1)		//have we reached the minimum day?
							{
								t.intDay--;		//no, then decrement the day
							}
							else
							{
								t.intDay=31;		//yes, then reset the day to the highest possible value
							}
							write_date(defTab,7,0,false);
						break;
						case 4:		//set the year
							if(t.intYear>1)		//have we reached before 2005?  (intYear only holds the last 3 digits of the current year)
							{
								t.intYear--;		//no, then decrement the year
							}
							write_date(defTab,7,0,false);
						break;
					}
					uTmrState=defDlyAfterBtnPress; //load up the blink timer for the range level number
					uBtnDwnTmr=defBtnDbn;	//load up the debounce timer, so we don't immediately go back in here
				}
				//has the enter button been pressed?
				if(bit_is_clear(defPushBtnPort,defEnterBtn) && uBtnEnterTmr==0)  //is the Menu button 2 pressed?
				{		//exit out of the set time
					uTimeSetState++;
					if (uTimeSetState == 2)	//refresh the time when we go to change the month
					{
						ShowLargeTime(0,2);		//uStartColumn, uRow
					}
					uTmrState=defSettingBlinkOff+1;		//as soon as the the enter button is pressed, the next thing to change will start blinking
					if(uTimeSetState > 4)
					{
						uState=50;	//go back to the main menu state
						uSelectedLine=defState50HighlightMax;	//set the highlighted line for the next state
						uTmrState=defStateTm+1;       //reload the state timer to stay in this state
					}
					uBtnEnterTmr=defBtnDbn;   //load up the debounce timer, so we don't immediately go back in here
				}
				uTmrState--;	//count down to a blink for the range number
				if(uTmrState==0)
				{
					uTmrState=defSettingBlinkOff;	//reset the range blinking number
				}
				if(uTmrState==defSettingBlinkOff)	//this code turns off the character that is blinking
				{
					switch(uTimeSetState)	//what are we going to set?
					{
						case 0:		//set the hour
							BigDigits(2,0,0,0,false);
						break;
						case 1:		//set the minute
							BigDigits(2,61,0,0,false);		//the column was 60
						break;
						case 2:		//set the month
							write_date(defTab,7,1,false);
						break;
						case 3:		//set the day
							write_date(defTab,7,2,false);
						break;
						case 4:		//set the year
							write_date(defTab,7,3,false);
						break;
					}
				}
				if(uTmrState==defSettingBlinkOn)	//this code turns on the character that is blinking
				{
					switch(uTimeSetState)	//what are we going to set?
					{
						case 0:		//set the hour
							ShowLargeTime(0,2);		//uStartColumn, uRow
							write_date(defTab,7,0,false);
						break;
						case 1:		//set the minute
							ShowLargeTime(0,2);		//uStartColumn, uRow
						break;
						case 2:		//set the month
							write_date(defTab,7,0,false);
						break;
						case 3:		//set the day
							write_date(defTab,7,0,false);
						break;
						case 4:		//set the year
							write_date(defTab,7,0,false);
						break;
					}
				}				
			break;

			//uTimeSetState = 0 set the hour
			//uTimeSetState = 1 set the minute
			//intAlarmMin
			//intAlarmHour
			case 61:	//We are going to set the alarm clock time
				//has the up button been pressed?
				if(bit_is_clear(defPushBtnPort,defUpBtn) && uBtnUpTmr==0)  //is the Menu button 1 pressed?
				{
					switch(uTimeSetState)	//what are we going to set?
					{
						case 0:		//set the hour
							if(intAlarmHour<23)		//have we reached the maximum hour count?
							{
								intAlarmHour++;		//no, increment the hour
							}
							else	
							{
								intAlarmHour=0;		//reset the hour count since we have reached the 23rd hour
							}
							ShowLargeAlarmTime(0,2);		//uStartColumn, uRow

						break;
						case 1:		//set the minute
							if(intAlarmMin<59)		//have we reached the maximum minute count?
							{
								intAlarmMin++;		//no, then increment the minute
							}
							else		
							{
								intAlarmMin=0;		//reset the hour count since we have reached the 59th minute
							}
							ShowLargeAlarmTime(0,2);		//uStartColumn, uRow
						break;
					}
					uTmrState=defDlyAfterBtnPress; //load up the blink timer for the range level number
					uBtnUpTmr=defBtnDbn;	//load up the debounce timer, so we don't immediately go back in here
				}
				//has the down button been pressed?
				if(bit_is_clear(defPushBtnPort,defDownBtn) && uBtnDwnTmr==0)  //is the Menu button 2 pressed?
				{
					switch(uTimeSetState)	//what are we going to set?
					{
						case 0:		//set the hour
							if(intAlarmHour!=0)		//have we reached the minimum day?
							{
								intAlarmHour--;		//no, then decrement the day
							}
							else
							{
								intAlarmHour=23;		//yes, then rese the hour to the highest possible value
							}
							ShowLargeAlarmTime(0,2);		//uStartColumn, uRow
						break;
						case 1:		//set the minute
							if(intAlarmMin!=0)		//have we reached the minimum minute?
							{
								intAlarmMin--;		//no, then decrement the minute
							}
							else
							{
								intAlarmMin=59;		//yes, then reset the minute to highest possible value
							}
							ShowLargeAlarmTime(0,2);		//uStartColumn, uRow
						break;
					}
					uTmrState=defDlyAfterBtnPress; //load up the blink timer for the range level number
					uBtnDwnTmr=defBtnDbn;	//load up the debounce timer, so we don't immediately go back in here
				}
				//has the enter button been pressed?
				if(bit_is_clear(defPushBtnPort,defEnterBtn) && uBtnEnterTmr==0)  //is the Menu button 2 pressed?
				{		//exit out of the set time
					uTimeSetState++;
					uTmrState=defSettingBlinkOff+1;		//as soon as the the enter button is pressed, the next thing to change will start blinking
					if(uTimeSetState > 1)
					{
						eeprom_write_word((uint16_t*)&EEHour,intAlarmHour);	//store the alarm hour in EEPROM when the user has set a new value
						eeprom_write_word((uint16_t*)&EEMinute,intAlarmMin);	//store the alarm minute in EEPROM when the user has set a new value
						uState=50;	//go back to the main menu state
						uSelectedLine=defState50HighlightMax;	//set the highlighted line for the next state
						uTmrState=defStateTm+1;       //reload the state timer to stay in this state
					}
					uBtnEnterTmr=defBtnDbn;   //load up the debounce timer, so we don't immediately go back in here
				}
				uTmrState--;	//count down to a blink for the range number
				if(uTmrState==0)
				{
					uTmrState=defSettingBlinkOff;	//reset the range blinking number
				}
				if(uTmrState==defSettingBlinkOff)	//this code turns off the character that is blinking
				{
					switch(uTimeSetState)	//what are we going to set?
					{
						case 0:		//set the hour
							BigDigits(2,0,0,0,false);
						break;
						case 1:		//set the minute
							BigDigits(2,61,0,0,false);		//the column was 60
						break;
					}
				}
				if(uTmrState==defSettingBlinkOn)	//this code turns on the character that is blinking
				{
					switch(uTimeSetState)	//what are we going to set?
					{
						case 0:		//set the hour
							ShowLargeAlarmTime(0,2);		//uStartColumn, uRow
						break;
						case 1:		//set the minute
							ShowLargeAlarmTime(0,2);		//uStartColumn, uRow
						break;
					}
				}				
			break;

			case 62:	//sets the backlight on and off times
                if(bit_is_clear(defPushBtnPort,defUpBtn) && uBtnUpTmr==0)  //is the Menu button 1 pressed?
                {
					if(uTimeSetState==0)	//are we setting the backlight off time?
					{
						if(uBacklightOffHour==23)	//is it time to cycle the hour back around?
						{
							uBacklightOffHour=0;
						}
						else
						{
							uBacklightOffHour++;	//increase the hour
						}
						setBacklightOffTime();		//show the new backlight off hour on the display
					}
					if(uTimeSetState==1)	//are we setting the backlight on time?
					{
						if(uBacklightOnHour==23)	//is it time to cycle the backlight on hour around?
						{
							uBacklightOnHour=0;
						}
						else
						{
							uBacklightOnHour++;		//increase the backlight on hour
						}
						setBacklightOnTime();	//update the backlight on time on the the display
					}
                    uTmrState=defDlyAfterBtnPress;  //load up the blink timer
                    uBtnUpTmr=defBtnDbn;    //load up the debounce timer, so we don't immediately go back in here
				}
                //has the down button been pressed?
                if(bit_is_clear(defPushBtnPort,defDownBtn) && uBtnDwnTmr==0)  //is the Menu button 2 pressed?
                {
					if(uTimeSetState==0)
					{
						if(uBacklightOffHour==0)
						{
							uBacklightOffHour=23;
						}
						else
						{
							uBacklightOffHour--;
						}
						setBacklightOffTime();
					}
					if(uTimeSetState==1)
					{
						if(uBacklightOnHour==0)
						{
							uBacklightOnHour=23;
						}
						else
						{
							uBacklightOnHour--;
						}
						setBacklightOnTime();
					}
                    uTmrState=defDlyAfterBtnPress;  //load up the blink timer
                    uBtnDwnTmr=defBtnDbn;   //load up the debounce timer, so we don't immediately go back in here
				}                
				//has the enter button been pressed?
                if(bit_is_clear(defPushBtnPort,defEnterBtn) && uBtnEnterTmr==0)  //is the Menu button 2 pressed?
                {   //pressing the enter button takes us back to the main menu
					if(uTimeSetState==0)
					{
						uTimeSetState=1;	//go allow the user to set the backlight on time
						//if the display is blinking this makes sure the value is written when we go to set the backlight off time
						setBacklightOffTime();	//update the backlight on time on the the display
					}
					else
					{
						eeprom_write_byte((u08*)&EEBacklightOnHour,uBacklightOnHour);	//store the backlight Hour on value in EEPROM
						eeprom_write_byte((u08*)&EEBacklightOffHour,uBacklightOffHour);//store the backlight on hour in EEPROM
						uState=50;      //go back to the Main Menu
						uTmrState=defStateTm+1;       //reload the state timer
						uSelectedLine=defState50HighlightMax;	//when we get back to the main menu, the exit button will be highlighted
					}
                    uBtnEnterTmr=defBtnDbn;   //load up the debounce timer, so we don't immediately go back in here
                }				
                //from here to the break blinks the Trigger Value
                uTmrState--;    //count down to a blink
                if(uTmrState==0)
                {
                    uTmrState=defSettingBlinkOff;   //reset the number blinking timer
                }
                if(uTmrState==defSettingBlinkOff)   //turn off the number
                {
					if(uTimeSetState==0)
					{
						set_line(2);	//The trigger value is on line 5
						write_pixel_spaces(33); //write over the digits that are not used anymore
					}
					else
					{
						set_line(5);	//The trigger value is on line 5
						write_pixel_spaces(33); //write over the digits that are not used anymore
					}
                }
                if(uTmrState==defSettingBlinkOn)    //make the minimum Speed number visible
                {
					if(uTimeSetState==0)
					{
						setBacklightOffTime();
					}
					else
					{
						setBacklightOnTime();
					}
                }
				if(uTmrState>defStateTm)	//paint the new screen for the backlight on and off times
				{
					drawText((unsigned char *)strBacklightOffTime,0,0,false);	//colum, row, no highlight
					drawText((unsigned char *)strUsuallyTen,defTab,1,false);	//colum, row, no highlight
					setBacklightOffTime();
					drawText((unsigned char *)strBacklightOnTime,0,3,false);	//colum, row, no highlight
					drawText((unsigned char *)strUsuallySix,defTab,4,false);	//colum, row, no highlight
					setBacklightOnTime();
					uTmrState=defSettingBlinkOff+1;	//don't go in here again and start the blining of the active backlight off time
				}			
			break;

			case 63:	//allows the user to change the photo trigger value
                if(bit_is_clear(defPushBtnPort,defUpBtn) && uBtnUpTmr==0)  //is the Menu button 1 pressed?
                {
					if(intUserPhotoTriggerValue<975)
					{
						intUserPhotoTriggerValue=intUserPhotoTriggerValue+25;
					}
					set_line(5);
                    write_pixel_spaces(20);
					write_small_numbers(intUserPhotoTriggerValue,false,3);	//

                    uTmrState=defDlyAfterBtnPress;  //load up the blink timer
                    uBtnUpTmr=defBtnDbn;    //load up the debounce timer, so we don't immediately go back in here
				}
                //has the down button been pressed?
                if(bit_is_clear(defPushBtnPort,defDownBtn) && uBtnDwnTmr==0)  //is the Menu button 2 pressed?
                {
					if(intUserPhotoTriggerValue>25)
					{
						intUserPhotoTriggerValue=intUserPhotoTriggerValue-25;
					}
					set_line(5);
                    write_pixel_spaces(20);
					write_small_numbers(intUserPhotoTriggerValue,false,3);	//
                    uTmrState=defDlyAfterBtnPress;  //load up the blink timer
                    uBtnDwnTmr=defBtnDbn;   //load up the debounce timer, so we don't immediately go back in here
				}
                //has the enter button been pressed?
                if(bit_is_clear(defPushBtnPort,defEnterBtn) && uBtnEnterTmr==0)  //is the Menu button 2 pressed?
                {   //pressing the enter button takes us back to the main menu
					eeprom_write_word((uint16_t*)&EEPhotoTriggerValue,intUserPhotoTriggerValue);	//store the phototrigger value in EEPROM when the user has set a new value
                    uState=50;      //go back to the Main Menu
                    uTmrState=defStateTm+1;       //reload the state timer
					uSelectedLine=defState50HighlightMax;	//when we get back to the main menu, the exit button will be highlighted
                    uBtnEnterTmr=defBtnDbn;   //load up the debounce timer, so we don't immediately go back in here
                }
                //from here to the break blinks the Trigger Value
                uTmrState--;    //count down to a blink
                if(uTmrState==0)
                {
                    uTmrState=defSettingBlinkOff;   //reset the number blinking timer
                }
                if(uTmrState==defSettingBlinkOff)   //turn off the number
                {
					set_line(5);	//The trigger value is on line 5
                    write_pixel_spaces(90); //write over the digits that are not used anymore
					ADMUX=0;	//select analog channel 0 for the ADC conversion
					ADCSRA|=0x40;	//start the ADC conversion process
                }
                if(uTmrState==defSettingBlinkOn)    //make the minimum Speed number visible
                {
					uTmrGeneral=40;
					ADCSRA|=0x10;	//return the ADC, the 10 bit value is stored in the integer ADC
                    drawText((unsigned char *)strPhotocellReading,defTab,3,false);	//put up the label for the 
					write_char(0x00,false);	//A white space 
					write_small_numbers(ADC,false,3);	//write the 3 digit ADC
					drawText((unsigned char *)strSetTriggerValue,defTab,4,false);	//colum, row, no highlight
					set_line(5);
                    write_pixel_spaces(20);
					write_small_numbers(intUserPhotoTriggerValue,false,3);	//
                }
				if(uTmrState>defStateTm)	//paint the new screen
				{
					drawText((unsigned char *)strPhotoTriggerValue,0,0,false);	//colum, row, no highlight
					drawText((unsigned char *)strDarkRoom,defTab,1,false);	//colum, row, no highlight
					drawText((unsigned char *)strBrightRoom,defTab,2,false);	//colum, row, no highlight
					drawText((unsigned char *)strPhotocellReading,defTab,3,false);	//colum, row, no highlight
					drawText((unsigned char *)strSetTriggerValue,defTab,4,false);	//colum, row, no highlight
					uTmrState=defSettingBlinkOff+1;	//don't go in here again and 
				}
			break;

			case 64:	//This puts up the "about this clock" information
				if(bit_is_clear(defPushBtnPort,defUpBtn) && uBtnUpTmr==0)  //is the Menu button 1 pressed?
				{				
					uTmrState=defStateTm;
					uState=50;			//go back to the main menu
					uBtnUpTmr=defBtnDbn;   	//load up the debounce timer, so we don't immediately go back in here
				}
				if(bit_is_clear(defPushBtnPort,defDownBtn) && uBtnDwnTmr==0)  //is the Menu button 2 pressed?
				{
					uTmrState=defStateTm;
					uState=50;			//go back to the main menu
					uBtnDwnTmr=defBtnDbn;   	//load up the debounce timer, so we don't immediately go back in here
				}
				if(bit_is_clear(defPushBtnPort,defEnterBtn) && uBtnEnterTmr==0)  //is the Menu button 2 pressed?
				{	
					uTmrState=defStateTm;
					uState=50;			//go back to the main menu
					uBtnEnterTmr=defBtnDbn;   	//load up the debounce timer, so we don't immediately go back in here
				}
				if(uTmrState==5)
				{
					clear_display();	//clear the whole display
					paintMenu((unsigned char *)str64,defState64HighlightMin,defState64HighlightMax);	//string, column, line, inverted
					uTmrState--;	//don't go in here again
				}
			break;

		}
	}
}

void InitLCD(void)
{
//	u08 uEVR=0x20;
//	u08 uRR=0x27;
	
	sbi(defLCDContPort,defLcdRES);       //take the display out of reset

	write_command(0xe2);		//reset display, datasheet
	//write_command(0xa2);		//lcd bias setting, not used for the old oprex display!!!!
	write_command(0xa1);		//reverse segment driver select, must be set to 0xa1.
	write_command(0xc0);		//common output normal, datasheet
	write_command(0xa6);
	//write_command(0xc8);
	#ifdef defS1D15605			//older Optrex displays, F-51553
		write_command(0x24);	//v5 resistor ratio, S1D15605 driver IC
	#else						//new Optrex Displays, F-51852
		write_command(0xe6);	//Driver on/off, NJU6676 driver IC
	#endif
	write_command(0x81);		//electronic volume set, datasheet
	#ifdef defS1D15605			//older Optrex displays, F-51553
		write_command(0x10);		//electronic volume init, datasheet
	#else						//new Optrex Displays, F-51852
		write_command(0x20);	//the second part of the electronic volume set
	#endif
	write_command(0x2f);		//power control, datasheet:  turn on everything
	write_command(0x40);		//start line set, datasheet:  start on line address 0
	write_command(0xb0);		//page address set, datasheet: page address 0
	write_command(0x10);		//column address high, datasheet
	write_command(0x00);		//column address low, datasheet
	write_command(0xaf);		//turn on the display, in datasheet
	#ifdef defS1D15605			//older Optrex displays, F-51553
		write_command(0x1b);	//v5 resistor ratio, S1D15605 driver IC
	#else						//new Optrex Displays, F-51852
		write_command(uRR);	//resistor ratio, was 0x27!!!!!!
		write_command(0xe7);	//Driver on/off, NJU6676 driver IC
	#endif
	write_command(0x81);		//electronic volume set
	#ifdef defS1D15605			//older Optrex displays, F-51553
		write_command(0x30);	//the second part of the electronic volume set
	#else						//new Optrex Displays, F-51852
		write_command(uEVR);	//the second part of the electronic volume set, was 0x20!!!!!
	#endif

/*
	//These are the commands that TVI Electronics uses to initialize the Optrex LCD
	write_command(0xA3);	//LCD bias 1/7th (0xA3)
	write_command(0xA1);	//ADC select reverse
	write_command(0xC0);	//common output normal
	write_command(0xA6);	//display normal
	write_command(0x26);	//V5 resistor ratio
	write_command(0x81);	//electronic volume set
	write_command(0x20);	//electronic volume init
	write_command(0x2F);	//power control set
	
//	write_command(0x40);		//start line set, datasheet:  start on line address 0
//	write_command(0xb0);		//page address set, datasheet: page address 0
//	write_command(0x10);		//column address high, datasheet
//	write_command(0x00);		//column address low, datasheet
	
	write_command(0xAF);	//display on
*/
}

//----------------------------------------------------------------------------
//Routine:     write_command (write a command to the graphical LCD)
//
//Function:
//			This function serially writes a command to the Optrex LCD
//Varibles passed:
//       uLcdCommand:  The control command to send to the LCD
//Returns:
//       none
//----------------------------------------------------------------------------
void write_command(unsigned char uLcdCommand)
{
    sbi(defLCDContPort,defLcdRD);       //change state on the read line of the LCD
    cbi(defLCDContPort,defLcdA0);       //clear A0 on the LCD for a command
    cbi(defLCDContPort,defLcdCS1);      //clear the Chip Select on the LCD for
    cbi(defLCDContPort,defLcdWR);       //change state on the Write line of the LCD
	send_serial_data(uLcdCommand);					//send the data to the LCD serially
    sbi(defLCDContPort,defLcdWR);		//we are done writing to the LCD
    sbi(defLCDContPort,defLcdCS1);      //now set the Chip Select on the LCD
    cbi(defLCDContPort,defLcdRD);       //change state on the read line of the LCD
}

//----------------------------------------------------------------------------
//Routine:     write_date
//
//Function:		This subroutine takes the current month, day and year and puts
//		it on the bottom of line of the LCD display.  The format is "month" 
//		(abbreviated), day (two digits), and the year (The year always starts
//		with the number 2, and then three digits)
//			
//Varibles passed:
//		uStartRow: 0 to 7, the start row of the big text
//		uStartColum:  0 to 112, the pixel count to start the number from the left side of the display
//		uBlank:  Blank the month day or year
//			=0  don't blank anything
//			=1  Blank the Month
//			=2  Blank the day
//			=3  Blank the year
//  	boolInvert: if true, blue on a white background, if false, white on a blue background
//Returns:
//       none
//----------------------------------------------------------------------------
void write_date(unsigned char uStartColumn,unsigned char uRow, unsigned char uBlank,bool boolInvert)
{
	set_line(uRow);		//set the line for writing a string of characters
	moveCursorToColumn(uStartColumn);	//write white spaces to get to the right column
	if(uBlank != 1)	//go in here if we are not blanking the month
	{
		switch(t.intMonth)	//find the month string based upon an integer
		{
			case 1:		//January
				drawText((unsigned char *)strJan,uStartColumn,uRow,boolInvert);	//show the month on the display
			break;
			case 2:		//February
				drawText((unsigned char *)strFeb,uStartColumn,uRow,boolInvert);	//show the month on the display
			break;
			case 3:		//March
				drawText((unsigned char *)strMarch,uStartColumn,uRow,boolInvert);	//show the month on the display
			break;
			case 4:		//April
				drawText((unsigned char *)strApril,uStartColumn,uRow,boolInvert);	//show the month on the display
			break;
			case 5:
				drawText((unsigned char *)strMay,uStartColumn,uRow,boolInvert);	//show the month on the display
			break;
			case 6:
				drawText((unsigned char *)strJune,uStartColumn,uRow,boolInvert);	//show the month on the display
			break;
			case 7:
				drawText((unsigned char *)strJuly,uStartColumn,uRow,boolInvert);	//show the month on the display
			break;
			case 8:
				drawText((unsigned char *)strAug,uStartColumn,uRow,boolInvert);	//show the month on the display
			break;
			case 9:
				drawText((unsigned char *)strSept,uStartColumn,uRow,boolInvert);	//show the month on the display
			break;
			case 10:
				drawText((unsigned char *)strOct,uStartColumn,uRow,boolInvert);	//show the month on the display
			break;
			case 11:		//November
				drawText((unsigned char *)strNov,uStartColumn,uRow,boolInvert);	//show the month on the display
			break;
			case 12:		//December
				drawText((unsigned char *)strDec,uStartColumn,uRow,boolInvert);	//show the month on the display
			break;
		}
		write_char(0x00,false);	//write a space between the month and day
	}
	else	//we are putting spaces in for the month
	{
		write_char(0x00,false);	//A white space to keep the day and year in their right place
		write_char(0x00,false);
		write_char(0x00,false);
		write_char(0x00,false);
		write_char(0x00,false);
		write_data(0x00);		//this puts a verticle, one pixel space to keep the day and year in their correct positions

	}
	if(uBlank!=2)	//are we blanking the day?
	{
		write_small_numbers(t.intDay,false,2);	//put the day after the month, suppress leading 0
		write_char(12,false);	//This is a comma to display after the day
	}
	else	//yes, we are putting up spaces where the day should be
	{
		write_char(0x00,false);	//A white space
		write_char(0x00,false);	//A white space
		write_char(0x00,false);	//A white space
	}	
	if(uBlank!=3)	//are we blanking the year?
	{
		write_char(0x00,false);	//A white space after comma, between the day and year
		write_char(18,false);		// put up a "2" for the first digit in year
		write_small_numbers(t.intYear,false,3);	//put up the last three digits of the year, don't suppress the leading zeros (3 digits)
	}
	else	//yes, we are blanking the year
	{
		write_char(0x00,false);	//A white space to cover whatever was there.
		write_char(0x00,false);	//A white space 
		write_char(0x00,false);	//A white space 
		write_char(0x00,false);	//A white space 
		write_char(0x00,false);	//A white space 
	}
}

//----------------------------------------------------------------------------
//Routine:     write_small_numbers
//
//Function:		This routine takes a integer that is passed to it and displays
//		the integer on the LCD.  It does this by doing a BCD conversion on the
//		the integer.  The maximum input value is 999 and the lowest input value
//		is 0.  (the number is displayed in 5x7 format)
//			
//Varibles passed:
//		intSmallNum: 0 to 999, This number will be displayed on the LCD
//		boolBlankZero:  if true, then blank the leading zeros
//		uNumbersDisplayed:  1= one number displayed
//			2=two numbers displayed
//			3= three numbers displayed
//Returns:
//       none
//----------------------------------------------------------------------------
void write_small_numbers(unsigned int intSmallNum, bool boolBlankZero, unsigned char uNumbersDisplayed)
{
	unsigned char uGamma;
	unsigned char uBeta;
	
	
	integerToArray(intSmallNum);	//take an integer and convert (BCD) it into a 3 byte array
	for(uGamma=1;uGamma<4;uGamma++)	//loop through the 3 digits in the array
	{
		uBeta=str[uGamma]-32;		//convert a number in the array into an ASCII character
		if(boolBlankZero==false || uBeta!=16)	//do we need to blank the leading zeros?
		{
			if(uNumbersDisplayed==1 && uGamma==3)	//go in here only if we are displaying one digit
			{
				write_char(uBeta,false);	//put the character on the LCD
			}
			else if(uNumbersDisplayed ==2 && uGamma>=2)	//go in here only if we are displaying two digits
			{
				write_char(uBeta,false);	//put the character on the LCD
			}
			else if(uNumbersDisplayed ==3 && uGamma>=1)	//go in here only if we are displaying three digits
			{
				write_char(uBeta,false);	//put the character on the LCD
			}
		}
	}
}

//----------------------------------------------------------------------------
//Routine:     showLargeTime
//
//Function:		This routine displays the current time in large numbers,
//		which is 24x16 format.  It displays AM or PM in small letters depending
//		on the time of day.
//			
//Varibles passed:
//		uStartColumn:  sets the upperleft hand X position of the time
//		uRow:  sets the upper left hand Y pistion of the time
//
//Returns:
//       none
//----------------------------------------------------------------------------
void ShowLargeTime(unsigned char uStartColumn, unsigned char uRow)
{
	unsigned int intTempHour;

	if(t.intHour>11)	//Do we need to set the PM notification?
	{
		intTempHour=t.intHour-12;	//yes, set the hours to approapriate time
		if(intTempHour==0)		//show 12 instead of the zero hour
		{
			intTempHour=12;
		}
		drawText((unsigned char *)strPM,uStartColumn+95,uRow,false);	//show the PM indication
	}
	else	//show the AM notification
	{
		intTempHour=t.intHour;	
		if(intTempHour==0)
		{
			intTempHour=12;	//show 12 instead of the 0 hour
		}
		drawText((unsigned char *)strAM,uStartColumn+95,uRow,false);	//show the AM notification
	}
	BigDigits(uRow,uStartColumn+45,t.intMin,2,false);	//show the minute 45 pixels after the start of the time (2 digits)
	subLargeCOlon(uRow,uStartColumn+45,false);		//show the colon between the hour and minute
	BigDigits(uRow,uStartColumn,intTempHour,0,false);	//show the hour, we don't need to see the 0 in the 10's digit

}

//----------------------------------------------------------------------------
//Routine:     showLargeAlarmTime
//
//Function:		This routine displays the alarm time in large numbers,
//		which is 24x16 format.  It displays AM or PM in small letters depending
//		on the time of day.
//			
//Varibles passed:
//		uStartColumn:  sets the upperleft hand X position of the time
//		uRow:  sets the upper left hand Y pistion of the time
//
//Returns:
//       none
//----------------------------------------------------------------------------
void ShowLargeAlarmTime(unsigned char uStartColumn, unsigned char uRow)
{
	unsigned int intTempHour;

	if(intAlarmHour>11)	//Do we need to set the PM notification?
	{
		intTempHour=intAlarmHour-12;	//yes, set the hours to approapriate time
		if(intTempHour==0)		//show 12 instead of the zero hour
		{
			intTempHour=12;
		}
		drawText((unsigned char *)strPM,uStartColumn+95,uRow,false);	//show the PM indication
	}
	else	//show the AM notification
	{
		intTempHour=intAlarmHour;	
		if(intTempHour==0)
		{
			intTempHour=12;	//show 12 instead of the 0 hour
		}
		drawText((unsigned char *)strAM,uStartColumn+95,uRow,false);	//show the AM notification
	}
	BigDigits(uRow,uStartColumn+45,intAlarmMin,2,false);	//show the minute 45 pixels after the start of the time (2 digits)
	subLargeCOlon(uRow,uStartColumn+45,false);		//show the colon between the hour and minute
	BigDigits(uRow,uStartColumn,intTempHour,0,false);	//show the hour, we don't need to see the 0 in the 10's digit

}

//----------------------------------------------------------------------------
//Routine:     setBacklightOffTime
//
//Function:		This routine is for the "setting of the backlight times" menu selection.
//		It formats line 2 (from the top of the screen) to allow the user to change the 
//		backlight off hour.  It formats the line by putting up the hour, colon, the minute(00),
//		and the AM or PM label.  
//			
//Varibles passed:
//		none
//
//Returns:
//      none
//----------------------------------------------------------------------------
void setBacklightOffTime(void)
{
	set_line(2);	//line 2 will have the backlight off time, only the hour is adjustable
	write_pixel_spaces(20);	//move in the left side of the screen a bit
	if(uBacklightOffHour<=12)	//do we need to show the AM time?
	{
		if(uBacklightOffHour==0)	//when the hour is zero, it is actually 12:00am
		{
			write_small_numbers(12,false,2);	//show the AM time (hour)
		}
		else
		{
			write_small_numbers(uBacklightOffHour,false,2);	//show the AM time (hour)
		}
	}
	else
	{
		write_small_numbers(uBacklightOffHour-12,false,2);	//show the pM time (hour)
	}
	drawText((unsigned char *)strColonZero,255,255,false);	//colum, row, no highlight, show ":00" for the minute
	if(uBacklightOffHour<12)	//do we need to put up the AM or PM label?
	{
		drawText((unsigned char *)strAM,255,255,false);	//colum, row, no highlight, put up the AM label
	}
	else
	{
		drawText((unsigned char *)strPM,255,255,false);	//colum, row, no highlight, put up the PM label
	}
}

//----------------------------------------------------------------------------
//Routine:     setBacklightOnTime
//
//Function:		This routine is for the "setting of the backlight times" menu selection.
//		It formats line 5 (from the top of the screen) to allow the user to change the 
//		backlight on hour.  It formats the line by putting up the hour, colon, the minute(00),
//		and the AM or PM label.  
//			
//Varibles passed:
//		none
//
//Returns:
//      none
//----------------------------------------------------------------------------
void setBacklightOnTime(void)
{
	set_line(5);	//line 5 will have the backlight on time, only the hour is adjustable
	write_pixel_spaces(20);	//move in the left side of the screen a bit
	if(uBacklightOnHour<=12)
	{
		if(uBacklightOnHour==0)		//when the hour is zero, it is actually 12:00am
		{
			write_small_numbers(12,false,2);	//show the AM time (hour)
		}
		else
		{
			write_small_numbers(uBacklightOnHour,false,2);	//show the AM time (hour)
		}
	}
	else
	{
		write_small_numbers(uBacklightOnHour-12,false,2);	//show the pM time (hour)
	}
	drawText((unsigned char *)strColonZero,255,255,false);	//colum, row, no highlight, show ":00" for the time
	if(uBacklightOnHour<12)		//do we need to put up the AM or PM label?
	{
		drawText((unsigned char *)strAM,255,255,false);	//colum, row, no highlight, put up the AM label
	}
	else
	{
		drawText((unsigned char *)strPM,255,255,false);	//colum, row, no highlight, put up the PM label
	}
}

