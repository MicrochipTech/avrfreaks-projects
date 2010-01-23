/*
Author:  Matt Meerian
Date:  October 6, 2008
Target Processor:  ATtiny2313 from Atmel
Language used:  AVR GCC compiler (in the "C" language)
Overview:	The purpose of this software is to give automated responses to a user pressing a button
	on an intercomm box.  The overall design works like an updated version of the tape player intercomm
	in the movie "Ferris Bueller's Day Off"
	The microcontroller is used to control a VMusic2 module through the UART.  This module can play MP3
	files and send the audio out of an earphone port.  
compiler:  WINAVR 20080610
debugger:  AVR studio 4.14, build 589
*/ 

#include <avr\io.h>
#include <avr\interrupt.h>  //used to allow interrupts
//#include <avr\wdt.h>        //for the watchdog timer
#include <stdbool.h>    //this file allows boolean data types in "C"
//my header files
#include "define.h"		//defines for the current project, of course.
#include "uart.h"		//routines for the serial port
#include "avr\sleep.h"		//used to put the microcontroller in low power mode

//Declair the data types
typedef unsigned char u08;  //A character, 8 bits
typedef unsigned short u16; //a short integer, big endian 16 bits, 0 to 65535
typedef bool ubool;         //a boolean, true or false, must include the header file "stdbool.h"

//subroutine definitions
void SrvTock(void);
void uCinit(void);				//initialization routine for the ATMEGA32
/*---------------------------------------------------------------------------*/

/*global varibles*/
//The volatile keyword should be used when a variable is used in an interrupt routine
volatile u16 uTock=defTock;	//A counter used for the 20mS tock
u08 uState=0;			//the state for the state machine
u16 uTmrState=1;		//a state timer
u08 u08DebounceTalk=0;	//a debounce timer for the push to talk button
u08 u08TmrUserPressedButton=0;	//after the user pressed the button, this counts down to turning off the push to talk LED
u16 u16ReturnToStateZero=0;		//If the user presses a button, but doesn't get to the end of the dialog, this will return them to the start where the doorbell dings
u08 u08TmrRtsRelease;	//a timer used to turn off the RTS line going to the MP3 player

//**************************************************************************
//----------------------------------------------------------------------------
//Routine:     signal(sig_overflow0)  (timer 0 overflow interrupt)
//
//Function:    signal handler for tcnt0 overflow interrupt
//              happens every 64.39 microseconds
//Varibles passed:
//       none
//Returns:
//       none
//----------------------------------------------------------------------------
SIGNAL(SIG_OVERFLOW0)
{
	if (uTock > 0)
	{
      uTock--;      //This counts down to a 20ms tock.  (which is run in the main loop)
	}
}
 
//----------------------------------------------------------------------------
//Routine:     main (the main loop of the program)
//
//Function:    This is where to program will exicute until an interrupt happens or
//              someone yanks the power plug.
//
//Varibles passed:
//       none
//Returns:
//       integer (no idea where this integer is returned to)
//----------------------------------------------------------------------------
// Main loop
int main( void )  
{
	uCinit();		//go through the initialization process for the microcontroller
	sei();        	//enable interrupts
	while(1)       	//go into an infinite loop here
	{
        SrvTock();  	//go to the tock every 20ms
	}
	return 0;
} 
 
//----------------------------------------------------------------------------
//Routine:       SrvTock  (Service the tock)
//
//Function:      Exicute the tock every 20mS from the main loop.
//
//
//Varibles passed:
//       none
//Returns:
//       none
//----------------------------------------------------------------------------
void SrvTock(void)
{
    if (uTock == 0) 
    {
        uTock = defTock;   //reload the Tock (20mS) timer
        if(u08DebounceTalk!=0 && (defPushBtnPort & (1<<defTalkButton))!=0)     //the debounce timer
        {	// for the push to talk button, also the user has to of let go of the button to start counting
           	u08DebounceTalk--;     //decrement the push to talk debounce timer
        }
		//used to turn on and off the push to talk LED
		if((defPushBtnPort & (1<<defTalkButton))==0)	//is the push to talk button pressed?
		{
			defLedTalkPort |= (1<<defLedTalkPin);	//turn on the push to talk LED
			u08TmrUserPressedButton=20;		//start the timer to turn off of the push to talk LED	
		}
		if(u08TmrUserPressedButton>0)	//are we counting down to shutting off the push to talk LED?
		{
			u08TmrUserPressedButton--;	//counting down to turning off the LED
			if(u08TmrUserPressedButton==0)	//is it time to turn off the push to talk LED?
			{
				defLedTalkPort &= ~(1<<defLedTalkPin);	//turn off the push to talk LED				
			}
		}
		//are we counting down to releasing the RTS line going to the VMusic 2 MP3 player?
		if(u08TmrRtsRelease>0)	
		{
			if(u08TmrRtsRelease==1)	//is it time to shut off the RTS line
			{
				defRtsPort &= ~(1<<defRtsPin);	//release the RTS line going to the VMUSIC 2 MP3 player
			}
			u08TmrRtsRelease--;		//counting down!
		}
		//resets the system back to state zero if there have not been any button presses for awhile
		if(uState!=0)	//are we in a state other than the starting state?
		{
			if(u16ReturnToStateZero>0)	//are we counting down to the start of dialog?
			{
				u16ReturnToStateZero--;	//yes
			}
			else
			{
				uState=0;	//there must have not been any button presses for at least 2 minutes
			}
		}

		//--------------------------------------------------------------------------------
        switch (uState)
        {
            case 0:  //MP3 playter:  "ding-dong"
				if((defPushBtnPort & (1<<defTalkButton))==0 && u08DebounceTalk==0)	//is the minute button pressed and has not been previously pressed in awhile?
				{
					defRtsPort |= (1<<defRtsPin);	//set the RTS line going to the Vmusic2 module high
					uart_puts_P("VPF a.mp3\r");	//send a command to the VMusic2 module to start playing a sound file
							// \r is a carriage return, dec 13
	           		u08TmrRtsRelease=defRtsTime;	//start the countdown to releasing the RTS line
					u08DebounceTalk=defBtnDbn;   //load up the debounce timer, so we don't immediately go back in here
					uTmrState=300;	//start the timer to leave this state
					u08TmrUserPressedButton=250;
				}
                if(uTmrState>1)	//are we counting down to leaving the state?
				{
					uTmrState--;	//yes
					if(uTmrState==2)	//is it time to leave the state?
					{
						uState=1;		//leave the state
						uTmrState=100;
	                }
				}
				u16ReturnToStateZero=defTwoMinutes;	//(.02)(50)(60)=3000 tocks per minute
		    break;

            case 1:  //MP3 playter:  "Who is it?"
				if(uTmrState==100)
				{
					defLedTalkPort |= (1<<defLedTalkPin);	//turn on the push to talk LED
				}

				if(uTmrState==75)	
				{
					defRtsPort |= (1<<defRtsPin);	//set the RTS line going to the Vmusic2 module high
					uart_puts_P("VPF b.mp3\r");	//send a command to the VMusic2 module to start playing a sound file
	           		u08TmrRtsRelease=defRtsTime;	//start the countdown to releasing the RTS line
				}
                if(uTmrState>1)	//are we counting down to leaving the state?
				{
					uTmrState--;	//yes
					if(uTmrState==2)	//is it time to leave the state?
					{
						uState=2;		//leave the state
						defLedTalkPort &= ~(1<<defLedTalkPin);	//turn off the push to talk LED
	                }
				}
            break;

            case 2:  //listen for the user to release the push to talk button
				if((defPushBtnPort & (1<<defTalkButton))==0 && u08DebounceTalk==0)	//is the minute button pressed and has not been previously pressed in awhile?
				{
					uTmrState=100;	//start the timer to leave this state
				}
                if(uTmrState>10)	//are we counting down to leaving the state?
				{
					uTmrState--;	//yes
					if(uTmrState==11)	//is it time to leave the state?
					{
						uState=3;		//leave the state
						uTmrState=600;
	                }
				}
			break;

            case 3:  //MP3 playter:  "Oh, I'm sorry..."
				if(uTmrState==599)
				{
					defLedTalkPort |= (1<<defLedTalkPin);	//turn on the push to talk LED
				}
				if(uTmrState==550)	
				{
					defRtsPort |= (1<<defRtsPin);	//set the RTS line going to the Vmusic2 module high
					uart_puts_P("VPF c.mp3\r");	//send a command to the VMusic2 module to start playing a sound file
	           		u08TmrRtsRelease=defRtsTime;	//start the countdown to releasing the RTS line
				}
                if(uTmrState>1)	//are we counting down to leaving the state?
				{
					uTmrState--;	//yes
					if(uTmrState==2)	//is it time to leave the state?
					{
						uState=4;		//leave the state
						defLedTalkPort &= ~(1<<defLedTalkPin);	//turn off the push to talk LED
	                }
				}
            break;

            case 4:  //listen for the user to release the push to talk button
				if((defPushBtnPort & (1<<defTalkButton))==0 && u08DebounceTalk==0)	//is the minute button pressed and has not been previously pressed in awhile?
				{
					uTmrState=100;	//start the timer to leave this state
				}
                if(uTmrState>10)	//are we counting down to leaving the state?
				{
					uTmrState--;	//yes
					if(uTmrState==11)	//is it time to leave the state?
					{
						uState=5;		//leave the state
						uTmrState=150;
	                }
				}
			break;

            case 5:  //MP3 playter:  "have a nice day."
				if(uTmrState==149)
				{
					defLedTalkPort |= (1<<defLedTalkPin);	//turn on the push to talk LED
				}
				if(uTmrState==75)	
				{
					defRtsPort |= (1<<defRtsPin);	//set the RTS line going to the Vmusic2 module high
					uart_puts_P("VPF d.mp3\r");	//send a command to the VMusic2 module to start playing a sound file
	           		u08TmrRtsRelease=defRtsTime;	//start the countdown to releasing the RTS line
				}
                if(uTmrState>1)	//are we counting down to leaving the state?
				{
					uTmrState--;	//yes
					if(uTmrState==2)	//is it time to leave the state?
					{
						uState=6;		//leave the state
						defLedTalkPort &= ~(1<<defLedTalkPin);	//turn off the push to talk LED
	                }
				}
            break;

/*
			case 6:		//just wait here

			break;

			case 10:	//plays all of the mp3's on the thumb drive
				if((defPushBtnPort & (1<<defTempButton))==0 && u08DebounceTemp==0)	//
				{
					uTmrState=3;	//start the timer to leave this state
					u08DebounceTemp=50;
				}
				if(uTmrState==3)
				{
					defRtsPort |= (1<<defRtsPin);	//set the RTS line going to the Vmusic2 module high
				}
				if(uTmrState==2)
				{				
					//uart_puts_P("VPF a.mp3\r");	//send a command to the VMusic2 module to start playing a sound file \r is a carriage return, dec 13
					//uart_puts_P("VSV 0\r");
					uart_puts_P("V3A\r");	//play all of the tracks on the thumb drive
				}
				if(uTmrState==1)
				{
					defRtsPort &= ~(1<<defRtsPin);	//set the RTS line going to the Vmusic2 module high
				}
				if(uTmrState>0)
				{
					uTmrState--;
				}
				if(u08DebounceTemp>0)
				{
					u08DebounceTemp--;
				}
			break;
*/
		}
	}
}

//----------------------------------------------------------------------------
//Routine:     uCinit  (initialization of the microcontroller)
//
//Function:    initialize the I/O lines, timers, interrupts, etc.
//
//Varibles passed:
//       none
//Returns:
//       none
//----------------------------------------------------------------------------
void uCinit(void) 
{
   DDRB=0xFD;           //only the front panel push button is an input
   PORTB=0x02;          //enable pull-up resistors
   DDRD=0xFF;           //Portd outputs
   PORTD=0x00;          //PortD pullup resistors

   //8 bit timer 0 initialization, used for the 20mS tock.
    TIMSK=(1<<TOIE0);	//enable timer 0 overflow
    TCNT0=0;            //reset TCNT0
    TCCR0B=1;           //count with cpu clock /1

	uart_init((4000000)/((9600)*16l)-1);	//set the uart to value for 9600 baud, (4MHz crystal)
}


