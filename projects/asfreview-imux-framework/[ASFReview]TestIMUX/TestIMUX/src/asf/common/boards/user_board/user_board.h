/**
 * \file
 *
 * \brief User board definition template
 *
 */

#ifndef USER_BOARD_H
#define USER_BOARD_H

/* This file is intended to contain definitions and configuration details for
 * features and devices that are available on the board, e.g., frequency and
 * startup time for an external crystal, external memory devices, LED and USART
 * pins.
 */

#define USBUSARTPORT USARTF0
#define DEBUGPORT    USARTC1

// Definitions for the output bits of the expansion port
#define SELFTEST  0x01
#define WAKE      0x02
#define FULLSCALE 0x04
#define PWR       0x08
#define PWD       0x10
#define REDLED    0x20
#define GREENLED  0x40
#define YELLOWLED 0x80

#endif // USER_BOARD_H
