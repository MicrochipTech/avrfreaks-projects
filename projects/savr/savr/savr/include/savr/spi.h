#ifndef _spi_h_Included_
#define _spi_h_Included_
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
 * @file spi.h
 *
 * A simple SPI interface.
 *
 * Notes:
 *  The SS line must be manually set by the user.
 */

namespace SPI {


    /**
     * Initialize the SPI subsystem
     *
     * @param sysClock  Clock speed of the chip
     * @param spiFreq   Desired SPI clock frequency    
     */
    void        Init(uint32_t sysClock, uint32_t spiFreq);


    /**
     * Send a block of data over the SPI
     *
     * @param input a pointer to the source data
     * @param length the size of the source data
     */
    void        SendBlock(uint8_t *input, size_t length);


    /**
     * Reads a block of data from the SPI
     *
     * @param input a pointer to the destination buffer
     * @param length the number of bytes to read
     * @param filler a byte to send continuously while reading
     */
    void        GetBlock(uint8_t *input, size_t length, uint8_t filler);
    
    
    /**
     * Tx/Rx a byte
     *
     * @param input the byte to send
     *
     * @return the byte read from the SPI line
     */
    uint8_t     TrxByte(uint8_t input);

};

#endif
