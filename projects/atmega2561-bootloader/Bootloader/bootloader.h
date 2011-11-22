#ifndef BOOTLOADER_H
#define BOOTLOADER_H
#include <intrinsics.h>
#include <stdbool.h>
#include "mem_settings.h"

/**
 * Versioning:  Versions are built up by a high byte and a low byte. The high
 *              byte represents function changes such as new features and bug 
 *              fixes. The low byte represents changes in the communication
 *              protocol. The low byte must be the same in the bootloader and
 *              in the BootloaderClient running on the PC.
 * 
 * 
 */

#define BL_VER_HIGH 0x00
#define BL_VER_LOW 0x02

/** 
 * Uncomment USART to enable the server MCU code (the server MCU is
 * connected to the PC through RS232). Uncomment CAN to enable the client MCU
 * code. USART and CAN are mutually exclusive!
 * OVERLAY and PROCESSOR are also mutually exclusive and defines the led pins.
 */

#define USART
#define OVERLAY
//#define CAN
//#define PROCESSOR

//#define DEBUG_FLASH_WRITE
#define ENABLE_FLASH_WRITE
#define ENABLE_FLASH_ERASE
#define ENABLE_EEPROM_WRITE

/**
 * Change APP_INFO_ADDR to the location in EEPROM where the application_info
 * is to be stored. The app_info is 16 bytes.
 */
#define APP_INFO_ADDR 0
#define _WAIT_FOR_SPM() while( SPMCR_REG & (1<<SPMEN) );
#define _ENABLE_RWW_SECTION() __DataToR0ByteToSPMCR_SPM( 0x00, 0x11 )

typedef enum bootload_state { NORMAL, TRANSFER_CODE_HEADER, TRANSFER_EEPROM_HEADER, TRANSFER_CODE_STATE, TRANSFER_EEPROM_STATE, RUN_APP, FORWARD, WAIT_APP_INFO } bl_state_t;

typedef struct page_data {
  unsigned char data[SPM_PAGESIZE];
  unsigned int page_ptr;
} page_data_t;

typedef struct page_eeprom {
  unsigned char data[EEPROM_PAGE_SIZE];
  unsigned int address;
  unsigned int page_ptr;
} page_eeprom_t;

typedef struct application_info {
  unsigned int address;
  unsigned int version;
  char name[8];
  unsigned long date;
} app_info_t;

extern bool transfer_init;
extern volatile bl_state_t state;
extern volatile unsigned long rx_counter;
extern bool (*mem_write_ptr)(void);

void store_app_info(app_info_t *app_info);
void read_app_info(app_info_t *app_info);
bool write_flash_page();
bool write_eeprom_page();
//void add_to_crc16(unsigned char byte);

#endif
