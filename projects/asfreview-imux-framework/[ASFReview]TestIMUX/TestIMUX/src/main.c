/**
 * \file
 *
 * \brief Empty user application template
 *
 */

/*
 * Include header files for all drivers that have been imported from
 * AVR Software Framework (ASF).
 */
#include <asf.h>

int main (void)
{
	board_init();
	// Insert application code here, after the board has been initialized.
	
	for(;;)
	{
		//start ADC conversions
		//wait until ADC finishes the conversions
		//Send the data gathered by the serial port
	}		
}
