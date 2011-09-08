#ifndef _utils_h_Included_
#define _utils_h_Included_
/*********************************************************************************
 Copyright (C) 2011 by Stefan Filipek

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
*********************************************************************************/

/**
 * @file utils.h
 *
 * Random, useful utilities.
 *
 * Contains a random collection of utilities that I have found
 * useful.
 *
 */

#include <stddef.h>
#include <stdint.h>

namespace Utils {

/**
 * Prints a bunch of hex
 *
 * Prints each byte out in capital hex, zero padded, and
 * separated by spaces. For large amounts of data, see
 * print_block().
 *
 * Relies on avr-libc and proper binding of stdout
 *
 * @param data a pointer to the data to print
 * @param size the number of bytes to print
 */
void PrintHex(void *data, size_t size);


/**
 * Prints a block of hex in a pretty way
 *
 * Prints a block of hex out along with the associated address
 * and ASCII characters. Any line that needs padding has byte
 * values represented by "--"
 *
 * Relies on avr-libc and proper binding of stdout
 *
 * @param data  a pointer to the data to print
 * @param size  the number of bytes to print
 * @param index a starting address to print with the data
 * @param width the number of bytes to print on a single line
 */
void PrintBlock(void *data, size_t size, size_t index=0, size_t width=16);


}

#endif
