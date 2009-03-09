/*************************************************************//**
* \file sd.c
*
* SD SPI interface
*
* A simple SD interface using SPI. This relies on the associated
* SPI interface, but should be relatively simple to recreate.
*
* This interface is somewhat bloated. It is not intended to be
* fast, but instead helpful for debugging and learning.
*
******************************************************************/

#include <inttypes.h>
#include <avr/sfr_defs.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>

#include "sd.h"
#include "spi.h"
#include "utils.h"


#define   SD_USE_CRC


// Not all defines are used... yet...
#define   CMD_GO_IDLE_STATE     0   // Reset the SD
#define   CMD_SEND_OP_COND      1   // Activate initialization process
#define   CMD_SEND_IF_COND      8   // Send interface operating condition
#define   CMD_SEND_CSD          9   // Send Card specific data
#define   CMD_SEND_CID          10  // Send Card ID
#define   CMD_STOP_TRANSMISSION 12  // Stop read/write transmission
#define   CMD_SEND_STATUS       13  // Request card send status
#define   CMD_SET_BLOCKLEN      16  // Set block length (in bytes)
#define   CMD_READ_BLOCK        17  // Read a block of data
#define   CMD_WRITE_BLOCK       24  // Write a block of data
#define   CMD_ERASE_BLOCK_START 32  // Set first sector for erase
#define   CMD_ERASE_BLOCK_END   33  // Set last sector for erase
#define   CMD_ERASE             38  // Erase all selected sectors
#define   CMD_READ_OCR          58  // Read OCR register
#define   CMD_CRC_ONOFF         59  // 0 to turn off CRC, 1 to turn on CRC

#define   APP_CMD               55  // Signal start of application specific command
#define   SD_SEND_OP_COND       41  // Send operation conditions on host side

#define   R1_IDLE               0x01
#define   R1_ERASE_RESET        0x02
#define   R1_ILLEGAL_CMD        0x04
#define   R1_CRC_ERROR          0x08
#define   R1_ERASE_ERROR        0x10
#define   R1_ADDR_ERROR         0x20
#define   R1_PARAM_ERROR        0x40

#define   NO_RESPONSE           0xFF

#define   BLOCK_SIZE            512
#define   FILL_BYTE             0xFF
#define   START_BLOCK           0xFE

static void      SD_DelayBytes(uint16_t bytes);
static void      SD_SendCommand(uint8_t command, uint32_t arg);
static uint8_t   SD_CheckForCard(void);
static uint8_t   SD_GetResponse(uint8_t *buf, uint16_t length);
static uint8_t   SD_ReadData(uint8_t *buf, uint16_t length);

static void      SD_DecodeR1(uint8_t res);
//static void      SD_DecodeR2(uint16_t res);
static void      SD_DecodeDataRes(uint8_t res);
static void      SD_DecodeDataErr(uint8_t res);

static uint8_t   SD_CRC7 (uint8_t *bytes, uint32_t length);
static uint16_t  SD_CRC16(uint8_t *bytes, uint32_t length);
static uint16_t  SD_CRC16_Fill(uint16_t crc, uint8_t const_value, uint32_t length);


// SD_* is already non-reentrant, so a global buffer is... OK...
static uint8_t scratch[32];


/**
 * Initialize the SD card.
 *
 * Attempts to initialize an SD card on the SPI bus. Will
 * go through the standard initialization procedures, and
 * then print out the CID and CSD in hex.
 *
 * @return 1 if sucessful, 0 otherwise
 */
uint8_t
SD_Init(void)
{
  uint16_t i;
  uint8_t res;

  // Delay a buncha clocks
  SPI_SSOff();
  SD_DelayBytes(20);

  // Check if the card is inserted
  if(!SD_CheckForCard())
  {
    printf("Card NOT found\n");
    return 0;
  }
  printf("Card found\n");

  // Standard init commands
  SD_SendCommand(CMD_SEND_IF_COND, 0);
  res = SD_GetResponse(scratch, 5);

  SD_SendCommand(CMD_READ_OCR, 0);
  res = SD_GetResponse(scratch, 5);

  // Wait for card to init fully
  i=0;
  do {
    SD_SendCommand(CMD_SEND_OP_COND, 0);
    res = SD_GetResponse(scratch, 1);
    i++;
  }while(res && i < 10000);

  if(res) {
    printf("  Error: cmd=%02hX, res=%02hX\n", CMD_SEND_OP_COND, res);
    return 0;
  }

#ifdef SD_USE_CRC
  SD_SendCommand(CMD_CRC_ONOFF, 1);
#else
  SD_SendCommand(CMD_CRC_ONOFF, 0);
#endif
  res = SD_GetResponse(scratch, 1);
  if(res) {
    printf("  Error: cmd=%02hX, res=%02hX\n", CMD_SEND_CID, res);
    SD_DecodeR1(res);
    return 0;
  }


  SD_SendCommand(CMD_SEND_CID, 0);
  // 128bits + 16 CRC
  res = SD_GetResponse(scratch, 1) || !SD_ReadData(scratch, 18);

  if(res) {
    printf("  Error: cmd=%02hX, res=%02hX\n", CMD_SEND_CID, res);
    return 0;
  }else{
    printf("CID: ");
    print_hex(scratch, 16);
    printf("\n");
    //printf(" CRC16: %d\n", SD_CRC16(scratch, 18));
  }


  SD_SendCommand(CMD_SEND_CSD, 0);
  // 128bits + 16 CRC
  res = SD_GetResponse(scratch, 1) || !SD_ReadData(scratch, 18);

  if(res) {
    printf("  Error: cmd=%02hX, res=%02hX\n", CMD_SEND_CID, res);
    return 0;
  }else{
    printf("CSD: ");
    print_hex(scratch, 16);
    printf("\n");
    //printf(" CRC16: %d\n", SD_CRC16(scratch, 18));
  }


  return 1;
}


/**
 * Read a block of data from the SD card.
 *
 * Will read a block of data into the specified
 * buffer.
 *
 * @param buf a character pointer to the destination
 * @param addr a 32bit start address
 * @param size the size of the desired block (16bit)
 *
 * @return 1 if sucessful, 0 otherwise
 */
uint8_t
SD_ReadBlock(uint8_t *buf, uint32_t addr, uint16_t size)
{
  uint8_t res;

  // Set the block length... won't work on SDHC
  SD_SendCommand(CMD_SET_BLOCKLEN, size);
  res = SD_GetResponse(scratch, 1);
  if(res) {
    printf("  Error: cmd=%02hX, res=%02hX\n", CMD_SET_BLOCKLEN, res);
    SD_DecodeR1(res);
    return 0;
  }

  SD_SendCommand(CMD_READ_BLOCK, addr);
  res = SD_GetResponse(scratch, 1);
  if(res) {
    printf("  Error: cmd=%02hX, res=%02hX\n", CMD_READ_BLOCK, res);
    SD_DecodeR1(res);
    return 0;
  }

  return SD_ReadData(buf, size);
}


/**
 * Writes a block of data to the SD card.
 *
 * Writes to the SD card in 512byte block sizes. If the
 * data is not large enough, fills the rest of the block
 * with FILL_BYTE.
 *
 * @param data pointer to the source of data to write
 * @param addr the start address (32bit, must be block aligned)
 * @param size the size of the source data (16bit)
 *
 * @return 1 if sucessful, 0 otherwise
 */
uint8_t
SD_WriteBlock(uint8_t *data, uint32_t addr, uint16_t size)
{
  uint8_t res;
  uint8_t funcRes = 1;
  uint16_t i;
  uint16_t crc;

  crc = SD_CRC16(data, (uint32_t)size);

  // Set the block length... won't work on SDHC
  SD_SendCommand(CMD_SET_BLOCKLEN, BLOCK_SIZE);
  res = SD_GetResponse(scratch, 1);
  if(res) {
    printf("  Error: cmd=%02hX, res=%02hX\n", CMD_SET_BLOCKLEN, res);
    SD_DecodeR1(res);
    return 0;
  }

  // Tell it we want to write
  SD_SendCommand(CMD_WRITE_BLOCK, addr);
  res = SD_GetResponse(scratch, 1);
  if(res) {
    printf("  Error: cmd=%02hX, res=%02hX\n", CMD_WRITE_BLOCK, res);
    SD_DecodeR1(res);
    return 0;
  }

  SPI_SSOn();

  // Send "Start Block" byte
  SPI_TrxByte(START_BLOCK);

  // Send data
  SPI_SendBlock(data, size);

  // Fill empty space
  for(i=size; i<BLOCK_SIZE; i++) {
    SPI_TrxByte(FILL_BYTE);
  }

  // Send CRC (recalc for fill bytes
  crc = SD_CRC16_Fill(crc, FILL_BYTE, BLOCK_SIZE-size);
  SPI_TrxByte((uint8_t)(crc>>8));
  SPI_TrxByte((uint8_t)crc);

  SPI_SSOff();

  // Response?
  res = SD_GetResponse(scratch, 1);
  if((res & 0x0F) != 0x05) {
    printf("Error: Data rejected (%02hX)\n", res);
    SD_DecodeDataRes(res);
    funcRes = 0;
  }

  SPI_SSOn();

  // Wait for completion
  i=0;
  do {
    res = SPI_TrxByte(0xFF);
    i++;
  }while(res != NO_RESPONSE && i < 50000);

  SPI_SSOff();

  // Did it take a crazy amount of time?
  if(res != NO_RESPONSE) {
    printf("  Error: cmd=%02hX, res=%02hX\n", 0xFF, res);
    return 0;
  }

  return funcRes;
}


/**
 * Erases a block of data from the SD card.
 *
 * Erases data starting at addr and ending at 
 * addr+size (inclusive). The SD card will erase
 * all 512byte blocks that the range touches.
 *
 * @param addr the 32bit start address
 * @param size the size of the block to erase (32bit)
 *
 * @return 1 if sucessful, 0 otherwise
 */
uint8_t
SD_EraseBlock(uint32_t addr, uint32_t size)
{
  uint8_t res;

  SD_SendCommand(CMD_ERASE_BLOCK_START, addr);
  res = SD_GetResponse(scratch, 1);
  if(res) {
    printf("  Error: cmd=%02hX, res=%02hX\n", CMD_ERASE_BLOCK_START, res);
    SD_DecodeR1(res);
    return 0;
  }

  SD_SendCommand(CMD_ERASE_BLOCK_END, addr+size);
  res = SD_GetResponse(scratch, 1);
  if(res) {
    printf("  Error: cmd=%02hX, res=%02hX\n", CMD_ERASE_BLOCK_END, res);
    SD_DecodeR1(res);
    return 0;
  }


  SD_SendCommand(CMD_ERASE, 0);
  res = SD_GetResponse(scratch, 1);
  if(res) {
    printf("  Error: cmd=%02hX, res=%02hX\n", CMD_ERASE, res);
    SD_DecodeR1(res);
    return 0;
  }

  // Optionally, the card will send a busy token (response R1b)
  // Wait until a non-zero response is sent back, indicating 
  // that the erase is complete
  SPI_SSOn();
  while(SPI_TrxByte(0xFF) == 0);
  SPI_SSOff();

  return 1;
}


/**
 * Clocks out on the SPI line
 *
 * Sends 0xFF over the SPI line as a way to feed the SD
 * card a clock with no data.
 *
 * @param bytes the number of bytes to clock out (16bit)
 */
void
SD_DelayBytes(uint16_t bytes) {
  while(bytes) {
    SPI_TrxByte(0xFF);
    bytes--;
  }
}


/**
 * Checks for the presence of a card.
 *
 * Issues CMD_GO_IDLE_STATE 10 times
 * or until a response is received.
 *
 * @return 1 if card is found, 0 otherwise
 */
uint8_t
SD_CheckForCard(void) {
  unsigned int i;
  
  // Reset card and check response (retry x 10)
  for(i=0; i<10; i++) {
    SD_SendCommand(CMD_GO_IDLE_STATE, 0);
    if(SD_GetResponse(scratch, 1) != NO_RESPONSE) {
      return 1;
    }
  }

  return 0;
}


/**
 * Sends a command to the card.
 *
 * Sends a command and argument with the proper CRC
 * over the SPI line to the SD card. Use SD_GetResponse()
 * to read the card's response.
 *
 * @see SD_GetResponse
 *
 * @param command the command to send
 * @param arg the 32-bit argument to send with the command
 */
void
SD_SendCommand(uint8_t command, uint32_t arg) {
  uint8_t temp[6]; // Block of data to send
  uint8_t i;

  // command is 0 1 x x x x x x
  command &= 0x3F;
  command |= 0x40;
  temp[0] = command;

  // Copy args... not the most optimal?
  for(i=1; i<5; i++) {
    // AVRs are little endian, data to send is big
    temp[i] = ((uint8_t *)&arg)[4-i];
  }

  // Last byte is CRC, shifted left 1, last bit set
  temp[5] = 0;
  temp[5] = (SD_CRC7(temp, 5) << 1) | 0x01;

  //printf(" Sending command:");
  //for(i=0; i<6; i++) {
  //  printf(" %02X", temp[i]);
  //}
  //printf("\n");

  SPI_SSOn();

  SPI_TrxByte(0xFF);
  SPI_SendBlock(temp, 6);
  SPI_TrxByte(0xFF);

  SPI_SSOff();
}


/**
 * Reads a response from the SD card
 *
 * Reads a response of a given length into the specified buffer.
 * The entire response is loaded in the provided buffer, and
 * the first byte is duplicated as the return value
 *
 * @param buf a pointer to the destination buffer
 * @param length the number of bytes to read as the
 * response (16bit)
 *
 * @return the first byte of the response
 */
uint8_t
SD_GetResponse(uint8_t * buf, uint16_t length) {
  uint8_t res  = NO_RESPONSE;
  uint8_t resx = NO_RESPONSE;
  uint8_t i = 0;

  SPI_SSOn();

  while(i < 20 && res == NO_RESPONSE)   {
    i++;
    res = SPI_TrxByte(0xFF);
  }
  //printf("  Resp: %02hX", res);
  buf[0] = res;

  // Read the rest of the response, up to length
  for(i=1; i<length; i++) {
    resx = SPI_TrxByte(0xFF);
    buf[i] = resx;
    //printf(" %02hX", resx);
  }

  //printf("\n");

  SPI_SSOff();

  return res; 
}


/**
 * Reads a data response of a given length
 *
 * The entire response is loaded in the provided buffer.
 * Assumes it's the start of the response and looks for a 
 * valid START_BLOCK (0xFE). Will wait for 100 bytes
 * for a valid start or error token.
 *
 * @param buf a pointer to the destination buffer
 * @param length the number of bytes to read (16bit)
 *
 * @return 1 if sucessful, 0 otherwise.
 */
uint8_t
SD_ReadData(uint8_t * buf, uint16_t length) {

  uint8_t  res = 0;
  uint16_t  i = 0;
  uint16_t retryCount = 100;

  SPI_SSOn();

  // Find data start, or error token
  do {
    res = SPI_TrxByte(0xFF);
  }while(res == NO_RESPONSE && retryCount--);

  if(res != START_BLOCK) {
    printf("  Error: cmd=%02hX, res=%02hX\n", 0xFF, res);
    SD_DecodeDataErr(res);
    return 0;
  }
  
  // Data is comin our way...
  for(i=0; i<length; i++) {
    buf[i] = SPI_TrxByte(0xFF);
  }

  SPI_SSOff();

  return 1;
}


/**
 * Decodes the R1 response type
 * 
 * Decodes the R1 response and prints out an error message, if applicable.
 *
 * @param res the R1 response
 */
void
SD_DecodeR1(uint8_t res) {

  if(res)
    printf("  Error: "); // Padding for all responses

  if(res & 0x80) {
    printf("Not an R1 resp\n");
    return;
  }

  if(res & 0x01)
    printf("Card idle\n");
  if(res & 0x02)
    printf("Erase rst\n");
  if(res & 0x04)
    printf("Illegal cmd\n");
  if(res & 0x08)
    printf("CRC\n");
  if(res & 0x10)
    printf("Erase seq\n");
  if(res & 0x20)
    printf("Addr\n");
  if(res & 0x40)
    printf("Param\n");
}

/**
 * Decodes the R2 response type
 * 
 * Decodes the R2 response and prints out an error message, if applicable.
 *
 * @param res the R2 response
 */
//void
//SD_DecodeR2(uint16_t res) {
//
//}


/**
 * Decodes the data response
 * 
 * Decodes the data response and prints out an error message, if applicable.
 *
 * @param res the data response
 */
void
SD_DecodeDataRes(uint8_t res) {

  if(res)
    printf("  Error: "); // Padding for all responses

  // Should have x x x 0 s s s 1
  if((res & 0x10) || !(res && 0x01)) {
    printf("Not a data resp\n");
    return;
  }

  switch(res & 0x0F) {
    case 0x05: // 0 1 0 1
      // Data accepted
      break;
    case 0x0B: // 1 0 1 1
      printf("CRC\n");
      break;
    case 0x0D: // 1 1 0 1
      printf("Write\n");
      break;
    default:
      printf("Not a data resp\n");
      break;
  };
}


/**
 * Decodes the data error response
 * 
 * Decodes the data error response and prints out an error message, if applicable.
 *
 * @param res the data error response
 */
void
SD_DecodeDataErr(uint8_t res) {

  if(res)
    printf("  Error: "); // Padding for all responses

  // Should have 0 0 0 0 x x x x
  if(res & 0xF0) {
    printf("Not a data err\n");
    return;
  }

  if(res & 0x01)
    printf("Unk\n");
  if(res & 0x02)
    printf("CC\n");
  if(res & 0x04)
    printf("ECC\n");
  if(res & 0x08)
    printf("Range\n");
}


#define CMD_BYTE_LENGTH 6 
/**
 * A simple CRC7 calculation
 *
 * CRC7 function, stolen from some SHARP docs, which stole it from some Siemens docs...
 *
 * @param bytes a pointer to the source data
 * @param length the length of the source data (32bit)
 *
 * @return the resultant CRC (8bit)
 */
uint8_t
SD_CRC7(uint8_t * bytes, uint32_t length) {
  uint8_t byte, ibit;
  uint8_t reg = 0;

  for (byte = 0; byte < length; byte++) {
    for (ibit=0; ibit<8; ibit++) {
      reg <<= 1;
      reg ^= ((((bytes[byte] << ibit) ^ reg) & 0x80) ? 0x9 : 0);
    }
  }
  return reg;
}


#define CRC16_POLYN     0x1021
/**
 * A simple CRC16 calculation (Xmodem)
 *
 * @param bytes a pointer to the source data
 * @param length the length of the source data (32bit)
 *
 * @return the resultant CRC (16bit)
 */
uint16_t
SD_CRC16(uint8_t * bytes, uint32_t length) {
  uint32_t ibyte;
  uint8_t  ibit;
  uint16_t reg = 0;


  for (ibyte = 0; ibyte < length; ibyte++) {

    reg ^= ((uint16_t)bytes[ibyte]) << 8;
    for (ibit=0; ibit<8; ibit++) {
      if(reg & 0x8000) {
        reg = (reg << 1) ^ CRC16_POLYN;
      }else{
        reg <<= 1;
      }
    }

  }
  return (uint16_t)reg;
}

/**
 * Continues a CRC16 (Xmodem) check.
 *
 * Allows you to calculate the crc for a constant
 * stream of bytes.
 *
 * @param crc the starting CRC
 * @param const_value the constant byte to use
 * @param length the number of bytes to run through the CRC
 *
 * @return the resultant CRC (16bit)
 */
uint16_t
SD_CRC16_Fill(uint16_t crc, uint8_t const_value, uint32_t length) {
  uint32_t ibyte;
  uint8_t  ibit;
  uint16_t reg = crc;
  uint16_t constVal = ((uint16_t)const_value) << 8;

  for (ibyte = 0; ibyte < length; ibyte++) {

    reg ^= constVal;
    for (ibit=0; ibit<8; ibit++) {
      if(reg & 0x8000) {
        reg = (reg << 1) ^ CRC16_POLYN;
      }else{
        reg <<= 1;
      }
    }

  }
  return (uint16_t)reg;
}


