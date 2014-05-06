/*****************************************************************************
* MAMoRo Bootloader
*
* Author: 	Jidan Al-Eryani,	jidan@gmx.net 
* Date: 	18.4.2007
*
* Purpose:
* After reset, the uC checks the state of button1 to know if data should downloaded to the flash memory or not.
*After that it start configuring the FPGA. Finally, it start the user applicaiton program.
*
*MAMoRo Bootloader - see "8.3	Programming the Overall System" in the thesis
*
* Tested with: 
*	+MCU: ATmega128(L) 
*	+FPGA: Spartan3 XC3S400 -4  TQ144
*	+Dataflash: AT45DB041B (512K X 8)
*	+Compiler: avr-gcc (GCC) 4.1.1
*
*
*
****************************************************************************/

#include <inttypes.h>
#include <avr/io.h>
#include "usart.h"
#include "xmodem.h"
#include "fpgacon.h"
#include "dataflash.h"

#define SW2 PF0
#define SW3 PF1


int main(void)
{

	//set switch3 pins
	PORTF |= (1<<SW3);
	DDRF &= ~((1<<SW3));

	if (!(PINF & (1<<SW3)))
		xmodem_download(0);
	

	//configure the FPGA; flash memory start address=0, size of configuration file=212392
	configure_fpga(0, 212392);
	
	// start the program application
	//myprogram();

	while(1);
	return 0;
}
