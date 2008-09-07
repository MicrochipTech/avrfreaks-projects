/*
* Program: Multitherm - A multiple DS18x20 thermometer control and logging device
* File Name: owihighlevel.c
* Purpose: High level funtions to access a Dallas Semiconductor 1-Wire bus
*
* Description:
*  High level access to Dallas 1-Wire devices.  These functions are not device
*  speed/timing dependent.
*
*  Based on Dallas Semiconductor Application Note AN126
*     "1-Wire Communication Through Software"
*  Based on Dallas Semiconductor Application Note AN187
*     "1-Wire Search Algorithm"
*  Based on code by Peter Dannegger and Martin Thomas
*
*    Programmer: Tom Blough
*   Host System: ATMega16L tested with both internal RC osc at 8MHz
*                and external 8MHz crystal
*  Date Created: 2007/09/20
*      Revision: $WCREV$  $WCDATE$
* Modifications:
*/

// $Header: C:\\RCS\\C\\Documents\040and\040Settings\\tblough\\My\040Documents\\Electronics\\AVR\\Projects\\multitherm\\owi_highlevel.c,v 1.1 2007-10-28 19:36:19+08 tblough Exp tblough $

#include <avr/io.h>

#include "owi_defs.h"
#include "owi_highlevel.h"
#include "crc.h"


/* read or write a full byte of data to the 1-Wire bus
 * note that owi_byte_io( 0xff) is the same as owi_byte_rd()
 * and owi_byte_io( data) is the same as owi_byte_wr( data)
 */
extern uint8_t owi_byte_io( uint8_t data)
{
	uint8_t i, result = 0;

	// write the data, and add it to the result
	for( i = 0; i < 8; i++)
		result |= (owi_bit_io( data & (1 << i)) << i);

	return result;
}

/* send a device specific command to one (or all) devices on the 1-wire bus
 * if id contains the rom code of one of the devices on the bus, that device
 * will be selected and the command sent to the single device.  if id is
 * null, the command will be sent to all devices on the 1-wire bus
 */
extern uint8_t owi_device_command( uint8_t command, uint8_t *id)
{
	uint8_t i;

	if( owi_reset())
	{
		if( id)
		{
			owi_byte_io( OWI_MATCH_ROM); // to a single device
			i = OWI_ROMCODE_SIZE;
			do
			{
				owi_byte_io( *id);
				id++;
			} while( --i);
		} else
		{
			owi_byte_io( OWI_SKIP_ROM); // to all devices
		}
		owi_byte_io( command);	// send the command

		return OWI_BUS_OK;
	}
	return OWI_BUS_ERROR;
}

/* Searches and returns 1 id found on the 1-Wire(R) bus.
 *
 *  diff   The bit position where the algorithm made a 
 *         choice the last time it was run. This should
 *         be >= 64 (0x40) when a search is initiated. Supplying 
 *         the return argument of this function when calling 
 *         repeatedly will go through the complete slave 
 *         search.
 *
 *    id   A pointer to an 8 byte char array where the 
 *         discovered identifier will be placed.
 *
 *   cmd   Either SearchROM (0xF0) or AlarmSearch (0xEC)
 */
extern uint8_t owi_search( uint8_t diff, uint8_t *id, uint8_t cmd)
{
	uint8_t i, j, next_diff;
	uint8_t b;

	if( ! owi_reset())
		return OWI_PRESENCE_ERROR; // error, no device found

	owi_byte_io( cmd); // ROM search command
	next_diff = OWI_LAST_DEVICE; // unchanged on last device

	i = OWI_ROMCODE_SIZE * 8; // 8 bytes

	do
	{
		j = 8; // 8 bits
		do
		{
			b = owi_bit_io( 1); // read bit
			if( owi_bit_io( 1) )
			{ // read complement bit
				if( b) // 11
					return OWI_DATA_ERR; // data error
			} else
			{
				if( !b)
				{ // 00 = 2 devices
					if( diff > i || (( *id & 1) && diff != i))
					{
						b = 1; // now 1
						next_diff = i; // next pass 0
					}
				}
			}
			owi_bit_io( b); // write bit
			*id >>= 1;
			if( b)
				*id |= 0x80; // store bit

			i--;

		} while( --j);

		id++; // next byte

	} while( i);

	return next_diff; // to continue search
}


