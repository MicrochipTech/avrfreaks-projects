/*************************************************************//**
* \file queue.c
*
* Circular queue
*
* Circular queue functionality.
*
******************************************************************/

#include "queue.h"

/**
 * Place data on to a Queue
 *
 * Non-blocking. Will return error if queue is full.
 *
 * @param input the byte to place on the Queue
 * @param q a pointer to the Queue to use
 *
 * @return 1 if sucessful, 0 otherwise
 */
uint8_t
en_q(uint8_t input, Queue* q)
{
	if(q->size != _QSIZE)   // If we're not full
	{
		if(q->size != 0)
		{
			q->bottom++;
			if(q->bottom >= _QSIZE)
				q->bottom = 0;
		}

		q->data[q->bottom] = input;
		q->size++;
		return 1;
	}
	return 0;      // Errors by default
}

/**
 * Grab data from a Queue
 *
 * Non-blocking. Will return error if queue is empty
 *
 * @param target a pointer to a byte to place the read value
 * @param q a pointer to the Queue to use
 *
 * @return 1 if sucessful, 0 otherwise
 */
uint8_t
de_q(uint8_t* target, Queue* q)
{
	if(q->size != 0)     // If we're not empty
	{
		*target = q->data[q->top];

		if(q->size != 1)
		{
			q->top++;
			if(q->top >= _QSIZE)
				q->top = 0;
		}
		q->size--;
		return 1;
	}
	return 0;      // Errors by default
}
