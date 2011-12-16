//*****************************************************************************
//
// Work File 	: ch595.h
// Description	: Driver for 595 shift register.
//
// Author       : Ivan Karežiæ
// Created      : 2011-03-03
// Revised      : 2011-03-03
// Version      : 1.0
// Target MCU	: Atmel AVR
// Uses         : avr-libc
//
// This code is distributed under the GNU Public License
// which can be found at http://www.gnu.org/licenses/gpl.txt
//
//*****************************************************************************

#ifndef SIPO_595_H_
#define SIPO_595_H_

#include <avr/io.h>

/*
 * /SRCLR -> PB5
 * SRCLK -> PB6
 * RCLK -> PG3
 * SI -> PG4
*/
// port usage definition
#define SRCLR_PORT	PORTB
#define SRCLK_PORT	PORTB
#define RCLK_PORT	PORTG
#define SI_PORT		PORTG

// if SIPO uses 2 different ports, both must be defined
#define SIPO1_DDR	DDRB
#define SIPO2_DDR	DDRG

// SIPO pin definitions
#define SRCLR_PIN	(1<<PIN6)
#define SRCLK_PIN	(1<<PIN7)
#define RCLK_PIN	(1<<PIN3)
#define SI_PIN		(1<<PIN4)

// Used to specify function of each SIPO in chain
// NOTE: First byte goes to first SIPO in chain!!!
#define OUTPUT		0
#define ERROR		1
#define STATUS		2

#define SPIO_COUNT	3				// Set number of SIPO ICs
uint8_t buff_content[SPIO_COUNT];	// Contains content of each driver, number of drivers is 3

void sipo_init();							// Initializes SIPO ports and cleans their registers
void sipo_clr();							// Cleans all registers
void sipo_sendbyte(uint8_t* sipo_buffer);	// Fills registers with array

#endif /* SIPO_595_H_ */
