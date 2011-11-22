#ifndef LEDS_H
#define LEDS_H
#include "bootloader.h"

#ifdef OVERLAY
#define LED_PORT1 PORTA
#define LED_PORT2 PORTA
#define LED_PORT3 PORTA
#define LED_DDR1 DDRA
#define LED_DDR2 DDRA
#define LED_DDR3 DDRA
#define LED3 0x80
#define LED2 0x40
#define LED1 0x20
#elif defined PROCESSOR
#define LED_PORT1 PORTD
#define LED_PORT2 PORTG
#define LED_PORT3 PORTG
#define LED_DDR1 DDRD
#define LED_DDR2 DDRG
#define LED_DDR3 DDRG
#define LED1 0x80
#define LED2 0x01
#define LED3 0x02
#endif
//void leds_init();

#endif
