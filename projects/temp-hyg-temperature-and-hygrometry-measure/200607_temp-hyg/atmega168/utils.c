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


// ----------------- Fifo utils -----------------------------------

void fifoInit(volatile fifo_t *fifo, volatile uint8_t *buffer, uint8_t size)
{
	fifo->data = buffer;
	fifo->size = size;
	fifo->start = 0;
	fifo->end = 0;
	fifo->length = 0;
}

uint8_t fifoAddByte(volatile fifo_t *fifo, uint8_t d)
{
	if (fifo->length == fifo->size) return 0;
	fifo->data[fifo->end] = d;
	fifo->end = (fifo->end + 1) % fifo->size;
	fifo->length ++;
	return 1;
}

uint8_t fifoAddBuffer(volatile fifo_t *fifo, volatile uint8_t *data, uint8_t dataLength)
{
	uint8_t i;
	
	for (i=0; i<dataLength; i++)
	{
		if (!fifoAddByte(fifo, data[i])) return 0;
	}
	return 1;
}

uint8_t fifoGetByte(volatile fifo_t *fifo)
{
	uint8_t d;
	
	if (fifo->length == 0) return 0;
	d = fifo->data[fifo->start];
	fifo->start = (fifo->start + 1) % fifo->size;
	fifo->length --;
	return d;
}


// --------------------- date utils -------------------------

void dateStampBuild(dateStamp_t *stamp, uint8_t year, uint8_t month, uint8_t day, uint8_t baseYear)
{
	uint8_t dvYear, dvMonth;
	
	dvYear = year;
	dvMonth = month;
	stamp->b.d = day;
	if (day == 0)
	{
		stamp->b.d = 31;
		dvMonth --;
	}
	if (dvMonth == 0)
	{
		dvMonth = 12;
		dvYear --;
	}
	dateValidate(&dvYear, &dvMonth, &(stamp->b.d));
	if (dvYear < baseYear) stamp->b.ym = 0;
	else stamp->b.ym = ((dvYear - baseYear) << 4) + dvMonth;
}

void dateValidate(uint8_t *year, uint8_t *month, uint8_t *day)
{
	if (*year > 99) *year = 99;
	if (*month < 1) *month = 1;
	if (*month > 12) *month = 12;
	if (*day < 1) *day = 1;
	if (*day < 29) return;
	if (*month == 2)			// in February
	{
		// allow 29 days on bissextile years
		if ((*day == 29) && (*year % 4 == 0)) return;
		*day = 28;
	}
	else if ((*month == 4) || (*month == 6) || (*month == 9) || (*month == 11)) 
	{
		// allow 30 days
		if (*day < 31) return;
		*day = 30;
	}
	else
	{
		// allow 31 days
		if (*day < 32) return;
		*day = 31;
	}
}
