#ifndef EEPROM169
#define EEPROM169

void EEPROM_write(unsigned long uiAddress, unsigned char ucData);
unsigned char EEPROM_read(unsigned long uiAddress);
void clearEEprom();

#endif
