/******************************************************************************
 * (C) Copyright 2006, Artur Lipowski
 *
 * No guarantees, warrantees, or promises, implied or otherwise.
 * May be used for hobby or commercial purposes provided copyright
 * notice remains intact.
 *
 ******************************************************************************/

#include "ring_buf.h"


extern void UART_put(uint8_t byte);
extern bool Is_UART_Ready(void);


void clear_buf(volatile RingBuffer_T * rb)
{
    SST_DECLARE_INT_USAGE;

    SST_INT_LOCK_AND_SAVE();
    rb->head = 0;
    rb->tail = 0;
    rb->len = 0;
    SST_INT_LOCK_RESTORE();
}


uint8_t get_buf(volatile RingBuffer_T * rb)
{
    //SST_DECLARE_INT_USAGE;
    uint8_t byte;

    //SST_INT_LOCK_AND_SAVE();

    byte = rb->buf[rb->head];
    rb->head++;
    rb->head %= BUF_SIZE;
    rb->len--;

    //SST_INT_LOCK_RESTORE();
    return byte;
}


void put_buf(volatile RingBuffer_T * rb, uint8_t byte)
{
    SST_DECLARE_INT_USAGE;

    SST_INT_LOCK_AND_SAVE();

    if (rb->len < BUF_SIZE)
    {
        if ((0 == rb->len) && Is_UART_Ready())
        {
            UART_put(byte);
        }
        else
        {
            rb->buf[rb->tail] = byte;
            rb->tail++;
            rb->tail %= BUF_SIZE;
            rb->len++;
        }
    }

    SST_INT_LOCK_RESTORE();
}
