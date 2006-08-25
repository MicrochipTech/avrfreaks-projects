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
	struct
	{
		uint8_t average;
		uint8_t min;
		uint8_t max;
	} temperature;
	struct
	{
		uint8_t average;
		uint8_t min;
		uint8_t max;
	} humidity;
}
dbRecord_t;

#define DB_TEMPERATURE 1
#define DB_HUMIDITY 2

#define DB_TEMPERATURE_OFFSET 26	/* Allows to go from -26 to 99.5 */

dbSensor_t *dbGetSensor(uint8_t);
void dbInit(void);

#endif
