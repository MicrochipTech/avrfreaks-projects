/************************
Test Bootloader Program

Copyright (C) 2005  Edward Welch welcher@edjusted.com

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.



************************/
#include <avr/io.h>
#include <avr/boot.h>
#include <inttypes.h>
#include <avr/interrupt.h>
#include <avr/signal.h>

#define BAUDPIN 0x20
#define BAUDPORT PINC
#define PROGPIN 0x40
#define PROGPORT PINC


//Delcare Function Prototypes
void nop(void);
void udp_send(uint8_t);

//Main Program
void main(void)
{
	//Disable Interupts
	cli();
	//Declare Variables
	void (*funcptr)( void ) = 0x0000; 
	uint8_t page[64];
	uint8_t x,rec,i,j,l,numpages,temp;
	uint16_t k, tempaddr, ADDRESS;
	
	ADDRESS = 0x0000;
	
	//Initialize Ports
	DDRD = 0xFF;		//PortD output
	DDRC = 0x00;		//PortC input
	PORTC = 0xFF;		//Enable pull-ups on PORTC
	
	//Initalize UART
	UCSRB = (1<<TXEN)|(1<<RXEN);		//enable RX and TX

	//Check PC5 to see if SP baudrate has been set to 38.4kbps
	if ((BAUDPORT & BAUDPIN) == BAUDPIN)	//If this condition is true, SP baudrate is 9600 and needs to be changed
	{
		outp(51, UBRRL); 				//init UART speed for 8 MHz 9600bps
		
		//Wait for SP
		for(i=1;i<255;i++)
			for(k=1;k<1000;k++)
			 nop();
		
		
		//Clear IO3 on SP
		outp(0x90,UDR);					//send writex command to siteplayer
		do								//wait for UDR to clear
			x = inp(UCSRA) & 0x20;
		while (x != 0x20);
		
		outp(0x14,UDR);					//send address to siteplayer LB
		do								//wait for UDR to clear
			x = inp(UCSRA) & 0x20;
		while (x != 0x20);
	
		outp(0xFF,UDR);					//send address to siteplayer HB
		do								//wait for UDR to clear
			x = inp(UCSRA) & 0x20;
		while (x != 0x20);
		
		outp(0x00,UDR);					//Set I03 on SP = 0 
		do								//wait for UDR to clear
			x = inp(UCSRA) & 0x20;
		while (x != 0x20);
		
		for (j = 1; j<10; j++)			//Short delay
		nop();
		
		//Send new Baudrate setting
		outp(0x91,UDR);					//send writex command to siteplayer
		do								//wait for UDR to clear
			x = inp(UCSRA) & 0x20;
		while (x != 0x20);
		
		outp(0x1A,UDR);					//send address to siteplayer LB
		do								//wait for UDR to clear
			x = inp(UCSRA) & 0x20;
		while (x != 0x20);
	
		outp(0xFF,UDR);					//send address to siteplayer HB
		do								//wait for UDR to clear
			x = inp(UCSRA) & 0x20;
		while (x != 0x20);
		
		outp(0xDF,UDR);					//send data to siteplayer LB
		do								//wait for UDR to clear
			x = inp(UCSRA) & 0x20;
		while (x != 0x20);
		
		outp(0xFF,UDR);					//send data to siteplayer HB
		do								//wait for UDR to clear
			x = inp(UCSRA) & 0x20;
		while (x != 0x20);
		
		for (j = 1; j<100; j++)		//Short Delay
		nop();
			
	}
		
	outp(12, UBRRL); 					// init UART speed for 8 MHz 38.4kbs
		
	for (i = 1; i<10;i++)
	{
		PORTD = 0xFF;
		for(j = 1; j<255; j++)
			for(k=1; k<500; k++);
		PORTD = 0x00;
		for(j = 1; j<255; j++)
			for(k=1; k<500; k++);
	}
	
	//Wait for input from Host Computer
/*	temp = 0;
	while ( temp  == 0)
	{
		do							//Wait for byte
			x = inp(UCSRA) & 0x80;
		while (x != 0x80);
		
		temp = UDR;
	}
*/	
	
	//Check PORTC to see if programming mode is requested
	temp = PROGPORT & PROGPIN;
	if (temp == PROGPIN)
	{
		temp = 0x02;
	}
	else temp = 0x01;
	
	//If computer sends 0x01 enter programming mode, else start main program
	if (temp == 0x01)
	{
		
		//Blink LED
		for (i = 1; i<20;i++)
		{
			PORTD = 0xFF;
			for(j = 1; j<255; j++)
				for(k=1; k<500; k++);
			PORTD = 0x00;
			for(j = 1; j<255; j++)
				for(k=1; k<500; k++);
				
			
		}
		
		
		//Erase Flash
		for (k = 0; k < 6144 ; k +=64)
		{
			boot_page_erase(k);
					while(boot_rww_busy())
					{
						boot_rww_enable();
					}
					
		}
		
		//SEND 0x02 To confirm program mode
			udp_send(0x02);
		
		//Receive # of Pages
		
			do
				x = inp(UCSRA) & 0x80;
			while (x != 0x80);
			
			numpages = UDR;
		
		//Send ACK for # of Pages (0x03)
			udp_send(0x03);
		
		
		//Enter Programming Loop
		
		for (l=1;l<=numpages;l++)
		{		
			//Wait for host computer to initiate page send
				rec = 0;
				while (rec != 0x04 )
				{
					do								//Wait for byte
						x = inp(UCSRA) & 0x80;
					while (x != 0x80);
					
					rec = UDR;
				}
				
			//Send 0x05 to start page transfer one byte at a time
				UDR = 0x80;
				do								//wait for UDR to clear
					x = UCSRA & 0x20;
				while (x != 0x20);
				
				UDR = 0x00;
				do								//wait for UDR to clear
					x = UCSRA & 0x20;
				while (x != 0x20);
				
				UDR = 0x05;						//Load ACK
				do								//wait for UDR to clear
					x = UCSRA & 0x20;
				while (x != 0x20);
				
				UDR = 0x50;						//Send ACK
					do							//wait for UDR to clear
					x = UCSRA & 0x20;
					while (x != 0x20);
			
			//Receive Page
				for (i=0;i<64;i++)
				{
					do
						x = inp(UCSRA) & 0x80;
					while (x!= 0x80);
					page[i] = UDR;
					
					if (i < 63)
					{
						UDR = 0x50;						//Send ready
						do								//wait for UDR to clear
						x = UCSRA & 0x20;
						while (x != 0x20);						
					}	
				}
			
			//Erase Page (commented out since Flash has already been erased)
				//boot_page_erase(ADDRESS);
				//while(boot_rww_busy())
				//{
				//	boot_rww_enable();
				//}
				
			//Load Page
				for(k = 0; k < (0 + SPM_PAGESIZE); k += 2)
				{
					tempaddr = page[k] + (page[k+1] << 8);
					boot_page_fill(k, tempaddr);
				}
				
			// Write page.
				boot_page_write(ADDRESS);
				while(boot_rww_busy())
				{
					boot_rww_enable();
				}
				
			//Increment Address to next page
				ADDRESS = ADDRESS + 64;
	
			//Tell host computer to send next page
				udp_send(0x06);
			
		}	
		
		//Done programming, Enable Interupts and reset device to run application.
		udp_send(0x09);
		nop();
		nop();
		sei();
		WDTCR = (1<<WDE);
		for (;;);
		//funcptr();
	}
	//else funcptr();
	else
	{
		//WDTCR = (1<<WDE);
		//for (;;);
		//asm volatile("rjmp 0x0000"::);
		funcptr();
	}
}

void nop(void)
	{
		asm volatile("nop"::);
	}

void udp_send(uint8_t val)
{
			uint8_t x;
			
			UDR = 0x80;
			do								//wait for UDR to clear
				x = UCSRA & 0x20;
			while (x != 0x20);
			
			UDR = 0x00;
			do								//wait for UDR to clear
				x = UCSRA & 0x20;
			while (x != 0x20);
			
			UDR = val;
			do								//wait for UDR to clear
				x = UCSRA & 0x20;
			while (x != 0x20);
			
			UDR = 0x50;
			do								//wait for UDR to clear
				x = UCSRA & 0x20;
			while (x != 0x20);
}
