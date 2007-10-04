/*
TITLE : DRIVER FOR MCP6S28 PGA
FILE NAME : PGA.c
DESCRIPTION : MAIN PROGRAM
AUTHOR : BALAJI V
DATE : 10TH OCTOBER, 2007
*/

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include "PGA.h"

int main(void)
{
	spiInit();
	sei();							//SETS GLOBAL INTERRUPT
	while(1)
	{
	//-------------------------------------------------------------------------------------------------
	//CODE FOR ATMEGA2560
	
		cbi(PORTB,0);				//ASSERT CHIP SELECT LOW (SELECT CHIP)
		spiTransferByte(0x40);		//TRANSFER GAIN INSTRUCTION - 0B0100 0000
		spiTransferByte(0x01);		//TRANSFER GAIN DATA - 0B0000 0001
		sbi(PORTB,0);				//ASSERT CHIP SELECT HIGH (DESELECT CHIP)
		delay_ms(10);	
		cbi(PORTB,0);				//ASSERT CHIP SELECT LOW (SELECT CHIP)
		spiTransferByte(0x41);		//TRANSFER CHANNEL INSTRUCTION - 0100 0001
		spiTransferByte(0x00);		//TRANSFER CHANNEL DATA - 0B0000 0000
		sbi(PORTB,0);				//ASSERT CHIP SELECT HIGH (DESELECT CHIP)
		
	//-------------------------------------------------------------------------------------------------
	
	//-------------------------------------------------------------------------------------------------
	//CODE FOR ATMEGA16
	/*
		cbi(PORTB,4);				//ASSERT CHIP SELECT LOW (SELECT CHIP)
		spiTransferByte(0b01000000);//TRANSFER GAIN INSTRUCTION - 0100 0000
		spiTransferByte(0b00000001);//TRANSFER GAIN DATA - 0B0000 0001
		sbi(PORTB,4);				//ASSERT CHIP SELECT HIGH (DESELECT CHIP)
		//sbi(DDRB,0);				//ASSERT CHIP SELECT HIGH (DESELECT CHIP)
		delay_ms(10);	
		cbi(PORTB,4);				//ASSERT CHIP SELECT LOW (SELECT CHIP)
		//cbi(DDRB,0);				//ASSERT CHIP SELECT LOW (SELECT CHIP)
		spiTransferByte(0x41);		//TRANSFER CHANNEL INSTRUCTION - 0100 0001
		spiTransferByte(0x00);		//TRANSFER CHANNEL DATA - 0000 0000
		sbi(PORTB,4);				//ASSERT CHIP SELECT HIGH (DESELECT CHIP)
	*/	
	
	//-------------------------------------------------------------------------------------------------
	}
}

