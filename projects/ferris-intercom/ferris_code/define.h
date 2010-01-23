/*
Author:  Matt Meerian
Date:  October 6, 2008
Target Processor:  ATtiny2313 from Atmel
Language used:  AVR GCC compiler (in the "C" language)
compiler:  WINAVR 20080610
debugger:  AVR studio 4.14, build 589
*/ 

//defines

#define defTock            311      //There are 311 overflows to make 20mS on a 64.39uS timer
#define defStateTm         200      //There are 50, 20mS counts to make 1 sec
#define defBtnDbn          14       //this is the front panel button debounce, was orginally 10

#define defPushBtnPort		PINB	//This is the port for the push buttons
#define defTalkButton		1		//This is the pin on the microcontroller that toggles when the user presses the talk button
#define defRtsPort			PORTB	//the request to send port for going to the Vmusic2 module
#define defRtsPin			0		//the request to send pin for the VMUSIC2 module
#define defLedTalkPort		PORTD	//the port that controls the output LED
#define defLedTalkPin		3		//this pin controls the LED talking indication

#define defTwoMinutes		6000	//(.02)(50)(60)=3000 tocks per minute
#define defRtsTime			20	//this sets the time to release the RTS line


