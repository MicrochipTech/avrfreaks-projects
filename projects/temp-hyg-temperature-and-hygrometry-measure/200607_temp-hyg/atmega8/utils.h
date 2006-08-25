#ifndef UTILS_H
#define UTILS_H

typedef struct
{
	volatile uint8_t *data;		// a pointer to the buffer container the data
	volatile uint8_t size;		// the total size of the buffer
	volatile uint8_t length;	// the actual length of the fifo
	volatile uint8_t start;		// the place at which to read from
	volatile uint8_t end;		// the place at which to write to
}
fifo_t;

uint8_t int16ToString(int16_t, char *);

#define fifoHasData(fifo) (fifo)->length

void fifoInit(fifo_t *, uint8_t *, uint8_t);
uint8_t fifoAddByte(fifo_t *, uint8_t);
uint8_t fifoAddBuffer(fifo_t *, uint8_t *, uint8_t);
uint8_t fifoGetByte(fifo_t *);

#endif
