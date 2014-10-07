/*
 * pc_int.c
 *
 * Created: 7.4.2014 18:50:26
 *  Author: PjV
 *
 * Version: 1.0  
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include "macros.h"

/**************************************************************//**
 * Initialize Pin Change Interrupt PORTA:6, by pushbutton 
 *****************************************************************/
void PCINT_Init(void){
	
	/* PORTA:6 - Pin31 - Pushbutton, Input Pulled Up */
	
	/* Set the bits of the appropriate PCMSKx register high 
	   to enable pin change detection on that pin */
	PCMSK0 |= (1<<PCINT6);	//PA6, pushbutton, pin 31(INT6) enabled
	
	/* Then enable the interrupt for the corresponding vector, 
	   (and of course the global interrupt flag) */
	PCICR |= (1<<PCIE0);	// Any change on any enabled PCINT:0 to PCINT:7 pin will cause an interrupt.
							// Interrupt pins are enabled individually by the PCMSK0 Register (above).
							// The corresponding interrupt of Pin Change Interrupt Request is executed from the PCI0 Interrupt
							// Vector (ISR below).
}

/**************************************************************//**
* Pin Change Interrupt by Pushbutton PORTA:6, wakes up from idle. 
* Start/Stop tracking.
* Turn the green led ON until the pushbutton is released.
******************************************************************/
ISR(PCINT0_vect)
{
	unsigned char pin_value;
	
	pin_value = PINA;
	pin_value &= 0b01000000;//clear all others than 6th, button pin
	
	if (pin_value == 0)
	{
		/* HIGH to LOW pin change */
		GREEN_LED_ON;
	} 
	else
	{
		/* LOW to HIGH pin change */
		GREEN_LED_OFF;
	}
}
