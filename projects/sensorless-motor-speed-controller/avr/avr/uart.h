/* copyright
   * (c) 2004 Stephen Humble stephenhumble at telstra.com
   *
   *  This file is part of robotx see http://Robotx.sourceforge.net
   *  dual sensorless motor driver firmware.
   *
   *  This is free software; you can redistribute it and/or modify
   *  it under the terms of the GNU General Public License as published by
   *  the Free Software Foundation; either version 2 of the License, or
   *  (at your option) any later version.
*/
/* -*- indent-tabs-mode:T; c-basic-offset:8; tab-width:8; -*- vi: set ts=8:
   * $Id: uart.h,v 1.10 2004/12/07 05:31:28 stephenhumble Exp $
   *
   * (c) 2002 Trammell Hudson <hudson@swcp.com>
   *************
   *
   *  This file is part of the autopilot onboard code package.
   *  
   *  Autopilot is free software; you can redistribute it and/or modify
   *  it under the terms of the GNU General Public License as published by
   *  the Free Software Foundation; either version 2 of the License, or
   *  (at your option) any later version.
   *  
   *  Autopilot is distributed in the hope that it will be useful,
   *  but WITHOUT ANY WARRANTY; without even the implied warranty of
   *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   *  GNU General Public License for more details.
   *  
   *  You should have received a copy of the GNU General Public License
   *  along with Autopilot; if not, write to the Free Software
   *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
   *
*/

#ifndef _UART_H_
#define _UART_H_

#include <sig-avr.h>
#include <interrupt.h>
#include <inttypes.h>
#include <io.h>

extern void init_uart( void );

extern void putc(uint8_t	c);

extern volatile long int rxchar;

void put_cr();  // send a line end carridge return
void put_i16(int16_t c);
void put_bin(unsigned char c);
void put_u16(uint16_t c);
void putw(int16_t c);
void putl(int32_t c);
void hexputc(uint8_t c);
void hexputw(int16_t c);
void hexputl(int32_t c);

uint32_t geti(uint32_t **c);
uint16_t getw(uint16_t **c);

void syncwait();

#endif
