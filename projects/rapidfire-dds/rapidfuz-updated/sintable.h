/* Justin Goff */
/* Copyright (C) 2008 Justin Goff */
/* stonefoz@myfastmail.com */
/* vim:set ts=4:sw=4:sts=4:expandtab:cindent: */

#ifndef _SINTAB_H_
#define _SINTAB_H_

#include <avr/pgmspace.h>

#define SINTAB2SIZE 5
#define SINTABSIZE (1<<(SINTAB2SIZE))

/* 32 entry sin table  8 bit, 0-255 */
extern const uint8_t sintable[] PROGMEM;

#endif

