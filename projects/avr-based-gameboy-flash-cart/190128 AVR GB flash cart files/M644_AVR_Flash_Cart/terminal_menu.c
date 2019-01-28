#include <avr/io.h>
#include <util/delay.h>
#include <avr/pgmspace.h>
#include "../01_INCLUDE/usart_funcs.h"
#include "../01_INCLUDE/ymodem_funcs.h"
#include "memory_latched.h"
#include "terminal_menu.h"
#include "GameBoy_Info.h"
#include "eeprom_stuff.h"

// Menu defines
#define MENU_ROM_INFO   0x00
#define MENU_RX_ROM     0x01
#define MENU_DUMP_ROM   0x02
#define MENU_RX_SAVE    0x03
#define MENU_DUMP_SAVE  0x04
#define MENU_FLASH_DIAG 0x05
#define MENU_FORCE_MBC  0x06
#define MENU_NEW_FW     0x07
#define MENU_GB_MODE    0x08

// Menu function prototypes
uint8_t show_menu(void);
uint8_t show_ROM_info();
uint8_t receive_ROM();
uint8_t dump_ROM();
uint8_t receive_save();
uint8_t dump_save();
uint8_t flash_diagnostic();
uint8_t force_mbc_mode();
uint8_t flash_new_firmware();
void enter_GB_mode();

void launch_mbc_mode(void);
uint8_t wait_for_confirm_cancel();
void wait_for_continue();

extern void MBC_mode_MBC1_16M8k(void);
extern void MBC_mode_MBC1_4M32k(void);
extern void MBC_mode_MBC2(void);
extern void MBC_mode_MBC3(void);
extern void MBC_mode_MBC5(void);

// Menu PROGMEM strings
char commonReturn[] PROGMEM = "Press SPACEBAR to return to the main menu\r\n";
char commonAbort[] PROGMEM = "\'N\' received. Aborting.\r\n";
char commonEmpty[] PROGMEM = "Not implemented yet\r\n";

// Global variables
uint32_t fileSize = 0;
uint32_t RAMaddress = 0;
uint16_t RAMWriteErrors;
uint16_t RAMWriteErrorsAddrFirst;
uint16_t RAMWriteErrorsAddrLast;

// Function prototypes
void flash_buff_receive(unsigned char* buffer, uint16_t byteCnt);
uint8_t flash_buff_send(unsigned char* buffer, uint16_t byteCnt);
void sram_buff_receive(unsigned char* buffer, uint16_t byteCnt);
uint8_t sram_buff_send(unsigned char* buffer, uint16_t byteCnt);

// *************************************** MENU BELOW HERE *********************************************** 
/*
  Return vals for use in the main program:
  0 - Get ROM info from flash
  1 - Receive a ROM from the PC
  2 - Dump a ROM to the PC
  3 - Receive a SRAM save from the PC
  4 - Dump a SRAM save to the PC
  5 - Flash diagnostic (confirm present)
  6 - UART settings (change BAUD)

*/
uint8_t show_menu(void) {

  uint8_t menuVal;
  
  while(1) {

    usart_clear_ht();
    usart_transmit_str_P(PSTR("Welcome to Blake's GB flash cart\r\nChoose one of the following options\r\n\r\n"));
    usart_transmit_str_P(PSTR("0 - Show system and ROM information\r\n"));
    usart_transmit_str_P(PSTR("1 - Download new ROM to cart\r\n"));
    usart_transmit_str_P(PSTR("2 - Dump ROM to PC\r\n"));
    usart_transmit_str_P(PSTR("3 - Download save file to cart\r\n"));
    usart_transmit_str_P(PSTR("4 - Dump save file to PC\r\n"));
    usart_transmit_str_P(PSTR("5 - View flash IC detection\r\n"));
    usart_transmit_str_P(PSTR("6 - Force MBC mode (for incorrect cartridge header or compatibility tests)\r\n"));
    usart_transmit_str_P(PSTR("7 - Flash new cart firmware (must be <56KB due to placement of bootloader)\r\n"));
    usart_transmit_str_P(PSTR("8 - Enter Gameboy mode - WARNING! DISCONNECT UART!\r\n"));
    usart_transmit_str_P(PSTR("\r\nDisconnect the cable before using the cart in a Gameboy!\r\n"));

    usart_transmit_str_P(PSTR("\r\nUser selection: "));
    
    menuVal = usart_receive() - '0';
    usart_transmit(menuVal + '0');
    usart_newline();

    if(menuVal == MENU_ROM_INFO) {
      show_ROM_info();

    } else if (menuVal == MENU_RX_ROM) {
      receive_ROM();

    } else if(menuVal == MENU_DUMP_ROM) {
      dump_ROM();

    } else if(menuVal == MENU_RX_SAVE) {
      receive_save();
    
    } else if(menuVal == MENU_DUMP_SAVE) {
      dump_save();
    
    } else if(menuVal == MENU_FLASH_DIAG) {
      flash_diagnostic();
    
    } else if(menuVal == MENU_FORCE_MBC) {
      force_mbc_mode();

    } else if(menuVal == MENU_NEW_FW) {
      flash_new_firmware();

    } else if(menuVal == MENU_GB_MODE) {
      enter_GB_mode();      

    } else {

      usart_transmit_str_P(PSTR("Unknown request\r\n"));
      wait_for_continue();
    }
  }

  return 0;
}

uint8_t show_ROM_info() {
  
  get_GBROM_info(&gbRomData);

  usart_transmit_str_P(PSTR("Gameboy ROM info (from flash):\r\n"));
  usart_transmit_str_P(PSTR("ROM name:         "));
  usart_transmit_str(gbRomData.name);
  usart_newline();
  usart_transmit_str_P(PSTR("ROM size:         0x"));
  usart_transmit(nibble_to_hex(gbRomData.ROMsize >> 4));
  usart_transmit(nibble_to_hex(gbRomData.ROMsize));
  usart_transmit(' ');
  usart_transmit_int32(gbRomData.ROMsizeBytes);
  usart_newline();
  usart_transmit_str_P(PSTR("SRAM size:        0x"));
  usart_transmit(nibble_to_hex(gbRomData.RAMsize >> 4));
  usart_transmit(nibble_to_hex(gbRomData.RAMsize));
  usart_transmit(' ');
  usart_transmit_int32(gbRomData.RAMsizeBytes);
  usart_newline();
  usart_transmit_str_P(PSTR("MBC type:         0x"));
  usart_transmit(nibble_to_hex(gbRomData.MBCtype >> 4));
  usart_transmit(nibble_to_hex(gbRomData.MBCtype));
  usart_transmit(' ');
  usart_transmit_str(gbRomData.MBCname);
  usart_newline();
  usart_transmit_str_P(PSTR("CGB support?:     0x"));
  usart_transmit(nibble_to_hex(gbRomData.CGBsupport >> 4));
  usart_transmit(nibble_to_hex(gbRomData.CGBsupport));
  usart_transmit(' ');
  usart_transmit_str(gbRomData.CGBsupportText);
  usart_newline();
  usart_transmit_str_P(PSTR("\r\nMBC mode on boot (From EEPROM)\r\n"));
  usart_transmit_str_P(PSTR("MBC mode:         "));
  usart_transmit(nibble_to_hex(eepData.MBCMode >> 4));
  usart_transmit(nibble_to_hex(eepData.MBCMode));
  usart_transmit(' ');
  usart_transmit_str(eepData.MBCName);
  usart_newline();
  usart_transmit_str_P(PSTR("\r\nFirmware rev.:    1.0 26 Jan 19\r\n\n"));

  wait_for_continue();
  return 0;
}

uint8_t receive_ROM() {

  usart_transmit_str_P(PSTR("Confirm you wish to erase and flash a new ROM (Y/N)\r\n"));

  if(wait_for_confirm_cancel()) {
    usart_transmit_str_P(commonAbort);
    wait_for_continue();
    return 1;
  }

  usart_transmit_str_P(PSTR("Erasing chip(s)\r\n"));
  sst_flash_chip_erase(0);
  sst_flash_chip_erase(1);
  sst_flash_chip_erase(2);
  sst_flash_chip_erase(3);
  
  usart_transmit_str_P(PSTR("10 second delay. Quick, select the file to download via YMODEM protocol\r\n"));

  for(uint8_t i=0; i<10; i++) {
    usart_transmit(i+'0');
    _delay_ms(1000);
  }
  usart_newline();

  flashAddress.address32 = 0; // = 0 normally, change this to debug storing at a high address
  uint8_t retVal = ymodem_receive(flash_buff_receive, &fileSize);
  
  _delay_ms(250);

  if (retVal) {
    for(uint8_t i=0; i<10; i++) {
      usart_transmit(i+'0');
      _delay_ms(1000);
    }
    usart_transmit_str_P(PSTR("\r\nError: "));
    usart_transmit_int32(retVal);
  } 

  usart_transmit_str_P(PSTR("\r\nFile received! Size: "));
  usart_transmit_int32(fileSize);
  usart_transmit_str_P(PSTR("\r\nMax address        : "));
  usart_transmit_int32(flashAddress.address32);
  usart_newline();
  

  // Must update the EEPROM information too!
  get_GBROM_info(&gbRomData);
  
  // Set the new mode 
  if(gbRomData.MBCtype < 0x04) {
    // MBC1 or ROM only
    
    if(gbRomData.RAMsizeBytes > 8192 && gbRomData.ROMsizeBytes <= 524288) {
      eepData.MBCMode = CART_MODE_MBC1_4M;
      strcpy_P(eepData.MBCName, PSTR("MBC1_4M "));
    } else if(gbRomData.RAMsizeBytes > 8192 && gbRomData.ROMsizeBytes > 524288) {
      eepData.MBCMode = CART_MODE_MBC3;
      strcpy_P(eepData.MBCName, PSTR("MBC3    "));
      usart_transmit_str_P(PSTR("WARNING: ROM and RAM size both exceed MBC1 mode sizes. Assuming MBC3 mode\r\n"));
    } else {
      eepData.MBCMode = CART_MODE_MBC1_16M;
      strcpy_P(eepData.MBCName, PSTR("MBC1_16M"));
    }
  
  } else if(gbRomData.MBCtype == 0x05 || gbRomData.MBCtype == 0x06) {
    // MBC2
    eepData.MBCMode = CART_MODE_MBC2;
    strcpy_P(eepData.MBCName, PSTR("MBC2    "));

  } else if(gbRomData.MBCtype >= 0x0F && gbRomData.MBCtype <= 0x13) {
    // MBC3
    eepData.MBCMode = CART_MODE_MBC3;
    strcpy_P(eepData.MBCName, PSTR("MBC3    "));

  } else if(gbRomData.MBCtype >= 0x19) {
    // MBC5
    eepData.MBCMode = CART_MODE_MBC5;
    strcpy_P(eepData.MBCName, PSTR("MBC5    "));
  } else {
    // default to MBC116M8k unless someone else says...
    eepData.MBCMode = CART_MODE_MBC1_16M;
    strcpy_P(eepData.MBCName, PSTR("MBC1_16M"));
  }

  // EEPROM WRITE CYCLE GOES HERE IF DATA IS DIFFERENT!
  write_update_gbeepromdat(&eepData);

  wait_for_continue();
  return 0;
}

uint8_t dump_ROM() {

  usart_transmit_str_P(PSTR("Confirm you wish to dump the current ROM (Y/N)\r\n"));

  if(wait_for_confirm_cancel()) {
    // Abort
    usart_transmit_str_P(commonAbort);
    wait_for_continue();
    return 1;
  }

  usart_transmit_str_P(PSTR("Select YMODEM receive on your terminal to download. AVR will be waiting\r\n"));

  flashAddress.address32 = 0; // Reset the address again

  // Two different cases here for naming convention at the other end!
  // Might change this later to take the game name from the ROM and send that through
  if(gbRomData.CGBsupport != GB_PRE_CGB) {
    ymodem_send(flash_buff_send, "GBDump.gbc", gbRomData.ROMsizeBytes);
  } else {
    ymodem_send(flash_buff_send, "GBDump.gb", gbRomData.ROMsizeBytes);
  }
  
  usart_transmit_str_P(PSTR("File sent!\r\n"));

  wait_for_continue();
  return 0;
}

uint8_t receive_save() {

  usart_transmit_str_P(PSTR("Confirm you wish to download a save file (Y/N)\r\n"));

  if(wait_for_confirm_cancel()) {
    usart_transmit_str_P(commonAbort);
    wait_for_continue();
    return 1;
  }
  
  usart_transmit_str_P(PSTR("10 second delay. Quick, select the file to download via YMODEM protocol\r\n"));

  for(uint8_t i=0; i<10; i++) {
    usart_transmit(i+'0');
    _delay_ms(1000);
  }
  usart_newline();
  
  RAMaddress = 0;
  RAMWriteErrors = 0;
  ymodem_receive(sram_buff_receive, &fileSize);

  usart_transmit_str_P(PSTR("\r\nFile received! Size: "));
  usart_transmit_int32(fileSize);
  usart_newline();

  if(RAMWriteErrors) {
    usart_transmit_str_P(PSTR("RAM write errors: "));
    usart_transmit_int32(RAMWriteErrors);
    usart_newline();
    usart_transmit_str_P(PSTR("First Addr: "));
    usart_transmit_int32(RAMWriteErrorsAddrFirst);
    usart_newline();
    usart_transmit_str_P(PSTR("Last Addr: "));
    usart_transmit_int32(RAMWriteErrorsAddrLast);
    usart_newline();
  }

  wait_for_continue();
  return 0;
}

uint8_t dump_save() {

  usart_transmit_str_P(PSTR("Confirm you wish to dump the current save file (Y/N)\r\n"));

  if(wait_for_confirm_cancel()) {
    // Abort
    usart_transmit_str_P(commonAbort);
    wait_for_continue();
    return 1;
  }

  usart_transmit_str_P(PSTR("Select YMODEM receive on your terminal to download. AVR will be waiting\r\n"));

  RAMaddress = 0;
  ymodem_send(sram_buff_send, "GBDump.sav", gbRomData.RAMsizeBytes);

  usart_transmit_str_P(PSTR("File sent!\r\n"));

  wait_for_continue();
  return 0;
}

uint8_t flash_diagnostic() {

  uint8_t flashID = sst_read_softwareID(0);

  usart_transmit_str_P(PSTR("Flash chip ID (expecting 0xB7)\r\n0: 0x"));
  usart_transmit(nibble_to_hex(flashID >> 4));
  usart_transmit(nibble_to_hex(flashID));
  usart_newline();
  flashID = sst_read_softwareID(1);
  usart_transmit_str_P(PSTR("1: 0x"));
  usart_transmit(nibble_to_hex(flashID >> 4));
  usart_transmit(nibble_to_hex(flashID));
  usart_newline();
  flashID = sst_read_softwareID(2);
  usart_transmit_str_P(PSTR("2: 0x"));
  usart_transmit(nibble_to_hex(flashID >> 4));
  usart_transmit(nibble_to_hex(flashID));
  usart_newline();
  flashID = sst_read_softwareID(3);
  usart_transmit_str_P(PSTR("3: 0x"));
  usart_transmit(nibble_to_hex(flashID >> 4));
  usart_transmit(nibble_to_hex(flashID));
  usart_newline();

  wait_for_continue();
  return 0;
}

uint8_t flash_new_firmware() { 

  // Run the bootloader
  asm("jmp 0xE000");

  wait_for_continue();
  return 0;
}

uint8_t force_mbc_mode() {

  // Show the user the menu options
  usart_transmit_str_P(PSTR("Select one of the MBC modes below:\r\n"));
  usart_transmit_str_P(PSTR("0 - MBC1 16Mb\r\n"));
  usart_transmit_str_P(PSTR("1 - MBC1 4Mb\r\n"));
  usart_transmit_str_P(PSTR("2 - MBC2\r\n"));
  usart_transmit_str_P(PSTR("3 - MBC3\r\n"));
  usart_transmit_str_P(PSTR("4 - MBC5\r\n"));
  usart_transmit_str_P(PSTR("c - Cancel selection\r\n"));

  uint8_t c;

  do {
    c = usart_receive();

    if((c >= '0' && c <= '4') || (c == 'c')) {
      break;
    }

    usart_transmit_str_P(PSTR("Unknown selection:"));
    usart_transmit(c);
    usart_newline();
  } while(1);

  // Set the new mode 
  if(c == '0') {
    // MBC1 or ROM only
    eepData.MBCMode = CART_MODE_MBC1_16M;
    strcpy_P(eepData.MBCName, PSTR("MBC1_16M"));
  } else if (c == '1') {
    eepData.MBCMode = CART_MODE_MBC1_4M;
    strcpy_P(eepData.MBCName, PSTR("MBC1_4M "));
  } else if (c == '2') {
    eepData.MBCMode = CART_MODE_MBC2;
    strcpy_P(eepData.MBCName, PSTR("MBC2    "));
  } else if (c == '3') {
    eepData.MBCMode = CART_MODE_MBC3;
    strcpy_P(eepData.MBCName, PSTR("MBC3    "));
  } else if (c == '4') {
    eepData.MBCMode = CART_MODE_MBC5;
    strcpy_P(eepData.MBCName, PSTR("MBC5    "));
  } else if (c == 'c') {
    usart_transmit_str_P(PSTR("Cancel selection.\r\n"));
  }

  // EEPROM WRITE CYCLE GOES HERE IF DATA IS DIFFERENT!
  write_update_gbeepromdat(&eepData);

  usart_transmit_str_P(PSTR("MBC mode is now: "));
  usart_transmit_str(eepData.MBCName);
  usart_newline();

  wait_for_continue();
  return 0;
}

void enter_GB_mode() {

  usart_transmit_str_P(PSTR("Confirm you wish to disable communication and enter MBC mode (Y/N)\r\n"));

  if(wait_for_confirm_cancel()) {
    // Abort
    usart_transmit_str_P(commonAbort);
    wait_for_continue();
    return;
  }

  usart_transmit_str_P(PSTR("Bye!\r\n"));

  _delay_ms(250);

  launch_mbc_mode();
}


uint8_t wait_for_confirm_cancel() {
  
  char input; 
  do {
    input = usart_receive();
  } while((input != 'Y') && (input != 'N') && (input != 'y') && (input != 'n'));

  if((input == 'N') || (input == 'n')) {
    return 1;
  }

  return 0;
}

void wait_for_continue() {

  usart_transmit_str_P(commonReturn);

  char input; 
  do {
    input = usart_receive();
  } while(input != ' ');
}

void launch_mbc_mode(void) {
  
  // Init ports first
  // Should be common inputs and outputs for all MBC modes, even if functions are different

  DDRA = 0xFB; // All outputs except PA5, which is now GB_CS, maybe I need to change RUMBLE (PA2) to input for ADDR12 too
  DDRB = 0;  //DDRB = (1 << PB5) | (1 << PB6) | (1 << PB7); // Port B is data from the GB, note for MBC1, PB5-7 are not connected. Set them as Lo outputs
  DDRC = 0x7F; // MA output lines, with exception of PC7, which is 
  DDRD = 0;

  PORTA = (1 << PA5) | (1 << PA4) | (1 << PA7); // Set (PA5) OE pins and (PA7) INTRAMEN hi, PA4 is Flash WE
  PORTB = 0;
  PORTC = 0; // no pullups needed on these ports
  PORTD = (1 << PD0) | (1 << PD1); // Set UART pins hi

  

  // Hmmm. Will need to read EEPROM first to decide which is the correct mode...
  uint8_t mode = read_MBC_mode_only();

  if(mode == CART_MODE_MBC1_16M) {
    PRR = 0xFF;// Turn every peripheral off. We don't need them in this mode. (this includes the UART now!)
    MBC_mode_MBC1_16M8k();

  } else if (mode == CART_MODE_MBC1_4M) {
    PRR = 0xFF;// Turn every peripheral off. We don't need them in this mode. (this includes the UART now!)
    MBC_mode_MBC1_4M32k();

  } else if (mode == CART_MODE_MBC2) {
    PRR = 0xFF; // Turn every peripheral off. We don't need them in this mode. (this includes the UART now!)
    MBC_mode_MBC2();

  } else if (mode == CART_MODE_MBC3) {
    PRR = 0xF7;// Turn off everything except timer1! We want a pseudo RTC!
    MBC_mode_MBC3();

  } else if (mode == CART_MODE_MBC5) {
    PRR = 0xFF; // Turn every peripheral off. We don't need them in this mode. (this includes the UART now!)
    MBC_mode_MBC5();

  } else {
    PRR = 0xFF;// Turn every peripheral off. We don't need them in this mode. (this includes the UART now!)
    MBC_mode_MBC1_16M8k();
  }
}

// ******************************* Buffer functions *****************************************************************

void flash_buff_receive(unsigned char* buffer, uint16_t byteCnt)
{
  for(uint16_t i = 0; i<byteCnt; i++, flashAddress.address32++) {
    sst_flash_write_byte(flashAddress, buffer[i]);
    
  }
}

uint8_t flash_buff_send(unsigned char* buffer, uint16_t byteCnt)
{
  for(uint16_t i = 0; i<byteCnt; i++, flashAddress.address32++) {
     sst_flash_read_byte(flashAddress, &(buffer[i]));
  }

  return 0;
}

void sram_buff_receive(unsigned char* buffer, uint16_t byteCnt) {
  for(uint16_t i = 0; i<byteCnt; i++, RAMaddress++) {
    sram_write_byte(RAMaddress, buffer[i]);

/*    uint8_t c;
    if(sram_read_byte(RAMaddress, &c) != buffer[i]) {
      // Error in write and readback. 
      RAMWriteErrors++;

      if(RAMWriteErrors == 1) {
        RAMWriteErrorsAddrFirst = RAMaddress;
      }
      RAMWriteErrorsAddrLast = RAMaddress;
    }*/
  }

}

uint8_t sram_buff_send(unsigned char* buffer, uint16_t byteCnt) {
   for(uint16_t i = 0; i<byteCnt; i++, RAMaddress++) {
     sram_read_byte(RAMaddress, &(buffer[i]));
  }
  return 0;
}
