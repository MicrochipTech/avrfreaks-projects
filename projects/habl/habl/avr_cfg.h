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

#define VERSION_STRING "\r\nhabl 1.2\r\nfor help goto www.eit.se/habl\r\n"


#define AVR_FOSC 8000000L
#define BAUDRATE 19200

// Define this if boot loader shall enable watchdog timer
#define ENABLE_WDT



#endif
