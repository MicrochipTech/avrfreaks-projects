/* copyright
 * (c) 2004 Stephen Humble stephenhumble at telstra.com
 *
 *  This file is part of robotx see http://Robotx.sourceforge.net
 *  dual sensorless motor driver firmware.
 *
 *  This is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
*/
#include <io.h>
#include <avr/io.h>
#include <inttypes.h>
#include <stdlib.h>
#include <stdarg.h>
#include <math.h>
#include <avr/timer.h>
#include <avr/interrupt.h>
#include <avr/signal.h>

#ifndef AVRFAIL
#define AVRFAIL

#define R0      _SFR_IO8(0x40)
#define R1      _SFR_IO8(0x41)
#define R2      _SFR_IO8(0x42)
#define R3      _SFR_IO8(0x43)
#define R4      _SFR_IO8(0x44)
#define R5      _SFR_IO8(0x45)
#define R6      _SFR_IO8(0x46)
#define R7      _SFR_IO8(0x47)
#define R8      _SFR_IO8(0x48)
#define R9      _SFR_IO8(0x49)
#define R10      _SFR_IO8(0x4a)
#define R11      _SFR_IO8(0x4b)
#define R12      _SFR_IO8(0x4c)
#define R13      _SFR_IO8(0x4d)
#define R14      _SFR_IO8(0x4e)
#define R15      _SFR_IO8(0x4f)
#define R16      _SFR_IO8(0x50)
#define R17      _SFR_IO8(0x51)
#define R18      _SFR_IO8(0x52)
#define R19      _SFR_IO8(0x53)
#define R20      _SFR_IO8(0x54)
#define R21      _SFR_IO8(0x55)
#define R22      _SFR_IO8(0x56)
#define R23      _SFR_IO8(0x57)
#define R24      _SFR_IO8(0x58)
#define R25      _SFR_IO8(0x59)
#define R26      _SFR_IO8(0x5a)
#define R27      _SFR_IO8(0x5b)
#define R28      _SFR_IO8(0x5c)
#define R29      _SFR_IO8(0x5d)
#define R30      _SFR_IO8(0x5e)
#define R31      _SFR_IO8(0x5f)

#define RX      _SFR_IO16(0x5a)
#define RY      _SFR_IO16(0x5c)
#define RZ      _SFR_IO16(0x5e)

void fail (uint8_t code);
void dump();
void mem();
#endif
