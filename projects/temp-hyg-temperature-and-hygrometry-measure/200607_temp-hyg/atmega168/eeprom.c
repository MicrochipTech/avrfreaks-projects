#include <avr/io.h>
#include <avr/interrupt.h>

#include "eeprom.h"

#if defined (__AVR_ATmega168__)	
#define EEPROM_MWE EEMPE
#define EEPROM_WE EEPE
#elif defined (__AVR_ATmega8__)
#define EEPROM_MWE EEMWE
#define EEPROM_WE EEWE
#else
#  error "Device not defined"
#endif 


void eepromInit(void)
{
	EECR = 0;		// disable interrupts, erase & write at the same time
}

uint8_t eepromReadByte(uint16_t address)
{
	while (EECR & _BV(EEPROM_WE));	// wait for eeprom ready
	EEAR = address;					// set address, then increment
	EECR |= _BV(EERE);				// read eeprom
	return EEDR;					// read data
}

void eepromReadBuffer(uint16_t address, uint8_t *buffer, uint8_t length)
{
	uint8_t i;
	uint16_t eAddress = address;
	
	while (EECR & _BV(EEPROM_WE));		// wait for eeprom ready
	for (i=0; i<length; i++)
	{
		EEAR = eAddress ++;				// set address, then increment
		EECR |= _BV(EERE);				// read eeprom
		buffer[i] = EEDR;				// read data
	}
}

void eepromWriteByte(uint16_t address, uint8_t data)
{
	while (EECR & _BV(EEPROM_WE));	// wait for eeprom ready
	EEAR = address;					// set address
	EECR |= _BV(EERE);				// read eeprom
	if (EEDR == data) return;		// return if writing the same as read
	EEDR = data;					// set data
	cli();							// disable interrupts
	EECR |= _BV(EEPROM_MWE);		// set master write enable
	EECR |= _BV(EEPROM_WE);			// write eeprom
	sei();							// enable interrupts
}

void eepromWriteBuffer(uint16_t address, uint8_t *buffer, uint8_t length)
{
	uint16_t eAddress = address;
	uint8_t i;
	
	for (i=0; i<length; i++) eepromWriteByte(eAddress ++, buffer[i]);
}
