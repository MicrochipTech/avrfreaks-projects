/* 				
/			PS2_KB.H
/
/		 PS/2 Keyboard driver
/
*/


#include "ps2_low.h"
#include <avr/pgmspace.h>

#define BUFF_SIZE 16


void KbInit(void);
uint8_t KbGetChar(void);
void Decode(uint8_t);

