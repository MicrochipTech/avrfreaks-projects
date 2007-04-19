/** @file bcd.h Header file with the declarations of the BCD conversion
			routines.
*/

#ifndef __BCD_HEADER__
#define __BCD_HEADER__

#include <inttypes.h>

/** @def BCD_LOW_NIBBLE_CHAR Converts the low nibble of a BCD encoded
		character to the corresponding ASCII character. */
#define BCD_LOW_NIBBLE_CHAR(x) 	('0' + ((x) & 0x0F))

/** @def BCD_LOW_NIBBLE_CHAR Converts the high nibble of a BCD encoded
		character to the corresponding ASCII character. */
#define BCD_HIGH_NIBBLE_CHAR(x) ('0' + ((x) >> 4))

extern uint8_t CharToBCD2(uint8_t input);
extern uint16_t CharToBCD3(uint8_t input);

#endif // __BCD_HEADER__

