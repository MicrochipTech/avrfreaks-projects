#include "watchdog.h"
//=============================================================================
//
// Initialize watchdog timer
//
void wdInit(void)
{
#if defined(__IOM16_H) || defined(__IOM32_H)
    WDTCR = (7 << WDP0) | (1 << WDE);
#else
    WDTCR = (1 << WDCE) | (1 << WDE);
#endif
    WDTCR = (7 << WDP0) | (1 << WDE);
}

//
// Reset watchdog timer
//
void wdReset(void)
{
   wdt_reset();
}
