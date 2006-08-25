#include <avr/sleep.h>
#include <string.h>

#include "utils.h"
#include "event.h"

typedef struct 
{
	void (*handler)(void);
	uint8_t event;
}
eventHandler_t;

volatile uint8_t eventQueueData[EVENT_MAX_EVENTS];
volatile fifo_t eventQueue;

eventHandler_t eventHandler[EVENT_MAX_HANDLERS];
uint8_t eventLastHandler;
uint8_t eventCurrentEvent;

void eventInit(void)
{
	fifoInit(&eventQueue, eventQueueData, EVENT_MAX_EVENTS);
	eventLastHandler = 0;
	eventCurrentEvent = EVENT_NIL;
	set_sleep_mode(SLEEP_MODE_IDLE);
}

uint8_t eventRegisterHandler(void *eventHandlerFunction, uint8_t event)
{
	if (eventLastHandler == EVENT_MAX_HANDLERS) return EVENT_ERROR;
	eventHandler[eventLastHandler].handler = eventHandlerFunction;
	eventHandler[eventLastHandler].event =  event;
	eventLastHandler ++;
	return eventLastHandler-1;
}

/* ---------------- NOT REQUIRED IN THIS SETUP -----------------------------
void eventUnregisterHandler(void *eventHandlerFunction)
{
	uint8_t i;
	
	for (i=0; i<eventLastHandler; i++) 
	{
		if (eventHandler[i].handler == eventHandlerFunction)
		{
			eventLastHandler --;
			for (; i<eventLastHandler; i++) 
				memcpy(&(eventHandler[i]), &(eventHandler[i+1]), sizeof(eventHandler_t));
			return;
		}
	}
}
*/

uint8_t eventGetEvent(void)
{
	if (!fifoHasData(&eventQueue)) return EVENT_NIL;
	return fifoGetByte(&eventQueue);
}

void eventLoop()
{
	uint8_t i;
	
	while (1)
	{
		eventCurrentEvent = eventGetEvent();
		if (eventCurrentEvent == EVENT_NIL) 
		{
			sleep_mode();
		}
		else 
		{
			for (i=0; i<eventLastHandler; i++)
			{
				if ((eventHandler[i].event == eventCurrentEvent) || 
					(eventHandler[i].event == (eventCurrentEvent & EVENT_TYPE_MASK))) 
					eventHandler[i].handler();
			}
		}
	}
}

void eventAddEvent(uint8_t event)
{
	fifoAddByte(&eventQueue, event);
}
