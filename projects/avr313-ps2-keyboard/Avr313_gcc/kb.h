// Keyboard communication routines

#ifndef __KB_INCLUDED
#define __KB_INCLUDED

// Keyboard konnections
#define PIN_KB  PIND
#define PORT_KB PORTD
#define CLOCK   2
#define DATAPIN 3


void init_kb(void);
void decode(unsigned char sc);
void put_kbbuff(unsigned char c);
int  get_kbchar(void);
#endif

