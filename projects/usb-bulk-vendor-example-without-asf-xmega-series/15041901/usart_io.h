/*
*
*	tcj 3-30-2105
*
*	15033001
*
*	usart_io.h
*
*		setup of USARTs for ATXMEGA 16C4, 32C4 controllers
*
*		implementation of printf(), putchar(), getcha()r, kbhit() functionality for AVR32UC3B
*

*	source file needs two #defines:
*
*		1. frequency of peripheral bus: e.g #define F_PER 48000000L  (note the L for long)
*
*		2. which USART will be used for output from sr_printf(): e.g. #define PRINT_USART 0     (or 1 or 2)
*
*
*	followed by #include "usart_io.h"
*
*
*	usage:
*
*		setup_usart(usart_no, baud, parity, bits, stop)
*
*			usart_no: 0, 1 or 2  (for USARTC0, USARTC1, USARTD0)
*
*			baud: standard rate between 300 and 115200
*
*			parity: N, E, or O  (none, even, odd)
*
*			bits: 5, 6, 7, or 8
*
*			stop:	1 or 2 stop bits
*
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
    if(PRINT_USART == 0)
	{
		while (!(USARTC0_STATUS & USART_RXCIF_bm));		// wait for character in rx buffer
    	return USARTC0_DATA;							// return the character
	}

	else if(PRINT_USART == 1)
	{
		while (!(USARTC1_STATUS & USART_RXCIF_bm));		// wait for character in rx buffer
    	return USARTC1_DATA;							// return the character
	}

	else if(PRINT_USART == 2)
	{
		while (!(USARTD0_STATUS & USART_RXCIF_bm));		// wait for character in rx buffer
    	return USARTD0_DATA;							// return the character
	}	
}


uint8_t sr_kbhit(void)
{
	if(PRINT_USART == 0)
	{
    	if(USARTC0_STATUS & USART_RXCIF_bm) return 1;	// check if character in rx buffer; return 1 if so
		else return 0;									// otherwise return 0
	}

	else if(PRINT_USART == 1)
	{
    	if(USARTC1_STATUS & USART_RXCIF_bm) return 1;	// check if character in rx buffer; return 1 if so
		else return 0;									// otherwise return 0
	}

	else if(PRINT_USART == 2)
	{
    	if(USARTD0_STATUS & USART_RXCIF_bm) return 1;	// check if character in rx buffer; return 1 if so
		else return 0;									// otherwise return 0
	}

}


void sr_putchar(uint8_t inchar)				// sends a character out the tx line of the USART selected for sr_printf()
{
	if(PRINT_USART == 0)
	{
		while(!(USARTC0_STATUS & USART_DREIF_bm));
		USARTC0_DATA = inchar;
	}

	else if(PRINT_USART == 1)
	{
		while(!(USARTC1_STATUS & USART_DREIF_bm));
		USARTC1_DATA = inchar;
	}

	else if(PRINT_USART == 2)
	{
		while(!(USARTD0_STATUS & USART_DREIF_bm));
		USARTD0_DATA = inchar;
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

uint8_t setup_usart(uint8_t usart_no, uint16_t baud, uint8_t no_bits, uint8_t parity, uint8_t no_stop)
{

// set up USART

// usart clock = peripheral clock, defined in source file as F_PER.

	uint8_t i, par = 0, log2_scaler = 0;
	uint16_t bsel, scaler;



	
	switch(parity)
	{
		case 'N':	par = 0x00; break;
		case 'E':	par = 0x20; break;
		case 'O':	par = 0x30; break;
	}


	if(usart_no == 0)
	{
		PORTC_DIR |= 0x08;				// make bit 3 of PORTC output - tx
		PORTC_DIR &= ~0x04;				// make bit 2 of PORTC input - rx

		USARTC0_CTRLB |= USART_RXEN_bm;						// enable receiver
		USARTC0_CTRLB |= USART_TXEN_bm;						// enable transmitter

		USARTC0_CTRLC |= par;								// set parity
		USARTC0_CTRLC |= (no_stop - 1) >> 3;				// set stop bit
		USARTC0_CTRLC |= no_bits - 5;						// set number of data bits



		scaler = (F_PER/512)/baud;										// we're determining the minimum BSCALE that
																		// keeps BSEL under its max of 4096, but
		for(i = 0; i < 16; i++) if(scaler >> i) log2_scaler = i;		// as large as possible.  This gives maximum
		log2_scaler += 1;												// granularity for baud rate
		if(log2_scaler < 1) log2_scaler = 1;

		if(log2_scaler >= 7) USARTC0_BAUDCTRLB |= log2_scaler << 4;		// BSCALE is
		else USARTC0_BAUDCTRLB |= (log2_scaler + 9) << 4;				// two's complement

		bsel = ((8*F_PER)/(1 << log2_scaler))/baud - 1;

		USARTC0_BAUDCTRLB |= (bsel >> 8) & 0x0f;
		USARTC0_BAUDCTRLA |= bsel & 0xff;
	}

	if(usart_no == 1)
	{
		PORTC_DIR |= 0x80;				// make bit 7 of PORTC output - tx
		PORTC_DIR &= ~0x40;				// make bit 6 of PORTC input - rx

		USARTC1_CTRLB |= USART_RXEN_bm;						// enable receiver
		USARTC1_CTRLB |= USART_TXEN_bm;						// enable transmitter

		USARTC1_CTRLC |= par;								// set parity
		USARTC1_CTRLC |= (no_stop - 1) >> 3;				// set stop bit
		USARTC1_CTRLC |= no_bits - 5;						// set number of data bits



		scaler = (F_PER/512)/baud;										// we're determining the minimum BSCALE that
																		// keeps BSEL under its max of 4096, but
		for(i = 0; i < 16; i++) if(scaler >> i) log2_scaler = i;		// as large as possible.  This gives maximum
		log2_scaler += 1;												// granularity for baud rate
		if(log2_scaler < 1) log2_scaler = 1;

		if(log2_scaler >= 7) USARTC1_BAUDCTRLB |= log2_scaler << 4;		// BSCALE is
		else USARTC1_BAUDCTRLB |= (log2_scaler + 9) << 4;				// two's complement

		bsel = ((8*F_PER)/(1 << log2_scaler))/baud - 1;

		USARTC1_BAUDCTRLB |= (bsel >> 8) & 0x0f;
		USARTC1_BAUDCTRLA |= bsel & 0xff;
	}
	

	if(usart_no == 2)
	{
		PORTD_DIR |= 0x08;				// make bit 3 of PORTD output - tx
		PORTD_DIR &= ~0x04;				// make bit 2 of PORTD input - rx

		USARTD0_CTRLB |= USART_RXEN_bm;						// enable receiver
		USARTD0_CTRLB |= USART_TXEN_bm;						// enable transmitter

		USARTD0_CTRLC |= par;								// set parity
		USARTD0_CTRLC |= (no_stop - 1) >> 3;				// set stop bit
		USARTD0_CTRLC |= no_bits - 5;						// set number of data bits



		scaler = (F_PER/512)/baud;										// we're determining the minimum BSCALE that
																		// keeps BSEL under its max of 4096, but
		for(i = 0; i < 16; i++) if(scaler >> i) log2_scaler = i;		// as large as possible.  This gives maximum
		log2_scaler += 1;												// granularity for baud rate
		if(log2_scaler < 1) log2_scaler = 1;

		if(log2_scaler >= 7) USARTD0_BAUDCTRLB |= log2_scaler << 4;		// BSCALE is
		else USARTD0_BAUDCTRLB |= (log2_scaler + 9) << 4;				// two's complement

		bsel = ((8*F_PER)/(1 << log2_scaler))/baud - 1;

		USARTD0_BAUDCTRLB |= (bsel >> 8) & 0x0f;
		USARTD0_BAUDCTRLA |= bsel & 0xff;
	}


	return(0);
}


