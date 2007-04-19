/** @file bcd.c Routines for BCD conversions.
*/

#include <inttypes.h>

/** Convert a character into a BCD encoded character. The input must be in
	the range 0 to 99. The result is byte where the high and low nibbles
	contain the tens and ones of the input.
	@param input A byte in the range 0 to 99. If input is out of this range
				 the result is a non-BCD character.
	@result A BCD encoded character, where the high nibble contains the
			tens and the low nibble the ones of the input.
*/
uint8_t CharToBCD2(uint8_t input)
{
	uint8_t high = 0;

	while (input >= 10)
	{
		high++;
		input -= 10;
	}

	return  (high << 4) | input;
}

/** Convert a character into a BCD encoded character. The input must be in
	the range 0 to 255. The result is an integer where the three lowest
	nibbles contain the ones, tens and hundreds of the input.
	@param input A byte in the range 0 to 255.
	@result An integer where the three lowes nibbles contain the ones, tens
	and hundreds of the input.
*/
uint16_t CharToBCD3(uint8_t input)
{
	uint16_t high = 0;

	while (input >= 100)
	{
		high++;
		input -= 100;
	}

	high <<= 4;

	while (input >= 10)
	{
		high++;
		input -= 10;
	}

	return  (high << 4) | input;
}
