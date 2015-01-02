#include <avr/io.h>
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#define maxdevices 20
#define F_CPU 16000000UL
#include <avr/interrupt.h>
extern char sample_rate;      // at start up 5 min sample interval
extern signed long energy;
extern signed long prev_energy;
extern  signed int power;
extern signed long trend_p;
extern signed long trend_n;

extern unsigned long entries;
extern bool LCD (void);

extern char display1[25];
extern char display2[25];
extern char volatile min;
extern char volatile hour;
extern char volatile day;
extern char volatile month;
extern char volatile year;
extern char store[];
extern char volatile waitsec;
extern unsigned int volatile displmod;
extern bool volatile read;
