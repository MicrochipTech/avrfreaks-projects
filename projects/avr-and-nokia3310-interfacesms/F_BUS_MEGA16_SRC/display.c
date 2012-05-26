#include "display.h"

unsigned char _digit0=0,_digit1=1,_digit2=2,_digit3=3;

unsigned char _7seg1[] PROGMEM = {
	((0<<B) | (0<<C) | (0<<D) | (0<<E) | (1<<G) | (1<<DB)) ,   /* '0' */
	((0<<B) | (0<<C) | (1<<D) | (1<<E) | (1<<G) | (1<<DB)) ,   /* '1' */
	((0<<B) | (1<<C) | (0<<D) | (0<<E) | (0<<G) | (1<<DB)) ,   /* '2' */
	((0<<B) | (0<<C) | (0<<D) | (1<<E) | (0<<G) | (1<<DB)) ,   /* '3' */
	((0<<B) | (0<<C) | (1<<D) | (1<<E) | (0<<G) | (1<<DB)) ,   /* '4' */
	((1<<B) | (0<<C) | (0<<D) | (1<<E) | (0<<G) | (1<<DB)) ,   /* '5' */
	((1<<B) | (0<<C) | (0<<D) | (0<<E) | (0<<G) | (1<<DB)) ,   /* '6' */
	((0<<B) | (0<<C) | (1<<D) | (1<<E) | (1<<G) | (1<<DB)) ,   /* '7' */
	((0<<B) | (0<<C) | (0<<D) | (0<<E) | (0<<G) | (1<<DB)) ,   /* '8' */
	((0<<B) | (0<<C) | (0<<D) | (1<<E) | (0<<G) | (1<<DB)) ,   /* '9' */
	((0<<B) | (0<<C) | (1<<D) | (0<<E) | (0<<G) | (1<<DB)) ,   /* 'A' */
	((1<<B) | (0<<C) | (0<<D) | (0<<E) | (0<<G) | (1<<DB)) ,   /* 'B' */
	((1<<B) | (1<<C) | (0<<D) | (0<<E) | (1<<G) | (1<<DB)) ,   /* 'C' */
	((0<<B) | (0<<C) | (0<<D) | (0<<E) | (0<<G) | (1<<DB)) ,   /* 'D' */
	((1<<B) | (1<<C) | (0<<D) | (0<<E) | (0<<G) | (1<<DB)) ,   /* 'E' */
	((1<<B) | (1<<C) | (1<<D) | (0<<E) | (0<<G) | (1<<DB)) ,   /* 'F' */
	((0<<B) | (1<<C) | (1<<D) | (1<<E) | (0<<G) | (1<<DB)) };  /* 'Blank'*/

unsigned char _7seg2[] PROGMEM = {
	((0<<A) | (0<<F )) ,   /* '0' */
	((1<<A) | (1<<F )) ,   /* '1' */
	((0<<A) | (1<<F )) ,   /* '2' */
	((0<<A) | (1<<F )) ,   /* '3' */
	((1<<A) | (0<<F )) ,   /* '4' */
	((0<<A) | (0<<F )) ,   /* '5' */
	((0<<A) | (0<<F )) ,   /* '6' */
	((0<<A) | (0<<F )) ,   /* '7' */
	((0<<A) | (0<<F )) ,   /* '8' */
	((0<<A) | (0<<F )) ,   /* '9' */
	((0<<A) | (0<<F )) ,   /* 'A' */
	((1<<A) | (0<<F )) ,   /* 'B' */
	((0<<A) | (0<<F )) ,   /* 'C' */
	((1<<A) | (1<<F )) ,   /* 'D' */
	((0<<A) | (0<<F )) ,   /* 'E' */
	((0<<A) | (0<<F )) ,   /* 'F' */
	((0<<A) | (0<<F )) };  /* 'Blank'*/

	void RefreshDisplay(void)
	{
		EnableDigit0();
		/*clear all segments*/
		PORTD &= ~((1<<B) | (1<<C) | (1<<D) | (1<<E) | (1<<G) | (1<<DB));
		PORTC &= ~((1<<A) | (1<<F));

		PORTD |= pgm_read_byte (&_7seg1 [_digit0]);
		PORTC |= pgm_read_byte (&_7seg2 [_digit0]);
		_delay_ms (_disp_delay_time);

		EnableDigit1();
		/*clear all segments*/
		PORTD &= ~((1<<B) | (1<<C) | (1<<D) | (1<<E) | (1<<G) | (1<<DB));
		PORTC &= ~((1<<A) | (1<<F));

		PORTD |= pgm_read_byte (&_7seg1 [_digit1]);
		PORTC |= pgm_read_byte (&_7seg2 [_digit1]);
		_delay_ms (_disp_delay_time);

		EnableDigit2();
		/*clear all segments*/
		PORTD &= ~((1<<B) | (1<<C) | (1<<D) | (1<<E) | (1<<G) | (1<<DB));
		PORTC &= ~((1<<A) | (1<<F));

		PORTD |= pgm_read_byte (&_7seg1 [_digit2]) & (~(1 << DB));
		PORTC |= pgm_read_byte (&_7seg2 [_digit2]);
		_delay_ms (_disp_delay_time);

		EnableDigit3();
		/*clear all segments*/
		PORTD &= ~((1<<B) | (1<<C) | (1<<D) | (1<<E) | (1<<G) | (1<<DB));
		PORTC &= ~((1<<A) | (1<<F));

		PORTD |= pgm_read_byte (&_7seg1 [_digit3]);
		PORTC |= pgm_read_byte (&_7seg2 [_digit3]);
		_delay_ms (_disp_delay_time);

	}

	void InitDisplay(void)
	{
		DDRB |= ((1 << _7SEG_CA_1)|(1 << _7SEG_CA_2)|(1 << _7SEG_CA_3));
		DDRA |= ((1 << _7SEG_CA_0));
		DDRC |= ((1 << LED_0)|(1 << LED_1)|(1 << A)|(1 << F));
		DDRD |= ((1<<B) | (1<<C) | (1<<D) | (1<<E) | (1<<G) | (1<<DB));
		PORTC = ((1 << LED_0)|(1 << LED_1));
	}

	void write_temp(unsigned int value)
	{
		/*put the temperature in the XX.X form*/
		_digit1 = (unsigned char)(value & 0x000f) ;
		_digit2 = (unsigned char)((value & 0x00f0 ) >> 4)  ;
		_digit3 = (unsigned char)((value & 0x0f00 ) >> 8) ;
		_digit0 = 0x10;//(unsigned char)((value & 0xf000 ) >> 12) ;
	}
