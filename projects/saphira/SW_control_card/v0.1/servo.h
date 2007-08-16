/*

	 Servo functions (servo.h) for Atmega16 and WINAVR

	 Version 0.1ß		2005-10-14

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


	X_SERVO:
	
	90 deg left 	= 974
	90 deg right 	= 876
	center 				= 925
	
	Y_SERVO
	
	90 deg up	 		= 880
	max down tilt = 950
	level					= 925

*/


#ifndef _SERVO_H_
#define _SERVO_H_

#define Y_SERVO OCR1A
#define X_SERVO OCR1B

#define x_left90	974.0
#define	x_center 	925.0
#define x_right90	876.0

#define y_up90		880.0
#define	y_center 	925.0
#define y_maxdown	950.0

// Commands

#define  CM_SET_X_SERVO	 	0
#define  CM_SET_Y_SERVO   1
#define  cm_failed				255;


#define true 1
#define false 0

void InitPWM(void);
void SetServo_Pos(char *twi_data);
void SetServo_X_Pos(float v);
void SetServo_Y_Pos(int v);

#endif		/* _SERVO_H_ */
