//----------------------------------------------------------------------------------
//
// Sensirion SHTxx Humidity Sensor Library
//
// Library for using the SHT1x humidity and temperature
// sensors from Sensirion (http://www.sensirion.com).
// Based on Sensirion application note "Sample Code SHTxx".
//
// To use:
// - supply 5V to SHTxx (constants in calculations assume 5V, see ShtCalculate() and SHTxx datasheet)
// - connect the clock pin of the SHTxx to pin B1 (see Sht.h to change)
// - connect the data pin of the SHTxx to pin B2 (see Sht.h to change)
//
// - call ShtInit() to initialize pins, call when the processor starts
// - call ShtMeasure(MEASURE_TEMP) and ShtMeasure(MEASURE_HUMIDITY) to make the measurements
// - call ShtCalculate() to convert measurements to real-world units
//
// - call ShtReadStatus() and ShtWriteStatus() to modify the status register
//
//
// ToDo:
// - verify checksum digits sent from SHTxx
// - implement soft-reset
// - handle 12/8-bit temp/humidity readings
//
// History:
// 2003-Jul-03	BL	- Created
//
//----------------------------------------------------------------------------------

#include <avr/io.h>
#include "sht.h"
#include "delay.h"


void enable_data(void) 		{ sbi(SHT_DDR, SHT_DATA);	}	// Drive DATA pin
void disable_data(void)		{ cbi(SHT_DDR, SHT_DATA); 		// Release DATA pin
							  sbi(SHT_PORT, SHT_DATA); }
void data_high(void)		{ sbi(SHT_PORT, SHT_DATA); }	// DATA pin high
void data_low(void)			{ cbi(SHT_PORT, SHT_DATA); }	// DATA pin low
void clock_high(void)		{ sbi(SHT_PORT, SHT_CLOCK); } 	// CLOCK pin high
void clock_low(void)		{ cbi(SHT_PORT, SHT_CLOCK); }	// CLOCK pin low



//----------------------------------------------------------------------------------
// Initialize AVR i/o pins.
//----------------------------------------------------------------------------------
void ShtInit(void)
{
	sbi(SHT_DDR, SHT_CLOCK);		// Set clock pin to output
	cbi(SHT_PORT, SHT_CLOCK);		// Turn off clock pin
	disable_data();
}

//----------------------------------------------------------------------------------
// generates a transmission start
//       _____         ________
// DATA:      |_______|
//           ___     ___
// SCK : ___|   |___|   |______
//
//----------------------------------------------------------------------------------
void transstart(void)
{
	enable_data();
	data_high();
	clock_low();
	DELAY(SHT_DELAY);
	DELAY(SHT_DELAY);

	clock_high(); DELAY(SHT_DELAY);
	data_low();	DELAY(SHT_DELAY);
	clock_low();	DELAY(SHT_DELAY); DELAY(SHT_DELAY);
	clock_high();	DELAY(SHT_DELAY);
	data_high();	DELAY(SHT_DELAY);
	clock_low();


}

//----------------------------------------------------------------------------------
// communication reset: DATA-line=1 and at least 9 SCK cycles followed by transstart
//       _____________________________________________________         ________
// DATA:                                                      |_______|
//          _    _    _    _    _    _    _    _    _        ___     ___
// SCK : __| |__| |__| |__| |__| |__| |__| |__| |__| |______|   |___|   |______
//----------------------------------------------------------------------------------
void ShtReset(void)
{
	unsigned char i;
	enable_data();
	data_high();
	clock_low();
	for(i=0;i<10;i++) //9 SCK cycles
	{
		clock_high();
		Delay(SHT_DELAY);
		clock_low();
		Delay(SHT_DELAY);
	}
	transstart(); //transmission start
}

//----------------------------------------------------------------------------------
// reads a byte form the Sensibus and gives an acknowledge in case of "ack=1"
//----------------------------------------------------------------------------------
char read_byte(void)
{
	unsigned char i,val=0;
	disable_data();

	// Read in 8 bits, LSB first
	for (i=0x80;i>0;i/=2)
	{
		clock_high();
		DELAY(SHT_DELAY);
		if (bit_is_set(SHT_PIN, SHT_DATA))
			val=(val | i); //read bit
		clock_low();
	}

	// Send ACK
	enable_data();
	data_low();
	clock_high();
	DELAY(SHT_DELAY);
	clock_low();
	disable_data();

	return val;
}

//----------------------------------------------------------------------------------
// Writes a byte on the Sensibus and checks the acknowledge.
// Returns 0 if the successful
//----------------------------------------------------------------------------------
char write_byte(unsigned char value)
{
	unsigned char i;
	unsigned char error = 0;

	// Write each bit one at a time, LSB first
	enable_data();
	for (i=0x80;i>0;i/=2)
	{
		if (i & value)
			data_high();
		else
			data_low();

		clock_high();
		DELAY(SHT_DELAY);
		DELAY(SHT_DELAY);
		clock_low();

	}
	disable_data();

	// Read ACK
	clock_high();
	DELAY(SHT_DELAY);
	error = bit_is_set(SHT_PIN, SHT_DATA);
	clock_low();

	return error; //error=1 in case of no acknowledge
}

//----------------------------------------------------------------------------------
// Read humidity or temperature from the sensor.
// Returns the value in ticks. Use sht_calc() to convert to real world units.
// Returns 0xFFFF if the measurment failed
//----------------------------------------------------------------------------------
int ShtMeasure(unsigned char mode)
{
	unsigned int 	temp = 0xFFFF;
	unsigned char	checksum;
	unsigned char 	c;

	// Signal start of communications
	transstart();

	// Request measurement
	write_byte(mode);

	// Sensor lowers the data line when measurement
	// is complete. Wait up to 2 seconds for this.
	for (c=0; c<20; c++)
	{
		if (! bit_is_set(SHT_PIN, SHT_DATA))
			break;
		DELAY(1000000/10);
	}

	// Read the measurement
	if (! bit_is_set(SHT_PIN, SHT_DATA))
	{
		temp = read_byte();
		temp = temp << 8;
		temp += read_byte();
		checksum = read_byte();
	}

	return temp;
}

//----------------------------------------------------------------------------------------
// Calculates tempurature in ^C and humidity in %RH (temperature compensated)
// sht_measure() returns temp and humidity in ticks. Use this function to convert
// to compensated values in real world units.
//
// This function returns integers with 2 assumed decimal places. For example 2550
// means 25.50. This is to avoid including the floating point math library.
//
// input :	humi [Ticks] (12 bit)
// 			temp [Ticks] (14 bit)
// output: 	humi [%RH] (2 fixed decimals)
// 			temp [^C]  (2 fixed decimals)
//----------------------------------------------------------------------------------------
void ShtCalculate(int *p_temperature, int *p_humidity)
{
	const long D1x100 = -40 * 100;					// for 5V power
	const long D2x100 = 0.01 * 100;					// for 14bit temp
	const long C1x100 = -4 * 100;					// for 12bit humidity
	const long C2x10000 = 0.0405 * 10000;			// for 12bit humidity
	const long C3x10000000 = -0.0000028 * 10000000;	// for 12bit humidity
	const long T1x100000 = 0.01 * 100000;			// for 12bit humidity
	const long T2x100000 = 0.00008 * 100000;		// for 12bit humidity

	long t = *p_temperature;			// temperatere in ticks from sensor
	long rh = *p_humidity;				// humidity in ticks from sensor

	long t_C;							// temperature in celcius: 2 fixed decimals
	long rh_lin;						// relative humidity: 2 fixed decimals
	long rh_true;						// temp compensated humidity: 2 fixed decimals

	t_C = D1x100 + D2x100*t;			// calculate tempurature in celcius from ticks

	rh_lin = (C3x10000000*rh*rh)/100000 + (C2x10000*rh)/100 + C1x100;
	rh_true = ((t_C-(25*100)) * (T1x100000 + T2x100000*rh))/100000 + rh_lin;

	if(rh_true>10000)rh_true=10000; 	//cut if the value is outside of
	if(rh_true<10)rh_true=10; 			//the physical possible range

	*p_temperature=(int)t_C; 			//return temperature [^C]
	*p_humidity=(int)rh_true;	 		//return humidity[%RH]
}

//----------------------------------------------------------------------------------
// Reads the status register with checksum (8-bit)
//----------------------------------------------------------------------------------
char ShtReadStatus(unsigned char *p_value)
{
	unsigned char error=0;
	unsigned char checksum=0;

	transstart(); 					//transmission start
	error = write_byte(STATUS_REG_R); 	//send command to sensor
	*p_value = read_byte(); 			//read status register (8-bit)
	checksum = read_byte(); 			//read checksum (8-bit)
	return error; 						//error=1 in case of no response form the sensor
}


//----------------------------------------------------------------------------------
// Writes the status register . Note this library only supports the default
// 14 bit temp and 12 bit humidity readings.
//----------------------------------------------------------------------------------
char ShtWriteStatus(unsigned char value)
{
	unsigned char error=0;
	transstart(); 					//transmission start
	error += write_byte(STATUS_REG_W);	//send command to sensor
	error += write_byte(value); 		//send value of status register
	return error; 						//error>=1 in case of no response form the sensor
}
