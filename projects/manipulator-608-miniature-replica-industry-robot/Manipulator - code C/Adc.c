/*
 * Adc.c
 *
 *  Created on: 3 gru 2013
 *      Author: M.R.O.
 */
#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/delay.h>
#include <avr\pgmspace.h>
#include "Globals_Defs.h"

/***************************
 * --->>  Functions  <<--- *
 ***************************/

uint8 ReadCalibrationByte( uint8 index )
{
uint8 result;

/* Load the NVM Command register to read the calibration row. */
NVM_CMD = NVM_CMD_READ_CALIB_ROW_gc;
result = pgm_read_byte(index);

/* Clean up NVM Command register. */
NVM_CMD = NVM_CMD_NO_OPERATION_gc;

return( result );
}

uint16 AdcRead(uint8 channel)
{
	ADCA.CH0.MUXCTRL = (channel<<3);
	ADCA.CH0.CTRL |= ADC_CH_START_bm;
	while(!ADCA.CH0.INTFLAGS);
	ADCA.CH0.RES = 0;
	return ADCA.CH0RES;
}
