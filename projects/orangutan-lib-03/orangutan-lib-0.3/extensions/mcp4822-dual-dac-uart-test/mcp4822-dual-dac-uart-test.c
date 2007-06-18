// mcp4822-dual-dac-uart-test

// This example talks to a Microchip MCP4822.  It's a dual 12-bit
// DAC with an SPI interface.  Rather than use the on-board SPI
// interface on the ATMega, this example uses the on-board USART
// in SPI mode.
//
// The MCP4822 has a pin for latching the DAC output, but in this case 
// the !LDAC pin is tied to ground, so the outputs latch on the rising 
// edge of !CS.
//
// In this implementation of the SPI code, the chip select
// functionality is not handled by the SPI code.  It is up to the
// programmer to handle this, as you can see in this example.
// The chip select is a normally high I/O pin that is brought low 
// to select the SPI device of choice.  In this case wel'll be 
// talking to the MCP4822.
//
// For wiring, the MCP4822-to-Baby-O looks like this:
//
// MCP4822		Connect To
// 1 VDD		+5V off of a 7805
// 2 !CS		Baby-O PB0 (Any I/O pin would work, matter of choice)
// 3 SCK		Baby-O PD4 (XCK)
// 4 SDI		Baby-O PD1 (TXD (instead of MOSI))
// 5 VoutA		Output pin A
// 6 AVSS		Gnd
// 7 VoutB		Output pin B
// 8 !LDAC		Gnd (outputs latch on !CS when this is grounded)
//
// In case this is your first foray into SPI interfacing, as it was
// mine, here's a quick run-down on some of the naming conventions:
// MOSI stands for Master Out Slave In.  MISO stands for Master In
// Slave Out.  The datasheet on the MCP4822 does not use this naming
// convention, and uses SDI (Slave Data In) instead.  Since the
// Baby-O is our SPI master and the MCP4822 is a slave device, MOSI
// on the Baby-O should connect to SDI on the MCP4822.  There's no
// provision on the MCP4822 to send data to the SPI master, so the
// MISO pin on the Baby-O (PB4) is not connected.

// Get our Orangutan-specific information.
#include "device.h"

// AVR I/O definitions
#include <avr/io.h>

// So we can use delays
#include <util/delay.h>

// Include the SPI code
#include "uart-spi.h"

// Include the chip-specific code
#include "mcp4822-uart.h"

// Set our !CS line up as PB0

#define CS			PB0
#define CS_DDR		DDRB
#define CS_PORT		PORTB

int main(void)
{
	// Variables to hold our output values
	uint16_t	channel_a, channel_b;

	// Set up our !CS I/O line and bring it high so the MCP4822 isn't
	// selected when we initialize the SPI stuff.
	CS_DDR |= (1 << CS);
	CS_PORT |= (1 << CS);

	// Initialize the UART-SPI subsystem:
	uart_spi_init();
	
	// We can't toggle the LED in this example because the LED is tied to the
	// UART's TX line, which is in use.

	// Endless loop
	for(;;)
	{
		// Loop Channel A's value from 0 to 4095
		// This represents voltages in the range of:
		//
		// Gain		Voltage
		//	1x		0 - 2.048V
		//	2x		0 - 4.096V
		//
		for(channel_a = 0; channel_a < 4090; channel_a += 5)
		{
			// Make Channel B the opposite of Channel A
			channel_b = 4095 - channel_a;
			
			// We need to toggle CS once for each 16-bit transmission:

			// Bring !CS low to select the MCP4822
			CS_PORT &= ~(1 << CS);
			// Transmit our first channel worth of data
			transmit_mcp4822(0, 0, 1, channel_a);
			// Bring !CS high to latch the outputs
			CS_PORT |= (1 << CS);

			// Bring !CS low to select the MCP4822
			CS_PORT &= ~(1 << CS);
			// Transmit our second channel worth of data
			transmit_mcp4822(1, 0, 1, channel_b); 
			// Bring !CS high to latch the outputs
			CS_PORT |= (1 << CS);

			// If you're using an oscilloscope to see your output, this
			// isn't a bad delay time for a nice ramp function.  If you're
			// stuck using a meter, try increasing this value to slow things
			// down a little:
			//
			// There's a 4us settle time, so we're not pushing things AT ALL.
			// This makes for about 2 seconds of run time up the slope from
			// 0V to 4.090V.  Plenty slow enough to see on a multimeter.
			_delay_ms(1);
		}
	}

	return 0;

}
