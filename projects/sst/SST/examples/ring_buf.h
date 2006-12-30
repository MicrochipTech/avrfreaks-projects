#ifndef RING_BUF_H_
#define RING_BUF_H_

/******************************************************************************
 * (C) Copyright 2006, Artur Lipowski
 *
 * No guarantees, warrantees, or promises, implied or otherwise.
 * May be used for hobby or commercial purposes provided copyright
 * notice remains intact.
 *
 ******************************************************************************/

#include <sst.h>

#define BUF_SIZE    8

typedef struct {
    CPU_Base_T len;
    CPU_Base_T head;
    CPU_Base_T tail;
    uint8_t buf[BUF_SIZE];
} RingBuffer_T;

#define IS_BUF_FULL(_buf_)  (BUF_SIZE == (_buf_).len)
#define IS_BUF_EMPTY(_buf_) (0 == (_buf_).len)

void clear_buf(volatile RingBuffer_T * rb);
uint8_t get_buf(volatile RingBuffer_T * rb);
void put_buf(volatile RingBuffer_T * rb, uint8_t byte);


#endif /*RING_BUF_H_*/
