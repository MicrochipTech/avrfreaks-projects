/*
 * single7seg.c
 *
 * Created: 12/23/2020 8:32:20 PM
 *  Author: CmdrZin
 *
 * A Common Anode 7-segment LED is connected to PORTE and PORTF of an ATmega4809 Curiosity Nano board.
 * The pins of these ports were closest to the end of the board and using them made the protoboard 
 * connections easier.
 */ 
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <stdbool.h>

// Mapped to PF5:2  PE3:0
/* *** LED Segment TABLE *** */
/*
 *      A
 *    F   B
 *      G
 *    E   C
 *      D   dp
 *
 *    GFEDdCBA
 */
const uint8_t segments[] PROGMEM = {
	// d C D E G F A B
	0b01110111,		// 0
	0b01000001,		// 1
	0b00111011,		// 2
	0b01101011,		// 3
	0b01001101,		// 4
	0b01101110,		// 5
	0b01111011,		// 6
	0b01000011,		// 7
	0b01111111,		// 8
	0b01001111,		// 9
	0b01011111,		// A
	0b01111100,		// B
	0b00110110,		// C
	0b01111001,		// D
	0b00111110,		// E
	0b00011110,		// F
	0b00000000		// BLANK
};

void init_7seg() {
	PORTE_DIR |= PIN3_bm | PIN2_bm | PIN1_bm | PIN0_bm;
	PORTF_DIR |= PIN5_bm | PIN4_bm | PIN3_bm | PIN2_bm;
}

void setDisplay(uint8_t val, bool dp) {
	uint8_t pattern;
	if( val < sizeof(segments)) {
		// Clear Ports
		PORTE_OUT &= 0xF0;
		PORTF_OUT &= 0xC3;

		pattern = pgm_read_byte(&segments[val]);
		
		PORTE_OUT |= pattern & 0x0F;
		PORTF_OUT |= (pattern>>2) & 0x3C;
	}
	
	if(dp) {
		PORTF_OUT |= PIN5_bm;
	}
}
