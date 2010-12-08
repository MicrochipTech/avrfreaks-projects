#ifndef utils_H
#define utils_H

unsigned int szlen (char *string);
unsigned int szlen_fmp (char *string);
void mcopy (char *dst, char*src, unsigned int len);
void mcopy_fpm (char *dst, char*src, unsigned int len);
char * i2a (unsigned int value);
char * i2azp (unsigned int value, int zpad);
unsigned char bcd2dec (unsigned char bcd);
unsigned char dec2bcd (unsigned char dec);

#endif
