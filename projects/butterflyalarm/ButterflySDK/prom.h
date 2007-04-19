/** @file eeprom.h Declarations of EEPROM functions
*/

#ifndef __EEPROM_HEADER__
#define __EEPROM_HEADER__

/*******************************************************************************
* Exported functions
*******************************************************************************/

extern void EEPROMWriteBytes(uint8_t *buffer,
							 char bufferLength,
							 uint16_t address);

extern void EEPROMReadBytes(char *buffer,
							char bufferLength,
							uint16_t address);

#endif // __EEPROM_HEADER__
