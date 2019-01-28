#include "memory_latched.h"
#include <util/delay.h>
#include <avr/pgmspace.h>

/*   The Flash chip in use is the SST39SF040. 512k x 8
      19 address bits (A0-A18) (PORTC lo8, PORTA mid8, PORTD2-4 hi3)
      8  data bits (D0-D7)     (PORTB)
      1  /Chip enable     CE   (PD7)
      1  /Output enable   OE   (PD6)
      1  /Write enable    WE   (PD5)
*/

// This version of software uses latches from the data lines for address lines to free up IO. It'll be clunkier than before

#define _NOP() do { __asm__ __volatile__ ("nop"); } while (0) // used for a very short delay

#define SST_CS              PD7
#define SST_CS_PORT         PORTD
#define SST_CS_DDR          DDRD
#define SST_OE              PD4
#define SST_OE_PORT         PORTD
#define SST_OE_DDR          DDRD
#define SST_WE              PA4
#define SST_WE_PORT         PORTA
#define SST_WE_DDR          DDRA

// Changed this name to 'memory', because it's used for both SRAM and flash
#define MEMORY_DATA_PORT    PORTB
#define MEMORY_DATA_DDR     DDRB
#define MEMORY_DATA_PIN     PINB

#define LATCH_OE            PA5   // Dedicated pin
#define LATCH_OE_PORT       PORTA
#define LATCH_OE_DDR        DDRA
#define LATCH_CLK_LO8       PD2   // Shared with GB_CLK
#define LATCH_CLK_LO8_PORT  PORTD
#define LATCH_CLK_LO8_DDR   DDRD
#define LATCH_CLK_MID6      PA2   // Shared with GB_CS
#define LATCH_CLK_MID6_PORT PORTA
#define LATCH_CLK_MID6_DDR  DDRA

#define SST_HI5_MASK        0x7F // MA address lines. pin 7 is used for A8 input, so don't mask it
#define SST_HI5_PORT        PORTC
#define SST_HI5_DDR         DDRC

#define SST_CMDMODE_BYTE    0xA0
#define SST_CMDMODE_SECTOR1 0x80 //not implemented yet
#define SST_CMDMODE_SECTOR2 0x30 //not implemented yet
#define SST_CMDMODE_CHIP1   0x80 
#define SST_CMDMODE_CHIP2   0x10 
#define SST_CMDMODE_ID_READ 0x90
#define SST_CMDMODE_ID_EXIT 0xF0

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

#define SST_MUX             PD6
#define SST_MUX_PORT        PORTD
#define SST_MUX_DDR         DDRD      

static void sst_flash_command_sequence(uint8_t chipNum, uint8_t mode);
uint8_t sst_flash_write_sub_byte(FLASHADDR address, uint8_t writeData);

// Global variables
FLASHADDR flashAddress;

void memory_init(void) {

  // Set up all of the pins
  MEMORY_DATA_DDR = 0xFF; // Data - don't assert the bus in case we're wrong about the mode
  SST_HI5_DDR = SST_HI5_MASK; // MA address lines. 

  SST_CS_DDR |= (1 << SST_CS);
  SST_OE_DDR |= (1 << SST_OE);
  SST_WE_DDR |= (1 << SST_WE);

  LATCH_OE_DDR |= (1 << LATCH_OE);
  LATCH_CLK_LO8_DDR |= (1 << LATCH_CLK_LO8); 
  LATCH_CLK_MID6_DDR |= (1 << LATCH_CLK_MID6);

  SRAM_ADDR13_14_DDR |= SRAM_ADDR13_14_MASK;
  SRAM_CS_DDR |= (1 << SRAM_CS);
  SRAM_WE_DDR |= (1 << SRAM_WE);
  SRAM_OE_DDR |= (1 << SRAM_OE);

  // Pull up inverted control pins
  SST_CS_PORT |= (1 << SST_CS);
  SST_OE_PORT |= (1 << SST_OE);
  SST_WE_PORT |= (1 << SST_WE);
  LATCH_OE_PORT |= (1 << LATCH_OE);
  SRAM_CS_PORT |= (1 << SRAM_CS);
  SRAM_WE_PORT |= (1 << SRAM_WE);
  SRAM_OE_PORT |= (1 << SRAM_OE);

  SST_MUX_DDR |= (1 << SST_MUX);
  SST_MUX_PORT |= (1 << SST_MUX);
}

uint8_t sst_flash_read_byte(FLASHADDR address, uint8_t *returnData) {

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
  SST_HI5_PORT = address.address32 >> 14;//(address.address8[1] >> 6) | (address.address8[2] << 2); //address.address32 >> 14;
  _NOP();

  // Set read mode by pulling OE and CS low, after making data pins an input!
  MEMORY_DATA_DDR = 0x00; // Make data lines an input
  LATCH_OE_PORT &= ~(1 << LATCH_OE);
  _NOP();
  SST_CS_PORT &= ~(1 << SST_CS);
  SST_OE_PORT &= ~(1 << SST_OE);

  // 2 x NOP required to meet the 70ns data retrieval period
  _NOP();
  _NOP();
  _NOP();
  _NOP();

  // Read the data in!
  *returnData = MEMORY_DATA_PIN;

  // Pull OE and CS up again, and make PORTB outputs again
  SST_OE_PORT |= (1 << SST_OE);
  SST_CS_PORT |= (1 << SST_CS);
  LATCH_OE_PORT |= (1 << LATCH_OE);
  MEMORY_DATA_DDR = 0xFF;

  return 0;
}

uint8_t sst_flash_write_byte(FLASHADDR address, uint8_t writeData) {

  // Select the correct chip  & put the chip into write mode. We'll use the /WE control method
//  SST_HI5_PORT = address.address32 >> 14;//(address.address8[1] >> 6) | (address.address8[2] << 2); //address.address32 >> 14;
//  _NOP();
  SST_CS_PORT &= ~(1 << SST_CS);

  sst_flash_command_sequence((address.address8[2] >> 3), SST_CMDMODE_BYTE);

  // Now finally! The data!
  sst_flash_write_sub_byte(address, writeData);

  // Move to #DATA polling by pulling CS high again
  SST_CS_PORT |= (1 << SST_CS);

  // Wait until it's applied...
  // Data polling didn't seem to work, bit toggling bit looks okay...
  uint8_t i,j;
  sst_flash_read_byte(address, &i);
  i &= 0x40;
  do {
    j = i;
    sst_flash_read_byte(address, &i);
    i &= 0x40;
  } while(i != j);

  // Do this twice in case the last time was a fluke...
  sst_flash_read_byte(address, &i);
  i &= 0x40;
  do {
    j = i;
    sst_flash_read_byte(address, &i);
    i &= 0x40;
  } while(i != j);
//  _delay_us(20);

  return 0;
}

static void sst_flash_command_sequence(uint8_t chipNum, uint8_t mode) {

  FLASHADDR address;
  // First 0x5555, 0xAA
  address.address32 = 0x5555;
  address.address8[2] |= (chipNum << 3);
  sst_flash_write_sub_byte(address, 0xAA);

  // Now 0x2AAA,0x55
  address.address32 = 0x2AAAA;
  address.address8[2] |= (chipNum << 3);
  sst_flash_write_sub_byte(address, 0x55);

  // Now 0x5555 again
  address.address32 = 0x5555;
  address.address8[2] |= (chipNum << 3);
  sst_flash_write_sub_byte(address, mode);
}

uint8_t sst_flash_write_sub_byte(FLASHADDR address, uint8_t writeData) {

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
  SST_HI5_PORT = address.address32 >> 14;//(address.address8[1] >> 6) | (address.address8[2] << 2); //address.address32 >> 14;
  _NOP();
  _NOP();

  // Latch the address onto the chip by pulling /WE low
  LATCH_OE_PORT &= ~(1 << LATCH_OE);
  _NOP();
  _NOP();

  // Set the data byte for the rising edge. @20MHz, CLK cycles are slow enough we don't need to delay here
  MEMORY_DATA_PORT = writeData;
  _NOP();
  _NOP();
  _NOP();
  
  SST_WE_PORT &= ~(1 << SST_WE);
  _NOP();
  _NOP();
  _NOP();
  
  SST_WE_PORT |= (1 << SST_WE);
  _NOP();
  _NOP();
  _NOP();

  LATCH_OE_PORT |= (1 << LATCH_OE);
  _NOP();
  _NOP();
  _NOP();

  return 0;
}

uint8_t sst_flash_sector_erase(FLASHADDR addressA12andup) {
  
  return 0;
}

uint8_t sst_flash_chip_erase(uint8_t chipNum) {

  // Select the correct chip  & put the chip into write mode. We'll use the /WE control method
  SST_CS_PORT &= ~(1 << SST_CS);

  sst_flash_command_sequence(chipNum, SST_CMDMODE_CHIP1);
  sst_flash_command_sequence(chipNum, SST_CMDMODE_CHIP2);

  // Exit chip erase mode.
  SST_CS_PORT |= (1 << SST_CS);

  _delay_ms(100); // Wait period for a whole chip erase

  return 0;
}

/*
  Returns the hex byte ID of the SST chip
*/
uint8_t sst_read_softwareID(uint8_t chipNum) {

  uint8_t chipID;
  FLASHADDR address;
  address.address32 = 1;
  address.address8[2] = chipNum << 3;

  // Put the chip into ID mode. We'll use the /WE control method

  SST_CS_PORT &= ~(1 << SST_CS);
  sst_flash_command_sequence(chipNum, SST_CMDMODE_ID_READ);
  SST_CS_PORT |= (1 << SST_CS);

  sst_flash_read_byte(address, &chipID);

  // Exit ID mode. We'll use the /WE control method

  SST_CS_PORT &= ~(1 << SST_CS);
  sst_flash_command_sequence(chipNum, SST_CMDMODE_ID_EXIT);
  SST_CS_PORT |= (1 << SST_CS);

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
