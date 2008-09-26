#ifndef AESGLOBAL_H
#define AESGLOBAL_H

#include "bootldr.h"

//=============================================================================
// General purpose i/o macros
//=============================================================================

#define set_bit(PORT, BITNUM)    ((PORT) |=  (1<<(BITNUM)))
#define clear_bit(PORT, BITNUM)  ((PORT) &= ~(1<<(BITNUM)))
#define toggle_bit(PORT, BITNUM) ((PORT) ^=  (1<<(BITNUM)))

//=============================================================================
// Global registers used in aes.c
//=============================================================================

#if KEY_COUNT > 0

register uint8_t *powTbl      asm("r10");           //!< Final location of exponentiation lookup table.
register uint8_t *logTbl      asm("r8");            //!< Final location of logarithm lookup table.
register uint8_t *sBox        asm("r6");            //!< Final location of s-box.
register uint8_t *sBoxInv     asm("r4");            //!< Final location of inverse s-box.
register uint8_t *expandedKey asm("r2");            //!< Final location of expanded key.

#endif

#endif
