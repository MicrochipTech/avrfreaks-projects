#include <avr/io.h>
#include <stdint.h>
#include <stdio.h>
#include <avr/interrupt.h>
#include <stdbool.h>

#define F_CPU 16000000UL
#include <avr/pgmspace.h>
#include <util\delay.h>
#define maxdevices  20
#define offset 13954	//800*15*4186*2.778/10000      //13.954 KWh by 15°C

extern unsigned long entries;
extern char store[];  // 8bit orig format without 9th sign bit
extern volatile char waitsec;
extern char resetL(void);
extern bool LCD(void);
extern volatile bool update;
extern unsigned int volatile displmod;
extern char display1[25];				//LCD outp buffer
extern char display2[25];