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
	Easy code for testing only .. test read write data from to internal eeprom
	this sample code use 1356bytes flash and 66bytes ram.
	Tested in ARDUINO Leonardo board with internal RC oscilator 8Mhz
	Pin PF1 (A4) is used to select what bank is used for read write.
	PF1 - 1 read wrtie from to 0 - 63 position
	PF1 - 0 read write from to 64 - 127 positon


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
	sbi(PORTF,1);	// pull up for PORT F pin 1 - ARUINO Leonardo board A4 pin
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
		}
}



 
