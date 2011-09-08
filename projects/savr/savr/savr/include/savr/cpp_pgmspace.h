#ifndef _cpp_pgmspace_h_Included_
#define _cpp_pgmspace_h_Included_
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
#include <avr/pgmspace.h>

/**
 * @file cpp_pgmspace.h
 *
 * This is to fix issues with avr-libc / GCC combination for defining strings
 * in program space.
 */

#define CPP_PROGMEM __attribute__((section(".progmem.cpp")))

#undef PROGMEM
#define PROGMEM CPP_PROGMEM

#undef PSTR
#define PSTR(s) (__extension__({static const char CPP_PROGMEM __c[] = (s); &__c[0];}))


#endif
