/* Justin Goff */
/* Copyright (C) 2008 Justin Goff */
/* stonefoz@myfastmail.com */
/* vim:set ts=4:sw=4:sts=4:expandtab:cindent: */

#ifndef _GLOBALS_H_
#define _GLOBALS_H_

#include <avr/eeprom.h>

/* Globals */
#define CTL 50000
#define MODES 5
#define BLINKDELAY 100

/* ISR flags */

#define I_SOFTTICK 0 
#define I_ZEROC	   1 	

/* 
 * not volitile 
 * do not access from outside of ISR 
 */ 

extern union pax_u
{
    uint32_t acc;
    uint8_t b[2];
}  pax;

/* 
 * global only to speed up ISR 
 * Do not access from outside of ISR
 */
extern uint32_t pax_next;
/*
 * Rate of fire in divided timer clocks 
 * and zero cross count 
 */
volatile struct ratectl_s
{
    uint32_t step;
    uint8_t count;
} ratectl;

/* 
 * buttons 
 */
#define B_MODE 	0
#define B_LTRIG 1
#define B_RTRIG 2
#define B_MAC0	3
#define B_MAC1	4
#define B_MAC2	5
#define B_MAC3	6
#define B_MAC4	7

extern struct buttons_s
{
    uint8_t flags;
} buttons;

/*
 * Global flags
 */
extern volatile struct isrflags_s
{
    uint8_t flags;
} isrflags;

#endif

