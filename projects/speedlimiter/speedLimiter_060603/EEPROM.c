/*------------------------------------------------------------------------
|
|	Project:		speedLimiter
|					EEPROM.c
|
|	Author:			Rasmus Renzi,	rasmus.r@spray.se
|
|
|
--------------------------------------------------------------------------
|
|	Description:	Speedlimiter for engines with mechanical ignition
|					systems.
--------------------------------------------------------------------------*/

#include <avr/io.h>
#include <avr/interrupt.h>
#include "speedLimiter.h"
#include "EEPROM.h"


#define EEPROM_C

void EEPROM_write(unsigned char address, unsigned char data)
{
	unsigned char masterWriteEnable;
	unsigned char writeEnable;	

	cli();

	while (EECR & _BV(EEPE));								// Wait for completion of previous write
	EEARH = 0;												// Set up address register
	EEARL = address;										// Set up address register 
	EEDR = data;
	masterWriteEnable = EECR;
	masterWriteEnable |= _BV(EEMPE);						// Set EEMPE (master WE) bit in tmp. register variable
	writeEnable = masterWriteEnable| _BV(EEPE);				// Set EEPE (WE) bit in tmp. register variable
	EECR = masterWriteEnable;								// Set actual master write enable bit in control register
	EECR = writeEnable;										// Set actual write enable bit in control register (within 4 clk. cycles).

	sei();
}

unsigned char EEPROM_read(unsigned char address)
{
	cli();													
	while(EECR & (1<<EEPE));								// Wait for completion of previous write
	EEARH = 0;												// Set up address register
	EEARL = address;										// Set up address register
	EECR |= (1<<EERE);										// Start eeprom read by writing EERE
	return EEDR;											// Return data from Data Register
	sei();
}



#undef EEPROM_C
