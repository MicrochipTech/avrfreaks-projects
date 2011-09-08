#ifndef _queue_h_Included_
#define _queue_h_Included_
/*********************************************************************************
 Copyright (C) 2011 by Stefan Filipek

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
*********************************************************************************/


/**
 * @file queue.h
 *
 * Circular queue functionality.
 */

#include <stdint.h>
#include <stddef.h>

class Queue {

private:
    static const uint8_t MAX_SIZE=8;        ///< Maximum supported size

    uint8_t data[MAX_SIZE];                 ///< Queue data
    uint8_t top;                            ///< Index to top
    uint8_t bottom;                         ///< Index to bottom
    uint8_t size;                           ///< Current size of the queue

public:

    Queue();

    /**
     * Place data on to a Queue
     *
     * Non-blocking. Will return error if queue is full.
     *
     * @param input the byte to place on the Queue
     *
     * @return 1 if sucessful, 0 otherwise
     */
    uint8_t Enq(uint8_t input);


    /**
     * Grab data from a Queue
     *
     * Non-blocking. Will return error if queue is empty
     *
     * @param target a pointer to a byte to place the read value
     *
     * @return 1 if sucessful, 0 otherwise
     */
    uint8_t Deq(uint8_t* target);

};

#endif
