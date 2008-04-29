/*****************************************************************************
* Author: 	Jidan Al-Eryani,	jidan@gmx.net 
* Date: 	18.4.2007
*
*
* Purpose: 
* Simple program to test the interaction between the microcontroller and FPGA.
* This Program makes the robot platform move forward (hence the name of program = Go Ahead)
* and turn randomly right or left around an obstacle if one is detected.
* The sensors are two infrared sensors and the motors encoders.
*
* see "9.2 Go Ahead" in thesis
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
#include "goahead.h"

#define SW2 PF0
#define SW3 PF1


int main(void)
{

	//set switch3 pins
	PORTF |= (1<<SW3);
	DDRF &= ~((1<<SW3));

	if (!(PINF & (1<<SW3)))
		xmodem_download(0);
	
	
	//xmodem_upload(0, 212392);


	configure_fpga(0, 212392);
	goahead();

	while(1);
	return 0;
}
