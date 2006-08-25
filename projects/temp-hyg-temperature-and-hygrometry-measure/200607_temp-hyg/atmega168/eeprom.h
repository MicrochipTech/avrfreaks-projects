#ifndef EEPROM_H
#define EEPROM_H 1

void eepromInit(void);
uint8_t eepromReadByte(uint16_t);
void eepromReadBuffer(uint16_t, uint8_t *, uint8_t);
void eepromWriteByte(uint16_t, uint8_t);
void eepromWriteBuffer(uint16_t, uint8_t *, uint8_t);

#endif
