/*****************************************************************************\
|* Example program showing off the port to gcc of the Atmel library to read
|* a one-wire device. No support for the interrupt-driven UART interface, this
|* just needs 1 digital i/o pin (I likes to use my usarts for other things :)
\*****************************************************************************/

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include <util/delay.h>
#include <avr/io.h>
#include <avr/interrupt.h> 
#include <avr/eeprom.h> 
 
#include "1wire.h"
#include "uart.h"

/*****************************************************************************\
|* Function prototypes
\*****************************************************************************/
int16_t ds18b20ReadTemperature(uint8_t bus, uint8_t * id);

/*****************************************************************************\
|* Defines for the temperature conversion
\*****************************************************************************/
#define DS18B20_FAMILY_ID                0x28 
#define DS18B20_START_CONVERSION         0x44
#define DS18B20_READ_SCRATCHPAD          0xbe
#define DS18B20_ERROR                    -1000

/*****************************************************************************\
|* Make the 1-wire "bus" on pin-7 of port ONEWIRE_PORT (see 1wire.h)
\*****************************************************************************/
#define BUS 							ONEWIRE_PIN_7
#define MAX_DEVICES 					8

/*****************************************************************************\
|* Set up stdio
\*****************************************************************************/
FILE _io = FDEV_SETUP_STREAM(uart_putchar, uart_getchar, _FDEV_SETUP_RW);

/*****************************************************************************\
|* Initialise the firmware - program entry point
\*****************************************************************************/
int main (void) 
	{
	signed int temp = 0;
	
	/*************************************************************************\
	|* Set up the uart
	\*************************************************************************/
	initUart();
    stdout = stdin = &_io;
	printf("Booting!\n");
		
	/*************************************************************************\
	|* Declare the devices and initialise
	\*************************************************************************/
	static oneWireDevice devices[MAX_DEVICES];
	oneWireDevice *ds18b20;
	oneWireInit(BUS);
	
	/*************************************************************************\
	|* Search for and populate the devices until all CRC's are read correctly
	\*************************************************************************/
	printf("Searching...\n");
	while (oneWireSearchBuses(devices, MAX_DEVICES, BUS) != ONEWIRE_SEARCH_COMPLETE)
		;
	
	/*************************************************************************\
	|* After device enumeration, find the temperature sensor
	\*************************************************************************/
	printf("Finding temperature sensor..\n");
	ds18b20 = oneWireFindFamily(DS18B20_FAMILY_ID, devices, MAX_DEVICES);
	
	/*************************************************************************\
	|* Just print the temperature constantly
	\*************************************************************************/
	while (1)
		{
		if (ds18b20 != NULL)
			temp 	= ds18b20ReadTemperature(ds18b20->bus, ds18b20->id);
		
		uint16_t fraction	= 625 * (temp & 0x0f);
		temp				= temp >> 4;
		printf("Temperature: %d.%04d\n", temp, fraction);
		_delay_ms(3000);
		}
	}


/*****************************************************************************\
|* Read the temperature from an identified sensor
\*****************************************************************************/
int16_t ds18b20ReadTemperature(uint8_t bus, uint8_t * id)
	{
    int16_t temperature;
    
    // Reset, presence.
    if (!oneWireDetectPresence(bus))
        return DS18B20_ERROR; 
		
    // Match the id found earlier.
    oneWireMatchRom(id, bus);
	
    // Send start conversion command.
    oneWireSendByte(DS18B20_START_CONVERSION, bus);
    
	// Wait until conversion is finished.
    // Bus line is held low until conversion is finished.

#ifdef ONEWIRE_USE_PARASITIC_POWER
	ONEWIRE_RELEASE_BUS(bus);
	_delay_ms(850);
	
#else
    while (!oneWireReadBit(bus))
    	;
#endif		
    // Reset, presence.
    if(!oneWireDetectPresence(bus))
        return DS18B20_ERROR;
    
    // Match id again.
    oneWireMatchRom(id, bus);
	
    // Send READ SCRATCHPAD command.
    oneWireSendByte(DS18B20_READ_SCRATCHPAD, bus);
	
    // Read only two first bytes (temperature low, temperature high)
    // and place them in the 16 bit temperature variable.
    temperature = oneWireReceiveByte(bus);
    temperature |= (oneWireReceiveByte(bus) << 8);
    
    return temperature;
	}
