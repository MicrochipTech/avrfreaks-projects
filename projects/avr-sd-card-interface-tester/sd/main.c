/*************************************************************//**
* \file main.c
*
* SD Card Tester
*
* This application provides a command-line-like interface
* to run simple commands against an SD card attached to
* the SPI line.
*
* Currently, this software has only been tested on the ATmega16.
*
* \author Stefan Filipek
* \date March 2009
*
******************************************************************/


#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include <stdio.h>
#include <inttypes.h>
#include <stdlib.h>
#include <string.h>

#include "spi.h"
#include "sci.h"
#include "terminal.h"
#include "sd.h"
#include "utils.h"

#define Interrupts_Disable() cli()
#define Interrupts_Enable() sei()

static uint8_t get(char*);
static uint8_t read(char*);
static uint8_t write(char*);
static uint8_t erase(char*);
static uint8_t scan(char*);
static uint8_t sdinit(char*);
static uint8_t help(char*);

// Command list
static CommandList cmdList = {
  {"get", get},
  {"read", read},
  {"write", write},
  {"erase", erase},
  {"scan", scan},
  {"sdinit", sdinit},
  {"help", help}
};
static const uint16_t cmdLength = sizeof(cmdList)/sizeof(CommandDef);

// Terminal display
static const char * const welcomeMessage = "\nSD Card Tester v0.2.0";
static const char * const promptString = "> ";


/**
 * Main
 */
int
main(void) {

  SCI_Init();
  SPI_Init();
  TERM_Init(cmdList, cmdLength, welcomeMessage, promptString);
  
  Interrupts_Enable();
  
  /* Read/interpret commands from the terminal (never returns) */
  TERM_Run();

  /* NOTREACHED */
  return 0;
}


/**
 * Read data off the SPI line.
 *
 * @param args a space separated string containing the number
 * of bytes to read off, limited to 256.
 */
uint8_t
get(char * args) {
  uint8_t readByte;
  uint8_t readLength;

  readLength = (uint8_t)strtoul(args, (char**)NULL, 0);

  printf("Getting %u bytes\n", readLength);

  SPI_SSOn();
  while(readLength) {
    readByte = SPI_TrxByte(0xFF);
    printf("%02hX ", readByte);
    readLength--;
  }
  SPI_SSOff();

  printf("\n");

  return 1;
}


/**
 * Scans through the SD card looking for non-blank sections.
 *
 * Scans across the SD card in 32-byte blocks and prints out
 * ones containing at least one non FF value. Formatted using
 * print_block() in utils.c.
 *
 * Contiguous blocks are printed as such. Any breaks are
 * signaled with an extra newline between blocks.
 *
 * @see print_block
 *
 * @param args a space seperated string containing
 * an address followed by a length.
 *
 * @return 1, always
 */
uint8_t
scan(char * args) {
  char * token;
  char * currentArg;
  uint32_t i;
  uint8_t  j;
  uint8_t  breakFound       = 0;
  uint8_t  breakPrintNeeded = 0;

  uint8_t buff[32];
  
  uint32_t addr = 0;
  uint32_t size = 0;

  // Convert arguments to longs
  currentArg = strtok_r(args, " ", &token);
  addr       = strtoul(currentArg, (char**)NULL, 0);

  currentArg = strtok_r(NULL, " ", &token);
  size       = strtoul(currentArg, (char**)NULL, 0);


  // Read from SD in 32-byte block sizes and print
  for(i=0; i<size; i+=32, addr+=32) {

    if(!SD_ReadBlock(buff, addr, 32)) {
      printf("Error reading addr 0x%08lX\n", addr);
      break; // On error
    }

    // Only print the block if there's non-FF data
    breakFound = 1;
    for(j=0; j<32; j++) {
      if(buff[j] != 0xFF) {
        breakFound = 0;
        breakPrintNeeded = 1;
        print_block(buff, addr, 32, 16);
        break;
      }
    }

    // Print a separating line between blocks
    if(breakFound && breakPrintNeeded) {
      breakPrintNeeded = 0;
      printf("\n");
    }

  }

  return 1; 
}


/**
 * Read a block of data from the SD card.
 *
 * Reads a block of data and prints it using the print_block
 * function in utils.c
 *
 * @see print_block
 *
 * @param args a space seperated string containing
 * an address (32bit) followed by a length (16bit).
 *
 * @return 1, always
 */
uint8_t
read(char * args) {
  char * token;
  char * currentArg;
  uint32_t i;

  uint8_t buff[32];
  
  uint32_t addr = 0;
  uint16_t size = 0;

  // Convert args to longs
  currentArg = strtok_r(args, " ", &token);
  addr       = strtoul(currentArg, (char**)NULL, 0);

  currentArg = strtok_r(NULL, " ", &token);
  size       = (uint16_t)strtoul(currentArg, (char**)NULL, 0);

  printf("addr: %08lX, size: %08X\n", addr, size);

  for(i=0; i<size; i+=32) {
    if(!SD_ReadBlock(buff, addr, 32))
      break;

    print_block(buff, addr, 32, 16);
    addr += 32;
  }

  return 1; 
}


/**
 * Write some text to the SD card.
 *
 * Writes a block of text to the address specified. Right now
 * the address must be block aligned, or an error will occur.
 *
 * @param args a space seperated string containing
 * an address (32bit) followed by the string to write.
 *
 * @return 1 if successful, 0 otherwise
 */
uint8_t
write(char * args) {
  char * message;
  
  uint32_t addr = 0;
  uint32_t size = 0;
  char *   argEnd = strlen(args) + args;

  // Special tokenization
  message = args;
  while(*message != '\0' && *message != ' ') message++;
  *message = '\0';
  message++;

  if(message > argEnd) message = argEnd;

  addr  = strtoul(args, (char**)NULL, 0);
  size  = strlen(message);

  printf("addr: %08lX, size: %08lX\n", addr, size);

  // Write
  return SD_WriteBlock((uint8_t*)message, addr, size);
}



/**
 * Erase a block of data on the SD card.
 *
 * @param args a space seperated string containing
 * the start address followed by the length (both 32bit).
 *
 * @return 1 if successful, 0 otherwise
 */
uint8_t
erase(char * args) {
  char * token;
  char * currentArg;
  
  uint32_t addr = 0;
  uint32_t size = 0;

  char buff[16];

  // Convert args to longs
  currentArg = strtok_r(args, " ", &token);
  addr       = strtoul(currentArg, (char**)NULL, 0);

  currentArg = strtok_r(NULL, " ", &token);
  size       = strtoul(currentArg, (char**)NULL, 0);

  printf("Erasing addr: %08lX, size: %08lX\n", addr, size);
  printf("Type \"yes\" to confirm ");
  TERM_GetLine(buff, 16);

  if(strcmp(buff, "yes") != 0) {
    printf("Canceled.\n");
  }

  SD_EraseBlock(addr, size);

  return 1; 
}


/**
 * Initialize the SD card.
 *
 * @param args is not used
 * @return 1, always
 */
uint8_t
sdinit(char * args) {
  printf("Initializing SD Card...\n");
  SD_Init();
  return 1; 
}


/**
 * Prints out a list of supported commands.
 *
 * @param args (not used)
 *
 * @return 1, always
 */
uint8_t
help(char * args) {
  uint16_t i;

  // List out all available commands
  printf("Available commands:\n");
  for(i=0; i<cmdLength; i++) {
    printf("  %s\n", cmdList[i].commandName); 
  }

  return 1;
}


EMPTY_INTERRUPT(__vector_default)
