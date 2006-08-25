#include <avr/interrupt.h>
#include <avr/signal.h>
#include <avr/delay.h>

#include "event.h"
#include "lcd.h"
#include "backlight.h"
#include "timer.h"
#include "clock.h"
#include "shtxx.h"
#include "utils.h"
#include "buttons.h"
#include "screens.h"
#include "db.h"
#include "eeprom.h"

void buttonsHandle(void)
{
	switch (screen)
	{
		case SCREEN_MENU:
			screenMenuHandleButtons();
			break;
		case SCREEN_TIME_AND_DATE:
			screenDateAndTimeHandleButtons();
			break;
		case SCREEN_TEMPERATURE:
		case SCREEN_HUMIDITY:
			screenGraphHandleButtons();
			break;
		default:
			if (eventGetCurrentEvent() == EVENT_BUTTON_ESC)
			{	
				if (screen == SCREEN_HOME) screenMenuDraw();
				else screenHomeDraw();
			}
			break;
	}
}

int main(void)
{ 
	// core initialization
	cli();
	eventInit();
		
	// hardware modules init
	lcdInit();
	backlightInit();
	timerInit();
	shtxxInit();
	buttonsInit();
	eepromInit();
	
	// software modules init
	clockInit();
	dbInit();
	
	// register handlers
	eventRegisterHandler(screenTimeRefresh, EVENT_TIME_SEC);
	eventRegisterHandler(screenSensorsRefresh, EVENT_SENSOR);
	eventRegisterHandler(screenGraphRefresh, EVENT_TIME);
	eventRegisterHandler(buttonsHandle, EVENT_BUTTON);

	screenHomeDraw();
	//screenGraphDraw(SCREEN_TEMPERATURE);
	// screenDateAndTimeDraw();
	
	// main loop
	sei();
	eventLoop();
	
	return 0;
}
