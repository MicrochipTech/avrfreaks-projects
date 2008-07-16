#ifndef _LED_H_
#define _LED_H_
#include "../config.h"

#define led_init() LED_DDR |=  (1<<LED_PIN)
#define LED_STATUS_ON()   LED_PORT |= (1<<LED_PIN)
#define LED_STATUS_OFF()  LED_PORT &= ~(1<<LED_PIN)

#endif

