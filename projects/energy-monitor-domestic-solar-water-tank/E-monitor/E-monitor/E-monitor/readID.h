#include <avr/io.h>
#include <stdio.h>
#include <stdint.h>
#include <avr/interrupt.h>
#include <stdbool.h>
#define F_CPU 16000000UL
#include <util/delay.h>
#define readRom 0x33
#define match 0x55
#define read_scratch 0xbe
#define skipR 0xcc
#define maxdevices 20


extern void write(char);			// ds1820.c
extern void start (void);			// ds1820.c
extern bool resetL(void);		// ds1820.
extern char display1 [25];				// display.c
extern char display2 [25];
extern bool LCD(void);				// display.c
extern char store[];
extern char read_T (void);
extern void seti(void);
extern bool convert_T(void);
extern bool resetL(void);
extern bool volatile singlemode;
extern char volatile waitsec;
extern  unsigned int volatile displmod;