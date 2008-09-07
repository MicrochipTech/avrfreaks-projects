/*
* Program: Multitherm - A multiple DS18x20 thermometer control and logging device
* File Name: crc.c
* Purpose: Computes the DOW CRC8 or CRC16 value of a byte array
*
* Description:
*  A CRC-8/16 (DOW) implementation optimized for small code size.
*  If the actual CRC value is appended to the data, the return
*  value of the function will be 0.
*
*  Based on Maxim/Dallas Semiconductor App Note 27 "Understanding
*  and Using Cyclic Redundancy Checks with Dallas Semiconductor
*  iButton Products"
*
*
*    Programmer: Tom Blough
*   Host System: ATMega16L tested with both internal RC osc at 8MHz
*                and external 8MHz crystal
*  Date Created: 2007/09/20
*      Revision: $WCREV$  $WCDATE$
* Modifications:
*/

// $Header: C:\\RCS\\C\\Documents\040and\040Settings\\tblough\\My\040Documents\\Electronics\\AVR\\Projects\\multitherm\\crc.c,v 1.1 2007-10-28 19:34:06+08 tblough Exp tblough $

#import <inttypes.h>
#import "crc.h"

/* Compute the DOW CRC8 value of a byte array.
*         *data : pointer to array of bytes to compute CRC
*     num_bytes : number of bytes in the array
*       returns : the CRC of the data
*/
extern uint8_t crc8( uint8_t *data, uint8_t num_bytes)
{
	uint16_t byte_ctr;
	uint8_t bit_ctr, temp, crc, cur_byte;

	crc = 0;
	for( byte_ctr = 0; byte_ctr < num_bytes; byte_ctr++)
	{
		cur_byte = data[byte_ctr];
		for( bit_ctr = 8; bit_ctr > 0; bit_ctr--)
		{
			temp = ((crc ^ cur_byte) & 0x01);
			if( temp == 0)
			{
				crc >>= 1;
			}
			else
			{
				crc ^= 0x18;
				crc >>= 1;
				crc |= 0x80;
			}
			cur_byte >>= 1;
		}
	}
	return crc;
}

/* Compute the DOW CRC16 value of a byte array.
*         *data : pointer to array of bytes to compute CRC
*     num_bytes : number of bytes in the array
*       returns : the CRC of the data
*/
uint16_t crc16( uint8_t *data, uint16_t num_bytes)
{
	uint16_t byte_ctr;
	uint8_t bit_ctr, temp, crc, cur_byte;

	crc = 0;
	for( byte_ctr = 0; byte_ctr < num_bytes; byte_ctr++)
	{
		cur_byte = data[byte_ctr];
		for( bit_ctr = 8; bit_ctr > 0; bit_ctr--)
		{
			temp = ((crc ^ cur_byte) & 0x01);
			if( temp == 0)
			{
				crc >>= 1;
			}
			else
			{
				crc ^= 0x4002;
				crc >>= 1;
				crc |= 0x8000;
			}
			cur_byte >>= 1;
		}
	}
	return crc;
}
