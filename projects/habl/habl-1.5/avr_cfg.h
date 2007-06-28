/*
avr_cfg.h

provide functions to set up hardware

This file is free software; you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
General Public License for more details.

If you do modify it please make a note about it in the history 
section below.

History

2005-03-23
Created by Henrik Björkman

2007-03-15
Modified for use in boot loader.

*/


#ifndef AVR_CFG_H
#define AVR_CFG_H

// Using avr-gcc 4.3.2 the code is not as small as it was using 4.1.2
// The version string and the CRC check no longer both fit in 512 words.
// So there are three options here.
// 1) Use old compiler 
// 2) Remove WELCOME_STRING (comment WELCOME_STRING out below)
// 3) Dont use CRC (comment USE_XMODEM_CRC out below)
// 4) Make boot sector larger (1024 words)
// If you wish to use a 1024 word bootloader section you also need to change
// fuse bits see avrdude_usb_atmega88.txt and makefile.

#define WELCOME_STRING "\r\nwww.eit.se/habl/"VERSION_STRING"\r\n"

#define AVR_FOSC 16000000L
#define BAUDRATE 19200

// Define this if boot loader shall enable watchdog timer
#define ENABLE_WDT


//#define USE_XMODEM_CRC


#endif
