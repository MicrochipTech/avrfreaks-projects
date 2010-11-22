#ifndef __SERIAL_H__
#define __SERIAL_H__

/******************************************************************************
 *
 * ID      : $Id: serial.h 29 2010-11-21 12:50:50Z SH $ 
 * Revsion : $Rev: 29 $
 * Author  : $Author: SH $
 * Date    : $Date: 2010-11-21 13:50:50 +0100 (So, 21 Nov 2010) $   
 *
 ******************************************************************************
 *
 * License
 *
 * Copyright (c) 2010 by Stefan Heesch, http://www.heesch.net
 *
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>
 *
 ******************************************************************************/


/****************************************************************************
 *  Include Definitions
 ****************************************************************************/
 #include <stdlib.h>
 #include <avr\io.h>
 #include <avr\interrupt.h>
 #include <avr\pgmspace.h>


/****************************************************************************
 *  Function prototypes
 ****************************************************************************/
 void uart_init(unsigned long baudrate);
 void uart_putc(char byte);
 void uart_puts(char* str );
 void uart_puts_p(char* string);
 char uart_getc(void);



 #endif /* __SERIAL_H__ */




