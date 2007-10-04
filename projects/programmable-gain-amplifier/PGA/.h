/*
TITLE : DRIVER FOR MCP6S28 PGA
FILE NAME : PGA.h
DESCRIPTION : HEADER FILE
AUTHOR : BALAJI V
DATE : 10TH OCTOBER, 2007
*/

#include "global.h"
volatile u08 spiTransferComplete;

//FUNCTION DECLARATIONS

//---------------------------------SPI FUNCTION DECLARATIONS---------------------
void spiInit(void);
u08 spiTransferByte(u08 data);
u16 spiTransferWord(u16 data);
void delay_ms(unsigned short ms);
//---------------------------------SPI FUNCTION DECLARATIONS---------------------
//---------------------------------UART FUNCTION DECLARATIONS---------------------


//---------------------------------UART FUNCTION DECLARATIONS---------------------



// SPI interrupt service handler
SIGNAL(SIG_SPI)
{
	spiTransferComplete = TRUE;
}

void spiInit(void)
{
//-------------------------------------------------------------------------

	//SETTINGS FOR ATMEGA2560
	
	sbi(DDRB,0);			//SS PIN IS OUTPUT (MASTER MODE)
	sbi(DDRB,1);			//SCK PIN IS OUTPUT (MASTER MODE)
	sbi(DDRB,2);			//MOSI PIN IS OUTPUT (MASTER MODE)
	cbi(DDRB,3);			//MISO PIN IS INPUT (MASTER MODE)
	
	sbi(PORTB,0);			//SS PIN IS DRIVEN HIGH (DESELECT THE DEVICE)
	sbi(PORTB,1);			//SCK PIN IS DRIVEN HIGH (START GIVING CLOCK)
	sbi(PORTB,2);			//MOSI PIN IS HIGH (MASTER TRANSMITTING 1)
	
//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
	//SETTINGS FOR ATMEGA16
	/*
	sbi(DDRB,4);			//SS PIN IS OUTPUT (MASTER MODE)
	sbi(DDRB,7);			//SCK PIN IS OUTPUT (MASTER MODE)
	sbi(DDRB,5);			//MOSI PIN IS OUTPUT (MASTER MODE)
	cbi(DDRB,6);			//MISO PIN IS INPUT (MASTER MODE)
	

	
	sbi(PORTB,4);			//SS PIN IS DRIVEN HIGH (DESELECT THE DEVICE)
	sbi(PORTB,7);			//SCK PIN IS DRIVEN HIGH (START GIVING CLOCK)
	sbi(PORTB,5);			//MOSI PIN IS HIGH (MASTER TRANSMITTING 1)
	*/
//-------------------------------------------------------------------------	
//-------------------------------------------------------------------------	
	
	sbi(SPCR,SPIE);			//ENABLE SPI INTERRUPTS
	sbi(SPCR,SPE);			//ENABLE SPI
	cbi(SPCR,DORD);			//DIRECTION IS MSB FIRST THEN LSB
	sbi(SPCR,MSTR);			//MICROCONTROLLER IS SELECTED AS SPI MASTER
	cbi(SPCR,CPOL);			//LEADING EDGE SAMPLE (RISING EDGE)
	cbi(SPCR,CPHA);			//SETUP (FALLING EDGE)
	cbi(SPCR,SPR1);			//SCK FREQUENCY = FS/4
	cbi(SPCR,SPR0);
	cbi(SPSR,SPI2X);		//DOUBLE SPEED BIT = 0
//-------------------------------------------------------------------------	
}
u08 spiTransferByte(u08 data)
{
	spiTransferComplete = FALSE;
	outb(SPDR,data);
	while(!spiTransferComplete);
	return inb(SPDR);
}
u16 spiTransferWord(u16 data)
{
	u16 rxData = 0;
	rxData = (spiTransferByte(data>>8))<<8;
	rxData |= spiTransferByte(data);
	return rxData;
}

void delay_ms(unsigned short ms)
{
	unsigned short outer1, outer2;
	outer1 = 200; 
	while (outer1) 
	{
		outer2 = 10;
		while (outer2)
		{
			while(ms)
				ms--;
			outer2--;
		}
		outer1--;
	}
}
