//*****************************************************************************
//
// Watchdog Timer ATtiny10	
//
//*****************************************************************************
#ifndef _WATCHDOG_H_
#define _WATCHDOG_H_

#define watchdog_reset()   asm("WDR");

void watchdog_set(unsigned char uc_wdt_prescale);
void watchdog_enable(void);
void watchdog_disable(void);


#endif

	