#ifndef SBDEF
  #define SBDEF
  #include <stdio.h>
  #include "config.h"
  #include "AVR035.h"
#include "ee_settings.h"
#define EEPROM_VERSION 2
#define SBNG_VERSION_MAJOR 3
#define SBNG_VERSION_MINOR 0
u16_t * webserver;

#define SBNG_TARGET 3
uint16_t timer_update;
u16_t * webserver;
char tempbuf[120];  //atleast 250byte



#endif
