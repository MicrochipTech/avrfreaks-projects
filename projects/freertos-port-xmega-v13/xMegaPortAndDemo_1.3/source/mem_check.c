/*
 * mem_check.c
 *
 * Created: 29.07.2011 00:44:34
 *  Author: http://www.rn-wissen.de/index.php/Speicherverbrauch_bestimmen_mit_avr-gcc
 */ 

#include <avr/io.h>  // RAMEND
#include "mem_check.h"

// Mask to init SRAM and check against
#define MASK 0xaa

// From linker script
extern unsigned char __heap_start;

// !!! This doesn't work together with malloc et.al. (whose use is
// !!! discouraged on AVR, anyway). alloca, however, is no problem
// !!! because it allocates on stack.

//  Get minimum of free memory (in bytes) up to now.
unsigned short get_mem_unused (void)
{
   unsigned short unused = 0;
   unsigned char *p = &__heap_start;

   do
   {
      if (*p++ != MASK)
         break;

      unused++;
   } while (p <= (unsigned char*) RAMEND);

   return unused;
}

// !!! Never call this function, it is part of .init-Code
void __attribute__ ((naked, section(".init3"))) init_mem (void);
void init_mem (void)
{
   //  Use inline assembler so it works even with optimization turned off
   __asm volatile (
      "ldi r30, lo8 (__heap_start)"  "\n\t"
      "ldi r31, hi8 (__heap_start)"  "\n\t"
      "ldi r24, %0"                  "\n\t"
      "ldi r25, hi8 (%1)"            "\n"
      "0:"                           "\n\t"
      "st  Z+,  r24"                 "\n\t"
      "cpi r30, lo8 (%1)"            "\n\t"
      "cpc r31, r25"                 "\n\t"
      "brlo 0b"
         :
         : "i" (MASK), "i" (RAMEND+1)
   );
}

//EOF