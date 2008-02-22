/*
 * This code is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 * 
 * 2008-02-28 Initail releas by Moritz Strübe <morty@gmx.net>
 * 
 * See headerfile for changelog! 
 * 
 */

#include "crc32.h"

uint32_t crc32_reset(void){
	return 0xffffffffU;       /* preload shift register, per CRC-32 spec */
}


uint32_t crc32_return(const uint32_t crc32){
	return ~crc32;            /* transmit complement, per CRC-32 spec */
}


void crc32_calc(const uint8_t data, uint32_t * const crc32){
	uint8_t bit;
	(*crc32) ^= data;
	for (bit = 0; bit < 8; ++bit) {
		if ((*crc32) & 1)
			(*crc32) = ((*crc32) >> 1) ^ 0xEDB88320U;
		else
			(*crc32) = ((*crc32) >> 1);
	}
}


void crc32_calc_table(const uint8_t data, uint32_t * const crc32, const uint32_t crc32_table[256]){
#if LITTLE_ENDIAN == 1
	(*crc32) = ((*crc32) >> 8) ^ crc32_table[data ^ ((uint8_t)(*crc32))];
#else
	(*crc32) = ((*crc32) >> 8) ^ crc32_table[data ^ ((*crc32) & 0xFF)];
#endif
}
