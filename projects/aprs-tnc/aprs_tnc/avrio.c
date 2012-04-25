//************************************************************************
// File:	avrio.c
//
//			Serial I/O function library
//			ADC function library
//
//			(C) 2007 George Scolaro
//************************************************************************


#include <avr/io.h>
#include <avr/interrupt.h>

#include "avrio.h"
#include "nmea.h"
#include "main.h"


//*******************************************************************
// This function initializes the USART baud rate, data size etc.
// TX and RX interrupts are enable for data transmission
//

extern void	usart_init(void)
{
	// initialize USART0

	UBRR0H = 0;		// baud rate = 4800 baud at 14.7456 MHz
	UBRR0L = 191;	// typical GPS NMEA baud rate

	UCSR0A = 0x0;	// U2X0 = 0 (16 tics per bit)

	UCSR0C = (3<<UCSZ00);	// 8 data bits, no parity

	// enable Rx and Tx hardware interrupts

	UCSR0B = _BV(RXCIE0)|_BV(TXCIE0)|_BV(RXEN0)|_BV(TXEN0);

	// initialize USART1

	UBRR1H = 0;		// baud rate = 4800 baud at 14.7456 MHz
	UBRR1L = 191;	// for PC communication

	UCSR1A = 0x0;	// U2X0 = 0 (16 tics per bit)

	UCSR1C = (3<<UCSZ10);	// 8 data bits, no parity

	// enable Rx and Tx hardware interrupts

	UCSR1B = _BV(RXCIE1)|_BV(TXCIE1)|_BV(RXEN1)|_BV(TXEN1);

}


//*******************************************************************
// This function pushes a new character into the output buffer
// then pre-advances the pointer to the next empty location.
//

void usart_tx0_byte(uint8_t data)
{
	if (++o_wrptr0 == OBUF0_SIZE)	// if write pointer reached buffer end
		o_wrptr0 = 0;				//  wrap back to zero
	o_buf0[o_wrptr0] = data;		// store data in circular buffer

	if (UCSR0A & _BV(UDRE0)) {		// if the TX buffer is empty
		if (++o_rdptr0 == OBUF0_SIZE)	// if read pointer reached buffer end
			o_rdptr0 = 0;			//  wrap back to zero
		UDR0 = o_buf0[o_rdptr0];	// Place the byte in the output buffer
	}
}


//*******************************************************************
// This function sends a null-terminated string to the USART TX port.

extern void usart_tx0_string(char *address)
{
	while (*address != 0)			// if not null,
		usart_tx0_byte(*(address++));//  transmit byte
}


//*******************************************************************
// This function pushes a new character into the output buffer
// then pre-advances the pointer to the next empty location.
//

void usart_tx1_byte(uint8_t data)
{
	if (++o_wrptr1 == OBUF1_SIZE)	// if write pointer reached buffer end
		o_wrptr1 = 0;				//  wrap back to zero
	o_buf1[o_wrptr1] = data;		// store data in circular buffer

	if (UCSR1A & _BV(UDRE1)) {		// if the TX buffer is empty
		if (++o_rdptr1 == OBUF1_SIZE)	// if read pointer reached buffer end
			o_rdptr1 = 0;			//  wrap back to zero
		UDR1 = o_buf1[o_rdptr1];	// Place the byte in the output buffer
	}
}


//*******************************************************************
// This function sends a null-terminated string to the USART TX port.

extern void usart_tx1_string(char *address)
{
	while (*address != 0)			// if not null,
		usart_tx1_byte(*(address++));//  transmit byte
}

//*******************************************************************
// This function checks to see if a valid command has been sent from
// the PC.

extern uint8_t get_pc_command()
{
	uint8_t nchar, toss_packet = 0;

	if (pc_eol) {			// got full pc packet
		if (i_buf1[0] != '$')	// something wrong, throw it away
			toss_packet = 1;
		else {				// process command
			i_rdptr0 = 0;

		}
		if (toss_packet) {
			cli();
			i_wrptr1 = 0;
			pc_start = 1;			// look for PC '$' - start of packet
			pc_eol = 0;				// and try again
			nchar = UDR1;			// read character in
			sei();
			return (1);				// done
		}
		return(0);
	}
	return(2);						// nothing to do
}


//*******************************************************************
// Initializes the ADC control registers.

extern void adc_init(void)
{
	ADMUX = 0x47;		// Initialize the ADC to use AVCC

	ADCSRA = _BV(ADEN) | _BV(ADSC) | 7;// Enable ADC, start conversion, prescale of 128
	ADCSRB = 0;
	ADCSRA |= _BV(ADSC);	// start another conversion
}


//*******************************************************************
// This function reads the data from the A/D Channel and
// returns it to the calling function.


extern uint16_t adc_read()
{
	uint16_t adc_data;

	if (ADCSRA & _BV(ADSC))
		return (0);			// ADC not ready, return 0

	adc_data = ADC;			// get ADC data
	ADCSRA |= _BV(ADSC);	// start another conversion
	return (adc_data);		// read ADC channel and return the data
}


//*******************************************************************
// Output string to LCD.
//  Passes null delimited string and line number

#define LCD_LENGTH	8

void lcd(char *string, uint8_t line_no, uint8_t char_no)
{
	uint8_t line_len = 0;

	command_mode = 1;		// command mode

	if (line_no == 0) {
		to_lcd(0x8, 1);		// set address to first row
		to_lcd(char_no, 1);
	}
	else {
		to_lcd(0xc, 1);		// set address to 2nd row (0x40 starting)
		to_lcd(char_no, 1);
	}

	command_mode = 0;		// data mode

	while (*string) {
		char_to_lcd(*string++);
		line_len++;
	}
	for ( ; line_len < LCD_LENGTH; line_len++)
		char_to_lcd(' ');	// clear to end of line
}

//*******************************************************************
// Write 8 bit data to LCD in 2 nibbles

void char_to_lcd(uint8_t data)
{
	to_lcd((data >> 4) & 0xf, 1);
	to_lcd(data & 0xf, 1);
	quick_wait(40);			// ~40 uS delay
}

//*******************************************************************
// LCD write routine
// 
// command_mode = 1 => writing commands to LCD controller
//				= 0 => writing data to LCD


#define RW 0x4			// PB2
#define EN 0x2			// PB1
#define RS 0x1			// PB0

#define BF 0x40			// PC6

#define LCDOUT	0x70	// 3 bits for LCD data output
#define LCDMSB	0x40	// msb for LCD data output


void to_lcd(uint8_t data, uint8_t poll)
{
	uint8_t done;

	data &= 0xf;			// strip off top bits

	DDRC &= ~LCDMSB;		// LCD data is input
	DDRA &= ~LCDOUT;

	PORTB |= RW;			// R/W = 1 (Read)
	PORTB &= ~RS;			// RS = 0

	done = 0;
	delay_ticker = 0;		// set ticker timeout

	while (poll) {			// Note: will timeout after 10ms even if no LCD
		PORTB |= EN;		// set EN high
		quick_wait(1);
		if (!(PINC & BF))	// wait on BF = 1
			done = 1;
		PORTB &= ~EN;		// set EN low
		quick_wait(1);

		// dummy second nibble, but must read it anyway

		PORTB |= EN;		// set EN high
		quick_wait(2);
		PORTB &= ~EN;		// set EN low

		if (done || delay_ticker > 16)	// wait on BF = 1 or 10msec elapsed
			break;
	} 

	DDRC |= LCDMSB;			// LCD data is output
	DDRA |= LCDOUT;

	if (!command_mode)		// if not command mode then
		PORTB |= RS;		//  data mode

	PORTB &= ~RW;			// R/W = 0 (write)

	PORTA = (PORTA & ~LCDOUT) | ((data & 0x7) << 4);// put data out to LCD
	PORTC = (PORTC & ~LCDMSB) | ((data & 0x8) << 3);
	quick_wait(1);
	PORTB |= EN;			// set EN high
	quick_wait(1);
	PORTB &= ~EN;			// set EN low
}

//*******************************************************************
// Initialize LCD controller
// mode = 0 => enable display without cursor
//		= 1 => enable display with cursor

void init_lcd(int mode)
{
	command_mode = 1;	// command mode

	to_lcd(0x3, 0);		// don't poll
	delay_wait(16);
	to_lcd(0x3, 0);
	delay_wait(8); 
	to_lcd(0x3, 0);
	delay_wait(8); 
	to_lcd(0x3, 0);
	delay_wait(8); 
	to_lcd(0x2, 0);		// 4 bit mode
	delay_wait(8); 

	to_lcd(0x2, 1);		// 4 bit mode
	to_lcd(0x8, 1);

	to_lcd(0x0, 1);		// disable display & cursor
	to_lcd(0x8, 1);

	to_lcd(0x0, 1);		// clear display
	to_lcd(0x1, 1);
	delay_wait(1); 

	to_lcd(0x0, 1);		// increment char position after each write
	to_lcd(0x6, 1);

	to_lcd(0x0, 1);
	if (mode == 0)
		to_lcd(0xc, 1);	// enable display, but not cursor
	else
		to_lcd(0xe, 1);	// enable display and cursor
}

//*******************************************************************
// wait 1 tick = 1/1200sec

void delay_wait(uint8_t delay)
{
	delay_ticker = 0;
	while (delay_ticker < delay)	// roughly 10ms for delay == 8
		;
}

//*******************************************************************
// Just to add a few 100nsec of delay

uint8_t quick_wait(uint8_t delay)
{
	uint8_t i, j;

	j = delay;

	for (i = 0; i < delay; i++) {
		j++;
		j = j >> 2;
		j++;
		j = j >> 2;
		j++;
		j = j >> 2;
		j++;
		j = j >> 2;
		j++;
	}
	return(j);
}

