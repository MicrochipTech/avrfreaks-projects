/*
Author:  Matt Meerian
Date:  December 9, 2007
Purpose:  This software is used as a custom alarm clock.  The user has 4 push buttons:  Alarm, Time, Hour,
	and Minute.  The time is displayed on four, blue, 7-segment displays.  There are also four other blue
	LED's:  two for the colon between the hour and minute, one for the AM indication, and one for the alarm
	indication.  The time is stored in a battery backed up PCF8563P.  The alarm time and alarm state 
	(on/off) is stored in EEPROM on the uController.
Developement System:  AVR Studio, Version 4.13 service pack 1
Target Microcontroller:  ATMega8535 from Atmel (AVR series)
Target Board:  002-0021-00 Rev A
Software Version:  1.0
*/

#include <avr\io.h>
#include <avr\interrupt.h>  //used to allow interrupts
#include <stdbool.h>    	//this file allows boolean data types in "C"
#include "define.h"			//defines, must be before the global variables since some global
#include <avr\pgmspace.h>   //must be included to store arrays in FLASH
#include <avr\eeprom.h>     //used for writing to the EEPROM
#include <string.h>

//subroutine definitions for the main.c file
void SrvTock(void);	//every 20mS this routine is run
void uCinit(void);	//initialization routine for the ATMEGA128
void sendToLEDdriver(unsigned int u16Driver1Data,unsigned int u16Driver2Data);	//shift 32 bits of data out to the display drivers
void showDigit(unsigned char u08Digit1, unsigned char u08Digit2, unsigned char u08Digit3, unsigned char u08Digit4);	//format the raw numbers to be sent to the display drivers
void integerToArray(unsigned int intNumToString);	//takes a single int and breaks it up into a string (hunds, tens, and ones digit)
void AlarmShutdown(void);	//sees if we need to turn the alarm off
void alarmSounding(void);	//uses OC1A to sound the alarm
void alarmSilent(void);		//turns off OC1A to shut off the alarm

//---------------------------------------------------------------------------
//Declair the data types
typedef unsigned char u08;  //A character, 8 bits
typedef unsigned short u16; //a short integer, big endian 16 bits, 0 to 65535
typedef bool ubool;         //a boolean, true or false, must include the header file "stdbool.h"
#include "twi.h"		//used for the real time clock

/*global varibles*/
//Note:  Variables used in interrupts must be declaired as volatile
volatile u16 uTock=defTock;	//A counter used for the 20mS tock
volatile u08 u08State=defStateShowTime;	//the variable for the state machine
u08 u08BtnMnTmr;	//The debounce timer for the "minute" button
u08 u08BtnHrTmr;	//The debounce timer for the "hour" button
u08 u08BtnTimeTmr;	//The debounce timer for the "time" button
u08 u08BtnAlarmTmr;	//The debounce timer for the "alarm" button
u08 u08AlarmMinute;	//the minute the alarm should go off
u08 u08AlarmHour;	//the hour the alarm should go off
u08 u08TmrAdc;		//this is used as a timer to take an ADC reading from the photocell
u08 u08PhotocellValue;	//this holds the raw ADC value from the photocell
u08 u08ArrayStrTemp[2];	//used to hold tempory values used in display drivers
u08 u08Temp;	//used in several places
u08 u08TmrUpdateDisplay;	//counts down to updateing the display
char str[6];	//for the integer to string routine
u08 u08TmrAlarmOsc;	//makes the alarm sound oscillate (beep, pause, beep, pause...)
u08 u08TmrStateDelay=0;	//is used as a slight pause for when a state needs to change
u08 u08OldPhotocell=0;	//This is the old value of the photocell
u08 u08RtcHour;		//the hour variable for the RTC
u08 u08RtcMinute;	//the minute variable for the RTC
u08 u08RtcSecond;	//the second variable for the RTC
u08 u08RtcDay;		//the day variable for the RTC
u08 u08RtcMonth;	//the month variable for the RTC
uint u08RtcYear;	//the year variable for the RTC
volatile u08 u08ToLoad[4];	//used to hold the values that will be loaded into the display drivers
volatile bool boolLedAm;	//when true, the AM indication LED should be on
volatile bool boolDisplayOff;	//when set to true, the display has been shut off for the dimming

volatile bool boolLedAlarm;		//when true, the alarm is set
bool boolAlarmCanBeTurnedOff=true;	//when true, the alarm can be turned off
bool boolAlarmSounding=false;	//when true, the audible alarm is sounding
uint u16TmrShutOffAlarm=0;	//this counts down to turning off the alarm after it has started sounding

volatile u08 u08TmrLedDimming=100;	//keeps track of where we are at in dimming of the LED display
volatile u08 u08LedIntensity=100;	//sets the intesity of the LED display

const unsigned char EEalarmMinute __attribute__ ((section (".eeprom"))) = 0xFF;	//the minute the alarm should go off
const unsigned char EEalarmHour __attribute__ ((section (".eeprom"))) = 0xFF;	//the hour the alarm should go off
const unsigned char EEalarmOn __attribute__ ((section (".eeprom"))) = 0xFF;		//when 1, the alarm was on before power was lost

//These two strings are used to turn on (and off) the correct LEDs in the two 7-segment display drivers 
//  to show the input numbers.  (you really need to look at the schematic,002-0021-00 to figure this out.)
//  These lookup table bytes are in flash
prog_uchar strLedDrivU3[]={0x77,0x14,0xB3,0xB6,0xD4,0xE6,0xE7,0x34,0xF7,0xF4,0x00,0x00};	//LED1 and LED2 on the schematic	
						//0	   1	2	 3    4    5     6    7    8    9	 blank
prog_uchar strLedDrivU4[]={0x77,0x41,0x3B,0x6B,0x4D,0x6E,0x7E,0x43,0x7F,0x4F,0x00,0x00};	//LED3 and LED4 on the schematic
					        //0	   1	2	 3    4    5    6    7    8    9	blank

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
	uCinit();		//go through the initialization process for the microcontroller
	sei();        	//enable interrupts
	u08State=defStateShowTime;		//start out in the "display the time" state
	twi_setup();	//initialize the RTC chip (PCF8563P)
	rtc_get_timeNdate(&u08RtcHour, &u08RtcMinute, &u08RtcSecond, &u08RtcDay, &u08RtcMonth, &u08RtcYear);	//get the time and date from the RTC chip
	while(1)       // go into an infinite loop here
	{
        SrvTock();        //go to the tock every 20ms

	}
	return 0;
}


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
	u08TmrLedDimming--;	//counting down to dimming

	if(u08TmrLedDimming<2)	//is it time to turn on the LED's?
	{
		PORTC |= (1<<4);	//debug
		PORTC |= (1<<5);	//debug
		showDigit(u08ToLoad[0],u08ToLoad[1],u08ToLoad[2],u08ToLoad[3]);	//load the current data to load into the display drivers
		PORTB |= (1<<defTopColonLed);		//turn on the top part of the colon between the hour and minute
		PORTB |= (1<<defBottomColonLed);	//turn on the bottom part of the colon between the hour and minute
		if(boolLedAm==true)		//do we need to turn on the AM indication LED?
		{
			PORTA |= (1<<defAmLed);	//turn on the AM light
		}
		if(boolLedAlarm==true)	//do we need to turn on the alarm indication LED?
		{
			PORTC |= (1<<defAlarmLed);		//turn on the alarm indicator
		}
		u08TmrLedDimming=defMinLedIntensity+2;	//reset the LED dimming timer
		PORTC &= ~(1<<5);	//debug
		boolDisplayOff=false;	//we are not ready to turn the display
	}
	if(u08TmrLedDimming<u08LedIntensity)	//is it time to turn off the LED's?
	{	//this boolean value is used so we don't miss a turn off in the interupt
		if(boolDisplayOff==false)	//is it time to turn the display off
		{	//the larger the value of u08LedIntensity, the dimmer the display
			showDigit(10,10,10,10);	//blank the 7-segment display	
			PORTB &= ~(1<<defTopColonLed);		//turn off the top part of the colon between the hour and minute
			PORTB &= ~(1<<defBottomColonLed);	//turn off the bottom part of the colon between the hour and minute
			PORTA &= ~(1<<defAmLed);	//turn off the AM light
			PORTC &= ~(1<<defAlarmLed);		//turn off the alarm indicator
			PORTC &= ~(1<<4);	//debug
			boolDisplayOff=true;	//do not go back in here until the display has been turned back on
		}
	}


}

//----------------------------------------------------------------------------
//Routine:      SrvTock  (Service the tock)
//
//Function:     Exicute the tock every 20mS from the main loop.
//				This is where ALL the good stuff happens
//
//Varibles passed:
//       none
//Returns:
//       none
//----------------------------------------------------------------------------
void SrvTock(void)
{
    if (uTock == 0)
    {
        uTock = defTock;   //reload the Tock (20mS) timer
        if(u08BtnMnTmr!=0)	//the debounce timer for the minute button
        {
            u08BtnMnTmr--;     
        }
        if(u08BtnHrTmr!=0)	//the debounce timer for the hour button
        {
        	u08BtnHrTmr--;     
        }
        if(u08BtnTimeTmr!=0)	//the debounce timer for the time button
        {
            u08BtnTimeTmr--;   
        }
        if(u08BtnAlarmTmr!=0)	//the debounce timer for the alarm button
        {
            u08BtnAlarmTmr--;     
        }
		
		//This takes the ADC reading for the photocell
		if(u08TmrAdc==1)	//do we need to start the ADC reading
		{
			ADMUX|=(1<<REFS0)|(1<<ADLAR);    //ADC reference to AVCC, 8 bit reading, 
			ADCSRA|=(1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0)|(1<<ADEN)|(1<<ADSC);   //divide by n, ADC to free-running mode, enable ADC, start A2D conversions
		}
		if(u08TmrAdc==2)	//is it time to get the photocell reading?
		{
			u08PhotocellValue=ADCH;  //get the ADC value and store it
			if(u08PhotocellValue>defMinLedIntensity)	//do not allow the display to get to turn off
			{
				u08PhotocellValue=defMinLedIntensity;	//set the intensity of the LED display to the minimum
			}
			else	//we must be at the maximum intenisity
			{
				if(u08PhotocellValue<defMaxLedIntensity)	//are we below the maximum intesity of the LED display?
				{
					u08PhotocellValue=defMaxLedIntensity;	//then set the LED display to the maximum intesity
				}
			}
			if(u08RtcHour>=7 && u08RtcHour<22)	//is it after 7am and before 10pm? (sets the intensity to maximum during the day)
			{
				u08PhotocellValue=defMaxLedIntensity;	//then set the LED display to the maximum intesity
			}
			//if(u08State != defStateShowTime)	//if we are in a state were we are setting the time or the alarm,
			//{
				//u08PhotocellValue=defMaxLedIntensity;	//then set the LED display to the maximum intesity
			//}
			if(u08OldPhotocell<u08PhotocellValue)	//this slows down the dimming response by slowing down the changes from the photocell
			{
				u08OldPhotocell++;	//we need to take the intensity of the display down
				if(u08OldPhotocell < (defMinLedIntensity-40))	//this takes the display down quicker, but causes noticeable jumps
				{
					u08OldPhotocell=u08OldPhotocell+5;
				}
			}
			else if (u08OldPhotocell>u08PhotocellValue)	//do we need to make the LED display brighter?
			{
				u08OldPhotocell--;	//make the display brighter
			}
			u08LedIntensity=u08OldPhotocell;	//load the photocell value into the LED dimmer variable
		}

		if(u08TmrAdc==0)	//is it time to reset the ADC reading timer?
		{
			u08TmrAdc=20;	//yes, reset the ADC reading counter
		}
		else
		{
			u08TmrAdc--;	//count down to an ADC reading
		}

		if(u08TmrUpdateDisplay==0)	//update the display about every 40mS
		{
			u08TmrUpdateDisplay=2;

			if(u08State==defStateShowTime || u08State==defStateSetTime)	//are we displaying the current time or setting the time?
			{
				if(u08State==defStateShowTime)	//only get the current time when we are displaying the current time
				{
					rtc_get_timeNdate(&u08RtcHour, &u08RtcMinute, &u08RtcSecond, &u08RtcDay, &u08RtcMonth, &u08RtcYear);	//get the time and date from the RTC chip
				}
				//NOTE:  The returned hour ranges from 0 to 23, we need to put that into 12 hour format
				integerToArray(u08RtcMinute);	//convert the minute char into a string
				u08ArrayStrTemp[0]=str[3];		//get the results of the minute conversion into a temporary string
				u08ArrayStrTemp[1]=str[4];
				u08Temp=u08RtcHour;		//load the hour into a tempory variable
				if(u08Temp>12)		//since the hour is in military time, we need it in 12 hour format
				{
					u08Temp=u08Temp-12;		
					boolLedAm=false;	//turn off the AM light
				}
				else if (u08Temp==0)	//is it between midnight and 1am?
				{
					u08Temp=12;		//then we should show the 0 from the clock as 12am
					boolLedAm=true;		//turn on the AM light
				}
				else	//it must be between 1am and 1pm
				{
					if(u08Temp==12)		//are we between noon and 1pm
					{
						boolLedAm=false;	//then turn off the PM light
					}
					else		//it must be between 1am and noon
					{
						boolLedAm=true;	//turn on the AM light
					}
				}
				integerToArray(u08Temp);	//convert the hour char to a string
				if(str[3]==0)	//do we need to blank the leading zero in the hour?
				{
					str[3]=10;	//this clears the left most LED digit
				}
				u08ToLoad[0]=str[3];	//load all of the LED driver values into the output string
				u08ToLoad[1]=str[4];
				u08ToLoad[2]=u08ArrayStrTemp[0];
				u08ToLoad[3]=u08ArrayStrTemp[1];
			}
		}
		else
		{
			u08TmrUpdateDisplay--;	//count down to the next time to update the display
		}

		//this creates the "beep - pause - beep ..." when the alarm is going off
		if(u08TmrAlarmOsc==1)	//do we need to turn the alarm sound on?
		{	//NOTE:  when u08TmrAlarmOsc is zero, the alarm does not need to oscilate
			u08TmrAlarmOsc=defAlarmOscOn;	//reload the timer for turning the alarm sound on/off
			alarmSounding();	//turn on the alarm sound
		}
		if(u08TmrAlarmOsc==defAlarmOscOff)	//is it time to turn the alarm sound off?
		{
			alarmSilent();		//turn off the alarm sound off
		}
		if(u08TmrAlarmOsc>1)	//do we need to count down to turning the alarm on/off?
		{
			u08TmrAlarmOsc--;	//count down to cycling the alarm on/off
		}

		if(u16TmrShutOffAlarm!=0)	//are we counting down to shutting the alarm off after minutes?
		{
			u16TmrShutOffAlarm--;	//counting down to alarm turnoff
			if(u16TmrShutOffAlarm==1)	//do we need to turn off the alarm
			{
				AlarmShutdown();	//turn off the fucking alarm! we are tired of listening to it after 20 minutes
			}
		}

		//This switch statement is the main state machine
        switch (u08State)
        {
			case defStateShowTime:		//show the time
				if(boolLedAlarm==true && u08AlarmHour==u08RtcHour && u08AlarmMinute==u08RtcMinute && boolAlarmCanBeTurnedOff==true)	//is it time for the alarm to go off?
				{
					boolAlarmSounding=true;		//the alarm is going off
					boolAlarmCanBeTurnedOff=false;	//if the user turns off the alarm in the same minute as it turned on, we do not want it start going off again
					u08TmrAlarmOsc=defAlarmOscOn;			//allow the alarm osciallation to happen
					u16TmrShutOffAlarm=defAlarmTimeout;		//load up the 20 minute timer to shutdown the alarm
				}
				if(u08AlarmMinute!=u08RtcMinute)		//once are are not at the same minute as the alarm going off
				{
					boolAlarmCanBeTurnedOff=true;	//  ...we can then allow the alarm to go off
				}
				if((defPushBtnPort & (1<<defBtnMinute))==0 && u08BtnMnTmr==0)	//is the minute button pressed and has not been previously pressed in awhile?
				{
					AlarmShutdown();	//if the user pressed any button, then turn off the alarm (if it is going off)
					u08BtnMnTmr=defBtnDbn;   //load up the debounce timer, so we don't immediately go back in here
				}
				if((defPushBtnPort & (1<<defBtnHour))==0 && u08BtnHrTmr==0)	//is the hour button pressed and has not been previously pressed in awhile?
				{
					AlarmShutdown();	//if the user pressed any button, then turn off the alarm (if it is going off)
					u08BtnHrTmr=defBtnDbn;   //load up the debounce timer, so we don't immediately go back in here
				}
				if((defPushBtnPort & (1<<defBtnTime))==0 && u08BtnTimeTmr==0)	//is the time button pressed and has not been previously pressed in awhile?
				{
					AlarmShutdown();	//if the user pressed any button, then turn off the alarm (if it is going off)
					u08State=defStateSetTime;	//go set the time
					u08BtnTimeTmr=defBtnDbn;   //load up the debounce timer, so we don't immediately go back in here
				}
				if((defPushBtnPort & (1<<defBtnAlarm))==0 && u08BtnAlarmTmr==0)	//is the alarm button pressed and has not been previously pressed in awhile?
				{
					AlarmShutdown();	//if the user pressed any button, then turn off the alarm (if it is going off)
					if(boolLedAlarm==true)		//is the alarm on?
					{
						eeprom_write_byte((u08*)&EEalarmOn,0);	//set the alarm to off
						boolLedAlarm=false;	//turn off the alarm indicator
					}
					else
					{
						eeprom_write_byte((u08*)&EEalarmOn,1);	//set the alarm to on
						boolLedAlarm=true;	//turn on the alarm indicator
					}
					u08State=defStateSetAlarm;	//go set the alarm
					u08BtnAlarmTmr=defBtnDbn;   //load up the debounce timer, so we don't immediately go back in here
				}
			break;

			case defStateSetTime:	//set the time
				if((defPushBtnPort & (1<<defBtnMinute))==0 && u08BtnMnTmr==0)	//is the minute button pressed and has not been previously pressed in awhile?
				{
					if(u08RtcMinute<59)		//have we reached the maximum minute count?
					{
						u08RtcMinute++;		//no, then increment the minute
					}
					else
					{
						u08RtcMinute=0;		//reset the hour count since we have reached the 59th minute
					}
					u08BtnMnTmr=defBtnDbn;   //load up the debounce timer, so we don't immediately go back in here
				}
				if((defPushBtnPort & (1<<defBtnHour))==0 && u08BtnHrTmr==0)	//is the hour button pressed and has not been previously pressed in awhile?
				{
					if(u08RtcHour<23)		//have we reached the maximum hour count?
					{
						u08RtcHour++;		//no, increment the hour
					}
					else
					{
						u08RtcHour=0;		//reset the hour count since we have reached the 23rd hour
					}
					u08BtnHrTmr=defBtnDbn;   //load up the debounce timer, so we don't immediately go back in here
				}
				if((defPushBtnPort & (1<<defBtnTime))!=0 && u08BtnTimeTmr==0)	//is the time button released?
				{
					u08State=defStateShowTime;		//when the user lets go of the time button, go back to the display state
					rtc_set_time(u08RtcHour, u08RtcMinute, 0);	//write to the RTC
					u08BtnTimeTmr=defBtnDbn;   //load up the debounce timer, so we don't immediately go back in here
				}
				if((defPushBtnPort & (1<<defBtnAlarm))==0 && u08BtnAlarmTmr==0)	//is the minute button pressed and has not been previously pressed in awhile?
				{
					u08BtnAlarmTmr=defBtnDbn;   //load up the debounce timer, so we don't immediately go back in here
				}
			break;

			case defStateSetAlarm:	//set the alarm
				//NOTE:  The returned hour ranges from 0 to 23, we need to put that into 12 hour format
				integerToArray(u08AlarmMinute);	//convert the minute char into a string
				u08ArrayStrTemp[0]=str[3];		//get the results of the minute conversion into a temporary string
				u08ArrayStrTemp[1]=str[4];
				u08Temp=u08AlarmHour;		//load the hour into a tempory variable
				if(u08Temp>12)		//since the hour is in military time, we need it in 12 hour format
				{
					u08Temp=u08Temp-12;		
					boolLedAm=false;	//turn off the AM light
				}
				else if (u08Temp==0)	//is it between midnight and 1am?
				{
					u08Temp=12;		//then we should show the 0 from the clock as 12am
					boolLedAm=true;		//turn on the AM light
				}
				else	//it must be between 1am and 1pm
				{
					if(u08Temp==12)		//are we between noon and 1pm
					{
						boolLedAm=false;	//then turn off the PM light
					}
					else		//it must be between 1am and noon
					{
						boolLedAm=true;	//turn on the AM light
					}
				}
				integerToArray(u08Temp);	//convert the hour char to a string
				if(str[3]==0)	//do we need to blank the leading zero in the hour?
				{
					str[3]=10;	//this clears the left most LED digit
				}
				u08ToLoad[0]=str[3];		//load the data into the string that puts data into the display drivers
				u08ToLoad[1]=str[4];
				u08ToLoad[2]=u08ArrayStrTemp[0];
				u08ToLoad[3]=u08ArrayStrTemp[1];

				if((defPushBtnPort & (1<<defBtnMinute))==0 && u08BtnMnTmr==0 && u08TmrStateDelay == 0)	//is the minute button pressed and has not been previously pressed in awhile?
				{	//don't go in here if we are changing state
					if(u08AlarmMinute<59)		//have we reached the maximum minute count?
					{
						u08AlarmMinute++;		//no, then increment the minute
					}
					else
					{
						u08AlarmMinute=0;		//reset the minute count since we have reached the 59th minute
					}
					u08BtnMnTmr=defBtnDbn;   //load up the debounce timer, so we don't immediately go back in here
				}
				if((defPushBtnPort & (1<<defBtnHour))==0 && u08BtnHrTmr==0 && u08TmrStateDelay == 0)	//is the hour button pressed and has not been previously pressed in awhile?
				{	//don't go in here if we are changing state
					if(u08AlarmHour<23)		//have we reached the maximum hour count?
					{
						u08AlarmHour++;		//no, then increment the minute
					}
					else
					{
						u08AlarmHour=0;		//reset the hour count since we have reached the 59th minute
					}
					u08BtnHrTmr=defBtnDbn;   //load up the debounce timer, so we don't immediately go back in here
				}
				if((defPushBtnPort & (1<<defBtnTime))==0 && u08BtnTimeTmr==0  && u08TmrStateDelay != 0)	//is the time button pressed and has not been previously pressed in awhile?
				{	//don't go in here if we are changing state
					u08BtnTimeTmr=defBtnDbn;   //load up the debounce timer, so we don't immediately go back in here
				}
				if((defPushBtnPort & (1<<defBtnAlarm))!=0 && u08BtnAlarmTmr==0  && u08TmrStateDelay == 0)	//is the alarm button released?
				{	//don't go in here if we are changing state
					//the user must have let go of the alarm button
					eeprom_write_byte((u08*)&EEalarmMinute,u08AlarmMinute);	//set the alarm minute to a legit value
					eeprom_write_byte((u08*)&EEalarmHour,u08AlarmHour);	//set the alarm minute to a legit value
					u08TmrStateDelay=defStateDelay;	//set a delay to go to the next state
					u08BtnAlarmTmr=defBtnDbn;   //load up the debounce timer, so we don't immediately go back in here
				}
				if(u08TmrStateDelay != 0)	//are we counting down to a state change?
				{
					if(u08TmrStateDelay==1)	//is it time for a state change?
					{
						u08State=defStateShowTime;	//go back to the display state
					}
					u08TmrStateDelay--;		//counting down to a state change
				}
			break;
		}
	}
}

//----------------------------------------------------------------------------
//Routine:		sendToLEDdriver (Send data to the LED driver IC)
//
//Function:		This routine bit-bangs data through an SPI port to the LED
//				display driver.  (using a data line, clock line, latch line and
//				an enable line)
//
//Varibles passed:
//       unsigned int u16DriverData: raw data value to be sent to the display driver
//Returns:
//       none
//----------------------------------------------------------------------------
void sendToLEDdriver(unsigned int u16Driver1Data, unsigned int u16Driver2Data)
{
	volatile u16 u16Counter=1;		//this int holds the bit mask to shift through the 16 bit integer
	volatile u08 u08ShiftCounter;	//used to count a bit shift through a 16 bit interger
	
	PORTD &= ~(1<<defLedLatch);		//start out with all of the lines going to the driver chip held low
	PORTD &= ~(1<<defLedEnable);
	PORTD &= ~(1<<defLedData);
	PORTD &= ~(1<<defLedClock);
	asm volatile("nop");
	PORTD |= (1<<defLedLatch);	//take the latch high to start loading in data
	PORTD |= (1<<defLedEnable);	//turn the drivers off while shift in data
	for(u08ShiftCounter=0;u08ShiftCounter<16;u08ShiftCounter++)
	{
		if(u16Driver1Data & u16Counter)		//This uses a bit mask to set the data line 
		{
			PORTD |= (1<<defLedData);		//we want to turn on the corresponding LED driver output using the data line
		}
		else
		{
			PORTD &= ~(1<<defLedData);		//we want to turn off the corresponding LED driver output
		}
		u16Counter = (u16Counter<<1);
		PORTD |= (1<<defLedClock);			//clock the data into the LED driver chip
		asm volatile("nop");
		PORTD &= ~(1<<defLedClock);		//get the clock line ready for next time
	}
	u16Counter=1;
	for(u08ShiftCounter=0;u08ShiftCounter<16;u08ShiftCounter++)
	{
		if(u16Driver2Data & u16Counter)		//This uses a bit mask to set the data line 
		{
			PORTD |= (1<<defLedData);		//we want to turn on the corresponding LED driver output using the data line
		}
		else
		{
			PORTD &= ~(1<<defLedData);		//we want to turn off the corresponding LED driver output
		}
		u16Counter = (u16Counter<<1);
		PORTD |= (1<<defLedClock);			//clock the data into the LED driver chip
		asm volatile("nop");
		PORTD &= ~(1<<defLedClock);		//get the clock line ready for next time
	}
	asm volatile("nop");
	PORTD &= ~(1<<defLedLatch);	//take the latch low to latch in the data
	PORTD &= ~(1<<defLedEnable);	//turn the drivers on after the data has been loaded into the driver ic
}

//----------------------------------------------------------------------------
//Routine:		showDigit (show a number 0 through 9)
//
//Function:		This routine takes a number 0 to 9, grabs the appropriate LED's
//				to light up from the lookup table, and then sends the lookup table
//				data (two bytes shoved into one 16 bit number) to the display.
//
//Varibles passed:
//				u08 u08Digit:  a number, 0 to 9 to display
//Returns:
//       none
//----------------------------------------------------------------------------
void showDigit(u08 u08Digit1, u08 u08Digit2, u08 u08Digit3, u08 u08Digit4)
{
	volatile u16 u16Drv1Total=0;	//used to total up the two bytes into one integer for the number to be displayed
	volatile u16 u16Drv2Total=0;	//used to total up the two bytes into one integer for the number to be displayed

	if(u08Digit1>=10)		//is the passed parameter an illegal value?
	{
		u08Digit1=10;	//yes, then set it to a legal value, 10 is a blank for the most significant LED digit
	}
	if(u08Digit2>=10)		//is the passed parameter an illegal value?
	{
		u08Digit2=10;	//yes, then set it to a legal value
	}
	if(u08Digit3>=10)		//is the passed parameter an illegal value?
	{
		u08Digit3=10;	//yes, then set it to a legal value
	}
	if(u08Digit4>=10)		//is the passed parameter an illegal value?
	{
		u08Digit4=10;	//yes, then set it to a legal value
	}
	//LED1 and LED2 on the 002-0021-00, (LED 1 is the left most 7-segment display)
	u16Drv1Total = pgm_read_byte(&strLedDrivU3[u08Digit1]);
	u16Drv1Total = (u16Drv1Total<<4);
	u16Drv1Total = u16Drv1Total & 0xFF00;
	u16Drv1Total = u16Drv1Total | pgm_read_byte(&strLedDrivU3[u08Digit2]);
	u16Drv1Total = (u16Drv1Total<<4);
	u16Drv1Total = u16Drv1Total | (pgm_read_byte(&strLedDrivU3[u08Digit1]) & 0x000F);
	//LED 3 and LED4 on the 002-0021-00, (LED4 is the right most 7-sement display)
	u16Drv2Total = pgm_read_byte(&strLedDrivU4[u08Digit4]);
	u16Drv2Total = (u16Drv2Total<<4);
	u16Drv2Total = u16Drv2Total & 0xFF00;
	u16Drv2Total = u16Drv2Total | pgm_read_byte(&strLedDrivU4[u08Digit3]);
	u16Drv2Total = (u16Drv2Total<<4);
	u16Drv2Total = u16Drv2Total | (pgm_read_byte(&strLedDrivU4[u08Digit4]) & 0x000F);
	if(u08Digit4==10)
	{
		sendToLEDdriver(0,0);		//send out the number to the two display drivers
	}
	else
	{
		sendToLEDdriver(u16Drv1Total,u16Drv2Total);		//send out the number to the two display drivers
	}
}

//----------------------------------------------------------------------------
//Routine:		uCinit 
//
//Function:		Initialize the microcontroller registers.
//
//Varibles passed:
//				none
//Returns:
//       		none
//----------------------------------------------------------------------------
void uCinit(void)
{
	u08 u08AlarmOn;		//says weither the alarm should go off or not

	PORTA=0xF7;          	//enable pull-up resistors
	DDRA=0x08;   			//set PORTA to outputs
	PORTB=0xE0;          	//enable pull-up resistors
	DDRB=0xFF;   			//set PORTB to outputs
	PORTC=0x03;          	//enable pull-up resistors
	DDRC=0xFC;   			//set PORTC to outputs
	PORTD=0xC0;          	//PortD has no pullup resistors enabled
	DDRD=0x3F;           	//set all of port D to output

	//timer 0 initialization (8 bit) (for the 20mS tock)
	TIMSK = (1<<TOIE0);   //Enable TCNT0 overflow
	TCNT0 = 0;         	// reset TCNT0
	TCCR0 = 1;        	//count with cpu clock/1

	//get values out of EERPOM
	u08AlarmMinute = eeprom_read_byte((u08*)&EEalarmMinute);//get the alarm minute out of the eeprom
	u08AlarmHour = eeprom_read_byte((u08*)&EEalarmHour);	//get the alarm hour out of the eeprom
	u08AlarmOn = eeprom_read_byte((u08*)&EEalarmOn);		//see if the alarm was on previous to power down
	//make sure the values from EEPROM are legit
	if(u08AlarmMinute>59)		//is the alarm minute an illegal value?
	{
		eeprom_write_byte((u08*)&EEalarmMinute,0);	//set the alarm minute to a legit value
		u08AlarmMinute=0;		//yes, then reset the ram value to a legal value
	}
	if(u08AlarmHour>23)		//is the alarm hour an illegal value?
	{
		eeprom_write_byte((u08*)&EEalarmHour,7);	//set the alarm hour to a legit value
		u08AlarmHour=7;		//yes, then reset the ram value to a legal value
	}
	if(u08AlarmOn==0)		//should the alarm be off at powerup?
	{				
		boolLedAlarm=false;	//turn off the alarm indicator
	}
	else if(u08AlarmOn==1)	//should the alarm be on at powerup?
	{
		boolLedAlarm=true;	//turn on the alarm indicator
	}
	else if(u08AlarmOn>1)		//is the alarm On state an illegal value?
	{
		eeprom_write_byte((u08*)&EEalarmOn,0);	//set the alarm on state to a legit value
		u08AlarmOn=0;		//yes, then reset the ram value to a legal value
	}
}


//----------------------------------------------------------------------------
//Routine:		integerToArray 
//
//Function:		
//				Takes a 16 bit integer and converts it to a string, with the ones,
//				tens, and hundreds digits in fixed positions
//
//Varibles passed:
//				intNumToString:	the number to be converted to a string	
//Returns:
//       		none
//----------------------------------------------------------------------------
//note:  The standard itoa() puts the ones, tens, and hundreds digit in different places based upon
//		 the size of the input number
void integerToArray(unsigned int intNumToString)
{
    unsigned int minus = 0;
    long int d;
    char *ptr;

    strcpy (str,"00000");
	str[4]=0;
	str[3]=0;
	str[2]=0;
	str[1]=0;
	str[0]=0;
    ptr = &str[sizeof(str) -1];
    *ptr-- = '\0';
    if (intNumToString < 0)
    {
         minus = 1; 
		  intNumToString = -intNumToString;
    }
    while (intNumToString > 0)
    {
         d = intNumToString % 10;
         //*ptr-- = d + '0';
		 *ptr-- = d;		//we need a non-ASCII value
         intNumToString /= 10;
     }
    if (minus)
        *ptr = '-';
    else
        ptr++;
}

//----------------------------------------------------------------------------
//Routine:		AlarmShutdown
//
//Function:		If the alarm is going off, then this turns off the alarm
//
//Varibles passed:
//				none
//Returns:
//       		none
//----------------------------------------------------------------------------
void AlarmShutdown(void)
{
	if(boolAlarmSounding==true)
	{
		alarmSilent();	//turn off timer 1 for the alarm
		u08TmrAlarmOsc=0;	//turn off the alarm oscillation
		boolAlarmSounding=false;	//the alarm is no longer sounding
	}
}

//----------------------------------------------------------------------------
//Routine:		alarmSounding
//
//Function:		This uses timer 1 to create a frequency on OC1A, this is pin 19
//				on the 40 pin DIP version of the ATMEGA8535
//
//Varibles passed:
//				none
//Returns:
//       		none
//----------------------------------------------------------------------------
void alarmSounding(void)
{
	//these settings have the alarm running at 975Hz (period of 1.025mS)
	// the OCR1A setting makes the output pulse high for about 320uS.
	OCR1AH = 0x00;    	//Set a initial value in the OCR1A-register
    OCR1AL = 0xA0;    	//Timer 1 counts to this value,
	TCCR1A = (1<<COM1A1 | 1<<WGM11);	//attach the OC1A pin to timer 1, turn on fast 9-bit PWM (WGM11)
	TCCR1B = (1<<WGM12 | 1<<CS11);	//fast 9-bit PWM (WGM12), clock for timer 1 divided by 8 from the prescaler (CS11)
/*
	//configuration for 4KHz output:		(sounds like a fire alarm, bad)
	OCR1AH = 0x00;    	//Set a initial value in the OCR1A-register
    OCR1AL = 0xA0;    	//Timer 1 counts to this value,
		//0x2AA		170uS high
		//0x1F0		124uS high	 	(10 bit = 0x3FF max count)
		//0x1A0		104uS high     (out of 256uS)
		//0xA0		40uS high
	TCCR1A = (1<<COM1A1 | 1<<WGM11 | 1<<WGM10);	//attach the OC1A pin to timer 1, turn on fast 9-bit PWM
	TCCR1B = (1<<WGM12 | 1<<CS10);	//fast 9-bit PWM, clock for timer 1 divided by , | 1<<CS10
*/
}

//----------------------------------------------------------------------------
//Routine:		alarmSilent
//
//Function:		This stops timer 1 from making the alarm sound on OC1A.
//
//Varibles passed:
//				none
//Returns:
//       		none
//----------------------------------------------------------------------------
void alarmSilent(void)
{
    OCR1AH = 0;	//turn off OC1A
    OCR1AL = 0;
	TCCR1A = 0;		
	TCCR1B = 0;
}
