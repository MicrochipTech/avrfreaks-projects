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
#include <savr/sci.h>
#include <savr/terminal.h>
#include <savr/utils.h>
#include <savr/twi.h>

#define Interrupts_Disable() cli()
#define Interrupts_Enable() sei()

uint8_t ScanTWI(char *args) {
    uint8_t i=0;
    uint8_t res=0;
    uint8_t state=0;

    printf("Devices found:\n");
    for(i=0; i<0x7F; ++i) {
        // Try to read from the address...
        res = TWI::Address(i, 1);
        if(res == 0) {
            printf("  0x%02X\n", i);
        }

        // If we get an ACK, read out a byte or else errors can occur
        // Else, we can do a repeated Start and look at the next address...
        state = TWI::State();
        if(state == TW_MR_SLA_ACK) {
            TWI::Get();
            TWI::Stop();
        }
    }
    TWI::Stop();
    return 0;
}

uint8_t GetTime(char *args) {
    uint8_t res = 0;
    uint8_t i = 0;
    uint8_t temp = 0;
    char *token;
    char *currentArg;
    uint8_t addr;

    uint8_t buff[8];

    currentArg = strtok_r(args, " ", &token);
    addr = strtoul(currentArg, (char**) NULL, 0);

    // Write mode first to set the pointer to 0
    res = TWI::Address(addr, 0);
    if(res != 0) {
        printf_P(PSTR("Failed to address 0x%02X\n"), addr);
        TWI::PrintState();
        return 1;
    }
    TWI::Send(0);
    TWI::Stop();


    res = TWI::Address(addr, 1);
    if(res != 0) {
        printf_P(PSTR("Failed to address 0x%02X\n"), addr);
        TWI::PrintState();
        return 1;
    }

    for(i=0; i<sizeof(buff)-1; ++i) {
        buff[i] = TWI::GetAck();
    }
    buff[7] = TWI::Get(); // Last read has no ACK

    printf("Raw: ");
    Utils::PrintHex(buff, 8);
    printf("\n");

    printf_P(PSTR(" Year    :   xx%d%d\n"), buff[6]>>4, buff[6]&0xF);
    printf_P(PSTR(" Month   :   %d%d\n"),   buff[5]>>4, buff[5]&0xF);
    printf_P(PSTR(" Day (M) :   %d%d\n"),   buff[4]>>4, buff[4]&0xF);
    printf_P(PSTR(" Day (W) :   %d\n"),     buff[3]);
    if((buff[2] & 0x40) == 0) {
        // Military time
        printf_P(PSTR(" Hours   :   %d%d (Mil)\n"), buff[2]>>4, buff[2]&0xF);
    }else{
        buff[2] &= ~0x40;
        temp = (buff[2] & 0x20) == 0x20; // 1 for PM
        printf_P(PSTR(" Hours   :   %d%d (%cM)\n"), buff[2]>>4, buff[2]&0xF, (temp ? 'P' : 'A'));
    }
    printf_P(PSTR(" Minutes :   %d%d\n"),   buff[1]>>4, buff[1]&0xF);
    buff[0] &= 0x7F; // Mask out CH bit
    printf_P(PSTR(" Seconds :   %d%d\n"),   buff[0]>>4, buff[0]&0xF);

    TWI::Stop();
    return 0;
}

uint8_t wrap_TWI_PrintState(char *args) {
    TWI::PrintState();
    return 0;
}

uint8_t wrap_TWI_Address(char *args) {
    uint8_t res = 0;
    char *token;
    char *currentArg;
    uint8_t b, rw;

    currentArg = strtok_r(args, " ", &token);
    b = strtoul(currentArg, (char**) NULL, 0);

    currentArg = strtok_r(NULL, " ", &token);
    rw = strtoul(currentArg, (char**) NULL, 0);
    printf_P(PSTR("Addressing 0x%02X, %S\n"), b, (rw?PSTR("Read"):PSTR("Write")));

    res = TWI::Address(b, rw);
    printf_P(PSTR("Res: 0x%02X\n"), res);
    TWI::PrintState();
    return 0;
}

uint8_t wrap_TWI_GetAck(char *args) {
    printf_P(PSTR("Res: 0x%02X\n"), TWI::GetAck());
    TWI::PrintState();
    return 0;
}

uint8_t wrap_TWI_Get(char *args) {
    printf_P(PSTR("Res: 0x%02X\n"), TWI::Get());
    TWI::PrintState();
    return 0;
}

uint8_t wrap_TWI_Send(char *args) {
    char *token;
    char *currentArg;

    uint8_t b;
    currentArg = strtok_r(args, " ", &token);
    b = strtoul(currentArg, (char**) NULL, 0);

    printf_P(PSTR("Sending: 0x%02X\n"), b);
    TWI::Send(b);
    TWI::PrintState();
    return 0;
}

uint8_t wrap_TWI_Stop(char *args) {
    TWI::Stop();
    TWI::PrintState();
    return 0;
}

uint8_t wrap_TWI_Start(char *args) {
    TWI::Start();
    TWI::Wait();
    TWI::PrintState();
    return 0;
}

uint8_t wrap_TWI_State(char *args) {
    printf_P(PSTR("Res: 0x%02X\n"), TWI::State());
    TWI::PrintState();
    return 0;
}



/**
 * Terminal command callbacks
 */

// Command list
static CMD::CommandList cmdList = {
    {"Time",            GetTime,                "Gets the time: Time [addr]"},
    {"Scan",            ScanTWI,                "Scans the bus and prints any addresses found"},
    {"PrintState",      wrap_TWI_PrintState,    "Prints current bus state"},
    {"Addr",            wrap_TWI_Address,       "Starts bus and address a device: Addr [addr] [1=read, 0=write]"},
    {"GetAck",          wrap_TWI_GetAck,        "Read a byte with ack"},
    {"Get",             wrap_TWI_Get,           "Read a byte without ack"},
    {"Send",            wrap_TWI_Send,          "Send a byte: Send [byte]"},
    {"Start",           wrap_TWI_Start,         "Bus Start (rarely needed)"},
    {"Stop",            wrap_TWI_Stop,          "Bus Stop"},
    {"State",           wrap_TWI_State,         "Get bus status byte"},
  

};
static const size_t cmdLength = sizeof(cmdList) / sizeof(CMD::CommandDef);


// Terminal display
#define welcomeMessage PSTR("\nTWI Clock Test\n")
#define promptString   PSTR("] ")


/**
 * Main
 */
int main(void) {

    SCI::Init(F_CPU, 38400);  // bps
    TWI::Init(F_CPU, 500000); // Bus freq in Hz

    Term::Init(welcomeMessage, promptString);

    Interrupts_Enable();

    Term::Run(cmdList, cmdLength);

    /* NOTREACHED */
    return 0;
}


EMPTY_INTERRUPT(__vector_default)
