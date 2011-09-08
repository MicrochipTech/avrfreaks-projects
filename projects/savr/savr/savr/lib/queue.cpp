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

#include <util/atomic.h>

#include <savr/queue.h>

/**
 * @par Implementation notes:
 */
Queue::Queue() :
    top(0),
    bottom(0),
    size(0)
{
    // Empty
}


/**
 * @par Implementation notes:
 */
uint8_t
Queue::Enq(uint8_t input)
{
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
        if(size == MAX_SIZE) return 1;

        data[bottom++] = input;
        if(bottom >= MAX_SIZE) bottom = 0;
        size++;
    }
    return 0;
}


/**
 * @par Implementation notes:
 */
uint8_t
Queue::Deq(uint8_t* target)
{
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
        if(size == 0) return 1;

        *target = data[top++];
        if(top >= MAX_SIZE) top = 0;
        size--;
    }
    return 0;
}

