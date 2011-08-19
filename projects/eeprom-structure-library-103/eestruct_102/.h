#ifndef EEPROMSTRUCT_H
#define EEPROMSTRUCT_H

unsigned int Crc16Memory(void *AData, unsigned int ASize);
void ClearEEPROM();
unsigned char LoadEEPROMStructure(void *AStructure);
unsigned char SaveEEPROMStructure(void *AStructure);

#endif

