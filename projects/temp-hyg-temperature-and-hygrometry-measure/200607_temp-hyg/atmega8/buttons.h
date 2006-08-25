#ifndef BUTTONS_H
#define BUTTONS_H

#include "event.h"

// debounce period is BUTTONS_VALID * BUTTONS_DEBOUNCE_PERIOD = 200ms
#define BUTTONS_DEBOUNCE_PERIOD		EVENT_TIME_50MS
#define BUTTONS_VALID 				4

// start to repeat the button after BUTTONS_DEBOUNCE_PERIOD * BUTTONS_REPEAT_START = 1.5s
#define BUTTONS_REPEAT_START 		30
// repeat key every BUTTONS_REPEAT_START * BUTTONS_DEBOUNCE_PERIOD = 0.5s
#define BUTTONS_REPEAT_PERIOD 		10

// start to fast repeat the button after BUTTONS_DEBOUNCE_PERIOD * BUTTONS_FAST_START = 4s
#define BUTTONS_FAST_START 			80
// fast repeat key every BUTTONS_FAST_PERIOD * BUTTONS_DEBOUNCE_PERIOD = 0.20s
#define BUTTONS_FAST_PERIOD 		4


void buttonsInit(void);
uint8_t buttonsGet(void);


#endif
