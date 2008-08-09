#ifndef __debug_h_included__
#define __debug_h_included__

#include <avr/io.h>

#ifndef uchar
	#define uchar						unsigned char
#endif

#define EEPROM_ADDRESS_MAX              512

unsigned int alamat_EEPROM;
unsigned int idx_eep;

void tulisEEPROM(uchar data);

#endif
