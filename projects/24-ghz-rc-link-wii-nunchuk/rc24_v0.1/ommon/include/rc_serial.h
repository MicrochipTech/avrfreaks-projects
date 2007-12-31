#ifndef _RC_SERIAL_H_
#define _RC_SERIAL_H_

void 	Serial_Init			(long baud);

void 	Serial_Write		(uint8_t c);
uint8_t Serial_Read			(void);

void 	Serial_PrintDec		(long n);
void 	Serial_PrintHex		(long n);
void 	Serial_PrintStr		(const char *s);

void 	Serial_PrintIntegerInBase(unsigned long n, unsigned long base);

#endif

