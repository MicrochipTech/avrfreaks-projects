#include <stdlib.h>
#include <inttypes.h>
#include <avr/io.h>
#include <avr/eeprom.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <util/delay.h>
#include <math.h>
#include "i2cmaster.h"
#include "uart.h"

// change to your settings if needed.
#define UART_BAUD_RATE      9600     
 
#define L3G4200D_ADDRESS1	0xD0
#define L3G4200D_ADDRESS2	0xD2
#define L3G4200D_WHOAMI		0xD3

int16_t gyroscope[3];   

char buffer[32];

void sleep(uint16_t ms)
{
	while(ms>0)
	{
		_delay_ms(1);
		ms=ms-1;
	}
}

void outputHex(int16_t v)
{
	itoa(v, buffer, 16);   // convert interger into string (decimal format)         
	if (v<0x10) uart_puts_P("0x0");
	else uart_puts_P("0x");
	uart_puts(buffer);
}

uint8_t checkI2CDeviceIsResponding(uint8_t addr)
{
    uint8_t value = i2c_start(addr+I2C_WRITE); // try to write to device, but doesn't actually write anything.
    i2c_stop(); // note - don't send byte to write.
    return (value==0);  // if start command is successfull, then a device has responded.
}

uint8_t readI2Cbyte(uint8_t address, uint8_t location)
{
	uint8_t value;
    i2c_start(address+I2C_WRITE); // first, indicate where to read from
    i2c_write(location);
    i2c_start(address+I2C_READ); // repeat start and read
    value = i2c_readNak(); // one byte.
    i2c_stop();
    return value;
}

void writeI2Cbyte(uint8_t address, uint8_t location, uint8_t value)
{
    i2c_start(address+I2C_WRITE); // write mode
    i2c_write(location); // tell where to write
    i2c_write(value); // give byte to write.
    i2c_stop();
}

void readI2CbyteArray(uint8_t address, uint8_t location, uint8_t *array,uint8_t size)
{
    i2c_start(address+I2C_WRITE);
    i2c_write(location);  // start from first register.
    i2c_start(address+I2C_READ);
	int i=0;
	while(i<size)
	{
		if ((i+1)!=size)
			array[i]=i2c_readAck();
		else array[i]=i2c_readNak(); // read without ACK on last byte
		i++;
	}
    i2c_stop();
}


int main( void )
{
    uart_init( UART_BAUD_SELECT(UART_BAUD_RATE,F_CPU) );
    sei();

	uart_puts_P("L3G4200D test\n");
	i2c_init();   


	uint8_t l3g4200address=L3G4200D_ADDRESS1; // gyro can be assigned to 2 addresses, we check the two here.

	uart_puts_P("Trying address ");
	outputHex(l3g4200address);
	uart_puts_P("...");

	if (!checkI2CDeviceIsResponding(l3g4200address))
	{
		uart_puts_P("Not found...\n");

		l3g4200address=L3G4200D_ADDRESS2;
		uart_puts_P("Trying address ");
		outputHex(l3g4200address);
		uart_puts_P("...");
		if (!checkI2CDeviceIsResponding(l3g4200address))
		{
			uart_puts_P("Not found...\n");
			uart_puts_P("Please check I2C bus connection, pull ups on SDA/SCL, power to the gyro, decoupling capacitors etc...\n");
			for(;;);
		}
	}
	uart_puts_P("A device answered !\n");

	uart_puts_P("Is this a L3G4200D ? ");


    if(readI2Cbyte(l3g4200address,0x0F)!=L3G4200D_WHOAMI)
	{
		uart_puts_P("No - wrong device ? that's is straaaange.\n");
		for(;;);
	}
	else
		uart_puts_P("Yes !\n");

	uart_puts_P("Configuring gyro for basic measurements, no filtering, 500sps...\n");

    writeI2Cbyte(l3g4200address, 0x20, 0xBF);  // see datasheet for various possible settings.
    writeI2Cbyte(l3g4200address, 0x24, 0x00); 
    writeI2Cbyte(l3g4200address, 0x23, 0x90);

	uart_puts_P("Reading x,y,z data\n");
	for(;;)
	{
		// read 3 int16 which is 6 bytes.
		readI2CbyteArray(l3g4200address,0xA8,(uint8_t *)gyroscope,2*3);
		
		uart_puts_P("Angles x,y,z in °/s : ");
		float angle;
		int i=0;
		while(i<3)
		{
			angle=gyroscope[i]*0.0175f;
			dtostrf(angle,5,4, buffer);
			uart_puts(buffer);
			if (i!=2) uart_puts_P(", ");
			i++;
		}
		
		uart_puts_P("    raw : ");	

		i=0;
		while(i<3)
		{
			itoa( gyroscope[i], buffer, 10);  // output 16 bit signed value to decimal.      
			uart_puts(buffer);
			if (i!=2) uart_puts_P(", ");
			i++;
		}
		uart_puts_P("\n");

		sleep(100); // wait 100ms so user can read something
	}




}


