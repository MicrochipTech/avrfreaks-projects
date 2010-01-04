//=============================================================================
// Watchdog operation.
//
// $Id: watchdog.h,v 1.4.2.2 2006/07/20 20:08:42 SOY Exp $
//=============================================================================

#ifndef __WATCHDOG_H_INCLUDED__
#define __WATCHDOG_H_INCLUDED__

#include <avr/wdt.h>
//=============================================================================
// Inline function prototypes

//
// Initialize watchdog timer
//
void wdInit(void);
//
// Reset watchdog timer
//
void wdReset(void);
#endif  // __WATCHDOG_H_INCLUDED__
