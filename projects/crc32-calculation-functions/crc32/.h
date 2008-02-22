#ifndef CRC32_H_
#define CRC32_H_

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
 * 2008-02-28 Optimization using extern inline instead of extern
 * 				also added some const resulting in more optimized
 * 				code. 
 * 
 * 2008-02-22 Initail releas by Moritz Strübe <morty@gmx.net>
 * 
 * 
 */

#include <inttypes.h>


/**
 * There is a bit of optimization in gcc if you use little endian. 
 * IMHO the compiler should see this, but it doesn't.
 * You can turn the optimization on by setting LITTLE_ENDIAN to 1.
 */
#define LITTLE_ENDIAN 1

// I'm not sure about non-gnu compilers.
#ifdef __GNUC__
#ifndef __ATTR_CONST__
#define __ATTR_CONST__ __attribute__ ((const))
#endif
#endif


/** \file
 * This is a CRC32 implementation, that actually works. There is a slow and
 * a fast version. The slow one hasn't got a lookup table, but therefore 
 * needs little memory.  You do not need the .c if you use this version.
 * 
 * Ther also is a tabled version. You have to initialise the the table first.
 * Then call the crc32_calc_table instead of crc32_calc.
 * 
 * To speed things up inline is used. This avoids function calls, which are
 * more expensive then inlining the function.
 * Therefore ther are two .c - files. One with the table generation wich is
 * never inlined and one with the inlining functions. The later is needed 
 * when using -O0. In this case ther is no inlining done. Therefore ther must
 * be a "real" function. 
 * 
 * Example:
 * \code
 * main(){
 * 	int i;
 *	uint32_t crc;
 * 	char test[] = "Teststring";
 *	crc = crc32_reset();
 * 	for(i= 0; i < strlen(test); i++){
 * 		crc32_calc(test[i], &crc32);
 *	}
 * 	printf("The crc is: 0x%08x", crc32_return(crc32));
 * }
 * \endcode
 * 
 * \see http://www.ross.net/crc/download/crc_v3.txt
 * \see ecos RealTek 8139 ethernet driver
 * \author Moritz Strübe (morty@gmx.net)
 */




// static inlines are as quick as macros but more easy to debug!
// There have to be non-static defines, because doxygen ignores
// static defines in the default settings.

/**
 * Before starting a CRC32 calculation you have to reset the CRC.
 * \param crc32 The crc32 variable.
 */
extern inline uint32_t crc32_reset(void) __ATTR_CONST__;
extern inline uint32_t crc32_reset(void){
	return 0xffffffffU;       /* preload shift register, per CRC-32 spec */
}

/**
 * The final CRC32 is the inverted value of the CRC variable. 
 * \param crc32 The crc32 variable.
 * \return The actual crc32 result. 
 */
extern inline uint32_t crc32_return(const uint32_t crc32) __ATTR_CONST__;
extern inline uint32_t crc32_return(const uint32_t crc32){
	return ~crc32;            /* transmit complement, per CRC-32 spec */
}


/**
 * Add byte to CRC32 calculation.
 * \param data A byte of data to add to the string
 * \param crc32 the CRC32 variable.
 */

extern inline void crc32_calc(const uint8_t data, uint32_t * const crc32){
	uint8_t bit;
	(*crc32) ^= data;
	for (bit = 0; bit < 8; ++bit) {
		if ((*crc32) & 1)
			(*crc32) = ((*crc32) >> 1) ^ 0xEDB88320U;
		else
			(*crc32) = ((*crc32) >> 1);
	}
}

/**
 * Initializes a lookup talbe for a quicker CRC32 calculation. 
 * \param crc32_table The table must hold 256 32bit values and 
 * tharefore has to be 1024 bytes long.
 * \sa crc32_calc_table
 */ 
void crc32_init_table(uint32_t crc32_table[256]);

/**
 * Add byte to CRC32 calculation. Same as crc32_calc but uses a lookup table.
 * \param data A byte of data to add to the string
 * \param crc32 The CRC32 variable.
 * \param crc32_table The CRC lookup table. This table has to be initialized using crc32_init_table.
 * \sa crc32_init_table
 */
extern inline void crc32_calc_table(const uint8_t data, uint32_t * crc32, uint32_t const crc32_table[256]){
#if LITTLE_ENDIAN == 1
	(*crc32) = ((*crc32) >> 8) ^ crc32_table[data ^ ((uint8_t)(*crc32))];
#else
	(*crc32) = ((*crc32) >> 8) ^ crc32_table[data ^ ((*crc32) & 0xFF)];
#endif
}



#endif /*CRC32_H_*/
