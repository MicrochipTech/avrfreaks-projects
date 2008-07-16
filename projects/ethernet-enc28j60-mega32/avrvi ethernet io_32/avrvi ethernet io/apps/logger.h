#ifndef _LOGGER_H_
#define _LOGGER_H_
#include "../main.h"
#include "../io/clock.h"
#include "../io/lm75.h"
#include "../util/string.h"
#include <avr/pgmspace.h>
#include <avr/eeprom.h>

//for older gcc versions:
#ifndef EEMEM
 #define EEMEM  __attribute__ ((section (".eeprom")))
#endif

#define LOGGER_NOW 0
#define LOGGER_MIN 1
#define LOGGER_MAX 2

//store current day in eeprom
extern unsigned char logger_temp_today[] EEMEM;

extern unsigned char logger_count;
extern unsigned int logger_lastsave;

void logger_init(void);
void logger_do(void);
void logger_output_temp(unsigned char *buffer, unsigned char type);

#endif
