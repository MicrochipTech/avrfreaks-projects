/** @file sensors.h Header file for Sensors module.
*/

#ifndef _SENSORS_HEADER__
#define _SENSORS_HEADER__

/*******************************************************************************
* Definitiosn
*******************************************************************************/

#define DEGREE_CELCIUS		0
#define DEGREE_FAHRENHEIT	1

/*******************************************************************************
* Type definitions
*******************************************************************************/

/** Sensor type enumeration */
typedef enum
{
	TEMPERATURE_SENSOR = 0,
	VOLTAGE_SENSOR = 1,
	LIGHT_SENSOR = 2,
	NO_SENSOR
} SensorType;

/** Temperature union. Depending on the selected degree type, one of the two
	fields holds valid information.
*/
typedef union
{
	/** Temperature in Celcius degrees (-15..60) */
	int8_t celcius;

	/** Temperature in Fahrenheit degrees (0..140) */
	uint8_t fahrenheit;

} Temperature;

/*******************************************************************************
* Exported functions
*******************************************************************************/

extern void SensorsInit(void);
extern void SensorsConfigure(uint8_t degreeFmt);
extern void SensorsSelect(SensorType sensor);
extern Temperature SensorsReadTemperature(void);
extern uint8_t SensorsReadVoltage(void);
extern uint16_t SensorsReadLight(void);
extern void SensorsDisplayMeasurement(void);

#endif //_SENSORS_HEADER__

