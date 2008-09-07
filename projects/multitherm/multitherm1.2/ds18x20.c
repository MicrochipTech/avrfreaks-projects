/*
* Program: Multitherm - A multiple DS18x20 thermometer control and logging device
* File Name: ds18x20.c
* Purpose: Control and Data functions for Dallas Semiconductor DS18x20 Thermometer
*
* Description:
*  Read temperature data from DS18x20 series thermometers
*
*  Based on previous work by Martin Thomas, Peter Dannegger,
*    Carsten Foss and others.
*
*    Programmer: Tom Blough
*   Host System: ATMega16L tested with both internal RC osc at 8MHz
*                and external 8MHz crystal
*  Date Created: 2007/09/20
*      Revision: $WCREV$  $WCDATE$
* Modifications:
*/

// $Header: C:\\RCS\\C\\Documents\040and\040Settings\\tblough\\My\040Documents\\Electronics\\AVR\\Projects\\multitherm\\ds18x20.c,v 1.1 2007-10-28 19:34:35+08 tblough Exp tblough $

#include <avr/io.h>
#include <stdio.h>

#include "ds18x20.h"
#include "owi_defs.h"
#include "owi_highlevel.h"
#include "owi_lowlevel.h"
#include "crc.h"
#include "delay.h"


/* start measurement (CONVERT_T) for all sensors if input id==NULL 
   or for single sensor. then id is the rom-code */
extern uint8_t DS18X20_start_meas( uint8_t id[])
{
	if( ! owi_device_command( DS18X20_CONVERT_T, id))
	{
		// wait for measurement cycle to complete
		delay_ms( DS18B20_TCONV_12BIT);
		return OWI_BUS_OK;
	}
	return OWI_BUS_ERROR;
}

extern int DS18X20_get_meas( uint8_t id[])
{
	uint8_t i, sp[DS18X20_SP_SIZE];
	uint16_t meas;

	// make sure we have a device to check and the bus is working
	if( id == NULL || owi_device_command( DS18X20_READ, id)) return DS18X20_BAD_TEMP;

	// get the scratchpad data
	for( i = 0 ; i < DS18X20_SP_SIZE; i++) sp[i] = owi_byte_io( 0xFF);
	if( crc8( &sp[0], DS18X20_SP_SIZE)) return DS18X20_BAD_TEMP;

	// got good data
	meas = sp[0] | (((uint16_t)sp[1]) << 8);

	return meas;
}

// returns the temperature * 100 as an int
extern int DS18X20_get_temp( uint8_t id[])
{
	uint8_t sp[DS18X20_SP_SIZE];
	uint16_t meas;
	uint8_t i, subzero, cel;
	uint8_t need_rounding[] = { 1, 3, 4, 6, 9, 11, 12, 14 };

	// make sure we have a device to check and the bus is working
	if( id == NULL || owi_device_command( DS18X20_READ, id)) return DS18X20_BAD_TEMP;
	// get the scratchpad data
	for( i = 0 ; i < DS18X20_SP_SIZE; i++) sp[i] = owi_byte_io( 0xFF);
	if( crc8( &sp[0], DS18X20_SP_SIZE)) return DS18X20_BAD_TEMP;

	// got good data so start conversion
	meas = sp[0];  // LSB
	meas |= ((uint16_t)sp[1]) << 8; // MSB

	// convert to 18S20 9bit temp to 12bit temperature 
	if( OWI_FAMILY( id[0]) == DS18S20_FAMILY_CODE)
	{
		/* Extended measurements for DS18S20 contributed by Carsten Foss */
		meas &= (uint16_t) 0xfffe;	// Discard LSB , needed for later extended precicion calc
		meas <<= 3;					// Convert to 12-bit , now degrees are in 1/16 degrees units
		meas += (16 - sp[6]) - 4;	// Add the compensation , and remember to subtract 0.25 degree (4/16)
	}

	// check for negative 
	if( meas & 0x8000) 
	{
		subzero = 1;      // mark negative
		meas ^= 0xffff;  // convert to positive => (twos complement)++
		meas++;
	}
	else subzero = 0;
	
	// clear undefined bits for B != 12bit
	if( OWI_FAMILY( id[0]) == DS18B20_FAMILY_CODE)
	{
		i = sp[DS18B20_CONF_REG];
		if ( (i & DS18B20_12_BIT) == DS18B20_12_BIT ) ;	// both bits set
		else if( (i & DS18B20_11_BIT) == DS18B20_11_BIT) // bit 6
			meas &= ~(DS18B20_11_BIT_UNDF);
		else if( (i & DS18B20_10_BIT) == DS18B20_10_BIT) // bit 5
			meas &= ~(DS18B20_10_BIT_UNDF);
		else // neither bit 6 or 5
			meas &= ~(DS18B20_9_BIT_UNDF);
	}			
	
	cel = (uint8_t)(meas >> 4);	// save the integer degrees 
	meas = (uint8_t)(meas & 0x000F);	// get the fractional part

	// need rounding?
	if( !subzero)
	{
		for( i = 0; i < sizeof(need_rounding); i++)
		{
			if( meas == need_rounding[i])
			{
				meas++;
				break;
			}
		}
	}
	meas += cel * 10;

	return ((subzero) ? -meas : meas);
}

