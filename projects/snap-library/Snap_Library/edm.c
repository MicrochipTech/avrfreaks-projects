/*
Copyright (c) 2002 Colin O'Flynn

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

The routines in this file are tested to work with SNAP lab (checksum, CRC8 and
CRC16 tested), as well CRC8 and CRC16 produce the same result as the built-in
function in BASCOM-AVR. These may NOT be official CRC algorithms, so be warned
*/


#include <avr/io.h>
#include "edm.h"


//---------------------------------------------------------
#if CHECKSUM_E == TRUE

	unsigned char checksum_1_byte (unsigned char *data_in, unsigned short number_of_bytes_to_read)
	{
		unsigned char		checksum;
		short				array_index_counter;

		array_index_counter	= 0;
		checksum 			= 0;

		while (array_index_counter != number_of_bytes_to_read)
		{
			array_index_counter++;
			checksum += data_in[array_index_counter - 1];
		}

		return checksum;
	}
#endif


//---------------------------------------------------------
#if CRC8_E == TRUE

	unsigned char crc8 (unsigned char *data_in, unsigned short number_of_bytes_to_read)
	{
		unsigned char				crc;
		unsigned short				loop_count;
		unsigned char				bit_counter;
		unsigned char				data;
		unsigned char				feedback_bit;

		crc = CRC8INIT;

		for (loop_count = 0; loop_count != number_of_bytes_to_read; loop_count++)
		{
			data = data_in[loop_count];

			bit_counter = 8;
			do
			{
				feedback_bit = (crc ^ data) & 0x01;
				if (feedback_bit == 0x01)
				{
					crc = crc ^ CRC8POLY;
				}
				crc = (crc >> 1) & 0x7F;
				if (feedback_bit == 0x01)
				{
					crc = crc | 0x80;
				}

				data = data >> 1;

				bit_counter--;

			} while (bit_counter > 0);
		}

		return crc;
	}

#endif


//---------------------------------------------------------
#if CRC16_E == TRUE

	unsigned int crc16 (unsigned char data_in[], unsigned short number_of_bytes_to_read)
	{
		return( crc16_continue(data_in, number_of_bytes_to_read, CRC16INIT) );
	}

	unsigned int crc16_continue (unsigned char data_in[], unsigned short number_of_bytes_to_read, unsigned int crc)
	{
		unsigned short				loop_count;
		unsigned char				bit_counter;

		for (loop_count = 0; loop_count != number_of_bytes_to_read; loop_count++)
			{
			crc = (data_in[loop_count] << 8) ^ crc;
			for (bit_counter = 0; bit_counter != 8; bit_counter++)
			{
				//check if MSB is one
				if ((crc & 0x8000) == 0x8000)
				{
					crc = (crc << 1) ^ CRC16POLY;
				}
				else
				{
					crc = crc << 1;
				}
			}
		}

		return crc;

	}
#endif
