#ifndef __SST39SF040__
#define __SST39SF040__

#include <avr/io.h>

// Funcs to read and write from this flash IC. Port info is hardwired atm.

typedef union FlashAddr {
   uint32_t address32;
   uint8_t address8[4];
} FLASHADDR;

extern FLASHADDR flashAddress;

#define SST_MFR_ID            0xBF    // I don't read this byte. I'm more concerned about size
#define SST_512K_ID           0xB7    // SST39SF040
#define SST_512K_MAX_ADDRESS  524287  // SST39SF040 has 524288 bytes etc...

#define MAC_MFR_ID            0xC2
#define MAC_1MB_ID            0xD6 // Don't think it's 0x58
#define MAC_1MB_MAX_ADDRESS   

void memory_init(void);

uint8_t mac_flash_read_byte(FLASHADDR address, uint8_t *returnData); // Address is IC agnostic
uint8_t mac_flash_write_byte(FLASHADDR address, uint8_t writeData); // Address is IC agnostic
uint8_t mac_flash_chip_erase(uint8_t chipNum);// Pretty self explanatory, chipNum is 0-7
uint8_t mac_read_softwareID(uint8_t chipNum); // Returns the chip ID (not MFR ID), chipNum is 0-1

uint8_t sram_read_byte(uint16_t address, uint8_t *returnData);
uint8_t sram_write_byte(uint16_t address, uint8_t writeData);

#endif
