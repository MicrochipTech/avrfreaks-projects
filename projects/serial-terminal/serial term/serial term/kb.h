// Keyboard communication routines

#ifndef __KB_INCLUDED
#define __KB_INCLUDED

#include <90s8515.h>
#include "pindefs.h"

#define ISC00 0
#define ISC01 1

#define back PORTD.5

void init_kb(void); 
interrupt [EXT_INT0] void ext_int0_isr(void);
void decode(unsigned char sc);
void put_kbbuff(unsigned char c);
int getchar_kb(void);
#endif

