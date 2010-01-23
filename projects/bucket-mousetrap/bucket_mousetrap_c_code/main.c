/*
Author:  Matt Meerian
Date:  January 3, 2007
Purpose:  This software uses 4 infrared emitters and 4 detectors.  When one of the detectors doesn't receive a signal,
	a solenoid will release a bucket and two high intensity LED's will flash signaling the bucket has been released.
Developement System:  AVR Studio, Version 4.12 service pack 2
C compiler:  AVRGCC 3.4.6 with Programers Notepad as the editor
Target Microcontroller:  ATtiny2313 from Atmel (AVR series)
*/

//Set the data types
typedef unsigned char u08;  //A character, 8 bits
typedef unsigned short u16; //a short integer, big endian 16 bits, 0 to 65535

//these are the include files
#include <avr\io.h>
#include <avr\interrupt.h>  //used to allow interrupts
#include <stdbool.h>    //this file allows boolean data types in "C"
#include <avr\pgmspace.h>   //must be included to store arrays in FLASH
#include <avr\wdt.h>        //for the watchdog timer
#include "define.h"		//hold constants for the program

//subroutine definitions
void uCinit(void);			//initialization routine for the ATtiny2313
void SrvTock(void);		//is run every 20mS, it sets up a software time base
void turn_Off_ir(void);	//turn off the 39KHz signal going to the IR LED's
void showBlinkA(void);		//blinks the first LED of where the photo interruption happened
void showBlinkB(void);		//blinks the second LED of where the photo interruption happened
void turnOffVisibleLeds(void);		//turns off the four visible downlooking LED's
void turnOnVisibleLeds(void);		//turns on the four visible LED's

/*global varibles*/
volatile u08 u08TmrTock=0;		//this is used to replace the integer that counted down to a 20mS tock, (the int was being corrupted)
volatile u08 u08TmrIR=defScanningRate;			//this is used to keep track of when the IR is transmitting
volatile u08 u08Ir1Detect=0;	//The timer reads the sensors, this keeps track of the number of times sensor 1 has not received a signal
volatile u08 u08Ir2Detect=0;	//The timer reads the sensors, this keeps track of the number of times sensor 2 has not received a signal
volatile u08 u08Ir3Detect=0;	//The timer reads the sensors, this keeps track of the number of times sensor 3 has not received a signal
volatile u08 u08Ir4Detect=0;	//The timer reads the sensors, this keeps track of the number of times sensor 4 has not received a signal
volatile u08 u08SensorReadNumber=1;		//this keeps track of what sensor is being read
volatile bool boolAlarm=false;			//after the solenoid has dropped, this is set to true
u08 u08State=0;					//This is used in the state machine
u08 u08Summary=0;				//a temporary variable for holding which value sensor was interrupted
u08 u08TmrFlash=0;				//count out the visible LED flashes
u08 u08Blink1;	//holds the number of times the visible LED needs to flash
u08 u08Blink2;	//holds the number of times the visible LED needs to flash
u08 u08Blink3;	//holds the number of times the visible LED needs to flash
u08 u08Blink4;	//holds the number of times the visible LED needs to flash
u16 u16Tmr30Sec=1500;		//count down to 30 seconds after powerup

//----------------------------------------------------------------------------
//Routine:     main (the main loop of the program)
//
//Function:    This is where to program will exicute until an interrupt happens or
//              someone yanks the power plug.
//
//Varibles passed:
//       none
//Returns:
//       interger (no idea where this integer is returned to)
//----------------------------------------------------------------------------
// Main loop
int main( void )
{
	cli();
	uCinit();			//go through the initialization process for the microcontroller
	sei();				//enable interrupts
	while(1)			//go into an infinite loop here
	{
		wdt_reset();      //reset the watchdog, oh ya, kick his ass across the floor
		SrvTock();        //go to the tock every 20ms
	}
   return 0; 
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
	//port initialization
	PORTB = 0b11110111;		//no pullups
	DDRB = 0b10101000;     	//use almost all the pins on PortB for output
	PORTD = 0b00100000;		//almost no pullups
	DDRD = 0b11111111; 		//use almost all the pins for output, except the interrupt pins

	//8 bit timer initialization (timer 0)
	TIMSK = (1<<TOIE0);   	//Enable TCNT0 overflow
	TCNT0 = 0;         		//reset TCNT0
	TCCR0A = 0;				//just a normal overflow
	TCCR0B = (1<<CS00)|(1<<CS01);        	//count with cpu clock/64
	
	//16 bit timer, generates a 39KHz modulated signal for the IR receivers
	TCCR1A = (1<<COM1A0)|(1<<WGM10)|(1<<WGM11);
	TCCR1B = (1<<WGM13)|(1<<WGM12)|(1<<CS10);
	OCR1AH = 0x01;		//setup the timer to output a 39KHz signal
	OCR1AL = 0x08;		

	//UART transmitt enable
    UCSRA = 0x00;
	UCSRB = (1<<TXEN);					//transmitt enable
	UCSRC = (1<<UCSZ0)|(1<<UCSZ1);		//8 data bits
	UBRRH = 0;
	UBRRL = 129;						//9600 baud at 20 MHz, if the clock is 4MHz, this value needs to be 25

	turnOffVisibleLeds();		//turn off all of the visible LED's
	PORTD |= (1<<defSolenoid);	//turn on the solenoid to hold the bucket up
}

//****************************************************************************
//----------------------------------------------------------------------------
//Routine:     ISR (SIG_OVERFLOW0)  (timer 0 overflow interrupt)
//
//Function:    signal handler for tcnt0 overflow interrupt
//              happens every 0.8192 mS, with a 20 MHz clock
//Varibles passed:
//       none
//Returns:
//       none
//----------------------------------------------------------------------------
ISR (SIG_OVERFLOW0)
{	//timer tock
	if(u08TmrTock>0)		//counts down to a 20mS tock
	{				//NOTE:  this was orginally an integer called intTock.  
		u08TmrTock--;	// It was found that the integer was being corrupted, so it was broke into two bytes
	}
	if(boolAlarm==false)		//has the solenoid been released yet?
	{
		if (u08TmrIR==0)	//is it time to start an IR sensor reading?
		{
			u08TmrIR=defScanningRate;		//reload the IR sensor counter
			if(u08SensorReadNumber==4)	//is it time to reset the sensor counter?
			{
				u08SensorReadNumber=1;		
			}
			else
			{
				u08SensorReadNumber++;	//read from the next sensor
			}
			TCCR1B = (1<<WGM13)|(1<<WGM12)|(1<<CS10);		//turn on the 38KHz to the address decoder
			switch(u08SensorReadNumber)	//turn on the IR LED using the address decoder
			{
				case 1:
					PORTD &= ~(1<<def74ls138A0);	//turn on the first infrared LED
					PORTD &= ~(1<<def74ls138A1);
				break;
				case 2:
					PORTD |= (1<<def74ls138A0);	//turn on the second infrared LED
					PORTD &= ~(1<<def74ls138A1);
				break;
				case 3:
					PORTD &= ~(1<<def74ls138A0);	//turn on the third infrared LED
					PORTD |= (1<<def74ls138A1);
				break;
				case 4:
					PORTD |= (1<<def74ls138A0);	//turn on the fourth infrared LED
					PORTD |= (1<<def74ls138A1);
				break;
			}
		}
		else if (u08TmrIR==defScanningRate/2)		//is it time to read the IR sensors?
		{
			switch(u08SensorReadNumber)	//which sensor are we on?
			{
				case 1:		//are we on the first sensor?
					if(bit_is_set(PINB,defIrRx1))	//are we receiving a signal from the IR sensor
					{
						u08Ir1Detect=u08Ir1Detect+1;	//increment the sensor interruption counter
					}
					else
					{
						u08Ir1Detect=0;	//reset the sensor interruption counter, it must have been a false trigger
					}
				break;
				case 2:		//are we on the second sensor?
					if(bit_is_set(PINB,defIrRx2))	//are we receiving a signal from the IR sensor
					{
						u08Ir2Detect=u08Ir2Detect+1;	//increment the sensor interruption counter
					}
					else
					{
						u08Ir2Detect=0;	//reset the sensor interruption counter, it must have been a false trigger
					}
				break;
				case 3:		//are we on the third sensor?
					if(bit_is_set(PINB,defIrRx3))	//are we receiving a signal from the IR sensor
					{
						u08Ir3Detect=u08Ir3Detect+1;	//increment the sensor interruption counter
					}
					else
					{
						u08Ir3Detect=0;	//reset the sensor interruption counter, it must have been a false trigger
					}
				break;
				case 4:		//are we on the fourth sensor?
					//PORTD &= ~(1<<defBriteLED2);	//off
					if(bit_is_set(PINB,defIrRx4))	//are we receiving a signal from the IR sensor
					{
						u08Ir4Detect=u08Ir4Detect+1;	//increment the sensor interruption counter
						//PORTD &= ~(1<<defBriteLED1);	//off
					}
					else
					{
						u08Ir4Detect=0;	//reset the sensor interruption counter, it must have been a false trigger
					}
				break;
			}
			u08TmrIR--;			//go on in the timer countdown
			turn_Off_ir();		//since we are done using the IR, turn it off
		}
		else
		{
			u08TmrIR--;		//keep counting down to the next sensor reading
		}
	}
}

//----------------------------------------------------------------------------
//Routine:       turn_off_ir (turn of the infrared LEDs)
//
//Function:      This routine turns off timer 1, which is sending out the 39KHz
//				 signal to the IR LED's
//
//Varibles passed:
//       none
//Returns:
//       none
//----------------------------------------------------------------------------
void turn_Off_ir()
{
	TCCR1B = 0;
}

//****************************************************************************
//----------------------------------------------------------------------------
//Routine:       SrvTock  (Service the tock)
//
//Function:      Exicute the tock every 20mS from the main loop.
//
//Varibles passed:
//       none
//Returns:
//       none
//----------------------------------------------------------------------------
void SrvTock(void)
{
	if (u08TmrTock==0 )		//has a 20mS tock happened?
	{
		u08TmrTock=25;	//reload the 20mS tock timer

		switch (u08State)	//controls the very small state machine
		{
			case 0:		//first 30 seconds after powerup, show any  interruption in the sensors by flashing LED's
				if(u08Ir1Detect>0)		//has sensor 1 had an interruption?
				{
					u08Summary = u08Summary | 0x01;		//yes, then set the sensor 1 flag
					u08Ir1Detect=0;						//reset the sensor 1 interruption counter
					u08Blink1=defNumBlinks;				//allow the visible LED's to show the interruption
				}
				if(u08Ir2Detect>0)		//has sensor 2 had an interruption?
				{
					u08Summary = u08Summary | 0x02;		//yes, then set the sensor 2 flag
					u08Ir2Detect=0;						//reset the sensor 2 interruption counter
					u08Blink2=defNumBlinks;				//allow the visible LED's to show the interruption
				}
				if(u08Ir3Detect>0)		//has sensor 3 had an interruption?
				{
					u08Summary = u08Summary | 0x04;		//yes, then set the sensor 3 flag
					u08Ir3Detect=0;						//reset the sensor 3 interruption counter
					u08Blink3=defNumBlinks;				//allow the visible LED's to show the interruption
				}
				if(u08Ir4Detect>0)		//has sensor 4 had an interruption?
				{
					u08Summary = u08Summary | 0x08;		//yes, then set the sensor 4 flag
					u08Ir4Detect=0;						//reset the sensor 4 interruption counter
					u08Blink4=defNumBlinks;				//allow the visible LED's to show the interruption
				}
				UDR=u08Summary;		//send the sensor status out of the UART
				if(u08Summary >0 && u16Tmr30Sec==0)	//has the sensors been interrupted?
				{
					PORTD &= ~(1<<defSolenoid);	//yes, then release the solenoid!
					boolAlarm=true;				//set the flag to show the solenoid has been released
				}
				
				if(u16Tmr30Sec==150 || u16Tmr30Sec==100 || u16Tmr30Sec==50)
				{
					turnOnVisibleLeds();	//blink all of the LED's before we enable solenoid
				}
				if(u16Tmr30Sec==110 || u16Tmr30Sec==60 || u16Tmr30Sec==10)
				{
					turnOffVisibleLeds();	//blink all of the LED's before we enable solenoid
				}
				if(u16Tmr30Sec!=0)		//is it time to enable the solenoid?
				{
					u16Tmr30Sec--;		//count down to enableing the solenoid
					u08Summary=0;
				}
				
				u08TmrFlash++;				//count out the blinking of the high intensity LED's
				switch (u08TmrFlash)		//this shows two blinks on one LED and then two flashes on the other LED
				{
					case 1:
						showBlinkA();			//turn on the blinking LED's
					break;
					case 3:
						turnOffVisibleLeds();	//turn off the blinking LED's
					break;
					case 5:
						showBlinkA();			//turn on the blinking LED's
					break;
					case 7:
						turnOffVisibleLeds();	//turn off the blinking LED's
					break;
					case 17:
						showBlinkB();			//turn on the blinking LED's
					break;
					case 19:
						turnOffVisibleLeds();	//turn off the blinking LED's
					break;
					case 21:
						showBlinkB();			//turn on the blinking LED's
					break;
					case 23:
						turnOffVisibleLeds();	//turn off the blinking LED's
					break;
					case 33:
						u08TmrFlash=0;			//reset the flash LED counter back to zero
					break;
				}
				//}
			break;
		}
	}
}

//----------------------------------------------------------------------------
//Routine:       showBlinkA 
//
//Function:      This routine does the first blink for the visible LED
//
//Varibles passed:
//       none
//Returns:
//       none
//----------------------------------------------------------------------------
void showBlinkA(void)
{
	if(u08Blink1 != 0)					//are we still blinking this LED?
	{
		PORTB |= (1<<defBriteLED3);	//turn on the LED for the blinking!
	}
	if(u08Blink2 != 0)					//are we still blinking this LED?
	{	
		PORTB |= (1<<defBriteLED4);	//turn on the LED for the blinking!
	}
	if(u08Blink3 != 0)					//are we still blinking this LED?
	{
		PORTB |= (1<<defBriteLED4);	//turn on the LED for the blinking!
	}
	if(u08Blink4 != 0)					//are we still blinking this LED?
	{
		PORTD |= (1<<defBriteLED2);	//turn on the LED for the blinking!
	}

}

//----------------------------------------------------------------------------
//Routine:       showBlinkB
//
//Function:      This routine does the second blink for the visible LEDs
//
//Varibles passed:
//       none
//Returns:
//       none
//----------------------------------------------------------------------------
void showBlinkB(void)
{
	if(u08Blink1 != 0)					//are we still blinking this LED?
	{
		PORTD |= (1<<defBriteLED1);	//turn on the LED for the blinking!
		if(boolAlarm==false)			//only count down when the solenoid has not been released
		{
			u08Blink1--;				//count down to the next blink for this LED
		}
	}
	if(u08Blink2 != 0)					//are we still blinking this LED?
	{	
		PORTB |= (1<<defBriteLED3);	//turn on the LED for the blinking!
		if(boolAlarm==false)			//only count down when the solenoid has not been released
		{
			u08Blink2--;				//count down to the next blink for this LED
		}
	}
	if(u08Blink3 != 0)					//are we still blinking this LED?
	{
		PORTD |= (1<<defBriteLED2);	//turn on the LED for the blinking!
		if(boolAlarm==false)			//only count down when the solenoid has not been released
		{
			u08Blink3--;				//count down to the next blink for this LED
		}
	}
	if(u08Blink4 != 0)					//are we still blinking this LED?
	{
		PORTD |= (1<<defBriteLED1);	//turn on the LED for the blinking!
		if(boolAlarm==false)			//only count down when the solenoid has not been released
		{
			u08Blink4--;				//count down to the next blink for this LED
		}
	}
}

//----------------------------------------------------------------------------
//Routine:       turnOffVisibleLeds 
//
//Function:      There are four visible LEDs that point down, two red and two blue.
//				 This routine turns them all off
//Varibles passed:
//       none
//Returns:
//       none
//----------------------------------------------------------------------------
void turnOffVisibleLeds(void)
{
	PORTD &= ~(1<<defBriteLED1);	//turn off visible LED 1 on powerup
	PORTD &= ~(1<<defBriteLED2);	//turn off visible LED 2 on powerup
	PORTB &= ~(1<<defBriteLED3);	//turn off visible LED 3 on powerup
	PORTB &= ~(1<<defBriteLED4);	//turn off visible LED 4 on powerup
}

//----------------------------------------------------------------------------
//Routine:       turnOnVisibleLeds 
//
//Function:      There are four visible LEDs that point down, two red and two blue.
//				 This routine turns them all on
//Varibles passed:
//       none
//Returns:
//       none
//----------------------------------------------------------------------------
void turnOnVisibleLeds(void)
{
	PORTD |= (1<<defBriteLED1);	//turn off visible LED 1 on powerup
	PORTD |= (1<<defBriteLED2);	//turn off visible LED 2 on powerup
	PORTB |= (1<<defBriteLED3);	//turn off visible LED 3 on powerup
	PORTB |= (1<<defBriteLED4);	//turn off visible LED 4 on powerup
}
