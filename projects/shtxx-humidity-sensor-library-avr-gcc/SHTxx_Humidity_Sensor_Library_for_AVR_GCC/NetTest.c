//----------------------------------------------------------------------------------
//
// Test SHTxx Library
//
// History:
// 2003-07-06	BL	- Created
//
//----------------------------------------------------------------------------------

#include <avr/io.h>
#include <avr/signal.h>
#include <avr/interrupt.h>
#include <avr/ina90.h>
#include <string.h>
#include <stdlib.h>
#include <avr/eeprom.h>

#include "uart.h"
#include "sht.h"
#include "delay.h"

#define XTAL 				8000000


// Output pins for status
#define LED_DDR				DDRD
#define LED_PORT			PORTD
#define LED_POWER_PIN    	5
#define LED_RX_PIN    		6
#define LED_TX_PIN    		7




// ------------------------------------------------------------------
//	PortB, PortC, PortD initialization
// ------------------------------------------------------------------
void PortInit(void)
{
	PORTB = 0xFF;							// enable pull-up resistors
	DDRB = 0x00;							// set pin direction (input)
	PORTC = 0x7F;							// enable pull-up resistors (pin 7 does not exist on this device)
	DDRC = 0x00;							// set pin direction (input)
	PORTC = 0xFF;							// enable pull-up resistors
	DDRC = 0x00;							// set pin direction (input)

	sbi(LED_DDR, LED_POWER_PIN);			// Set pin to output.
	sbi(LED_DDR, LED_RX_PIN);				// Set pin to output.
	sbi(LED_DDR, LED_TX_PIN);				// Set pin to output.
	cbi(LED_PORT, LED_POWER_PIN);			// Turn off pin
	cbi(LED_PORT, LED_RX_PIN);				// Turn off pin
	cbi(LED_PORT, LED_TX_PIN);				// Turn off pin

}

// ------------------------------------------------------------------
//	Initialize peripherals
// ------------------------------------------------------------------
void InitializeDevices(void)
{
	cli(); 									// disable all interrupts while we init

	PortInit();
	UartInit();
	ShtInit();

	MCUCR = 0x00;							// clear MCU Control Register
	GICR = 0x00;							// clear General Interrupt Control Register

	sei(); 									// reenable interrupts
}


// ------------------------------------------------------------------
//	Main
// ------------------------------------------------------------------
int main(void)
{
	unsigned char	c = 0;
	unsigned int	temp_ticks;
	unsigned int	humidity_ticks;
	int				temp;
	int 			humidity;

	// Initialize processor
	InitializeDevices();

	// Show we have power
	sbi(LED_PORT, LED_POWER_PIN);

	TxLine("");
	TxLine("SHTxx Test");
	TxLine("Note: readings have 2 implied decimal places. Divide by 100 to get the actual values");
	TxLine("");

	while (1)
	{
		// Read the temperature every 2 seconds. Delay at least 0.8 seconds
		// between reads to avoid self-heating of the SHTxx
		DELAY(2000000);

		// Reset communications. This is not neccessary, but included for good measure.
		ShtReset();

		// Read the status byte and display
		ShtReadStatus(&c);
		TxString("Status: ");
		TxUByte(c, 2);

		// Measure temperature and humidity
		temp_ticks = ShtMeasure(SHT_TEMPERATURE);
		humidity_ticks = ShtMeasure(SHT_HUMIDITY);

		// Convert measurements to real-world units
		temp = temp_ticks;
		humidity = humidity_ticks;
		ShtCalculate(&temp, &humidity);

		// Display results
		TxString(", Temp: ");
		TxUInt(temp, 10);

		TxString(", Humidity: ");
		TxUInt(humidity, 10);
		TxLine("");

		// Process any received keystrokes
		if (IsCharAvailable())
		{
			c = RxChar();
			switch (c)
			{
				// Turn on the SHTxx internal heater
				case 'o':
					ShtReset();
					ShtReadStatus(&c);
					c |= BV(2);
					c = ShtWriteStatus(c);
					TxUByte(c,10); TxLine(" Heater On");
					break;

				// Turn off the SHTxx internal heater
				case 'f':
					ShtReset();
					ShtReadStatus(&c);
					c &= ~BV(2);
					c = ShtWriteStatus(c);
					TxUByte(c,10); TxLine(" Heater OFF");
					break;

				// Default. Turn off all status bits. This is the factory default.
				default:
					ShtReset();
				   	ShtWriteStatus(0);
					c = ShtWriteStatus(0);
					TxUByte(c,10); TxLine(" All OFF");
					break;
			}
		}
	}

	return( 0 );
}



