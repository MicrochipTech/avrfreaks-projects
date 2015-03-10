/*
*
*	tcj 3-7-2105
*
*	15030701
*
*	usart_io.h
*
*		setup of USARTs for AVR32UC3B
*
*		implementation of printf(), putchar(), getcha()r, kbhit() functionality for AVR32UC3B
*

*	source file needs two #defines:
*
*		1. frequency of PBA bus: e.g #define F_PBA 48000000L  (note the L for long)
*
*		2. which USART will be used for output from sr_printf(): e.g. #define PRINT_USART 0     (or 1 or 2)
*
*
*	followed by #include "usart_io.h"
*
*
*	usage:
*
*		setup_usart(usart_no, baud, parity, bits, stop, pin_fcn)
*
*			usart_no: 0, 1 or 2
*
*			baud: standard rate between 300 and 115200
*
*			parity: N, E, or O  (none, even, odd)
*
*			bits: 5, 6, 7, or 8
*
*			stop:	1 or 2 stop bits
*
*			pin_fcn:  which function (A, B, C, D) and therefore which pins to connect USART to
*
*		sr_printf()  works just like printf()
*
*		sr_putchar(), sr_getchar, sr_kbhit also work like their namesakes
*
*/

#include <stdio.h>
#include <stdarg.h>


uint8_t sr_getchar(void)
{
    while (!AVR32_USART2.CSR.rxrdy);		// wait for character in rx buffer
    return AVR32_USART2.RHR.rxchr;			// return the character
}


uint8_t sr_kbhit(void)
{
    if(AVR32_USART2.CSR.rxrdy) return 1;	// check if character in rx buffer; return 1 if so
	else return 0;							// otherwise return 0
}


void sr_putchar(uint8_t inchar)				// sends a character out the tx line of the USART selected for sr_printf()
{
	if(PRINT_USART == 0)
	{
		while(!AVR32_USART0.CSR.txrdy);
		AVR32_USART0.THR.txchr = inchar;
	}

	else if(PRINT_USART == 1)
	{
		while(!AVR32_USART1.CSR.txrdy);
		AVR32_USART1.THR.txchr = inchar;
	}

	else if(PRINT_USART == 2)
	{
		while(!AVR32_USART2.CSR.txrdy);
		AVR32_USART2.THR.txchr = inchar;
	}
}

void outprintf(char *s)							// sends a string out the USART2 tx line
{
	while (*s)									// for formatted output, use sprintf, then call
	{											// this function (as fprintf doesn't seem possible)
        sr_putchar(*s);
        s++;
    }
}

void sr_printf(const char *format, ...)			// works like printf with argument list
{
	va_list arg;
	char outstring[100];

	va_start (arg, format);
	vsprintf (outstring, format, arg);
	va_end (arg);

	outprintf(outstring);

}

uint8_t setup_usart(uint8_t usart_no, uint16_t baud, uint8_t no_bits, uint8_t parity, uint8_t no_stop, uint8_t pin_fcn)
{

// set up USART

// CLK_USART = PBA clock, defined in source file as F_PBA.

	uint8_t par, tport, rport, portno, abcd_fcn;

	uint8_t tportarray[12] = {19,99,43,99,23,99,34,12,99,26,21,99};			// maps UART and function to GPIO pad
	uint8_t rportarray[12] = {18,99,42,99,24,99,35,17,99,27,20,99};			// 99 means doesn't exist
	
	switch(parity)
	{
		case 'N':	par = 4; break;
		case 'E':	par = 0; break;
		case 'O':	par = 1; break;
	}

	switch(pin_fcn)
	{
		case 'A':	abcd_fcn = 0; break;
		case 'B':	abcd_fcn = 1; break;
		case 'C':	abcd_fcn = 2; break;
		case 'D':	abcd_fcn = 3; break;
	}

	tport = tportarray[4*usart_no+abcd_fcn];
	rport = rportarray[4*usart_no+abcd_fcn];

	if((tport > 63) || (rport > 63)) return(99);		// return error if requested function doesn't exist for UART

	if(tport > 31) portno = 1; else portno = 0;

	if(usart_no == 0)
	{
		AVR32_USART0.CR.rxen = 1;						// enable receiver
		AVR32_USART0.CR.txen = 1;						// enable transmitter

		AVR32_USART0.MR.over = 1;						// this is 8x sample clock, not 16x
		AVR32_USART0.MR.par = par;						// parity
		AVR32_USART0.MR.chrl = no_bits - 5;				// length
		AVR32_USART0.MR.nbstop = 2*(no_stop - 1);		// stop bits
		AVR32_USART0.BRGR.cd = F_PBA/(8*baud);			// baud rate divisor

		AVR32_GPIO.port[portno].gperc = 1 << tport;		// tx   /   this is a peripheral
		AVR32_GPIO.port[portno].pmr0s = 1 << tport;		// it's a B function peripheral

		AVR32_GPIO.port[portno].gperc = 1 << rport;		// rx   /   this is a peripheral
		AVR32_GPIO.port[portno].pmr0s = 1 << rport;		// it's a B function peripheral
	}

	if(usart_no == 1)
	{
		AVR32_USART1.CR.rxen = 1;						// enable receiver
		AVR32_USART1.CR.txen = 1;						// enable transmitter

		AVR32_USART1.MR.over = 1;						// this is 8x sample clock, not 16x
		AVR32_USART1.MR.par = par;						// parity
		AVR32_USART1.MR.chrl = no_bits - 5;				// length
		AVR32_USART1.MR.nbstop = 2*(no_stop - 1);		// stop bits
		AVR32_USART1.BRGR.cd = F_PBA/(8*baud);			// baud rate divisor

		AVR32_GPIO.port[portno].gperc = 1 << tport;		// tx   /   this is a peripheral
		AVR32_GPIO.port[portno].pmr0s = 1 << tport;		// it's a B function peripheral

		AVR32_GPIO.port[portno].gperc = 1 << rport;		// rx   /   this is a peripheral
		AVR32_GPIO.port[portno].pmr0s = 1 << rport;		// it's a B function peripheral
	}

	if(usart_no == 2)
	{
		AVR32_USART2.CR.rxen = 1;						// enable receiver
		AVR32_USART2.CR.txen = 1;						// enable transmitter

		AVR32_USART2.MR.over = 1;						// this is 8x sample clock, not 16x
		AVR32_USART2.MR.par = par;						// parity
		AVR32_USART2.MR.chrl = no_bits - 5;				// length
		AVR32_USART2.MR.nbstop = 2*(no_stop - 1);		// stop bits
		AVR32_USART2.BRGR.cd =     F_PBA/(8*baud);  	// baud rate divisor

		AVR32_GPIO.port[portno].gperc = 1 << tport;		// tx   /   this is a peripheral
		AVR32_GPIO.port[portno].pmr0s = 1 << tport;		// it's a B function peripheral

		AVR32_GPIO.port[portno].gperc = 1 << rport;		// rx   /   this is a peripheral
		AVR32_GPIO.port[portno].pmr0s = 1 << rport;		// it's a B function peripheral
	}

	return(0);
}


