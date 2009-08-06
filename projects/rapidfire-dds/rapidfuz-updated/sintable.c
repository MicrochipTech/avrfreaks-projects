/* Justin Goff */
/* Copyright (C) 2008 Justin Goff */
/* stonefoz@myfastmail.com */
/* vim:set ts=4:sw=4:sts=4:expandtab:cindent: */

#include "sintable.h"

#if SINTABSIZE == 32

/* 32 entry sin table  8 bit, 0-255 */
const uint8_t sintable[] PROGMEM =
{
    139, 164, 187, 208,
    225, 239, 253, 253,
    253, 253, 239, 225,
    208, 187, 164, 139,
    115,  90,  67,  46,
    29,    5,   5,   1,
    1,     5,   5,  29,
    46,  67,   90, 115
};

#elif SINTABSIZE == 64

/* 64 entry sin table  8 bit, 0-255 */
const uint8_t sintable[] PROGMEM =
{
127, 139, 152, 164, 176, 187, 198, 208,
217, 225, 233, 239, 244, 249, 252, 253,
254, 253, 252, 249, 244, 239, 233, 225,
217, 208, 198, 187, 176, 164, 152, 139,
127, 115, 102,  90,  78,  67,  56,  46,
 37,  29,  21,  15,  10,   5,   2,   1,
  0,   1,   2,   5,  10,  15,  21,  29,
 37,  46,  56,  67,  78,  90, 102, 115
};

#else
#error SINTABSIZE undefined
#endif

