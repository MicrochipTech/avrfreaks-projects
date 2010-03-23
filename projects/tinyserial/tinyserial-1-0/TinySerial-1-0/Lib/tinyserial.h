#ifndef _TINY_SERIAL_H_
#define _TINY_SERIAL_H_

// The Tinyserial library.
// Copyright (C) 2010 Timothy Jon Fraser <tfraser@alum.wpi.edu>
// 
// This file is part of the Tinyserial library.
// 
// The Tinyserial library is free software: you can redistribute it
// and/or modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation, either version 3
// of the License, or (at your option) any later version.
// 
// The Tinyserial is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public
// License along with the Tinyserial library.  If not, see
// <http://www.gnu.org/licenses/>.
// 
// 
// This is the C-callable interface to libtinyserial.a.  See below for
// examples on how to call each function.  Note that although I have
// borrowed function names from the ANSI/ISO C library standard, the
// paramters required do not conform to these standards.

// Constants for ts_init().
#define TS_CONFIG_16MHZ_1200BAUD  832
#define TS_CONFIG_16MHZ_2400BAUD  415
#define TS_CONFIG_16MHZ_9600BAUD  103
#define TS_CONFIG_16MHZ_14400BAUD 68
#define TS_CONFIG_16MHZ_19200BAUD 51
#define TS_CONFIG_16MHZ_28800BAUD 33
#define TS_CONFIG_16MHZ_38400BAUD 25

#define TS_MODE_READONLY  (1<<RXEN0)
#define TS_MODE_WRITEONLY (1<<TXEN0)
#define TS_MODE_READWRITE (TS_MODE_READONLY|TS_MODE_WRITEONLY)

//Constants for ts_getchar() and ts_gets().
#define TS_ECHO_OFF 0x00
#define TS_ECHO_ON  0x01

// Call this function first to configure USART0.
// Example:  ts_init(TS_CONFIG_16MHZ_9600BAUD, TS_MODE_WRITEONLY);
void ts_init(unsigned int, unsigned char);

// Call this function to write single characters to USART0.
// Example:  ts_putc('x');
void ts_putc(unsigned char);

// Call this function to read singles characters from USART0.  Use the
// parameter to control whether the program should echo the characters
// it reads back to the USART0 or not.
// Example: char byte = ts_getc(TS_ECHO_ON);
char ts_getc(unsigned char);

// Call this function to write C strings stored in SRAM to USART0.
// Example:  ts_puts("Hello World!");
void ts_puts(char *);

// Call this function to read C strings from USART0.
// Example:
// #define INPUT_LENGTH 16
// char buffer[INPUT_LENGTH + 1];
// unsigned int length;
// length = ts_gets(buffer, INPUT_LENGTH, TS_ECHO_ON);
// buffer[length] = '\0';
unsigned int ts_gets(char *, unsigned int, unsigned char);

// Call this function to write C strings stored in Flash to USART0.
// Example:
// char thestr[] __attribute((__progmem__)) = THEDATA;
// ts_putsf(thestr);
void ts_putsf(char *);

#endif
