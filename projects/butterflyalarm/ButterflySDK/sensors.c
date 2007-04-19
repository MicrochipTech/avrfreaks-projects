/** @file sensors.c Sensors module. Provide routines for performing sensors
	measurements (temperature,light, voltage) and displaying the results.
*/

#include <avr/io.h>
#include <avr/pgmspace.h>

#include "c_ext.h"
#include "adc.h"
#include "sensors.h"
#include "lcd.h"
#include "bcd.h"

/*******************************************************************************
* Look-up Tables
*******************************************************************************/

#define NTC_CELCIUS_POSITIVE_TABLE_SIZE		61
#define NTC_CELCIUS_NEGATIVE_TABLE_SIZE		15
#define NTC_FAHRENHEIT_TABLE_SIZE			141

#define NTC_CELCIOUS_ZERO_VALUE_HIGH 		810
#define NTC_CELCIOUS_ZERO_VALUE_LOW			800

/** Positive Celcius temperatures look-up table. Holds the ADC values that
	corresponds to temperatures 0 to 60 Celcius degrees.
*/
const int NTCCelciusPositiveTable[NTC_CELCIUS_POSITIVE_TABLE_SIZE] PROGMEM =
	{
		806,796,786,775,765,754,743,732,720,709,697,685,673,661,649,
		636,624,611,599,586,574,562,549,537,524,512,500,488,476,464,
		452,440,429,418,406,396,385,374,364,354,344,334,324,315,306,
		297,288,279,271,263,255,247,240,233,225,219,212,205,199,193,
		187,
	};

/** Positive Celcius temperatures look-up table. Holds the ADC values that
	corresponds to temperatures -1 to -15 Celcius degrees.
*/
const int NTCCelciusNegativeTable[NTC_CELCIUS_NEGATIVE_TABLE_SIZE] PROGMEM =
	{
		815,825,834,843,851,860,868,876,883,891,898,904,911,917,923,
	};

/** Positive Farenheit temperatures look-up table. Holds the ADC values that
	corresponds to temperatures 0 to 140 Fahrenheit degrees.
*/
const int NTCFahrenheitTable[NTC_FAHRENHEIT_TABLE_SIZE] PROGMEM =
	{
		938, 935, 932, 929, 926, 923, 920, 916, 913, 909, 906, 902, 898,
		894, 891, 887, 882, 878, 874, 870, 865, 861, 856, 851, 847, 842,
		837, 832, 827, 822, 816, 811, 806, 800, 795, 789, 783, 778, 772,
		766, 760, 754, 748, 742, 735, 729, 723, 716, 710, 703, 697, 690,
		684, 677, 670, 663, 657, 650, 643, 636, 629, 622, 616, 609, 602,
		595, 588, 581, 574, 567, 560, 553, 546, 539, 533, 526, 519, 512,
		505, 498, 492, 485, 478, 472, 465, 459, 452, 446, 439, 433, 426,
		420, 414, 408, 402, 396, 390, 384, 378, 372, 366, 360, 355, 349,
		344, 338, 333, 327, 322, 317, 312, 307, 302, 297, 292, 287, 282,
		277, 273, 268, 264, 259, 255, 251, 246, 242, 238, 234, 230, 226,
		222, 219, 215, 211, 207, 204, 200, 197, 194, 190, 187,
	};

#define LIGHT_ADC_TABLE_SIZE	21

/** vRef look-up table. Used to find the vRef from the light sensor
	measurement, when then voltage-reading function is being used */
const int LightADCTable[LIGHT_ADC_TABLE_SIZE] PROGMEM =
	{
		0x35,0x60,0x80,0x0B0,0x11D,0x13D,0x15A,0x17A,0x197,0x1B9,0x1DA,
		0x1F9,0x216,0x240,0x26D,0x282,0x2A2,0x2EF,0x332,0x3B0,0x3F2
	};

/** Light voltage look-up table. Precalculated as vRef * 600. Used from
	the voltage-reading function.
*/
const uint16_t LightVoltageTable[LIGHT_ADC_TABLE_SIZE] PROGMEM =
	{
		1691, 1692, 1694, 1696, 1699, 1701, 1703, 1705, 1706, 1708, 1710,
		1712, 1714, 1718, 1720, 1722, 1724, 1729, 1736, 1750, 1763
	};

/*******************************************************************************
* Static variables
*******************************************************************************/

/** Temperature degree format: 0 Celcius, 1 Fahrentype */
static uint8_t degreeFormat;

/** Type of sensor selected */
static SensorType sensorType;

/*******************************************************************************
* Functions
*******************************************************************************/

/** Initializes the Sensors module */
void SensorsInit(void)
{
	degreeFormat = DEGREE_CELCIUS;
	sensorType = NO_SENSOR;
}

/** Selects the sensor type.
	@param sensor The type of the sensor selected (Temperature, voltage, light)
*/
void SensorsSelect(SensorType sensor)
{
	sensorType = sensor;

	if (sensor != NO_SENSOR)
	{
		ADCInit((uint8_t) sensor);
	}
	else
	{
		ADCDisable();
	}
}

/** Configures the temperature degree type.
	@param degreeFmt 0 for Celcius degrees, 1 for Fahrenheit
*/
void SensorsConfigure(uint8_t degreeFmt)
{
	degreeFormat = degreeFmt;
}

/** Performs a temperature measurement.
	@result The temperature measured
*/
Temperature SensorsReadTemperature(void)
{
	Temperature temp;
	uint8_t i;
	uint16_t valueADC = ADCRead();

	temp.celcius = 0;

	if(degreeFormat == DEGREE_CELCIUS)
	{
		if(valueADC > NTC_CELCIOUS_ZERO_VALUE_HIGH)
		{
			for (i=0; i<=NTC_CELCIUS_NEGATIVE_TABLE_SIZE; i++)
			{
				if (valueADC <= pgm_read_word(&NTCCelciusNegativeTable[i]))
				{
					break;
				}
			}
			temp.celcius = -i-1;
		}
		else if (valueADC < NTC_CELCIOUS_ZERO_VALUE_LOW)
		{
			for (i=0; i<NTC_CELCIUS_POSITIVE_TABLE_SIZE; i++)
			{
				if (valueADC >= pgm_read_word(&NTCCelciusPositiveTable[i]))
				{
					break;
				}
			}
			temp.celcius = i;
		}
	}
	else if (degreeFormat == DEGREE_FAHRENHEIT)
	{
		for (i=0; i<=NTC_FAHRENHEIT_TABLE_SIZE; i++)
		{
			if (valueADC > pgm_read_word(&NTCFahrenheitTable[i]))
			{
				break;
			}
		}
		temp.fahrenheit = i;
	}

	return temp;
}

/** Performs a voltage measurement
	@result A number between 0 and 50. Multiplying this number with 100mV
		gives the actual voltage
*/
uint8_t SensorsReadVoltage(void)
{
	uint16_t valueADC, valueADCTemp;
	uint8_t i;
	uint32_t result;
	uint16_t vRef;

	valueADC = ADCRead();

	//  Do a Light-measurement first to determine the vRef,
	//  because the LDR affects the vRef

	valueADCTemp = valueADC;

	ADCInit(LIGHT_SENSOR);
	valueADC = ADCRead();

	// Find vRef
	for (i=0; i<LIGHT_ADC_TABLE_SIZE; i++)
	{
		if (valueADC <= pgm_read_word(&LightADCTable[i]))
		{
			break;
		}
	}
	if(!i)
	{
		// Too bright
		//vRef = 2.815;
		vRef = 1689; // vRef * 600
	}
	else if(i >= LIGHT_ADC_TABLE_SIZE)
	{
		//Totally dark
		//vRef = 2.942;
		vRef = 1765; // vRef * 600
	}
	else
	{
		vRef = pgm_read_word(&LightVoltageTable[i]);
	}

	// Light-measurement finished - restore value
	ADCInit(VOLTAGE_SENSOR);
	valueADC = valueADCTemp;

	// Calculate the voltage
	result = ((uint32_t) valueADC) * ((uint32_t) vRef);

	result = result >> 10; // Divide with 1024

	if (result > 500)
		return 50;

	return (uint8_t) (result/10);
}

/** Performs a light measurement.
	@result The value of the ADC register
*/
uint16_t SensorsReadLight(void)
{
	// The relation between ADC-value and lux is yet to be found.
	// The ADC value is returned.
	uint16_t valueADC;

	valueADC = ADCRead();

	return valueADC;
}

/** Displays the result of the current measurement in the LCD screen */
void SensorsDisplayMeasurement(void)
{
	if (sensorType == TEMPERATURE_SENSOR)
	{
		Temperature temp = SensorsReadTemperature();

		if (degreeFormat == DEGREE_CELCIUS)
		{
			uint8_t bcdValue;

			LcdWriteChar(0, ' ');
			if (temp.celcius < 0)
			{
				LcdWriteChar(1,'-');
				temp.celcius = -temp.celcius;
			}
			else
				LcdWriteChar(1,' ');

			// Assume temp <= 99 C
			bcdValue = CharToBCD2(temp.celcius);

			LcdWriteChar(2, BCD_HIGH_NIBBLE_CHAR(bcdValue));
			LcdWriteChar(3, BCD_LOW_NIBBLE_CHAR(bcdValue));
			LcdWriteChar(4,'*');
			LcdWriteChar(5,'C');
		}
		else
		{
			uint16_t bcdValue;

			LcdWriteChar(0, ' ');

			bcdValue = CharToBCD3(temp.fahrenheit);

			if (temp.fahrenheit > 99)
				LcdWriteChar(1, BCD_LOW_NIBBLE_CHAR(bcdValue >> 8));
			else
				LcdWriteChar(1, ' ');
			bcdValue = bcdValue & 0xFF;
			LcdWriteChar(2, BCD_HIGH_NIBBLE_CHAR(bcdValue));
			LcdWriteChar(3, BCD_LOW_NIBBLE_CHAR(bcdValue));
			LcdWriteChar(4,'*');
			LcdWriteChar(5,'F');
		}
		LcdWriteChar(6,'\0');
	}
	else if (sensorType == VOLTAGE_SENSOR)
	{
		uint8_t voltage;

		voltage = SensorsReadVoltage();
		voltage = CharToBCD2(voltage);

		LcdWriteChar(0, ' ');
		LcdWriteChar(1, ' ');
		LcdWriteChar(2, ' ');
		LcdWriteChar(3, BCD_HIGH_NIBBLE_CHAR(voltage));
		LcdWriteChar(4, 'V');
		LcdWriteChar(5, BCD_LOW_NIBBLE_CHAR(voltage));
		LcdWriteChar(6, '\0');
	}
	else if (sensorType == LIGHT_SENSOR)
	{
		uint16_t light = SensorsReadLight();
		uint8_t high, low;

		LcdWriteChar(0, 'A');
		LcdWriteChar(1, 'D');
		LcdWriteChar(2, 'C');
		LcdWriteChar(3, '0' + (light >> 8));

		high = ((uint8_t) (light & 0xF0)) >> 4;
		low = ((uint8_t) (light & 0xF));

		LcdWriteChar(4, high > 9 ? 'A' + high - 10 : '0' + high);
		LcdWriteChar(5, low > 9 ? 'A' + low - 10 : '0' + low);
		LcdWriteChar(6, '\0');
	}
}
