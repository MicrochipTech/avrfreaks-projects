/*************************************************************************************
 Title:     C include file for the PS/2 library (ps2.c)
 Author:    Jan Pieter de Ruiter <janpieterd@lycos.nl>
 Date:      13-Jun-2002
 Software:  AVR-GCC with AvrStudio
 Target:    Any AVR device

 DESCRIPTION
       Basic routines for implementing the ps/2 protocol (for a mouse, keyboard etc.)

**************************************************************************************/
#ifndef __PS2_H_
#define __PS2_H_ 1

#include <avr/io.h>

#define _BV(bit)   (1 << (bit)) 
#define INB(sfr)   _SFR_BYTE(sfr) 
#define SBI(x,y) (x |= (1<<y)); 				/* set bit y in byte x */ 
#define CBI(x,y) (x &= (~(1<<y))); 				/* clear bit y in byte x */ 
#define BIT_IS_SET(sfr, bit)   (INB(sfr) & _BV(bit)) 
#define BIT_IS_CEAR(sfr, bit)   (!(INB(sfr) & _BV(bit))) 
#define LOOP_UNTIL_BIT_IS_SET(sfr, bit)   do {asm volatile ("nop"::);} while (BIT_IS_CEAR(sfr, bit)) 
#define LOOP_UNTIL_BIT_IS_CLEAR(sfr, bit)   do {asm volatile ("nop"::);} while (BIT_IS_SET(sfr, bit)) 

/* Select ports for the CLK and DATA lines here **************/
#define CLKPORT PORTA  // port for CLK line
#define DATAPORT PORTA // port for DATA line
#define CLK	0		   // pin on CLKPORT for CLK line
#define DATA 1	       // pin on DATAPORT for DATA line

#define CLKDDR DDRA
#define CLKPIN PINA
#define DATADDR	DDRA
#define DATAPIN	PINA
/*******************************************/

void delay(long microseconds);   // Function for internal usage
int Read_ps2data(void);          // Receives a data byte from a device
void Write_ps2data(int data);    // Sends a data byte to a device

#endif
