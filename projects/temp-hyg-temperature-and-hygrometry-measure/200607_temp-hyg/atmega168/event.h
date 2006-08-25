#ifndef EVENT_H
#define EVENT_H

#define EVENT_ERROR 0xFF

#define EVENT_MAX_HANDLERS 24
#define EVENT_MAX_EVENTS 128

// 4 top bits define the event type
// 4 bottom bits define the event within that type
#define EVENT_TYPE_MASK		0xF0

#define EVENT_NIL 			0
#define EVENT_TIME			(1 << 4) 
#define EVENT_TIME_10MS		(EVENT_TIME + 1) 
#define EVENT_TIME_50MS		(EVENT_TIME + 2)
#define EVENT_TIME_SEC		(EVENT_TIME + 3)
#define EVENT_TIME_MIN		(EVENT_TIME + 4)
#define EVENT_TIME_HOUR		(EVENT_TIME + 5)
#define EVENT_TIME_DAY		(EVENT_TIME + 6)
#define EVENT_TIME_MONTH	(EVENT_TIME + 7)
#define EVENT_TIME_YEAR		(EVENT_TIME + 8)
#define EVENT_TIME_ADJUST	(EVENT_TIME + 9)
#define EVENT_BUTTON		(2 << 4)
#define EVENT_BUTTON_ENTER	(EVENT_BUTTON + 1)
#define EVENT_BUTTON_ESC	(EVENT_BUTTON + 2)
#define EVENT_BUTTON_UP		(EVENT_BUTTON + 3)
#define EVENT_BUTTON_DOWN	(EVENT_BUTTON + 4)
#define EVENT_BUTTON_NEW	(EVENT_BUTTON + 5)
#define EVENT_BUTTON_RESET	(EVENT_BUTTON + 6)
#define EVENT_ADC_COMPLETE	(3 << 4)
#define EVENT_USART			(4 << 4)
#define EVENT_USART_RX		(EVENT_USART + 1)
#define EVENT_USART_RX_EOL	(EVENT_USART + 2)
#define EVENT_USART_TX		(EVENT_USART + 4)
#define EVENT_SENSOR		(5 << 4)
#define EVENT_APPLICATION	(8 << 4)

extern uint8_t eventCurrentEvent;

#define eventGetCurrentEvent() eventCurrentEvent
void eventInit(void);
uint8_t eventGetEvent(void);
uint8_t eventRegisterHandler(void *, uint8_t);
void eventUnregisterHandler(void *);
void eventLoop(void);
void eventAddEvent(uint8_t);


#endif
