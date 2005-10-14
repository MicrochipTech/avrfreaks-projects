
#ifndef INCLUDE_PCSerialIO
#define INCLUDE_PCSerialIO

#include <inttypes.h>
#include <avr\io.h>

void    PCSIOInitSerialIO(void);
int8_t  PCSIOPutChar(char d);
uint8_t PCSIODataAvailable(void);
char 	PCSIOGetChar(void);


void    PCSIOPutNum(int32_t value);
void    PCSIOPutStr(const char *str);
void 	PCSIOPutStrNum_P(const char *progmemPtr, int32_t n);
void 	PCSIOPutStr_P(const char *progmemPtr);
void    PCSIOGetStr_P(char *buffer, const unsigned char *EOLTerminators, uint8_t maxlen);
int16_t PCSIOGetInt(void);
int32_t PCSIOGetLong(void);

//write char to serial out. No wait/check to see if last char sent.
//caller responsible for managing this.

static inline void PCSIOPutCharNoWait(char c)	{ UDR0 = c;}


#endif //INCLUDE_PCSerialIO