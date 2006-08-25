#ifndef DB_H
#define DB_H 1

typedef struct
{
	int16_t value;
	int16_t min;
	int16_t max;
}
dbSensor_t;

typedef struct
{
	uint8_t value;
	uint8_t min;
	uint8_t max;
}
dbValue_t;

// values are starting from 1 
#define DB_TEMPERATURE 1
#define DB_HUMIDITY 2

#define DB_HOUR 1
#define DB_DAY 2
#define DB_MONTH 3
#define DB_YEAR 4

#define DB_TEMPERATURE_OFFSET 27	/* Allows to go from -27 (0x00) to 100 (0xFE) by half degrees */

#define DB_NB_YEARS 10				/* Max 16 */

dbSensor_t *dbGetSensor(uint8_t);
int16_t dbGetRealValue(uint8_t, uint8_t);
void dbGetSensorData(uint8_t, uint8_t, dbValue_t *, uint8_t *, uint8_t *, uint8_t *, uint8_t *);
void dbInit(void);

#endif
