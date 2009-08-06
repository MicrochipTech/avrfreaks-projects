/* Justin Goff */
/* Copyright (C) 2008 Justin Goff */

#ifndef _SINTAB_H_
#define _SINTAB_H_

#include <avr/pgmspace.h>

#define SINTAB2SIZE 5
#define SINTABSIZE (1<<(SINTAB2SIZE))

/* 32 entry sin table  8 bit, 0-255 */
extern const uint8_t sintable[] PROGMEM;

#endif

