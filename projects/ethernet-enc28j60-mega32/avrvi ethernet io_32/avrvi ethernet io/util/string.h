#ifndef _STRING_H_
#define _STRING_H_
#include <stdio.h>
#include <avr/pgmspace.h>
#include "../io/softuart.h"

int string_search_progmem(PGM_P pointer, unsigned char *buffer);
#define string_compare_progmem(s,c) string_compare_progmem2(PSTR((s)), (c), 1)
#define string_compare_progmem_noeof(s,c) string_compare_progmem2(PSTR((s)), (c), 0)
#define string_compare_progmem_plen(s,c) string_compare_progmem_plen2(PSTR((s)),sizeof((s))-1, (c))
char string_compare_progmem2(PGM_P pointer, unsigned char *string, unsigned char check_eof);
char string_compare_progmem_plen2(PGM_P pointer, unsigned int plen, unsigned char *string);
void string_uint16_to_buffer(unsigned char *buffer, unsigned int val);
unsigned int string_buffer_to_uint16(unsigned char *buffer);
void string_uint8_to_buffer(unsigned char *buffer, unsigned char val);
void string_uint8_to_buffer_02dez(unsigned char *buffer, unsigned char val);
unsigned long string_buffer_to_uint32(unsigned char *buffer);
void string_progmem_to_buffer(PGM_P pointer, unsigned char *buffer, unsigned int len);
#endif
