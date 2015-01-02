#include <avr/io.h>
#include <stdint.h>
#include <stdio.h>
#include <avr/interrupt.h>
#include <stdbool.h>
#define F_CPU 16000000UL
#include <util/delay.h>
extern bool update;        // update display
extern unsigned long entries;
extern volatile char waitsec;
extern volatile char sec;
extern volatile char min;
extern volatile char hour;
extern volatile char day;
extern volatile unsigned int month;
extern volatile char year;
extern volatile unsigned int displmod;         //counter for display mode
extern volatile char mdays[];
extern volatile char keys;
extern char sample_rate;      // at start up 5 min sample intervall
extern char display1[25];
extern char display2[25];
extern bool LCD(void);
extern volatile bool keyin;
extern void displ_calendar(void);
extern void setFebruary (void);
