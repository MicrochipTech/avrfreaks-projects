#ifndef _EEPROM_STUFF_
#define _EEPROM_STUFF_

#include <avr/io.h>

// EEPROM data for settings storage
typedef struct GBEEPROMDat {
  uint8_t MBCMode;
  char MBCName[9];
  uint32_t uartBaud;
  uint16_t writeCount; // So we can move to the next address if need be

} GBEEPROMDATA;

extern GBEEPROMDATA eepData;

// EEPROM defines for our cart mode. 0x40 is the high byte offset we'll use to IJMP later!
#define CART_MODE_MBC1_16M  0x40 // "MBC1_16M"
#define CART_MODE_MBC1_4M   0x41 // "MBC1_4M "
#define CART_MODE_MBC5      0x42 // "MBC5    "
#define CART_MODE_MBC3      0x43 // "MBC3    "
#define CART_MODE_MBC2      0x45 // "MBC2    "

uint8_t read_gbeepromdat(GBEEPROMDATA *eepData);
uint8_t write_update_gbeepromdat(GBEEPROMDATA *eepData);
uint8_t compare_gbeepromdat(GBEEPROMDATA *eepData); // Returns 1 if EEPROM values != eepData values (else 0)
uint8_t program_initial_gbeepromdat(uint8_t MBCMode); // Initialiser function for EEPROM
uint8_t read_MBC_mode_only(); // returns the MBC mode value from EEPROM (for starting quickly)
#endif 
