#include <avr/io.h>

#include "event.h"
#include "hardware.h"
#include "backlight.h"

#define backlightOn() LCD_BACKLIGHT_PORT |= _BV(LCD_BACKLIGHT)
#define backlightOff() LCD_BACKLIGHT_PORT &= ~_BV(LCD_BACKLIGHT)

uint8_t backlightTimer;

void backlightSetAlight(void)
{
	backlightTimer = 0;
	backlightOn();
}

void backlightTimeout(void)
{
	if (backlightTimer > BACKLIGHT_ALIGHT_PERIOD) return;
	backlightTimer ++;
	if (backlightTimer > BACKLIGHT_ALIGHT_PERIOD) backlightOff();
}

void backlightInit(void)
{
	LCD_BACKLIGHT_DDR |= _BV(LCD_BACKLIGHT);
	backlightSetAlight();
	eventRegisterHandler(backlightSetAlight, EVENT_BUTTON);
	eventRegisterHandler(backlightTimeout, EVENT_TIME_SEC);
}
