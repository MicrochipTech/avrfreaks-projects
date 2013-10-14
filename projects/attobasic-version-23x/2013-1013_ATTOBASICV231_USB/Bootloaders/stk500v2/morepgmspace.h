/* Copyright (c) 2006, Carlos Lamas
   Copyright (c) 2009-2010, Jan Waclawek

   All rights reserved.

   Source to be appended to <avr/pgmspace.h> someday?

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions are met:

   * Redistributions of source code must retain the above copyright
     notice, this list of conditions and the following disclaimer.
   * Redistributions in binary form must reproduce the above copyright
     notice, this list of conditions and the following disclaimer in
     the documentation and/or other materials provided with the
     distribution.
   * Neither the name of the copyright holders nor the names of
     contributors may be used to endorse or promote products derived
     from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
  POSSIBILITY OF SUCH DAMAGE. */

#ifndef MOREPGMSPACE_H
#define MOREPGMSPACE_H  1

#define __need_size_t
#include <stddef.h>
#include <inttypes.h>  // stdint.h is the standard header, but uint_farptr_t is defined in inttypes.h
#include <stdint.h>
#include <avr/pgmspace.h>


#ifndef __ATTR_CONST__
#define __ATTR_CONST__ __attribute__((__const__))
#endif

#ifndef __ATTR_PROGMEM__
#define __ATTR_PROGMEM__ __attribute__((__progmem__))
#endif

#ifndef __ATTR_PURE__
#define __ATTR_PURE__ __attribute__((__pure__))
#endif

/* PROGMEM_FAR macro
*/

#ifndef PROGMEM_FAR
#define PROGMEM_FAR __attribute__((__section__("._progmem_far")))
#endif

#ifndef PROGMEM_SEG1
#define PROGMEM_SEG1 __attribute__((__section__("._progmem_segment1")))
#endif

#ifndef PROGMEM_SEG2
#define PROGMEM_SEG2 __attribute__((__section__("._progmem_segment2")))
#endif

#ifndef PROGMEM_SEG3
#define PROGMEM_SEG3 __attribute__((__section__("._progmem_segment3")))
#endif

#define PROGMEM_SEG1_BASE 0x10000UL
#define PROGMEM_SEG2_BASE 0x20000UL
#define PROGMEM_SEG3_BASE 0x30000UL

/* PFSTR() macro


*/

# define PFSTR(s) (__extension__({static char __c[] PROGMEM_FAR = (s); GET_FAR_ADDRESS(__c[0]);}))
# define PS1STR(s) (__extension__({static char __c[] PROGMEM_SEG1 = (s); PROGMEM_SEG1_BASE + (uint16_t)&__c[0];}))
# define PS2STR(s) (__extension__({static char __c[] PROGMEM_SEG2 = (s); PROGMEM_SEG2_BASE + (uint16_t)&__c[0];}))
# define PS3STR(s) (__extension__({static char __c[] PROGMEM_SEG3 = (s); PROGMEM_SEG3_BASE + (uint16_t)&__c[0];}))



/* GET_FAR_ADDRESS() macro

   This macro facilitates the obtention of a 32 bit "far" pointer (only 24 bits
   used) to data even passed the 64KB limit for the 16 bit ordinary pointer. It
   is similar to the '&' operator, with some limitations.

   Comments:

   - The overhead is minimal and it's mainly due to the 32 bit size operation.

   - 24 bit sizes guarantees the code compatibility for use in future devices.

   - hh8() is an undocumented feature but seems to give the third significant byte
     of a 32 bit data and accepts symbols, complementing the functionality of hi8()
     and lo8(). There is not an equivalent assembler function to get the high
     significant byte.

   - 'var' has to be resolved at linking time as an existing symbol, i.e, a simple
     type variable name, an array name (not an indexed element of the array, if the
     index is a constant the compiler does not complain but fails to get the address
     if optimization is enabled), a struct name or a struct field name, a function
     identifier, a linker defined identifier,...

   - The natural place for this macro should be the header avr/pgmspace.h and the
     name... pgm_get_far_address?

   - The returned value is the identifier's VMA (virtual memory address) determined
     by the linker and falls in the corresponding memory region. The AVR Harvard
     architecture requires non overlapping VMA areas for the multiple address spaces
     in the processor: Flash ROM, RAM, and EEPROM. Typical offset for this are
     0x00000000, 0x00800xx0, and 0x00810000 respectively, derived from the linker
   script used and linker options. The value returned can be seen then as a
     universal pointer.

*/

#define GET_FAR_ADDRESS(var)                          \
({                                                    \
  uint_farptr_t tmp;                                \
                                                      \
  __asm__ __volatile__(                             \
                                                      \
      "ldi  %A0, lo8(%1)"           "\n\t"    \
      "ldi  %B0, hi8(%1)"           "\n\t"    \
      "ldi  %C0, hh8(%1)"           "\n\t"    \
      "clr  %D0"                    "\n\t"    \
    :                                             \
      "=d" (tmp)                                \
    :                                             \
      "p"  (&(var))                             \
  );                                                \
  tmp;                                              \
})


// for segmented access, pointers are 16-bit
typedef uint16_t uint_segptr_t;


#if defined (__AVR_HAVE_LPMX__)

  #define pgm_read_byte_seg1(addr)          \
  (__extension__({                          \
      uint16_t __addr16 = (uint16_t)(addr); \
      uint8_t __result;                     \
      __asm__                               \
      (                                     \
          "ldi r30, 1"   "\n\t"             \
          "out %2, r30"  "\n\t"             \
          "movw r30, %1" "\n\t"             \
          "elpm %0, Z+"  "\n\t"             \
          : "=r" (__result)                 \
          : "r" (__addr16),                 \
            "I" (_SFR_IO_ADDR(RAMPZ))       \
          : "r30", "r31"                    \
      );                                    \
      __result;                             \
  }))


  #define pgm_read_word_seg1(addr)          \
  (__extension__({                          \
      uint16_t __addr16 = (uint16_t)(addr); \
      uint16_t __result;                    \
      __asm__                               \
      (                                     \
          "ldi r30, 1"    "\n\t"            \
          "out %2, r30"   "\n\t"            \
          "movw r30, %1"  "\n\t"            \
          "elpm %A0, Z+"  "\n\t"            \
          "elpm %B0, Z"   "\n\t"            \
          : "=r" (__result)                 \
          : "r" (__addr16),                 \
            "I" (_SFR_IO_ADDR(RAMPZ))       \
          : "r30", "r31"                    \
      );                                    \
      __result;                             \
  }))


  #define pgm_read_dword_seg1(addr)         \
  (__extension__({                          \
      uint16_t __addr16 = (uint16_t)(addr); \
      uint32_t __result;                    \
      __asm__                               \
      (                                     \
          "ldi r30, 1"    "\n\t"            \
          "out %2,  r30"  "\n\t"            \
          "movw r30, %1"  "\n\t"            \
          "elpm %A0, Z+"  "\n\t"            \
          "elpm %B0, Z+"  "\n\t"            \
          "elpm %C0, Z+"  "\n\t"            \
          "elpm %D0, Z"   "\n\t"            \
          : "=r" (__result)                 \
          : "r" (__addr16),                 \
            "I" (_SFR_IO_ADDR(RAMPZ))       \
          : "r30", "r31"                    \
      );                                    \
      __result;                             \
  }))


#else
  #error "No pgm_seg support for ATM103"
#endif




#ifdef __cplusplus
extern "C" {
#endif


extern size_t strlen_PF (uint_farptr_t src) __ATTR_CONST__; /* program memory can't change */
extern size_t strnlen_PF (uint_farptr_t src, size_t len) __ATTR_CONST__; /* program memory can't change */
extern void *memcpy_PF (void *dest, uint_farptr_t src, size_t len);
extern int memcmp_PF(const void *, uint_farptr_t, size_t) __ATTR_PURE__;
extern char *strcpy_PF (char *dest, uint_farptr_t src);
extern char *strncpy_PF (char *dest, uint_farptr_t src, size_t len);
extern char *strcat_PF (char *dest, uint_farptr_t src);
extern size_t strlcat_PF (char *dst, uint_farptr_t src, size_t siz);
extern char *strncat_PF (char *dest, uint_farptr_t src, size_t len);
extern int strcmp_PF (const char *s1, uint_farptr_t s2) __ATTR_PURE__;
extern int strncmp_PF (const char *s1, uint_farptr_t s2, size_t n) __ATTR_PURE__;
extern int strcasecmp_PF (const char *s1, uint_farptr_t s2) __ATTR_PURE__;
extern int strncasecmp_PF (const char *s1, uint_farptr_t s2, size_t n) __ATTR_PURE__;
extern char *strstr_PF (const char *s1, uint_farptr_t s2);
extern size_t strlcpy_PF (char *dst, uint_farptr_t src, size_t siz);


#ifdef __cplusplus
}
#endif


#endif     /* MOREPGMSPACE_H */
