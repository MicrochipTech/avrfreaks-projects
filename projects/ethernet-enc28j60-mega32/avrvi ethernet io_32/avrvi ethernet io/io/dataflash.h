#ifndef _DATAFLASH_H_
#define _DATAFLASH_H_

#include <avr/io.h>

//pin config
#include "../config.h"

//rst/cs macros
#define DATAFLASH_RESET_LO() DATAFLASH_PORT &= ~(1<<DATAFLASH_PIN_RST);
#define DATAFLASH_RESET_HI() DATAFLASH_PORT |= (1<<DATAFLASH_PIN_RST);
#define DATAFLASH_CS_LO()    DATAFLASH_PORT &= ~(1<<DATAFLASH_PIN_CS);
#define DATAFLASH_CS_HI()    DATAFLASH_PORT |= (1<<DATAFLASH_PIN_CS);

//dataflash globals:
#define DATAFLASH_MAX_PAGE 2048


//dataflash registers:
#define DATAFLASH_BLOCK_ERASE         0x50
#define DATAFLASH_MEM_PAGE_READ       0x52
#define DATAFLASH_READ_BUFFER0        0xD4
#define DATAFLASH_READ_BUFFER1        0xD6
#define DATAFLASH_MEM_TO_BUFFER0      0x53
#define DATAFLASH_MEM_TO_BUFFER1      0x55
#define DATAFLASH_MEM_COMP_BUFFER0    0x60
#define DATAFLASH_MEM_COMP_BUFFER1    0x61
#define DATAFLASH_WRITE_BUFFER0       0x84
#define DATAFLASH_WRITE_BUFFER1       0x87
#define DATAFLASH_BUFFER0_TO_MEM_WE   0x83
#define DATAFLASH_BUFFER1_TO_MEM_WE   0x86
#define DATAFLASH_BUFFER0_TO_MEM_NE   0x88
#define DATAFLASH_BUFFER1_TO_MEM_NE   0x89
#define DATAFLASH_MEM_WR_TH_BUFFER0   0x82
#define DATAFLASH_MEM_WR_TH_BUFFER1   0x85
#define DATAFLASH_AUTO_RW_TH_BUFFER0  0x58
#define DATAFLASH_AUTO_RW_TH_BUFFER1  0x59
#define DATAFLASH_READ_STATUS         0xD7

#define DATAFLASH_READY 							0x80

//initialise dataflash
void dataflash_init(void);

//dump flash content using given putchar function:
void dataflash_dump(void);

//copy given buffer to flash page
void dataflash_copy_buffer_to_page(unsigned int page, unsigned char buffer);

//write one byte to given page buffer
void dataflash_write_to_page_buffer(unsigned int byte, unsigned char buffer, unsigned char data);
//write len bytes to given page buffer
void dataflash_write_n_to_page_buffer(unsigned int byte, unsigned char buffer, unsigned char *data, unsigned int len);

//copy flash page to given buffer
void dataflash_copy_page_to_buffer(unsigned int page, unsigned char buffer);

//read one byte from buffer
unsigned char dataflash_read_buffer(unsigned int byte, unsigned char buffer);
//read 256 byte to buffer ptr
void dataflash_read_n_to_buffer(unsigned char *buffer, unsigned char selbuf);

//read 1 byte directly from flash:
unsigned char dataflash_read_flash(unsigned int page, unsigned int byte);

//wait for dataflash ready flag
void dataflash_busy_wait(void);

//read&write one byte on spi
unsigned char dataflash_spi_writeread_byte(unsigned char data);
#endif

