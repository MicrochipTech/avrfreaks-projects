#ifndef _QUEUE_H
#define _QUEUE_H 1

/*************************************************************//**
* \file queue.h
*
* Circular queue header
*
* Circular queue functionality.
*
******************************************************************/

#include <inttypes.h>

//! The maximum size, in bytes, of a queue (be reasonable)
#define _QSIZE 8

//! Queue structure
struct queue {
  //! Queue data
	uint8_t data[_QSIZE];
  //! Index to top
	uint8_t top;
  //! Index to bottom
	uint8_t bottom;
  //! Current size of the queue
	uint8_t size;
};

//! Typedef for better... typing...
typedef struct queue Queue;

uint8_t en_q(uint8_t, Queue*);
uint8_t de_q(uint8_t*, Queue*);

#endif
