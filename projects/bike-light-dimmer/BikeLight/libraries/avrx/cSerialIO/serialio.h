// serialio.h

#ifndef INCLUDE_SerialIO
#define INCLUDE_SerialIO

#include <inttypes.h>
#ifdef __cplusplus
extern "C" {
#endif

void InitSerialIO(uint8_t ubrr);
int AVRXGetChar(void);
int AVRXPutChar(char);
void AVRXResetSerialInput(void);

int _getcharecho(void);
int _pushchar(unsigned char);
void _putflashstring(const char *);
/*
void PutCR(void);
void PutSpace(void);
unsigned GetHexWord(void);
unsigned GetHexByte(void);
unsigned GetHexChar(void);

int IsHex(char);
int IsWhite(char);

void _PutHex(unsigned, unsigned char);
void PutHexWord(unsigned);
void PutHexByte(unsigned);
*/

#ifdef __cplusplus
}
#endif


#endif //INCLUDE_SerialIO