#include "memory_latched.h"
#include <util/delay.h>
#include <avr/pgmspace.h>

#include "../01_INCLUDE/usart_funcs.h"
#define DEBUG_LED_STOP  PORTA|=(1<<PA6);char input; do { input = usart_receive();} while(input != ' '); PORTA&=~(1<<PA6);

/*   
  The flash chip in use on the prototype board is the SST39SF040. 512k x 8

  The flash chip in use of the MBC shrink board is the Macronix MX29F800C 1MBx8 (or 512k x 16)
*/

// This version of software uses latches from the data lines for address lines to free up IO. It'll be clunkier than before

#define _NOP() do { __asm__ __volatile__ ("nop"); } while (0) // used for a very short delay

#define MAC_CS              PD7
#define MAC_CS_PORT         PORTD
#define MAC_CS_DDR          DDRD
#define MAC_OE              PD4
#define MAC_OE_PORT         PORTD
#define MAC_OE_DDR          DDRD
#define MAC_WE              PA4
#define MAC_WE_PORT         PORTA
#define MAC_WE_DDR          DDRA

// Changed this name to 'memory', because it's used for both SRAM and flash
#define MEMORY_DATA_PORT    PORTC
#define MEMORY_DATA_DDR     DDRC
#define MEMORY_DATA_PIN     PINC

#define LATCH_OE            PA5   // Dedicated pin
#define LATCH_OE_PORT       PORTA
#define LATCH_OE_DDR        DDRA
#define LATCH_CLK_LO8       PD2   // Shared with GB_CLK
#define LATCH_CLK_LO8_PORT  PORTD
#define LATCH_CLK_LO8_DDR   DDRD
#define LATCH_CLK_MID6      PA2   // Shared with GB_CS
#define LATCH_CLK_MID6_PORT PORTA
#define LATCH_CLK_MID6_DDR  DDRA

#define FLASH_HI5_MASK      0x7F // MA address lines. pin 7 is used for A8 input, so don't mask it
#define FLASH_HI5_PORT      PORTB
#define FLASH_HI5_DDR       DDRB

#define FLASH_MUX           PD6 // GB_A14
#define FLASH_MUX_PORT      PORTD
#define FLASH_MUX_DDR       DDRD    

#define SRAM_ADDR13_14_MASK 0x03 // PA0 and PA1 
#define SRAM_ADDR13_14_PORT PORTA
#define SRAM_ADDR13_14_DDR  DDRA
#define SRAM_CS             PA7
#define SRAM_CS_PORT        PORTA
#define SRAM_CS_DDR         DDRA
#define SRAM_WE             PD3
#define SRAM_WE_PORT        PORTD
#define SRAM_WE_DDR         DDRD
#define SRAM_OE             PD4
#define SRAM_OE_PORT        PORTD
#define SRAM_OE_DDR         DDRD

#define MAC_CMDMODE_BYTE        0xA0
#define MAC_CMDMODE_ID_READ     0x90
#define MAC_CMDMODE_MODE_RESET  0xF0
#define MAC_CMDMODE_CHIP1       0x80 
#define MAC_CMDMODE_CHIP2       0x10 

static void mac_flash_command_sequence(uint8_t chipNum, uint8_t mode);
uint8_t mac_flash_write_sub_byte(FLASHADDR address, uint8_t writeData);

// Global variables
FLASHADDR flashAddress;

void memory_init(void) {

  // Set up all of the pins
  MEMORY_DATA_DDR = 0xFF; // Data - don't assert the bus in case we're wrong about the mode
  FLASH_HI5_DDR = FLASH_HI5_MASK; // MA address lines. 

  MAC_CS_DDR |= (1 << MAC_CS);
  MAC_OE_DDR |= (1 << MAC_OE);
  MAC_WE_DDR |= (1 << MAC_WE);

  LATCH_OE_DDR |= (1 << LATCH_OE);
  LATCH_CLK_LO8_DDR |= (1 << LATCH_CLK_LO8); 
  LATCH_CLK_MID6_DDR |= (1 << LATCH_CLK_MID6);

  SRAM_ADDR13_14_DDR |= SRAM_ADDR13_14_MASK;
  SRAM_CS_DDR |= (1 << SRAM_CS);
  SRAM_WE_DDR |= (1 << SRAM_WE);
  SRAM_OE_DDR |= (1 << SRAM_OE);

  // Pull up inverted control pins
  MAC_CS_PORT |= (1 << MAC_CS);
  MAC_OE_PORT |= (1 << MAC_OE);
  MAC_WE_PORT |= (1 << MAC_WE);
  LATCH_OE_PORT |= (1 << LATCH_OE);
  SRAM_CS_PORT |= (1 << SRAM_CS);
  SRAM_WE_PORT |= (1 << SRAM_WE);
  SRAM_OE_PORT |= (1 << SRAM_OE);

  FLASH_MUX_DDR |= (1 << FLASH_MUX);
  FLASH_MUX_PORT |= (1 << FLASH_MUX);
}

uint8_t mac_flash_read_byte(FLASHADDR address, uint8_t *returnData) {

  // Set the address to read from
  // Remember you need to latch the address in!
  MEMORY_DATA_PORT = address.address8[0];
  _NOP();
  LATCH_CLK_LO8_PORT |= (1 << LATCH_CLK_LO8); // Rising edge triggered
  _NOP();
  LATCH_CLK_LO8_PORT &= ~(1 << LATCH_CLK_LO8);
  _NOP();

  MEMORY_DATA_PORT = address.address8[1];
  _NOP();
  LATCH_CLK_MID6_PORT |= (1 << LATCH_CLK_MID6);
  _NOP();
  LATCH_CLK_MID6_PORT &= ~(1 << LATCH_CLK_MID6);
  _NOP();

  // Address for ADDR14-21
  FLASH_HI5_PORT = address.address32 >> 14;//(address.address8[1] >> 6) | (address.address8[2] << 2); //address.address32 >> 14;
  _NOP();

  // Set read mode by pulling OE and CS low, after making data pins an input!
  MEMORY_DATA_DDR = 0x00; // Make data lines an input
  LATCH_OE_PORT &= ~(1 << LATCH_OE);
  _NOP();
  MAC_CS_PORT &= ~(1 << MAC_CS);
  MAC_OE_PORT &= ~(1 << MAC_OE);

  // 2 x NOP required to meet the 70ns data retrieval period
  _NOP();
  _NOP();
  _NOP();
  _NOP();

  // Read the data in!
  *returnData = MEMORY_DATA_PIN;

  // Pull OE and CS up again, and make PORTB outputs again
  MAC_OE_PORT |= (1 << MAC_OE);
  MAC_CS_PORT |= (1 << MAC_CS);
  LATCH_OE_PORT |= (1 << LATCH_OE);
  MEMORY_DATA_DDR = 0xFF;

  return 0;
}

uint8_t mac_flash_write_byte(FLASHADDR address, uint8_t writeData) {

  // Select the correct chip  & put the chip into write mode. We'll use the /WE control method
//  FLASH_HI5_PORT = address.address32 >> 14;//(address.address8[1] >> 6) | (address.address8[2] << 2); //address.address32 >> 14;
//  _NOP();
  MAC_CS_PORT &= ~(1 << MAC_CS);

  mac_flash_command_sequence((address.address8[2] >> 4), MAC_CMDMODE_BYTE);

  // Now finally! The data!
  mac_flash_write_sub_byte(address, writeData);

  // Move to #DATA polling by pulling CS high again
  MAC_CS_PORT |= (1 << MAC_CS);

  // #DATA polling, as per P37 of datasheet
  do {
    uint8_t i;
    mac_flash_read_byte(address, &i);

    if((i & 0x80) != ((~writeData) & 0x80)) {
      break;
    }

    if((i & 0x20) != 0x20) {
      continue;
    }

    mac_flash_read_byte(address, &i);

    if((i & 0x80) != ((~writeData) & 0x80)) {
      break;
    } else {
/*      usart_transmit_str("Wr Addr Fail:");
      usart_transmit_int32(address.address32);
      usart_transmit_str("\r\nData:");
      usart_transmit_int32(writeData);
      usart_transmit_str("\r\n");*/
      break;
    }

  } while(1);

/* Bit toggling below - seems to be slow with the macronix chip. Commenting out while doing some tests
  // Wait until it's applied...
  // Data polling didn't seem to work, bit toggling bit looks okay...
  uint8_t i,j;
  mac_flash_read_byte(address, &i);
  i &= 0x40;
  do {
    j = i;
    mac_flash_read_byte(address, &i);
    i &= 0x40;
  } while(i != j);

  // Do this twice in case the last time was a fluke...
  mac_flash_read_byte(address, &i);
  i &= 0x40;
  do {
    j = i;
    mac_flash_read_byte(address, &i);
    i &= 0x40;
  } while(i != j);*/

  return 0;
}

static void mac_flash_command_sequence(uint8_t chipNum, uint8_t mode) {

  FLASHADDR address;
  // First 0xAAA, 0xAA
  address.address32 = 0xAAA;
  address.address8[2] |= (chipNum << 4);
  mac_flash_write_sub_byte(address, 0xAA);

  // Now 0x555,0x55
  address.address32 = 0x555;
  address.address8[2] |= (chipNum << 4);
  mac_flash_write_sub_byte(address, 0x55);

  // Now 0xAAA again
  address.address32 = 0xAAA;
  address.address8[2] |= (chipNum << 4);
  mac_flash_write_sub_byte(address, mode);
}

uint8_t mac_flash_write_sub_byte(FLASHADDR address, uint8_t writeData) {

  // Set the address to read from
  // Remember you need to latch the address in!
  MEMORY_DATA_PORT = address.address8[0];
  _NOP();
  LATCH_CLK_LO8_PORT |= (1 << LATCH_CLK_LO8); // Rising edge triggered
  _NOP();
  LATCH_CLK_LO8_PORT &= ~(1 << LATCH_CLK_LO8);
  _NOP();

  MEMORY_DATA_PORT = address.address8[1];
  _NOP();
  LATCH_CLK_MID6_PORT |= (1 << LATCH_CLK_MID6);
  _NOP();
  LATCH_CLK_MID6_PORT &= ~(1 << LATCH_CLK_MID6);
  _NOP();

// Address for ADDR14-21
  FLASH_HI5_PORT = address.address32 >> 14;//(address.address8[1] >> 6) | (address.address8[2] << 2); //address.address32 >> 14;
  _NOP();
  _NOP();

  // Latch the address onto the chip by pulling /WE low
  LATCH_OE_PORT &= ~(1 << LATCH_OE);
  _NOP();
  _NOP();

  // Set the data byte for the rising edge
  MEMORY_DATA_PORT = writeData;
  _NOP();
  _NOP();
  _NOP();
  
  MAC_WE_PORT &= ~(1 << MAC_WE);
  _NOP();
  _NOP();
  _NOP();

  MAC_WE_PORT |= (1 << MAC_WE);
  _NOP();
  _NOP();
  _NOP();

  LATCH_OE_PORT |= (1 << LATCH_OE);
  _NOP();
  _NOP();
  _NOP();

  return 0;
}

uint8_t mac_flash_chip_erase(uint8_t chipNum) {

  // Select the correct chip  & put the chip into write mode. We'll use the /WE control method
  MAC_CS_PORT &= ~(1 << MAC_CS);

  mac_flash_command_sequence(chipNum, MAC_CMDMODE_CHIP1);
  mac_flash_command_sequence(chipNum, MAC_CMDMODE_CHIP2);

  // Exit chip erase mode.
  MAC_CS_PORT |= (1 << MAC_CS);

  // Put a bit toggle check here to look for the end of the erase cycle instead of the current delay
  uint8_t i,j;
  FLASHADDR address;
  address.address8[2] = chipNum << 4;
  
  mac_flash_read_byte(address, &i);
  i &= 0x40;
  do {
    j = i;
    mac_flash_read_byte(address, &i);
    i &= 0x40;
  } while(i != j);

  // Do this twice in case the last time was a fluke...
  mac_flash_read_byte(address, &i);
  i &= 0x40;
  do {
    j = i;
    mac_flash_read_byte(address, &i);
    i &= 0x40;
  } while(i != j);
//  _delay_ms(32000); // Max wait period for a whole chip erase

  return 0;
}

/*
  Returns the hex byte ID of the SST chip
*/
uint8_t mac_read_softwareID(uint8_t chipNum) {

  uint8_t chipID;
  FLASHADDR address;
  address.address32 = 2;
  address.address8[2] = chipNum << 4;

  // Put the chip into ID mode. We'll use the /WE control method

  MAC_CS_PORT &= ~(1 << MAC_CS);
  mac_flash_command_sequence(chipNum, MAC_CMDMODE_ID_READ);
  MAC_CS_PORT |= (1 << MAC_CS);

  mac_flash_read_byte(address, &chipID);

  // Exit ID mode. We'll use the /WE control method

  MAC_CS_PORT &= ~(1 << MAC_CS);
  //mac_flash_reset_mode(chipID);
  //mac_flash_command_sequence(chipNum, MAC_CMDMODE_MODE_RESET);
  mac_flash_write_sub_byte(address, MAC_CMDMODE_MODE_RESET);
  MAC_CS_PORT |= (1 << MAC_CS);

  return chipID;
}

// *********************************** SRAM funcs ************************************************

uint8_t sram_read_byte(uint16_t address, uint8_t *returnData) {

  // Set the address to read from
  // Remember you need to latch the address in!
  MEMORY_DATA_PORT = (uint8_t)(address);
  _NOP();
  LATCH_CLK_LO8_PORT |= (1 << LATCH_CLK_LO8); // Rising edge triggered
  _NOP();
  LATCH_CLK_LO8_PORT &= ~(1 << LATCH_CLK_LO8);

  MEMORY_DATA_PORT = (uint8_t)(address >> 8);
  _NOP();
  LATCH_CLK_MID6_PORT |= (1 << LATCH_CLK_MID6);
  _NOP();
  LATCH_CLK_MID6_PORT &= ~(1 << LATCH_CLK_MID6);

  SRAM_ADDR13_14_PORT = (SRAM_ADDR13_14_PORT & (~SRAM_ADDR13_14_MASK)) | ((uint8_t)(address >> 13) & SRAM_ADDR13_14_MASK);

  // Set read mode by pulling OE and CS low, after making data pins an input!
  MEMORY_DATA_DDR = 0x00; // Make data lines an input
  LATCH_OE_PORT &= ~(1 << LATCH_OE);

  SRAM_CS_PORT &= ~(1 << SRAM_CS);
  SRAM_OE_PORT &= ~(1 << SRAM_OE);

  _NOP();
  _NOP();

  // 2 x NOP required to meet the 55ns data retrieval period
  _NOP();
  _NOP();

  // Read the data in!
  *returnData = MEMORY_DATA_PIN;

  // Pull OE and CS up again, and make PORTB outputs again
  SRAM_OE_PORT |= (1 << SRAM_OE);
  SRAM_CS_PORT |= (1 << SRAM_CS);
  LATCH_OE_PORT |= (1 << LATCH_OE);
  MEMORY_DATA_DDR = 0xFF;

  return 0;
}

uint8_t sram_write_byte(uint16_t address, uint8_t writeData) {

  // Set the address to read from
  // Remember you need to latch the address in!
  MEMORY_DATA_PORT = (uint8_t)(address);
  _NOP();
  LATCH_CLK_LO8_PORT |= (1 << LATCH_CLK_LO8); // Rising edge triggered
  _NOP();
  LATCH_CLK_LO8_PORT &= ~(1 << LATCH_CLK_LO8);

  MEMORY_DATA_PORT = (uint8_t)(address >> 8);
  _NOP();
  LATCH_CLK_MID6_PORT |= (1 << LATCH_CLK_MID6);
  _NOP();
  LATCH_CLK_MID6_PORT &= ~(1 << LATCH_CLK_MID6);

  SRAM_ADDR13_14_PORT = (SRAM_ADDR13_14_PORT & (~SRAM_ADDR13_14_MASK)) | ((uint8_t)(address >> 13) & SRAM_ADDR13_14_MASK);

  MEMORY_DATA_PORT = writeData;

  // Set read mode by pulling OE and CS low, after making data pins an input!
  LATCH_OE_PORT &= ~(1 << LATCH_OE);
_NOP(); // Test NOPS to find out why writing isn't working
_NOP();
SRAM_OE_PORT |= (1 << SRAM_OE);
_NOP();
_NOP();
  SRAM_CS_PORT &= ~(1 << SRAM_CS);
_NOP(); // Test NOPS to find out why writing isn't working
_NOP();
  SRAM_WE_PORT &= ~(1 << SRAM_WE);

  _NOP();
  _NOP();

  // 2 x NOP required to meet the 55ns data write period
  _NOP();
  _NOP();
_NOP(); // Test NOPS to find out why writing isn't working
_NOP();
  // Pull OE and CS up again, and make PORTB outputs again
  SRAM_WE_PORT |= (1 << SRAM_WE);
  SRAM_CS_PORT |= (1 << SRAM_CS);
_NOP(); // Test NOPS to find out why writing isn't working
_NOP();
  LATCH_OE_PORT |= (1 << LATCH_OE);

  return 0;
}
