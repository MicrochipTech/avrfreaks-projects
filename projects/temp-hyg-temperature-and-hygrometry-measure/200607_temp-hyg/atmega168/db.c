#include <inttypes.h>

#include "shtxx.h"
#include "db.h"
#include "event.h"
#include "clock.h"
#include "eeprom.h"
#include "utils.h"

#define DB_REC_SIZE (sizeof(dbRecord_t)-1)
#define DB_INITIAL_YEAR 0
#define DB_DAYS_STORAGE (DB_INITIAL_YEAR+1)
#define DB_MONTHS_STORAGE (DB_DAYS_STORAGE+31*DB_REC_SIZE)
#define DB_YEARS_STORAGE (DB_MONTHS_STORAGE+12*DB_REC_SIZE)
#define DB_END_STORAGE (DB_YEARS_STORAGE+(DB_NB_YEARS+1)*DB_REC_SIZE)
#define DB_HOURS_STORAGE 0xF000

typedef struct
{
	uint16_t temperature;
	uint16_t humidity;
	uint8_t count;
}
dbSum_t;

typedef struct
{
	struct 
	{
		uint8_t min;
		uint8_t max;
	} temperature;
	struct 
	{
		uint8_t min;
		uint8_t max;
	} humidity;
}
dbMinMax_t;


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
	dateStamp_t stamp;
}
dbRecord_t;

typedef struct
{
	uint8_t temperature;
	uint8_t humidity;
	dateStamp_t stamp;
}
dbShortRecord_t;

dbSensor_t dbTemperature;
dbSensor_t dbHumidity;
uint8_t dbActualTemperature;
uint8_t dbActualHumidity;
uint8_t dbInitialYear;
dbSum_t dbSecondSum;
dbSum_t dbMinuteSum;
dbShortRecord_t dbHours[31];
dbMinMax_t dbHoursMinMax;

int16_t dbGetRealValue(uint8_t sensor, uint8_t value)
{
	if (sensor == DB_TEMPERATURE) return ((value >> 1) - DB_TEMPERATURE_OFFSET) * 10 + (value & 0x01 ? 5 : 0);
	return value;
}

void dbRetrieveValues(void)
{
#if defined(DEBUGGING)
	clock_t *now = clockGet();
#endif	
	dbTemperature.value = shtxxGetLastMeasure(SHTXX_TEMPERATURE);
#if defined(DEBUGGING)
	dbTemperature.value = now->second * 10 + 20;
#endif	
	if (dbTemperature.value < dbTemperature.min) dbTemperature.min = dbTemperature.value;
	if (dbTemperature.value > dbTemperature.max) dbTemperature.max = dbTemperature.value;
	dbHumidity.value = shtxxGetLastMeasure(SHTXX_HUMIDITY);
#if defined(DEBUGGING)
	dbHumidity.value = now->second + 5;
#endif	
	if (dbHumidity.value < dbHumidity.min) dbHumidity.min = dbHumidity.value;
	if (dbHumidity.value > dbHumidity.max) dbHumidity.max = dbHumidity.value;

	if (dbTemperature.value < -10*DB_TEMPERATURE_OFFSET) dbActualTemperature = 0;
	else if (dbTemperature.value > 995) dbActualTemperature = 0xFE;
	else dbActualTemperature = ((dbTemperature.value/10 + DB_TEMPERATURE_OFFSET) << 1) + (dbTemperature.value%10 >= 5 ? 1 : 0);
	dbActualHumidity = (dbHumidity.value & 0x00FF);
}

dbSensor_t *dbGetSensor(uint8_t sensor)
{
	if (sensor == DB_TEMPERATURE) return &dbTemperature;
	if (sensor == DB_HUMIDITY) return &dbHumidity;
	return 0;
}

void dbResetSum(dbSum_t *s)
{
	s->temperature = 0;
	s->humidity = 0;
	s->count = 0;
}

void dbResetMinMax(dbMinMax_t *mm)
{
	mm->temperature.min = 0xFF;
	mm->temperature.max = 0;
	mm->humidity.min = 0xFF;
	mm->humidity.max = 0;
}

void dbReset(void)
{
	clock_t *now;
	dateStamp_t today;
	uint8_t i;
	uint16_t address;
	
	now = clockGet();
	if (now->year < dbInitialYear)
	{
		clockSet(dbInitialYear, 1, 1, 0, 0, 0);
		for (address=DB_DAYS_STORAGE; address<DB_END_STORAGE; address++) eepromWriteByte(address, 0xFF);
	}
	dateStampBuild(&today, now->year, now->month, now->day, dbInitialYear);
	// invalidate hours records if required
	if (dbHours[0].stamp.i != today.i)  
	{
		for (i=0; i<31; i++) dateStampInvalidate(&(dbHours[i].stamp));
		dbResetMinMax(&dbHoursMinMax);
	}
	// in all cases, reset the lower counters
	dbResetSum(&dbSecondSum);
	dbResetSum(&dbMinuteSum);
}

void dbUpdateSum(dbSum_t *s, uint8_t temperature, uint8_t humidity)
{
	s->temperature += temperature;
	s->humidity += humidity;
	s->count ++;
}

void dbUpdateMinMax(dbMinMax_t *mm, uint8_t temperature, uint8_t humidity)
{
	if (temperature < mm->temperature.min) mm->temperature.min = temperature;
	if (temperature > mm->temperature.max) mm->temperature.max = temperature;
	if (humidity < mm->humidity.min) mm->humidity.min = humidity;
	if (humidity > mm->humidity.max) mm->humidity.max = humidity;
}

void dbWriteRecord(uint16_t wAddress, dbSum_t *s, dbMinMax_t *mm, dateStamp_t *stamp)
{
	dbRecord_t record;

	record.temperature.average = s->temperature / s->count;
	record.humidity.average = s->humidity / s->count;
	record.temperature.min = mm->temperature.min;
	record.temperature.max = mm->temperature.max;
	record.humidity.min = mm->humidity.min;
	record.humidity.max = mm->humidity.max;
	record.stamp.b.ym = stamp->b.ym;
	eepromWriteBuffer(wAddress, (uint8_t *)&record, DB_REC_SIZE);
}

void dbCreateRecord(uint16_t wAddress, uint16_t rAddress, uint8_t count)
{
	uint8_t i;
	dbSum_t sum;
	dbMinMax_t minMax;
	dbRecord_t record;
	dateStamp_t date;
	clock_t *now;
	
	now = clockGet();
	dbResetSum(&sum);
	dbResetMinMax(&minMax);
	if (rAddress == DB_DAYS_STORAGE) dateStampBuild(&date, now->year, now->month - 1, 1, dbInitialYear);
	else dateStampBuild(&date, now->year - 1, 1, 1, dbInitialYear);
	for (i=0; i<count; i++)
	{
		eepromReadBuffer(rAddress+i*DB_REC_SIZE, (uint8_t *)&record, DB_REC_SIZE);
		if (record.stamp.b.ym == date.b.ym)
		{
			dbUpdateSum(&sum, record.temperature.average, record.humidity.average);
			dbUpdateMinMax(&minMax, record.temperature.min, record.humidity.min);
			dbUpdateMinMax(&minMax, record.temperature.max, record.humidity.max);
		}
	}
	date.b.ym = (date.b.ym & 0xF0) + 1; 	// only keep the year part
	dbWriteRecord(wAddress, &sum, &minMax, &date);
}

void dbStoreValues(void)
{
	uint8_t i, hour;
	clock_t *now;
	dbSum_t sum;
	dateStamp_t date;
	
	now = clockGet();
	switch (eventGetCurrentEvent())
	{
		case EVENT_TIME_SEC:
			dbUpdateSum(&dbSecondSum, dbActualTemperature, dbActualHumidity);
			break;
		case EVENT_TIME_MIN:
			dbUpdateSum(&dbMinuteSum, dbSecondSum.temperature/dbSecondSum.count, dbSecondSum.humidity/dbSecondSum.count);
			dbResetSum(&dbSecondSum);
			break;
		case EVENT_TIME_HOUR:
			hour = now->hour - 1;
			if (hour == 0xFF) hour = 23;
			dbHours[hour].temperature = dbMinuteSum.temperature/dbMinuteSum.count;
			dbHours[hour].humidity = dbMinuteSum.humidity/dbMinuteSum.count;
			dbResetSum(&dbMinuteSum);
			dateStampBuild(&(dbHours[hour].stamp), now->year, now->month, now->day - (now->hour == 0 ? 1 : 0), dbInitialYear);
			dbUpdateMinMax(&dbHoursMinMax, dbHours[hour].temperature, dbHours[hour].humidity);
			break;
		case EVENT_TIME_DAY:
			dateStampBuild(&date, now->year, now->month, now->day - 1, dbInitialYear);
			dbResetSum(&sum);
			// sum up the values which are matching the date stamp
			for (i=0; i<24; i++)
			{
				if (dbHours[i].stamp.i == date.i) 
					dbUpdateSum(&sum, dbHours[i].temperature, dbHours[i].humidity);
			}
			dbWriteRecord(DB_DAYS_STORAGE + (dateStampGetDay(&date)-1)*DB_REC_SIZE, &sum, &dbHoursMinMax, &date);
			dbResetMinMax(&dbHoursMinMax);
			break;
		case EVENT_TIME_MONTH:
			dateStampBuild(&date, now->year, now->month - 1, 1, dbInitialYear);
			dbCreateRecord(DB_MONTHS_STORAGE + (dateStampGetMonth(&date)-1)*DB_REC_SIZE, DB_DAYS_STORAGE, 31);
			break;
		case EVENT_TIME_YEAR:
			dateStampBuild(&date, now->year - 1, 1, 1, dbInitialYear);
			dbCreateRecord(DB_YEARS_STORAGE + dateStampGetYear(&date, 0)*DB_REC_SIZE, DB_MONTHS_STORAGE, 12);
			break;
	}
}

void dbGetSensorRecord(uint8_t sensor, uint16_t address, uint8_t *rvalue, uint8_t *rmin, uint8_t *rmax, dateStamp_t *rstamp)
{
	dbRecord_t record;

	if (address >= DB_HOURS_STORAGE)
	{
		if (sensor == DB_TEMPERATURE) (*rvalue) = dbHours[address - DB_HOURS_STORAGE].temperature;
		else if (sensor == DB_HUMIDITY) (*rvalue) = dbHours[address - DB_HOURS_STORAGE].humidity;
		rstamp->i = dbHours[address - DB_HOURS_STORAGE].stamp.i;
		(*rmin) = (*rvalue);
		(*rmax) = (*rvalue);
	} 
	else
	{
		eepromReadBuffer(address, (uint8_t *)&record, DB_REC_SIZE);
		rstamp->b.ym = record.stamp.b.ym;
		if (sensor == DB_TEMPERATURE) 
		{
			*rvalue = record.temperature.average;
			*rmin = record.temperature.min;
			*rmax = record.temperature.max;
		}
		else if (sensor == DB_HUMIDITY)
		{
			*rvalue = record.humidity.average;
			*rmin = record.humidity.min;
			*rmax = record.humidity.max;
		}
	}
}

void dbGetSensorData(uint8_t sensor, uint8_t period, dbValue_t *data, uint8_t *min, uint8_t *max, uint8_t *start, uint8_t *end)
{
	uint8_t i, rValue, rMin, rMax, index;
	dateStamp_t rDate, vDate;
	clock_t *now;
	
	for (i=0; i<31; i++) data[i].value = 0xFF;
	(*min) = 0xFF;
	(*max) = 0x00;
	now = clockGet();
	if (period == DB_HOUR)
	{
		(*end) = now->hour;
		(*start) = now->hour + 1;
		if ((*start) > 23) (*start) -= 24;
		index = (*start);
		i = 0;
		// start on the day before, since we are starting display next hour from now
		dateStampBuild(&vDate, now->year, now->month, now->day - 1, dbInitialYear);
		do
		{
			dbGetSensorRecord(sensor, DB_HOURS_STORAGE + index, &rValue, &rMin, &rMax, &rDate);
			if (rDate.i == vDate.i) 
			{
				data[i].value = rValue;
				if (rMin < (*min)) (*min) = rMin;
				if (rMax > (*max)) (*max) = rMax;
			}
			data[i].min = 0xFF;
			data[i].max = 0xFF;
			i++;
			if (++index > 23)
			{	 
				index = 0;
				// end up with the records from today
				dateStampBuild(&vDate, now->year, now->month, now->day, dbInitialYear);
			}
		} 
		while (index != (*end));
	}
	else if (period == DB_DAY)
	{
		(*end) = now->day;
		(*start) = now->day + 1;
		if ((*start) > 31) (*start) -= 31;
		index = (*start);
		i = 0;
		dateStampBuild(&vDate, now->year, now->month - 1, 1, dbInitialYear);
		do
		{
			dbGetSensorRecord(sensor, DB_DAYS_STORAGE + (index-1)*DB_REC_SIZE, &rValue, &rMin, &rMax, &rDate);
			if (rDate.b.ym == vDate.b.ym) 
			{
				data[i].value = rValue;
				data[i].min = rMin;
				data[i].max = rMax;
				if (rMin < (*min)) (*min) = rMin;
				if (rMax > (*max)) (*max) = rMax;
			}
			i++;
			if (++index > 31) 
			{
				index = 1;
				dateStampBuild(&vDate, now->year, now->month, 1, dbInitialYear);
			}
		} 
		while (index != (*end));
	}
	else if (period == DB_MONTH)
	{
		(*end) = now->month;
		(*start) = now->month + 1;
		if ((*start) > 12) (*start) -= 12;
		index = (*start);
		i = 0;
		dateStampBuild(&vDate, now->year - 1, 1, 1, dbInitialYear);
		do
		{
			dbGetSensorRecord(sensor, DB_MONTHS_STORAGE + (index-1)*DB_REC_SIZE, &rValue, &rMin, &rMax, &rDate);
			if (dateStampIsValid(&rDate))
			{
				if (rDate.b.ym == vDate.b.ym)
				{
					data[i].value = rValue;
					data[i].min = rMin;
					data[i].max = rMax;
					if (rMin < (*min)) (*min) = rMin;
					if (rMax > (*max)) (*max) = rMax;
				}
			}
			i++;
			if (++index > 12) 
			{
				index = 1;
				dateStampBuild(&vDate, now->year, 1, 1, dbInitialYear);
			}
		} 
		while (index != (*end));
	}
	else if (period == DB_YEAR)
	{
		(*start) = dbInitialYear;
		(*end) = (*start) + DB_NB_YEARS - 1;
		index = (*start);
		i = 0;
		while (index <= (*end))
		{
			dbGetSensorRecord(sensor, DB_YEARS_STORAGE + i*DB_REC_SIZE, &rValue, &rMin, &rMax, &rDate);
			dateStampBuild(&vDate, index, 1, 1, dbInitialYear);
			if (rDate.b.ym == vDate.b.ym) 
			{
				data[i].value = rValue;
				data[i].min = rMin;
				data[i].max = rMax;
				if (rMin < (*min)) (*min) = rMin;
				if (rMax > (*max)) (*max) = rMax;
			}
			i ++;
			index ++;
		}
	}
	
	// make sure we have valid data to show
	if ((*min) == 0xFF) (*min) = (sensor == DB_TEMPERATURE ? DB_TEMPERATURE_OFFSET << 1 : 0x00);
	if ((*max) == 0x00) (*max) = (sensor == DB_TEMPERATURE ? (50 + DB_TEMPERATURE_OFFSET) << 1 : 100);;	
	// make sure max > min
	if ((*min) >= (*max)) (*max) = (*min) + 1;
	// make sure the min & max values are even (to display a nice middle)
	(*min) = (*min) & 0xFE;
	(*max) += ((*max) & 0x01 ? 1 : 0);
}

void dbInit(void)
{
	// check that we start with a proper initial value
	dbInitialYear = eepromReadByte(DB_INITIAL_YEAR);
	if (dbInitialYear == 0xFF) 
	{
		eepromWriteByte(DB_INITIAL_YEAR, CLOCK_FIRST_YEAR);
		dbInitialYear = CLOCK_FIRST_YEAR;
	}

	dbTemperature.min = 32767;
	dbTemperature.max = -32767;
	dbHumidity.min = 32767;
	dbHumidity.max = -32767;

	dbHours[0].stamp.i = 0;
	dbReset();
	
	eventRegisterHandler(dbReset, EVENT_TIME_ADJUST);
	eventRegisterHandler(dbRetrieveValues, EVENT_SENSOR);
	eventRegisterHandler(dbStoreValues, EVENT_TIME);
}
