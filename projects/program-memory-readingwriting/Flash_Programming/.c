/*****************************************************
Project : Program Flash in-program
Date    : 11 JUN 2010
Author  : Luke Wardensky
Comments: Demonstrates routines to read and write to
	the flash memory from inside a program.

Requirements: Peter Fleury's UART library http://jump.to/fleury
			  write_flash_page must be located in .bootloader section of memory

Chip type           : ATmega64
Clock frequency     : 8.000000 MHz
*****************************************************/
#define F_CPU 8000000UL
#define BAUD 9600

#include <avr/io.h>
#include <avr/boot.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <uart.h>
#include <util/delay.h>

BOOTLOADER_SECTION void write_flash_page(unsigned int FlashAddr, unsigned char *buf);

int main()
{
	unsigned char buffer[256] = {"This string was written to and read from flash memory by the Flash_Programming program.\r\n"};
	char read_buffer[256];

	asm("SEI");
	uart1_init(UART_BAUD_SELECT(BAUD, F_CPU));

	uart1_puts("Writing string to flash.\r\n");
	write_flash_page(0x6400, buffer);


	uart1_puts("Reading from flash:\r\n");
	for(int i=0; i<256; i++)
	{
		read_buffer[i] = pgm_read_byte(0x6400+i);
	}

	read_buffer[255] = '\0';
	uart1_puts(read_buffer);

	while(1);

	return 0;
}

/////////////////////////////////////////////////
// write_flash_page
// Arguments: FlashAddr is the byte address for the start of the flash page to be written.
//			  buf is the 256 byte array to be written to FlashAddr
/////////////////////////////////////////////////
BOOTLOADER_SECTION void write_flash_page(unsigned int FlashAddr, unsigned char *buf)
{
	unsigned int pagptr;

	eeprom_busy_wait();
	boot_page_erase(FlashAddr);                  //erase one Flash page
	boot_spm_busy_wait();
	for(pagptr = 0; pagptr < SPM_PAGESIZE; pagptr += 2) //fill data to Flash buffer
	{
  		boot_page_fill(pagptr, buf[pagptr] | (buf[pagptr + 1] << 8));
	}

	boot_page_write(FlashAddr);                  //write buffer to one Flash page
	boot_spm_busy_wait();                        //wait Flash page write finish
	boot_rww_enable();                  //enable application section
}
