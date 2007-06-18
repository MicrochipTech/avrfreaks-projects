// mcp4822

// Wrapper routine for talking to an MCP4822 chip over the SPI
// bus.  NOTE:  It is up to the user to handle chip select!  !CS must
// be brought low in order to enable the MCP4822 prior to calling
// transmit_mcp4822(), and should be brought high afterward to latch
// the DAC values.
//
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

void transmit_mcp4822(uint8_t channel, uint8_t gain, uint8_t output_enable, uint16_t data);
