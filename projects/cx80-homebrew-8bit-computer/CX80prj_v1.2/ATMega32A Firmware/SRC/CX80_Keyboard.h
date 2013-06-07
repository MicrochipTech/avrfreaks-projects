/*
 * CX80_Keyboard.h
 *
 * Created: 03/04/2013 11:05:44
 *  Author: Calogiuri Enzo Antonio
 */ 


#ifndef CX80_KEYBOARD_H_
#define CX80_KEYBOARD_H_

#include "CX80.h"

extern byte Flags;
extern byte KBScancode;

void CX80_KeyboardInit(void);
byte CX80_GetKey(void);
byte CX80_GetLine(char *buf, byte bufsize);

#endif /* CX80_KEYBOARD_H_ */