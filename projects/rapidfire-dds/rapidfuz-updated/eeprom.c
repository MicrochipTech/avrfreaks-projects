/* Justin Goff */
/* Copyright (C) 2008 Justin Goff */
/* stonefoz@myfastmail.com */
/* vim:set ts=4:sw=4:sts=4:expandtab:cindent: */

#include "config.h"
#include <avr/eeprom.h>
#include "globals.h"
#include "eeprom.h"

EEMEM eeprom_record_s 
eeprom_records[(EEPROMSIZE/(sizeof(eeprom_record_s)))];

uint8_t eeprompos;

/*
 * Find current eeprom position
 */
void eeprominit(void)
{
    uint8_t count;
    for(
        eeprompos=0;
        eeprompos++, count=eeprom_read_byte(&eeprom_records[eeprompos].index);
        eeprompos<EERECS
    );
}

/*
 * Load settings into running mode
 */
void eepromload(uint8_t mode)
{
}

/*
 * Store running mode into eprom
 */

void eepromstore(uint8_t mode)
{
}

