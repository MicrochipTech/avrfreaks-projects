/*
	CONTROLLER SHARES 1 BI-DIRECTIONAL LINE WITH CUBE TO SEND AND RECIEVE	
	DATA.  LINE IS PULLED TO HIGH BY CUBE @ ABOUT 3.4v. I CONFIGURED THE	
	AVR PORT AS AN INPUT WITH NO PULL-UPS. THEN TO PULL THE LINE LOW I JUST	
	SWITCHED THE PIN TO OUTPUT TO PULL THE LINE LOW. TO SEND A LOW 
	BIT - LINE IS PULLED LOW FOR FOR 3uS THEN HIGH FOR 1uS.  A HIGH
	BIT IS 1uS LOW THEN 3uS HIGH LOW FOLLOWED BY A 1uS HIGH.												

	LOW          HIGH														
	     _		   ___														
    |   | |		| |   |														
	|___| |		|_|   |														
	
	^ 			^															
	|___________|__ALL BITs START ON FALLING EDGE.							

	IN THIS EXAMPLE I USED A M8 RUNNING AT 16 Mhz.  DATA STORAGE IS DONE
	TEMPORARILY IN AN ARRAY OF CHAR.  THIS SEEMED TO BE THE	FASTEST METHOD.
*/

#define F_CPU 16000000UL 
#include <avr/io.h>
#include <stdio.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include "UART.h"

unsigned char Data[64];   // This is not pretty, but the data needs to be stored quickly!
unsigned char Bytes[8];
int i,j;
unsigned char temp[10];
unsigned char hex[] ="0123456789ABCDEF";
unsigned char tByte;
uint8_t CommandString[]={0,1,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,1,0,1};

void Rec_Dat(uint8_t *bufptr)
{
	// following ASM Routine SendS CommandString - Several Timing loops are used to establish proper timing.
	// I am trying to clean this up.  I initially did this with SBI's,CBI's and a lot of NOP
	// I don't know how appropriate the coding style is for in-line assembly but it seems to work.
	asm volatile (
				"ldi		r20, 0x1A	\n"	//load r20 with 26 one more than command string
				"1:						\n"	//Loop:
				"dec		r20			\n"	
				"breq		7f			\n"	// jump to end
				"sbi		0x14, 0x05	\n"	// pull line low
				"ld			r18, %a0+	\n" // load CommandString Value from memory
				"cpi		r18, 0		\n"	// check if it is a 1 or 0
				"brne		4f			\n" // value is highbit jump to highbit
				"ldi		r19, 0x0D	\n" // load r19 with for first low loop
				"2:						\n" //Low1:
				"dec		r19			\n"	// decrement r19	
				"brne		2b			\n"	// back to Low1
				"nop					\n"	// for timing
				"nop					\n"	// dito
				"cbi		0x14, 0x05	\n"	// Release line to high
				"ldi		r19,0x03	\n"	// Second loop while line is high
				"3:						\n" //Low2:
				"dec		r19			\n"	// decrement r19
				"brne		3b			\n"	// back to Low2
				"nop					\n"	// Another timing NOP
				"rjmp		1b			\n"	// back to top for next bit
				"4:						\n"	//Highbit:
				"ldi		r19, 0x02	\n"	// Short loop since line is already low
				"5:						\n"	//High1:
				"dec		r19			\n"	// decrement r19
				"brne		5b			\n"	// back to High1
				"nop					\n"	// Timing
				"nop					\n"	// dito
				"cbi		0x14, 0x05	\n"	// line back to high
				"ldi		r19, 0x0E	\n"	// long loop for highbit
				"6:						\n"	//High2:
				"dec		r19			\n"	// decrement r19	
				"brne		6b			\n"	// back to High2
				"rjmp		1b			\n"	// back to top of loop
				"7:						\n"	// the end!
				:
				:"e" (bufptr)
				: "r18","r19","r20"
				);

  for(i=0; i<64; i++)
    {
	while (PINC & (1 << PC5)) {}   // Wait for low transistion
	
		asm volatile (	"nop \n" "nop \n" "nop \n" "nop \n" "nop \n" "nop \n" "nop \n" "nop \n"
					"nop \n" "nop \n" "nop \n" "nop \n" "nop \n" "nop \n" "nop \n" "nop \n");
		asm volatile (	"nop \n" "nop \n" "nop \n" "nop \n" "nop \n" "nop \n" "nop \n" "nop \n");

	   
	if (PINC & (1 << PC5))   // is high
	{
		Data[i]=1;
	
	}
	else
	{
		Data[i]=0;
		while (!(PINC & (1 << PC5))) {}
	}
	
	}
}

void Print_Data_Bits(void)

{ j=0;
for (i=0; i<64; i++)
	{ j++;
		if (Data[i]) 
			{ UART_puts("1"); }
		else
			{ UART_puts("0"); }
		if (j==8)
			{ UART_puts(" "); j=0; }
	}
UART_puts("\n\r\n");
}

void Print_Data_Bytes(void)
{

j=8;
for(i=0; i<64; i++)
	{
	j--;
	if (Data[i])   
			{ Bytes[(i>>3)] |= (1<<j); }
	else
			{ Bytes[(i>>3)] &= ~(1<<j); }
	if (j==0) j=8;
	}


/*	Print in HEX  uncomment if you want it displayed in HEX
for(i=0; i<8; i++)
	{
	tByte = ((Bytes[i] & 0XF0) >>4);
	UART_putc(hex[tByte]);
	tByte = (Bytes[i] & 0x0F);
	UART_putc(hex[tByte]);
	UART_puts(" ");
	}
UART_puts("\n\r\n");
*/

for (i=0; i<8; i++)
	{
	UART_puts(itoa(Bytes[i],temp,10));
	UART_puts(" ");
	}
	UART_puts("\n\r\n");
}


int main(void)

{
	DDRC &= ~(1 << PC5);	// pin 0 input pin 
	PORTC &= ~(1 << PC5);	// pin 0 no pull-ups
	UART_init();
	sei();
	while (1)
	{
		Rec_Dat(CommandString);
		Print_Data_Bits();
		Print_Data_Bytes();
	}
}

	

