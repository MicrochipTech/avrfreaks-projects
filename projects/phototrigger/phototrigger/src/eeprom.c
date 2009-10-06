/*
 * eeprom.c
 *
 *  Created on: 2009-09-03
 *      Author: liku
 */

#include <stdint.h>

#include <avr/eeprom.h>

#include "rtc.h"

uint32_t eemagic         EEMEM = 1885893746l; // "phtr"

uint8_t  eecontr         EEMEM = 255;
uint16_t eebacklight     EEMEM = 512;

int32_t  eetltotal       EEMEM = 100; // number of photos in time lapse mode
uint16_t eetlshutreltime EEMEM = 150; // shutter release time
tm eetlbulb   EEMEM = {.tm_hour = 0, .tm_min = 0, .tm_sec = 0}; // bulb time in time lapse, by default no bulb
tm eetlperiod EEMEM = {.tm_hour = 0, .tm_min = 1, .tm_sec = 0}; // time between photos, by default 10 seconds

tm eebbulb    EEMEM = {.tm_hour = 0, .tm_min = 1, .tm_sec = 0}; // bulb time in bulb, by default 10 seconds
