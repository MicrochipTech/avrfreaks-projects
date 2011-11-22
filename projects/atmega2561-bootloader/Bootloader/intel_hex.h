#ifndef INTEL_HEX_H
#define INTEL_HEX_H
#include <stdbool.h>
#include "bootloader.h"

#define IHEX_DATA_SIZE_INDEX 0
#define IHEX_DATA_ADDR_INDEX 1
#define IHEX_DATA_TYPE_INDEX 3
#define IHEX_DATA_START_INDEX 4

typedef enum intel_hex_data_types { DATA_RECORD, EOF_RECORD, EXT_SEG_ADDR_REC, START_SEG_ADDR_REC, EXT_LIN_ADDR_REC, START_LIN_ADDR_REC } ihex_types_t;

extern unsigned char hex_index;
extern unsigned char hex_data_size;
extern int hex_checksum;
extern unsigned char hex_in_byte;
extern unsigned char hex_odd;
extern unsigned char hex_type;
extern unsigned int hex_address_ext;
extern unsigned int hex_address;

extern page_data_t incoming_page;
extern page_data_t flash_complete_page;
extern bool flash_page_written;
extern page_eeprom_t incoming_eeprom_page;

extern void (*enqueue_page)(void);
extern void (*data_record)(void);

void init_ihex();
unsigned char ascii_to_bin(unsigned char data);
void parse_ihex(unsigned char byte);
void eeprom_enqueue_page();
void eeprom_data_record();
void flash_enqueue_page();
void flash_data_record();
void flash_fill_end();

#endif
