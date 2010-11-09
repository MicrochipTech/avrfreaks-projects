/**
 * @file pal_boardtypes.h
 *
 * @brief PAL board types for AT90USB1286
 *
 * This header file contains board types based on AT90USB1286.
 *
 * $Id: pal_boardtypes.h 16325 2009-06-23 16:40:23Z sschneid $
 *
 * @author    Atmel Corporation: http://www.atmel.com
 * @author    Support email: avr@atmel.com
 */
/*
 * Copyright (c) 2009, Atmel Corporation All rights reserved.
 *
 * Licensed under Atmel's Limited License Agreement --> EULA.txt
 */
 // 9/18/2010  MOdified to create board type T2 for the Teensy2 -jkl

/* Prevent double inclusion */
#ifndef PAL_BOARDTYPES_H
#define PAL_BOARDTYPES_H

/* Boards for AT86RF230B */
#define T2PP                    (0x01)		// need both names for conditional compilation
#define T2                    (0x02)		// 

#endif  /* PAL_BOARDTYPES_H */
/* EOF */
