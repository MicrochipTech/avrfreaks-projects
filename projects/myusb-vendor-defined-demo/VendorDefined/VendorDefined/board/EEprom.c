/*
	EEPROM functions for the AVR.
	
	2008-03-17, P.Harvey-Smith.
*/

#include "EEprom.h"

/* Initialise the EEPROM */
void InitEE(void)
{
	/* Init EEPROM writes to do erase & write together */
	EECR &= ~((1<<EEPM1) | (1<<EEPM0));
}

/* Read the EEPROM */
uint8_t ReadEE(uint16_t	Address)
{
	WaitEE();				// Wait for eeprom to be free
	
	EEAR=Address;			// Setup address to read
	EECR |= (1<<EERE);		// Do the read, clock will halt for 4 cycles
	
	return EEDR;			// Return the read data
}

/* Write the EEPROM */
void WriteEE(uint16_t	Address, uint8_t Data)
{
	WaitEE();				// Wait for eeprom to be free
	
	EEAR=Address;			// Setup address to write
	EEDR=Data;				// Setup data to write
	
	cli();					// Disable inturrupts while writing
	EECR |= (1<<EEMPE);	// Prepare for write instruction
	EECR |= (1<<EEPE);		// Do the write
	sei();					// Re-enable inturrupts
}
