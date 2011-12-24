/*--------------------- IR remote control receiver (NEC format):------------------------*/
/*--------------------------------------------------------------------------------------*/
/*-------------------AVR used: ATMEGA16 (8 MHz Internal oscillator)---------------------*/
/*--------------------------------------------------------------------------------------*/
/*---------------------------Compiler: AVR-GCC (WinAVR 2010)----------------------------*/
/*--------------------------------------------------------------------------------------*/
/*-------------Author: Abdullbasit H. AlHaita(onlyabdullbasit@gmail.com)----------------*/
/*--------------------------------------------------------------------------------------*/
/*----------------------------------Date: 28 Nov 2010-----------------------------------*/
/*------------------------------------- I R . C ----------------------------------------*/


/*--------------------------------- I N C L U D E S ------------------------------------*/
#include <avr/io.h>
#include <avr/interrupt.h>
#include "usart.h"
/*-------------------------------- V A R I A B L E S -----------------------------------*/

unsigned char BitCount,DataCode,CustomCode,_DataCode,_CustomCode;
unsigned long IR=0;//received 32-bit command( custom code , custom code' , Data Code , Data Code' )
unsigned int oldICR1,newICR1,CycleCount;

/*------------------------------ D I F I N I T I O N S ---------------------------------*/
ISR (SIG_INPUT_CAPTURE1)
{
	oldICR1 = newICR1 ;
	newICR1 = ICR1;
	CycleCount=newICR1-oldICR1;

	if(BitCount==0)
	{
		if((CycleCount<1800) && (CycleCount>1620)) // Header detected (1687.5 cycles)
		{
			BitCount+=1;
		}
	}
	if((BitCount>0) && (CycleCount<150) && (CycleCount>140))// logical "0" detected
	{

		BitCount+=1;
		IR<<=1;
	}
	if((BitCount>0) && (CycleCount<300) && (CycleCount>280))// logical "1" detected
	{
		BitCount+=1;
		IR<<=1;
		IR|=1;
	}
	if(BitCount ==33)
	{
		BitCount=0;
		_DataCode  = ~((IR & 0xFFFF) & 0xFF);//must equal to DataCode
		DataCode = (IR & 0xFFFF)>>8;
		_CustomCode  = ~((IR >> 16) & 0xFF);//must equal to CustomCode
		CustomCode = (IR >> 16)>>8;

		if((DataCode== _DataCode) && (CustomCode == _CustomCode))
			printf("Received command: 0x%08lx\n",IR);
		else
			printf("Error receiving command\n");
	}

}

void initIR(void)
{
	TCCR1B=(1<<CS11)|(1<<CS10); // clk/64
	TIMSK=(1<<TICIE1);
	sei();
}
