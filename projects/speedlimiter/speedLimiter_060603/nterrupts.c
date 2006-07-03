/*------------------------------------------------------------------------
|
|	Project:		speedLimiter
|					interrupts.c
|
|	Author:			Rasmus Renzi,	rasmus.r@spray.se
|
|
|
--------------------------------------------------------------------------
|
|	Description:	Speedlimiter for engines with mechanical ignition
|					systems.
--------------------------------------------------------------------------*/
#define INTERRUPTS_C


// Include files
#include <avr/io.h>
#include <avr/interrupt.h>
#include <inttypes.h>
#include "speedLimiter.h"
#include "interrupts.h"


/*  ISR for Timer0 *************************************************************
 *  Generates a tic every 50µs for clock value = 1 MHz 			   		   	   */
ISR (TIMER0_COMPA_vect)    // Timer0 interrupt routine *************************
{
	us50Cnt++;
	//imAlive();
}

/*  ISR for PCINT0 *************************************************************
 *  Generates an interrupt for every ignition pulse 					   	   */
ISR (PCINT1_vect) 		   // PCINT0 interrupt routine *************************
{

	portTmp = PINC & 0x20;														// Mask PC5
	portTmp = portTmp >> 5;


	if((portTmp == 1)&&(cutNow == 0))
	{
		PORTC = PORTC & 0x6F;													// Clear PC4
	}
	else
	{
		PORTC = PORTC | 0x10;													// Set PC4	
	}

																				// Interrupt is generated on positive and negative edge
	pulseCount++;																// This will cause two interrupts / ignition
	if(pulseCount > 9)															// Measure time between every 5th ignition
	{																			// (Between every 10th interrupt)
		pulseCount = 0;
		tmpIgnTime = us50Cnt;													// Sample the current time count
		ignPeriodTime = tmpIgnTime - prevIgnTime;
		prevIgnTime = tmpIgnTime;												// Store the current time sample as "previous". To be used in next calculation

		if(ignPeriodTime < currentLimitTime)									// Check current speed against limit
		{
			cutNow = 1;															// Cut ignition

//			if(ignPeriodTime < (prevIgnPeriodTime/2))							// False pulse?
//			{
//				cutNow = 0;
//				prevIgnPeriodTime = ignPeriodTime*2;														
//			}
//			else
//			{
//				cutNow = 1;														// Cut ignition
//				prevIgnPeriodTime = ignPeriodTime;
//			}

		}
		else
		{
			cutNow = 0;															// Enable ignition output
//			prevIgnPeriodTime = ignPeriodTime;
		}
	}
}

#undef INTERRUPTS_C

