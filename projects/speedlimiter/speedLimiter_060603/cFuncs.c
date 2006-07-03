/*------------------------------------------------------------------------
|
|	Project:		speedLimiter
|					miscFuncs.c
|
|	Author:			Rasmus Renzi,	rasmus.r@spray.se
|
|
|
--------------------------------------------------------------------------
|
|	Description:	Speedlimiter for engines with mechanical ignition
|					systems.
--------------------------------------------------------------------------*/

#define MISCFUNCS_C

// Include files
#include <avr/io.h>
#include <avr/interrupt.h>
#include <inttypes.h>
#include "speedLimiter.h"
#include "displayDrv.h"
#include "miscFuncs.h"


void aboutText(void)
{
	ClearDisplay();
	PrintString("Author:...Date:\r");
	SetToLn2();
	PrintString(AUTHOR_DATE);
}


/*--------I'm alive indication function-----------*/
void imAlive(void)
{                 
	lifeCycleCnt++;
	if(lifeCycleCnt > MAX_LIFE_CYCLES)
	{
	    lifeCycleCnt = 0;
	    if(blink == 0)
	    {
		    blink = 1;
		    LIFE_SET;
	    }
	    else
	    {
	        blink = 0;
	        LIFE_CLR;
	    }
	}
}


void checkButtons(void)
{
	upPressed = 0; downPressed = 0; selPressed = 0; tmpBit = 0;

	MENU_UP(tmpBit);														// Check UP button
	if(tmpBit==1)
	{
		DelayFunc(5000);
		MENU_UP(tmpBit);
		if(tmpBit==1)														// Debounce
		{
			do
			{
				MENU_UP(tmpBit);
			}
			while(tmpBit==1);												// Wait for button to be released
			upPressed = 1;
		}
	}

	MENU_DN(tmpBit);														// Check DOWN button
	if(tmpBit==1)
	{
		DelayFunc(5000);
		MENU_DN(tmpBit);
		if(tmpBit==1)														// Debounce
		{
			do
			{
				MENU_DN(tmpBit);
			}
			while(tmpBit==1);												// Wait for button to be released
			downPressed = 1;
		}
	}

	MENU_SL(tmpBit);														// Check if current menu item is selected
	if(tmpBit==1)
	{
		DelayFunc(5000);
		MENU_SL(tmpBit);
		if(tmpBit==1)														// Debounce
		{
			do
			{
				MENU_SL(tmpBit);
			}
			while(tmpBit==1);												// Wait for button to be released
			selPressed = 1;
		}
	}
}

 
void intToASCII(char *outputString, unsigned int inputValue)
{
	unsigned char nmb1, nmb2, nmb3, nmb4;

	nmb4 = inputValue/1000;
	nmb3 = (inputValue - (nmb4*1000))/100;
	nmb2 = (inputValue - (nmb4*1000) - (nmb3*100)) / 10; 
	nmb1 = (inputValue - (nmb4*1000) - (nmb3*100) - (nmb2*10));

	outputString[3] = nmb1 + 48;
	outputString[2] = nmb2 + 48 ;
	outputString[1] = nmb3 + 48 ;
	outputString[0] = nmb4 + 48;
}

 
/*--------Delay function-----------*/
void DelayFunc(unsigned int delayCount)
{
    do
    {
    	delayCount--;        
    }
    while(delayCount > 0);
}


void SplashText()
{
	unsigned char spalshStrn[16];
	unsigned int index = 0;

	// Write splash text - Ln1: speedCounter, Ln2:
	spalshStrn[0]=' '; spalshStrn[1]=' '; spalshStrn[2]='s'; spalshStrn[3]='p'; spalshStrn[4]='e'; spalshStrn[5]='e';
	spalshStrn[6]='d'; spalshStrn[7]='C'; spalshStrn[8]='o'; spalshStrn[9]='u'; spalshStrn[10]='n'; spalshStrn[11]='t';
	spalshStrn[12]='e'; spalshStrn[13]='r'; spalshStrn[14]=' '; spalshStrn[15]=' ';
	ClearDisplay();
	for(index = 0; index < 16; index++)
	{
		PutCharAtPosLn1(index, 0xFF);
		DelayFunc(15000);
	}
	for(index = 0; index < 16; index++)
	{
		PutCharAtPosLn1(index, spalshStrn[index]);
		DelayFunc(15000);
	}
			
	for(index = 0; index < 16; index++)
	{
		PutCharAtPosLn2(index, 0xFF);
		DelayFunc(15000);
	}
	for(index = 0; index < 16; index++)
	{
		PutCharAtPosLn2(index, ' ');
		DelayFunc(15000);
	}

	// Ln1: Firmware, Ln2: Revision x.x.x
	spalshStrn[0]=' '; spalshStrn[1]=' '; spalshStrn[2]=' '; spalshStrn[3]=' '; spalshStrn[4]='F'; spalshStrn[5]='i';
	spalshStrn[6]='r'; spalshStrn[7]='m'; spalshStrn[8]='w'; spalshStrn[9]='a'; spalshStrn[10]='r'; spalshStrn[11]='e';
	spalshStrn[12]=' '; spalshStrn[13]=' '; spalshStrn[14]=' '; spalshStrn[15]=' ';
	ClearDisplay();
	for(index = 0; index < 16; index++)
	{
		PutCharAtPosLn1(index, 0xFF);
		DelayFunc(10000);
	}
	for(index = 0; index < 16; index++)
	{
		PutCharAtPosLn1(index, spalshStrn[index]);
		DelayFunc(10000);
	}
	spalshStrn[0]='R'; spalshStrn[1]='e'; spalshStrn[2]='v'; spalshStrn[3]='i'; spalshStrn[4]='s'; spalshStrn[5]='i';
	spalshStrn[6]='o'; spalshStrn[7]='n'; spalshStrn[8]=':'; spalshStrn[9]=' '; spalshStrn[10]='1'; spalshStrn[11]='.';
	spalshStrn[12]='0'; spalshStrn[13]='.'; spalshStrn[14]='2'; spalshStrn[15]=' ';

	for(index = 0; index < 16; index++)
	{
		PutCharAtPosLn2(index, 0xFF);
		DelayFunc(10000);
	}
	for(index = 0; index < 16; index++)
	{
		PutCharAtPosLn2(index, spalshStrn[index]);
		DelayFunc(10000);
	}

	for(index = 0; index < 5; index++)
	{
		DelayFunc(65000);
	}

	aboutText();

	for(index = 0; index < 8; index++)
	{
		DelayFunc(30000);
	}

	ClearDisplay();
	PrintString("** MAIN  MENU **\r");
}


#undef MISCFUNCS_C
