#include <avr/io.h>
#include "event.h"
#include "hardware.h"
#include "buttons.h"

uint8_t buttonsPress[BUTTONS_NB];

void buttonsDebounce(void)
{
	uint8_t buttonsPortValue, i;
	
	buttonsPortValue = BUTTONS_PORT & BUTTONS_MASK;
	for (i=0; i<BUTTONS_NB; i++)
	{
		if (buttonsPortValue & _BV(i + BUTTONS_FIRST)) 
		{
			buttonsPress[i] = 0;
		}
		else 
		{
			buttonsPress[i]++;
			if (buttonsPress[i] == BUTTONS_FAST_START + BUTTONS_FAST_PERIOD) buttonsPress[i] = BUTTONS_FAST_START;
		}
	}
	if (buttonsPortValue == 0) return;
	
	if (buttonsPress[BUTTONS_DOWN] == BUTTONS_VALID) eventAddEvent(EVENT_BUTTON_DOWN);
	if (buttonsPress[BUTTONS_UP] == BUTTONS_VALID) eventAddEvent(EVENT_BUTTON_UP);
	if (buttonsPress[BUTTONS_ENTER] == BUTTONS_VALID) eventAddEvent(EVENT_BUTTON_ENTER);
	if (buttonsPress[BUTTONS_ESC] == BUTTONS_VALID) eventAddEvent(EVENT_BUTTON_ESC);
	if (buttonsPress[BUTTONS_UP] >= BUTTONS_FAST_START)
	{
		if (buttonsPress[BUTTONS_UP] == BUTTONS_FAST_START) eventAddEvent(EVENT_BUTTON_UP);
	}
	else if ((buttonsPress[BUTTONS_UP] >= BUTTONS_REPEAT_START) && !(buttonsPress[BUTTONS_UP]%BUTTONS_REPEAT_PERIOD))
		eventAddEvent(EVENT_BUTTON_UP);
	if (buttonsPress[BUTTONS_DOWN] >= BUTTONS_FAST_START)
	{
		if (buttonsPress[BUTTONS_DOWN] == BUTTONS_FAST_START) eventAddEvent(EVENT_BUTTON_DOWN);
	}
	else if ((buttonsPress[BUTTONS_DOWN] >= BUTTONS_REPEAT_START) && !(buttonsPress[BUTTONS_DOWN]%BUTTONS_REPEAT_PERIOD))
		eventAddEvent(EVENT_BUTTON_DOWN);
}

void buttonsInit(void)
{
	BUTTONS_PORT_DDR &= ~BUTTONS_MASK;
	BUTTONS_PORT_PULLUPS = BUTTONS_MASK;
	eventRegisterHandler(buttonsDebounce, BUTTONS_DEBOUNCE_PERIOD);
}
