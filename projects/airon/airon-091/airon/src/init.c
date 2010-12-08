/**
 * init.c
 * System init
 */

#include <avr/io.h>

#include "timing.h"
#include "lcd.h"
#include "rtc.h"
#include "io.h"
#include "adc.h"

#include <avr/pgmspace.h>

void SystemInit (void)
{
	InitTimer2	();

	/** start system ticks, enable global interrupt **/
	SREG |= 0x80;

	/** leave this as first step, to avoid shorts on the lines */
	InitIO    	();

	InitLcd		();
	InitADC		();
	InitRTC		();
}
