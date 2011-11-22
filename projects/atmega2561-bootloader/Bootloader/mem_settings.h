#ifndef MEM_SETTINGS_H
#define MEM_SETTINGS_H

#include <ioavr.h>

#define SPMCR_REG SPMCSR

#define EEPROM_PAGE_SIZE 16

#define BOOT_SIZE 8192
#ifndef SPM_PAGESIZE
#if defined(__AT90CAN128__)
#define SPM_PAGESIZE 256 // page size in bytes
#else
#error Page size not defined for this MPU!
#endif
#endif

#define APP_END FLASHEND - BOOT_SIZE // Should be 0x1DFFF
#define APP_PAGES APP_END/SPM_PAGESIZE

#endif
