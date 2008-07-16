#ifndef _CRC8_H_
#define _CRC8_H_
#include <avr/pgmspace.h>

unsigned char crc8_calc(unsigned char *data, unsigned char crc_start, unsigned int len);
unsigned char crc8_calc_byte_rev0x07(unsigned char crc, unsigned char data);
unsigned char crc8_calc_byte(unsigned char crc, unsigned char data);
extern unsigned char crc8_lookuptable[] PROGMEM;
#endif
