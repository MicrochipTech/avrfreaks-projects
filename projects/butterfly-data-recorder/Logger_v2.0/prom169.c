/* ATEMG169 specific EEPROM I/O 
*/
#include <avr/eeprom.h>
#include "eeprom169.h"

void EEPROM_write(unsigned long uiAddress, unsigned char ucData)
{
	/* Wait for completion of previous op */
	while(EECR & (1<<EEWE))	;

	/* Set up address and Data Registers */
	EEAR = (unsigned int) uiAddress;
	EEDR = ucData;

	/* Write logical one to EEMWE */
	EECR |= (1<<EEMWE);

	/* Start eeprom write by setting EEWE */
	EECR |= (1<<EEWE);
}

unsigned char EEPROM_read(unsigned long uiAddress)
{
	/* Wait for completion of previous op */
	while(EECR & (1<<EEWE))	;

	/* Set up address register */
	EEAR = (unsigned int) uiAddress;

	/* Start eeprom read by writing EERE */
	EECR |= (1<<EERE);

	/* Return data from Data Register */
	return EEDR;
}

// Clear EEPROM to 0xFF's
void clearEEprom()
{
	unsigned int adr;

	for(adr = 0; adr < 512; adr++)
	{
		EEPROM_write(adr,0xFF);
	}

}

