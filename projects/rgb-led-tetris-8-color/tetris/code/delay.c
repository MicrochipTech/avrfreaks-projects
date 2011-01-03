// delay.c
// for NerdKits with ATmega168, 14.7456 MHz clock
// mrobbins@mit.edu

#include <inttypes.h>
#include "delay.h"

// delay_us(...):
// delays a given number of microseconds
inline void delay_us(long us) {
  //_delay_us(us);
  long i;
  for(i=0; i<us; i++) {
    NOP;	// two is right for 8MHz, tested by mikey and robtruax
    NOP;
    
    NOP;	// so adding 7 more will yield a slightly slow clock (1.017us)
    NOP;
    NOP;
    NOP;
    NOP;
    NOP;
    NOP;
  }
}

// delay_ms(...):
// delays a given number of milliseconds
void delay_ms(int ms) {
  //_delay_ms(ms);
  int i;
  for(i=0; i<ms; i++)
    delay_us(1000);
}
