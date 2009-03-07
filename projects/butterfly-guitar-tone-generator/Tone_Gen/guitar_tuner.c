// ***********************************************************************
// Program:	Guitar Tone Generator
// Author:	David Wilson - wilson479@hotmail.com
// Date:	07/03/09
// ***********************************************************************//

#include <avr/io.h>
#include <avr/delay.h>
#include "LCD_Driver.h"	// "Dean's LCD_Driver - www.AVRfreaks.com"


int main(void)
{
	DDRE &= ~(1<<PE2);	// Joystick left
	DDRE &= ~(1<<PE3);	// Joystick right
	PORTE = 0xFF;		// Enable pullup resistor on inputs

	DDRB |=(1<<PB5);	// Piezo out

	OCR1A = 0x0009;		// Volume

	ICR1 = 1516.852228;	// Pitch (for example only)

	TCCR1B |= ((1<<CS10)|(1<<WGM13));	// No prescale, phase correct PWM	

	TCCR1A |= ((1<<WGM11)|(1<<COM1A1));	// Phase correct PWM, clear OCR1A at top 

	int menuSel = 0;	//Menu position counter initially off

	LCD_Init();		// Initiate LCD
	sei();			// Enable global interrupts (needed for LCD operation)

	while(1)		// Main loop
	{	
	if(~PINE & (1<<PE2))				// Joystick left press?
	{
		_delay_ms(120);					// 120mS debounce - very needed!!!
		if(~PINE & (1<<PE2))			// Joystick still pressed?
		{
			if(menuSel>0){menuSel--;}	// Scroll menu left untill bottom
		}
	}

	if(~PINE & (1<<PE3))				// Joystick right pressed?
	{
		_delay_ms(120);					// 120mS debounce - IMPORTANT!!
		if(~PINE & (1<<PE3))			// Still pressed?
		{
			if(menuSel<=5){menuSel++;}	// Scroll menu right untill top
		}
	}

		switch(menuSel)		// Menu functions
		{
		case 0:				// Off position
		{
		LCD_puts("Off");
		ICR1 = 0;
		}
		break;

		case 1:				// Lowest guitar string tone
		{
		LCD_puts("Low E");	
		ICR1 = 6067.224851;	// 82.41Hz
		}
		break;

		case 2:				// Guitar A string
		{
		LCD_puts("A");
		ICR1 = 4545.454545;	// 110Hz
		}
		break;

		case 3:				// Guitar D string
		{
		LCD_puts("D");
		ICR1 = 3405.298645;	// 146.83Hz
		}
		break;

		case 4:				// Guitar G string
		{
		LCD_puts("G");
		ICR1 = 2551.020408;	//196Hz
		}
		break;

		case 5:				// Guitar B string
		{
		LCD_puts("B");
		ICR1 = 2024.783348;	// 246.94Hz
		}
		break;

		case 6:				// Guitar high E string
		{
		LCD_puts("High E");
		ICR1 = 1516.852228;	// 329.63Hz
		}
		break;
		}
	}
}
