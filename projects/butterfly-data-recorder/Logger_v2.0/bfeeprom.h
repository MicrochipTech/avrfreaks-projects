
#ifndef BF_EEPROM_H_
#define BF_EEPROM_H_

void LoadEEPROM(char *pBuffer, char num_bytes, unsigned int *EE_START_ADR);
void StoreEEPROM(char *pBuffer, char num_bytes, unsigned int *EE_START_ADR);

#endif
