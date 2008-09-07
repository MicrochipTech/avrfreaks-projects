/*
	EEPROM functions for the AVR.
	
	2008-03-17, P.Harvey-Smith.
*/

#include <avr/io.h>
#include <avr/interrupt.h>

#ifndef __EEPROM__
#define __EEPROM__

#define EEPROMSize	512

/* Initialise the EEPROM */
void InitEE(void);

/* Read the EEPROM */
uint8_t ReadEE(uint16_t	Address);

/* Write the EEPROM */
void WriteEE(uint16_t	Address, uint8_t Data);

#define WaitEE()	while(EECR & (1<<EEPE)) ;
#endif
