//Datei: display.h
//Hardwaredefinitionen
//
// 03.01.2003
// Compiler AVR-GCC 3.2

#ifndef DISPLAY_H
#define DISPLAY_H

#define NOP() asm volatile ("nop" ::)

//Die beiden LED's
#define RED_ON() { cbi(PORTD,7); }
#define RED_OFF() { sbi(PORTD,7); }

#define GREEN_ON() { cbi(PORTD,6); }
#define GREEN_OFF() { sbi(PORTD,6); }

#endif
