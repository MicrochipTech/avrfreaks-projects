/* Teensy RawHID example
 * http://www.pjrc.com/teensy/rawhid.html
 * Copyright (c) 2009 PJRC.COM, LLC
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above description, website URL and copyright notice and this permission
 * notice shall be included in all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */


/*
	Easy code for testing only .. test to maximum byte send to MCU from MCU..

	this sample code use 1472bytes flash and 68bytes ram.


*/



#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <util/delay.h>


// global definition
#define 	outb(addr, data)   addr = (data)
#define 	inb(addr)   (addr)
#define 	outw(addr, data)   addr = (data)
#define 	inw(addr)   (addr)
#define 	outp(addr, data)   addr = (data)
#define 	inp(addr)   (addr)
#define 	BV(bit)   (1<<(bit))
#define 	cbi(reg, bit)   reg &= ~(BV(bit))
#define 	sbi(reg, bit)   reg |= (BV(bit))
#define 	cli()   __asm__ __volatile__ ("cli" ::)
#define 	sei()   __asm__ __volatile__ ("sei" ::)
#define 	DDR(x)   ((x)-1)
#define 	PIN(x)   ((x)-2)
#define B2X(b) ((b)<=9)?((b)+'0'):((b)-10+'A')

#include "usb_rawhid.h"

#define CPU_PRESCALE(n)	(CLKPR = 0x80, CLKPR = (n))

volatile uint8_t do_output=0;


int main(void)
{
uint8_t temp ;


	sbi(PORTF,1);	// pull up for PORT F pin 1 - ARUINO Leonardo board A4 pin

	// UART setting for serial port debug if received data are same

	UCSR1A = 0;                         // importantly U2X1 = 0
	UCSR1B = (1 << TXEN1);                         // interrupts enabled in here if you like
	UCSR1C = _BV(UCSZ11) | _BV(UCSZ10); // no parity, 8 data bits, 1 stop bit
	UBRR1  = 51;                       // 9600 baud @ 8MHz XTAL with U2X1 = 0    
	SU = 0;

	// set for 8 MHz clock
	CPU_PRESCALE(0);
	// Initialize the USB, and then wait for the host to set configuration.
	// If the Teensy is powered without a PC connected to the USB port,
	// this will wait forever.
	usb_init();
	while (!usb_configured()) /* wait */ ;

	// Wait an extra second for the PC's operating system to load drivers
	// and do whatever it does to actually be ready for input
	_delay_ms(1000);

	while (1) 
		{
		if (!(PINF & (1<<1)))
			{
			while (!(PINF & (1<<1)));
			//temp = 0;
			for (temp=0;temp < (RAWHID_TX_SIZE ); temp++) {buffer[temp]=126-temp;} // fill buffer for send .. with 40,3F,3E..2,1 (in hex)
			usb_rawhid_send(buffer,8); // send buffer 8 - timeout..
			}

		if (SU)
			{
			SU = 0;
			 for (temp=0;temp < (RAWHID_TX_SIZE + 1); temp++)
				{
				while(!(UCSR1A & (1 << UDRE1)));   //wait until tx register is empty
				if (buffer[temp]) outb(UDR1,buffer[temp]);
				}
			}


		}
}



 
