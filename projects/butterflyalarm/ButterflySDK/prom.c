/** @file eeprom.c EEPROM routines. Based on the gcc Butterfly port by Martin
	Thomas, KL, .de */

#include <avr/io.h>
#include <inttypes.h>
#include <avr/interrupt.h>

/** Reads one byte from EEPROM
	@param addr Address in EEPROM to read the byte from
	@result The byte read
*/
uint8_t eeprom_read_byte_169(const uint16_t *addr)
{
	// Wait for completion of previous write
	while(EECR & (1<<EEWE));
	cli();
	// Set up address register
	EEAR = *addr;
	// Start eeprom read by writing EERE
	EECR |= (1<<EERE);
	// Return data from Data Register
	sei();
	return EEDR;
}

/** Writes one byte to EEPROM
	@param addr Address in EEPROM where the byte will be written to
	@param val Value to write
*/
void eeprom_write_byte_169(uint16_t *addr, uint8_t val)
{
	// Wait for completion of previous write
	while(EECR & (1<<EEWE));
	cli();
	// Set up address and Data Registers
	EEAR = *addr;
	EEDR = val;
	// Write logical one to EEMWE
	EECR |= (1<<EEMWE);
	// Start eeprom write by setting EEWE
	EECR |= (1<<EEWE);
	while(EECR & (1<<EEWE)); // paranoia
	sei();
}

/** Writes a number of bytes to the EEPROM
	@param buffer Location in RAM where the bytes to be written are stored
	@param bufferLength Number of bytes to write
	@param address Address in EEPROM where the data will be written to
*/
void EEPROMWriteBytes(uint8_t *buffer, char bufferLength, uint16_t address)
{
	uint8_t i;
	for (i=0;i<bufferLength;i++) {
		eeprom_write_byte_169(&address, buffer[i]);
		address++;
	}
}

/** Reads a number of bytes from the EEPROM
	@param buffer Location in RAM where the read bytes will be copied to
	@param bufferLength Number of bytes to read
	@param address Address in EEPROM to read the bytes from
*/
void EEPROMReadBytes(char *buffer, char bufferLength, uint16_t address)
{
	uint8_t i;
	for (i=0;i<bufferLength;i++) {
		buffer[i]=eeprom_read_byte_169(&address);
		address++;
	}
}
