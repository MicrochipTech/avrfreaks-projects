#include "eeprom_stuff.h"
#include <avr/eeprom.h>
#include <avr/power.h>
#include <avr/pgmspace.h>

GBEEPROMDATA eepData;

// Need funcs in here to retrieve, compare and store eepData

/*
typedef struct GBEEPROMDat {
  uint8_t MBCMode;
  char MBCName[9];
  uint32_t uartBaud;
  uint16_t writeCount; // So we can move to the next address if need be

} GBEEPROMDATA;

EEPROM layout:
0x00    MBCMode
0x01-09 MBCName
0x0A-0D uartBaud
0x0E-0F writeCount
*/

uint8_t read_gbeepromdat(GBEEPROMDATA *eepData) {
  
  (*eepData).MBCMode = eeprom_read_byte(0x00); // Address 0
  (*eepData).uartBaud = eeprom_read_dword((uint32_t*)0x0A);
  (*eepData).writeCount = eeprom_read_word((uint16_t*)0x0E);

  if((*eepData).MBCMode == CART_MODE_MBC1_16M) {
    strcpy_P((*eepData).MBCName, PSTR("MBC1_16M"));
  } else if((*eepData).MBCMode == CART_MODE_MBC1_4M) {
    strcpy_P((*eepData).MBCName, PSTR("MBC1_4M "));
  } else if((*eepData).MBCMode == CART_MODE_MBC2) {
    strcpy_P((*eepData).MBCName, PSTR("MBC2    "));
  } else if((*eepData).MBCMode == CART_MODE_MBC3) {
    strcpy_P((*eepData).MBCName, PSTR("MBC3    "));
  } else if((*eepData).MBCMode == CART_MODE_MBC5) {
    strcpy_P((*eepData).MBCName, PSTR("MBC5    "));
  } else {
    strcpy_P((*eepData).MBCName, PSTR("MODE UNK"));
  }

  return 0;
}

uint8_t write_update_gbeepromdat(GBEEPROMDATA *eepData) {
  
  

  (*eepData).writeCount++;
// Trying to drop the clock speed for writes, as I seem to have errors
//clock_prescale_set(clock_div_2);
  eeprom_update_byte(0x00, (*eepData).MBCMode);
  eeprom_update_dword((uint32_t*)0x0A, (*eepData).uartBaud);
  eeprom_update_word((uint16_t*)0x0E, (*eepData).writeCount);

//clock_prescale_set(clock_div_1);

  return 0;
}

uint8_t compare_gbeepromdat(GBEEPROMDATA *eepData) {
  
  GBEEPROMDATA temp;
  temp.MBCMode = eeprom_read_byte(0x00); // Address 0
  temp.uartBaud = eeprom_read_dword((uint32_t*)0x0A);

  if (temp.MBCMode != (*eepData).MBCMode) {
    return 1;
  } else if (temp.uartBaud != (*eepData).uartBaud){
    return 1;
  } 

  return 0;
}

uint8_t program_initial_gbeepromdat(uint8_t MBCmode) {

  eeprom_write_byte(0x00, MBCmode);
  eeprom_write_dword((uint32_t*)0x0A, 115200UL);
  eeprom_write_word((uint16_t*)0x0E, 1);

  return 0;
}

uint8_t read_MBC_mode_only() {
  return eeprom_read_byte(0x00);
}
