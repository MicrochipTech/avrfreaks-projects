#include <avr/pgmspace.h>
#include "GameBoy_Info.h"
#include "memory_latched.h"

// Gameboy global variables
GBROMDATA gbRomData;

void get_GBROM_info(GBROMDATA *gbromData) {

  flashAddress.address32 = 0x0134; // Offset to filename
  // Read ROM name from the flash chip
  for(uint8_t i=0; i< 17; i++, flashAddress.address32++) {
    sst_flash_read_byte(flashAddress, &((*gbromData).name[i]));
  }
  (*gbromData).name[16] = '\0';

  flashAddress.address32 = 0x0148;
  sst_flash_read_byte(flashAddress, &((*gbromData).ROMsize));

  flashAddress.address32 = 0x0149;
  sst_flash_read_byte(flashAddress, &((*gbromData).RAMsize));

  flashAddress.address32 = 0x0147;
  sst_flash_read_byte(flashAddress, &((*gbromData).MBCtype));

  flashAddress.address32 = 0x0143;
  sst_flash_read_byte(flashAddress, &((*gbromData).CGBsupport));

  // Now fill out the additional GBROMDATA fields
  // Convert hex ROM size to bytes ROM size
  if((*gbromData).ROMsize == GBROM_32k) {
    (*gbromData).ROMsizeBytes = 32768;
  } else if ((*gbromData).ROMsize == GBROM_64k) {
    (*gbromData).ROMsizeBytes = 65536;
  } else if ((*gbromData).ROMsize == GBROM_128k) {
    (*gbromData).ROMsizeBytes = 131072;
  } else if ((*gbromData).ROMsize == GBROM_256k) {
    (*gbromData).ROMsizeBytes = 262144;
  } else if ((*gbromData).ROMsize == GBROM_512k) {
    (*gbromData).ROMsizeBytes = 524288;
  } else if ((*gbromData).ROMsize == GBROM_1M) {
    (*gbromData).ROMsizeBytes = 1048576;
  } else if ((*gbromData).ROMsize == GBROM_2M) {
    (*gbromData).ROMsizeBytes = 2097152;
  } else {
    (*gbromData).ROMsizeBytes = 0;
  }

  // Convert the RAM size
  if((*gbromData).RAMsize == GBRAM_0k) {
    (*gbromData).RAMsizeBytes = 0;
  } else if((*gbromData).RAMsize == GBRAM_2k) {
    (*gbromData).RAMsizeBytes = 2048;
  } else if((*gbromData).RAMsize == GBRAM_8k) {
    (*gbromData).RAMsizeBytes = 8192;
  } else if((*gbromData).RAMsize == GBRAM_32k) {
    (*gbromData).RAMsizeBytes = 32768;
  } else if((*gbromData).RAMsize == GBRAM_64k) {
    (*gbromData).RAMsizeBytes = (uint32_t) 65536;
  } else if((*gbromData).RAMsize == GBRAM_128k) {
    (*gbromData).RAMsizeBytes = 131072UL;
  } else {
    (*gbromData).RAMsizeBytes = 0;
  }

  // Get the MBC mode name
  if((*gbromData).MBCtype == GBCART_ROM_ONLY) {
    strcpy_P((*gbromData).MBCname,PSTR("ROM only"));
  } else if((*gbromData).MBCtype == GBCART_MBC1) {
    strcpy_P((*gbromData).MBCname,PSTR("MBC1"));
  } else if((*gbromData).MBCtype == GBCART_MBC1_RAM) {
    strcpy_P((*gbromData).MBCname,PSTR("MBC1 RAM"));
  } else if((*gbromData).MBCtype == GBCART_MBC1_RAM_BATT) {
    strcpy_P((*gbromData).MBCname,PSTR("MBC1 RAM BATT"));
  } else if((*gbromData).MBCtype == GBCART_MBC5) {
    strcpy_P((*gbromData).MBCname,PSTR("MBC5"));
  } else if((*gbromData).MBCtype == GBCART_MBC5_RAM) {
    strcpy_P((*gbromData).MBCname,PSTR("MBC5 RAM"));
  } else if((*gbromData).MBCtype == GBCART_MBC5_RAM_BATT) {
    strcpy_P((*gbromData).MBCname,PSTR("MBC5 RAM BATT"));
  } else if((*gbromData).MBCtype == GBCART_MBC5_RUMBLE) {
    strcpy_P((*gbromData).MBCname,PSTR("MBC5 RUMBLE"));
  } else if((*gbromData).MBCtype == GBCART_MBC5_RUMBLE_RAM) {
    strcpy_P((*gbromData).MBCname,PSTR("MBC5 RUMBLE RAM"));
  } else if((*gbromData).MBCtype == GBCART_MBC5_RUMBLE_RAM_BATT) {
    strcpy_P((*gbromData).MBCname,PSTR("MBC5 RUMBLE RAM BATT"));
  } else if((*gbromData).MBCtype == GBCART_MBC3_RTC_BATT) {
    strcpy_P((*gbromData).MBCname,PSTR("MBC3 RTC BATT"));
  } else if((*gbromData).MBCtype == GBCART_MBC3_RTC_RAM_BATT) {
    strcpy_P((*gbromData).MBCname,PSTR("MBC3 RTC RAM BATT"));
  } else if((*gbromData).MBCtype == GBCART_MBC3) {
    strcpy_P((*gbromData).MBCname,PSTR("MBC3"));
  } else if((*gbromData).MBCtype == GBCART_MBC3_RAM) {
    strcpy_P((*gbromData).MBCname,PSTR("MBC3 RAM"));
  } else if((*gbromData).MBCtype == GBCART_MBC3_RAM_BATT) {
    strcpy_P((*gbromData).MBCname,PSTR("MBC3 RAM BATT"));
  } else if((*gbromData).MBCtype == GBCART_MBC2) {
    strcpy_P((*gbromData).MBCname,PSTR("MBC2"));
    (*gbromData).RAMsizeBytes = 512; // Special case here, as MBC2 header may not disclose internal MBC RAM
  } else if((*gbromData).MBCtype == GBCART_MBC2_BATT) {
    strcpy_P((*gbromData).MBCname,PSTR("MBC2 BATT"));
    (*gbromData).RAMsizeBytes = 512; // Special case here, as MBC2 header may not disclose internal MBC RAM
  } else {
    strcpy_P((*gbromData).MBCname,PSTR("Unsupported MBC type"));
  }
  
  if((*gbromData).CGBsupport == GB_PRE_CGB) {
    strcpy_P((*gbromData).CGBsupportText, PSTR("Pre-CGB"));
  } else if ((*gbromData).CGBsupport == GB_AND_CGB) {
    strcpy_P((*gbromData).CGBsupportText, PSTR("GB & CGB"));
  } else if ((*gbromData).CGBsupport == CGB_ONLY) {
    strcpy_P((*gbromData).CGBsupportText, PSTR("CGB only"));
  } else {
    strcpy_P((*gbromData).CGBsupportText, PSTR("UNK VAL"));
  }
}
