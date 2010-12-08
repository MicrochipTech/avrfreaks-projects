/**
 * utils.c
 *
 * general tools
 */

#include "utils.h"

#include <avr/io.h>
#include <avr/pgmspace.h>

uint8_t bcd2dec (uint8_t bcd)
{
	return (bcd&0x0f)+((bcd>>4)&0x07)*10;
}

uint8_t dec2bcd (uint8_t dec)
{
	return ((dec/10)<<4)+((dec%10)&0x0f);
}

uint16_t szlen (char *string)
{
	uint16_t len=0;
	while (*string++!=0) len++;

	return len;
}

uint16_t szlen_fmp (char *string)
{
	uint16_t len=0;
	while (pgm_read_byte_near(string++)!=0) len++;

	return len;
}

void mcopy (char *dest, char *src, uint16_t size)
{
	while (size--) *dest++=*src++;
}

void mcopy_fpm (char *dest, char *src, uint16_t size)
{
	while (size--) *dest++=pgm_read_byte_near(src++);
}

char i2a_buff[6]={0};

char * i2a (uint16_t value)
{
	uint8_t i=4;

	do {
		i2a_buff[i--]=(value%10)+48;
		value/=10;
	} while (value);

	return &i2a_buff[i+1];
}

char * i2azp (uint16_t value, int zpad)
{
	char *p = i2a(value);

	while (p>=(&i2a_buff[5]-zpad)) *--p='0';

	return ++p;
}
