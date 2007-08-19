// Demonstrator.c Joystick version
#include <stdlib.h>
#include <stdio.h>
#include "Osccal.h"
#include "PC_Comm.h"
#include "Demonstrator.h"
#include "LCD_driver.h"
#include "LCD_functions.h"
#include "LCD_test.h"
#include "Messages.h"

// Start-up delay before scrolling a string over the LCD. "LCD_driver.c"
extern char gLCD_Start_Scroll_Timer;


// declare global variables
volatile char KEY = 0;
volatile char KEY_VALID = 0;
volatile char ENABLED = 1; // start enabled
// identify yourself specifically	

const char LCD_START_msg[] PROGMEM = "FREQ-UP-DO-DUTY-L-R\0";

void initializer()
{
//	char LCDstring[20];
	// Calibrate the oscillator:
    OSCCAL_Calibrate();   

	// Initialize the USART
	USARTinit();

	// initialize the LCD
    LCD_Init();                 
	

    // Init port pins B
    PORTB |= PINB_MASK;
	PORTB |= (1<<0); // Set PB0 pull high for input
	DDRB |= (1<<5); // set PB5 as out 

    // Init port pins D
	DDRD = 0xFF; // set PORTD for output

    // Init port pins E
    DDRE = 0x00;	// All inputs
    PORTE |= PINE_MASK;

    // Enable pin change interrupt on PORTB and PORTE
	PCMSK0 = PINE_MASK;
	PCMSK1 = PINB_MASK;
	PCMSK1 |= (1<<0);	// PB0
	EIFR = (1<<6)|(1<<7); 
	EIMSK = (1<<6)|(1<<7);
	period = 30000;
	pulswith = period/2;	// start with 50% duty cycle


	setOCR1A(period, pulswith,1,KEY_PUSH);

	LCD_puts_f(LCD_START_msg, 1);


		// Mode 14 Fast PWM, CLK/1 prescaler
	TCCR1A = (1<<COM1A1)|(1<<COM1A0)|(1<<WGM11)|(0<<WGM10);
	TCCR1B = (1<<WGM13)|(1<<WGM12)|(0<<CS12)|(0<<CS11)|(1<<CS10);


	
   	PORTD = 0XFF; // set LEDs off
	
	// say hello
	sendString("\rPC_Comm.c ready to communicate.\r");   	 
	// identify yourself specifically
	sendString("You are talking to the JoyStick demo.\r");

}


void setOCR1A(unsigned int period,unsigned int pulswith,int LCDinfo, char k)
{ 
	char LCDstring[20];
	
	if(period > 63000)
		period = 62500;
	if(pulswith > 62500)
		pulswith = 62300;
	if(pulswith > period)
		pulswith = period-10;
	if(period < 2)
	{
		period=2;
		pulswith = 1;
	}

	LCD_Clear();

	switch(k)
	{
		case KEY_UP:
		case KEY_DOWN:	
			if(LCDinfo)
				sprintf(LCDstring,"FREQEN");
			else
				sprintf(LCDstring,"%6u",period);

			LCD_puts(LCDstring,0);
			LCD_UpdateRequired(1,0);
			break;
		case KEY_LEFT:
		case KEY_RIGHT:
			if(LCDinfo)
				sprintf(LCDstring,"DUTY");
			else
				sprintf(LCDstring,"%6u",pulswith);

			LCD_puts(LCDstring,0);
			LCD_UpdateRequired(1,0);
			break;
		case KEY_PUSH:
			if(LCDinfo)
				sprintf(LCDstring,"Start");
				LCD_puts(LCDstring,0);
				LCD_UpdateRequired(1,0);
			break;
	}

	// Sets the compare value
    ICR1 = period;
    OCR1A = pulswith;
}

	


void parseInput(char s[])
{
	// parse first character	
	switch (s[0])
	{

		case 'j':
			if( (s[1] == 'o') && (s[2] == 'y'))
			joystick();
			break;
		case 'd':
			if( (s[1] == 'e') && (s[2] == 'm') && (s[3] == 'o') && (s[4] == '?') )
			sendString("You are talking to the LightMeter demo.\r");
			break;
		default:
			sendString("\rYou sent: '");
			sendChar(s[0]);
			sendString("' - I don't understand.\r");
			break;
		
	}
	s[0] = '\0';
}

void joystick()
{
	if(ENABLED == 0) ENABLED = 1;
	else ENABLED = 0;
}



SIGNAL(SIG_PIN_CHANGE0)
{
    PinChangeInterrupt();
}

SIGNAL(SIG_PIN_CHANGE1)
{
	if(PINB0)
	{
		TCNT1H=0;		// reset TCNT1H/L
		TCNT1L=0;		// after trigger of PB0
	    EIFR = (1<<PCIF1) | (1<<PCIF0);     // Delete pin change interrupt flags 
	}
	else
   	 PinChangeInterrupt();    
}

void PinChangeInterrupt(void)
{
    char buttons;
    static char key;

    buttons = (~PINB) & PINB_MASK;
    buttons |= (~PINE) & PINE_MASK;

    // Output virtual keys
    if (buttons & (1<<BUTTON_A))
        key = KEY_UP;
    else if (buttons & (1<<BUTTON_B))
        key = KEY_DOWN;
    else if (buttons & (1<<BUTTON_C))
        key = KEY_LEFT;
    else if (buttons & (1<<BUTTON_D))
        key = KEY_RIGHT;
    else if (buttons & (1<<BUTTON_O))
        key = KEY_PUSH;
    else
        key = KEY_INVALID;
  
    if(key != KEY_INVALID)
    {
        if (!KEY_VALID)
        {
            KEY = key;          // Store key in global key buffer
            KEY_VALID = TRUE;
        }
    }


    EIFR = (1<<PCIF1) | (1<<PCIF0);     // Delete pin change interrupt flags 
	
	if(ENABLED)
	{
		getkey();
	}
}



char getkey(void)
{
    char k;
	static unsigned int deltaPeriod = 2;
	static unsigned int deltaPulswith = 1;
	static unsigned int period=30000;
	static unsigned int pulswith=15000;
	static char lastkey;
	int LCDinfo;

    cli(); // disable interrrupts so 'KEY' won't change while in use

    if (KEY_VALID) // Check for unread key in buffer
    {
        k = KEY;
        KEY_VALID = FALSE;
    }
    else
        k = KEY_INVALID; // No key stroke available

    sei(); // enable interrupts

	if(k != KEY_INVALID)
	{
		sendString("The joystick position is: ");

		if(period < 1)
			period = 1;
		if(pulswith < 0)
			pulswith = 0;
		if(pulswith > period)
			pulswith = period -1;
		
		switch(k)
		{
			case KEY_UP:
				sendString("UP");
				if(lastkey == k)
				{
					deltaPeriod *= 2;
					deltaPulswith *= 2;
					LCDinfo=0;
				}
				else
				{
					deltaPeriod =2;
					deltaPulswith =1;
					LCDinfo=1;

				}
				period -= deltaPeriod;
				pulswith -= deltaPulswith;
				setOCR1A(period, pulswith,LCDinfo,k);
				break;
			case KEY_DOWN:
				sendString("DOWN");
				if(lastkey == k)
				{
					deltaPeriod *= 2;
					deltaPulswith *= 2;
					LCDinfo=0;

				}
				else
				{
					deltaPeriod =2;
					deltaPulswith =1;
					LCDinfo=1;
				}
				period += deltaPeriod;
				pulswith += deltaPulswith;
				setOCR1A(period, pulswith,LCDinfo,k);
				break;
			case KEY_LEFT:
				sendString("LEFT");
				if(lastkey == k)
				{
					deltaPulswith *= 2;
					LCDinfo=0;
				}
				else
				{
					deltaPulswith =1;
					LCDinfo=1;
				}
				pulswith -= deltaPulswith;
				setOCR1A(period, pulswith,LCDinfo,k);

				break;
			case KEY_RIGHT:
				sendString("RIGHT");
				if(lastkey == k)
				{
					deltaPulswith *= 2;
					LCDinfo=0;
				}
				else
				{
					deltaPulswith =1;
					LCDinfo=1;
				}
				pulswith += deltaPulswith;
				setOCR1A(period, pulswith,LCDinfo,k);

				break;
			case KEY_PUSH:
				sendString("PUSH");
				period = 30000;
				pulswith = period/2;
				LCDinfo=1;
				setOCR1A(period, pulswith,LCDinfo,k);
				break;
			default:
				sendString("?");
				break;
		}
		
		if( deltaPulswith >  1000)
			deltaPulswith =  1000;
		if( deltaPulswith <  0)
			deltaPulswith =  1;

		if( deltaPeriod >  2000)
			deltaPeriod =  2000;
		if( deltaPeriod < 0)
			deltaPeriod = 2;
		
		if((period < 4000) | (period > 55000))
			if(deltaPeriod > 200)
				deltaPeriod = 200;
		if((pulswith < period*0.1) | (pulswith > period*0.9))
			if(deltaPulswith > period*.05)
				deltaPeriod = period*.05;

		sendChar('\r');
		lastkey = k;
	}	

    return k;
}
