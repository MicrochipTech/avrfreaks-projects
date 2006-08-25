#include <inttypes.h>
#include "utils.h"

uint8_t int16ToString(int16_t number, char *buffer)
{
	uint8_t length = 0;
	uint16_t n = number;
	uint16_t absNumber = number;
	
	if (number < 0) 
	{
		buffer[length++] = '-';
		absNumber = -number;
	}
	n = absNumber;
	if (absNumber >= 10000) 
	{
		buffer[length++] = n/10000 + '0';
		n = n % 10000;
	}
	if (absNumber >= 1000) 
	{
		buffer[length++] = n/1000 + '0';
		n = n % 1000;
	}
	if (absNumber >= 100) 
	{
		buffer[length++] = n/100 + '0';
		n = n % 100;
	}
	if (absNumber >= 10) buffer[length++] = n/10 + '0';
	buffer[length++] = (n % 10) + '0';
	return length;
}

void fifoInit(fifo_t *fifo, uint8_t *buffer, uint8_t size)
{
	fifo->data = buffer;
	fifo->size = size;
	fifo->start = 0;
	fifo->end = 0;
	fifo->length = 0;
}

uint8_t fifoAddByte(fifo_t *fifo, uint8_t d)
{
	if (fifo->length == fifo->size) return 0;
	fifo->data[fifo->end] = d;
	fifo->end = (fifo->end + 1) % fifo->size;
	fifo->length ++;
	return 1;
}

uint8_t fifoAddBuffer(fifo_t *fifo, uint8_t *data, uint8_t dataLength)
{
	uint8_t i;
	
	for (i=0; i<dataLength; i++)
	{
		if (!fifoAddByte(fifo, data[i])) return 0;
	}
	return 1;
}

uint8_t fifoGetByte(fifo_t *fifo)
{
	uint8_t d;
	
	if (fifo->length == 0) return 0;
	d = fifo->data[fifo->start];
	fifo->start = (fifo->start + 1) % fifo->size;
	fifo->length --;
	return d;
}
