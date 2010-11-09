/**
 * @file pal_types.h
 *
 * @brief Definition of supported PAL types
 *
 * This header file contains the supported PAL types.
 *
 * $Id: pal_types.h 19915 2010-01-14 09:29:08Z sschneid $
 *
 * @author    Atmel Corporation: http://www.atmel.com
 * @author    Support email: avr@atmel.com
 */
/*
 * Copyright (c) 2009, Atmel Corporation All rights reserved.
 *
 * Licensed under Atmel's Limited License Agreement --> EULA.txt
 */
 // 9/12/2010  Modified for Teensy2++  -jkl
 // 9/18/2010  Modified for Teensy2  -jkl

/* Prevent double inclusion */
#ifndef PAL_TYPES_H
#define PAL_TYPES_H

/* PAL_GENERIC_TYPE */
#define AVR                             (0x01)
#define MEGA_RF                         (0x02)
#define XMEGA                           (0x03)
#define AVR32                           (0x04)
#define ARM7                            (0x05)

#if (PAL_GENERIC_TYPE == AVR)
    /* PAL_TYPE for AVR 8-bit MCUs */
    #define AT90USB1287                 (0x01)
    #define ATMEGA1281                  (0x02)
    #define ATMEGA1284P                 (0x03)
    #define ATMEGA2561                  (0x04)
    #define ATMEGA644P                  (0x05)
	#define AT90USB1286                 (0x06)		// Added 9/12/2010
	#define ATMEGA32U4                  (0x07)		// Added 9/18/2010

#elif (PAL_GENERIC_TYPE == XMEGA)
    /* PAL_TYPE for XMEGA MCUs */
    #define ATXMEGA128A1                (0x01)
    #define ATXMEGA256A3                (0x02)

#elif (PAL_GENERIC_TYPE == AVR32)
    /* PAL_TYPE for AVR 32-bit MCUs */

#elif (PAL_GENERIC_TYPE == ARM7)
    /* PAL_TYPE for ARM7 MCUs */

    /* AT91SAM7X256 and AT91SAM7CX256:
     *
     * AT91SAM7CX256 is the same as AT91SAM7X256 except hardware AES engine.
     * If this boardtype is used, the file are actually taken from the
     * directory PAL/ARM7/AT91SAM7X256.
     * Only in case security shall be used, separate security files are selected
     * based on this PAL type.
     */
    #define AT91SAM7X256                (0x01)
    #define AT91SAM7XC256               (0x02)

#elif (PAL_GENERIC_TYPE == MEGA_RF)
    /* PAL_TYPE for MEGA-RF single chips (MCU plus transceiver) */
    #define ATMEGA128RFA1               (0x01)

#else
    #error "Undefined PAL_GENERIC_TYPE"
#endif

/* Depending on the generic device type include platform-dependend types (IAR, GCC) */
#if ((PAL_GENERIC_TYPE == AVR) || (PAL_GENERIC_TYPE == XMEGA) || (PAL_GENERIC_TYPE == MEGA_RF))
#include "avrtypes.h"
#elif (PAL_GENERIC_TYPE == ARM7)
#include "armtypes.h"
#else
#error "Unknown PAL_GENERIC_TYPE"
#endif

#endif  /* PAL_TYPES_H */
/* EOF */
