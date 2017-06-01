/**
 * \file
 *
 * \brief User board initialization template
 *
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

#define _CONF_BOARD_

#include "project.h"
#include <asf.h>
#include <board.h>
#include <conf_board.h>



void board_init(void)
{
	// ********************************************* Processor definition
	//AVR32_PART_IS_DEFINED(UC3C2512C);

	// system clock initialization code.
	sysclk_init();

	// Configure Osc0 in crystal mode (i.e. use of an external crystal source, with
	// frequency FOSC0) with an appropriate startup time then switch the main clock
	// source to Osc0.
	pcl_switch_to_osc(PCL_OSC0, BOARD_OSC0_HZ, 0x00000003);
	
 	//  gpio outputs: LEDs
	gpio_configure_pin(GRN_LED, GPIO_DIR_OUTPUT);
	gpio_configure_pin(RED_LED, GPIO_DIR_OUTPUT);
	 
	LED_OFF(RED_LED);
	LED_OFF(GRN_LED);

	// init position sensor quadrature detectors I/O and interrupt hardware
	PositionSensorInit();

	//  set up PWMs in CTCs
 	TC_PWM_Init();
	
	// set up ADC for Vsense
	ADCIFA_init();
	//adcifa_start_itimer( , Fs= Fadc/(count+1))

	// Set up serial comms
	Comms_Init();

}
