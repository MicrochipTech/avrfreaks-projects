#include <avr/io.h>
#include <stdint.h>
#include <stdio.h>
#include <avr/interrupt.h>
#include <stdbool.h>
#include <avr/pgmspace.h>
#define F_CPU 16000000UL
#include <util/delay.h>

#define read_scratch 0xbe
#define match 0x55
#define readRom 0x33
#define convert 0x44
#define skipR 0xcc
#define maxdevices 20

extern bool volatile singlemode;
extern bool LCD (void);			//error=true
extern char display1 [25];
extern char display2 [25];
extern volatile char min;
extern volatile char hour;
extern volatile char day;
extern volatile char month;
extern volatile char waitsec;
extern bool volatile NBM;
extern char volatile waitsec;

