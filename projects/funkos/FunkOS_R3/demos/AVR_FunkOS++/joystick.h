//---------------------------------------------------------------------------
// FunkOS - Copyright (c) 2009, Funkenstein Software, See license.txt for details
//---------------------------------------------------------------------------
/*! \file  joystick.h
 
	Description:
		Joystick I/O structure definition
*/

#ifndef __JOYSTICK_H_
#define __JOYSTICK_H_

#include "types.h"
#include "driver.h"

//---------------------------------------------------------------------------
typedef enum
{
	JOY_IN_READ = DRIVER_EVENTS,
	JOY_IN_COPY
} JOY_IN_EVENT;

//---------------------------------------------------------------------------
typedef enum
{
	JOY_OUT_WRITE = DRIVER_EVENTS,
	JOY_OUT_COPY
} JOY_OUT_EVENT;

//---------------------------------------------------------------------------
// Create a union between a single unsigned long (32-bit) and the various
// types of joystick data.

typedef struct
{	
	//-------------------------------------------------------------------
	// Define a struct for the digital I/O common to most joypads.
	//-------------------------------------------------------------------
	union
	{
		//-------------------------------------------------------------------
		// Raw Data field - for easy copying.
		//-------------------------------------------------------------------
		ULONG ulRawData;	
		
		//-------------------------------------------------------------------
		// Raw type - D-pad and 28 buttons.. for custom joystick hardware
		//-------------------------------------------------------------------
		struct
		{
			unsigned int bUp:1;
			unsigned int bDown:1;
			unsigned int bLeft:1;
			unsigned int bRight:1;
			
			unsigned int abButton1:1;
			unsigned int abButton2:1;
			unsigned int abButton3:1;
			unsigned int abButton4:1;
			unsigned int abButton5:1;
			unsigned int abButton6:1;
			unsigned int abButton7:1;
			unsigned int abButton8:1;
			unsigned int abButton9:1;
			unsigned int abButton10:1;
			unsigned int abButton11:1;
			unsigned int abButton12:1;
			unsigned int abButton13:1;
			unsigned int abButton14:1;
			unsigned int abButton15:1;
			unsigned int abButton16:1;
			unsigned int abButton17:1;
			unsigned int abButton18:1;
			unsigned int abButton19:1;
			unsigned int abButton20:1;
			unsigned int abButton21:1;
			unsigned int abButton22:1;
			unsigned int abButton23:1;
			unsigned int abButton24:1;
			unsigned int abButton25:1;
			unsigned int abButton26:1;
			unsigned int abButton27:1;
			unsigned int abButton28:1;
		} JOY_RAW;
		
		//-------------------------------------------------------------------
		// Define these structs so that they convert to/from valid formats
		// from one joy type to another using only the ulRawData field.
		//-------------------------------------------------------------------
		struct
		{
			unsigned int bDirs:4;
			unsigned int bFaceButtons:6;
			unsigned int bSelectStart:2;
			unsigned int bAuxButtons1:4;
			unsigned int bAuxButtons2:8;
			unsigned int bAuxButtons3:8;
		}  JOY_BUTTON_GROUP;

		//-------------------------------------------------------------------		
		// NES Type - A B SELECT START
		//-------------------------------------------------------------------
		struct
		{
			// 4 bits
			unsigned int bUp:1;
			unsigned int bDown:1;
			unsigned int bLeft:1;
			unsigned int bRight:1;
					
			// 6 bits
			unsigned int bA:1;
			unsigned int bB:1;
			unsigned int u_NA1:2;
			unsigned int u_NA2:2;
			
			// 2 bits
			unsigned int bSelect:1;
			unsigned int bStart:1;
			
			// 20 bits
			unsigned int u_NA3:4;
			unsigned int u_NA4:8;
			unsigned int u_NA5:8;
		} JOY_NES;
		
		//-------------------------------------------------------------------
		// SNES Type - A B X Y L R Select Start
		//-------------------------------------------------------------------
		struct
		{
			//4 bits
			unsigned int bUp:1;
			unsigned int bDown:1;
			unsigned int bLeft:1;
			unsigned int bRight:1;
			
			//6 bits
			unsigned int bA:1;
			unsigned int bB:1;
			unsigned int bL:1;
			unsigned int bX:1;
			unsigned int bY:1;
			unsigned int bR:1;
			
			//2 bits
			unsigned int bSelect:1;
			unsigned int bStart:1;
			
			//20 bits
			unsigned int u_NA1:4;
			unsigned int u_NA2:8;
			unsigned int u_NA3:8;
		} JOY_SNES;
		
		//-------------------------------------------------------------------
		// Genesis type - A B C / X Y Z Mode Start
		//-------------------------------------------------------------------
		struct
		{
			//4 bits
			unsigned int bUp:1;
			unsigned int bDown:1;
			unsigned int bLeft:1;
			unsigned int bRight:1;
			
			//6 bits			
			unsigned int bA:1;
			unsigned int bB:1;
			unsigned int bC:1;
			unsigned int bX:1;
			unsigned int bY:1;
			unsigned int bZ:1;			
			
			//2 bits
			unsigned int bMode:1;
			unsigned int bStart:1;
			
			//20 bits
			unsigned int u_NA1:4;
			unsigned int u_NA2:8;
			unsigned int u_NA3:8;
		} JOY_GEN;
		
		//-------------------------------------------------------------------
		// TG-16 type - I II SELECT RUN
		//-------------------------------------------------------------------
		struct
		{
			//4 bits
			unsigned int bUp:1;
			unsigned int bDown:1;
			unsigned int bLeft:1;
			unsigned int bRight:1;
			
			//6 bits			
			unsigned int bI:1;
			unsigned int bII:1;
			unsigned int u_NA1:2;
			unsigned int u_NA2:2;
			
			//2 bits
			unsigned int bSelect:1;
			unsigned int bRun:1;
			
			//20 bits
			unsigned int u_NA3:4;
			unsigned int u_NA4:8;
			unsigned int u_NA5:8;
		} JOY_TG16;
		
		//-------------------------------------------------------------------
		// 1-button arcade stick
		//-------------------------------------------------------------------
		struct
		{
			//4 bits
			unsigned int bUp:1;
			unsigned int bDown:1;
			unsigned int bLeft:1;
			unsigned int bRight:1;
			
			//6 bits			
			unsigned int bButton:1;
			unsigned int u_NA:3;
			
			//20 bits
			unsigned int u_NA2:8;
			unsigned int u_NA3:8;
			unsigned int u_NA4:8;
		} JOY_ARCADE;
	
		//-------------------------------------------------------------------
		// Atari 2600 - Button 1
		//-------------------------------------------------------------------
		struct
		{
			//4 bits
			unsigned int bUp:1;
			unsigned int bDown:1;
			unsigned int bLeft:1;
			unsigned int bRight:1;
			
			//6 bits			
			unsigned int bButton:1;
			unsigned int u_NA:3;
			
			//20 bits
			unsigned int u_NA2:8;
			unsigned int u_NA3:8;
			unsigned int u_NA4:8;
		} JOY_ATARI2600;
		
		//-------------------------------------------------------------------
		// Atari 7800 - Button L R 
		//-------------------------------------------------------------------
		struct
		{
			//4 bits
			unsigned int bUp:1;
			unsigned int bDown:1;
			unsigned int bLeft:1;
			unsigned int bRight:1;
			
			//6 bits			
			unsigned int bButtonL:1;
			unsigned int bButtonR:1;
			unsigned int u_NA:4;
			
			//2 bits
			unsigned int u_NA2:2;
			
			//20 bits
			unsigned int u_NA3:4;
			unsigned int u_NA4:8;
			unsigned int u_NA5:8;
		} JOY_ATARI7800;
		
		//-------------------------------------------------------------------
		// PSX - L1 L2 L3 R1 R2 R3 Tr Sq X O Select Start
		//-------------------------------------------------------------------
		struct
		{
			//4 bits
			unsigned int bUp:1;
			unsigned int bDown:1;
			unsigned int bLeft:1;
			unsigned int bRight:1;
			
			//6 bits			
			unsigned int bX:1;
			unsigned int bO:1;
			unsigned int bTriangle:1;
			unsigned int bSquare:1;
			unsigned int bL1:1;
			unsigned int bR1:1;
			
			// 2 bits			
			unsigned int bSelect:1;
			unsigned int bStart:1;
			
			// 20 bits
			unsigned int bL2:1;
			unsigned int bL3:1;
			unsigned int bR2:1;
			unsigned int bR3:1;
			unsigned int u_NA1:8;
			unsigned int u_NA2:8;
		} JOY_PSX;
	} JOY_DIGITAL;
	
	//-----------------------------------------------------------------------
	// Analog controls (0-65535) - 2X, 2Y Axis.
	//-----------------------------------------------------------------------
	USHORT usXAxis1;
	USHORT usYAxis1;
	
	USHORT usXAxis2;
	USHORT usYAxis2;
	
} JOYSTICK_REPORT_STRUCT;

//---------------------------------------------------------------------------
void Joystick_Copy(JOYSTICK_REPORT_STRUCT *pstDst_, JOYSTICK_REPORT_STRUCT *pstSrc_);
void Joystick_Clear(JOYSTICK_REPORT_STRUCT *pstJoy_);
void Joystick_Delay(USHORT usDelayUS_);
#endif //__JOYSTICK_H_
