/*------------------------------------------------------------------------
|
|	Project:		speedLimiter
|					displayDrv.c
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

#include <avr/io.h>
#include "speedLimiter.h"
#include "displayDrv.h"
#include "miscFuncs.h"


#define DISPLAYDRV_C

void InitDisplay(void)
{
	DISPL_DATA(0x0C);                                   	 // On, cursor, blink
	DelayFunc(2000);
	DISPL_EN_SET;
	DelayFunc(2000);
	DISPL_EN_CLR;
	DelayFunc(2000);

	DISPL_DATA(0x01);                                   	 // Clear
	DelayFunc(2000);
	DISPL_EN_SET;
	DelayFunc(2000);
	DISPL_EN_CLR;
	DelayFunc(2000);


	DISPL_DATA(0x3C);                                   	// Bus lenght, nmb of lines, font
	DelayFunc(2000);
	DISPL_EN_SET;
	DelayFunc(2000);
	DISPL_EN_CLR;
	DelayFunc(2000);
}

void ClearDisplay(void)
{
	DISPL_DATA(0x01);                                   	 // Clear
	DelayFunc(500);
	DISPL_EN_SET;
	DelayFunc(500);
	DISPL_EN_CLR;
	DelayFunc(500);
}

void SetToLn2(void)
{
    	// Init display to line 2
		DISPL_DATA(0xC0);                                   // DDRAM addr. 40h
		DelayFunc(500);
		DISPL_EN_SET;
		DelayFunc(500);
		DISPL_EN_CLR;
		DelayFunc(500);
}

void PutCharAtPosLn1(int pos, int inputChar)
{
	// Init display to line 1
	DISPL_DATA((0x80+pos));                                	// DDRAM addr. 00h + value of 'pos'
	DelayFunc(500);
	DISPL_EN_SET;
	DelayFunc(500);
	DISPL_EN_CLR;
	DelayFunc(500);

	DISPL_DATA(inputChar);									// Output the character at selected position
	DISPL_RS_SET;
	DelayFunc(500);
	DISPL_EN_SET;
	DelayFunc(500);
	DISPL_EN_CLR;
	DISPL_RS_CLR;
}

void PutCharAtPosLn2(int pos, int inputChar)
{
	// Init display to line 2
	DISPL_DATA((0xC0+pos));                                	// DDRAM addr. 40h + value of 'pos'
	DelayFunc(50);
	DISPL_EN_SET;
	DelayFunc(50);
	DISPL_EN_CLR;
	DelayFunc(50);

	DISPL_DATA(inputChar);									// Output the character at selected position
	DISPL_RS_SET;
	DelayFunc(50);
	DISPL_EN_SET;
	DelayFunc(50);
	DISPL_EN_CLR;
	DISPL_RS_CLR;
}

void PutChar(int inputChar)
{
	DISPL_DATA(inputChar);
	DISPL_RS_SET;
	DelayFunc(50);
	DISPL_EN_SET;
	DelayFunc(50);
	DISPL_EN_CLR;
	DISPL_RS_CLR;
}

void PrintString(unsigned char *firstChar)
{

	unsigned int charPtr = 0;

	while((charPtr < 16) && (*(firstChar + charPtr) != 0xD))
	{
		PutChar((int)(*(firstChar + charPtr)));
		charPtr++;
	}


}

#undef DISPLAYDRV_C
