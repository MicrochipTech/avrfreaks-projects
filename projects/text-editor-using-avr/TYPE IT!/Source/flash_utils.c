
#include <avr/boot.h>
#include <avr/interrupt.h>

void WritePage (uint16_t page, const uint8_t *buf) __attribute__ ((section (".bootloader")));


/* Write one flash page(starting at 'page') from buffer memory; 
/Disables interrupts, reenable at end */
void WritePage (uint16_t page, const uint8_t *buf) 
{

	uint8_t i;
	uint8_t sreg;

	// Disable interrupts.
	sreg = SREG;
	cli();

	boot_page_erase(page);
	boot_spm_busy_wait();      // Wait until the memory is erased.

	for (i=0; i<SPM_PAGESIZE; i+=2)
	{
		// Set up little-endian word.
		uint16_t w = *buf++;
		w += (*buf++) << 8;
		boot_page_fill(page + i, w);
	}

	boot_page_write(page);     // Store buffer in flash page.
	boot_spm_busy_wait();       // Wait until the memory is written.

	// Reenable RWW-section again. We need this if we want to jump back
	// to the application after bootloading.
	boot_rww_enable(); 
	
	// Re-enable interrupts
	sei();
	SREG = sreg;
	
}



	