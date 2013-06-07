/*
 * CX80_Led.c
 *
 * Created: 20/02/2013 11:31:02
 *  Author: Calogiuri Enzo Antonio
 */ 

#include "CX80.h"

/*Setup the pins used to control the status led*/
void Setup_CX80_Led_Port(void)
{
	LED_PORT_DDR = 0xFF;
	LED_PORT = 0;
}

/*Power on a led*/
void Enable_CX80_Led(unsigned char nLed)
{
	LED_PORT |= (1 << nLed);
}

/*Power off a led*/
void Disable_CX80_Led(unsigned char nLed)
{
	LED_PORT &= ~(1 << nLed);
}