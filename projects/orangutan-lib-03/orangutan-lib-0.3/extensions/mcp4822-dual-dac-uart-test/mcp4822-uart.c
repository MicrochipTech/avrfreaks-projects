// mcp4822-uart

// This code talks to a Microchip MCP4822.  It's a dual 12-bit
// DAC with an SPI interface.  
//
// The mcp4822 example uses the onboard SPI interface to talk
// to the MCP4822.  Not all Orangutans can get at their SPI pins,
// so this example uses the SPI mode of the onboard USART to
// talk to the MCP4822.
//
// The MCP4822 has a pin for latching the DAC output, but in 
// this case the !LDAC pin is tied to ground, so the outputs latch 
// on the rising edge of !CS.
//
// As with the SPI code from the main library, the chip select
// functionality is not handled by the USART-SPI code.  It is up 
// to the programmer to handle this.  Since you might want to use 
// more than one of these chips at once, this code passes the buck, 
// too, so it's still up to the user to handle chip select.  The 
// chip select is a normally high I/O pin that is brought low to 
// select the SPI device of choice.
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

#include "device.h"

#include <avr/io.h>

#include "uart-spi.h"

// Now for a wrapper routine for transmitting our 12-bit values
// to the MCP4822

// "channel" selects which channel is being written to:
//	0 = A
//	1 = B
//
// "gain" is a user-selectable gain of 1x or 2x:
//	0 = 2x Vref
//	1 = 1x Vref
//
// "output_enable" determines whether a given channel is outputting
// a voltage, or whether the output goes high-impedance:
//	0 = Output High-Z
//	1 = Output Enabled

void transmit_mcp4822(uint8_t channel, uint8_t gain, uint8_t output_enable, uint16_t data)
{
	// Make sure each of our flags is either a 0 or a 1:
	channel &= 0x01;
	gain &= 0x01;
	output_enable &= 0x01;

	// Make sure our data is only 12 bits
	data &= 0xFFF;

	// Now pack the data with our flags:
	// channel << 15
	// gain << 13
	// output_enable << 12
	data |= (channel << 15) | (gain << 13) | (output_enable << 12);

	// Send our two bytes of data as per the datasheet:

	// High byte first
	uart_spi_transfer(data >> 8);

	// Low byte second
	uart_spi_transfer(data & 0xFF);
}

