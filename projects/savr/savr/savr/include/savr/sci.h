#ifndef _sci_h_Included_
#define _sci_h_Included_
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
 * @file sci.h
 *
 * SCI/UART interface
 *
 * This interface relies on avr-libc. It binds the compile-time
 * configured UART to stdin and stdout, allowing the user to
 * read/write data over the serial line using printf and similar
 * calls.
 *
 * This is intended to be one-per-system as the 'debug console'.
 */


namespace SCI {

    /**
     * Initialize the SCI subsystem
     *
     * @param sysClock  The clock frequency of the system
     * @param baud      The desired baud rate
     */
    void Init(uint32_t sysClock, uint32_t baud);

};


#endif
