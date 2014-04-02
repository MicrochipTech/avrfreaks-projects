/*
 * CRC_calc.c
 *
 * Created: 3/31/2014 7:54:45 PM
 *  Author: John.Cassidento
 */ 


#include <avr/io.h>
#include <util/crc16.h>
#include <avr/pgmspace.h>

// Assumes Flash section added in Project Properties >Toolchain > Linker > Memory Settings
//    .crc = 0xFFF
// Note that the above address is a word address, so byte address of 0x1FFE (8 kB) divided by 2.
#define CRC_SECTION __attribute__((section(".crc")))	

uint16_t const actual_crc CRC_SECTION = 0x3A39;		// arrived at by running once

	uint8_t byte = 0;								// to be moved into main
	uint16_t crc = 0;
	uint16_t word = 0;
	uint16_t flash_index = 0;

int main(void)
{
	PORTB |= (1 << PB3) | (1 << PB4);				// set outputs high for LED off
	DDRB |= (1 << PB3) | (1 << PB4);				// enable outputs

	for (flash_index = 0; flash_index <= FLASHEND-2; flash_index++)
	{
		byte = pgm_read_byte(flash_index);
		crc = _crc16_update(crc, byte);
	}

	word = pgm_read_word(&(actual_crc));

	if (crc == word)								// set breakpoint here to get actual CRC value
	{
		PORTB &= ~(1 << PB4);						// turn on LED
	} 
	else
	{
		PORTB &= ~(1 << PB3);						// turn on LED
	}
		
    while(1)
    {
        //TODO:: Please write your application code 
    }
}