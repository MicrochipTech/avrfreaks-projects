//Serial.h
//CODE
//CODE// Flow control flags

#define FC_DTRDSR 0x01
#define FC_RTSCTS 0x02
#define FC_XONXOFF 0x04

// ascii definitions
#include <stdio.h>
#include <time.h>

//#define VC_EXTRALEAN // Exclude rarely-used stuff from Windows headers
#include <string.h>

#define ASCII_BEL 0x07
#define ASCII_BS 0x08
#define ASCII_LF 0x0A
#define ASCII_CR 0x0D
#define ASCII_XON 0x11
#define ASCII_XOFF 0x13


HANDLE SerialInit(char*, int); 

char SerialGetc(HANDLE*);

void SerialPutc(HANDLE*, char);

