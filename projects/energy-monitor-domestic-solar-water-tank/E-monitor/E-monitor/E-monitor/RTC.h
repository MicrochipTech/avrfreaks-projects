#include <avr/io.h>
#include <stdint.h>
#include <stdio.h>

#include <stdbool.h>

#include <avr/interrupt.h>
#define fivemin 12
#define F_CPU 16000000UL
#include <util/delay.h>

extern volatile char waitsec;		// second timer for display
extern char display1 [];
extern char display2[];
extern bool LCD(void);				//TWI LCD feed
extern char volatile waitsec;
extern bool singlemode;
