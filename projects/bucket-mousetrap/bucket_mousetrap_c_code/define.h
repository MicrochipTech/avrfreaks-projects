/*
Author:  Matt Meerian
Date:  January 3, 2007
Purpose:  This file contains constants for the bucket mousetrap project.
*/

//portD equates
#define def74ls138A0		3		//74LS138 decoder address 0
#define def74ls138A1		2		//74LS138 decoder address 1
#define defBriteLED1		4		//this is the output for the visible, high intenisty, LED
#define defBriteLED2		5		//this is the output for the visible, high intenisty, LED
#define defSolenoid			6		//output pin that drives the solenoid that drops the bucket

//PORTB equates
#define defIrRx1			PB2		//IR receiver input pin
#define defIrRx2			PB1		//IR receiver input pin
#define defIrRx3			PB0		//IR receiver input pin
#define defIrRx4			PB4		//IR receiver input pin
#define defIrTx				3		//this is the output of timer1, it drives the 38KHz signal going to the LED's
#define defBriteLED3		5		//this is the output for the visible, high intenisty, LED
#define defBriteLED4		7		//this is the output for the visible, high intenisty, LED

#define defNumBlinks		20		//When the 30 second test phase is running after powerup, this sets the 
									// number times the visible leds will blink
#define defScanningRate		15		//This sets the number of 20mS "tocks" to wait until the IR sensor is read
