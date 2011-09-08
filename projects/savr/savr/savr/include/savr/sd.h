#ifndef _sd_h_Included_
#define _sd_h_Included_
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
 * @file sd.h
 *
 * A simple SD interface using SPI.
 *
 * This interface is somewhat bloated. It is not intended to be
 * fast, but instead helpful for debugging and learning.
 *
 * This library by default will compile in and use CRC checks. If you want
 * to disable this, compile with -DSD_NO_CRC.
 */

#include <savr/gpio.h>

namespace SD {

    /**
     * Initialize the SD card.
     *
     * Attempts to initialize an SD card on the SPI bus. Will
     * go through the standard initialization procedures, and
     * then print out the CID and CSD in hex.
     *
     * @param ss    The slave-select line for the card
     *
     * @return 1 if sucessful, 0 otherwise
     */
    uint8_t   Init(GPIO::Pin ss);


    /**
     * Read a block of data from the SD card.
     *
     * Will read a block of data into the specified
     * buffer.
     *
     * @param buf   a character pointer to the destination
     * @param addr  a 32bit start address
     * @param size  the size of the desired block
     *
     * @return 1 if sucessful, 0 otherwise
     */
    uint8_t   ReadBlock(uint8_t *buf, uint32_t addr, size_t size);


    /**
     * Writes a block of data to the SD card.
     *
     * Writes to the SD card in 512byte block sizes. If the
     * data is not large enough, fills the rest of the block
     * with FILL_BYTE.
     *
     * @param data  pointer to the source of data to write
     * @param addr  the start address (32bit, must be block aligned)
     * @param size  the size of the source data
     *
     * @return 1 if sucessful, 0 otherwise
     */
    uint8_t   WriteBlock(uint8_t *data, uint32_t addr, size_t size);


    /**
     * Erases a block of data from the SD card.
     *
     * Erases data starting at addr and ending at 
     * addr+size (inclusive). The SD card will erase
     * all 512byte blocks that the range touches.
     *
     * @param addr the 32bit start address
     * @param size the size of the block to erase (32bit)
     *
     * @return 1 if sucessful, 0 otherwise
     */
    uint8_t   EraseBlock(uint32_t addr, uint32_t size);

};
#endif
