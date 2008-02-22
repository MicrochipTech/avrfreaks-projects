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
 * 2008-02-22 Initail releas by Moritz Strübe <morty@gmx.net>
 * 
 * See headerfile for changelog!
 * 
 */




#include "crc32.h"



/**
 * crc32_table This must be an array of 256 uint32_t!
 */

void crc32_init_table(uint32_t crc32_table[256]){
	uint32_t crc;
	uint16_t i;
	uint8_t  j;
	
	for (i = 0; i < 256; i++) {
		crc = (unsigned long)i;
		for (j = 0; j < 8; j++) {
			if (crc & 1) {
				crc = 0xedb88320L ^ (crc >> 1);
			} else {
				crc = crc >> 1;
			}
		}
		crc32_table[i] = crc;
	}
}
