/*
Author:  Matt Meerian
Date:  August 22, 2010
Target Processor:  ATtiny2313
Languate used:  AVR GCC compiler (in the "C" language)
Overview:
	This software is used to control an airsoft gun to shoot an airsoft bullet through a small
	spinning airplane propeller.  Propeller drivin aircraft in the 1920's era used an interrupt
	gear to mechanically shoot bullets through an airplane's propeller.

compiler/debugger developement system:  AVR studio 4.18, build 684
*/

#ifndef define_file_H
#define define_file_H

void SrvTock(void);  	//the main tock routine
void uCinit(void);		//initialization routine
void toggleLED();

//defines
#define defTock         	100		//Controls the cyclopsing period of the LED's, 400 is really slow back and forth, 100 is quickly back and forth

#define defPortTrigger	PORTD		//
#define defPinTrigger 3				//

#define defSolenoidPort PORTD		//the pin that drives the trigger solenoid
#define defSolenoidPin	6

#define defStatusLedPort	PORTB	//used for LED output
#define defStatusLedPin		2

#define defFireButtonPort	PINB	//user input to "go"
#define defFireButtonPin	4

#define defLedOnTime	0x0010		//this sets the amount of time the LED will be on durning the prop cycle
#define defBladePosDivider	0x02	//this is the time

#define defTimer1TicksForSolenoid	320		//When the solenoid activates, there is a 20mS delay for the pellet to come
											//  out of the barrel, this is the number of Timer1 ticks.

#define defHitTheBlade			//are we going to try to hit one of the blades with the pellet?

#endif

