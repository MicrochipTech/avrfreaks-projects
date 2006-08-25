#ifndef CLOCK_H
#define CLOCK_H

#define CLOCK_BASE_YEAR 2000
#define CLOCK_FIRST_YEAR 06		

#define CLOCK_WITH_SECONDS 1
#define CLOCK_WITHOUT_SECONDS 0

#define CLOCK_RECORD 509	/* Store clock (yy.mm.dd.hh) at end of eeprom */

typedef struct
{
	uint8_t year;
	uint8_t month;
	uint8_t day;
	uint8_t hour;
	uint8_t minute;
	uint8_t second;
}
clock_t;

void clockInit(void);
void clockSet(uint8_t, uint8_t, uint8_t, uint8_t, uint8_t, uint8_t);
void clockDateValidate(uint8_t *, uint8_t *, uint8_t *);
clock_t *clockGet(void);
void clockGetTimeAsString(char *, uint8_t);
void clockGetDateAsString(char *);

#endif
