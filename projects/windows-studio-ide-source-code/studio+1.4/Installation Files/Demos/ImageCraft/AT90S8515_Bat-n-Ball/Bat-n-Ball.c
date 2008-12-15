// Bat-n-Ball.c
// Chris Brown 6th Feb, 2002
//
// Flash Studio+ & In-System-Debugger (ISD) "Bat-n-Ball" demo.
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
#define AT90S8515
//#define ATMEGA103

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

#define SIZEX 15						// Size of the court
#define SIZEY 6

char memory[(SIZEY+4)*(SIZEX+1)] = {"Bat-n-Ball Demo     00 : 00     "};
unsigned char leftScore, rightScore;


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


void DrawScores(void)
{
	memory[16+4] = (leftScore/10)+'0';
	memory[16+5] = (leftScore-((leftScore/10)*10))+'0';
	memory[16+9] = (rightScore/10)+'0';
	memory[16+10] = (rightScore-((rightScore/10)*10))+'0';
}


void main(void)
{
	unsigned char x, y, oldX, oldY;
	char dx, dy;
	unsigned char leftPos, rightPos;

//	MCUCR |= 0x80;						// Enable external sram (optional)

	INT0_Init();
	UART0_Init();
#ifdef AT90S8515
	GIMSK |= 0x40;						// Enable INT0 (AT90S8515)
#endif
#ifdef ATMEGA103
	EIMSK |= 0x01;						// Enable INT0 (ATmega103)
#endif
	SEI();								// Enable interrupts

	for (y=0; y<SIZEY+2; y++)			// Initialise the court
	{
		for (x=0; x<SIZEX+1; x++)
		{
			if (x == 7)
				memory[((y+2)<<4)+7] = '|';
			else
				memory[((y+2)<<4)+x] = ' ';
		}
	}
	for (x=0; x<15; x++)
	{
		memory[32+x] = '-';
		memory[48+(SIZEY*16)+x] = '-';
	}

	leftScore = 10;						// Start new game
	rightScore = 10;
	leftPos = 3;
	rightPos = 3;

	for (;;)							// Loop forever!!
	{

		SetBreakHere();					// Hard Breakpoint here

		if (leftScore == 10 || rightScore == 10) // If game is over
		{
			leftScore = 0;				// Reset scores
			rightScore = 0;
			memory[(3+leftPos)<<4] = ' '; // Erase the old bats
			memory[((3+rightPos)<<4)+(SIZEX-1)] = ' ';
			leftPos = 3;
			rightPos = 3;
			memory[(3+3)<<4] = '|';		// Draw the bats
			memory[((3+3)<<4)+(SIZEX-1)] = '|';
			x = 7;						// Ball's starting position
			y = 3;
			oldX = x;
			oldY = y;
			MakeLedGreen();
			dx = 1;						// Ball is going right & down
			dy = 1;
		}

		DrawScores();					// Draw the scores

		if (oldX == 7)
			memory[((oldY+3)<<4)+7] = '|';
		else
			memory[((oldY+3)<<4)+oldX] = ' ';
		x += dx;						// Update ball's position
		y += dy;
		memory[((3+y)<<4)+x] = 'o';

		if (dx == -1)					// If ball is moving left
		{
			memory[(3+leftPos)<<4] = ' '; // Erase left's bat
			if (y > leftPos)			// Move bat towards the ball
				leftPos++;
			if (y < leftPos)
				leftPos--;
			memory[(3+leftPos)<<4] = '|'; // Draw left's bat
		}

		if (dx == 1)					// If ball is moving right
		{
			memory[((3+rightPos)<<4)+(SIZEX-1)] = ' '; // Erase right's bat
			if (y > rightPos)			// Move bat towards the ball
				rightPos++;
			if (y < rightPos)
				rightPos--;
			memory[((3+rightPos)<<4)+(SIZEX-1)] = '|'; // Draw right's bat
		}

		if (x == 1 && leftPos == y)		// If left is hitting the ball
		{
			dx = 1;						// Hit it back
			MakeLedGreen();
		}

		if (x == SIZEX-2 && rightPos == y) // If right is hitting the ball
		{
			dx = -1;					// Hit it back
			MakeLedRed();
		}

		if (x == 0)						// If left missed
		{
			rightScore++;				// Add 1 to right's score
			memory[((3+y)<<4)+x] = ' ';
			x = 1;						// Left serves
			y = leftPos;
			dx = 1;
			dy = 1;
		}

		if (x == SIZEX-1)				// If right missed
		{
			leftScore++;				// Add 1 to left's score
			memory[((3+y)<<4)+x] = ' ';
			x = SIZEX-2;				// Right serves
			y = rightPos;
			dx = -1;
			dy = 1;
		}

		if (y == 0 || y == (SIZEY-1))	// If ball is at the top or bottom edge
			dy =- dy;					// Bounce

		oldX = x;						// Save ball's position
		oldY = y;
	}
}
