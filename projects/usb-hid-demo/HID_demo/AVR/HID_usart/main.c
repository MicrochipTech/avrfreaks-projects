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
	Easy code for testing only ..
	Simple USB <-> USART communication 
	Use 9600 speed, timeout 32ms
	Rx,Tx interrupt not used
	this sample code use less then 1600bytes flash and 68bytes ram.

	Tested on ARDUINO Leonardo dev board with internal RC 8Mhz crystall



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
uint8_t temp,rxcount ;
uint8_t	Rxbuff[RAWHID_TX_SIZE];


	// UART setting for serial port debug if received data are same

	UCSR1A = 0;                         // importantly U2X1 = 0
	UCSR1B = (1 << RXEN1) | (1 << TXEN1); // enable RX,TX , no interupt..
	UCSR1C = _BV(UCSZ11) | _BV(UCSZ10); // no parity, 8 data bits, 1 stop bit
	UBRR1  = 51;                       // 9600 baud @ 8MHz XTAL with U2X1 = 0    

	// status setting
	SU = 0;
	rxcount = 0;
	memset(&Rxbuff,0,sizeof(Rxbuff));

	// timer 0 for timeout receive data from usart
    TCCR0A = 0x00;
    TCCR0B = 0x05;	// prescaler 1024
	 

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

	TCNT0 =0;			// clear counter.
	sbi(TIFR0,TOV0);	// clear bit if set..

	while (1) 
		{

		// timeout every 32ms
		if (bit_is_set(TIFR0,TOV0))
			{
			TCNT0 =0;			// clear counter.
			sbi(TIFR0,TOV0);	// clear bit if set..
			if (rxcount)
				{
				usb_rawhid_send(Rxbuff,8); // send buffer 8 - timeout..
				rxcount = 0;				// clear counter
				memset(&Rxbuff,0,sizeof(Rxbuff));	// clear buffer
				}
			}
		
		// check if are some data
		if (UCSR1A & (1<<RXC1))
			{
			TCNT0 =0;			// clear counter.
			sbi(TIFR0,TOV0);	// clear bit if set..
			Rxbuff[rxcount++] = UDR1;	// store data to buffer
			if (rxcount>63)				// if buffer is full then send to USB
				{
				usb_rawhid_send(Rxbuff,8); // send buffer 8 - timeout..
				rxcount = 0;				// clear counter
				memset(&Rxbuff,0,sizeof(Rxbuff));	// clear buffer
				}
			}	

		// data are received send to usart..
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



 
