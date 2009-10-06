/*
 * eeprom.h
 *
 *  Created on: 2009-09-03
 *      Author: liku
 *
 *  This file contains locations for storing variables which should be
 *  persistent over CPU poweroffs.
 *
 */

#ifndef EEPROM_H_
#define EEPROM_H_

#include <stdint.h>

#include <avr/eeprom.h>

#include "rtc.h"

/* fix for eclipse - remove for final version */
#undef EEMEM
#define EEMEM __attribute__((section(".eeprom")))

extern uint32_t eemagic EEMEM; // "phtr"
extern uint16_t eebacklight EEMEM;
extern uint8_t eecontr EEMEM;
extern uint16_t eetltotal EEMEM; // number of photos in time lapse mode
extern uint16_t eetlshutreltime EEMEM; // shutter release time
extern tm eetlbulb EEMEM; // bulb time in time lapse, by default no bulb
extern tm eetlperiod EEMEM; // time between photos, by default 10 seconds
extern tm eebbulb EEMEM; // bulb time in bulb, by default 10 seconds

#endif /* EEPROM_H_ */
