/*------------------------------------------------------------------------
|
|	Project:		speedLimiter	revision: 1.0.2
|					speedLimiter.c
|
|	Author:			Rasmus Renzi,	rasmus.r@spray.se
|
|
|
--------------------------------------------------------------------------
|
|	Description:	Speedlimiter for engines with mechanical ignition
|					systems.
--------------------------------------------------------------------------
|
|	History:		20060304	0.0.1	First draft for AVR GCC compiler
|					20060322	0.0.2	First menu choise working
|					20060404	0.0.3	First draft with RPM calculation
|					20060429	0.0.4	Divided code in more modules/files
|					20060520	1.0.0	First issue of speedLimiter	
|					20060525	1.0.1	Changed variables and definitions
|					20060603	1.0.2	Minor updates	
|
|
--------------------------------------------------------------------------*/

#define SPEEDLIMITER_C

// Include files
#include <avr/io.h>
#include <avr/interrupt.h>
#include <inttypes.h>
#include "speedLimiter.h"
#include "displayDrv.h"
#include "EEPROM.h"
#include "miscFuncs.h"
#include "interrupts.h"


int main(void)
{
	// Declaration of local variables
	unsigned char menuLevel;

	PORTC = PORTC | 0x10;													// Set PC4 to avoid driving current through the ignition coil

	// Variable initiation
	prevIgnPeriodTime = 0xFFFF;
	lifeCycleCnt = 0;
	blink = 0;
	currentRPM = 0;
	us50Cnt = 0;
	prevIgnTime = 0;
	cutNow = 0;

	menuLevel = 5;
	currentLimit = FIXED_LIMIT;
	pulseCount = 0;
	fixedMode = 1;

	projectInit();
	timerInit();
	InitDisplay();

	if(0x06 == (~(PINB) & 0x06))												// Menu UP and menu DOWN pressed (on power up)
	{
		fixedMode = 0;															// Enter non fixed mode
	}


	if(fixedMode == 0)
	{
		currentLimit  = (unsigned char)((EEPROM_read(1))&(0xFF));				// Read stored limit value from EEPROM, low byte
		currentLimit |= (unsigned int)(((unsigned char)(EEPROM_read(0)))<<8);	// Read stored limit value from EEPROM, high byte

		if((currentLimit>9900)||(currentLimit<100))								// Check if value is valid
		{
			currentLimit = FIXED_LIMIT;
		}
	}
	else
	{
		currentLimit = FIXED_LIMIT;												// Set fixed engine speed limit value
	}
	currentLimitTime = 3000000/currentLimit;

	sei ();																		// Turn on global interrupts
	if(fixedMode == 0)
	{
		SplashText();
	}
	else
	{
		ClearDisplay();
		PrintString(" 18:35 - 03 JUN\r");
		SetToLn2();
		PrintString(" 22 (C)\r");
	}
	LIFE_SET;
// ======== MAIN BACKGROUNDLOOP FOR speedLimiter APPLICATION ========
	while (1)
	{

		if(fixedMode == 0)
		{
			// Check if any buttons are pressed
			checkButtons();

			if(selPressed == 1)													// Is menu selection made?
			{
				if(menuLevel == 1)												// Execute corresponding function
				{
					viewRPM();
				}
				else if(menuLevel == 2)
				{
					viewRPMbar();
				}
				else if(menuLevel == 3)
				{
					setLimit();
				}
				else if(menuLevel == 4)
				{
					about();
				}
				else if(menuLevel == 5)
				{
					ignitionTime();
				}
			}

			// Root menu
			if((upPressed == 1)&&(menuLevel < 6))								// Is menu item changed?
			{
				menuLevel++;
				if(menuLevel == 6)
				{
					menuLevel = 1;
				}
			}
			if((downPressed == 1)&&(menuLevel > 0))
			{
				menuLevel--;
				if(menuLevel == 0)
				{
					menuLevel = 5;
				}
			}
			if((menuLevel == 1) && (upPressed == 1||downPressed == 1))			// View menu item...
			{
				ClearDisplay();
				PrintString("** MAIN  MENU **\r");
				SetToLn2();
				PrintString("-> View RPM\r");
			}
			if((menuLevel == 2) && (upPressed == 1||downPressed == 1))
			{
				ClearDisplay();
				PrintString("** MAIN  MENU **\r");
				SetToLn2();
				PrintString("-> View RPM Bar\r");
			}
			if((menuLevel == 3) && (upPressed == 1||downPressed == 1))
			{
				ClearDisplay();
				PrintString("** MAIN  MENU **\r");
				SetToLn2();
				PrintString("-> Set limit\r");
			}
			if((menuLevel == 4) && (upPressed == 1||downPressed == 1))
			{
				ClearDisplay();
				PrintString("** MAIN  MENU **\r");
				SetToLn2();
				PrintString("-> About\r");
			}
			if((menuLevel == 5) && (upPressed == 1||downPressed == 1))
			{
				ClearDisplay();
				PrintString("** MAIN  MENU **\r");
				SetToLn2();
				PrintString("-> Ignition time\r");
			}
		}
	}
}


void viewRPM(void)
{
	unsigned char exit = 0;
	unsigned char valueString[9];
	ClearDisplay();
	PrintString("* Engine speed:\r");

	do
	{
		checkButtons();
		
		currentRPM = 3000000/ignPeriodTime;													// Calculate current RPM
		if(currentRPM>=10000)																// Check value
		{
			currentRPM -= currentRPM;
		}
		intToASCII(&(valueString[0]), currentRPM);
		valueString[4] = ' '; valueString[5] = 'R'; valueString[6] = 'P'; valueString[7] = 'M'; valueString[8] = 0x0D;
		PutCharAtPosLn2(0, ' '); PutCharAtPosLn2(1, ' ');
		PrintString(&(valueString[0]));
		DelayFunc(10000);
		if(selPressed == 1)
		{
			exit = 1;
			ClearDisplay();
			PrintString("** MAIN  MENU **\r");
		}
	}
	while(exit == 0);
}

void viewRPMbar(void)
{
	unsigned char exit = 0;
	unsigned char index, resIndex = 1;
	unsigned int longIndex, oldRPM = 0;
	ClearDisplay();
	PrintString("0    2    4    6r");
	SetToLn2();
	PutChar('*');
	do
	{
		currentRPM = 3000000/ignPeriodTime;													// Calculate current RPM

		if(oldRPM != currentRPM)															// Update bar, only if RPM value is updated
		{
			oldRPM = currentRPM;
			if(currentRPM>=10000)															// Check value
			{
				currentRPM -= currentRPM;
			}

			resIndex = 15;
			while(resIndex>0)
			{
				if(currentRPM>=((unsigned int)(resIndex*400)))
				{
					SetToLn2();
					PrintString("                ");
					index = 0;
					while(index < resIndex)
					{
						PutCharAtPosLn2(index, 0xFF);
						index++;
					}
					PutCharAtPosLn2(index, '*');
					resIndex = 1;
				}
				if(currentRPM<400)
				{
					SetToLn2();
					PrintString("*               ");
				}
				resIndex--;
			}
		}

		longIndex = 0;
		while((longIndex < 5000)&&(exit == 0))
		{
			checkButtons();
			if(selPressed == 1)
			{
				exit = 1;
				ClearDisplay();
				PrintString("** MAIN  MENU **\r");
			}
			longIndex++;
		}
	}
	while(exit == 0);
}


void setLimit(void)
{
	unsigned char exit = 0, index;
	unsigned char valueString[9];
	ClearDisplay();
	PrintString("* Set limit:\r");
	SetToLn2();
	if((currentLimit>9900)||(currentLimit<100))												// Check if value is valid
	{
		currentLimit = FIXED_LIMIT;
	}
	intToASCII(&(valueString[0]), currentLimit);
	valueString[4] = ' '; valueString[5] = 'R'; valueString[6] = 'P'; valueString[7] = 'M'; valueString[8] = 0x0D; 
	PutCharAtPosLn2(0, ' '); PutCharAtPosLn2(1, ' ');
	PrintString(&(valueString[0]));

	do
	{
		checkButtons();
		if(upPressed == 1)
		{
			currentLimit += LIMIT_STEP;
			if(currentLimit>RPM_ABS_STOP)
			{
				currentLimit = 0;
			}
			SetToLn2();
			intToASCII(&(valueString[0]), currentLimit);
			valueString[4] = ' '; valueString[5] = 'R'; valueString[6] = 'P'; valueString[7] = 'M'; valueString[8] = 0x0D; 
			PutCharAtPosLn2(0, ' '); PutCharAtPosLn2(1, ' ');
			PrintString(&(valueString[0]));
		}
	
		if(downPressed == 1)
		{
			currentLimit -= LIMIT_STEP;
			if(currentLimit>RPM_ABS_STOP)
			{
				currentLimit = 0;
			}
			if(currentLimit<100)
			{
				currentLimit = 0;
			}
			SetToLn2();
			SetToLn2();
			intToASCII(&(valueString[0]), currentLimit);
			valueString[4] = ' '; valueString[5] = 'R'; valueString[6] = 'P'; valueString[7] = 'M'; valueString[8] = 0x0D; 
			PutCharAtPosLn2(0, ' '); PutCharAtPosLn2(1, ' ');
			PrintString(&(valueString[0]));
		}

		if(selPressed == 1)
		{
			SetToLn2();
			PrintString("  Limit set!\r");
			for(index = 0; index < 5; index++)
			{
				DelayFunc(65000);
			}
			
			EEPROM_write(1, ((unsigned char)(currentLimit & 0xFF)));						// Save actual speed limit value to EEPROM, low byte
			EEPROM_write(0, ((currentLimit>>8) & 0xFF));									// Save actual speed limit value to EEPROM, high byte

			ClearDisplay();
			PrintString("** MAIN  MENU **\r");
			exit = 1;
		}
	}
	while(exit == 0);
	
	currentLimitTime = 3000000/currentLimit;
}


void about(void)
{
	unsigned char exit = 0;
	aboutText();
	do
	{
		checkButtons();
		
		if(selPressed == 1)
		{
			exit = 1;
			ClearDisplay();
			PrintString("** MAIN  MENU **\r");
		}
	}
	while(exit == 0);
}


void ignitionTime(void)
{
	unsigned char exit = 0;
	unsigned char valueString[9];
	unsigned int currentTime;
	ClearDisplay();
	PrintString("* Ignition time:\r");

	do
	{
		checkButtons();
		
		currentTime = ignPeriodTime/100;													// Calculate current ignition period time [ms]
		intToASCII(&(valueString[0]), currentTime);
		valueString[4] = ' '; valueString[5] = 'm'; valueString[6] = 's'; valueString[7] = 0x0D;
		PutCharAtPosLn2(0, ' '); PutCharAtPosLn2(1, ' ');
		PrintString(&(valueString[0]));
		DelayFunc(1000);
		if(selPressed == 1)
		{
			exit = 1;
			ClearDisplay();
			PrintString("** MAIN  MENU **\r");
		}
	}
	while(exit == 0);
}


/*-------- Initiate the project --------*/
void projectInit(void)
{

	// Input/Output Ports initialization

	// Port B initialization - Used for input buttons (Sel, Up, Down)
	// Func7=In Func6=In Func5=Out Func4=Out Func3=Out Func2=In Func1=In Func0=In 
	// State7=P State6=P State5=0 State4=0 State3=0 State2=P State1=P State0=T 
	PORTB=0xC6;
	DDRB=0x38;

	// Port C initialization - Used for display control signals
	// Func6=Out Func5=In Func4=Out Func3=Out Func2=Out Func1=Out Func0=Out 
	// State6=0 State5=P State4=0 State3=0 State2=0 State1=0 State0=0 
	PORTC=0x20;
	DDRC=0x1F;

	// Port D initialization - Used for display data bus
	// Func7=In Func6=In Func5=In Func4=In Func3=In Func2=In Func1=In Func0=In 
	// State7=T State6=T State5=T State4=T State3=T State2=T State1=T State0=T 
	PORTD=0x00;
	DDRD=0xFF;

	// External Interrupt(s) initialization - PCINT1 used for ignition interrupt
	// INT0: Off
	// INT1: Off
	// Interrupt on any change on pins PCINT0-7: Off
	// Interrupt on any change on pins PCINT8-14: On
	// Interrupt on any change on pins PCINT16-23: Off
	EICRA=0x00;
	EIMSK=0x00;
	PCICR=0x02;
	PCMSK1=0x20;
	PCIFR=0x02;
}

void timerInit(void)
{
	// Timer/Counter 0 initialization
	// Clock source: System Clock
	// Clock value: 125,000 kHz
	// Mode: CTC top = OCR0A (Clear TCNT0 on compare match)
	// OC0A output: Disconnected
	// OC0B output: Disconnected
	
	//TCCR0A=0x02;
	//TCCR0B=0x03;
	//TCNT0=0x00;
	//OCR0A=0x7D;
	//OCR0B=0x00;

	TCCR0A=0x02;
	TCCR0B=0x02;
	TCNT0=0x00;
	OCR0A=0x32;
	OCR0B=0x00;

	// Timer/Counter 0 Interrupt(s) initialization
	TIMSK0=0x02;
}

#undef SPEEDLIMITER_C
