#ifndef _EEPROM_H_
#define _EEPROM_H_
#include <compat/twi.h>

void eeprom_init(void);
void eeprom_send_start(void);
void eeprom_send_stop(void);
void eeprom_send_device_address(unsigned char addr);
void eeprom_send_byte(unsigned char byte);
unsigned char eeprom_read(void);
void eeprom_write(unsigned char data);
unsigned char eeprom_read_byte(unsigned int addr);
void eeprom_write_byte(unsigned int addr, unsigned char buf);
void eeprom_dump();
void eeprom_clear();
#endif
