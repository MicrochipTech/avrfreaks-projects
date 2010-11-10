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

#include <avr\io.h>
#include <avr\interrupt.h>  //used to allow interrupts
#include <stdbool.h>    	//this file allows boolean data types in "C"
#include <avr\wdt.h>        //for the watchdog timer
#include <stdlib.h>			//used for itoa routine
#include "x14-30xx_interupt_gear.h"	//defines, must be before the global variables since some global

//---------------------------------------------------------------------------
//Declair the data types
typedef unsigned char u08;  //A character, 8 bits
typedef unsigned short u16; //a short integer, big endian 16 bits, 0 to 65535
typedef bool ubool;         //a boolean, true or false, must include the header file "stdbool.h"

/*global varibles*/
//Note:  Variables used in interrupts must be declaired as volatile
volatile u16 uTock=defTock;	//A counter used for the 20mS tock
volatile u08 u08State=1;		//the state for the state machine 
u08 uTmrState=0;	//a state timer
volatile u08 u08SubState=0;	//controls where we are at in each state
volatile u08 u08Toggle;		//used to blink an LED
//volatile bool boolWaitingForProp=false;		//
volatile unsigned int intRPM[4];	//an array of intergers used to store the time between props passing
volatile u08 u08RpmCounter;		//stores where we are in taking several RPM readings
volatile u08 u08Timer1State;	//a sub state variable used in timer1 (weither we are turning on the LED or turning it off)
volatile unsigned int intTemp;

//macros
#define SETBIT(ADDRESS,BIT) (ADDRESS |= (1<<BIT))		//Set and clear bits in I/O registers 
#define CLEARBIT(ADDRESS,BIT) (ADDRESS &= ~(1<<BIT))
#define CHECKBIT(ADDRESS,BIT) (ADDRESS & (1<<BIT)) 		//Macro for testing of a single bit in an I/O location

/*
software flow:
	Determine RPM:  
		-start looking for an anlog comparitor interrupt
		-when the analog comparitor interrupt happens, start counting with the 16 bit timer/counter 1.
		-When the analog comparitor happens again, stop counting and store the result into the array intRPM array
		-Send the data out of the UART or display on an LCD.

	Determine a set time from an analog interrupt (half way through the current RPM:
		-wait for an analog comparitor interrupt
		-in the analog comparitor interrupt, setup timer 1 to compair match at a specific time period (1/2 of the current RPM)
		-when timer 1 overflows, turn on an LED for a short amount of time.

	If we have a 4MHz clock and timer 1 is set to divide by 256, then one timer count is 64uS
	If it takes 20.5mS between triggering the solenoid and the pellet coming out of the barrel, then the
		timer1 count should be 20.5mS / 64uS = 320
	If the power supply is set to 1V, we get an RPM count of 500 to 515
*/

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


	while(1)       // go into an infinite loop here
	{
		wdt_reset();	//reset the watchdog
       	SrvTock();        //go to the tock every 20ms
	}
	return 0;
}


//----------------------------------------------------------------------------
//Routine:     	signal(SIG_TIMER0_OVF)  (timer 0 overflow interrupt vector)
//
//Function:    	This interrupt happens when timer 0 overflows
//
//Varibles passed:
//       none
//Returns:
//       none
//----------------------------------------------------------------------------
SIGNAL(SIG_TIMER0_OVF)
{
	TCNT0=0;	//reset the counter
	if (uTock > 0)
	{
      uTock--;      //This counts down to a longer tock.  (a time reference for longer based timers
	}
}

//----------------------------------------------------------------------------
//Routine:     	SIG_TIMER1_COMPA (signal timer1 overflow interrupt vector)
//
//Function:    	
//Varibles passed:
//       none
//Returns:
//       none
//----------------------------------------------------------------------------
SIGNAL(SIG_TIMER1_COMPA)
{
	//blink the LED
	if(u08Timer1State==0)		//Is the prop at the correct place to turn on the LED to freeze the prop
	{
		TCCR1B &= ~(1<<CS12);	//turn off the timer1 compare match, it will be reloaded to turn off the LED
		toggleLED();
		defSolenoidPort |= (1<<defSolenoidPin);
		u08Timer1State++;		//we don't want to go back in here next time
		TCNT1=0x00;		//reset the counter 1
		OCR1A=defLedOnTime;	//set the timer1 compare match value, this is the time to turn off the LED after it is turned on
		TIMSK |= (1<<OCIE1A);		//turn on the timer1 output compare match
		TCCR1A = 0x00;
		TCCR1B = (1<<ICNC1)|(1<<CS12);	//start the 16 counter, we want to turn off the LED after a short blink
		TCCR1C = 0x00;		
	}
	else	//it must be time to turn off the LED
	{
		TCCR1B &= ~(1<<CS12);	//turn off the timer1 compare match
		toggleLED();
		u08State=1;		//don't go back in here until the button pressed
	}
}

//----------------------------------------------------------------------------
//Routine:     	signal(ANA_COMP_vect) (analog comparitor interrupt)
//
//Function:    	When a blade of the prop passes the infrared sensor, the signal to the analog comparitor
//				goes low for about 1mS.  This routine is fired when the analog comparitor interrupt is 
//				fired.  
//Varibles passed:
//       none
//Returns:
//       none
//----------------------------------------------------------------------------
SIGNAL(ANA_COMP_vect)
{

	if(u08State==0)	//not used anymore
	{
		ACSR=0x00;		//disable the analog comparitor
		defSolenoidPort |= (1<<defSolenoidPin);	//turn on the solenoid
//		boolWaitingForProp=false;	//we are no longer waiting for the analog comparitor
	}
	else if(u08State==1)		//are we counting the RPM of the propeller?
	{

		switch (u08SubState)	//This switch statement is the main state machine
        {		

			case 0:		//has the propeller fired the analog comparitor for the first time in this reading?
				TCCR1A=0x00;
				TCCR1B=(1<<ICNC1)|(1<<CS12);	//start the 16 counter, since a blade of the propeller passed the analog comparitor
				TCCR1C=0x00;
				u08SubState=u08SubState+1;	//we need to read the time between analog comparitor interrupts next time through
			break;

			case 1:		//has the propeller fired the analog comparitor for the second time in this reading?
				intRPM[u08RpmCounter]=TCNT1;	//store the 16 bit counter, this is the speed between interrupts
				u08RpmCounter++;	//next time through, the speed between interrupts is stored in the array location
				//if(u08RpmCounter>2)	//are we taking several readings of the RPM?
				//{
					u08State = 2;
				//}
				TCCR1B=0x00;	//stop the 16 bit counter, we don't need it for a number based upon the RPM
				TCNT1 = 0x00;	//reset the 16 bit counter for next time
				ACSR = 0x00;	//turn off the analog interrupt since we don't need it anymore
			break;
		}
	}
	else if(u08State==2)	//setting a specific time from the blade passing
	{
		ACSR=0x00;		//disable the analog comparitor
		//setup timer 1 to compare match at a specific time period (1/2 of the current RPM)
		TCNT1=0x00;		//reset the counter 1

		//OCR1A=intRPM[0]/defBladePosDivider;	//this is used to turn on an LED to illuminate the propeller


		#ifdef defHitTheBlade	//are we going to try to hit the second blade that goes by after the analog interrupt blade?
			intTemp=(intRPM[0]*2)-intTemp;	//set the timing to hit the second blade, minus the solenoid delay

		#else	//we are going to shoot the pellet between the first and second blade passing
			
			intTemp=intRPM[0]/2;			//First, we are going to find 1.5 times the time beween the blades 
			intTemp=(intRPM[0]*2)-intTemp;	//  passing the IR sensor.
		#endif
		if(intTemp < defTimer1TicksForSolenoid)		//Is the propeller rotating too fast?
		{
			//houston, we have a problem
		}
		intTemp=intTemp - defTimer1TicksForSolenoid;	//subtract the solenoid-to-pellet delay to get time to activate the solenoid
		OCR1A=intTemp;	//load in the time to delay before firing the solenoid


		TIMSK |= (1<<OCIE1A);		//turn on the timer1 output compare match
		TCCR1A = 0x00;
		TCCR1B = (1<<ICNC1)|(1<<CS12);	//start the 16 counter, since a blade of the propeller passed the analog comparitor
		TCCR1C = 0x00;
		u08Timer1State=0;
	}

}

//----------------------------------------------------------------------------
//Routine:      toggleLED  (toggle an LED on or off)
//
//Function:     yeah, it toggles an LED
//
//Varibles passed:
//       none
//Returns:
//       none
//----------------------------------------------------------------------------
void toggleLED()
{
	//blink the LED
	if(u08Toggle==0)	//is it time to turn on the LED?
	{
		u08Toggle=1;	//turn off the LED next time
		defStatusLedPort |= (1<<defStatusLedPin);	//turn on the LED
	}
	else	//it must be time to turn off the LED
	{
		u08Toggle=0;	//turn on the LED next time
		defStatusLedPort &= ~(1<<defStatusLedPin);	//turn off the LED
	}
}

//----------------------------------------------------------------------------
//Routine:      SrvTock  (Service the tock)
//
//Function:     Exicute the tock every 20mS from the main loop.
//				This is where ALL the good stuff happens
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
        uTock = 200;   //reload the Tock (20mS) timer
		wdt_reset();	//reset the watchdog

	    switch (u08State)	//This switch statement is the main state machine
        {		
/*
			case 0:
				if(uTmrState>0)		//we only go in here after the pushbutton has been pressed
				{
					if(boolWaitingForProp==false)	//has the analog comparitor interrupt (prop passing) happened yet?
					{
						uTmrState--;	//then count down to turning the solenoid off
					}
				}
				else
				{
					defSolenoidPort &= ~(1<<defSolenoidPin);	//turn off the solenoid
					if(CHECKBIT(defFireButtonPort,defFireButtonPin)==0)		//Has the user pressed the fire button?
					{
						uTmrState=25;	//don't go back into here for awhile, turn off the solenoid later
						boolWaitingForProp=true;
						//disable any interrupt flags before the analog comparitor has been enabled!
						ACSR = (1<<ACI)|(1<<ACIE)|(1<<ACIS1)|(0<<ACIS0);	//turn on the analog comparitor interrupt
					}				
				}
			break;
*/
			case 1:		//read RPM of the propeller (stores the time between the blades passing the IR sensor)
				if(uTmrState>0)		//we only go in here after the pushbutton has been pressed
				{
					uTmrState--;	//then count down to letting the user push the sample button again
					if(uTmrState==20)	//is it time to release the solenoid?
					{
						defSolenoidPort &= ~(1<<defSolenoidPin);	//release the solenoid, since we are done with it

					}
				}
				else
				{
					if(CHECKBIT(defFireButtonPort,defFireButtonPin)==0)		//Has the user pressed the sample RPM button?
					{
						uTmrState=25;	//don't go back into here for awhile
						u08SubState=0;	//setup the analog interrupt to be in the correct state
						if(u08RpmCounter>3)	//have we reached the end of the RPM storage array?
						{
							u08RpmCounter=0;	//yes, then start over with the array
						}
						//disable any interrupt flags before the analog comparitor has been enabled!
						ACSR = (1<<ACI)|(1<<ACIE)|(1<<ACIS1)|(0<<ACIS0);	//turn on the analog comparitor interrupt					
					}
				}
			break;

			case 2:		//This sets a specific time from analog interrupt using timer1
				if(uTmrState>0)		//we only go in here after the pushbutton has been pressed
				{
					uTmrState--;	//then count down to letting the user push the sample button again
				}
				else
				{
					if(CHECKBIT(defFireButtonPort,defFireButtonPin)==0)		//Has the user pressed the sample RPM button?
					{
						uTmrState=25;	//don't go back into here for awhile
						u08SubState=0;	//setup the analog interrupt to be in the correct state
						if(u08RpmCounter>3)	//have we reached the end of the RPM storage array?
						{
							u08RpmCounter=0;	//yes, then start over with the array
						}
						//disable any interrupt flags before the analog comparitor has been enabled!
						ACSR = (1<<ACI)|(1<<ACIE)|(1<<ACIS1)|(0<<ACIS0);	//turn on the analog comparitor interrupt, we are looking for a blade to pass
					}
				}
			break;
		}
	}
}

//----------------------------------------------------------------------------
//Routine:      uCinit (microcontroller initialize)
//
//Function:     Initialize the microcontroller by setting the port directions, port pullups and setting the
//				timer overflow rate.
//
//Varibles passed:
//       none
//Returns:
//       none
//----------------------------------------------------------------------------
void uCinit(void)
{
	PORTB=0b00011000;	//to enable a pullup resistor on this port, set the bit to "1"
	DDRB=0b11100100;  	//to set the pin as an output, set the bit to "1"
	PORTD=0b00000000;     	//to enable a pullup resistor on this port, set the bit to "1"
	DDRD=0b11111111;   	//to set the pin as an output, set the bit to "1"

	//This sets up an 8 bit 
	TIMSK = (1<<TOIE0);	//Enable a timer overflow interrupt
	TCCR0A = 0;			//just a normal overflow
	TCCR0B = (1<<CS00);	//count with cpu clock/8

	DIDR = (1<<AIN1D)|(1<<AIN0D);		//turn off the digital input pins on the analog comparitor pins
}


