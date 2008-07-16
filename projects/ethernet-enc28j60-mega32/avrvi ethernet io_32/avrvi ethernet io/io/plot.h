#ifndef _PLOT_H_
#define _PLOT_H_
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/eeprom.h>
#include "../net/mynic.h"
#include "clock.h"
#include "../apps/logger.h"

#define PLOT_SOURCE_EEPROM 1
#define PLOT_SOURCE_DATAFLASH 2


void plot_test(void);
extern PROGMEM unsigned char PLOT_BMP_HEAD[];

//void plot_generate_bmp(unsigned char *eeptr, unsigned int datapos);
unsigned int plot_generate_bmp(unsigned char *buffer, unsigned int datapos, unsigned int len, unsigned int streampos, unsigned char source);
#endif
