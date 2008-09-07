/*
* Program: Multitherm - A multiple DS18x20 thermometer control and logging device
* File Name: ds2415.c
* Purpose: Control and Data functions for Dallas Semiconductor DS2415 Real Time Clock
*
* Description:
*  Read Epoch data from DS2415 Real Time Clock.  Clock stores and increments a 32 bit
*  counter once per second.  This module stores and retrieves this count as second since
*  the Unix Epoch of January 1, 1970, 12:00:00am
*
*  Based on Maxim Application Note 517
*
*    Programmer: Tom Blough
*   Host System: ATMega16L tested with both internal RC osc at 8MHz
*                and external 8MHz crystal
*  Date Created: 2007/09/20
*      Revision: $WCREV$  $WCDATE$
* Modifications:
*/

// $Header: C:\\RCS\\C\\Documents\040and\040Settings\\tblough\\My\040Documents\\Electronics\\AVR\\Projects\\multitherm\\ds2415.c,v 1.1 2007-10-28 19:34:38+08 tblough Exp tblough $

#include <inttypes.h>
#include "ds2415.h"
#include "owi_defs.h"
#include "owi_highlevel.h"

// number of days at the beginning of each month used by _epoch_to_gregorian()
uint16_t accum_days[13] = { 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365 };

/* Write the control byte and 4 data bytes comprising the 32 bit seconds counter
 * to the DS2415
 */
extern uint8_t DS2415_write_clock( uint8_t ctrl, uint32_t rtc, uint8_t id[])
{
	// make sure we have a device to check and the bus is working
	if( id == NULL || owi_device_command( DS2415_WRITE_CLOCK, id)) return DS2415_CLOCK_ERROR;

	// set the clock data
	owi_byte_io( ctrl);
	owi_byte_io( (uint8_t)rtc); // LSB
	owi_byte_io( (uint8_t)(rtc >> 8));
	owi_byte_io( (uint8_t)(rtc >> 16));
	owi_byte_io( (uint8_t)(rtc >> 24));	// MSB
	owi_reset();	// activate the changes
	return DS2415_CLOCK_OK;
}

/* Read the control byte and 4 data bytes comprising the 32 bit seconds counter
 * from the DS2415
 */
extern uint8_t DS2415_read_clock( uint8_t *ctrl, uint32_t *rtc, uint8_t id[])
{
	// make sure we have a device to check and the bus is working
	if( id == NULL || owi_device_command( DS2415_READ_CLOCK, id)) return DS2415_CLOCK_ERROR;

	// get the clock data
	*ctrl = owi_byte_io( 0xFF);
	*rtc = ((uint32_t)owi_byte_io( 0xFF));	// LSB
	*rtc |= ((uint32_t)owi_byte_io( 0xFF)) << 8;
	*rtc |= ((uint32_t)owi_byte_io( 0xFF)) << 16;
	*rtc |= ((uint32_t)owi_byte_io( 0xFF)) << 24;	// MSB
	owi_reset();	// stop further clock transmissions
	return DS2415_CLOCK_OK;
}

// Convert a Gregorian calendar date and time into a Unix Epoch seconds count
extern uint32_t DS2415_gregorian_to_epoch( DS2415_date_t *dt)
{
	uint16_t days;
	uint32_t rtc;

	// convert gregorian date (dt) into total days
	days = 365 * (dt->year - 70) + accum_days[dt->month] + (dt->day - 1);
	days += (dt->year - 69) / 4;	// fixup for past leap years
	if( (dt->month > 1) && ((dt->year % 4) == 0)) days++;	// fixup for current leapyear

	// convert to seconds and add the time offset
	rtc = 3600 * ((uint32_t)dt->hour + (24 * (uint32_t)days));
	rtc += 60 * (uint32_t)dt->min;
	rtc += (uint32_t)dt->sec;

	return rtc;
}

// Convert a Unix Epoch seconds count into a Gregorian calendar date and time 
extern void DS2415_epoch_to_gregorian( uint32_t seconds, DS2415_date_t *dt)
{
	uint16_t tmp_uint_1, tmp_uint_2, tmp_uint_3;
	uint32_t tmp_ulong_1, tmp_ulong_2;

	// convert elapsed epoch seconds (rtc) count into gregorian date (dt)
	tmp_ulong_1 = seconds / 60;	// whole minutes
	dt->sec = seconds - (60 * tmp_ulong_1);

	tmp_ulong_2 = tmp_ulong_1 / 60;	// whole hours
	dt->min = tmp_ulong_1 - (60 * tmp_ulong_2);

	tmp_ulong_1 = tmp_ulong_2 / 24;	// whole days
	dt->hour = tmp_ulong_2 - (24 * tmp_ulong_1);

	tmp_ulong_2 = tmp_ulong_1 + 365 + 366;	// whole days since 1968
	tmp_uint_1 = tmp_ulong_2 / ((4 * 365) + 1);	// leap year periods

	tmp_uint_2 = tmp_ulong_2 % ((4 * 365) + 1);	// days since current lyear

	// if days are after a current leap year then add a leap year period
	if ((tmp_uint_2 >= (31 + 29))) tmp_uint_1++;

	tmp_uint_3 = (tmp_ulong_2 - tmp_uint_1) / 365;	// whole years
	tmp_uint_1 = tmp_ulong_2 - (tmp_uint_3 * 365) - tmp_uint_1;	// days since first of year

	if ((tmp_uint_2 <= 365) && (tmp_uint_2 >= 60)) {
		tmp_uint_1++;
	}
	dt->year = tmp_uint_3 + 68;		

	// search for what month it is based on how many days have past
	// within the current year
	tmp_uint_2 = 13;	// month
	tmp_uint_3 = 366;	// days to month
	while (tmp_uint_1 < tmp_uint_3)
	{
		 tmp_uint_2--;
		 tmp_uint_3 = accum_days[tmp_uint_2];
		 // fixup if month is past February and it is a leap year
		 if ((tmp_uint_2 >= 2) && ((dt->year % 4) == 0)) tmp_uint_3++;
	}
	dt->month = tmp_uint_2;
	dt->day = tmp_uint_1 - tmp_uint_3 + 1;
}

// The following methods are not really needed but can make life easier.
// comment out to reduce footprint - see note in header file
#ifdef DS2415_FULL_PKG

/* Wrapper to retrieve just the oscillator bit in the control byte without
 * having to explicitly handle the current time value
 */
extern uint8_t DS2415_get_osc_status( uint8_t id[])
{
	uint8_t ctrl;
	uint32_t rtc;

	if( DS2415_read_clock( &ctrl, &rtc, id) == DS2415_CLOCK_OK)
		return DS2415_OSC_STATUS( ctrl);

	return DS2415_CLOCK_ERROR;
}

/* Wrapper to modify just the oscillator bit in the control byte without
 * having to explicitly handle the current time value
 */
extern uint8_t DS2415_set_osc_status( uint8_t enable, uint8_t id[])
{
	uint8_t ctrl;
	uint32_t rtc;

	if( DS2415_read_clock( &ctrl, &rtc, id) == DS2415_CLOCK_OK)
	{
		if( enable)
			return DS2415_write_clock( DS2415_OSC_ON( ctrl), rtc, id);
		else		 
			return DS2415_write_clock( DS2415_OSC_OFF( ctrl), rtc, id);
	}
	return DS2415_CLOCK_ERROR;
}

/* Wrapper to set the seconds count based on a gregorian time without
 * having to explicitly handle the control byte
 */
extern uint8_t DS2415_set_time( DS2415_date_t *dt, uint8_t id[])
{
	uint8_t ctrl;
	uint32_t rtc;

	// need current control byte for write
	if( DS2415_read_clock( &ctrl, &rtc, id) == DS2415_CLOCK_OK)
		return DS2415_write_clock( ctrl, DS2415_gregorian_to_epoch( dt), id);
	return DS2415_CLOCK_ERROR;
}

/* Wrapper to return the gregorian time without
 * having to explicitly handle the control byte
 */
extern uint8_t DS2415_get_time( DS2415_date_t *dt, uint8_t id[])
{
	uint8_t ctrl;
	uint32_t rtc;

	if( DS2415_read_clock( &ctrl, &rtc, id) == DS2415_CLOCK_OK)
	{
		DS2415_epoch_to_gregorian( rtc, dt);		
		return DS2415_CLOCK_OK;
	}
	return DS2415_CLOCK_ERROR;
}
#endif


