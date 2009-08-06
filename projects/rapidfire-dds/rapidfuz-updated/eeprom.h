/* Justin Goff */
/* Copyright (C) 2008 Justin Goff */
/* stonefoz@myfastmail.com */
/* vim:set ts=4:sw=4:sts=4:expandtab:cindent: */

#ifndef _EEPROM_H_
#define _EEPROM_H_

#include "config.h"
#include <avr/eeprom.h>
#include "globals.h"


/*
 * individual mode
 */
typedef struct 
{
    uint8_t lburst;
    uint8_t rburst;
    uint16_t rate;
} mode_s;


/*
 * eeprom records
 * contains all modes and a sentinal index to find current place
 */
typedef struct 
{
    uint8_t index;
    mode_s modes[MODES];
} eeprom_record_s;

#define EERECS (EEPROMSIZE/sizeof(eeprom_record_s))

/*
 * sizeof isn't  evaluated until after macros... duh?? should find some
 * way to check size
 *
#if EERECS == 0
#   error "EEPROM record to large for device"
#endif
*/

extern EEMEM eeprom_record_s 
eeprom_records[(EEPROMSIZE/(sizeof(eeprom_record_s)))];

extern uint8_t eeprompos;

/*
 * Find current eeprom position
 */
extern void eeprominit(void);

/*
 * Load settings into running mode
 */
extern void eepromload(uint8_t mode);

/*
 * Store running mode into eprom
 */
extern void eepromstore(uint8_t mode);

#endif
