/*********************************************
* TCJ 2-17-15
*
* USB test
*
* USB and general testing of the at32uc3b1256
*
* 
* Chip type           : at32uc3b1256
* Clock frequency     : Internal clock oscillator at 48 MHz using external 12 MHz crystal
*
* note that when chip comes up in dfu mode (default) the external crystal is also default and it works
* automatically for USB (for an 8, 12, or 16 MHz crystal). 
* 
*
*
* In order to boot into dfu mode and thus allow programming, it is necessary to do a reset (bring the reset pin
* 47 down then up) while PA13 (pin 25) is down. This is done with pushbuttons. 
*
*
*
*********************************************/

#include <avr32/io.h>
#include <stdlib.h>
#include <inttypes.h>

#define F_PBA 48000000L
#define PRINT_USART 2

#include "usart_io.h"


int main(void)
{

	setup_usart(2,19200,8,'N',1,'B');

		
// set up main clock

	AVR32_PM.MCCTRL.osc0en = 1;						// enable oscillator 0 (12 MHz crystal)
	AVR32_PM.OSCCTRL0.mode = 7;						// select crystal > 8 MHz

	AVR32_PM.PLL[0].pllen = 1;						// enable PLL0
	AVR32_PM.PLL[0].pllosc = 0;						// OSC0 is source for PLL0
	AVR32_PM.PLL[0].pllmul = 3;						// multiply by 8 = 96 MHz
	AVR32_PM.PLL[0].plldiv = 0;						// no input division
	AVR32_PM.PLL[0].pllopt = 3;						// divide output by 2 = 48 MHz

	while(!(AVR32_PM.POSCSR.lock0 & 1));			// wait for PLL0 to lock

	AVR32_PM.MCCTRL.mcsel = 2;						// PLL0 is source for main clock

// Note that PBA clock is equal to main clock (48 MHz) as I have left pbadiv = 0



	sr_printf("hello, cruel world\n");

	sr_printf("\n\n\n");

	sr_printf("Ce n'est plus une ardeur dans mes veines cachee\n");

	sr_printf("C'est Venus tout entiere a sa proie attachee\n");

	sr_printf("\n\n\n");

	while(1)
	{

		if(sr_kbhit())  sr_putchar(sr_getchar());   // rs232: echo character	

	}	
}




