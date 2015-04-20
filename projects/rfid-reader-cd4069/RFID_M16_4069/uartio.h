/*
    Copyright (C) 2004 by Michal Vanka. All rights reserved.

    This file is part of ERFID FIRMWARE ONE, the firmware for ERFID1 board.
    For more information see http://www.vanka.net

    ERFID FIRMWARE ONE is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    ERFID FIRMWARE ONE is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with ERFID FIRMWARE ONE; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
    
    uartio.h
    
*/



#include <inttypes.h>

void uart_init(uint8_t sp);
void uart_send_string(char *s);
void uart_send_char(uint8_t c);

//values for 16000000 Hz crystal

//#define BAUD_9k6 103
//#define BAUD_57k6 16
#define B2X(b) ((b)<=9)?((b)+'0'):((b)-10+'A')

#define BAUD_9k6 0x33
//#define BAUD_57k6 16
