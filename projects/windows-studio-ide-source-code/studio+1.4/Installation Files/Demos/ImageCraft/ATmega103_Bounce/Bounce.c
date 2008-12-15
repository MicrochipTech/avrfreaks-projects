// Bounce.c
//
// Flash Studio+ & In-System-Debugger (ISD) "Bounce" demo.
//
// Use with any ATMEL AVR CPU that has internal SRAM and an RS232
// port or with Flash 5in1 AVR Starter Kits for added features.
//
// Requires a version 1.6 (or later) ISD library or source file:
//    libISD16ATx.a or ISD16_ATx.s

// ****** USER SETTINGS ******

// Un-comment the CPU you are using.
// Remember to set the CPU & ISD library (if you are using one)
// in your ImageCraft project settings too!
//#define AT90S8515
#define ATMEGA103

// Clock frequency - set this to your clock frequency
#define FOSC 3686400					// 3.6864Mhz

// Serial comms Baud rate - set this to your desired baud rate
#define BAUD 115200

// ***** END USER SETTINGS *****

#include <stdio.h>
#include <ina90.h>
#ifdef AT90S8515
#include <io8515.h>
#endif
#ifdef ATMEGA103
#include <iom103.h>
#endif

#define SIZEX 16						// Size of bounce area
#define SIZEY 8

char memory[SIZEY+2][SIZEX] = {"   Flash Demo   ","   AVR Bounce   "};
char message[SIZEY][SIZEX] = {"                ","  THE WORLD'S   ",
	"   SMALLEST,    ","   FASTEST,     ","    NO-ICE,     ",
	"   IN-SYSTEM    ","   DEBUGGER!    ","                "};


void UART0_Init(void)
{
	UCR  = 0x00;						// Disable while setting baud rate
	UBRR = (FOSC/(16*BAUD))-1;			// Set baud rate
	UCR  = 0x18;						// Enable Tx & Rx
}


void INT0_Init(void)
{
#ifdef AT90S8515
	DDRD |= 0x08;						// Set up PD3 (AVRDIL LED) = Output
	PORTD |= 0x04;						// Turn on Pull-Up PD2 (INT0) = 1
#endif
#ifdef ATMEGA103
	PORTD |= 0x01;						// Turn on Pull-Up PD0 (INT0) = 1
#endif
}


void MakeLedRed(void)
{
	DDRD &= 0xf7;						// Turn on red LED
}


void MakeLedGreen(void)
{
	DDRD |= 0x08;						// Turn off red LED
}


void main(void)
{
	unsigned char x, y, oldX, oldY;
	char dx, dy;

	MCUCR |= 0x80;						// Enable external sram (optional)

	INT0_Init();
	UART0_Init();
#ifdef AT90S8515
	GIMSK |= 0x40;						// Enable INT0 (AT90S8515)
#endif
#ifdef ATMEGA103
	EIMSK |= 0x01;						// Enable INT0 (ATmega103)
#endif
	SEI();								// Enable interrupts

	for (y=0; y<SIZEY; y++)				// Initialise grid
	{
		for (x=0; x<SIZEX; x++)
		{
			memory[y+2][x] = message[y][x];
		}
	}

	MakeLedRed();
	x = 0;								// Starting position
	y = 3;
	memory[y+2][x] = 'O';
	oldX = x;
	oldY = y;
	dx = 1;								// Right & down
	dy = 1;

	for (;;)							// Loop forever!!
	{
		memory[2+oldY][oldX] = message[oldY][oldX];
		x += dx;						// Update ball's position
		y += dy;
		memory[2+y][x] = 'O';

		if (x == 0 || x == (SIZEX-1))	// If ball is at the left or right edge
		{
			dx =- dx;					// Change direction
			MakeLedGreen();
		}

		SetBreakHere();					// Hard Breakpoint here

		if (y==0 || y== (SIZEY-1))		// If ball is at the top or bottom edge
		{
			dy =- dy;					// Change direction
			MakeLedRed();
		}

		oldX = x;						// Save ball's position
		oldY = y;
	}
}
