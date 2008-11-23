#ifndef MYFUNCTIONS_H_
#define MYFUNCTIONS_H_

#include <avr32/io.h>
#include "compiler.h"

void delay_ms(unsigned short time_ms);
void parseGPSBuff();
void displayGPS(void);

#endif /*MYFUNCTIONS_H_*/
