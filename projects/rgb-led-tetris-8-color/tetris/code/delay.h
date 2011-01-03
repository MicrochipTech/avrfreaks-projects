#ifndef __DELAY_H
#define __DELAY_H

#include <inttypes.h>

#ifndef NOP
#define NOP __asm__ __volatile__ ("nop")
#endif

void delay_us(long us); // micro-seconds
void delay_ms(int ms);  // mili-seconds

#endif
