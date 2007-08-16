/*

	 Servo functions (servo.c) for Atmega16 and WINAVR

	 Version 0.1ß		2005-10-06

	 Copyright (C) Andreas Kingbäck 2005  (andki@itn.liu.se)

	 This program is free software; you can redistribute it and/or modify it 
	 under the terms of the GNU General Public License as published by the 
	 Free Software Foundation; either version 2 of the License, or 
	 (at your option) any later version.

	 This program is distributed in the hope that it will be useful, 
	 but WITHOUT ANY WARRANTY; without even the implied warranty of 
	 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
	 See the GNU General Public License for more details.

	 You should have received a copy of the GNU General Public License along with this program; 
	 if not, write to the Free Software Foundation, Inc., 
	 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA

*/

#pragma language=extended

#ifndef ENABLE_BIT_DEFINITIONS
#define ENABLE_BIT_DEFINITIONS
// Enable the bit definitions in the iom128.h file
#endif

#include <ioavr.h>
#include <stdio.h>
#include "servo.h"


void InitPWM(void)
{
	// Confige PWM-Generator
	
	DDRD    = 0xFF;											// OC0A and OC0B as Output
	TCCR1A  = (1 << WGM10)| (1 << WGM11);							// PWM, Phase Correct
	TCCR1A |= (1 << COM1A0)|(1 << COM1A1)| (1 << COM1B1)|(1 << COM1B0); 						// Set at compare cleared at TOP
	TCCR1B  = (1 << CS11) | (1 << CS10); 							// clkio/64
	
	SetServo_Y_Pos(0);
	SetServo_X_Pos(0);
}


void SetServo_Pos(char *twi_data)					// Set servo pos from twi data
{
	float v;
	
	v = (twi_data[2] << 8) + twi_data[1];
	switch (twi_data[0])
	{
		case CM_SET_X_SERVO:
			SetServo_X_Pos(v);
			break;
			
		case CM_SET_Y_SERVO:
			SetServo_Y_Pos(v);
			break;
	}
}
	

void SetServo_X_Pos(float v)		// X Servo på -90 to +90 deg. Must be float to handle multiplication
{

	float x;

	x = (v * 0.5444) + x_center; // ((x_left90-x_right90)/180) = 0.5444
	
	if (x > x_left90) 
		x = x_left90;
	if (x < x_right90)
		x = x_right90;
		
	X_SERVO=x;
}

void SetServo_Y_Pos(int v)		// Y Servo på -45 to +90 deg
{
	int y;

	y = (v * 0.5444) + y_center; // ((x_left90-x_right90)/180) = 0.5444 if y used div with actual swing..
		
	if (y > y_maxdown) 
		y = y_maxdown;
	if (y < y_up90)
		y = y_up90;
		
	Y_SERVO=y;
	
}
