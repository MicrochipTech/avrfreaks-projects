#include <inttypes.h>
#include <avr/delay.h>

#include "hardware.h"
#include "shtxx.h"
#include "event.h"

#define SHTXX_MEASURE_INTERVAL 750 	/* how often to read the sensors, in ms */

// states for the shtxx
#define SHTXX_IDLE 					0
//		SHTXX_TEMPERATURE
//		SHTXX_HUMIDITY
//		SHTXX_RESET

int16_t shtxxTemperature;
int16_t shtxxHumidity;
uint8_t shtxxState;
uint8_t shtxxTickCount;
uint16_t shtxxTemperatureDbg;
uint16_t shtxxHumidityDbg;


// We use an external pullup on D, with D pin value as O
// 	- setting D as an input will generate a 1 through the pullup
//	- setting D as an output will generate a 0 
#define shtxxClearD() SHTXX_D_DDR |= _BV(SHTXX_D)
#define shtxxSetD() SHTXX_D_DDR &= ~_BV(SHTXX_D)
#define shtxxReleaseD() SHTXX_D_DDR &= ~_BV(SHTXX_D)

#define shtxxSetCLK() SHTXX_CLK_PORT |= _BV(SHTXX_CLK)
#define shtxxClearCLK() SHTXX_CLK_PORT &= ~_BV(SHTXX_CLK)
#define shtxxTinyDelay()	asm volatile ("nop\n nop\n")
#define shtxxStartupDelay() _delay_ms(15)
#define shtxxSmallDelay() _delay_us(1)
#define shtxxDelay() _delay_us(5)
#define shtxxDIsSet() bit_is_set(SHTXX_D_PIN, SHTXX_D)


uint8_t shtxxTransmit(uint8_t value) 
{ 
	uint8_t i, error = 0;  

	shtxxState = value;
	for (i=0x80; i>0; i/=2)
  	{ 
		if (i & value) shtxxSetD();	
		else shtxxClearD();
		shtxxSetCLK();
		shtxxDelay();
		shtxxClearCLK();
		shtxxTinyDelay();
	}
	// Check for acknowledge
	shtxxReleaseD();
	shtxxSetCLK();
	shtxxTinyDelay();
	error = shtxxDIsSet();
	shtxxDelay();
	shtxxClearCLK();
	return error; 
}

uint8_t shtxxReceive(uint8_t acknowledge) 
{ 
	uint8_t i, value = 0;

	for (i=0x80; i>0; i/=2)
    { 
		shtxxSetCLK();
		if (shtxxDIsSet()) value = (value | i);
		shtxxDelay();
		shtxxClearCLK();
		shtxxSmallDelay();
  	}
	if (acknowledge) shtxxClearD();
	else shtxxSetD();
	shtxxSetCLK();
	shtxxDelay();
	shtxxClearCLK();
	shtxxReleaseD();
	return value;
}

//----------------------------------------------------------------------------------
// Generates a transmission start 
//       _____         ________
// D :        |_______|
//           ___     ___
// CLK : ___|   |___|   |______
//----------------------------------------------------------------------------------
void shtxxTransmitStart(void)
{     
	shtxxSetD();
	shtxxClearCLK();
	shtxxSmallDelay(); shtxxSetCLK();
	shtxxSmallDelay(); shtxxClearD();
	shtxxSmallDelay(); shtxxClearCLK();
	shtxxDelay(); shtxxSetCLK();
	shtxxSmallDelay(); shtxxSetD();
	shtxxSmallDelay(); shtxxClearCLK();
}

//----------------------------------------------------------------------------------
// Communication reset: D=1 and at least 9 CLK cycles followed by TransmissionStart
//       _____________________________________________________         ________
// DATA:                                                      |_______|
//          _    _    _    _    _    _    _    _    _        ___     ___
// CLK : __| |__| |__| |__| |__| |__| |__| |__| |__| |______|   |___|   |______
//----------------------------------------------------------------------------------
void shtxxReset(void)
{  
	uint8_t i;

	shtxxSetD();
	shtxxClearCLK();
	for (i=0; i<9; i++)
	{
		shtxxSetCLK();
		shtxxSmallDelay();
		shtxxClearCLK();
	}
	shtxxTransmitStart();
	shtxxTransmit(SHTXX_RESET);
	shtxxStartupDelay();
}

void shtxxStartMeasure(uint8_t sensor)
{
	shtxxState = sensor;
	// send the start condition
	shtxxTransmitStart();
	// send the channel
	if (shtxxTransmit(sensor) == 0) return;
	// if error, return to idle state
	shtxxState = SHTXX_IDLE;
}

void shtxxRetrieveMeasure(uint16_t *measure, uint8_t *crc)
{
	// receive the first 8 bits of measure
	*measure = shtxxReceive(1) << 8;
	// receive the last eight bits
	*measure += shtxxReceive(1);	
	// receive the CRC
	*crc = shtxxReceive(0);
}

void shtxxMeasure(void) 
{
	shtxxTickCount ++;
	if (shtxxState == SHTXX_IDLE) 
	{
		// if we match the measure interval, start the measure
		if (shtxxTickCount >= SHTXX_MEASURE_INTERVAL/50)
		{
			shtxxTickCount = 0;
			shtxxStartMeasure(SHTXX_TEMPERATURE);
		}
		return;
	}
	// return if measure is not complete
	if (shtxxDIsSet()) return;
	
	if (shtxxState == SHTXX_TEMPERATURE)
	{
		uint8_t crc;
		
		shtxxRetrieveMeasure(&shtxxTemperatureDbg, &crc);
		
		// t°C = -40 + 0.01*SOT
		// we store 10 * t°C to ease display
		shtxxTemperature = shtxxTemperatureDbg / 10 - 400;
		// start humidity
		shtxxStartMeasure(SHTXX_HUMIDITY);
	} 
	else if (shtxxState == SHTXX_HUMIDITY)
	{
		int32_t temp;
		uint16_t hMeasure;
		uint8_t crc;

		shtxxRetrieveMeasure(&shtxxHumidityDbg, &crc);
	
		// RH%NT = -4 + 0.0405*SORH -2.8e-6*SORH*SORH
		hMeasure = shtxxHumidityDbg;
		temp = hMeasure * hMeasure * 28 / 10000000;
		temp = hMeasure * 405 / 10000 - 4 - temp;
		shtxxHumidity = temp;
		// RH% = (t°C - 25)*(0.01 + 0.00008*SORH) + RH%NT
		temp = (shtxxTemperature - 250) * (1 + 8 * hMeasure/1000) / 1000;
		shtxxHumidity = temp + shtxxHumidity; 
		if (shtxxHumidity < 0) shtxxHumidity = 0;
		if (shtxxHumidity > 100) shtxxHumidity = 100;

		eventAddEvent(EVENT_SENSOR);
		shtxxState = SHTXX_IDLE;
	}
}

// ----------------- Exposed methods ------------------------------

int16_t shtxxGetLastMeasure(uint8_t sensor) 
{
	switch (sensor)
	{
		case SHTXX_TEMPERATURE:
			return shtxxTemperature;
		case SHTXX_HUMIDITY:
			return shtxxHumidity;
/*
		case SHTXX_TEMPERATURE_DBG:
			return shtxxTemperatureDbg;
		case SHTXX_HUMIDITY_DBG:
			return shtxxHumidityDbg;
*/
		default:
			return 0;
	}
}

void shtxxInit(void) 
{
	shtxxState = SHTXX_IDLE;
	shtxxTickCount = 0;
	shtxxHumidity = 0x00FF;
	
	SHTXX_CLK_DDR |= _BV(SHTXX_CLK);							// define CLK as output
	shtxxClearCLK();											// Set it to low state
	SHTXX_D_PORT &= ~_BV(SHTXX_D);								// clear D pin
	shtxxStartupDelay();										// wait a minimum of 11ms before starting
	eventRegisterHandler(shtxxMeasure, EVENT_TIME_50MS); 		// check to see if we need to measure every 50ms
	shtxxReset();												// reset to avoid the effects of a bad rise of VDD
	shtxxStartMeasure(SHTXX_TEMPERATURE);						// start a measure
}
