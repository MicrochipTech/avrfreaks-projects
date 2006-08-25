#include <inttypes.h>

#include "hardware.h"
#include "dmx973b.h"

#define dmx973bSetDataMode() DMX973B_PORT |= _BV(DMX973B_A0)
#define dmx973bSetCommandMode() DMX973B_PORT &= ~_BV(DMX973B_A0)
#define dmx973bSetCLK() DMX973B_PORT |= _BV(DMX973B_CLK)
#define dmx973bClearCLK() DMX973B_PORT &= ~_BV(DMX973B_CLK)
#define dmx973bSetD() DMX973B_PORT |= _BV(DMX973B_D)
#define dmx973bClearD() DMX973B_PORT &= ~_BV(DMX973B_D)
#define dmx973bSetRST() DMX973B_PORT |= _BV(DMX973B_RST)
#define dmx973bClearRST() DMX973B_PORT &= ~_BV(DMX973B_RST)
#define dmx973bDelay()	asm volatile ("nop\n nop\n nop\n nop\n nop\n nop\n nop\n nop\n")

void dmx973bWrite(uint8_t data)
{
	uint8_t shifter = 0x80;
	
	while (shifter > 0)
	{
		if (shifter & data) dmx973bSetD();
		else dmx973bClearD();
		dmx973bDelay();
		dmx973bSetCLK();
		dmx973bDelay();
		dmx973bClearCLK();
		dmx973bDelay();
		shifter = shifter >> 1;
	}
}

void dmx973bSetLocation(uint8_t column, uint8_t line)
{
	dmx973bSetCommandMode();
	dmx973bWrite(DMX973B_SET_LINE + line);
	dmx973bWrite(DMX973B_SET_COL_H + (column >> 4));
	dmx973bWrite(DMX973B_SET_COL_L + (column & 0x0F));
}

void dmx973bInit(void)
{
	uint8_t i;

	DMX973B_DDR |= DMX973B_PORT_MASK;
	dmx973bClearRST();
	for (i=0; i<0xFF; i++) dmx973bDelay();
	dmx973bSetRST();
	dmx973bSetCommandMode();
	dmx973bWrite(DMX973B_SET_BIAS);
	dmx973bWrite(DMX973B_SET_ADC);
	dmx973bWrite(DMX973B_SET_CONTRAST);
	dmx973bWrite(DMX973B_SET_POWER_CTRL);
	dmx973bWrite(DMX973B_DISPLAY_ON);
}

void dmx973bWriteData(uint8_t data)
{
	dmx973bSetDataMode();
	dmx973bWrite(data);
}
