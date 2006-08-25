#include <inttypes.h>

#include "shtxx.h"
#include "db.h"
#include "event.h"
#include "clock.h"
#include "eeprom.h"

dbSensor_t dbTemperature;
dbSensor_t dbHumidity;

void dbRetrieveValues(void)
{
	dbTemperature.value = shtxxGetLastMeasure(SHTXX_TEMPERATURE);
	if (dbTemperature.value < dbTemperature.min) dbTemperature.min = dbTemperature.value;
	if (dbTemperature.value > dbTemperature.max) dbTemperature.max = dbTemperature.value;
	dbHumidity.value = shtxxGetLastMeasure(SHTXX_HUMIDITY);
	if (dbHumidity.value < dbHumidity.min) dbHumidity.min = dbHumidity.value;
	if (dbHumidity.value > dbHumidity.max) dbHumidity.max = dbHumidity.value;
}

dbSensor_t *dbGetSensor(uint8_t sensor)
{
	if (sensor == DB_TEMPERATURE) return &dbTemperature;
	if (sensor == DB_HUMIDITY) return &dbHumidity;
	return 0;
}

void dbInit(void)
{
	dbTemperature.min = 32767;
	dbTemperature.max = -32767;
	dbHumidity.min = 32767;
	dbHumidity.max = -32767;
		
	eventRegisterHandler(dbRetrieveValues, EVENT_SENSOR);
}
