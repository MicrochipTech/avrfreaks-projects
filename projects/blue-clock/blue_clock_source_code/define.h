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

//defines
#define defTock         	312		//when the timer interrupt allows for a 20mS tock (was 800 for a 51mS tock)

//portD equates
#define defLedData			4		//data line for the MBI5027GN LED display driver
#define defLedClock			2		//clock line for the MBI5027GN LED display driver
#define defLedLatch			1		//latch line for the MBI5027GN LED display driver
#define defLedEnable		3		//enable line for the MBI5027GN LED display driver
#define defLedFeedback		6		//Feedback line for the MBI5027GN LED display driver

#define defPushBtnPort		PINA	//This is the port for the push buttons
#define defBtnMinute		4		//The minute button pin
#define defBtnHour			5		//The hour button pin
#define defBtnTime			6		//The time button pin
#define defBtnAlarm			7		//The alarm button pin

//uController LED's
#define defTopColonLed		1		//PORTB, the top part of the colon between the hours and minutes
#define defBottomColonLed	0		//PORTB, the bottom part of the colon between the hours and minutes
#define defAmLed			3		//PORTA, the AM indication
#define defAlarmLed			2		//PORTC, the alarm on indication

#define defBtnDbn			12		//This controls the button debounce time (20mS * value) (was 5)

#define defStateShowTime	0		//we are showing the time on the display
#define defStateSetTime		1		//the user is setting the time of the clock using the front panel buttons
#define defStateSetAlarm	2		//the user is setting the alarm time of the clock

#define defAlarmOscOn		30		//when the alarm starts going off, this is when it will start makeing sound (20mS)
#define defAlarmOscOff		20		//when the alarm starts going off, this is when it will stop makeing sound (20mS)

#define defMaxLedIntensity	150		//70		//the maximum intesity of the LED display (out of 200) (lower number is briter)
#define defMinLedIntensity	200		//the minimum intensity of the LED display (out of 200) (higher the number, the dimmer)
#define defAlarmTimeout		60000	//counts down the alarm turning off after it goes off (if no one 
									// manually turns it off, after 20 minutes it will shut off
#define defStateDelay		75		//this is a delay from going from one state to another.
