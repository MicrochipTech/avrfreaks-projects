/*!
Author:  Matt Meerian
Date:  January 2, 2009
Purpose:  
	constants for the linear motion 3D LED matrix

Developement System:  AVR studio Version 4.15, build 623
Compiler:  WINAVR 20081205
Target Microcontroller:  ATMega16 from Atmel (AVR series)
*/

#define F_CPU              16000000  //! The clock at which the microcontroller will run

#define defTock            311  //! There are 311 overflows to make 20mS on a 64.39uS timer
#define defStateTm         3 	//! There are 50, 20mS counts to make 1 sec

