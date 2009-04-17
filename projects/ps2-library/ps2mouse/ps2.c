/*************************************************************************************
 Title:     PS/2 library
 Author:    Jan Pieter de Ruiter <janpieterd@lycos.nl>
 Date:      13-Jun-2002
 Software:  AVR-GCC with AvrStudio
 Target:    Any AVR device.

 DESCRIPTION
       Basic routines for implementing the ps/2 protocol (for a mouse, keyboard etc.)
       where the microcontroller acts as a "host"
       
       See http://panda.cs.ndsu.nodak.edu/~achapwes/PICmicro/ps2/ps2.html
       and http://panda.cs.ndsu.nodak.edu/~achapwes/PICmicro/mouse/mouse.html
       and http://panda.cs.ndsu.nodak.edu/~achapwes/PICmicro/keyboard/atkeyboard.html
       for more info

 USAGE
       See the C include ps2.h file for a description of each function
       
**************************************************************************************/
#include "ps2.h"
#include <avr/io.h>
#include <util/parity.h>

void delay(long microseconds)
{
	while(microseconds)microseconds--;	// Wait about "microseconds" microseconds (@ 4MHz oscillator)
}

int Read_ps2data(void)
{	
	int d[8];
	int i,parity;
	int f=1;
	int data=0;
	
	CBI(CLKDDR,CLK);             // Make CLK input
	CBI(DATADDR,DATADDR);        // Make DATA input     
	CBI(CLKPORT,CLK);            // Make CLK open
	CBI(DATAPORT,DATA);          // Make CLK open
	
	LOOP_UNTIL_BIT_IS_CLEAR(CLKPIN,CLK);     // Ignore startbit
	LOOP_UNTIL_BIT_IS_SET(CLKPIN,CLK);
	
	LOOP_UNTIL_BIT_IS_CLEAR(CLKPIN,CLK);     // Receive first databit
	if(BIT_IS_SET(DATAPIN,DATA))d[0]=1;
	else d[0]=0;
	LOOP_UNTIL_BIT_IS_SET(CLKPIN,CLK);

	LOOP_UNTIL_BIT_IS_CLEAR(CLKPIN,CLK);     // Receive second databit
	if(BIT_IS_SET(DATAPIN,DATA))d[1]=1;
	else d[1]=0;
	LOOP_UNTIL_BIT_IS_SET(CLKPIN,CLK);

	LOOP_UNTIL_BIT_IS_CLEAR(CLKPIN,CLK);     // etc.
	if(BIT_IS_SET(DATAPIN,DATA))d[2]=1;
	else d[2]=0;
	LOOP_UNTIL_BIT_IS_SET(CLKPIN,CLK);
	
	LOOP_UNTIL_BIT_IS_CLEAR(CLKPIN,CLK);
	if(BIT_IS_SET(DATAPIN,DATA))d[3]=1;
	else d[3]=0;
	LOOP_UNTIL_BIT_IS_SET(CLKPIN,CLK);
	
	LOOP_UNTIL_BIT_IS_CLEAR(CLKPIN,CLK);
	if(BIT_IS_SET(DATAPIN,DATA))d[4]=1;
	else d[4]=0;
	LOOP_UNTIL_BIT_IS_SET(CLKPIN,CLK);
	
	LOOP_UNTIL_BIT_IS_CLEAR(CLKPIN,CLK);
	if(BIT_IS_SET(DATAPIN,DATA))d[5]=1;
	else d[5]=0;
	LOOP_UNTIL_BIT_IS_SET(CLKPIN,CLK);
	
	LOOP_UNTIL_BIT_IS_CLEAR(CLKPIN,CLK);
	if(BIT_IS_SET(DATAPIN,DATA))d[6]=1;
	else d[6]=0;
	LOOP_UNTIL_BIT_IS_SET(CLKPIN,CLK);
	
	LOOP_UNTIL_BIT_IS_CLEAR(CLKPIN,CLK);
	if(BIT_IS_SET(DATAPIN,DATA))d[7]=1;
	else d[7]=0;
	LOOP_UNTIL_BIT_IS_SET(CLKPIN,CLK);
	
	LOOP_UNTIL_BIT_IS_CLEAR(CLKPIN,CLK);     // Receive parity bit
	if(BIT_IS_SET(DATAPIN,DATA))parity=0;
	else parity=1;
	LOOP_UNTIL_BIT_IS_SET(CLKPIN,CLK);

	LOOP_UNTIL_BIT_IS_CLEAR(CLKPIN,CLK);     // Ignore stop bit
	LOOP_UNTIL_BIT_IS_SET(CLKPIN,CLK);
	f=1;
	for(i=0;i<8;i++)   // Put all received bits in data
	{
		data+=d[i]*f;
		f*=2;
	}
	if(((parity==0)&&(parity_even_bit(data)==0))||((parity!=0)&&(parity_even_bit(data)!=0)))return data;   // Return data if received data is OK 
	else
	{
		Write_ps2data(0xFE);      // Send the "ReSend" commando
		return Read_ps2data();    // Receive data again
	}
	return 0;
}

void Write_ps2data(int data)
{
	CBI(DATADDR,DATA);       // Make DATA input
	CBI(DATAPORT,DATA);      // Make DATA open
	SBI(CLKDDR,CLK);         // Make CLK output
	CBI(CLKPORT,CLK);        // Make CLK open
	
	CBI(CLKPORT,CLK);	       // Cancel the mouse (if transmitting)
	delay(100);
	SBI(DATADDR,DATA);       // Make DATA output
	CBI(DATAPORT,DATA);      // Generate startbit
	delay(100);
	CBI(CLKDDR,CLK);         // Make CLK input
	CBI(CLKPORT,CLK);        // Make CLK open
	
	LOOP_UNTIL_BIT_IS_CLEAR(CLKPIN,CLK);     // Send first data bit
	if((data&0x01)==0){CBI(DATAPORT,DATA);}
	else SBI(DATAPORT,DATA);
	LOOP_UNTIL_BIT_IS_SET(CLKPIN,CLK);
	
	LOOP_UNTIL_BIT_IS_CLEAR(CLKPIN,CLK);     // Send first data bit
	if((data&0x02)==0){CBI(DATAPORT,DATA);}
	else SBI(DATAPORT,DATA);
	LOOP_UNTIL_BIT_IS_SET(CLKPIN,CLK);
													
	LOOP_UNTIL_BIT_IS_CLEAR(CLKPIN,CLK);     // etc.
	if((data&0x04)==0){CBI(DATAPORT,DATA);}
	else SBI(DATAPORT,DATA);
	LOOP_UNTIL_BIT_IS_SET(CLKPIN,CLK);
	
	LOOP_UNTIL_BIT_IS_CLEAR(CLKPIN,CLK);
	if((data&0x08)==0){CBI(DATAPORT,DATA);}
	else SBI(DATAPORT,DATA);
	LOOP_UNTIL_BIT_IS_SET(CLKPIN,CLK);
	
	LOOP_UNTIL_BIT_IS_CLEAR(CLKPIN,CLK);
	if((data&0x10)==0){CBI(DATAPORT,DATA);}
	else SBI(DATAPORT,DATA);
	LOOP_UNTIL_BIT_IS_SET(CLKPIN,CLK);
	
	LOOP_UNTIL_BIT_IS_CLEAR(CLKPIN,CLK);
	if((data&0x20)==0){CBI(DATAPORT,DATA);}
	else SBI(DATAPORT,DATA);
	LOOP_UNTIL_BIT_IS_SET(CLKPIN,CLK);
	
	LOOP_UNTIL_BIT_IS_CLEAR(CLKPIN,CLK);
	if((data&0x40)==0){CBI(DATAPORT,DATA);}
	else SBI(DATAPORT,DATA);
	LOOP_UNTIL_BIT_IS_SET(CLKPIN,CLK);
	
	LOOP_UNTIL_BIT_IS_CLEAR(CLKPIN,CLK);
	if((data&0x80)==0){CBI(DATAPORT,DATA);}
	else SBI(DATAPORT,DATA);
	LOOP_UNTIL_BIT_IS_SET(CLKPIN,CLK);
	
	LOOP_UNTIL_BIT_IS_CLEAR(CLKPIN,CLK);             // Send parity bit
	if(parity_even_bit(data)==0){CBI(DATAPORT,DATA);}
	else SBI(DATAPORT,DATA);
	LOOP_UNTIL_BIT_IS_SET(CLKPIN,CLK);

	LOOP_UNTIL_BIT_IS_CLEAR(CLKPIN,CLK);         // Send stop bit
	SBI(DATAPORT,DATA);
	LOOP_UNTIL_BIT_IS_SET(CLKPIN,CLK);
	
	CBI(DATADDR,DATA);
	CBI(DATAPORT,DATA);
	CBI(CLKDDR,CLK);
	CBI(CLKPORT,CLK);
	LOOP_UNTIL_BIT_IS_CLEAR(CLKPIN,CLK);    // Ignore acknowledge bit
	LOOP_UNTIL_BIT_IS_SET(CLKPIN,CLK);
}
