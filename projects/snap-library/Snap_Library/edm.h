/*
Copyright (c) 2002 Colin O'Flynn

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

The routines in this file are tested to work with SNAP lab (checksum, CRC8 and
CRC16 tested), as well CRC8 and CRC16 produce the same result as the built-in
function in BASCOM-AVR. These may NOT be official CRC algorithms, so be warned
*/

#ifndef _edm_header
#define _edm_header

/*IMPORTANT: These routines assume the following about your compiler, if it
is different these routines won't work as provided :(

unsigned char			8 	bits
unsigned int			16 bits
unsigned long			32 bits
*/

//define version of file
#define CRC_VERSION 0.1

/*save code space by only enabling the EDM you need


Each CRC lets you use FAST or SLOW mode. in FAST mode a table is used, which
takes up a large amount of RAM. in SLOW mode everything is calculated, so little
RAM is taken up. Normally SLOW mode should be fine, and it is recommened unless
you are processing tons of data bytes. Also some of the CRC don't have a FAST
mode encoded yet...*/

#define TRUE		1
#define FALSE		!TRUE

#define SLOW		1
#define FAST		!SLOW

//checksum 8 bit specific stuff
#define CHECKSUM_E 	FALSE			//TRUE = enabled FALSE = disabled

//CRC8 Specific stuff
#define CRC8_E		FALSE			//TRUE = enabled FALSE = disabled
#define CRC8INIT	0x00
#define CRC8POLY	0x18            //0X18 = X^8+X^5+X^4+X^0

//CRC16 Specific stuff
#define CRC16_E 	TRUE			//TRUE = enabled FALSE = disabled
#define CRC16INIT	0x0000
#define CRC16POLY 	0x1021          //0x1021 = X^16+X^12+X^5+X^0


//Prototype functions
/*All functions take a pointer to array of type unsigned char, where
"number_of_bytes_to_read" is how many bytes to read, or the final array index
+ 1. So crc16(test, 10) will return the crc16 of array test[0] to test[9] */

#if CHECKSUM_E == TRUE
	unsigned char checksum_1_byte (unsigned char *data_in, unsigned short number_of_bytes_to_read);
#endif

#if CRC8_E == TRUE
	unsigned char crc8 (unsigned char *data_in, unsigned short number_of_bytes_to_read);
#endif

#if CRC16_E == TRUE
	unsigned int crc16 (unsigned char data_in[], unsigned short number_of_bytes_to_read);
	unsigned int crc16_continue (unsigned char data_in[], unsigned short number_of_bytes_to_read, unsigned int crc);
#endif


#endif
