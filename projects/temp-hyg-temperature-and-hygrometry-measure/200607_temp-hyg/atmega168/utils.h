#ifndef UTILS_H
#define UTILS_H

typedef struct
{
	volatile uint8_t *data;		// a pointer to the buffer containing the data
	volatile uint8_t size;		// the total size of the buffer
	volatile uint8_t length;	// the actual length of the fifo
	volatile uint8_t start;		// the place at which to read from
	volatile uint8_t end;		// the place at which to write to
}
fifo_t;

typedef union
{
	struct
	{
		uint8_t ym;				// year-month
		uint8_t d;				// day
	} b;						// byte view
	
	uint16_t i;					// int view
}
dateStamp_t;

uint8_t int16ToString(int16_t, char *);

#define fifoHasData(fifoP) (fifoP)->length
void fifoInit(volatile fifo_t *, volatile uint8_t *, uint8_t);
uint8_t fifoAddByte(volatile fifo_t *, uint8_t);
uint8_t fifoAddBuffer(volatile fifo_t *, volatile uint8_t *, uint8_t);
uint8_t fifoGetByte(volatile fifo_t *);

#define dateStampGetYear(stampP, baseYear) (((stampP)->b.ym >> 4) + baseYear)
#define dateStampGetMonth(stampP) ((stampP)->b.ym & 0x0F)
#define dateStampGetDay(stampP) ((stampP)->b.d)
#define dateStampIsValid(stampP) (((stampP)->b.ym & 0x0F) < 13)
#define dateStampInvalidate(stampP) ((stampP)->b.ym = 0xFF)
void dateStampBuild(dateStamp_t *, uint8_t, uint8_t, uint8_t, uint8_t);
void dateValidate(uint8_t *, uint8_t *, uint8_t *);

#endif
