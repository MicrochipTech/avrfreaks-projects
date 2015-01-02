#include <avr/io.h>
#include <stdint.h>
#include <stdio.h>
#include <avr/interrupt.h>
#include <stdbool.h>
#define F_CPU 16000000UL
#include <util\delay.h>
extern unsigned long entries;
extern volatile bool update;        // update display
extern char sample_rate;      // at start up 5 min sample interval
extern void displ_energy(void);
extern void displ_Trend (void);
#define maxdevices 20
extern char store[maxdevices];			// temperature storage buffer
extern volatile bool NBM;
extern volatile bool read;
extern volatile char waitsec;
extern bool singlemode;