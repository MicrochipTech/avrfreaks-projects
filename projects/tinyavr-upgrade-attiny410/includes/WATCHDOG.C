//*****************************************************************************
//
//	Watchdog Timer ATtiny10
//
//*****************************************************************************
#include <iotiny10.h>
#include "watchdog.h"


//*****************************************************************************
//	Initialisierung Watchdogtimer
//  Input:	-
//	Output:	-
//*****************************************************************************
void watchdog_set(unsigned char uc_wdt_prescale)
{
   watchdog_reset();
   WDTCR |= (uc_wdt_prescale & 0x27);		   //Vorteiler setzen
}


//*****************************************************************************
//	Einschalten des Watchdogtimers
//  Input:	-
//	Output:	-
//*****************************************************************************
void watchdog_enable(void)
{
   watchdog_reset();
   WDTCR |= 0x08;
}


//*****************************************************************************
//	Ausschalten des Watchdogtimers
//  Input:	-
//	Output:	-
//*****************************************************************************
void watchdog_disable(void)
{
   CCP = 0xd8;    //CPU Change Protection - 4Taktezyklen freigeben, um WDTCR zu aendern
   WDTCR &= 0xF7;
}

	