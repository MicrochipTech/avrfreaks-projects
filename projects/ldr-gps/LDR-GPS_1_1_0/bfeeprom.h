
#ifndef BF_EEPROM_H_
#define BF_EEPROM_H_

// mt - moved to vcard.h
// #define EEPROM_START 0x100 //the name will be put at this adress

void LoadEEPROM(char *pBuffer, char num_bytes, unsigned char *EE_START_ADR);
void StoreEEPROM(char *pBuffer, char num_bytes, unsigned char *EE_START_ADR);

#endif
