/*
 * sdbootloader.cpp
 *
 * Created: 05/09/2012 07:53:04
 *  Author: Giuseppe Scarpi
 *
 *  A simple bootloader for ATMega 1280/2560, that reads a file from the
 *  SD Card, and writes the content to the flash. At the end, performs
 *  a reboot.
 *
 *  The code relies on two Arduino libraries, but can run on any hardware
 *  with an SD socket connected to the SPI interface.
 *
 *  Be warned that the FAT16 library cannot handle cards larger than 2Gb.
 *  I am aware of other FAT libraries that can also manage 4 Gb cards, but
 *  at the moment I don't have the time to do the porting.
 *
 *  To reflash your MCU you need to:
 *  - enable the bootloader fuse, and select the largest bootloader area,
 *		4096 words
 *	- write the bootloader in the bootloader area
 *	- convert the file you intend to write from HEX to binary. There are
 *		various utilities around. Personally, I suggest srec_cat, open
 *		source and very flexible.
 *	- copy the file in the root of SD. It must have a fixed name, that you
 *		can set by changing #define FLASHFILE.
 *
 *  If you use srec_cat, then the command line to convert the file into
 *  binary is:
 *  srec_cat <filename>.hex - I -o <filename>.bin -BIN
 *
 * This Library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This Library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You can obtain a copy of the GNU General Public License here
 * <http://www.gnu.org/licenses/>.
 *
 */

#include <util/delay.h>
#include <avr/boot.h>
#include <avr/wdt.h>
#include "SdCard.h"
#include "Fat16.h"

// name of the file for reflashing
#define FLASHFILE "REFLASH.BIN"

// size of bootsize in bytes
#define BOOTSIZE	8192

// Definitions for LED used to indicate reflashing phases
#define LEDPORT PORTL
#define LEDDDR	DDRL
#define LEDBIT _BV(5)

SdCard sdcard;
Fat16 fs;

extern uint8_t __stack;
extern uint8_t _end;

void __jumpMain	(void) __attribute__ ((naked)) __attribute__ ((section (".init9")));

void __jumpMain(void)
{
	SPL = (RAMEND & 0xFF);
	SPH = (RAMEND >> 8);
	asm volatile ( "clr __zero_reg__" );									// GCC depends on register r1 set to 0
	asm volatile ( "out %0, __zero_reg__" :: "I" (_SFR_IO_ADDR(SREG)) );	// set SREG to 0
	asm volatile ( "jmp main");												// jump to main()
}

// Blink pane leds and internal led to give some rudimental indication
// In my case I have a LED on port L. You will need to change the port/pin
// by changing the #define or disable the function if you have no HMI
void blink(const uint8_t times)
{
	LEDDDR |= LEDBIT;
	for (uint8_t i = 0; i < times; i++)
	{
		LEDPORT &= ~LEDBIT;
		_delay_ms(150);
		LEDPORT |= LEDBIT;
		_delay_ms(150);
	}
	_delay_ms(1500);
}

void boot_program_page (uint32_t page, uint8_t *buf)
{
	uint16_t i;

	eeprom_busy_wait ();
	boot_page_erase (page);
	boot_spm_busy_wait ();      // Wait until the memory is erased.

	for (i=0; i<SPM_PAGESIZE; i+=2)
	{
		// Set up little-endian word.
		uint16_t w = *buf++;
		w += (*buf++) << 8;
		boot_page_fill (page + i, w);
	}
	boot_page_write (page);     // Store buffer in flash page.
	boot_spm_busy_wait();       // Wait until the memory is written.

	// Reenable RWW-section again. We need this if we want to jump back
	// to the application after bootloading.
	boot_rww_enable ();
}

int main(void)
{
	uint8_t buffer[SPM_PAGESIZE];
	uint32_t pageAddress = 0;
	int16_t fileIndex;

	// immediately disable watchdog, just in case

	// Clear WDRF in MCUSR, otherwise wdt_disable() can't work
	MCUSR &= ~(1<<WDRF);
	// Now disable WD
	wdt_disable();

	// If you have a LED available, you can use it to warn the user that
	// bootloader has started
	blink(2);

	/*
	 *	Reflashing requires some conditions to start:
	 *	- SD card must be inserted, valid, and with a valid filesystem
	 *	- a firmware upgrade file must exist and can be opened
	 */

	if (
		sdcard.init(1)
		&&
		fs.init(&sdcard)
		&&	// Check if a flash update exists
		((fileIndex = fs.getFileDirEntry(FLASHFILE)) != -1)
		&&
		fs.open(fileIndex,O_RDONLY)
	)
	{
		// An update file exists, warns the user
		blink(3);
		while ( fs.read(buffer,sizeof(buffer)) )
		{
			boot_program_page(pageAddress,buffer);
			pageAddress += SPM_PAGESIZE;
			// blink() is too slow, so I prefer to directly toggle the LED
			// at each page write
			LEDPORT ^= LEDBIT;
			// Stop at the end of application space, don't update bootloader
			// no matter how long is the file
			if (pageAddress > (FLASHEND -(2*BOOTSIZE) + 1))
				break;
		}
		fs.close();
		// Update completed. Remove the file to avoid re-reflashing in case
		// of further boot
		fs.remove(FLASHFILE);
		blink(4);
	}

	/*
		We can arrive here after a successful update or in case or error.
		Anyway, there is nothing else to do than resetting the MCU
	*/
	SPL = (RAMEND & 0xFF);
	SPH = (RAMEND >> 8);
	asm("jmp 0");
}
