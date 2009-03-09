/*************************************************************//**
* \file utils.c
* 
* Random, useful utilities
*
* Contains a random collection of utilities that I have found
* useful.
*
* \author Stefan Filipek
* \date March 2009
*
******************************************************************/

#ifndef _UTILS_H_
#define _UTILS_H_ 1

#include <stdint.h>

void print_hex(void *, uint16_t size);
void print_block(void *, uint32_t index, uint16_t size, uint16_t width);

#endif
