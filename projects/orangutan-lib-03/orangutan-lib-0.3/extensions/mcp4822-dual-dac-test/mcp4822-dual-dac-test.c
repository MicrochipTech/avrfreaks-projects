// mcp4822-dual-dac-test

// This example talks to a Microchip MCP4822.  It's a dual 12-bit
// DAC with an SPI interface.  The MCP4822 has a pin for latching
// the DAC output, but in this case the !LDAC pin is tied to ground,
// so the outputs latch on the rising edge of !CS.
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
// 3 SCK		Baby-O PB5 (SCK)
// 4 SDI		Baby-O PB3 (MOSI)
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
#include "spi.h"

// Include the chip-specific code
#include "mcp4822.h"

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

	// Set up our LED as an output so we can have a heartbeat
	LED_DDR |= (1 << LED0);

	// Initialize the SPI subsystem:
	spi_init();
	
	// Endless loop
	for(;;)
	{
		// Loop Channel A's value from 0 to 4095
		for(channel_a = 0; channel_a < 4095; channel_a += 1)
		{
			// Make Channel B the opposite of Channel A
			channel_b = 4096 - channel_a;
			
			// We need to toggle CS once for each 16-bit transmission:

			// Bring !CS low to select the MCP4822
			CS_PORT &= ~(1 << CS);
			// Transmit our first channel worth of data to channel A (0), 2x gain (0),
			// with the channel's output enabled (1), and our 12-bit data.
			transmit_mcp4822(0, 0, 1, channel_a);
			// Bring !CS high to latch the outputs
			CS_PORT |= (1 << CS);

			// Bring !CS low to select the MCP4822
			CS_PORT &= ~(1 << CS);
			// Transmit our second channel worth of data to channel B (1), 2x gain (0)
			// with the channel's output enabled (1), and our 12-bit data.
			transmit_mcp4822(1, 0, 1, channel_b); 
			// Bring !CS high to latch the outputs
			CS_PORT |= (1 << CS);

			// Toggle the LED
			LED_PORT ^= (1 << LED0);

			// If you're using an oscilloscope to see your output, this
			// isn't a bad delay time for a nice ramp function.  If you're
			// stuck using a meter, try increasing this value to slow things
			// down a little.  With a 1ms delay, it takes about 4 seconds
			// to cycle:
			_delay_ms(1);
		}
	}

	return 0;

}
