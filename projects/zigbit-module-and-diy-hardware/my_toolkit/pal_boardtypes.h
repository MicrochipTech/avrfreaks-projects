/**
 * @file pal_boardtypes.h
 *
 * @brief PAL board types for ATmega1281
 *
 * This header file contains board types based on ATmega1281.
 *
 * $Id: pal_boardtypes.h 24274 2010-11-23 10:11:19Z awachtle $
 *
 * @author    Atmel Corporation: http://www.atmel.com
 * @author    Support email: avr@atmel.com
 */
/*
 * Copyright (c) 2009, Atmel Corporation All rights reserved.
 *
 * Licensed under Atmel's Limited License Agreement --> EULA.txt
 */

/* Prevent double inclusion */
#ifndef PAL_BOARDTYPES_H
#define PAL_BOARDTYPES_H

/* === Includes ============================================================= */

#if defined(VENDOR_BOARDTYPES) && (VENDOR_BOARDTYPES != 0)
#include "vendor_boardtypes.h"
#else   /* Use standard board types as defined below. */

/* === Macros =============================================================== */

/*
 * Boards for AT86RF230A
 */
/* STK541 USB Adapter board with Radio Controller board RCB230 V3.1 */
#define RCB_3_1_230A            (0x01)
/* STK500 & STK501 boards with Radio Extender board REB230A V2.2 */
#define REB_2_2_230A            (0x02)



/*
 * Boards for AT86RF230B
 */
/*
 * RCB Breakout Board with Radio Controller board RCB230 3.2
 * http://www.dresden-elektronik.de/shop/prod85.html
 *
 * RCB Breakout Board Light with Radio Controller board RCB230 3.2
 * http://www.dresden-elektronik.de/shop/prod84.html
 */
#define RCB_3_2_BREAKOUT_BOARD  (0x11)

/*
 * Plain Radio Controller board RCB230 V3.2
 */
#define RCB_3_2_PLAIN           (0x12)

/*
 * Sensor Terminal board with Radio Controller board RCB230 3.2
 * http://www.dresden-elektronik.de/shop/prod75.html
 */
#define RCB_3_2_SENS_TERM_BOARD (0x13)

/* STK541 USB Adapter board with Radio Controller board RCB230 V3.2 */
#define RCB_3_2_STK541          (0x14)

/* STK500 & STK501 boards with Radio Extender board REB230B V2.3 */
#define REB_2_3_STK500_STK501   (0x15)

/* Atmel ZigBit 2.4GHz Wireless Module on MeshBean2 platform */
#define ATZB_24_MN2             (0x16)

/*** my board type ***/
#define MY_BOARD (0x17)



/*
 * Boards for AT86RF231
 */
/*
 * RCB Breakout Board with Radio Controller board RCB231 4.0
 * http://www.dresden-elektronik.de/shop/prod85.html
 *
 * RCB Breakout Board Light with Radio Controller board RCB231 4.0
 * http://www.dresden-elektronik.de/shop/prod84.html
 */
#define RCB_4_0_BREAKOUT_BOARD  (0x21)

/*
 * Remote Control Demonstration platform using push button keys.
 */
#define RCB_4_0_KEY_RC          (0x22)

/*
 * Plain Radio Controller board RCB231 V4.0
 */
#define RCB_4_0_PLAIN           (0x23)

/*
 * Sensor Terminal board with Radio Controller board RCB231 4.0
 * http://www.dresden-elektronik.de/shop/prod75.html
 */
#define RCB_4_0_SENS_TERM_BOARD (0x24)

/* STK541 USB Adapter board with Radio Controller board RCB231 V4.0.2 */
#define RCB_4_0_STK541          (0x25)

/* STK500 & STK501 boards with Radio Extender board REB231 V4.0.1 */
#define REB_4_0_STK500_STK501   (0x26)

/*
 * RCB Breakout Board with Radio Controller board RCB231ED 4.1
 * http://www.dresden-elektronik.de/shop/prod85.html
 *
 * RCB Breakout Board Light with Radio Controller board RCB231ED 4.1
 * http://www.dresden-elektronik.de/shop/prod84.html
 */
#define RCB_4_1_BREAKOUT_BOARD  (0x27)

/*
 * Plain Radio Controller board RCB231ED V4.1.1
 */
#define RCB_4_1_PLAIN           (0x28)

/*
 * Sensor Terminal board with Radio Controller board RCB231 4.1
 * http://www.dresden-elektronik.de/shop/prod75.html
 */
#define RCB_4_1_SENS_TERM_BOARD (0x29)

/*
 * STK541 USB Adapter board with Radio Controller board RCB231ED V4.1.1
 * (using antenna diversity)
 */
#define RCB_4_1_STK541          (0x2A)

/* STK500 & STK501 boards with Radio Extender board REB231ED V4.1.1 */
#define REB_4_1_STK500_STK501   (0x2B)

/*
 * Remote Control Demonstration platform using push button keys.
 */
#define RCB_4_1_KEY_RC          (0x2C)


/*
 * Boards for AT86RF212
 */
/*
 * RCB Breakout Board with Radio Controller board RCB212SMA V5.3.21
 * http://www.dresden-elektronik.de/shop/prod85.html
 *
 * RCB Breakout Board Light with Radio Controller board RCB212SMA V5.3.2
 * http://www.dresden-elektronik.de/shop/prod84.html
 */
#define RCB_5_3_BREAKOUT_BOARD  (0x31)

/*
 * Remote Control Demonstration platform using push button keys.
 */
#define RCB_5_3_KEY_RC          (0x32)

/* Plain Radio Controller board RCB212SMA V5.3.2 */
#define RCB_5_3_PLAIN           (0x33)

/*
 * Sensor Terminal board with Radio Controller board RCB 5.3
 * http://www.dresden-elektronik.de
 */
#define RCB_5_3_SENS_TERM_BOARD (0x34)

/* STK541 USB Adapter board with Radio Controller board RCB212SMA V5.3.2 */
#define RCB_5_3_STK541          (0x35)

/* STK500 & STK501 boards with Radio Extender board REB212 V5.0.2 */
#define REB_5_0_STK500_STK501   (0x36)

/* ZigBit 700/800/900 MHz Wireless Module on MeshBean2 platform */
#define ATZB_900_MN2            (0x37)

/* Atmel internal */
#define EVAL212                 (0x3E)
#define EMU212                  (0x3F)


/*
 * Boards for AT86RF232
 */
/*
 * RCB Breakout Board with Radio Controller board RCB232SMA V7.3
 * http://www.dresden-elektronik.de/shop/prod85.html
 *
 * RCB Breakout Board Light with Radio Controller board RCB232SMA V7.3
 * http://www.dresden-elektronik.de/shop/prod84.html
 */
#define RCB_7_3_BREAKOUT_BOARD  (0x41)

/*
 * Remote Control Demonstration platform using push button keys.
 */
#define RCB_7_3_KEY_RC          (0x42)

/* Plain Radio Controller board RCB232SMA V7.3 */
#define RCB_7_3_PLAIN           (0x43)

/*
 * Sensor Terminal board with Radio Controller board RCB 7.3
 */
#define RCB_7_3_SENS_TERM_BOARD (0x44)

/* STK541 USB Adapter board with Radio Controller board RCB232SMA V7.3 */
#define RCB_7_3_STK541          (0x45)

/* STK500 & STK501 boards with Radio Extender board REB232 V7.0 */
#define REB_7_0_STK500_STK501   (0x46)

/* STK541 USB Adapter board with Radio Controller board RCB232SMA V7.4 */
#define RCB_7_4_STK541          (0x47)

/*
 * Remote Control Demonstration platform using push button keys.
 */
#define RCB_7_4_KEY_RC          (0x48)

/*
 * Sensor Terminal board with Radio Controller board RCB 7.4
 */
#define RCB_7_4_SENS_TERM_BOARD (0x49)

/* Atmel internal */
#define EVAL232                 (0x4E)
#define EMU232                  (0x4F)
#endif  /* #if defined(VENDOR_BOARDTYPES) && (VENDOR_BOARDTYPES != 0) */

#endif  /* PAL_BOARDTYPES_H */
/* EOF */
