//goal to control a single 74HC595 as a port expander on PB0, PB1 and PB2

//Attempting to code in C and fit on Tiny13

//Assuming MCU clock of 128kHz (Int RC fuse setting)

#include <avr/io.h>		// include I/O definitions (port names, pin names, etc)
#include <avr/interrupt.h>	// include interrupt support
//#include <util/delay.h>

#include "global.h"	

const int SCK = 0;
const int RCK = 3;
const int SER = 4;
	
const char output = 0b11110000;
							// Frame
char seq[16]= {	0b10000000,		//1
				0b11000000,		//2
				0b11100000,		//3
				0b11110000,		//4
				0b11111000,		//5
				0b11111100,		//6
				0b11111110,		//7
				0b11111111,		//8
				0b01111111,		//9
				0b00111111,		//10
				0b00011111,		//11
				0b00001111,		//12
				0b00000111,		//13
				0b00000011,		//14
				0b00000001,		//15
				0b00000000,		//16
				};

void delay();

void delay()
{
	long i =0;
	for(i=0;i<1;i++)
	{
	__asm__ volatile ("nop" ::);		//so delay routine not optimised away
	}
}


// ***************************************************************************
// Func: write_value
// Purpose is to take a char and set up an 74HC595 with the values contained
// in the char.
// Input:  char input - the byte to output via the 74HC595
// Input:  int SCK - the number of the pin on PORTB that is connected to SCK
// Input:  int RCK - the number of the pin on PORTB that is connected to RCK
// Input:  int SER - the number of the pin on PORTB that is connected to SER
// ***************************************************************************
int write_value (char input, int SCK, int RCK, int SER)
{
	//Init
		//Set port B to output
	DDRB = 0xFF;
	PORTB = 0x00;

	PORTB &= ~(1<<RCK);

	PORTB |= (1<<SER);
	PORTB &= ~(1<<SCK);
	delay();
	PORTB |= (1<<SCK);
	delay();
	PORTB &= ~(1<<SCK);
	PORTB &= ~(1<<SER);
	//PORTB |= (1<<RCK);
	
	delay();


//+++++++++++++++++++++
	int counter = 0;
	for(counter = 0; counter <= 7; counter++)
	{

	//clock low
	PORTB &= ~(1<<RCK);
	PORTB &= ~(1<<SCK);
	PORTB &= ~(1<<SER);
	
	delay();


		if( ((1<<counter)&input) != 0 )
		{	//1
			PORTB |= (1<<SER);		//1	
		}
		else
		{	//0
			PORTB &= ~(1<<SER);	 	//0
		}


	PORTB |= (1<<SCK);
	PORTB |= (1<<RCK);


	delay();
	}
		
	

	return 0;
}


int main()
{

	//Set port B to output
	DDRB = 0xFF;
	PORTB = 0x00;
	
	write_value(output, SCK, RCK, SER);

	
	while(1)
	{
		int j =0;
		for(j=0;j<=15;j++)
		{
			//write_value((0b00000001<<j), SCK, RCK, SER);
			write_value(seq[j], SCK, RCK, SER);
			
			long k = 0;
			for(k=0;k<=100000;k++)
			{
				__asm__ volatile ("nop" ::);		//so delay routine not optimised away
			}
		}
	}

	return 0;
//**************************	





 return 0;


}



/*
Old code - unrolled loops effectively - see also "known_goodXXXXXX.c" for previous version of this code


	PORTB &= ~(1<<RCK);

	PORTB |= (1<<SER);
	PORTB &= ~(1<<SCK);
	delay();
	PORTB |= (1<<SCK);
	delay();
	PORTB &= ~(1<<SCK);
	PORTB &= ~(1<<SER);
	//PORTB |= (1<<RCK);
	
	delay();
	 
	//clock high
	PORTB |= (1<<SER);		//1			//QH (right most LED)
	PORTB |= (1<<SCK);
	PORTB |= (1<<RCK);

	delay();
//++++++++++++++++++++++
//2
	//clock low
	PORTB &= ~(1<<RCK);
	PORTB &= ~(1<<SCK);
	PORTB &= ~(1<<SER);

	delay();
	
	//clock high
	PORTB &= ~(1<<SER);		//0
	PORTB |= (1<<SCK);
	PORTB |= (1<<RCK);

	delay();
//++++++++++++++++++++++
//3

	//clock low
	PORTB &= ~(1<<RCK);
	PORTB &= ~(1<<SCK);
	PORTB &= ~(1<<SER);

	delay();
	
	//clock high
	PORTB |= (1<<SER);		//1
	PORTB |= (1<<SCK);
	PORTB |= (1<<RCK);

	delay();
//++++++++++++++++++++++
//4

	//clock low
	PORTB &= ~(1<<RCK);
	PORTB &= ~(1<<SCK);
	PORTB &= ~(1<<SER);

	delay();
	
	//clock high
	PORTB |= (1<<SER);		//1
	PORTB |= (1<<SCK);
	PORTB |= (1<<RCK);

	delay();
//++++++++++++++++++++++
//5

	//clock low
	PORTB &= ~(1<<RCK);
	PORTB &= ~(1<<SCK);
	PORTB &= ~(1<<SER);

	delay();
	
	//clock high
	PORTB &= ~(1<<SER);		//0
	PORTB |= (1<<SCK);
	PORTB |= (1<<RCK);

	delay();
//++++++++++++++++++++++
//6

	//clock low
	PORTB &= ~(1<<RCK);
	PORTB &= ~(1<<SCK);
	PORTB &= ~(1<<SER);

	delay();
	
	//clock high
	PORTB |= (1<<SER);		//1
	PORTB |= (1<<SCK);
	PORTB |= (1<<RCK);

	delay();
//++++++++++++++++++++++
//7
	//clock low
	PORTB &= ~(1<<RCK);
	PORTB &= ~(1<<SCK);
	PORTB &= ~(1<<SER);

	delay();
	
	//clock high
	PORTB &= ~(1<<SER);		//0
	PORTB |= (1<<SCK);
	PORTB |= (1<<RCK);

	delay();
//++++++++++++++++++++++
//8
	//clock low
	PORTB &= ~(1<<RCK);
	PORTB &= ~(1<<SCK);
	PORTB &= ~(1<<SER);

	delay();
	
	//clock high
	PORTB &= ~(1<<SER);		//1
	PORTB |= (1<<SCK);
	PORTB |= (1<<RCK);

	delay();
//++++++++++++++++++++++


//	PORTB &= ~(1<<RCK);
//	delay();
//	PORTB |= (1<<RCK);
//	delay();
	PORTB &= ~(1<<RCK);


*/
