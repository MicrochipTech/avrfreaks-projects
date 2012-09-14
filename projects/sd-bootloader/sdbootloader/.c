/*
 * sdboot.c
 *
 * Created: 05/09/2012 07:45:02
 *  Author: Giuseppe
 */ 

#include <math.h>
#include <util/delay.h>
#include <avr/boot.h>
#include <avr/wdt.h>
#include "SdCard.h"
#include "Fat16.h"

SdCard sdcard;
Fat16 fs;

// name of the file for reflashing
#define FLASHFILE "reflash.bin"

// size of bootsize in bytes
#define BOOTSIZE	8192

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
void blink(const uint8_t times)
{
	DDRK = 0xff;
	DDRL |= _BV(5);
	for (uint8_t i = 0; i < times; i++)
	{
		PORTK = 0xff;
		PORTL &= ~_BV(5);
		_delay_ms(150);
		PORTK = 0;
		PORTL |= _BV(5);
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

	// Clear WDRF in MCUSR
	MCUSR &= ~(1<<WDRF);
	// Now disable WD
	wdt_disable();

	// bootloader ready
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
		// An update file exists
		blink(3);
		while ( fs.read(buffer,sizeof(buffer)) )
		{
			boot_program_page(pageAddress,buffer);
			pageAddress += SPM_PAGESIZE;
			// This check makes the led blink more slowly
			if (pageAddress & 0x0200)
			PORTL ^= _BV(5);
			// Stop at the end of application space, don't update bootloader
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
		Anyway, there is nothing to do if not reset the MCU
	*/
	// Start application
	SPL = (RAMEND & 0xFF);
	SPH = (RAMEND >> 8);
	asm("jmp 0");
}
