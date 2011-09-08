/*********************************************************************************
 Copyright (C) 2011 by Stefan Filipek

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
*********************************************************************************/

#include <inttypes.h>
#include <avr/sfr_defs.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#include <stdio.h>

#include <savr/cpp_pgmspace.h>
#include <savr/sd.h>
#include <savr/spi.h>
#include <savr/utils.h>


#ifndef SD_NO_CRC
#define SD_USE_CRC
#endif


// Not all defines are used... yet...
#define CMD_GO_IDLE_STATE           0     // Reset the SD
#define CMD_SEND_OP_COND            1     // Activate initialization process
#define CMD_SEND_IF_COND            8     // Send interface operating condition
#define CMD_SEND_CSD                9     // Send Card specific data
#define CMD_SEND_CID                10    // Send Card ID
#define CMD_STOP_TRANSMISSION       12    // Stop read/write transmission
#define CMD_SEND_STATUS             13    // Request card send status
#define CMD_SET_BLOCKLEN            16    // Set block length (in bytes)
#define CMD_READ_BLOCK              17    // Read a block of data
#define CMD_WRITE_BLOCK             24    // Write a block of data
#define CMD_ERASE_BLOCK_START       32    // Set first sector for erase
#define CMD_ERASE_BLOCK_END         33    // Set last sector for erase
#define CMD_ERASE                   38    // Erase all selected sectors
#define CMD_READ_OCR                58    // Read OCR register
#define CMD_CRC_ONOFF               59    // 0 to turn off CRC, 1 to turn on CRC

#define APP_CMD                     55    // Signal start of application specific command
#define SD_SEND_OP_COND             41    // Send operation conditions on host side

#define R1_IDLE                     0x01
#define R1_ERASE_RESET              0x02
#define R1_ILLEGAL_CMD              0x04
#define R1_CRC_ERROR                0x08
#define R1_ERASE_ERROR              0x10
#define R1_ADDR_ERROR               0x20
#define R1_PARAM_ERROR              0x40

#define NO_RESPONSE                 0xFF

#define BLOCK_SIZE                  512
#define FILL_BYTE                   0xFF
#define START_BLOCK                 0xFE

static void DelayBytes(uint16_t bytes);
static void SendCommand(uint8_t command, uint32_t arg);
static uint8_t CheckForCard(void);
static uint8_t GetResponse(uint8_t *buf, uint16_t length);
static uint8_t ReadData(uint8_t *buf, uint16_t length);

static void DecodeR1(uint8_t res);
//static void DecodeR2(uint16_t res);
static void DecodeDataRes(uint8_t res);
static void DecodeDataErr(uint8_t res);


#ifdef SD_USE_CRC
static uint8_t CRC7 (uint8_t *bytes, uint32_t length);
static uint16_t CRC16(uint8_t *bytes, uint32_t length);
static uint16_t CRC16_Fill(uint16_t crc, uint8_t const_value, uint32_t length);
#else
#define CRC7(x, y)           ((uint8_t)0x4A)
#define CRC16(x, y)          0xFFFF
#define CRC16_Fill(x, y, z)  0xFFFF
#endif


// Error printing used all over the place
static void Error(uint8_t cmd, uint8_t res);


// SD::* is already non-reentrant, so a global buffer is... OK...
static uint8_t scratch[32];
static GPIO::Pin _ss;



/**
 * @par Implementation Notes:
 */
uint8_t
SD::Init(GPIO::Pin ss)
{
    uint16_t i;
    uint8_t res;

    _ss = ss;

    // Delay a buncha clocks
    GPIO::Out(_ss);
    GPIO::High(_ss);
    DelayBytes(20);

    // Check if the card is inserted
    if(!CheckForCard())
    {
        printf_P(PSTR("Card NOT found\n"));
        return 0;
    }
    printf_P(PSTR("Card found\n"));

    // Standard init commands
    SendCommand(CMD_SEND_IF_COND, 0);
    res = GetResponse(scratch, 5);

    SendCommand(CMD_READ_OCR, 0);
    res = GetResponse(scratch, 5);

    // Wait for card to init fully
    i=0;
    do {
        SendCommand(CMD_SEND_OP_COND, 0);
        res = GetResponse(scratch, 1);
        i++;
    }while(res && i < 10000);

    if(res) {
        Error(CMD_SEND_OP_COND, res);
        return 0;
    }

#ifdef SD_USE_CRC
    SendCommand(CMD_CRC_ONOFF, 1);
#else
    SendCommand(CMD_CRC_ONOFF, 0);
#endif
    res = GetResponse(scratch, 1);
    if(res) {
        Error(CMD_SEND_CID, res);
        DecodeR1(res);
        return 0;
    }


    SendCommand(CMD_SEND_CID, 0);
    // 128bits + 16 CRC
    res = GetResponse(scratch, 1) || !ReadData(scratch, 18);

    if(res) {
        Error(CMD_SEND_CID, res);
        return 0;
    }else{
        printf_P(PSTR("CID: "));
        Utils::PrintHex(scratch, 16);
        putchar('\n');
        //printf(" CRC16: %d\n", CRC16(scratch, 18));
    }


    SendCommand(CMD_SEND_CSD, 0);
    // 128bits + 16 CRC
    res = GetResponse(scratch, 1) || !ReadData(scratch, 18);

    if(res) {
        Error(CMD_SEND_CID, res);
        return 0;
    }else{
        printf_P(PSTR("CSD: "));
        Utils::PrintHex(scratch, 16);
        putchar('\n');
        //printf(" CRC16: %d\n", CRC16(scratch, 18));
    }


    return 1;
}


/**
 * @par Implementation Notes:
 */
uint8_t
SD::ReadBlock(uint8_t *buf, uint32_t addr, size_t size)
{
    uint8_t res;

    // Set the block length... won't work on SDHC
    SendCommand(CMD_SET_BLOCKLEN, size);
    res = GetResponse(scratch, 1);
    if(res) {
        Error(CMD_SET_BLOCKLEN, res);
        DecodeR1(res);
        return 0;
    }

    SendCommand(CMD_READ_BLOCK, addr);
    res = GetResponse(scratch, 1);
    if(res) {
        Error(CMD_READ_BLOCK, res);
        DecodeR1(res);
        return 0;
    }

    return ReadData(buf, size);
}


/**
 * @par Implementation Notes:
 */
uint8_t
SD::WriteBlock(uint8_t *data, uint32_t addr, size_t size)
{
    uint8_t res;
    uint8_t funcRes = 1;
    uint16_t i;
    uint16_t crc;

    crc = CRC16(data, (uint32_t)size);

    // Set the block length... won't work on SDHC
    SendCommand(CMD_SET_BLOCKLEN, BLOCK_SIZE);
    res = GetResponse(scratch, 1);
    if(res) {
        Error(CMD_SET_BLOCKLEN, res);
        DecodeR1(res);
        return 0;
    }

    // Tell it we want to write
    SendCommand(CMD_WRITE_BLOCK, addr);
    res = GetResponse(scratch, 1);
    if(res) {
        Error(CMD_WRITE_BLOCK, res);
        DecodeR1(res);
        return 0;
    }

    GPIO::Low(_ss);

    // Send "Start Block" byte
    SPI::TrxByte(START_BLOCK);

    // Send data
    SPI::SendBlock(data, size);

    // Fill empty space
    for(i=size; i<BLOCK_SIZE; i++) {
        SPI::TrxByte(FILL_BYTE);
    }

    // Send CRC (recalc for fill bytes
    crc = CRC16_Fill(crc, FILL_BYTE, BLOCK_SIZE-size);
    SPI::TrxByte((uint8_t)(crc>>8));
    SPI::TrxByte((uint8_t)crc);

    GPIO::High(_ss);

    // Response?
    res = GetResponse(scratch, 1);
    if((res & 0x0F) != 0x05) {
        printf_P(PSTR("Error: Data rejected (%02hX)\n"), res);
        DecodeDataRes(res);
        funcRes = 0;
    }

    GPIO::Low(_ss);

    // Wait for completion
    i=0;
    do {
        res = SPI::TrxByte(0xFF);
        i++;
    }while(res != NO_RESPONSE && i < 50000);

    GPIO::High(_ss);

    // Did it take a crazy amount of time?
    if(res != NO_RESPONSE) {
        Error(0xFF, res);
        return 0;
    }

    return funcRes;
}


/**
 * @par Implementation Notes:
 */
uint8_t
SD::EraseBlock(uint32_t addr, uint32_t size)
{
    uint8_t res;

    SendCommand(CMD_ERASE_BLOCK_START, addr);
    res = GetResponse(scratch, 1);
    if(res) {
        Error(CMD_ERASE_BLOCK_START, res);
        DecodeR1(res);
        return 0;
    }

    SendCommand(CMD_ERASE_BLOCK_END, addr+size);
    res = GetResponse(scratch, 1);
    if(res) {
        Error(CMD_ERASE_BLOCK_END, res);
        DecodeR1(res);
        return 0;
    }


    SendCommand(CMD_ERASE, 0);
    res = GetResponse(scratch, 1);
    if(res) {
        Error(CMD_ERASE, res);
        DecodeR1(res);
        return 0;
    }

    // Optionally, the card will send a busy token (response R1b)
    // Wait until a non-zero response is sent back, indicating 
    // that the erase is complete
    GPIO::Low(_ss);
    while(SPI::TrxByte(0xFF) == 0) ;
    GPIO::High(_ss);

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
DelayBytes(uint16_t bytes)
{
    while (bytes) {
        SPI::TrxByte(0xFF);
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
CheckForCard(void)
{
    unsigned int i;
    
    // Reset card and check response (retry x 10)
    for(i=0; i<10; i++) {
        SendCommand(CMD_GO_IDLE_STATE, 0);
        if(GetResponse(scratch, 1) != NO_RESPONSE) {
            return 1;
        }
    }

    return 0;
}


/**
 * Sends a command to the card.
 *
 * Sends a command and argument with the proper CRC
 * over the SPI line to the SD card. Use GetResponse()
 * to read the card's response.
 *
 * @see GetResponse
 *
 * @param command the command to send
 * @param arg the 32-bit argument to send with the command
 */
void
SendCommand(uint8_t command, uint32_t arg)
{
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
    temp[5] = (CRC7(temp, 5) << 1) | 0x01;

    //printf(" Sending command:");
    //for(i=0; i<6; i++) {
    //    printf(" %02X", temp[i]);
    //}
    //printf("\n");

    GPIO::Low(_ss);

    SPI::TrxByte(0xFF);
    SPI::SendBlock(temp, 6);
    SPI::TrxByte(0xFF);

    GPIO::High(_ss);
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
GetResponse(uint8_t * buf, uint16_t length)
{
    uint8_t res  = NO_RESPONSE;
    uint8_t resx = NO_RESPONSE;
    uint8_t i = 0;

    GPIO::Low(_ss);

    while(i < 20 && res == NO_RESPONSE) {
        i++;
        res = SPI::TrxByte(0xFF);
    }
    //printf("    Resp: %02hX", res);
    buf[0] = res;

    // Read the rest of the response, up to length
    for(i=1; i<length; i++) {
        resx = SPI::TrxByte(0xFF);
        buf[i] = resx;
        //printf(" %02hX", resx);
    }

    //printf("\n");

    GPIO::High(_ss);

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
ReadData(uint8_t * buf, uint16_t length)
{
    uint8_t  res = 0;
    uint16_t i = 0;
    uint16_t retryCount = 100;

    GPIO::Low(_ss);

    // Find data start, or error token
    do {
        res = SPI::TrxByte(0xFF);
    }while(res == NO_RESPONSE && retryCount--);

    if(res != START_BLOCK) {
        Error(0xFF, res);
        DecodeDataErr(res);
        return 0;
    }
    
    // Data is comin our way...
    for(i=0; i<length; i++) {
        buf[i] = SPI::TrxByte(0xFF);
    }

    GPIO::High(_ss);

    return 1;
}


/**
 * Prints an error string
 *
 * Prints an error string containing the command and response
 * in hex.
 *
 * @param cmd the command attempted
 * @param res the response byte
 */
void
Error(uint8_t cmd, uint8_t res)
{
    printf_P(PSTR("  Error: cmd=%02hX, res=%02hX\n"), cmd, res);
}


/**
 * Decodes the R1 response and prints out an error message, if applicable.
 *
 * @param res the R1 response
 */
void
DecodeR1(uint8_t res)
{
    if(res)
        printf_P(PSTR("  Error: ")); // Padding for all responses

    if(res & 0x80) {
        printf_P(PSTR("Not an R1 resp\n"));
        return;
    }

    if(res & 0x01)
        printf_P(PSTR("Card idle\n"));
    if(res & 0x02)
        printf_P(PSTR("Erase rst\n"));
    if(res & 0x04)
        printf_P(PSTR("Illegal cmd\n"));
    if(res & 0x08)
        printf_P(PSTR("CRC\n"));
    if(res & 0x10)
        printf_P(PSTR("Erase seq\n"));
    if(res & 0x20)
        printf_P(PSTR("Addr\n"));
    if(res & 0x40)
        printf_P(PSTR("Param\n"));
}

/**
 * Decodes the R2 response type
 * 
 * Decodes the R2 response and prints out an error message, if applicable.
 *
 * @param res the R2 response
 */
//void
//DecodeR2(uint16_t res) {
//
//}


/**
 * Decodes the data response and prints out an error message, if applicable.
 *
 * @param res the data response
 */
void
DecodeDataRes(uint8_t res)
{

    if(res)
        printf_P(PSTR("  Error: ")); // Padding for all responses

    // Should have x x x 0 s s s 1
    if((res & 0x10) || !(res && 0x01)) {
        printf_P(PSTR("Not a data resp\n"));
        return;
    }

    switch(res & 0x0F) {
        case 0x05: // 0 1 0 1
            // Data accepted
            break;
        case 0x0B: // 1 0 1 1
            printf_P(PSTR("CRC\n"));
            break;
        case 0x0D: // 1 1 0 1
            printf_P(PSTR("Write\n"));
            break;
        default:
            printf_P(PSTR("Not a data resp\n"));
            break;
    };
}


/**
 * Decodes the data error response and prints out an error message, if applicable.
 *
 * @param res the data error response
 */
void
DecodeDataErr(uint8_t res)
{

    if(res)
        printf_P(PSTR("  Error: ")); // Padding for all responses

    // Should have 0 0 0 0 x x x x
    if(res & 0xF0) {
        printf_P(PSTR("Not a data err\n"));
        return;
    }

    if(res & 0x01)
        printf_P(PSTR("Unk\n"));
    if(res & 0x02)
        printf_P(PSTR("CC\n"));
    if(res & 0x04)
        printf_P(PSTR("ECC\n"));
    if(res & 0x08)
        printf_P(PSTR("Range\n"));
}

#ifdef SD_USE_CRC

#define CRC7_POLYN      0x09                ///< 7-bit CRC polynomial (x^7 is implied in the algo)
#define CRC7_B_POLYN    (CRC7_POLYN << 1)   ///< MSB byte aligned polynomial for our algorithm

/**
 * A simple CRC7 calculation
 *
 * @param bytes a pointer to the source data
 * @param length the length of the source data (32bit)
 *
 * @return the resultant CRC (8bit)
 */
uint8_t
CRC7(uint8_t * bytes, uint32_t length)
{
    uint32_t ibyte;
    uint8_t  ibit;
    uint8_t  reg = 0;


    for (ibyte = 0; ibyte < length; ibyte++) {

        reg ^= bytes[ibyte];
        for (ibit=0; ibit<8; ibit++) {
            if(reg & 0x80) {
                reg = (reg << 1) ^ CRC7_B_POLYN;
            }else{
                reg <<= 1;
            }
        }

    }
    return reg>>1;
}


#define CRC16_POLYN         0x1021          ///< 16-bit CRC polynomial (x^16 is implied in the algo)
/**
 * A simple CRC16 calculation (Xmodem)
 *
 * @param bytes a pointer to the source data
 * @param length the length of the source data (32bit)
 *
 * @return the resultant CRC (16bit)
 */
uint16_t
CRC16(uint8_t * bytes, uint32_t length)
{
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
CRC16_Fill(uint16_t crc, uint8_t const_value, uint32_t length)
{
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

#endif
