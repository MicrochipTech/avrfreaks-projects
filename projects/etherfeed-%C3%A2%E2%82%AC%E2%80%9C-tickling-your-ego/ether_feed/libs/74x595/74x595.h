#ifndef _74x595_H_
#define _74x595_H_

#include <stdint.h>
#include "global-conf.h"

#ifndef S74595_0 
#error "74x595.h - pins not defined. Please define in global-conf.h"
#endif

/*
// SER pin 14: set/clear bit on serial data pin
#define S74595_0 PORTC&=~(1<<PORTC5)
#define S74595_1 PORTC|=(1<<PORTC5)
// RCLK pin 12:
#define S74595_RCLKDOWN PORTC&=~(1<<PORTC4)
#define S74595_RCLKUP PORTC|=(1<<PORTC4)
// SRCLK pin 11:
#define S74595_CLOCKDOWN PORTC&=~(1<<PORTC3)
#define S74595_CLOCKUP PORTC|=(1<<PORTC3)
*/

void multi_set74x595(uint8_t *, uint8_t);
void set74x595(uint8_t);
void init74x595(void);


#endif // end ifndef _74x595_H_
