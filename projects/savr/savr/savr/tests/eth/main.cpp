/*************************************************************//**
 * @file main.c
 *
 * @author Stefan Filipek
 ******************************************************************/
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include <stdio.h>
#include <inttypes.h>
#include <stdlib.h>
#include <string.h>

#include <savr/cpp_pgmspace.h>
#include <savr/spi.h>
#include <savr/sci.h>
#include <savr/terminal.h>
#include <savr/sd.h>
#include <savr/utils.h>


#define Interrupts_Disable() cli()
#define Interrupts_Enable() sei()

static uint8_t get(char*);
static uint8_t put(char*);
static uint8_t sson(char*);
static uint8_t ssoff(char*);
static uint8_t ethread(char*);
static uint8_t ethwrite(char*);

// Command list
static CMD::CommandList cmdList = {
    {"get",       get,        "Get a number of bytes (puts 0xFF): get [numBytes]"},
    {"put",       put,        "Send a byte: put [byte]"},
    {"sson",      sson,       "Slave Select asserted"},
    {"ssoff",     ssoff,      "Slave Select deasserted"},
    {"ethread",   ethread,    "Read from the given W5100 address: ethread [addr]"},
    {"ethwrite",  ethwrite,   "Write to the given W5100 address: ethread [addr] [byte]"},
};
static const uint16_t cmdLength = sizeof(cmdList)/sizeof(CMD::CommandDef);


// Terminal display
static const char CPP_PROGMEM welcomeMessage[]  = "\nSPI Tester v0.1\n";
static const char CPP_PROGMEM promptString[]    = "] ";


/**
 * Main
 */
int
main(void) {

    SCI::Init(F_CPU, 57600);
    SPI::Init(F_CPU, F_CPU/2);

    Term::Init(welcomeMessage, promptString);

    Interrupts_Enable();

    Term::Run(cmdList, cmdLength);

    /* NOTREACHED */
    return 0;
}

uint8_t
sson(char * args) {
    SPI::SSOn();
    return 0;
}

uint8_t
ssoff(char * args) {
    SPI::SSOff();
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

    printf_P(PSTR("Getting %u bytes\n"), readLength);

    while(readLength) {
        readByte = SPI::TrxByte(0xFF);
        printf_P(PSTR("%02hX "), readByte);
        readLength--;
    }

    printf("\n");

    return 0;
}

uint8_t
put(char * args) {
    uint8_t byte;
    uint8_t readByte;
    byte = (uint8_t)strtoul(args, (char**)NULL, 0);

    printf_P(PSTR("Putting 0x%02X\n"), byte);

    readByte = SPI::TrxByte(byte);
    printf_P(PSTR("Got %02hX\n"), readByte);

    return 0;
}


#define READ_OP     0x0F
#define WRITE_OP    0xF0

uint8_t
ethread(char * args) {
    uint8_t     readByte;
    uint16_t    readAddr;

    readAddr = (uint16_t)strtoul(args, (char**)NULL, 0);

    printf_P(PSTR("Getting byte from addr 0x%04X: "), readAddr);

    SPI::SSOn();

    if(SPI::TrxByte(READ_OP) != 0) goto ethread_fail;
    if(SPI::TrxByte((readAddr>>8) & 0xFF) != 1) goto ethread_fail;
    if(SPI::TrxByte((readAddr)    & 0xFF) != 2) goto ethread_fail;
    readByte = SPI::TrxByte(0xFF);

    SPI::SSOff();

    printf_P(PSTR("%02hX\n"), readByte);

    return 0;

ethread_fail:
    SPI::SSOff();
    printf_P(PSTR("Failed\n"));
    return 1;
}

uint8_t
ethwrite(char * args) {
    char * token;
    char * currentArg;

    uint8_t     writeByte;
    uint16_t    writeAddr;

    currentArg = strtok_r(args, " ", &token);
    writeAddr  = (uint16_t)strtoul(currentArg, (char**)NULL, 0);

    currentArg = strtok_r(NULL, " ", &token);
    writeByte  = (uint8_t)strtoul(currentArg, (char**)NULL, 0);

    printf_P(PSTR("Writing 0x%02X from addr 0x%04X: "), writeByte, writeAddr);

    SPI::SSOn();

    if(SPI::TrxByte(WRITE_OP) != 0) goto ethwrite_fail;
    if(SPI::TrxByte((writeAddr>>8) & 0xFF) != 1) goto ethwrite_fail;
    if(SPI::TrxByte((writeAddr)    & 0xFF) != 2) goto ethwrite_fail;
    if(SPI::TrxByte(writeByte) != 3) goto ethwrite_fail;

    SPI::SSOff();

    printf_P(PSTR("Done\n"));

    return 0;

ethwrite_fail:
    SPI::SSOff();
    printf_P(PSTR("Failed\n"));
    return 1;
}



EMPTY_INTERRUPT(__vector_default)
