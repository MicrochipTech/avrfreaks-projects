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
#include "utils.h"

#include <ctype.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

/**
 * Prints a bunch of hex
 *
 * Prints each byte out in capital hex, zero padded, and
 * separated by spaces. For large amounts of data, see
 * print_block().
 *
 * Relies on avr-libc and proper binding of stdout
 *
 * @param item a pointer to the data to print
 * @param size the number of bytes to print
 */
void
print_hex(void * item, uint16_t size) {

  uint8_t * input;
  uint16_t i;

  input = (uint8_t *)item;

  for(i=0; i<size; i++) {
    printf("%02X ", input[i]);
  }

}

/**
 * Prints a block of hex in a pretty way
 *
 * Prints a block of hex out along with the associated address
 * and ASCII characters. Any line that needs padding has byte
 * values represented by "--"
 *
 * Relies on avr-libc and proper binding of stdout
 *
 * @param item a pointer to the data to print
 * @param index an address index to print with the data
 * @param size the number of bytes to print
 * @param width the number of bytes to print on a single line
 */
void
print_block(void * item, uint32_t index, uint16_t size, uint16_t width) {

  uint8_t * input;
  uint16_t i;
  uint16_t j;

  input = (uint8_t *)item;

  for(i=0; i<size; i+=width, index+=width) {
    printf("0x%08lX:", index);
    printf("   ");

    for(j=0; j<width; j++) {
      if(i+j < size) {
        printf("%02X ", input[i+j]);
      } else {
        printf("-- ");
      }
    }

    printf("   ");

    for(j=0; j<width; j++) {
      if(i+j < size) {
        printf("%c", isprint(input[i+j]) ? input[i+j] : '.');
      } else {
        printf(".");
      }
    }
    printf("\n");
    fflush(stdout); // Null op for avr-libc, included for copy/paste compatibility
  }

}
